#ifndef PATHCONDITION_H
#define PATHCONDITION_H

#include "utils/EntityHandle.h"

class CBaseMonster;
class CASDocumentation;
class CPathWaypoint;
struct PathConditionPair_t;

struct PathConditionGroup_t
{
	EntityHandle<CPathCondition> m_hCondition;
	PathConditionGroup_t *m_pNext;
};

class CPathCondition : public CBaseEntity
{
public:
	void Spawn( void ) { }

	void KeyValue( KeyValueData *data );

	void EXPORT Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );

	bool IsValid( CBaseMonster *monster );

	CPathWaypoint *GetStartingPoint( void );

	bool m_fEnabled;
	float m_flMinDistance;
	float m_flMaxDistance;
	PathConditionPair_t *m_pConditions;
};

class CPathConditionController : public CBaseEntity
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *data );
	void EXPORT Initialize( void );
	void ClearAll( void );
	CPathCondition *FindBestGroup( CBaseMonster *monster );
	void EXPORT Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );

	bool m_fEnabled;
	PathConditionGroup_t *m_pGroups;
};

void RegisterSCScriptPathConditionEntity( CASDocumentation *documentation );

#endif
