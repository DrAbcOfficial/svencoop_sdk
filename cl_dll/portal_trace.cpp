#include "hud.h"
#include "cl_util.h"
#include "portal_manager.h"
#include "event_api.h"

struct PortalSurfaceTrace
{
	bool			m_bHit;
	ClientPortal	*m_pPortal;
	float			m_flScale;
	Vector			m_vecPosition;
	Vector			m_vecNormal;
};

// Projects onto the plane that drops the normal's largest axis, then counts edge crossings
bool PointInPolygon(const Vector &point, const glpoly_s *poly, const Vector &normal)
{
	int firstAxis, secondAxis;

	if (fabs(normal.x) > fabs(normal.y) && fabs(normal.x) > fabs(normal.z))
	{
		firstAxis = 1;
		secondAxis = 2;
	}
	else
	{
		firstAxis = 0;
		secondAxis = fabs(normal.y) > fabs(normal.z) ? 2 : 1;
	}

	bool inside = false;

	for (int i = 0, previous = poly->numverts - 1; i < poly->numverts; previous = i++)
	{
		const float *current = poly->verts[i];
		const float *last = poly->verts[previous];

		if ((current[secondAxis] > point[secondAxis]) != (last[secondAxis] > point[secondAxis])
			&& (last[firstAxis] - current[firstAxis]) * (point[secondAxis] - current[secondAxis])
			/ (last[secondAxis] - current[secondAxis]) + current[firstAxis] > point[firstAxis])
			inside = !inside;
	}

	return inside;
}

void P_TraceSurface(cl_entity_t *entity, ClientPortal &portal, const Vector &start,
	const Vector &direction, float &distance, PortalSurfaceTrace &trace)
{
	Vector localStart = start - portal.GetOrigin();
	Vector localDirection = direction;
	Vector rotation = -portal.GetAngles();
	Vector hitPosition(0, 0, 0), hitNormal(0, 0, 0);
	msurface_t *hitSurface = NULL;

	RotateVectorReverse(localStart, rotation, Vector(0, 0, 0));
	RotateVectorReverse(localDirection, rotation, Vector(0, 0, 0));

	for (auto surface : portal.m_PortalSurfaces)
	{
		glpoly_s *poly = surface->polys;
		Vector normal(0, 0, 0);
		Vector first(poly->verts[0]);

		for (int i = 2; i < poly->numverts; i++)
		{
			Vector third(poly->verts[i]);

			normal = CrossProduct(third - Vector(poly->verts[1]), third - first);
			if (normal.Length() > 0.01f)
			{
				normal = normal.Normalize();
				break;
			}

			normal = Vector(0, 0, 0);
		}

		float facing = DotProduct(normal, localDirection);

		if (facing >= -0.01f)
			continue;

		float intersection = DotProduct(first - localStart, normal) / facing;

		if (intersection <= 0.01f || distance + 1 < intersection)
			continue;

		Vector point = localStart + localDirection * intersection;

		if (PointInPolygon(point, poly, normal))
		{
			distance = intersection;
			hitSurface = surface;
			hitPosition = point;
			hitNormal = normal;
		}
	}

	if (hitSurface)
	{
		RotateVector(hitPosition, portal.GetAngles(), Vector(0, 0, 0));
		RotateVector(hitNormal, portal.GetAngles(), Vector(0, 0, 0));
		trace.m_bHit = true;
		trace.m_pPortal = &portal;
		trace.m_flScale = portal.GetZoom();
		trace.m_vecPosition = hitPosition + portal.GetOrigin();
		trace.m_vecNormal = -hitNormal;
	}
}

void ClientPortalManager::TracePortal(const Vector &start, const Vector &direction, float distance,
	int flags, int ignore, PortalTraceResult *result)
{
	Vector traceStart = start;
	Vector traceDirection = direction.Normalize();
	float remainingDistance = distance;
	float totalFraction = 0;
	float scale = 1;
	int stepIndex = 0;

	for (;;)
	{
		pmtrace_t trace = {};
		Vector end = traceStart + traceDirection * remainingDistance;

		gEngfuncs.pEventAPI->EV_PlayerTrace(traceStart, end, flags, ignore, &trace);

		float hitDistance = (trace.endpos - traceStart).Length();
		PortalSurfaceTrace portalTrace = {};
		ClientPortal portalCopy;

		portalTrace.m_flScale = 1;

		for (auto &entry : m_Portals)
		{
			// Keep the copied portal when an earlier surface hit survives the next iteration
			portalCopy = entry.second;
			if (portalCopy.GetPortalMode() == PORTAL_PORTAL && portalCopy.IsActive()
				&& portalCopy.IsShootThroughEnabled())
			{
				P_TraceSurface(gEngfuncs.GetEntityByIndex(trace.ent), portalCopy, traceStart,
					traceDirection, hitDistance, portalTrace);
			}
		}

		if (stepIndex == 0)
			result->startsolid = trace.startsolid;

		PortalTraceStep *step = new PortalTraceStep;

		memset(step, 0, sizeof(*step));
		result->m_Steps.push_back(step);
		step->m_iStep = stepIndex;
		step->m_flFraction = hitDistance / scale / distance;
		totalFraction += step->m_flFraction;
		step->m_flTotalFraction = totalFraction;
		step->m_flScale = scale;
		step->m_vecStart = traceStart;
		step->m_vecDirection = traceDirection;

		if (portalTrace.m_bHit)
		{
			ClientPortal *portal = portalTrace.m_pPortal;
			Vector origin = (portalTrace.m_vecPosition - portal->GetOrigin()) * portalTrace.m_flScale;
			Vector rotation = portal->GetSourceAngles() - portal->GetAngles();

			RotateVectorReverse(origin, rotation, Vector(0, 0, 0));
			RotateVectorReverse(traceDirection, rotation, Vector(0, 0, 0));
			traceStart = origin + portal->GetSourceOrigin();
			remainingDistance = (remainingDistance - hitDistance) * portalTrace.m_flScale;
			scale *= portalTrace.m_flScale;
			step->m_iPortal = portal->m_pEntity->index;
			step->m_vecEnd = portalTrace.m_vecPosition;
			step->m_vecPlaneNormal = portalTrace.m_vecNormal;
		}
		else
		{
			step->m_iPortal = -1;
			step->m_vecEnd = trace.endpos;
			step->m_vecPlaneNormal = trace.plane.normal;

			if (trace.fraction == 1)
				totalFraction = 1;
		}

		step->m_flDistance = (step->m_vecEnd - step->m_vecStart).Length();
		stepIndex++;

		if (!portalTrace.m_bHit)
		{
			result->allsolid = trace.allsolid;
			result->inopen = trace.inopen;
			result->inwater = trace.inwater;
			result->endpos = trace.endpos;
			result->plane = trace.plane;
			result->ent = trace.ent;
			result->deltavelocity = trace.deltavelocity;
			result->hitgroup = trace.hitgroup;
			result->m_vecDirection = traceDirection;
			result->m_flScale = scale;
			break;
		}
	}

	result->fraction = totalFraction;
}
