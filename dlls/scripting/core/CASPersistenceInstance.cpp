#include "scripting/core/CASPersistenceInstance.h"
#include <stdlib.h>

CASPersistenceInstance::CASPersistenceInstance(const CString &name)
    : m_Name(name)
{
}

CASPersistenceInstance::~CASPersistenceInstance()
{
}

CASPersistenceInstance::ValueMap_t::const_iterator CASPersistenceInstance::Find(const CString &key) const
{
	return m_Values.find(key);
}

bool CASPersistenceInstance::Exists(const CString &key) const
{
	return Find(key) != m_Values.end();
}

const CString &CASPersistenceInstance::GetString(const CString &key) const
{
	ValueMap_t::const_iterator entry = Find(key);

	return entry != m_Values.end() ? entry->second : CString::EMPTY_STRING;
}

bool CASPersistenceInstance::GetBoolean(const CString &key) const
{
	ValueMap_t::const_iterator entry = Find(key);

	return entry != m_Values.end() && entry->second.ICompare("1") == 0;
}

long CASPersistenceInstance::GetLong(const CString &key, int radix) const
{
	ValueMap_t::const_iterator entry = Find(key);

	return entry != m_Values.end() ? strtol(entry->second.Data(), NULL, radix) : 0;
}

unsigned long CASPersistenceInstance::GetUlong(const CString &key, int radix) const
{
	ValueMap_t::const_iterator entry = Find(key);

	return entry != m_Values.end() ? strtoul(entry->second.Data(), NULL, radix) : 0;
}

float CASPersistenceInstance::GetFloat(const CString &key) const
{
	ValueMap_t::const_iterator entry = Find(key);

	return entry != m_Values.end() ? (float) strtod(entry->second.Data(), NULL) : 0.0f;
}

void CASPersistenceInstance::Set(const CString &key, const CString &value)
{
	m_Values[key] = value;
}

void CASPersistenceInstance::Set(const CString &key, bool value)
{
	Set(key, CString(value));
}

void CASPersistenceInstance::Set(const CString &key, int value)
{
	Set(key, CString(value));
}

void CASPersistenceInstance::Set(const CString &key, unsigned int value)
{
	Set(key, CString(value));
}

void CASPersistenceInstance::Set(const CString &key, float value)
{
	Set(key, CString(value));
}

void CASPersistenceInstance::Clear(const CString &key)
{
	ValueMap_t::iterator entry = m_Values.find(key);

	if (entry != m_Values.end())
		m_Values.erase(entry);
}

void CASPersistenceInstance::Clear(void)
{
	m_Values.clear();
}
