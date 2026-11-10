#include "extdll.h"
#include "CASUtility.h"
#include "server_support.h"
#include "entvars_types.h"
#include "RGBA.h"
#include "cbase.h"
#include "player.h"
#include "CServer.h"
#include "CVoteManager.h"
#include "CASObjectInstance.h"

class CASPlayerInVolumeAdapter : public IPlayerInVolumeListener
{
public:
	CASPlayerInVolumeAdapter( asIScriptObject *listener, CASModule *module ) : m_pInstance( NULL )
	{
		if ( listener )
			m_pInstance = new CASObjectInstance( listener->GetObjectType(), listener, module );
	}

	virtual ~CASPlayerInVolumeAdapter()
	{
		if ( m_pInstance )
			m_pInstance->Release();

		m_pInstance = NULL;
	}

	virtual void PlayerDetected( bool inVolume, CBasePlayer *player )
	{
		if ( !m_pInstance )
			return;

		bool optional = false;

		m_pInstance->CallMethod( optional, "void PlayerDetected(const bool, CBasePlayer@)", 0,
		                         (int) inVolume, player );
	}

private:
	CASObjectInstance *m_pInstance;
};

CASUtility::~CASUtility()
{
}

void CASUtility::BloodStream( const Vector &origin, const Vector &direction, int color, int amount )
{
	UTIL_BloodStream( origin, direction, color, amount );
}

void CASUtility::BloodDrips( const Vector &origin, const Vector &direction, int color, int amount )
{
	UTIL_BloodDrips( origin, direction, color, amount );
}

Vector CASUtility::RandomBloodVector()
{
	return UTIL_RandomBloodVector();
}

bool CASUtility::ShouldShowBlood( int color )
{
	return UTIL_ShouldShowBlood( color ) != 0;
}

void CASUtility::BloodDecalTrace( TraceResult &trace, int color )
{
	UTIL_BloodDecalTrace( &trace, color );
}

void CASUtility::DecalTrace( TraceResult &trace, int decal )
{
	UTIL_DecalTrace( &trace, decal );
}

void CASUtility::PlayerDecalTrace( TraceResult &trace, int playerIndex, int decal, bool custom )
{
	UTIL_PlayerDecalTrace( &trace, playerIndex, decal, custom );
}

void CASUtility::GunshotDecalTrace( TraceResult &trace, int decal )
{
	UTIL_GunshotDecalTrace( &trace, decal );
}

void CASUtility::Sparks( const Vector &position )
{
	UTIL_Sparks( position );
}

void CASUtility::Ricochet( const Vector &position, float scale )
{
	UTIL_Ricochet( position, scale );
}

float CASUtility::WaterLevel( const Vector &position, float minimum, float maximum )
{
	return UTIL_WaterLevel( position, minimum, maximum );
}

void CASUtility::Bubbles( const Vector &minimum, const Vector &maximum, int count )
{
	UTIL_Bubbles( minimum, maximum, count );
}

void CASUtility::BubbleTrail( const Vector &start, const Vector &end, int count )
{
	UTIL_BubbleTrail( start, end, count );
}

void CASUtility::StringToVector( Vector &vector, const CString &text, char separator )
{
	UTIL_StringToVector( vector, text.Data(), separator );
}

void CASUtility::StringToRGBA( RGBA &color, const CString &text, char separator )
{
	UTIL_StringToRGBA( color, text.Data(), separator );
}

bool CASUtility::IsStringInt( const CString &text )
{
	return UTIL_IsStringInt( text.Data() );
}

bool CASUtility::IsStringFloat( const CString &text )
{
	return UTIL_IsStringFloat( text.Data() );
}

bool CASUtility::IsWholeNumber( float value, int &rounded )
{
	rounded = (int) floor( (double) value + 0.5 );
	return fabs( (double) value - rounded ) <= 0.000005;
}

bool CASUtility::IsString3DVec( const CString &text )
{
	return UTIL_IsString3DVec( text.Data() );
}

void CASUtility::TraceLine( const Vector &start, const Vector &end, IGNORE_MONSTERS ignoreMonsters,
                            edict_t *ignore, TraceResult &trace )
{
	UTIL_TraceLine( start, end, ignoreMonsters, ignore, &trace );
}

void CASUtility::TraceLine( const Vector &start, const Vector &end, IGNORE_MONSTERS ignoreMonsters,
                            IGNORE_GLASS ignoreGlass, edict_t *ignore, TraceResult &trace )
{
	UTIL_TraceLine( start, end, ignoreMonsters, ignoreGlass, ignore, &trace );
}

void CASUtility::TraceHull( const Vector &start, const Vector &end, IGNORE_MONSTERS ignoreMonsters,
                            int hull, edict_t *ignore, TraceResult &trace )
{
	UTIL_TraceHull( start, end, ignoreMonsters, hull, ignore, &trace );
}

void CASUtility::TraceToss( edict_t *entity, edict_t *ignore, TraceResult &trace )
{
	TRACE_TOSS( entity, ignore, &trace );
}

bool CASUtility::TraceMonsterHull( edict_t *entity, const Vector &start, const Vector &end,
                                   IGNORE_MONSTERS ignoreMonsters, edict_t *ignore, TraceResult &trace )
{
	return TRACE_MONSTER_HULL( entity, start, end, ignoreMonsters == ignore_monsters, ignore, &trace ) != 0;
}

void CASUtility::TraceModel( const Vector &start, const Vector &end, int hull, edict_t *entity, TraceResult &trace )
{
	UTIL_TraceModel( start, end, hull, entity, &trace );
}

CString CASUtility::TraceTexture( edict_t *entity, const Vector &start, const Vector &end )
{
	return CString( TRACE_TEXTURE( entity, start, end ) );
}

TraceResult CASUtility::GetGlobalTrace()
{
	return UTIL_GetGlobalTrace();
}

void CASUtility::GetCircularGaussianSpread( float &x, float &y ) const
{
	UTIL_GetCircularGaussianSpread( x, y );
}

CString CASUtility::BuildEntityLogString( const CString &name, const CString &userID,
                                          const CString &authID, const CString &team ) const
{
	return CString( EntityLogString( name.Data(), userID.Data(), authID.Data(), team.Data() ) );
}

CString CASUtility::BuildPlayerLogString( const CString &name, const CString &userID, const CString &authID ) const
{
	return CString( PlayerLogString( name.Data(), userID.Data(), authID.Data() ) );
}

CBaseEntity *CASUtility::FindEntityForward( CBaseEntity *entity, float distance )
{
	return ::FindEntityForward( entity, distance );
}

CBaseEntity *CASUtility::FindEntityForward( CBaseEntity *entity )
{
	return ::FindEntityForward( entity, 12048.0f );
}

bool CASUtility::IsPlayerInVolume( CBasePlayer *player, CBaseEntity *volume )
{
	if ( !player || !volume )
		return false;

	TraceResult trace;
	int hull = ( player->pev->flags & FL_DUCKING ) ? head_hull : human_hull;
	edict_t *volumeEntity = volume->pev ? ENT( volume->pev ) : NULL;

	trace.vecEndPos = Vector( 0, 0, 0 );
	trace.vecPlaneNormal = Vector( 0, 0, 0 );
	UTIL_TraceModel( player->pev->origin, player->pev->origin, hull, volumeEntity, &trace );

	return trace.fStartSolid != 0;
}

bool CASUtility::VoteActive() const
{
	return CServer::GetInstance()->GetVoteManager().FActiveVote();
}

int CASUtility::CountPlayersInBrushVolume( bool ignoreDeadPlayers, CBaseEntity *volume, int &inside,
                                           int &outside, asIScriptObject *listener ) const
{
	CASModule *module = listener ? CASServerManager::GetInstance()->m_pActiveModule : NULL;
	CASPlayerInVolumeAdapter adapter( listener, module );

	return UTIL_CountPlayersInBrushVolume( ignoreDeadPlayers, volume, inside, outside,
	                                       listener ? &adapter : NULL );
}

void CASUtility::FindHullIntersection( const Vector &source, TraceResult &input, TraceResult &output,
                                       const Vector &minimum, const Vector &maximum, edict_t *entity, float distance )
{
	UTIL_FindHullIntersection( source, input, minimum, maximum, entity, distance );
	output = input;
}

CString CASUtility::GetPlayerLog( edict_t *entity ) const
{
	return CString( PlayerLog( entity ) );
}
