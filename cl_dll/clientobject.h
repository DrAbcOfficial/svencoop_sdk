#ifndef CLIENTOBJECT_H
#define CLIENTOBJECT_H

#if !defined(DID_VEC3_T_DEFINE) && !defined(VECTOR_H)
#include "util_vector.h"
#endif

struct pmtrace_s;

class CClientObject
{
public:
	CClientObject();

	virtual void Think(void);
	virtual void Remove(void);
	virtual void Kill(void);
	virtual void Spawn(void);

	void SetThink(void (CClientObject::*pfnThink)(void));
	int LookupModel(const char *pszModel);
	float Time(void);
	Vector LocalPlayerOrigin(void);
	Vector LocalPlayerViewAngles(void);
	int LocalPlayerIndex(void);
	static void PlaySound(const char *pszSample, float flVolume, Vector vecOrigin, int iChannel,
		int iPitch, float flAttenuation, int iFlags, int iEntity);
	static void StopSound(int iChannel, const char *pszSample, int iEntity);
	static void TraceLine(Vector vecStart, Vector vecEnd, int iWorldOnly, int iIgnoreEntity, pmtrace_s *pTrace);

	void (CClientObject::*m_pfnThink)(void);
	float			m_flNextThink;
	CClientObject	*m_pNext;
	bool			m_bKill;
	Vector			m_vecOrigin;
	Vector			m_vecUnknown32;
	Vector			m_vecUnknown44;
	const char		*m_pszClassname;
	unsigned int	m_iUnknown60;
	int				m_iModelIndex;
	int				m_iAlpha;
	unsigned int	m_iUnknown72;
	unsigned int	m_iUnknown76;
	Vector			m_vecColor;
	unsigned int	m_iUnknown92;
};

class CObjectHandler
{
public:
	~CObjectHandler();

	void CleanList(void);
	void Think(void);
	bool operator<<(CClientObject *pObject);
	CClientObject *FindEntityByClassname(CClientObject *pStart, const char *pszClassname);
	CClientObject *GetFirstObject(void);

	float			m_flLastTime;
	CClientObject	m_Head;
	float			m_flFrameTime;
	float			m_flTime;
};

extern CObjectHandler g_pObjectHandler;

void ClientObject_Think(void);

#endif
