#ifndef CASPERSISTENCE_H
#define CASPERSISTENCE_H

#include "scripting/core/CASBaseClass.h"
#include "scripting/core/CASFileQuota.h"
#include "CString.h"
#include <map>

class CASDocumentation;
class CASPersistenceInstance;
class CScriptArray;

class CASPersistence : public CASBaseClass
{
public:
	CASPersistence();
	~CASPersistence();

	bool ShouldKeep(void) const;
	void SetKeep(bool keep);
	bool KeepIfPrevious(const CString &mapName);
	bool KeepIfPrevious(const CScriptArray *maps);
	const CString &GetPreviousMapName(void) const;
	const CString &GetCurrentMapName(void) const;
	void SetCurrentMap(const char *name);
	void PreMapInit(const char *name);
	void PostMapInit(void);

	bool Exists(const CString &name) const;
	bool IsValidHandle(unsigned int id) const;
	unsigned int RegisterInstance(const CString &name);
	void ClearInstance(const CString &name);
	void ClearInstance(unsigned int id);
	void Clear(void);

	const CString &GetName(unsigned int id) const;
	unsigned int Size(unsigned int id) const;
	bool Exists(unsigned int id, const CString &key) const;
	const CString &GetString(unsigned int id, const CString &key) const;
	bool GetBoolean(unsigned int id, const CString &key) const;
	long GetLong(unsigned int id, const CString &key, int radix) const;
	unsigned long GetUlong(unsigned int id, const CString &key, int radix) const;
	float GetFloat(unsigned int id, const CString &key) const;
	void Set(unsigned int id, const CString &key, const CString &value);
	void Set(unsigned int id, const CString &key, bool value);
	void Set(unsigned int id, const CString &key, int value);
	void Set(unsigned int id, const CString &key, unsigned int value);
	void Set(unsigned int id, const CString &key, float value);
	void Clear(unsigned int id, const CString &key);
	void Clear(unsigned int id);

	const unsigned int INVALID_ID;

private:
	typedef std::map<CString, unsigned int, CStringLess> InstanceNameMap_t;
	typedef std::map<unsigned int, CASPersistenceInstance *> InstanceMap_t;

	InstanceNameMap_t	m_InstanceNames;
	InstanceMap_t		m_Instances;

	CString				m_PreviousMap;
	CString				m_CurrentMap;

	bool				m_bKeep;
	unsigned int		m_uiNextId;
};

void RegisterSCScriptPersistence(CASDocumentation *documentation);

#endif
