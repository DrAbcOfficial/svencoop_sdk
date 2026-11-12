#include "scripting/core/CASScriptInfo.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASFunction.h"
#include "scripting/core/CASLog.h"
#include "scripting/core/CASDocumentation.h"

CASScriptInfo::CASScriptInfo(const char *initDeclaration)
	: m_pszInitDeclaration(initDeclaration), m_MinimumAdminLevel(ADMIN_NO)
{
}

CASScriptInfo::~CASScriptInfo()
{
}

void CASScriptInfo::Release(void) const
{
	if (InternalRelease())
		delete this;
}

const CString &CASScriptInfo::GetAuthor(void) const
{
	return m_Author;
}

void CASScriptInfo::SetAuthor(const CString &author)
{
	if (CheckAccess("SetAuthor"))
		m_Author = author;
}

const CString &CASScriptInfo::GetContactInfo(void) const
{
	return m_ContactInfo;
}

void CASScriptInfo::SetContactInfo(const CString &contactInfo)
{
	if (CheckAccess("SetContactInfo"))
		m_ContactInfo = contactInfo;
}

AdminLevel_t CASScriptInfo::GetMinimumAdminLevel(void) const
{
	return m_MinimumAdminLevel;
}

void CASScriptInfo::SetMinimumAdminLevel(AdminLevel_t minimumAdminLevel)
{
	if (m_MinimumAdminLevel < minimumAdminLevel)
		m_MinimumAdminLevel = minimumAdminLevel;
}

bool CASScriptInfo::CheckAccess(const char *functionName) const
{
	CASModule *module = CASServerManager::GetInstance()->m_pActiveModule;

	if (!module)
		return true;

	asIScriptContext *context = asGetActiveContext();

	if (!context)
		return true;

	CASFunction *initialization = module->GetFunction(m_pszInitDeclaration);

	if (!initialization)
		return true;

	asIScriptFunction *function = context->GetFunction();

	if (function == initialization->GetFunction())
		return true;

	CASLog *log = CASLog::GetInstance();
	int line = context->GetLineNumber();
	const char *sectionName = function->GetScriptSectionName();

	log->Error(AS_LOG_DEFAULT,
		"CScriptInfo::%s: You cannot set the author outside of '%s'!\nScript '%s', line %d\n",
		functionName, m_pszInitDeclaration, sectionName, line);
	return false;
}

void RegisterSCScriptPluginInfo(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Script info object.\nContains script information.",
		"CScriptInfo", sizeof(CASScriptInfo), asOBJ_REF);
	documentation->RegisterObjectBehaviour("AddRef", "CScriptInfo", asBEHAVE_ADDREF,
		"void AddRef()", asMETHOD(CASScriptInfo, AddRef), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Release", "CScriptInfo", asBEHAVE_RELEASE,
		"void Release()", asMETHOD(CASScriptInfo, Release), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the author name.", "CScriptInfo",
		"const string& GetAuthor() const", asMETHOD(CASScriptInfo, GetAuthor), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the author name.", "CScriptInfo",
		"void SetAuthor(const string& in szAuthor)", asMETHOD(CASScriptInfo, SetAuthor), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the author's contact info.", "CScriptInfo",
		"const string& GetContactInfo() const", asMETHOD(CASScriptInfo, GetContactInfo), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the author's contact info.", "CScriptInfo",
		"void SetContactInfo(const string& in szContactInfo)", asMETHOD(CASScriptInfo, SetContactInfo), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the minimum admin level required for commands.", "CScriptInfo",
		"AdminLevel_t GetMinimumAdminLevel() const", asMETHOD(CASScriptInfo, GetMinimumAdminLevel), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the minimum admin level required for commands. You cannot lower the minimum level.",
		"CScriptInfo", "void SetMinimumAdminLevel(const AdminLevel_t minimumAdminLevel)",
		asMETHOD(CASScriptInfo, SetMinimumAdminLevel), asCALL_THISCALL);
}
