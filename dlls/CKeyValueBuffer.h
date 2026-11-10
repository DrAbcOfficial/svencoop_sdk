#ifndef CKEYVALUEBUFFER_H
#define CKEYVALUEBUFFER_H

#include "CASBaseClass.h"
#include "CString.h"

struct edict_s;

class CKeyValueBuffer : public CASRefCountedBaseClass
{
public:
	CKeyValueBuffer(edict_s *client);
	virtual ~CKeyValueBuffer();

	void Release(void) const;
	edict_s *GetClient(void) const;

	virtual CString GetValue(const CString &key) const = 0;
	virtual void SetValue(const CString &key, const CString &value) = 0;
	virtual void RemoveValue(const CString &key) = 0;

private:
	edict_s *m_pClient;
};

class CInfoKeyValueBuffer : public CKeyValueBuffer
{
public:
	CInfoKeyValueBuffer(edict_s *client, char *buffer);
	virtual ~CInfoKeyValueBuffer();

	virtual CString GetValue(const CString &key) const;
	virtual void SetValue(const CString &key, const CString &value);
	virtual void RemoveValue(const CString &key);

private:
	char *m_pszBuffer;
};

class CPhysicsKeyValueBuffer : public CKeyValueBuffer
{
public:
	CPhysicsKeyValueBuffer(edict_s *client);
	virtual ~CPhysicsKeyValueBuffer();

	virtual CString GetValue(const CString &key) const;
	virtual void SetValue(const CString &key, const CString &value);
	virtual void RemoveValue(const CString &key);
};

#endif
