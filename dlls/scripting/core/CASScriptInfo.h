#ifndef CASSCRIPTINFO_H
#define CASSCRIPTINFO_H

#include "server/AdminLevel.h"
#include "scripting/core/CASBaseClass.h"
#include "CString.h"

class CASDocumentation;

class CASScriptInfo : public CASRefCountedBaseClass
{
public:
	CASScriptInfo(const char *initDeclaration);
	~CASScriptInfo();

	void Release(void) const;
	const CString &GetAuthor(void) const;
	void SetAuthor(const CString &author);
	const CString &GetContactInfo(void) const;
	void SetContactInfo(const CString &contactInfo);
	AdminLevel_t GetMinimumAdminLevel(void) const;
	void SetMinimumAdminLevel(AdminLevel_t minimumAdminLevel);

private:
	bool CheckAccess(const char *functionName) const;

	const char		*m_pszInitDeclaration;
	CString			m_Author;
	CString			m_ContactInfo;
	AdminLevel_t	m_MinimumAdminLevel;
};

void RegisterSCScriptPluginInfo(CASDocumentation *documentation);

#endif
