#include "extdll.h"
#include "scripting/api/ASReflection.h"
#include "scripting/core/CASASReflection.h"
#include "scripting/core/CASArguments.h"
#include "scripting/core/CASReturnValue.h"
#include "scripting/core/CASFunction.h"
#include "scripting/core/CASMethod.h"
#include "scripting/core/CASObjectType.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASReflection.h"
#include "scripting/core/CASLog.h"
#include "CString.h"
#include "server/server_support.h"
#include "../../../external/angelscript/sdk/add_on/scriptany/scriptany.h"
#include <string.h>

namespace
{
CASArguments *DefaultConstructArguments()
{
	return new CASArguments();
}

CASArguments *CopyConstructArguments(const CASArguments &other)
{
	return new CASArguments(other);
}

void ArgumentsConstructArguments(asIScriptGeneric *generic)
{
	generic->SetReturnAddress(new CASArguments(generic, 0));
}

void CASArguments_SetArguments(asIScriptGeneric *generic)
{
	CASArguments *arguments = (CASArguments *)generic->GetObject();

	generic->SetReturnByte(arguments->SetArguments(generic, 0));
}

CString CASBaseCallable_GetName(const CASFunction *callable)
{
	return CString(callable->GetFunction()->GetName());
}

CString CASBaseCallable_GetNamespace(const CASFunction *callable)
{
	return CString(callable->GetFunction()->GetNamespace());
}

CASReturnValue *CASBaseCallable_Call(CASArguments *arguments, CASBaseCallable *callable)
{
	if (!arguments)
	{
		CASServerManager::GetInstance()->SetExceptionOnActiveContext("Reflection::BaseCallable::Call: null arguments handle!", true);
		return NULL;
	}

	CASReturnValue *result = callable->Call(*arguments, true);

	arguments->Release();
	return result;
}

void CASBaseCallable_Call(CASBaseCallable *callable, asIScriptGeneric *generic, unsigned int firstArgument)
{
	CASArguments arguments(generic, firstArgument);
	CASReturnValue *result = callable->Call(arguments, true);

	generic->SetReturnObject(result);

	if (result)
		result->Release();
}

void CASFunction_Call(asIScriptGeneric *generic)
{
	CASBaseCallable_Call((CASBaseCallable *)generic->GetObject(), generic, 0);
}

void *CASMethod_GetThisPointer(asIScriptEngine *engine, CASMethod *method, asIScriptGeneric *generic)
{
	int typeId = generic->GetArgTypeId(0);

	if ((typeId >= asTYPEID_BOOL && typeId <= asTYPEID_DOUBLE) || (typeId > asTYPEID_DOUBLE && !(typeId & asTYPEID_MASK_OBJECT)))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Method::Call: Object type can't be a primitive type or enum!\n");
		return NULL;
	}

	asITypeInfo *type = engine->GetTypeInfoById(typeId);

	if (!type)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CMethod::Call: Could not get object type!\n");
		return NULL;
	}

	asITypeInfo *targetType = method->GetFunction()->GetObjectType();

	if (!targetType)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CMethod::Call: Could not get target object type!\n");
		return NULL;
	}

	bool handle = (typeId & asTYPEID_OBJHANDLE) != 0;
	void *object;

	if (handle)
		object = *(void **)generic->GetArgObject(0);
	else if (type->GetFlags() & asOBJ_REF)
		object = generic->GetArgObject(0);
	else
		object = generic->GetArgAddress(0);

	if (!object)
	{
		CASServerManager::GetInstance()->SetExceptionOnActiveContext("Reflection::Method::Call: null pointer exception!\n", true);
		return NULL;
	}

	void *castObject = NULL;

	if (engine->RefCastObject(handle ? &object : object, type, targetType, &castObject, false) < 0)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Reflection::Method::Call: 'This' object is not compatible with target object type!\n");
		return NULL;
	}

	engine->ReleaseScriptObject(castObject, targetType);
	return object;
}

CASMethod *CASBaseObjectData_GetMethod(const CString &declaration, CASBaseObjectData *object)
{
	CASMethod *method = object->GetMethod(declaration.Data());

	if (method)
		method->AddRef();

	return method;
}

bool CASBaseObjectData_MethodExists(const CString &declaration, CASBaseObjectData *object)
{
	return object->MethodExists(declaration.Data());
}

bool CASObjectType_DerivesFrom(const CString &name, const CASObjectType *type)
{
	return type->DerivesFrom(name.Data());
}

template <class From, class To>
To *Cast_UpCast(From *object)
{
	return object;
}

template <class From, class To>
To *Cast_DownCast(From *object)
{
	return dynamic_cast<To *>(object);
}

template <class Base, class Derived>
void RegisterCasts(CASDocumentation *documentation, const char *baseName, const char *derivedName)
{
	if (!strcmp(derivedName, baseName))
		return;

	char declaration[256];

	if ((unsigned int)UTIL_FormatString(declaration, sizeof(declaration), "%s@ opImplCast()", baseName) >= sizeof(declaration))
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Failed to format string for implicit cast for class '%s'!\n", derivedName);
	else
		documentation->RegisterObjectMethod(
			"Implicitly casts to base type.",
			derivedName,
			documentation->AllocString(declaration),
			asFUNCTION((Cast_UpCast<Derived, Base>)),
			asCALL_CDECL_OBJLAST);

	if ((unsigned int)UTIL_FormatString(declaration, sizeof(declaration), "%s@ opCast()", derivedName) >= sizeof(declaration))
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Failed to format string for explicit cast for class '%s'!\n", derivedName);
	else
		documentation->RegisterObjectMethod(
			"Explicitly casts to sub type.",
			baseName,
			documentation->AllocString(declaration),
			asFUNCTION((Cast_DownCast<Base, Derived>)),
			asCALL_CDECL_OBJLAST);
}

template <class T>
void RegisterCallable(CASDocumentation *documentation, const char *description, const char *name)
{
	documentation->RegisterObjectType(description, name, sizeof(T), asOBJ_REF);
	documentation->RegisterObjectBehaviour("AddRef", name, asBEHAVE_ADDREF, "void AddRef()", asMETHODPR(T, AddRef, () const, void), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour(
		"Release",
		name,
		asBEHAVE_RELEASE,
		"void Release()",
		asMETHODPR(T, Release, () const, void),
		asCALL_THISCALL);
	RegisterCasts<CASBaseCallable, T>(documentation, "Callable", name);
	documentation->RegisterObjectMethod(
		"Gets this instance's namespace.",
		name,
		"string GetNamespace() const",
		asFUNCTION(CASBaseCallable_GetNamespace),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod(
		"Gets this instance's name.",
		name,
		"string GetName() const",
		asFUNCTION(CASBaseCallable_GetName),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod(
		"Returns whether the given callable type is identical to this one.",
		name,
		"bool Equals(const Callable@ pCallable) const",
		asMETHOD(T, Equals),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Returns whether the given object type is compatible with this one.",
		name,
		"bool IsCompatible(const Callable@ pCallable) const",
		asMETHOD(T, IsCompatible),
		asCALL_THISCALL);
}
}

CScriptAny *CASReturnValue_ToAny(const CASReturnValue *returnValue)
{
	const CASArgument &argument = returnValue->m_ReturnValue;

	if (argument.m_ArgumentType & (ARGUMENT_NONE | ARGUMENT_VOID))
		return NULL;

	asINT64 integer;
	double real;

	CASReflection::ConvertInputArgToLargest(&argument, integer, real);

	CScriptAny *result = new CScriptAny(CASServerManager::GetInstance()->m_pEngine);

	if (argument.m_iTypeId >= asTYPEID_INT8 && argument.m_iTypeId <= asTYPEID_UINT64)
		result->Store(integer);
	else if (argument.m_iTypeId >= asTYPEID_FLOAT && argument.m_iTypeId <= asTYPEID_DOUBLE)
		result->Store(real);
	else
		result->Store(argument.GetArgumentAsPointer(), argument.m_iTypeId);

	return result;
}

void CASMethod_Call_Varargs(asIScriptGeneric *generic)
{
	CASMethod *method = (CASMethod *)generic->GetObject();
	void *object = CASMethod_GetThisPointer(CASServerManager::GetInstance()->m_pEngine, method, generic);

	if (!object)
	{
		generic->SetReturnObject(NULL);
		return;
	}

	method->SetThisPointer(object);
	CASBaseCallable_Call(method, generic, 1);
}

void CASMethod_Call(asIScriptGeneric *generic)
{
	CASMethod *method = (CASMethod *)generic->GetObject();
	void *object = CASMethod_GetThisPointer(CASServerManager::GetInstance()->m_pEngine, method, generic);

	if (!object)
	{
		generic->SetReturnObject(NULL);
		return;
	}

	CASArguments *arguments = (CASArguments *)generic->GetArgObject(1);

	if (!arguments)
	{
		CASServerManager::GetInstance()->SetExceptionOnActiveContext("Reflection::Method::Call: null arguments handle!", true);
		return;
	}

	method->SetThisPointer(object);

	CASReturnValue *result = method->Call(*arguments, true);

	generic->SetReturnObject(result);

	if (result)
		result->Release();
}

void RegisterSCScriptReflectionArguments(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("This class can hold arguments for a function or method call.", "Arguments", 0, asOBJ_REF);
	documentation->RegisterObjectBehaviour(
		"Default constructor",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments()",
		asFUNCTION(DefaultConstructArguments),
		asCALL_CDECL);
	documentation->RegisterObjectBehaviour(
		"Copy constructor",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(const Arguments& in other)",
		asFUNCTION(CopyConstructArguments),
		asCALL_CDECL);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments()",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in, ?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in, ?& in, ?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour(
		"Constructs a Arguments instance with the given arguments.",
		"Arguments",
		asBEHAVE_FACTORY,
		"Arguments@ Arguments(?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ArgumentsConstructArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectBehaviour("AddRef", "Arguments", asBEHAVE_ADDREF, "void AddRef()", asMETHOD(CASArguments, AddRef), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour(
		"Release",
		"Arguments",
		asBEHAVE_RELEASE,
		"void Release()",
		asMETHOD(CASArguments, Release),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Assignment operator",
		"Arguments",
		"Arguments& opAssign(const Arguments& in other)",
		asMETHOD(CASArguments, operator=),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets number of arguments.",
		"Arguments",
		"size_t GetCount() const",
		asMETHOD(CASArguments, GetCount),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Returns whether this instance holds any arguments.",
		"Arguments",
		"bool HasArguments() const",
		asMETHOD(CASArguments, HasArguments),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clears this instance's data.", "Arguments", "void Clear()", asMETHOD(CASArguments, Clear), asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments()",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in, ?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in, ?& in, ?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Sets the arguments this instance contains.",
		"Arguments",
		"bool SetArguments(?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASArguments_SetArguments),
		asCALL_GENERIC);
}

void RegisterSCScriptReflectionFunction(CASDocumentation *documentation)
{
	RegisterCallable<CASFunction>(documentation, "Reflection function class. Can be used to call global functions.", "Function");
	documentation->RegisterObjectMethod(
		"Calls this function. pArguments must be valid.",
		"Function",
		"ReturnValue@ Call(Arguments@ pArguments)",
		asFUNCTIONPR(CASBaseCallable_Call, (CASArguments *, CASBaseCallable *), CASReturnValue *),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Calls this function.", "Function", "ReturnValue@ Call()", asFUNCTION(CASFunction_Call), asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in, ?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in, ?& in, ?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this function.",
		"Function",
		"ReturnValue@ Call(?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASFunction_Call),
		asCALL_GENERIC);
}

void RegisterSCScriptReflectionMethod(CASDocumentation *documentation)
{
	RegisterCallable<CASMethod>(documentation, "Reflection method class. Can be used to call object methods.", "Method");
	documentation->RegisterObjectMethod(
		"Calls this method. pArguments must be valid.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, Arguments@ pArguments)",
		asFUNCTION(CASMethod_Call),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in, ?& in, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
	documentation->RegisterObjectMethod(
		"Calls this method.",
		"Method",
		"ReturnValue@ Call(?& in thisObject, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(CASMethod_Call_Varargs),
		asCALL_GENERIC);
}

void RegisterSCScriptReflectionObjectType(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Represents an object type", "ObjectType", 0, asOBJ_REF);
	documentation->RegisterObjectBehaviour(
		"AddRef",
		"ObjectType",
		asBEHAVE_ADDREF,
		"void AddRef()",
		asMETHOD(CASObjectType, AddRef),
		asCALL_THISCALL);
	documentation->RegisterObjectBehaviour(
		"Release",
		"ObjectType",
		asBEHAVE_RELEASE,
		"void Release()",
		asMETHOD(CASObjectType, Release),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets the number of methods this object type has.",
		"ObjectType",
		"uint GetMethodCount() const",
		asMETHOD(CASObjectType, GetMethodCount),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets the method at the given index.",
		"ObjectType",
		"Method@ GetMethodByIndex(uint uiIndex) const",
		asMETHOD(CASObjectType, GetMethodByIndex),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets a method by declaration.",
		"ObjectType",
		"Method@ GetMethod(const string& in szFunctionSignature)",
		asFUNCTION(CASBaseObjectData_GetMethod),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod(
		"Gets a method by declaration.",
		"ObjectType",
		"bool MethodExists(const string& in szFunctionSignature)",
		asFUNCTION(CASBaseObjectData_MethodExists),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod(
		"Returns whether the given object type is identical to this one.",
		"ObjectType",
		"bool Equals(const ObjectType@ pObjectType) const",
		asMETHOD(CASObjectType, Equals),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Returns whether the given object type is compatible with this one.",
		"ObjectType",
		"bool IsCompatible(const ObjectType@ pObjectType) const",
		asMETHOD(CASObjectType, IsCompatible),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Returns whether this object derives from the given type.",
		"ObjectType",
		"bool DerivesFrom(const string& in szName) const",
		asFUNCTION(CASObjectType_DerivesFrom),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod(
		"Returns whether this object derives from the given type.",
		"ObjectType",
		"bool DerivesFrom(const ObjectType@ pObjectType) const",
		asMETHODPR(CASObjectType, DerivesFrom, (const CASObjectType *) const, bool),
		asCALL_THISCALL);
}

void RegisterSCScriptReflection(CASDocumentation *documentation)
{
	documentation->SetDefaultNamespace("Reflection");
	documentation->RegisterEnum("Which type a reflection argument is", "ArgumentType");
	documentation->RegisterEnumValue(
		"Argument has no type. If an argument has this type it means something went wrong during parsing.",
		"ArgumentType",
		"AT_NONE",
		1);
	documentation->RegisterEnumValue("Argument is void.", "ArgumentType", "AT_VOID", 2);
	documentation->RegisterEnumValue("Argument is a primitive type.", "ArgumentType", "AT_PRIMITIVE", 4);
	documentation->RegisterEnumValue("Argument is an object type.", "ArgumentType", "AT_OBJECT", 24);
	documentation->RegisterEnumValue("Argument is an enum type.", "ArgumentType", "AT_ENUM", 32);
	RegisterSCScriptReflectionArguments(documentation);
	documentation->RegisterObjectType("Reflection return value.", "ReturnValue", sizeof(CASReturnValue), asOBJ_REF);
	documentation->RegisterObjectBehaviour(
		"AddRef",
		"ReturnValue",
		asBEHAVE_ADDREF,
		"void AddRef()",
		asMETHOD(CASReturnValue, AddRef),
		asCALL_THISCALL);
	documentation->RegisterObjectBehaviour(
		"Release",
		"ReturnValue",
		asBEHAVE_RELEASE,
		"void Release()",
		asMETHOD(CASReturnValue, Release),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Returns whether there is a return value or not.",
		"ReturnValue",
		"bool HasReturnValue() const",
		asMETHOD(CASReturnValue, HasReturnValue),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Returns the return value as an any instance, if there was a return value.",
		"ReturnValue",
		"any@ ToAny() const",
		asFUNCTION(CASReturnValue_ToAny),
		asCALL_CDECL_OBJLAST);
	RegisterCallable<CASBaseCallable>(documentation, "Reflection base class for callable objects.", "Callable");
	RegisterSCScriptReflectionFunction(documentation);
	RegisterSCScriptReflectionMethod(documentation);
	RegisterSCScriptReflectionObjectType(documentation);
	documentation->RegisterObjectType(
		"Reflection group. Represents a group of reflectable objects.",
		"IReflectionGroup",
		0,
		asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod(
		"Finds a global function. Searches by name by default, searches by declaration if fSearchByDecl is set to true.",
		"IReflectionGroup",
		"Function@ FindGlobalFunction(const string& in szName, bool fSearchByDecl = false)",
		asMETHOD(IASReflectionGroup, FindGlobalFunction),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets the number of global functions.",
		"IReflectionGroup",
		"uint GetGlobalFunctionCount() const",
		asMETHOD(IASReflectionGroup, GetGlobalFunctionCount),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets the global function at the given index.",
		"IReflectionGroup",
		"Function@ GetGlobalFunctionByIndex(uint uiIndex)",
		asMETHOD(IASReflectionGroup, GetGlobalFunctionByIndex),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Finds an object type. Searches by name by default, searches by declaration if fSearchByDecl is set to true.",
		"IReflectionGroup",
		"ObjectType@ FindObjectType(const string& in szName, bool fSearchByDecl = false)",
		asMETHOD(IASReflectionGroup, FindObjectType),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets the number of object types.",
		"IReflectionGroup",
		"uint GetObjectTypeCount() const",
		asMETHOD(IASReflectionGroup, GetObjectTypeCount),
		asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Gets the object type at the given index.",
		"IReflectionGroup",
		"ObjectType@ GetObjectTypeByIndex(uint uiIndex) const",
		asMETHOD(IASReflectionGroup, GetObjectTypeByIndex),
		asCALL_THISCALL);
	documentation->RegisterObjectType("Reflection global used for generial purpose needs.", "CReflection", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectProperty("Engine group", "CReflection", "IReflectionGroup Engine", asOFFSET(CASASReflection, m_EngineGroup));
	documentation->RegisterObjectProperty("Module group", "CReflection", "IReflectionGroup Module", asOFFSET(CASASReflection, m_ModuleGroup));
	documentation->RegisterGlobalProperty("Reflection global class instance.", "CReflection g_Reflection", CASASReflection::GetInstance());
	documentation->SetDefaultNamespace("");
}
