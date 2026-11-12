#ifndef CASDIRECTORYLIST_H
#define CASDIRECTORYLIST_H

#include "scripting/core/CASDirectory.h"
#include <stddef.h>

class CASFileQuota;

namespace ModuleType
{
	enum Type
	{
		UNKNOWN = -1,
		MAP,
		PLUGIN
	};
}

namespace FileAccessBit
{
	enum Type
	{
		NONE = 0,
		READ = 1 << 0,
		WRITE = 1 << 1
	};
}

namespace DirectoryFlagBit
{
	enum
	{
		NONE = 0,
		IMPLICIT_SUBDIRECTORIES = 1 << 0
	};
}

namespace TempDirFlagBit
{
	enum Type
	{
		NONE = 0,
		STARTUP = 1 << 0,
		MAP_CHANGE = 1 << 1,
		SHUTDOWN = 1 << 2
	};
}

class CASDirectoryList
{
public:
	CASDirectoryList();
	~CASDirectoryList();

	CASDirectory *FindDirectory(const char *path, bool nearestParent = false, bool *partial = NULL);
	const CASDirectory *FindDirectory(const char *path, bool nearestParent = false, bool *partial = NULL) const;
	CASDirectory *CreateDirectory(const char *path, unsigned char moduleTypes, unsigned char accessFlags,
		unsigned char directoryFlags, unsigned char tempFlags);
	void RemoveDirectory(CASDirectory *directory);
	void RemoveDirectory(const char *path);
	void RemoveAllDirectories(void);
	bool CanAccessDirectory(const CASDirectory *directory, ModuleType::Type moduleType,
		FileAccessBit::Type access) const;
	bool CanAccessDirectory(const char *path, ModuleType::Type moduleType, FileAccessBit::Type access,
		const CASDirectory **directory = NULL) const;
	void CreateWritableDirectories(void);
	void ClearTempDirectories(TempDirFlagBit::Type flags, CASFileQuota *quota);

private:
	friend class CWriteFileSystemToFile;

	CASDirectory	*m_pRoot;
};

#endif
