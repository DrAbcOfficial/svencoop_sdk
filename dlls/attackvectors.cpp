#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "attackvectors.h"

CAttackVectors::CAttackVectors()
{
	m_pHead = NULL;
}

CAttackVectors::~CAttackVectors()
{
	Clear();
}

void CAttackVectors::Add( const Vector &point )
{
	AttackVector *entry = new AttackVector;

	entry->m_vecPoint = point;
	entry->m_pNext = m_pHead;
	m_pHead = entry;
}

void CAttackVectors::Clear( void )
{
	while ( m_pHead )
	{
		AttackVector *next = m_pHead->m_pNext;

		delete m_pHead;
		m_pHead = next;
	}
}

bool CAttackVectors::PointIsOnPlane( CBaseEntity *entity, const Vector &point )
{
	entvars_t *vars = entity->pev;

	return point.x <= vars->absmin.x || point.x >= vars->absmax.x || point.y <= vars->absmin.y || point.y >= vars->absmax.y || point.z <= vars->absmin.z || point.z >= vars->absmax.z;
}

void CAttackVectors::DrawSprite( CBaseEntity *entity, const Vector &point, bool onPlane )
{
	edict_t *edict = entity->pev ? entity->pev->pContainingEntity : NULL;
	TraceResult trace = {};

	UTIL_TraceHull( point, point, ignore_monsters, 0, NULL, &trace );

	if ( trace.pHit == edict )
	{
		Add( point );
	}
	else if ( onPlane )
	{
		UTIL_TraceLine( point, entity->Center(), ignore_monsters, dont_ignore_glass, NULL, &trace );

		if ( trace.pHit == edict )
			Add( trace.vecEndPos );
	}
}

void CAttackVectors::CalculateAttackVectors( CBaseEntity *entity )
{
	if ( !entity || !entity->pev )
		return;

	edict_t *edict = entity->pev->pContainingEntity;

	if ( !edict || edict->free || !ENTINDEX( edict ) )
		return;

	Vector size = entity->pev->absmax - entity->pev->absmin;
	int xSteps = V_max( 3, V_min( 6, (int) ( size.x / 48.0f ) ) );
	int ySteps = V_max( 3, V_min( 6, (int) ( size.y / 48.0f ) ) );
	int zSteps = V_max( 3, V_min( 6, (int) ( size.z / 96.0f ) ) );
	Vector step( size.x / xSteps, size.y / ySteps, size.z / zSteps );
	Vector minimum = entity->pev->absmin;

	for ( int z = 0; z <= zSteps; z++ )
	{
		for ( int y = 0; y <= ySteps; y++ )
		{
			for ( int x = 0; x <= xSteps; x++ )
			{
				Vector point( minimum.x + x * step.x, minimum.y + y * step.y, minimum.z + z * step.z );

				DrawSprite( entity, point, PointIsOnPlane( entity, point ) );
			}
		}
	}
}
