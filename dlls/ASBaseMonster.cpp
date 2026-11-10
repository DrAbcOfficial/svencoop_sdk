#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "ASBaseMonster.h"
#include "CASServerManager.h"
#include "CASLog.h"
#include "../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"

EHANDLE &MonsterEntity_Get_m_hOldEnemy( unsigned int index, CBaseMonster *entity )
{
	if ( index >= MAX_OLD_ENEMIES )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "MonsterEntity::m_hOldEnemy: invalid index!\n" );
		index = 0;
	}

	return entity->m_hOldEnemy[index];
}

Vector &MonsterEntity_Get_m_vecOldEnemy( unsigned int index, CBaseMonster *entity )
{
	if ( index >= MAX_OLD_ENEMIES )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "MonsterEntity::m_vecOldEnemy: invalid index!\n" );
		index = 0;
	}

	return entity->m_vecOldEnemy[index];
}

WayPoint_t &MonsterEntity_Get_m_Route( unsigned int index, CBaseMonster *entity )
{
	if ( index >= ROUTE_SIZE )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "MonsterEntity::m_Route: invalid index!\n" );
		index = 0;
	}

	return entity->m_Route[index];
}

signed char &MonsterEntity_Get_m_rgbTimeBasedDamage( unsigned int index, CBaseMonster *entity )
{
	if ( index >= CDMG_TIMEBASED )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "MonsterEntity::m_rgbTimeBasedDamage: invalid index!\n" );
		index = 0;
	}

	return (signed char &) entity->m_rgbTimeBasedDamage[index];
}

bool MonsterEntity_FInViewCone( const Vector &origin, CBaseMonster *entity )
{
	return entity->FInViewCone( (Vector *) &origin ) != FALSE;
}

Schedule_t *MonsterEntity_ScheduleInList( const CString &name, CScriptArray *array, CBaseMonster *entity )
{
	asIScriptEngine *engine = array->GetArrayObjectType()->GetEngine();
	asITypeInfo *element = engine->GetTypeInfoById( array->GetElementTypeId() );

	if ( !element || !element->DerivesFrom( CASServerManager::GetInstance()->GetTypeInfoByName( "Schedule" ) ) )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "MonsterEntity::ScheduleInList: array must contain Schedules!\n" );
		return NULL;
	}

	return entity->ScheduleInList( name.Data(), (Schedule_t **) array->GetBuffer(), array->GetSize() );
}

Schedule_t *MonsterEntity_ScheduleFromName( const CString &name, CBaseMonster *entity )
{
	return entity->ScheduleFromName( name.Data() );
}

void MonsterEntity_PlaySentence( const CString &sentence, float duration, float volume, float attenuation, CBaseMonster *entity )
{
	entity->PlaySentence( sentence.Data(), duration, volume, attenuation );
}

void MonsterEntity_PlayScriptedSentence( const CString &sentence, float duration, float volume, float attenuation,
                                         bool concurrent, CBaseEntity *listener, CBaseMonster *entity )
{
	entity->PlayScriptedSentence( sentence.Data(), duration, volume, attenuation, concurrent, listener );
}

void MonsterEntity_SetSequenceByName( const CString &sequence, CBaseMonster *entity )
{
	entity->SetSequenceByName( sequence.Data() );
}

CBaseEntity *MonsterEntity_DropItem( const CString &name, const Vector &position, const Vector &angles, CBaseMonster *entity )
{
	return entity->DropItem( name.Data(), position, angles );
}

CString MonsterEntity_Get_m_szTextureName( CBaseMonster *entity )
{
	return CString( entity->m_szTextureName );
}
