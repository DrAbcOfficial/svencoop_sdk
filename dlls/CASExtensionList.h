#ifndef CASEXTENSIONLIST_H
#define CASEXTENSIONLIST_H

#include "CString.h"
#include <set>

struct CStringCaseInsensitiveLess
{
	bool operator()(const CString &first, const CString &second) const;
};

class CASExtensionList
{
public:
	CASExtensionList();
	~CASExtensionList();

	bool HasExtension(const char *extension, bool fileName) const;
	bool AddExtension(const char *extension);
	void RemoveExtension(const char *extension);
	void RemoveAllExtensions(void);

private:
	friend class CWriteFileSystemToFile;

	std::set<CString, CStringCaseInsensitiveLess>	m_Extensions;
};

#endif
