#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "gamerules/gamerules.h"
#include "scripting/core/CASAdminControl.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASLog.h"
#include "server/CServer.h"
#include "server/AdminLevel.h"

bool ValidateAdminAccess( CBasePlayer *invoker, CBasePlayer *target, const char *method, const char *failure )
{
	if ( !invoker )
		return false;

	if ( !g_pGameRules || !g_pGameRules->IsMultiplayer() )
	{
		CASLog::GetInstance()->Warning( AS_LOG_DEFAULT,
		                                "CAdminControl::%s: cannot execute, not in a multiplayer game!\n", method );
		return false;
	}

	if ( ::AdminLevel( invoker ) < ADMIN_YES )
	{
		CASLog::GetInstance()->Log( (ASLogLevel_t) 1,
		                            "CAdminControl::%s: %s, invoking player has no admin rights!\n", method, failure );
		return false;
	}

	if ( ::AdminLevel( target ) >= ADMIN_YES )
	{
		CASLog::GetInstance()->Log( (ASLogLevel_t) 1,
		                            "CAdminControl::%s: %s, target is an admin!\n", method, failure );
		return false;
	}

	return true;
}

CASAdminControl::CASAdminControl()
{
}

CASAdminControl::~CASAdminControl()
{
}

unsigned int CASAdminControl::GetReservedSlots( void ) const
{
	return CServer::GetInstance()->GetReservedSlots();
}

void CASAdminControl::SetReservedSlots( unsigned int slots )
{
	CServer::GetInstance()->SetReservedSlots( slots );
}

bool CASAdminControl::KillPlayer( CBasePlayer *invoker, CBasePlayer *target, float respawnTime )
{
	if ( !ValidateAdminAccess( invoker, target, "KillPlayer", "Cannot kill player" ) )
		return false;

	return KillPlayer( target, respawnTime );
}

bool CASAdminControl::KillPlayer( CBasePlayer *target, float respawnTime )
{
	if ( !target || !target->IsAlive() )
		return false;

	CServer::GetInstance()->KillPlayer( target, respawnTime );
	return true;
}

bool CASAdminControl::KickPlayer( CBasePlayer *invoker, CBasePlayer *target, float banTime )
{
	if ( !ValidateAdminAccess( invoker, target, "KickPlayer", "Cannot kick player" ) )
		return false;

	return CServer::GetInstance()->KickPlayer( target, NULL, banTime );
}

bool CASAdminControl::KickPlayer( CBasePlayer *target, float banTime )
{
	return CServer::GetInstance()->KickPlayer( target, NULL, banTime );
}

bool CASAdminControl::BanPlayer( CBasePlayer *invoker, CBasePlayer *target, float banTime )
{
	if ( !ValidateAdminAccess( invoker, target, "BanPlayer", "Cannot ban player" ) )
		return false;

	return CServer::GetInstance()->BanPlayer( target, NULL, banTime );
}

bool CASAdminControl::BanPlayer( CBasePlayer *target, float banTime )
{
	return CServer::GetInstance()->BanPlayer( target, NULL, banTime );
}

bool CASAdminControl::SlapPlayer( CBasePlayer *invoker, CBasePlayer *target, float damage, const Vector &direction, int damageType )
{
	if ( !ValidateAdminAccess( invoker, target, "SlapPlayer", "Cannot slap player" ) )
		return false;

	return SlapPlayer( target, damage, direction, damageType );
}

bool CASAdminControl::SlapPlayer( CBasePlayer *target, float damage, const Vector &direction, int damageType )
{
	if ( !target )
		return false;

	target->pev->velocity = target->pev->velocity + direction.Normalize() * 100.0f;

	entvars_t *world = CWorld::GetInstance()->pev;

	target->TakeDamage( world, world, damage, damageType );
	return true;
}

bool CASAdminControl::SlapPlayer( CBasePlayer *invoker, CBasePlayer *target, float damage, int damageType )
{
	float z = RANDOM_FLOAT( -1.0f, 1.0f );
	float y = RANDOM_FLOAT( -1.0f, 1.0f );
	float x = RANDOM_FLOAT( -1.0f, 1.0f );
	Vector direction( x, y, z );

	return SlapPlayer( invoker, target, damage, direction, damageType );
}

bool CASAdminControl::SlapPlayer( CBasePlayer *target, float damage, int damageType )
{
	float z = RANDOM_FLOAT( -1.0f, 1.0f );
	float y = RANDOM_FLOAT( -1.0f, 1.0f );
	float x = RANDOM_FLOAT( -1.0f, 1.0f );
	Vector direction( x, y, z );

	return SlapPlayer( target, damage, direction, damageType );
}

void RegisterSCScriptAdmin( CASDocumentation *documentation )
{
	asDWORD previousAccess = documentation->SetDefaultAccessMask( AS_ACCESS_PLUGIN );

	documentation->RegisterObjectType( "Admin control panel.", "CAdminControl", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod( "Sets the number of reserved slots.", "CAdminControl",
	                                     "void SetReservedSlots(uint uiReservedSlots)",
	                                     asMETHOD( CASAdminControl, SetReservedSlots ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the number of reserved slots.", "CAdminControl",
	                                     "uint GetReservedSlots() const",
	                                     asMETHOD( CASAdminControl, GetReservedSlots ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Kills the given player. Killing player must have admin rights. Target player cannot be an admin.", "CAdminControl",
	                                     "bool KillPlayer(CBasePlayer@ pKiller, CBasePlayer@ pKillee, float flRespawntime = -1.0f)",
	                                     asMETHODPR( CASAdminControl, KillPlayer, (CBasePlayer *, CBasePlayer *, float), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Kills the given player.", "CAdminControl",
	                                     "bool KillPlayer(CBasePlayer@ pKillee, float flRespawntime = -1.0f)",
	                                     asMETHODPR( CASAdminControl, KillPlayer, (CBasePlayer *, float), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Kicks the given player. Kicking player must have admin rights. Target player cannot be an admin.", "CAdminControl",
	                                     "bool KickPlayer(CBasePlayer@ pKicker, CBasePlayer@ pKickee, const float flCustomBanTime = -1)",
	                                     asMETHODPR( CASAdminControl, KickPlayer, (CBasePlayer *, CBasePlayer *, float), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Kicks the given player.", "CAdminControl",
	                                     "bool KickPlayer(CBasePlayer@ pKickee, const float flCustomBanTime = -1)",
	                                     asMETHODPR( CASAdminControl, KickPlayer, (CBasePlayer *, float), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Bans the given player. Banning player must have admin rights Target player cannot be an admin.", "CAdminControl",
	                                     "bool BanPlayer(CBasePlayer@ pBanner, CBasePlayer@ pBannee, const float flCustomBanTime = -1)",
	                                     asMETHODPR( CASAdminControl, BanPlayer, (CBasePlayer *, CBasePlayer *, float), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Bans the given player.", "CAdminControl",
	                                     "bool BanPlayer(CBasePlayer@ pBannee, const float flCustomBanTime = -1)",
	                                     asMETHODPR( CASAdminControl, BanPlayer, (CBasePlayer *, float), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Slaps the given player. Slapping player must have admin rights. Target player cannot be an admin.", "CAdminControl",
	                                     "bool SlapPlayer(CBasePlayer@ pSlapper, CBasePlayer@ pSlappee, float flDamage, const Vector& in vecDirection, int iDamageType = 0)",
	                                     asMETHODPR( CASAdminControl, SlapPlayer, (CBasePlayer *, CBasePlayer *, float, const Vector &, int), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Slaps the given player.", "CAdminControl",
	                                     "bool SlapPlayer(CBasePlayer@ pSlappee, float flDamage, const Vector& in vecDirection, int iDamageType = 0)",
	                                     asMETHODPR( CASAdminControl, SlapPlayer, (CBasePlayer *, float, const Vector &, int), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Slaps the given player. Slapping player must have admin rights. Target player cannot be an admin. Slap direction is randomized.", "CAdminControl",
	                                     "bool SlapPlayer(CBasePlayer@ pSlapper, CBasePlayer@ pSlappee, float flDamage, int iDamageType = 0)",
	                                     asMETHODPR( CASAdminControl, SlapPlayer, (CBasePlayer *, CBasePlayer *, float, int), bool ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Slaps the given player. Slap direction is randomized.", "CAdminControl",
	                                     "bool SlapPlayer(CBasePlayer@ pSlappee, float flDamage, int iDamageType = 0)",
	                                     asMETHODPR( CASAdminControl, SlapPlayer, (CBasePlayer *, float, int), bool ), asCALL_THISCALL );
	documentation->RegisterGlobalProperty( "CAdminControl instance for plugins", "CAdminControl g_AdminControl", CASAdminControl::GetInstance() );
	documentation->SetDefaultAccessMask( previousAccess );
}
