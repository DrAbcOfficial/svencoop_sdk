#include "extdll.h"
#include "CASPersistence.h"
#include "CASPersistenceInstance.h"
#include "CASServerManager.h"
#include "CASDocumentation.h"
#include "CASLog.h"
#include "../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"
#include <angelscript.h>
#include <limits.h>

CASPersistence::CASPersistence()
    : INVALID_ID(0), m_uiNextId(1)
{
}

CASPersistence::~CASPersistence()
{
	Clear();
}

bool CASPersistence::ShouldKeep(void) const
{
	return m_bKeep;
}

void CASPersistence::SetKeep(bool keep)
{
	m_bKeep = keep;
}

bool CASPersistence::KeepIfPrevious(const CString &mapName)
{
	if (m_PreviousMap == mapName)
		m_bKeep = true;

	return m_bKeep;
}

bool CASPersistence::KeepIfPrevious(const CScriptArray *maps)
{
	if (!maps)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASPersistence::KeepIfPrevious: array must be non-null!\n");
		return false;
	}

	unsigned int count = maps->GetSize();
	CASServerManager *manager = CASServerManager::GetInstance();
	asIScriptEngine *engine = manager->m_pEngine;
	asITypeInfo *elementType = engine->GetTypeInfoById(maps->GetElementTypeId());
	asITypeInfo *stringType = manager->GetTypeInfoByName("string");

	if (stringType != elementType)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASPersistence::KeepIfPrevious: array type must be string!\n");
		return false;
	}

	m_bKeep = false;

	for (unsigned int index = 0; index < count; index++)
	{
		if (KeepIfPrevious(*(const CString *) maps->At(index)))
			break;
	}

	maps->Release();
	return m_bKeep;
}

const CString &CASPersistence::GetPreviousMapName(void) const
{
	return m_PreviousMap;
}

const CString &CASPersistence::GetCurrentMapName(void) const
{
	return m_CurrentMap;
}

void CASPersistence::SetCurrentMap(const char *name)
{
	if (!name)
		return;

	m_PreviousMap = m_CurrentMap;
	m_CurrentMap = name;
}

void CASPersistence::PreMapInit(const char *name)
{
	m_bKeep = false;
	SetCurrentMap(name);
}

void CASPersistence::PostMapInit(void)
{
	if (m_PreviousMap == "")
		m_bKeep = true;
	else if (!m_bKeep)
		Clear();
}

bool CASPersistence::Exists(const CString &name) const
{
	return m_InstanceNames.find(name) != m_InstanceNames.end();
}

bool CASPersistence::IsValidHandle(unsigned int id) const
{
	return m_Instances.find(id) != m_Instances.end();
}

unsigned int CASPersistence::RegisterInstance(const CString &name)
{
	if (name == "")
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASPersistence::RegisterInstance: name must not be empty!\n");
		return INVALID_ID;
	}

	InstanceNameMap_t::iterator entry = m_InstanceNames.find(name);

	if (entry != m_InstanceNames.end())
		return entry->second;

	if (m_uiNextId == UINT_MAX)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASPersistence::RegisterInstance: Maximum number of objects reached!\n");
		return INVALID_ID;
	}

	m_InstanceNames.insert(std::make_pair(name, m_uiNextId));
	m_Instances.insert(std::make_pair(m_uiNextId, new CASPersistenceInstance(name)));

	return m_uiNextId++;
}

void CASPersistence::ClearInstance(const CString &name)
{
	InstanceNameMap_t::iterator entry = m_InstanceNames.find(name);

	if (entry != m_InstanceNames.end())
		Clear(entry->second);
}

void CASPersistence::ClearInstance(unsigned int id)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry == m_Instances.end())
		return;

	m_InstanceNames.erase(m_InstanceNames.find(entry->second->m_Name));
	delete entry->second;
	m_Instances.erase(entry);
}

void CASPersistence::Clear(void)
{
	for (InstanceMap_t::iterator entry = m_Instances.begin(); entry != m_Instances.end(); entry++)
	{
		if (entry->second)
			delete entry->second;

		entry->second = NULL;
	}

	m_Instances.clear();
	m_InstanceNames.clear();
	m_uiNextId = 1;
}

const CString &CASPersistence::GetName(unsigned int id) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? entry->second->m_Name : CString::EMPTY_STRING;
}

unsigned int CASPersistence::Size(unsigned int id) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? (unsigned int) entry->second->m_Values.size() : 0;
}

bool CASPersistence::Exists(unsigned int id, const CString &key) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? entry->second->Exists(key) : false;
}

const CString &CASPersistence::GetString(unsigned int id, const CString &key) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? entry->second->GetString(key) : CString::EMPTY_STRING;
}

bool CASPersistence::GetBoolean(unsigned int id, const CString &key) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? entry->second->GetBoolean(key) : false;
}

long CASPersistence::GetLong(unsigned int id, const CString &key, int radix) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? entry->second->GetLong(key, radix) : 0;
}

unsigned long CASPersistence::GetUlong(unsigned int id, const CString &key, int radix) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? entry->second->GetUlong(key, radix) : 0;
}

float CASPersistence::GetFloat(unsigned int id, const CString &key) const
{
	InstanceMap_t::const_iterator entry = m_Instances.find(id);

	return entry != m_Instances.end() ? entry->second->GetFloat(key) : 0.0f;
}

void CASPersistence::Set(unsigned int id, const CString &key, const CString &value)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry != m_Instances.end())
		entry->second->Set(key, value);
}

void CASPersistence::Set(unsigned int id, const CString &key, bool value)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry != m_Instances.end())
		entry->second->Set(key, value);
}

void CASPersistence::Set(unsigned int id, const CString &key, int value)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry != m_Instances.end())
		entry->second->Set(key, value);
}

void CASPersistence::Set(unsigned int id, const CString &key, unsigned int value)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry != m_Instances.end())
		entry->second->Set(key, value);
}

void CASPersistence::Set(unsigned int id, const CString &key, float value)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry != m_Instances.end())
		entry->second->Set(key, value);
}

void CASPersistence::Clear(unsigned int id, const CString &key)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry != m_Instances.end())
		entry->second->Clear(key);
}

void CASPersistence::Clear(unsigned int id)
{
	InstanceMap_t::iterator entry = m_Instances.find(id);

	if (entry != m_Instances.end())
		entry->second->Clear();
}

CASPersistence *Persistence_GetActivePersistence( void )
{
	return CASServerManager::GetInstance()->m_pPersistence;
}

void RegisterSCScriptPersistence(CASDocumentation *documentation)
{
	documentation->RegisterTypedef("Persistence object id type.", "PersistID_t", "uint");
	documentation->RegisterObjectType("Persistence manager", "CPersistence", 0, asOBJ_REF | asOBJ_NOCOUNT);

	documentation->RegisterObjectMethod("Returns whether the given persistence instance exists.",
	                                    "CPersistence", "bool Exists(const string& in szName) const",
	                                    asMETHODPR(CASPersistence, Exists, (const CString &) const, bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether the given persistence instance handle is valid.",
	                                    "CPersistence", "bool IsValidHandle(const PersistID_t ID) const",
	                                    asMETHOD(CASPersistence, IsValidHandle), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether the previous map's persistence instances will be kept.",
	                                    "CPersistence", "bool ShouldKeep() const",
	                                    asMETHOD(CASPersistence, ShouldKeep), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets whether the previous map's persistence instances will be kept.",
	                                    "CPersistence", "void SetKeep(bool fValue)",
	                                    asMETHOD(CASPersistence, SetKeep), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Will make the persistence manager keep the previous map's persistence instancesif the previous map's name matches any of the elements in the array.",
	                                    "CPersistence", "bool KeepIfPrevious(const array<string>@ pArray)",
	                                    asMETHODPR(CASPersistence, KeepIfPrevious, (const CScriptArray *), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Will make the persistence manager keep the previous map's persistence instancesif the previous map's name matches the given map name.",
	                                    "CPersistence", "bool KeepIfPrevious(const string& in szMapName)",
	                                    asMETHODPR(CASPersistence, KeepIfPrevious, (const CString &), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns a persistence instance tied to the given name. Will be created if it does not already exist.",
	                                    "CPersistence", "PersistID_t RegisterInstance(const string& in szName)",
	                                    asMETHOD(CASPersistence, RegisterInstance), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the name of the previous map. Can be empty if the server has just started.",
	                                    "CPersistence", "const string& GetPreviousMapName() const",
	                                    asMETHOD(CASPersistence, GetPreviousMapName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the name of the current map.",
	                                    "CPersistence", "const string& GetCurrentMapName() const",
	                                    asMETHOD(CASPersistence, GetCurrentMapName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clears the specified persistence instance.",
	                                    "CPersistence", "void ClearInstance(const string& in szName)",
	                                    asMETHODPR(CASPersistence, ClearInstance, (const CString &), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clears the specified persistence instance.",
	                                    "CPersistence", "void ClearInstance(const PersistID_t ID)",
	                                    asMETHODPR(CASPersistence, ClearInstance, (unsigned int), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clears all persistence instances.\nMake sure to avoid calling handles referencing the cleared instances.",
	                                    "CPersistence", "void Clear()",
	                                    asMETHODPR(CASPersistence, Clear, (void), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the name of this instance.",
	                                    "CPersistence", "const string& GetName(const PersistID_t ID) const",
	                                    asMETHOD(CASPersistence, GetName), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the number of persisted items in this instance.",
	                                    "CPersistence", "size_t Size(int iId) const",
	                                    asMETHOD(CASPersistence, Size), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether the given key exists in this instance.",
	                                    "CPersistence", "bool Exists(const PersistID_t ID, const string& in szKey)",
	                                    asMETHODPR(CASPersistence, Exists, (unsigned int, const CString &) const, bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the given value as a string, or  if it does not exist.",
	                                    "CPersistence", "const string& GetString(const PersistID_t ID, const string& in szKey)",
	                                    asMETHOD(CASPersistence, GetString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the given value as a boolean, or false if it does not exist.",
	                                    "CPersistence", "bool GetBoolean(const PersistID_t ID, const string& in szKey)",
	                                    asMETHOD(CASPersistence, GetBoolean), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the given value as a long, or 0 if it does not exist.",
	                                    "CPersistence", "int32 GetLong(const PersistID_t ID, const string& in szKey, int iRadix = 10)",
	                                    asMETHOD(CASPersistence, GetLong), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the given value as a unsigned long, or 0 if it does not exist.",
	                                    "CPersistence", "uint32 GetUlong(const PersistID_t ID, const string& in szKey, int iRadix = 10)",
	                                    asMETHOD(CASPersistence, GetUlong), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the given value as a float, or 0.0 if it does not exist.",
	                                    "CPersistence", "float GetFloat(const PersistID_t ID, const string& in szKey)",
	                                    asMETHOD(CASPersistence, GetFloat), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the given value as a string.",
	                                    "CPersistence", "void Set(const PersistID_t ID, const string& in szKey, const string& in szValue)",
	                                    asMETHODPR(CASPersistence, Set, (unsigned int, const CString &, const CString &), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the given value as a boolean.",
	                                    "CPersistence", "void Set(const PersistID_t ID, const string& in szKey, bool fValue)",
	                                    asMETHODPR(CASPersistence, Set, (unsigned int, const CString &, bool), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the given value as a string.",
	                                    "CPersistence", "void Set(const PersistID_t ID, const string& in szKey, int32 iValue)",
	                                    asMETHODPR(CASPersistence, Set, (unsigned int, const CString &, int), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the given value as a string.",
	                                    "CPersistence", "void Set(const PersistID_t ID, const string& in szKey, uint32 uiValue)",
	                                    asMETHODPR(CASPersistence, Set, (unsigned int, const CString &, unsigned int), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets the given value as a string.",
	                                    "CPersistence", "void Set(const PersistID_t ID, const string& in szKey, float flValue)",
	                                    asMETHODPR(CASPersistence, Set, (unsigned int, const CString &, float), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clears the given value.",
	                                    "CPersistence", "void Clear(const PersistID_t ID, const string& in szKey)",
	                                    asMETHODPR(CASPersistence, Clear, (unsigned int, const CString &), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clears all values.",
	                                    "CPersistence", "void Clear(const PersistID_t ID)",
	                                    asMETHODPR(CASPersistence, Clear, (unsigned int), void), asCALL_THISCALL);

	documentation->RegisterObjectProperty("Id used by invalid persistence objects.", "CPersistence",
	                                      "const PersistID_t INVALID_ID", asOFFSET(CASPersistence, INVALID_ID));

	asDWORD accessMask = documentation->SetDefaultAccessMask(AS_ACCESS_MAP);

	documentation->RegisterGlobalFunction("Gets the map script's persistence manager.",
	                                      "CPersistence@ get_g_Persistence()", asFUNCTION( Persistence_GetActivePersistence ), asCALL_CDECL);
	documentation->SetDefaultAccessMask(accessMask);
}
