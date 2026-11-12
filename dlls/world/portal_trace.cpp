#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "world/portal.h"
#include "world/portal_trace.h"

PortalTraceResult::~PortalTraceResult()
{
	for ( unsigned int i = 0; i < m_Steps.size(); i++ )
		delete m_Steps[i];
}

void P_TracePortal( const Vector &start, const Vector &direction, float distance,
                    IGNORE_MONSTERS ignoreMonsters, IGNORE_GLASS ignoreGlass, edict_t *ignore,
                    CBasePlayer *player, PortalTraceResult *result, PortalTraceCallback callback )
{
	Vector traceStart = start;
	Vector traceDirection = direction.Normalize();
	float remainingDistance = distance;
	float totalFraction = 0;
	float scale = 1;
	int stepIndex = 0;
	const PortalSet &portals = player ? player->GetVisiblePortals() : globalPortals;
	bool portalHit;

	do
	{
		TraceResult trace = {};
		Vector end = traceStart + traceDirection * remainingDistance;

		UTIL_TraceLine( traceStart, end, ignoreMonsters, ignoreGlass,
		                stepIndex == 0 ? ignore : NULL, &trace );

		float hitDistance = ( trace.vecEndPos - traceStart ).Length();
		PortalSurfaceTrace portalTrace = {};

		portalTrace.m_flScale = 1;

		for ( PortalSet::const_iterator current = portals.begin(); current != portals.end(); current++ )
		{
			EntityHandle<CPortal> handle = *current;
			CPortal *portal = handle;

			if ( portal && portal->IsActive() && portal->GetPortalMode() == PORTAL_PORTAL &&
			     portal->IsShootThroughEnabled() )
			{
				P_TraceSurface( portal->edict(), portal, traceStart, traceDirection, hitDistance, portalTrace );
			}
		}

		if ( stepIndex == 0 )
			result->fStartSolid = trace.fStartSolid;

		PortalTraceStep *step = new PortalTraceStep;

		memset( step, 0, sizeof( *step ) );
		result->m_Steps.push_back( step );
		step->m_iStep = stepIndex;
		step->m_vecStart = traceStart;
		step->m_vecDirection = traceDirection;
		step->m_flFraction = hitDistance / scale / distance;
		totalFraction += step->m_flFraction;
		step->m_flTotalFraction = totalFraction;
		step->m_flScale = scale;
		portalHit = portalTrace.m_bHit;

		if ( portalHit )
		{
			CPortal *portal = portalTrace.m_pPortal;
			Vector offset = ( portalTrace.m_vecPosition - portal->GetOrigin() ) * portalTrace.m_flScale;
			Vector rotation = portal->GetSourceAngles() - portal->GetAngles();

			UTIL_RotateVector( &offset, rotation, g_vecZero, true );
			traceStart = offset + portal->GetSourceOrigin();
			UTIL_RotateVector( &traceDirection, rotation, g_vecZero, true );
			remainingDistance = ( remainingDistance - hitDistance ) * portalTrace.m_flScale;
			scale *= portalTrace.m_flScale;
			step->m_pPortal = portal->edict();
			step->m_vecEnd = portalTrace.m_vecPosition;
			step->m_vecPlaneNormal = portalTrace.m_vecNormal;
		}
		else
		{
			step->m_pPortal = NULL;
			step->m_vecEnd = trace.vecEndPos;
			step->m_vecPlaneNormal = trace.vecPlaneNormal;

			if ( trace.flFraction == 1 )
				totalFraction = 1;
		}

		step->m_flDistance = ( step->m_vecEnd - step->m_vecStart ).Length();
		stepIndex++;

		if ( !portalHit )
		{
			result->fAllSolid = trace.fAllSolid;
			result->fInOpen = trace.fInOpen;
			result->fInWater = trace.fInWater;
			result->vecEndPos = trace.vecEndPos;
			result->flPlaneDist = trace.flPlaneDist;
			result->vecPlaneNormal = trace.vecPlaneNormal;
			result->pHit = trace.pHit;
			result->iHitgroup = trace.iHitgroup;
			result->m_vecDirection = traceDirection;
			result->m_flScale = scale;
		}

		if ( callback )
			callback( step );
	} while ( portalHit );

	result->flFraction = totalFraction;
}

void UTIL_TracePortal( const Vector &start, const Vector &direction, float distance,
                       IGNORE_MONSTERS ignoreMonsters, IGNORE_GLASS ignoreGlass, edict_t *ignore,
                       CBasePlayer *player, PortalTraceResult *result, PortalTraceCallback callback )
{
	P_TracePortal( start, direction, distance, ignoreMonsters, ignoreGlass, ignore,
	               player, result, callback );
}
