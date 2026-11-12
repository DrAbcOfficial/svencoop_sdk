#include "extdll.h"
#include "util.h"
#include "scripting/core/CASBaseManager.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASModuleInfo.h"
#include "scripting/core/CASModuleBuilder.h"
#include "scripting/core/CASModuleHandle.h"
#include "scripting/core/CASModuleHookManager.h"
#include "scripting/core/CASContextPool.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASDebugger.h"
#include "scripting/core/CASBaseRegistration.h"
#include "scripting/core/CASHookManager.h"
#include "scripting/core/CASHook.h"
#include "scripting/core/IASImplementationFactory.h"
#include "scripting/core/IASScriptGlobal.h"
#include "server/server_support.h"
#include "scripting/core/CASLog.h"
#include "scripting/core/CASVirtualFileSystem.h"
#include <angelscript.h>
#include <algorithm>

const float CASBaseManager::GARBAGE_COLLECT_INTERVAL = 10.0f;

static const int SCRIPT_PHASE_NONE = 0;
static const int SCRIPT_PHASE_MAP_BEGIN = 1;
static const int SCRIPT_PHASE_MAP_INIT = 2;
static const int SCRIPT_PHASE_MAP_ACTIVATE = 3;
static const int SCRIPT_PHASE_MAP_START = 4;
static const int SCRIPT_PHASE_MAP_END = 5;
static const int SCRIPT_STARTUP_FAILED = 0;
static const int SCRIPT_STARTUP_SUCCESS = 1;
static const int SCRIPT_ALREADY_STARTED = 2;
static const int SCRIPT_COMPILER_WARNINGS = 1;
static const int SCRIPT_COMPILER_ERRORS = 2;
static const int SCRIPT_PROPERTY_ACCESSOR_MODE = 2;
static const float SCRIPT_GARBAGE_COLLECT_DISABLED_TIME = -1.0f;
static const ASLogLevel_t SCRIPT_LIFECYCLE_LOG_LEVEL = (ASLogLevel_t) 1;

cvar_t as_compiler_treat_warnings_as_errors =
{
	"as_compiler_treat_warnings_as_errors", "0", FCVAR_PROTECTED
};

extern CASHook g_MapChangeHook;

void CASBaseManager::MessageCallback(const asSMessageInfo *message)
{
	if (message->type == asMSGTYPE_ERROR)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "%s (%d, %d) : %s\n",
			message->section, message->row, message->col, message->message);
	}
	else if (message->type == asMSGTYPE_WARNING)
	{
		CASLog::GetInstance()->Warning((ASLogLevel_t)COMPILER_WARNING_LOG_LEVEL,
			"%s (%d, %d) : %s\n", message->section, message->row, message->col, message->message);
	}
	else
	{
		CASLog::GetInstance()->Log((ASLogLevel_t)COMPILER_INFORMATION_LOG_LEVEL,
			"%s (%d, %d) : %s\n", message->section, message->row, message->col, message->message);
	}
}

bool CASBaseManager::IsActiveModuleOfType(ModuleType::Type type) const
{
	return m_pActiveModule && m_pActiveModule->m_pModuleInfo->m_ModuleType == type;
}

void CASBaseManager::MapActivate(void)
{
	PreMapActivate();

	bool success = OnMapActivate();

	PostMapActivate(success);

	m_iPhase = SCRIPT_PHASE_MAP_ACTIVATE;
}

void CASBaseManager::MapStart(void)
{
	PreMapStart();

	bool success = OnMapStart();

	PostMapStart(success);

	m_iPhase = SCRIPT_PHASE_MAP_START;
}

void CASBaseManager::Update(void)
{
}

bool CASBaseManager::SetExceptionOnActiveContext(const char *message, bool logMissingContext)
{
	if (!message)
		return false;

	asIScriptContext *context = asGetActiveContext();

	if (!context)
	{
		if (logMissingContext)
		{
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
				"CASBaseManager::SetExceptionOnActiveContext: tried to set exception on null context!\n");
		}

		return false;
	}

	context->SetException(message, true);
	return true;
}

void CASBaseManager::AddScriptGlobal(IASScriptGlobal *global)
{
	if (!global)
		return;

	if (IsRegisteredScriptGlobal(global))
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Global registered twice!\n");
		return;
	}

	m_ScriptGlobals.push_back(global);
}

void CASBaseManager::RemoveScriptGlobal(IASScriptGlobal *global)
{
	if (!global)
		return;

	std::vector<IASScriptGlobal *>::iterator entry =
		std::find(m_ScriptGlobals.begin(), m_ScriptGlobals.end(), global);

	if (entry == m_ScriptGlobals.end())
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Attempted to remove global that was never added!\n");
		return;
	}

	m_ScriptGlobals.erase(entry);
}

bool CASBaseManager::IsRegisteredScriptGlobal(IASScriptGlobal *global)
{
	return global && std::find(m_ScriptGlobals.begin(), m_ScriptGlobals.end(), global) != m_ScriptGlobals.end();
}

asITypeInfo *CASBaseManager::GetTypeInfoByName(const std::string &name)
{
	std::map<std::string, asITypeInfo *>::iterator entry = m_ObjectTypeCache.find(name);

	if (entry != m_ObjectTypeCache.end())
		return entry->second;

	asITypeInfo *type = m_pEngine->GetTypeInfoByName(name.c_str());

	if (!type)
		return NULL;

	std::pair<std::map<std::string, asITypeInfo *>::iterator, bool> result =
		m_ObjectTypeCache.insert(std::make_pair(name, type));

	if (!result.second)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"CASBaseManager::GetTypeInfoByName: failed to insert object type '%s' in cache! (cache size: %u)\n",
			type->GetName(), (unsigned int)m_ObjectTypeCache.size());
		return NULL;
	}

	type->AddRef();
	return type;
}

void CASBaseManager::ClearObjectTypeCache(void)
{
	for (std::map<std::string, asITypeInfo *>::iterator entry = m_ObjectTypeCache.begin();
		entry != m_ObjectTypeCache.end(); entry++)
	{
		entry->second->Release();
	}

	m_ObjectTypeCache.clear();
}

void CASBaseManager::SetupFileSystem(CASVirtualFileSystem &fileSystem)
{
	unsigned char moduleTypes = (1 << ModuleType::MAP) | (1 << ModuleType::PLUGIN);

	fileSystem.CreateDirectory("scripts", moduleTypes, FileAccessBit::READ, 0, 0);
	fileSystem.AddExtension("as");
}

CASBaseManager::CASBaseManager(IASImplementationFactory *factory)
	: m_bBuildAttempted(false), m_bBuildSucceeded(false), m_bScriptLifecycleEntered(false),
	  m_pFactory(factory), m_pEngine(NULL), m_pDebugger(NULL), m_pMapModule(NULL),
	  m_pActiveModule(NULL), m_pFileSystem(NULL),
	  m_flNextGarbageCollect(SCRIPT_GARBAGE_COLLECT_DISABLED_TIME), m_iPhase(SCRIPT_PHASE_NONE)
{
}

CASBaseManager::~CASBaseManager()
{
}

int CASBaseManager::Startup(void)
{
	if (m_pEngine)
		return SCRIPT_ALREADY_STARTED;

	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	if (!m_pEngine)
	{
		UTIL_HostError("Failed to create Angelscript engine!\n", -1);
		return SCRIPT_STARTUP_FAILED;
	}

	m_pFileSystem = new CASVirtualFileSystem;
	m_pEngine->SetMessageCallback(asMETHOD(CASBaseManager, MessageCallback), this, asCALL_THISCALL);
	m_pEngine->SetContextCallbacks(ASRequestContext, ASReturnContext, CASContextPool::GetInstance());
	m_pEngine->SetEngineProperty(asEP_COMPILER_WARNINGS,
		as_compiler_treat_warnings_as_errors.value > 0.0f ? SCRIPT_COMPILER_ERRORS : SCRIPT_COMPILER_WARNINGS);
	m_pEngine->SetEngineProperty(asEP_EXPAND_DEF_ARRAY_TO_TMPL, 1);
	m_pEngine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, 1);
	m_pEngine->SetEngineProperty(asEP_PROPERTY_ACCESSOR_MODE, SCRIPT_PROPERTY_ACCESSOR_MODE);
	m_pEngine->SetEngineProperty(asEP_IGNORE_DUPLICATE_SHARED_INTF, 1);

	CASLog *log = CASLog::GetInstance();

	log->Initialize(m_pFactory->GetLogFilename());

	{
		CASDocumentation documentation(m_pEngine);

		CASLog::GetInstance()->Log(SCRIPT_LIFECYCLE_LOG_LEVEL,
			"Registering Sven Co-op Angelscript API\nAngelscript version: %s\nSven Co-op API version: %s\n",
			"2.36.1", "1.26");

		m_pDebugger = new CASDebugger;
		CASHookManager::GetInstance()->RegisterHooks();

		CASBaseRegistration *registration = m_pFactory->CreateRegistration();

		registration->RegisterGameInterface(&documentation);
		registration->RegisterToStringCallbacks(m_pEngine);
		delete registration;

		documentation.RegistrationComplete();
	}

	CASLog::GetInstance()->Log(SCRIPT_LIFECYCLE_LOG_LEVEL,
		"Finished registering Sven Co-op Angelscript API\n");

	SetupFileSystem(*m_pFileSystem);
	m_pFileSystem->Startup();
	return SCRIPT_STARTUP_SUCCESS;
}

void CASBaseManager::Shutdown(void)
{
	if (!m_pEngine)
		return;

	m_pFileSystem->Shutdown();
	CASLog::GetInstance()->Shutdown();
	m_bBuildAttempted = false;
	m_bBuildSucceeded = false;
	m_bScriptLifecycleEntered = false;
	CASHookManager::GetInstance()->UnregisterHooks();

	delete m_pDebugger;
	m_pDebugger = NULL;

	delete m_pMapModule;
	m_pMapModule = NULL;

	CASContextPool::GetInstance()->ClearAll();

	if (m_pEngine)
		m_pEngine->Release();

	m_pEngine = NULL;
	delete m_pFileSystem;
	m_pFileSystem = NULL;
}

void CASBaseManager::OnMapBegin(void)
{
	CASContextPool::GetInstance()->Initialize();

	for (std::vector<IASScriptGlobal *>::iterator entry = m_ScriptGlobals.begin();
		entry != m_ScriptGlobals.end(); entry++)
	{
		(*entry)->Initialize();
	}

	m_pMapModule = new CASModuleHandle(ModuleType::MAP, "MapModule");
	m_iPhase = SCRIPT_PHASE_MAP_BEGIN;
	m_flNextGarbageCollect = gpGlobals->time + GARBAGE_COLLECT_INTERVAL;
}

bool CASBaseManager::IsScriptLifecycleEntered(void) const
{
	return m_bScriptLifecycleEntered;
}

CASModule *CASBaseManager::GetMapModule(void) const
{
	return m_pMapModule ? m_pMapModule->m_pModule : NULL;
}

void CASBaseManager::LoadMapScript(const char *fileName)
{
	if (!fileName)
		return;

	if (!m_pMapModule || !m_pMapModule->m_bPendingBuild)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"LoadMapScript called when map module is not ready!\n");
		return;
	}

	if (!*fileName)
		return;

	int result = m_pMapModule->m_pBuilder->AddScriptFromFile(fileName);

	if (!result)
	{
		CASLog::GetInstance()->Log(SCRIPT_LIFECYCLE_LOG_LEVEL,
			"Map script '%s' loaded\n", fileName);
	}
	else if (result < 0)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"Could not load map script '%s'\n", fileName);
	}
}

void CASBaseManager::LoadModules(void)
{
	CASModuleHandle *module = m_pMapModule;

	m_bScriptLifecycleEntered = true;
	m_bBuildSucceeded = module->Build(false);
	m_bBuildAttempted = true;

	if (m_bBuildSucceeded)
	{
		if (module->m_pBuilder->m_uiScriptCount)
			CASLog::GetInstance()->Log(SCRIPT_LIFECYCLE_LOG_LEVEL, "Map script compilation succeeded\n");
	}
	else
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Map script compilation failed\n");
	}

	module->FreeBuilder();
	MapInit();
}

void CASBaseManager::MapInit(void)
{
	PreMapInit();

	bool success = OnMapInit();

	PostMapInit(success);
	m_iPhase = SCRIPT_PHASE_MAP_INIT;
}

bool CASBaseManager::OnMapInit(void)
{
	if (!m_bBuildSucceeded)
		return false;

	return GetMapModule()->CallFunction(true, "void MapInit()", 0);
}

bool CASBaseManager::OnMapActivate(void)
{
	return GetMapModule()->CallFunction(true, "void MapActivate()", 0);
}

bool CASBaseManager::OnMapStart(void)
{
	return GetMapModule()->CallFunction(true, "void MapStart()", 0);
}

void CASBaseManager::OnMapEnd(const char *nextMap)
{
	if (m_bScriptLifecycleEntered)
	{
		CString mapName(nextMap);

		g_MapChangeHook.Call(0, &mapName);
	}

	m_bBuildAttempted = false;
	m_bBuildSucceeded = false;
	m_bScriptLifecycleEntered = false;

	delete m_pMapModule;
	m_pMapModule = NULL;

	if (m_pActiveModule)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Active module was not set to null!\n");
		m_pActiveModule = NULL;
	}

	for (std::vector<IASScriptGlobal *>::iterator entry = m_ScriptGlobals.begin();
		entry != m_ScriptGlobals.end(); entry++)
	{
		(*entry)->Shutdown();
	}

	m_pEngine->GarbageCollect(asGC_FULL_CYCLE, 1);
	ClearObjectTypeCache();
	CASLog::GetInstance()->CloseLogFile();
	m_pFileSystem->MapChange();
	m_iPhase = SCRIPT_PHASE_MAP_END;
}

void CASBaseManager::Think(void)
{
	m_pMapModule->m_pModule->Think();

	for (std::vector<IASScriptGlobal *>::iterator entry = m_ScriptGlobals.begin();
		entry != m_ScriptGlobals.end(); entry++)
	{
		(*entry)->Think();
	}

	CASContextPool::GetInstance()->Think(false);

	float time = UTIL_GetCurrentTime();

	if (time >= m_flNextGarbageCollect)
	{
		m_pEngine->GarbageCollect(asGC_ONE_STEP, 1);
		m_flNextGarbageCollect = UTIL_GetCurrentTime() + GARBAGE_COLLECT_INTERVAL;
	}
}

bool CASBaseManager::CallFunctionOnAllModules(CBasePlayer *player, unsigned char moduleTypes,
                                             const CASHook &hook, int argumentCount, va_list arguments)
{
	if (!m_pMapModule)
		return false;

	HookReturnCode_t result = HOOK_CONTINUE;
	CASModuleHookManager *hooks = m_pMapModule->m_pModule->m_pHookManager;

	if (hooks && (moduleTypes & (1 << ModuleType::MAP)))
		result = hooks->CallHook(hook, argumentCount, arguments);

	return result == HOOK_HANDLED;
}

bool CASBaseManager::CallFunctionOnAllModules(CBasePlayer *player, unsigned char moduleTypes,
                                             const CASHook &hook, int argumentCount, ...)
{
	va_list arguments;

	va_start(arguments, argumentCount);

	bool result = false;

	if (m_pMapModule)
		result = CallFunctionOnAllModules(player, moduleTypes, hook, argumentCount, arguments);

	va_end(arguments);
	return result;
}
