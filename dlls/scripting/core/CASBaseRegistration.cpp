#include "scripting/core/CASBaseRegistration.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASDebugger.h"
#include "scripting/core/CASHook.h"
#include "scripting/core/CASHookManager.h"
#include "scripting/core/CASModuleHookManager.h"
#include "scripting/core/CASScriptInfo.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASScheduler.h"
#include "scripting/core/CScriptDictionary.h"
#include "scripting/api/ASVarious.h"
#include "CString.h"
#include "../../../external/angelscript/sdk/add_on/scriptany/scriptany.h"
#include "../../../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"
#include "../../../external/angelscript/sdk/add_on/scriptgrid/scriptgrid.h"
#include "../../../external/angelscript/sdk/add_on/scripthandle/scripthandle.h"
#include "../../../external/angelscript/sdk/add_on/scriptmath/scriptmath.h"
#include "../../../external/angelscript/sdk/add_on/scriptmath/scriptmathcomplex.h"
#include "../../../external/angelscript/sdk/add_on/weakref/weakref.h"

void RegisterSCScriptASString(asIScriptEngine *engine, CASDocumentation *documentation);
void RegisterSCScriptReflection(CASDocumentation *documentation);
void RegisterSCScriptRegex(CASDocumentation *documentation);
void RegisterSCScriptFileSystem(CASDocumentation *documentation);
void RegisterSCScriptDebugger(CASDocumentation *documentation);

std::string SCScriptStringToString(void *object, int expandMembersLevel, CDebugger *debugger)
{
	return ((CString *)object)->Data();
}

std::string SCScriptArrayToString(void *object, int expandMembersLevel, CDebugger *debugger)
{
	return "Array: " + std::to_string(((CScriptArray *)object)->GetSize()) + " elements";
}

CASBaseRegistration::CASBaseRegistration()
{
}

CASBaseRegistration::~CASBaseRegistration()
{
}

void CASBaseRegistration::RegisterGameInterface(CASDocumentation *documentation)
{
	RegisterInterfaces(documentation);

	RegisterDependentInterfaces(documentation);
}

void CASBaseRegistration::RegisterToStringCallbacks(asIScriptEngine *engine)
{
	CASDebugger *debugger = CASServerManager::GetInstance()->m_pDebugger;

	debugger->RegisterToStringCallback(engine->GetTypeInfoByName("string"), SCScriptStringToString);
	debugger->RegisterToStringCallback(engine->GetTypeInfoByName("array"), SCScriptArrayToString);
}

void CASBaseRegistration::RegisterInterfaces(CASDocumentation *documentation)
{
	documentation->SetDefaultAccessMask(AS_ACCESS_ALL);
	RegisterSCScriptVarious(documentation);
	RegisterScriptAny(documentation->m_pEngine);
	RegisterScriptArray(documentation->m_pEngine, true);
	RegisterSCScriptstring_t(documentation);
	RegisterSCScriptASString(documentation->m_pEngine, documentation);
	RegisterScriptDictionary(documentation->m_pEngine);
	RegisterScriptGrid(documentation->m_pEngine);
	RegisterScriptHandle(documentation->m_pEngine);
	RegisterScriptMath(documentation->m_pEngine);
	RegisterScriptMathComplex(documentation->m_pEngine);
	RegisterScriptWeakRef(documentation->m_pEngine);
	RegisterSCScriptReflection(documentation);
	RegisterSCScriptRegex(documentation);
	RegisterSCScriptFileSystem(documentation);
	RegisterSCScriptScheduler(documentation);
	RegisterSCScriptDebugger(documentation);
}

void CASBaseRegistration::RegisterDependentInterfaces(CASDocumentation *documentation)
{
	RegisterSCScriptModuleHookReturnCode(documentation);
	CASHookManager::GetInstance()->RegisterSCScriptHooks(documentation);
	RegisterSCScriptModuleHookManager(documentation);
	RegisterSCScriptPluginInfo(documentation);
	RegisterSCScriptModule(documentation);
}
