#ifndef CASPERSISTENCEINSTANCE_H
#define CASPERSISTENCEINSTANCE_H

#include "CASBaseClass.h"
#include "CASFileQuota.h"
#include "CString.h"
#include <map>

class CASPersistenceInstance : public CASBaseClass
{
public:
	CASPersistenceInstance(const CString &name);
	~CASPersistenceInstance();

	bool Exists(const CString &key) const;
	const CString &GetString(const CString &key) const;
	bool GetBoolean(const CString &key) const;
	long GetLong(const CString &key, int radix) const;
	unsigned long GetUlong(const CString &key, int radix) const;
	float GetFloat(const CString &key) const;
	void Set(const CString &key, const CString &value);
	void Set(const CString &key, bool value);
	void Set(const CString &key, int value);
	void Set(const CString &key, unsigned int value);
	void Set(const CString &key, float value);
	void Clear(const CString &key);
	void Clear(void);

private:
	friend class CASPersistence;

	typedef std::map<CString, CString, CStringLess> ValueMap_t;

	ValueMap_t::const_iterator Find(const CString &key) const;

	CString		m_Name;
	ValueMap_t	m_Values;
};

#endif
