#include "extdll.h"
#include "server/server_support.h"
#include "scripting/core/CASDirectoryList.h"
#include "scripting/api/ASDirectoryEnumeration.h"
#include "scripting/core/CASFileQuota.h"
#include "scripting/core/CASFileSystemUtils.h"
#include "scripting/core/CASLog.h"

void FixDirectorySlashes(char *path)
{
	for (; *path; path++)
	{
		if (*path == '\\')
			*path = '/';
	}
}

void TrimDirectorySpaces(char *path)
{
	unsigned int length = strlen(path);

	while (length > 1 && path[length - 1] == ' ')
		path[--length] = 0;

	char *start = path;

	while (length > 1 && *start == ' ')
	{
		start++;
		length--;
	}

	if (start != path)
		memmove(path, start, length + 1);
}

CASDirectoryList::CASDirectoryList()
{
	m_pRoot = new CASDirectory(".", 0, 0, 0, 0);
}

CASDirectoryList::~CASDirectoryList()
{
	RemoveAllDirectories();
}

CASDirectory *CASDirectoryList::FindDirectory(const char *path, bool nearestParent, bool *partial)
{
	if (partial)
		*partial = false;

	if (!path || !*path)
		return NULL;

	char buffer[CASDirectory::MAX_NAME_LENGTH];

	if (!UTIL_CopyString(buffer, path, sizeof(buffer)))
		return NULL;

	FixDirectorySlashes(buffer);

	CASDirectory *pDirectory = m_pRoot;
	char *component = buffer;
	char *end = buffer + strlen(buffer);

	while (*component)
	{
		char *separator = strchr(component, '/');

		if (!separator)
			separator = end;

		*separator = 0;

		CASDirectory *pChild = pDirectory->FindChild(component);

		if (!pChild)
		{
			if (partial)
				*partial = true;

			return nearestParent ? pDirectory : NULL;
		}

		pDirectory = pChild;

		if (separator == end)
			return pDirectory;

		component = separator + 1;
	}

	return pDirectory;
}

const CASDirectory *CASDirectoryList::FindDirectory(const char *path, bool nearestParent, bool *partial) const
{
	return const_cast<CASDirectoryList *>(this)->FindDirectory(path, nearestParent, partial);
}

CASDirectory *CASDirectoryList::CreateDirectory(const char *path, unsigned char moduleTypes, unsigned char accessFlags,
	unsigned char directoryFlags, unsigned char tempFlags)
{
	CASDirectory *pDirectory = FindDirectory(path);

	if (pDirectory)
	{
		CASLog::GetInstance()->Warning(AS_LOG_DEFAULT,
			"CASDirectoryList::CreateDirectory: Directory '%s' already exists!\n", path);
		return pDirectory;
	}

	if (!path)
		return NULL;

	unsigned int length = strlen(path);
	char *buffer = new char[length + 1];

	UTIL_CopyString(buffer, path, length + 1);
	TrimDirectorySpaces(buffer);

	if (!*buffer)
	{
		delete[] buffer;
		return NULL;
	}

	FixDirectorySlashes(buffer);

	CASDirectory *pParent = m_pRoot;
	char *name = buffer;
	char *separator = strrchr(buffer, '/');

	if (separator)
	{
		bool partial = false;

		*separator = 0;
		pParent = FindDirectory(buffer, false, &partial);
		*separator = '/';

		if (partial)
		{
			CASLog::GetInstance()->Warning(AS_LOG_DEFAULT,
				"CASDirectoryList::CreateDirectory: Could not find a suitable parent for '%s'!\n", buffer);
			delete[] buffer;
			return NULL;
		}

		if (!pParent)
			pParent = m_pRoot;

		name = separator + 1;
	}

	pDirectory = new CASDirectory(name, moduleTypes, accessFlags, directoryFlags, tempFlags);

	if (!*pDirectory->m_szName)
	{
		delete pDirectory;
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
			"CASDirectoryList::CreateDirectory: Name too long for directory '%s'!\n", buffer);
		delete[] buffer;
		return NULL;
	}

	pParent->AddChild(pDirectory);
	delete[] buffer;
	return pDirectory;
}

void CASDirectoryList::RemoveDirectory(CASDirectory *directory)
{
	CASDirectory *pChild = directory->m_pFirstChild;

	while (pChild)
	{
		CASDirectory *pNext = pChild->m_pNextSibling;

		RemoveDirectory(pChild);
		pChild = pNext;
	}

	delete directory;
}

void CASDirectoryList::RemoveDirectory(const char *path)
{
	if (!path || !*path)
		return;

	CASDirectory *pDirectory = FindDirectory(path);

	if (!pDirectory)
		return;

	if (pDirectory->m_pParent)
		pDirectory->m_pParent->RemoveChild(pDirectory);

	RemoveDirectory(pDirectory);
}

void CASDirectoryList::RemoveAllDirectories(void)
{
	RemoveDirectory(m_pRoot);
	m_pRoot = NULL;
}

bool CASDirectoryList::CanAccessDirectory(const CASDirectory *directory, ModuleType::Type moduleType,
	FileAccessBit::Type access) const
{
	return directory && moduleType != ModuleType::UNKNOWN && access != FileAccessBit::NONE
		&& ((unsigned char)(1 << moduleType) & directory->m_uiModuleTypes)
		&& (access & directory->m_uiAccessFlags);
}

bool CASDirectoryList::CanAccessDirectory(const char *path, ModuleType::Type moduleType, FileAccessBit::Type access,
	const CASDirectory **directory) const
{
	if (directory)
		*directory = NULL;

	if (!path || !*path || strstr(path, "..")
		|| moduleType == ModuleType::UNKNOWN || access == FileAccessBit::NONE)
		return false;

	bool partial = false;
	const CASDirectory *pDirectory = FindDirectory(path, true, &partial);

	if (!pDirectory)
		return false;

	if (directory)
		*directory = pDirectory;

	if (partial && !(pDirectory->m_uiDirectoryFlags & DirectoryFlagBit::IMPLICIT_SUBDIRECTORIES))
		return false;

	return CanAccessDirectory(pDirectory, moduleType, access);
}

class CCreateWritableDirectories
{
public:
	void operator()( unsigned int, const CASDirectory *directory, const char *path )
	{
		if ( directory->m_uiAccessFlags & FileAccessBit::WRITE )
			CASFileSystemUtils::CreateDirectory( path );
	}
};

class CClearTempDirectories
{
public:
	CClearTempDirectories( TempDirFlagBit::Type flags, CASFileQuota *quota ) : m_Flags( flags ), m_pQuota( quota ) {}

	void operator()( unsigned int, const CASDirectory *directory, const char *path )
	{
		if ( directory->m_uiTempFlags & m_Flags )
		{
			CASLog::GetInstance()->Log( (ASLogLevel_t) DIRECTORY_LOG_LEVEL,
			                            "Clearing temporary directory '%s'\n", path );
			m_pQuota->RemoveFilesInDirectory( path );
		}
	}

private:
	enum
	{
		DIRECTORY_LOG_LEVEL = 2
	};

	TempDirFlagBit::Type	m_Flags;
	CASFileQuota			*m_pQuota;
};

void CASDirectoryList::CreateWritableDirectories( void )
{
	CCreateWritableDirectories operation;

	EnumerateDirectories( operation, m_pRoot );
}

void CASDirectoryList::ClearTempDirectories( TempDirFlagBit::Type flags, CASFileQuota *quota )
{
	if ( !flags || !quota )
		return;

	CClearTempDirectories operation( flags, quota );

	EnumerateDirectories( operation, m_pRoot );
}
