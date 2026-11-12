#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASServerImplementationFactory.h"
#include "scripting/core/CASCommonServerModuleValidation.h"
#include "scripting/core/CASPluginManager.h"
#include "scripting/core/CASPersistence.h"
#include "scripting/core/CASModuleHandle.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASModuleHookManager.h"
#include "scripting/core/CASConCommandSystem.h"
#include "server/CCustomEntityHandler.h"
#include "scripting/core/IASServerScriptGlobal.h"
#include "scripting/core/CASHook.h"
#include "scripting/core/CASVirtualFileSystem.h"
#include "scripting/api/ASFileSystem.h"

static const unsigned int SCRIPT_WRITE_FILE_LIMIT = 1024;

CASServerManager::CASServerManager()
	: CASBaseManager(new CASServerImplementationFactory),
	  m_pPersistence(NULL), m_pPluginManager(new CASPluginManager)
{
}

CASServerManager::~CASServerManager()
{
}

CASServerManager *CASServerManager::GetInstance(void)
{
	static CASServerManager *instance;

	if (!instance)
		instance = new CASServerManager;

	return instance;
}

int CASServerManager::Startup(void)
{
	int result = CASBaseManager::Startup();

	if (result == 1)
	{
		CASCommonServerModuleValidation::PrepareValidator();
		m_pPersistence = new CASPersistence;
	}

	return result;
}

void CASServerManager::Shutdown(void)
{
	delete m_pPluginManager;
	m_pPluginManager = NULL;

	if (m_pPersistence)
	{
		delete m_pPersistence;
		m_pPersistence = NULL;
	}

	CASCommonServerModuleValidation::CleanupValidator();
	CASBaseManager::Shutdown();
}

void CASServerManager::WorldCreated(void)
{
	CASConCommandSystem::GetInstance()->m_StateByte = 1;
}

void CASServerManager::OnMapBegin(void)
{
	m_pPluginManager->LoadAllPlugins(false, false);
	CASBaseManager::OnMapBegin();
}

void CASServerManager::OnMapEnd(const char *nextMap)
{
	CCustomEntityHandler::Clear();
	m_pPluginManager->OnMapEnd(nextMap);

	CASConCommandSystem *commands = CASConCommandSystem::GetInstance();

	commands->RemoveAllCommandsOfModule(m_pMapModule->GetModuleName());
	CASBaseManager::OnMapEnd(nextMap);
}

void CASServerManager::Think(void)
{
	m_pPluginManager->Think();
	CCustomEntityHandler::Think();
	CASBaseManager::Think();
}

void CASServerManager::PreMapInit(void)
{
	CASBaseManager::PreMapInit();
	m_pPersistence->PreMapInit(STRING(gpGlobals->mapname));

	for (std::vector<IASScriptGlobal *>::iterator entry = m_ScriptGlobals.begin(); entry != m_ScriptGlobals.end(); entry++)
		static_cast<IASServerScriptGlobal *>(*entry)->PreMapInit();
}

bool CASServerManager::OnMapInit(void)
{
	m_pPluginManager->CallMapFunction("void MapInit()");
	return CASBaseManager::OnMapInit();
}

void CASServerManager::PostMapInit(bool success)
{
	CASBaseManager::PostMapInit(success);
	m_pPersistence->PostMapInit();

	for (std::vector<IASScriptGlobal *>::iterator entry = m_ScriptGlobals.begin(); entry != m_ScriptGlobals.end(); entry++)
		static_cast<IASServerScriptGlobal *>(*entry)->PostMapInit(success);

	CASConCommandSystem::GetInstance()->m_StateByte = 0;
	CASConCommandSystem::GetInstance()->ExecuteQueue();
}

void CASServerManager::PreMapActivate(void)
{
	CASBaseManager::PreMapActivate();

	for (std::vector<IASScriptGlobal *>::iterator entry = m_ScriptGlobals.begin(); entry != m_ScriptGlobals.end(); entry++)
		static_cast<IASServerScriptGlobal *>(*entry)->PreMapActivate();
}

bool CASServerManager::OnMapActivate(void)
{
	m_pPluginManager->CallMapFunction("void MapActivate()");
	return CASBaseManager::OnMapActivate();
}

void CASServerManager::PostMapActivate(bool success)
{
	CASBaseManager::PostMapActivate(success);

	for (std::vector<IASScriptGlobal *>::iterator entry = m_ScriptGlobals.begin(); entry != m_ScriptGlobals.end(); entry++)
		static_cast<IASServerScriptGlobal *>(*entry)->PostMapActivate(success);
}

bool CASServerManager::OnMapStart(void)
{
	m_pPluginManager->CallMapFunction("void MapStart()");
	return CASBaseManager::OnMapStart();
}

bool CASServerManager::CallFunctionOnAllModules(CBasePlayer *player, unsigned char moduleTypes,
	const CASHook &hook, int argumentCount, va_list arguments)
{
	if (!m_pMapModule)
		return false;

	CASModuleHookManager *hooks = m_pMapModule->m_pModule->m_pHookManager;
	HookReturnCode_t result = HOOK_CONTINUE;

	if (hooks && (moduleTypes & (1 << ModuleType::MAP)))
		result = hooks->CallHook(hook, argumentCount, arguments);

	if ((hook.m_StopMode == (HookStopMode::Type)2 || result == HOOK_CONTINUE) &&
		(moduleTypes & (1 << ModuleType::PLUGIN)))
	{
		m_pPluginManager->CallFunctionOnAllModules(player, hook, argumentCount, arguments, result);
	}

	return result == HOOK_HANDLED;
}

bool CASServerManager::CallFunctionOnAllModules(CBasePlayer *player, unsigned char moduleTypes,
	const CASHook &hook, int argumentCount, ...)
{
	if (!m_pMapModule)
		return false;

	va_list arguments;

	va_start(arguments, argumentCount);
	bool result = CallFunctionOnAllModules(player, moduleTypes, hook, argumentCount, arguments);
	va_end(arguments);
	return result;
}

void CASServerManager::SetupFileSystem(CASVirtualFileSystem &fileSystem)
{
	CASBaseManager::SetupFileSystem(fileSystem);

	fileSystem.SetFileQuota(ASComputeFileQuota(), SCRIPT_WRITE_FILE_LIMIT);
	fileSystem.m_uiAccessFlags = FileAccessBit::READ | FileAccessBit::WRITE;

	unsigned char mapModules = 1 << ModuleType::MAP;
	unsigned char pluginModules = 1 << ModuleType::PLUGIN;
	unsigned char readWrite = FileAccessBit::READ | FileAccessBit::WRITE;
	unsigned char mapTempFlags = TempDirFlagBit::STARTUP | TempDirFlagBit::MAP_CHANGE | TempDirFlagBit::SHUTDOWN;
	unsigned char pluginTempFlags = TempDirFlagBit::STARTUP | TempDirFlagBit::SHUTDOWN;

	fileSystem.CreateDirectory("scripts/maps", mapModules, FileAccessBit::READ,
		DirectoryFlagBit::IMPLICIT_SUBDIRECTORIES, TempDirFlagBit::NONE);
	fileSystem.CreateDirectory("scripts/maps/temp", mapModules, readWrite,
		DirectoryFlagBit::NONE, mapTempFlags);
	fileSystem.CreateDirectory("scripts/maps/store", mapModules, readWrite,
		DirectoryFlagBit::IMPLICIT_SUBDIRECTORIES, TempDirFlagBit::NONE);
	fileSystem.CreateDirectory("scripts/plugins", pluginModules, FileAccessBit::READ,
		DirectoryFlagBit::IMPLICIT_SUBDIRECTORIES, TempDirFlagBit::NONE);
	fileSystem.CreateDirectory("scripts/plugins/temp", pluginModules, readWrite,
		DirectoryFlagBit::NONE, pluginTempFlags);
	fileSystem.CreateDirectory("scripts/plugins/store", pluginModules, readWrite,
		DirectoryFlagBit::IMPLICIT_SUBDIRECTORIES, TempDirFlagBit::NONE);
}
