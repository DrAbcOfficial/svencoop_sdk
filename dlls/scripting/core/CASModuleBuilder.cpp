#include "extdll.h"
#include "util.h"
#include "enginecallback.h"
#include "scripting/core/CASModuleBuilder.h"
#include "scripting/core/CASModuleInfo.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASHeaderList.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASLog.h"
#include "server/server_filesystem.h"
#include "server/server_resource.h"
#include "server/server_support.h"
#include <angelscript.h>
#include "../../../external/angelscript/sdk/add_on/scriptbuilder/scriptbuilder.h"
#include <string.h>

CASModuleBuilder::CASModuleBuilder(ModuleType::Type type, const char *name)
    : m_pModuleInfo(NULL), m_pScriptBuilder(new CScriptBuilder), m_pModule(NULL),
      m_uiIncludeErrors(0), m_bBuilt(false), m_uiScriptCount(0), m_pHeaders(NULL)
{
	m_pModuleInfo = CASModuleInfo::FindModuleInfo(type);

	if (!m_pModuleInfo)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASModuleBuilder::CASModuleBuilder: null module info!\n");
		return;
	}

	if (!name || !*name)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASModuleBuilder::CASModuleBuilder: null or empty module name!\n");
		return;
	}

	if (!m_pModuleInfo->m_pszDirectory || !*m_pModuleInfo->m_pszDirectory)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASModuleBuilder::CASModuleBuilder: null or empty directory name!\n");
		return;
	}

	m_pScriptBuilder->SetIncludeCallback(::IncludeCallback, this);
	m_pScriptBuilder->DefineWord("SERVER");
	m_pScriptBuilder->StartNewModule(CASServerManager::GetInstance()->m_pEngine, name);
	m_pModule = m_pScriptBuilder->GetModule();
	m_pModule->SetAccessMask(1 << m_pModuleInfo->m_ModuleType);
	m_pHeaders = m_pModuleInfo->GenerateHeaders();

	for (CASHeaderList::Header_t *header = m_pHeaders->m_pHead; header; header = header->m_pNext)
		AddScriptFromBuffer(header->m_pszName, header->m_pszSource);
}

CASModuleBuilder::~CASModuleBuilder()
{
	if (m_pHeaders)
		m_pHeaders->Release();

	m_pHeaders = NULL;
	m_pModule = NULL;

	delete m_pScriptBuilder;
	m_pScriptBuilder = NULL;
}

bool CASModuleBuilder::Build(CASModule **module)
{
	if (m_uiIncludeErrors)
		CASLog::GetInstance()->Log(AS_LOG_DEFAULT,
		                           "Failed to include %u script%s; compilation may fail!\n",
		                           m_uiIncludeErrors, m_uiIncludeErrors == 1 ? "" : "s");

	if (m_uiScriptCount)
		CASLog::GetInstance()->Log((ASLogLevel_t) 1, "Starting compilation: %u scripts\n", m_uiScriptCount);

	CASModule *builtModule = m_pModuleInfo->CreateModule(this, m_pModule);

	if (module)
		*module = builtModule;

	CASModule *previousModule = CASServerManager::GetInstance()->m_pActiveModule;

	CASServerManager::GetInstance()->m_pActiveModule = builtModule;
	int result = m_pScriptBuilder->BuildModule();
	CASServerManager::GetInstance()->m_pActiveModule = previousModule;
	m_bBuilt = result >= 0;

	if (m_bBuilt)
	{
		m_bBuilt = m_pModuleInfo->Validate(this);

		if (!m_bBuilt)
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Module '%s' failed validation\n", m_pModule->GetName());
	}

	if (!module)
		builtModule->Release();

	return m_bBuilt;
}

int CASModuleBuilder::IncludeCallback(const char *include, const char *from, CScriptBuilder *builder)
{
	static const ASLogLevel_t INCLUDE_LOG_LEVEL = (ASLogLevel_t)2;

	char directory[MAX_PATH + 1];
	char filename[MAX_PATH + 1];
	char path[MAX_PATH];
	int result = -1;

	if (UTIL_ExtractDirectoryFromFilename(from, directory, sizeof(directory), false))
	{
		int length = UTIL_FormatString(filename, sizeof(filename), "%s/%s.as", directory, include);

		if (length >= 0 && (unsigned int)length < sizeof(filename)
			&& UTIL_FindAbstractResourcePath(filename, path, sizeof(path)))
			result = AddScriptFromFile(path);
	}

	if (result < 0)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Failed to add include file '%s'!\n", include);
		m_uiIncludeErrors++;
	}
	else if (!result)
		CASLog::GetInstance()->Log(INCLUDE_LOG_LEVEL, "Included script '%s'\n", include);

	return result;
}

int IncludeCallback(const char *include, const char *from, CScriptBuilder *builder, void *userData)
{
	return ((CASModuleBuilder *)userData)->IncludeCallback(include, from, builder);
}

bool CASModuleBuilder::FormatScriptFilename(const char *filename, char *scriptPath, unsigned int scriptPathSize,
	char *localPath, unsigned int localPathSize)
{
	static const char SCRIPT_EXTENSION[] = ".as";
	static const unsigned int SCRIPT_EXTENSION_LENGTH = sizeof(SCRIPT_EXTENSION) - 1;

	if (!filename)
		return false;

	const char *path = filename;

	if (((IFileSystem_SvEngine *)g_pFileSystem)->FileExistsEx(filename, "GAME"))
	{
		if (!UTIL_CopyString(scriptPath, filename, scriptPathSize))
			return false;
	}
	else
	{
		int length = UTIL_FormatString(scriptPath, scriptPathSize, "%s/%s", m_pModuleInfo->m_pszDirectory, filename);

		if (length < 0 || (unsigned int)length >= scriptPathSize)
			return false;

		unsigned int pathLength = strlen(scriptPath);

		if (pathLength <= SCRIPT_EXTENSION_LENGTH)
			return false;

		if (strncmp(SCRIPT_EXTENSION, scriptPath + pathLength - SCRIPT_EXTENSION_LENGTH, SCRIPT_EXTENSION_LENGTH)
			&& !UTIL_AppendString(scriptPath, SCRIPT_EXTENSION, scriptPathSize))
			return false;

		path = scriptPath;
	}

	return g_pFileSystem->GetLocalPath(path, localPath, localPathSize) != NULL;
}

int CASModuleBuilder::AddScriptFromFile(const char *filename)
{
	char scriptPath[MAX_PATH] = {};
	char localPath[MAX_PATH] = {};

	if (!FormatScriptFilename(filename, scriptPath, sizeof(scriptPath), localPath, sizeof(localPath)))
		return -1;

	int result = m_pScriptBuilder->AddSectionFromFile(localPath);

	if (result == 1)
	{
		m_uiScriptCount++;

		if (m_pModuleInfo->m_bPrecacheScripts)
		{
			UTIL_FixSlashes(scriptPath);
			PRECACHE_GENERIC((char *)STRING(AllocStringLocal(scriptPath)));
		}
	}
	else if (result < 0)
		return -1;

	return result != 1;
}

const char *CASModuleBuilder::GetModuleName(void) const
{
	return m_pModule ? m_pModule->GetName() : "";
}

int CASModuleBuilder::AddScriptFromBuffer(const char *name, const char *source)
{
	if (!name || !*name || !source)
		return -1;

	int result = m_pScriptBuilder->AddSectionFromMemory(name, source);

	if (result < 0)
		return -1;

	return result != 1;
}

void CASModuleBuilder::ReportValidationError(const char *message) const
{
	CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "Error while validating module '%s': %s!\n",
		m_pModule->GetName(), message);
}
