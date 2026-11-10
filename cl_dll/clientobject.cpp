#include "hud.h"
#include "cl_util.h"
#include "clientobject.h"
#include "cl_entity.h"
#include "event_api.h"
#include "pm_defs.h"
#include "client_sound.h"

CObjectHandler g_pObjectHandler;

CClientObject::CClientObject() :
	m_vecOrigin(0, 0, 0),
	m_vecUnknown32(0, 0, 0),
	m_vecUnknown44(0, 0, 0),
	m_vecColor(0, 0, 0)
{
}

void CClientObject::Think(void)
{
	if (m_flNextThink > 0 && m_flNextThink <= Time() && m_pfnThink)
	{
		m_flNextThink = 0;
		(this->*m_pfnThink)();
	}
}

void CClientObject::Remove(void)
{
}

void CClientObject::Kill(void)
{
	m_bKill = true;
	SetThink(NULL);
}

void CClientObject::Spawn(void)
{
	m_pszClassname = "client_object";
}

void CClientObject::SetThink(void (CClientObject::*pfnThink)(void))
{
	m_pfnThink = pfnThink;
	m_flNextThink = Time();
	m_bKill = false;
}

int CClientObject::LookupModel(const char *pszModel)
{
	return gEngfuncs.pEventAPI->EV_FindModelIndex(pszModel);
}

float CClientObject::Time(void)
{
	return gEngfuncs.GetClientTime();
}

Vector CClientObject::LocalPlayerOrigin(void)
{
	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

	if (pPlayer)
		return pPlayer->origin;

	return Vector(0, 0, 0);
}

Vector CClientObject::LocalPlayerViewAngles(void)
{
	Vector vecAngles(0, 0, 0);

	gEngfuncs.GetViewAngles(vecAngles);
	return vecAngles;
}

int CClientObject::LocalPlayerIndex(void)
{
	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

	return pPlayer ? pPlayer->index : 0;
}

void CClientObject::PlaySound(const char *pszSample, float flVolume, Vector vecOrigin, int iChannel,
	int iPitch, float flAttenuation, int iFlags, int iEntity)
{
	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, iEntity, vecOrigin, iChannel,
		pszSample, flVolume, flAttenuation, iFlags, iPitch, -1, 0);
}

void CClientObject::StopSound(int iChannel, const char *pszSample, int iEntity)
{
	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(SOUNDMSG_STOP, iEntity, Vector(0, 0, 0), iChannel,
		pszSample, 0, 0, 0, 0, -1, 0);
}

void CClientObject::TraceLine(Vector vecStart, Vector vecEnd, int iWorldOnly, int iIgnoreEntity, pmtrace_s *pTrace)
{
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);

	int iIgnore = -1;

	if (iIgnoreEntity >= 0)
	{
		if (gEngfuncs.pEventAPI->EV_GetPhysent(iIgnoreEntity))
			iIgnore = gEngfuncs.pEventAPI->EV_GetPhysent(iIgnoreEntity)->info;
		else
			iIgnore = iIgnoreEntity;
	}

	gEngfuncs.pEventAPI->EV_PlayerTrace(vecStart, vecEnd,
		PM_STUDIO_BOX | (iWorldOnly ? PM_WORLD_ONLY : 0), iIgnore, pTrace);
}

CObjectHandler::~CObjectHandler()
{
	CleanList();
}

void CObjectHandler::CleanList(void)
{
	CClientObject *pObject = m_Head.m_pNext;

	while (pObject)
	{
		CClientObject *pNext = pObject->m_pNext;

		pObject->Remove();
		delete pObject;
		pObject = pNext;
	}

	m_Head.m_pNext = NULL;
}

void CObjectHandler::Think(void)
{
	m_flTime = gEngfuncs.GetClientTime();
	m_flFrameTime = m_flTime - m_flLastTime;

	CClientObject *pPrevious = &m_Head;
	CClientObject *pObject = m_Head.m_pNext;

	while (pObject)
	{
		CClientObject *pNext = pObject->m_pNext;

		if (pObject->m_bKill)
		{
			pPrevious->m_pNext = pNext;
			pObject->Remove();
			delete pObject;
		}
		else
		{
			pObject->Think();
			pPrevious = pObject;
		}

		pObject = pNext;
	}

	m_flLastTime = m_flTime;
}

bool CObjectHandler::operator<<(CClientObject *pObject)
{
	CClientObject *pLast = &m_Head;

	while (pLast->m_pNext)
		pLast = pLast->m_pNext;

	pLast->m_pNext = pObject;
	pObject->m_pNext = NULL;
	pObject->Spawn();
	return true;
}

CClientObject *CObjectHandler::FindEntityByClassname(CClientObject *pStart, const char *pszClassname)
{
	CClientObject *pObject = pStart ? pStart->m_pNext : m_Head.m_pNext;

	if (!pszClassname)
		return NULL;

	while (pObject)
	{
		if (pObject->m_pszClassname && !stricmp(pObject->m_pszClassname, pszClassname))
			return pObject;

		pObject = pObject->m_pNext;
	}

	return NULL;
}

CClientObject *CObjectHandler::GetFirstObject(void)
{
	return m_Head.m_pNext;
}

void ClientObject_Think(void)
{
	g_pObjectHandler.Think();
}
