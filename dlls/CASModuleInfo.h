#ifndef CASMODULEINFO_H
#define CASMODULEINFO_H

#include "CASDirectoryList.h"

class CString;
class CASVirtualFileSystem;
class CASFile;
class CASHeaderList;
class CASModule;
class CASModuleBuilder;
class asIScriptModule;

namespace DatabaseType
{
enum Type : int;
}

class CASModuleInfo
{
public:
	CASModuleInfo(const char *directory, ModuleType::Type type, unsigned char databaseTypes,
		bool precacheScripts, const char *initDeclaration);

	virtual CASHeaderList *GenerateHeaders(void);
	virtual CASModule *CreateModule(CASModuleBuilder *builder, asIScriptModule *module) = 0;
	virtual bool Validate(CASModuleBuilder *builder);
	virtual ~CASModuleInfo();

	static CASModuleInfo *FindModuleInfo(ModuleType::Type type);
	bool SupportsDatabase(DatabaseType::Type type) const;

private:
	friend CASFile *CASVirtualFileSystem_OpenFile(CASVirtualFileSystem *, const CString &, unsigned char);
	friend void CASVirtualFileSystem_RemoveFile(CASVirtualFileSystem *, const CString &);

	friend class CASEngineReflectionGroup;
	friend class CASModuleReflectionGroup;

	friend class CASConCommandSystem;
	friend class CASPlayerFuncs;
	friend class CASEntityLoader;

	friend class CASBaseManager;
	friend class CASModule;
	friend class CASModuleBuilder;
	friend class CASModuleHookManager;

	static CASModuleInfo	*m_pHead;

	CASModuleInfo	*m_pNext;
	const char		*m_pszDirectory;
	ModuleType::Type	m_ModuleType;
	unsigned char	m_uiDatabaseTypes;
	bool			m_bPrecacheScripts;
	const char		*m_pszInitDeclaration;
};

#endif
