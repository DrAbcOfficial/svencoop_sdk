#include "extdll.h"
#include "enginecallback.h"
#include "utils/CKeyValueBuffer.h"

CKeyValueBuffer::CKeyValueBuffer(edict_s *client)
	: m_pClient(client)
{
}

CKeyValueBuffer::~CKeyValueBuffer()
{
}

void CKeyValueBuffer::Release(void) const
{
	if (InternalRelease())
		delete this;
}

edict_s *CKeyValueBuffer::GetClient(void) const
{
	return m_pClient;
}

CInfoKeyValueBuffer::CInfoKeyValueBuffer(edict_s *client, char *buffer)
	: CKeyValueBuffer(client), m_pszBuffer(buffer)
{
}

CInfoKeyValueBuffer::~CInfoKeyValueBuffer()
{
}

CString CInfoKeyValueBuffer::GetValue(const CString &key) const
{
	return CString(g_engfuncs.pfnInfoKeyValue(m_pszBuffer, (char *)key.Data()));
}

void CInfoKeyValueBuffer::SetValue(const CString &key, const CString &value)
{
	g_engfuncs.pfnSetClientKeyValue(g_engfuncs.pfnIndexOfEdict(GetClient()),
		m_pszBuffer, (char *)key.Data(), (char *)value.Data());
}

void CInfoKeyValueBuffer::RemoveValue(const CString &key)
{
	g_engfuncs.pfnInfo_RemoveKey(m_pszBuffer, key.Data());
}

CPhysicsKeyValueBuffer::CPhysicsKeyValueBuffer(edict_s *client)
	: CKeyValueBuffer(client)
{
}

CPhysicsKeyValueBuffer::~CPhysicsKeyValueBuffer()
{
}

CString CPhysicsKeyValueBuffer::GetValue(const CString &key) const
{
	return CString(g_engfuncs.pfnGetPhysicsKeyValue(GetClient(), key.Data()));
}

void CPhysicsKeyValueBuffer::SetValue(const CString &key, const CString &value)
{
	g_engfuncs.pfnSetPhysicsKeyValue(GetClient(), key.Data(), value.Data());
}

void CPhysicsKeyValueBuffer::RemoveValue(const CString &key)
{
	g_engfuncs.pfnSetPhysicsKeyValue(GetClient(), key.Data(), "");
}
