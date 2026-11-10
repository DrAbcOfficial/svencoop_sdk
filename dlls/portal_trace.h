#ifndef SERVER_PORTAL_TRACE_H
#define SERVER_PORTAL_TRACE_H

#include <vector>

class CPortal;
class CBasePlayer;

struct PortalTraceStep
{
	int      m_iStep;
	Vector   m_vecStart;
	Vector   m_vecEnd;
	Vector   m_vecDirection;
	Vector   m_vecPlaneNormal;
	float    m_flDistance;
	float    m_flFraction;
	float    m_flTotalFraction;
	float    m_flScale;
	edict_t *m_pPortal;
};

struct PortalTraceResult : public TraceResult
{
	~PortalTraceResult();

	std::vector<PortalTraceStep *> m_Steps;
	Vector                        m_vecDirection;
	float                         m_flScale;
};

struct PortalSurfaceTrace
{
	bool     m_bHit;
	CPortal *m_pPortal;
	float    m_flScale;
	Vector   m_vecPosition;
	Vector   m_vecNormal;
};

typedef bool ( *PortalTraceCallback )( PortalTraceStep *step );

void P_TraceSurface( edict_t *entity, CPortal *portal, const Vector &start, const Vector &direction,
                     float &distance, PortalSurfaceTrace &trace );
void P_TracePortal( const Vector &start, const Vector &direction, float distance,
                    IGNORE_MONSTERS ignoreMonsters, IGNORE_GLASS ignoreGlass, edict_t *ignore,
                    CBasePlayer *player, PortalTraceResult *result, PortalTraceCallback callback = NULL );
void UTIL_TracePortal( const Vector &start, const Vector &direction, float distance,
                       IGNORE_MONSTERS ignoreMonsters, IGNORE_GLASS ignoreGlass, edict_t *ignore,
                       CBasePlayer *player, PortalTraceResult *result, PortalTraceCallback callback = NULL );

#endif
