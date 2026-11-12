#ifndef CASDIRECTORY_H
#define CASDIRECTORY_H

class CASDirectoryList;

class CASDirectory
{
public:
	enum { MAX_NAME_LENGTH = 260 + 1 };

	CASDirectory(const char *name, unsigned char moduleTypes, unsigned char accessFlags,
		unsigned char directoryFlags, unsigned char tempFlags);
	~CASDirectory();

	bool HasChild(const CASDirectory *child) const;
	CASDirectory *FindChild(const char *name);
	const CASDirectory *FindChild(const char *name) const;
	bool AddChild(CASDirectory *child);
	void RemoveChild(CASDirectory *child);

private:
	friend class CASDirectoryList;
	friend class CWriteFileSystemToFile;
	friend class CCreateWritableDirectories;
	friend class CClearTempDirectories;

	template <class Operation>
	friend bool EnumerateDirectories( Operation &, unsigned int &, const CASDirectory *,
	                                  const char *, char *, unsigned int );

	CASDirectory	*m_pParent;
	CASDirectory	*m_pFirstChild;
	CASDirectory	*m_pNextSibling;
	char			m_szName[MAX_NAME_LENGTH];
	unsigned char	m_uiModuleTypes;
	unsigned char	m_uiAccessFlags;
	unsigned char	m_uiDirectoryFlags;
	unsigned char	m_uiTempFlags;
};

#endif
