#include "extdll.h"
#include "enginecallback.h"
#include "SvenVersion.h"
#include "server_support.h"
#include "CASDocumentation.h"
#include "CASLog.h"
#include "CEscapeSequences.h"
#include "CKeyvaluesWriter.h"

const char *const CASDocumentation::GAME_VERSION = SVEN_GAME_VERSION_STRING;
const char *const CASDocumentation::API_VERSION = "1.26";

namespace
{
	template<class T>
	void DeleteDocumentationList(T *&head)
	{
		T *entry = head;

		while (entry)
		{
			T *next = entry->m_pNext;

			delete entry;
			entry = next;
		}

		head = NULL;
	}
}

CASDocumentation::CASDocumentation(asIScriptEngine *engine)
	: m_bOutputDocs(false), m_pEngine(engine), m_pszNamespace(""), m_pFunctions(NULL),
	m_pProperties(NULL), m_pInterfaces(NULL), m_pClasses(NULL), m_pEnums(NULL),
	m_pTypedefs(NULL), m_pFuncDefs(NULL)
{
	m_bOutputDocs = g_engfuncs.pfnCheckParm("-as_outputdocs", NULL) != 0;
}

CASDocumentation::~CASDocumentation()
{
	InterfaceInfo_t *interface = m_pInterfaces;

	while (interface)
	{
		InterfaceInfo_t *next = interface->m_pNext;

		DeleteDocumentationList(interface->m_pMethods);
		delete interface;
		interface = next;
	}

	m_pInterfaces = NULL;
	ClassInfo_t *type = m_pClasses;

	while (type)
	{
		ClassInfo_t *next = type->m_pNext;

		DeleteDocumentationList(type->m_pMethods);
		DeleteDocumentationList(type->m_pProperties);
		delete type;
		type = next;
	}

	m_pClasses = NULL;
	EnumInfo_t *enumeration = m_pEnums;

	while (enumeration)
	{
		EnumInfo_t *next = enumeration->m_pNext;

		DeleteDocumentationList(enumeration->m_pValues);
		delete enumeration;
		enumeration = next;
	}

	m_pEnums = NULL;
	DeleteDocumentationList(m_pFunctions);
	DeleteDocumentationList(m_pProperties);
	DeleteDocumentationList(m_pTypedefs);
	DeleteDocumentationList(m_pFuncDefs);
}

const char *CASDocumentation::FormatInt(int value)
{
	static char buffer[INTEGER_BUFFER_LENGTH];

	_snprintf(buffer, sizeof(buffer), "%i", value);
	return buffer;
}

const char *CASDocumentation::FormatUnsigned(unsigned int value)
{
	static char buffer[INTEGER_BUFFER_LENGTH];

	_snprintf(buffer, sizeof(buffer), "%u", value);
	return buffer;
}

const char *CASDocumentation::FormatChar(char value)
{
	static char buffer[CHARACTER_BUFFER_LENGTH];

	_snprintf(buffer, sizeof(buffer), "'%c'", value);
	return buffer;
}

const char *CASDocumentation::FormatEnumValue(int value, ENUM_TYPE type)
{
	if (type == ENUM_UNSIGNED)
		return FormatUnsigned(value);

	if (type == ENUM_CHAR)
		return FormatChar(value);

	return FormatInt(value);
}

void CASDocumentation::OutputDocInfo(CKeyvaluesWriter &writer)
{
	writer.WriteKeyvalue("DocVersion", FormatInt(DOCUMENTATION_VERSION));
	writer.WriteKeyvalue("GameVersion", GAME_VERSION);
	writer.WriteKeyvalue("APIVersion", API_VERSION);
}

void CASDocumentation::OutputInterfaces(CKeyvaluesWriter &writer)
{
	writer.BeginBlock("Interfaces");

	for (InterfaceInfo_t *entry = m_pInterfaces; entry; entry = entry->m_pNext)
	{
		writer.BeginBlock("Interface");
		writer.WriteKeyvalue("Namespace", entry->m_pszNamespace);
		writer.WriteKeyvalue("InterfaceName", entry->m_pszName);
		writer.WriteKeyvalue("Documentation", entry->m_pszDocumentation);
		writer.BeginBlock("Methods");

		for (MemberInfo_t *method = entry->m_pMethods; method; method = method->m_pNext)
		{
			writer.BeginBlock("Method");
			writer.WriteKeyvalue("Declaration", method->m_pszDeclaration);
			writer.WriteKeyvalue("Documentation", method->m_pszDocumentation);
			writer.EndBlock();
		}

		writer.EndBlock();
		writer.EndBlock();
	}

	writer.EndBlock();
}

void CASDocumentation::OutputClasses(CKeyvaluesWriter &writer)
{
	writer.BeginBlock("Classes");

	for (ClassInfo_t *entry = m_pClasses; entry; entry = entry->m_pNext)
	{
		char flags[FLAGS_BUFFER_LENGTH];

		writer.BeginBlock("Class");
		writer.WriteKeyvalue("Namespace", entry->m_pszNamespace);
		writer.WriteKeyvalue("ClassName", entry->m_pszName);
		writer.WriteKeyvalue("Documentation", entry->m_pszDocumentation);
		_snprintf(flags, sizeof(flags), "%d", (int)entry->m_Flags);
		writer.WriteKeyvalue("Flags", flags);
		writer.BeginBlock("Methods");

		for (MemberInfo_t *method = entry->m_pMethods; method; method = method->m_pNext)
		{
			writer.BeginBlock("Method");
			writer.WriteKeyvalue("Declaration", method->m_pszDeclaration);
			writer.WriteKeyvalue("Documentation", method->m_pszDocumentation);
			writer.EndBlock();
		}

		writer.EndBlock();
		writer.BeginBlock("Properties");

		for (MemberInfo_t *property = entry->m_pProperties; property; property = property->m_pNext)
		{
			writer.BeginBlock("Property");
			writer.WriteKeyvalue("Declaration", property->m_pszDeclaration);
			writer.WriteKeyvalue("Documentation", property->m_pszDocumentation);
			writer.EndBlock();
		}

		writer.EndBlock();
		writer.EndBlock();
	}

	writer.EndBlock();
}

void CASDocumentation::OutputEnums(CKeyvaluesWriter &writer)
{
	writer.BeginBlock("Enums");

	for (EnumInfo_t *entry = m_pEnums; entry; entry = entry->m_pNext)
	{
		writer.BeginBlock("Enum");
		writer.WriteKeyvalue("Namespace", entry->m_pszNamespace);
		writer.WriteKeyvalue("Documentation", entry->m_pszDocumentation);
		writer.WriteKeyvalue("Name", entry->m_pszName);
		writer.BeginBlock("Values");

		for (EnumValueInfo_t *value = entry->m_pValues; value; value = value->m_pNext)
		{
			writer.BeginBlock("Value");
			writer.WriteKeyvalue("Name", value->m_pszName);
			writer.WriteKeyvalue("Value", FormatEnumValue(value->m_iValue, entry->m_Type));
			writer.WriteKeyvalue("Documentation", value->m_pszDocumentation);
			writer.EndBlock();
		}

		writer.EndBlock();
		writer.EndBlock();
	}

	writer.EndBlock();
}

void CASDocumentation::OutputFunctions(CKeyvaluesWriter &writer)
{
	writer.BeginBlock("Functions");

	for (GlobalInfo_t *entry = m_pFunctions; entry; entry = entry->m_pNext)
	{
		writer.BeginBlock("Function");
		writer.WriteKeyvalue("Documentation", entry->m_pszDocumentation);
		writer.WriteKeyvalue("Namespace", entry->m_pszNamespace);
		writer.WriteKeyvalue("Declaration", entry->m_pszDeclaration);
		writer.EndBlock();
	}

	writer.EndBlock();
}

void CASDocumentation::OutputProperties(CKeyvaluesWriter &writer)
{
	writer.BeginBlock("Properties");

	for (GlobalInfo_t *entry = m_pProperties; entry; entry = entry->m_pNext)
	{
		writer.BeginBlock("Property");
		writer.WriteKeyvalue("Documentation", entry->m_pszDocumentation);
		writer.WriteKeyvalue("Namespace", entry->m_pszNamespace);
		writer.WriteKeyvalue("Declaration", entry->m_pszDeclaration);
		writer.EndBlock();
	}

	writer.EndBlock();
}

void CASDocumentation::OutputTypedefs(CKeyvaluesWriter &writer)
{
	writer.BeginBlock("Typedefs");

	for (TypedefInfo_t *entry = m_pTypedefs; entry; entry = entry->m_pNext)
	{
		writer.BeginBlock("Typedef");
		writer.WriteKeyvalue("Documentation", entry->m_pszDocumentation);
		writer.WriteKeyvalue("Namespace", entry->m_pszNamespace);
		writer.WriteKeyvalue("Type", entry->m_pszType);
		writer.WriteKeyvalue("Name", entry->m_pszName);
		writer.EndBlock();
	}

	writer.EndBlock();
}

void CASDocumentation::OutputFuncDefs(CKeyvaluesWriter &writer)
{
	writer.BeginBlock("FuncDefs");

	for (GlobalInfo_t *entry = m_pFuncDefs; entry; entry = entry->m_pNext)
	{
		writer.BeginBlock("FuncDef");
		writer.WriteKeyvalue("Documentation", entry->m_pszDocumentation);
		writer.WriteKeyvalue("Namespace", entry->m_pszNamespace);
		writer.WriteKeyvalue("Name", entry->m_pszDeclaration);
		writer.EndBlock();
	}

	writer.EndBlock();
}

void CASDocumentation::OutputDocumentationToFile(const char *fileName)
{
	if (!fileName || !*fileName)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"CASDocumentation::OutputDocumentationToFile: null or empty filename passed!\n");
		return;
	}

	CKeyvaluesWriter writer(fileName, GetEscapeSeqConversion());

	if (writer.m_hFile)
	{
		writer.BeginBlock("Angelscript Documentation");
		OutputDocInfo(writer);
		OutputInterfaces(writer);
		OutputClasses(writer);
		OutputEnums(writer);
		OutputFunctions(writer);
		OutputProperties(writer);
		OutputTypedefs(writer);
		OutputFuncDefs(writer);
		writer.EndBlock();
	}
	else
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"CASDocumentation::OutputDocumentationToFile: Failed to open file '%s'\n", fileName);
	}
}

void CASDocumentation::OutputGlobalsDocs(const char *fileName)
{
	if (!fileName)
		return;

	if (strstr(fileName, "..") || strstr(fileName, "/") || strstr(fileName, "\\"))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"as_outputdocs: Cannot output documentation; file name contains '..', '/' or '\\', which is not allowed\n");
		return;
	}

	if (strlen(fileName) >= FILE_NAME_LENGTH)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"as_outputdocs: Can't output globals docs to file '%s': file name too long!\n", fileName);
		return;
	}

	char path[FILE_NAME_LENGTH];

	if ((unsigned int)_snprintf(path, sizeof(path), "%s.txt", fileName) >= FILE_NAME_LENGTH)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"as_outputdocs: Failed to output globals docs to file '%s'\n", fileName);
		return;
	}

	OutputDocumentationToFile(path);
}

void CASDocumentation::RegistrationComplete(void)
{
	if (m_bOutputDocs)
	{
		char *fileName;

		g_engfuncs.pfnCheckParm("-as_outputdocs", &fileName);
		OutputGlobalsDocs(fileName ? fileName : "asdocs");
	}
}

const char *CASDocumentation::AllocString(const char *text)
{
	return m_Strings.Allocate(text);
}

const char *CASDocumentation::GetDefaultNamespace(void) const
{
	return m_pEngine->GetDefaultNamespace();
}

int CASDocumentation::SetDefaultNamespace(const char *nameSpace)
{
	int result = m_pEngine->SetDefaultNamespace(nameSpace);

	if (result >= 0)
		m_pszNamespace = nameSpace;

	return result;
}

asDWORD CASDocumentation::SetDefaultAccessMask(asDWORD accessMask)
{
	return m_pEngine->SetDefaultAccessMask(accessMask);
}

void CASDocumentation::RegisterGlobalFunction(const char *documentation, const char *declaration,
	const asSFuncPtr &function, asDWORD callingConvention, void *auxiliary)
{
	int result = m_pEngine->RegisterGlobalFunction(declaration, function, callingConvention, auxiliary);

	if (result >= 0 && m_bOutputDocs)
	{
		GlobalInfo_t *entry = new GlobalInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszNamespace = m_pszNamespace;
		entry->m_pszDeclaration = declaration;
		entry->m_pNext = m_pFunctions;
		m_pFunctions = entry;
	}
}

void CASDocumentation::RegisterGlobalProperty(const char *documentation, const char *declaration, void *address)
{
	int result = m_pEngine->RegisterGlobalProperty(declaration, address);

	if (result >= 0 && m_bOutputDocs)
	{
		GlobalInfo_t *entry = new GlobalInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszNamespace = m_pszNamespace;
		entry->m_pszDeclaration = declaration;
		entry->m_pNext = m_pProperties;
		m_pProperties = entry;
	}
}

void CASDocumentation::RegisterInterface(const char *documentation, const char *name)
{
	int result = m_pEngine->RegisterInterface(name);

	if (result >= 0 && m_bOutputDocs)
	{
		InterfaceInfo_t *entry = new InterfaceInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszNamespace = m_pszNamespace;
		entry->m_pszName = name;
		entry->m_pMethods = NULL;
		entry->m_pNext = m_pInterfaces;
		m_pInterfaces = entry;
	}
}

CASDocumentation::InterfaceInfo_t *CASDocumentation::LookupInterface(const char *name)
{
	for (InterfaceInfo_t *entry = m_pInterfaces; entry; entry = entry->m_pNext)
	{
		if (!strcmp(name, entry->m_pszName))
			return entry;
	}

	return NULL;
}

void CASDocumentation::RegisterInterfaceMethod(const char *documentation, const char *name, const char *declaration)
{
	int result = m_pEngine->RegisterInterfaceMethod(name, declaration);

	if (result >= 0 && m_bOutputDocs)
	{
		MemberInfo_t *entry = new MemberInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszDeclaration = declaration;

		InterfaceInfo_t *type = LookupInterface(name);

		if (type)
		{
			entry->m_pNext = type->m_pMethods;
			type->m_pMethods = entry;
		}
		else
		{
			delete entry;
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASDocumentation::RegisterInterfaceMethod: interface '%s' not registered prior to method '%s' registration!\n",
				name, declaration);
		}
	}
}

void CASDocumentation::RegisterObjectType(const char *documentation, const char *name, int byteSize, asDWORD flags)
{
	int result = m_pEngine->RegisterObjectType(name, byteSize, flags);

	if (result >= 0 && m_bOutputDocs)
	{
		ClassInfo_t *entry = new ClassInfo_t;
		char typeName[TYPE_NAME_LENGTH];
		const char *templateStart = strstr(name, "<");

		entry->m_pszDocumentation = documentation;
		entry->m_pszNamespace = m_pszNamespace;
		entry->m_Flags = flags;

		if (templateStart)
		{
			UTIL_CopyString(typeName, name, templateStart - name + 1);
			entry->m_pszName = AllocString(typeName);
		}
		else
		{
			entry->m_pszName = name;
		}

		entry->m_pMethods = NULL;
		entry->m_pProperties = NULL;
		entry->m_pNext = m_pClasses;
		m_pClasses = entry;
	}
}

CASDocumentation::ClassInfo_t *CASDocumentation::LookupClass(const char *name)
{
	if (!name || !*name)
		return NULL;

	const char *templateStart = strstr(name, "<");

	for (ClassInfo_t *entry = m_pClasses; entry; entry = entry->m_pNext)
	{
		if (templateStart)
		{
			if (!strncmp(name, entry->m_pszName, templateStart - name))
				return entry;
		}
		else if (!strcmp(name, entry->m_pszName))
		{
			return entry;
		}
	}

	return NULL;
}

void CASDocumentation::RegisterObjectMethod(const char *documentation, const char *name, const char *declaration,
	const asSFuncPtr &function, asDWORD callingConvention)
{
	int result = m_pEngine->RegisterObjectMethod(name, declaration, function, callingConvention);

	if (result >= 0 && m_bOutputDocs)
	{
		MemberInfo_t *entry = new MemberInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszDeclaration = declaration;

		ClassInfo_t *type = LookupClass(name);

		if (type)
		{
			entry->m_pNext = type->m_pMethods;
			type->m_pMethods = entry;
		}
		else
		{
			delete entry;
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASDocumentation::RegisterMethod: class '%s' not registered prior to method '%s' registration!\n",
				name, declaration);
		}
	}
}

void CASDocumentation::RegisterObjectBehaviour(const char *documentation, const char *name,
	asEBehaviours behaviour, const char *declaration, const asSFuncPtr &function,
	asDWORD callingConvention, void *auxiliary)
{
	int result = m_pEngine->RegisterObjectBehaviour(name, behaviour, declaration,
		function, callingConvention, auxiliary);

	if (result >= 0 && m_bOutputDocs &&
		(behaviour == asBEHAVE_CONSTRUCT || behaviour == asBEHAVE_LIST_CONSTRUCT ||
		 behaviour == asBEHAVE_FACTORY || behaviour == asBEHAVE_LIST_FACTORY))
	{
		MemberInfo_t *entry = new MemberInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszDeclaration = declaration;

		ClassInfo_t *type = LookupClass(name);

		if (type)
		{
			entry->m_pNext = type->m_pMethods;
			type->m_pMethods = entry;
		}
		else
		{
			delete entry;
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASDocumentation::RegisterMethod: class '%s' not registered prior to method '%s' registration!\n",
				name, declaration);
		}
	}
}

void CASDocumentation::RegisterObjectProperty(const char *documentation, const char *name,
	const char *declaration, int offset)
{
	int result = m_pEngine->RegisterObjectProperty(name, declaration, offset);

	if (result >= 0 && m_bOutputDocs)
	{
		MemberInfo_t *entry = new MemberInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszDeclaration = declaration;

		ClassInfo_t *type = LookupClass(name);

		if (type)
		{
			entry->m_pNext = type->m_pProperties;
			type->m_pProperties = entry;
		}
		else
		{
			delete entry;
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASDocumentation::RegisterMethod: class '%s' not registered prior to property '%s' registration!\n",
				name, declaration);
		}
	}
}

void CASDocumentation::RegisterEnum(const char *documentation, const char *name, ENUM_TYPE type)
{
	int result = m_pEngine->RegisterEnum(name);

	if (result >= 0 && m_bOutputDocs)
	{
		EnumInfo_t *entry = new EnumInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszNamespace = m_pszNamespace;
		entry->m_pszName = name;
		entry->m_Type = type;
		entry->m_pValues = NULL;
		entry->m_pNext = m_pEnums;
		m_pEnums = entry;
	}
}

CASDocumentation::EnumInfo_t *CASDocumentation::LookupEnum(const char *name)
{
	for (EnumInfo_t *entry = m_pEnums; entry; entry = entry->m_pNext)
	{
		if (!strcmp(name, entry->m_pszName))
			return entry;
	}

	return NULL;
}

void CASDocumentation::RegisterEnumValue(const char *documentation, const char *name, const char *valueName, int value)
{
	int result = m_pEngine->RegisterEnumValue(name, valueName, value);

	if (result >= 0 && m_bOutputDocs)
	{
		EnumValueInfo_t *entry = new EnumValueInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszName = valueName;
		entry->m_iValue = value;

		EnumInfo_t *type = LookupEnum(name);

		if (type)
		{
			entry->m_pNext = type->m_pValues;
			type->m_pValues = entry;
		}
		else
		{
			delete entry;
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASDocumentation::RegisterEnumValue: enum '%s' not registered prior to value '%s' registration!\n",
				name, valueName);
		}
	}
}

void CASDocumentation::RegisterTypedef(const char *documentation, const char *name, const char *type)
{
	int result = m_pEngine->RegisterTypedef(name, type);

	if (result >= 0 && m_bOutputDocs)
	{
		TypedefInfo_t *entry = new TypedefInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszNamespace = m_pszNamespace;
		entry->m_pszType = type;
		entry->m_pszName = name;
		entry->m_pNext = m_pTypedefs;
		m_pTypedefs = entry;
	}
}

int CASDocumentation::RegisterFuncDef(const char *documentation, const char *declaration)
{
	int result = m_pEngine->RegisterFuncdef(declaration);

	if (result >= 0 && m_bOutputDocs)
	{
		GlobalInfo_t *entry = new GlobalInfo_t;

		entry->m_pszDocumentation = documentation;
		entry->m_pszNamespace = m_pszNamespace;
		entry->m_pszDeclaration = declaration;
		entry->m_pNext = m_pFuncDefs;
		m_pFuncDefs = entry;
	}

	return result;
}
