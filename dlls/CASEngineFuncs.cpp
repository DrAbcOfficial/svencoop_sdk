#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "CASEngineFuncs.h"
#include "CCVarSystem.h"
#include "CKeyValueBuffer.h"
#include "CMultiplayGameMgrHelper.h"

CASEngineFuncs::CASEngineFuncs()
{
}

CASEngineFuncs::~CASEngineFuncs()
{
}

int CASEngineFuncs::ModelIndex( const CString &modelName )
{
	return g_engfuncs.pfnModelIndex( modelName.Data() );
}

int CASEngineFuncs::ModelFrames( int modelIndex )
{
	return g_engfuncs.pfnModelFrames( modelIndex );
}

void CASEngineFuncs::ChangeLevel( const CString &levelName )
{
	g_engfuncs.pfnChangeLevel( (char *) levelName.Data(), NULL );
}

float CASEngineFuncs::VecToYaw( const Vector &vector )
{
	return g_engfuncs.pfnVecToYaw( vector );
}

void CASEngineFuncs::VecToAngles( const Vector &vector, Vector &angles )
{
	g_engfuncs.pfnVecToAngles( vector, angles );
}

void CASEngineFuncs::MoveToOrigin( edict_t *entity, const Vector &goal, float distance, int moveType )
{
	g_engfuncs.pfnMoveToOrigin( entity, goal, distance, moveType );
}

void CASEngineFuncs::ChangeYaw( edict_t *entity )
{
	g_engfuncs.pfnChangeYaw( entity );
}

void CASEngineFuncs::ChangePitch( edict_t *entity )
{
	g_engfuncs.pfnChangePitch( entity );
}

int CASEngineFuncs::GetEntityIllum( edict_t *entity )
{
	return g_engfuncs.pfnGetEntityIllum( entity );
}

edict_t *CASEngineFuncs::FindClientInPVS( edict_t *startEntity )
{
	return g_engfuncs.pfnFindClientInPVS( startEntity );
}

edict_t *CASEngineFuncs::EntitiesInPVS( edict_t *startEntity )
{
	return g_engfuncs.pfnEntitiesInPVS( startEntity );
}

void CASEngineFuncs::MakeVectors( const Vector &vector )
{
	g_engfuncs.pfnMakeVectors( vector );
}

void CASEngineFuncs::AngleVectors( const Vector &angles, Vector &forward, Vector &right, Vector &up )
{
	g_engfuncs.pfnAngleVectors( angles, forward, right, up );
}

void CASEngineFuncs::MakeStatic( edict_t *entity )
{
	g_engfuncs.pfnMakeStatic( entity );
}

int CASEngineFuncs::EntIsOnFloor( edict_t *entity )
{
	return g_engfuncs.pfnEntIsOnFloor( entity );
}

int CASEngineFuncs::DropToFloor( edict_t *entity )
{
	return g_engfuncs.pfnDropToFloor( entity );
}

int CASEngineFuncs::WalkMove( edict_t *entity, float yaw, float distance, int move )
{
	return g_engfuncs.pfnWalkMove( entity, yaw, distance, move );
}

void CASEngineFuncs::GetAimVector( edict_t *entity, float speed, Vector &result )
{
	g_engfuncs.pfnGetAimVector( entity, speed, result );
}

void CASEngineFuncs::ServerCommand( const CString &command )
{
	g_engfuncs.pfnServerCommand( (char *) command.Data() );
}

void CASEngineFuncs::ServerExecute()
{
	g_engfuncs.pfnServerExecute();
}

void CASEngineFuncs::ParticleEffect( const Vector &origin, const Vector &direction, float color, float count )
{
	g_engfuncs.pfnParticleEffect( origin, direction, color, count );
}

void CASEngineFuncs::LightStyle( int style, const CString &value )
{
	g_engfuncs.pfnLightStyle( style, (char *) STRING( AllocStringLocal( value.Data() ) ) );
}

int CASEngineFuncs::DecalIndex( const CString &name )
{
	return g_engfuncs.pfnDecalIndex( name.Data() );
}

int CASEngineFuncs::PointContents( const Vector &point )
{
	return g_engfuncs.pfnPointContents( point );
}

float CASEngineFuncs::CVarGetFloat( const CString &name )
{
	return g_engfuncs.pfnCVarGetFloat( name.Data() );
}

CString CASEngineFuncs::CVarGetString( const CString &name )
{
	return CString( g_engfuncs.pfnCVarGetString( name.Data() ) );
}

void CASEngineFuncs::CVarSetFloat( const CString &name, float value )
{
	char buffer[256];

	sprintf( buffer, "%f", value );
	CCVarSystem::GetInstance()->SetCVar( name.Data(), buffer, CCVarSystem::SET_ALL_CVARS );
}

void CASEngineFuncs::CVarSetString( const CString &name, const CString &value )
{
	CCVarSystem::GetInstance()->SetCVar( name.Data(), value.Data(), CCVarSystem::SET_ALL_CVARS );
}

entvars_t *CASEngineFuncs::GetVarsOfEnt( edict_t *entity )
{
	return g_engfuncs.pfnGetVarsOfEnt( entity );
}

int CASEngineFuncs::IndexOfEdict( const edict_t *entity )
{
	return g_engfuncs.pfnIndexOfEdict( entity );
}

edict_t *CASEngineFuncs::PEntityOfEntIndex( int index )
{
	return g_engfuncs.pfnPEntityOfEntIndex( index );
}

edict_t *CASEngineFuncs::FindEntityByVars( entvars_t *vars )
{
	return g_engfuncs.pfnFindEntityByVars( vars );
}

void CASEngineFuncs::GetBonePosition( const edict_t *entity, int bone, Vector &origin, Vector &angles )
{
	g_engfuncs.pfnGetBonePosition( entity, bone, origin, angles );
}

void CASEngineFuncs::ClientPrintf( CBasePlayer *player, PRINT_TYPE printType, const CString &message )
{
	g_engfuncs.pfnClientPrintf( player->pev ? player->pev->pContainingEntity : NULL, printType, message.Data() );
}

void CASEngineFuncs::ServerPrint( const CString &message )
{
	g_engfuncs.pfnServerPrint( message.Data() );
}

CString CASEngineFuncs::Cmd_Args()
{
	return CString( g_engfuncs.pfnCmd_Args() );
}

CString CASEngineFuncs::Cmd_Argv( int index )
{
	return CString( g_engfuncs.pfnCmd_Argv( index ) );
}

int CASEngineFuncs::Cmd_Argc()
{
	return g_engfuncs.pfnCmd_Argc();
}

void CASEngineFuncs::GetAttachment( const edict_t *entity, int attachment, Vector &origin, Vector &angles )
{
	g_engfuncs.pfnGetAttachment( entity, attachment, origin, angles );
}

void CASEngineFuncs::SetView( const edict_t *entity, const edict_t *viewEntity )
{
	g_engfuncs.pfnSetView( entity, viewEntity );
}

float CASEngineFuncs::Time()
{
	return g_engfuncs.pfnTime();
}

void CASEngineFuncs::CrosshairAngle( const edict_t *entity, float pitch, float yaw )
{
	g_engfuncs.pfnCrosshairAngle( entity, pitch, yaw );
}

CString CASEngineFuncs::GetGameDir()
{
	char *directory = new char[261];

	g_engfuncs.pfnGetGameDir( directory );
	return CString( directory, true );
}

void CASEngineFuncs::RunPlayerMove( edict_t *entity, const Vector &viewAngles, float forwardMove, float sideMove,
                                    float upMove, unsigned short buttons, unsigned char impulse, unsigned char msec )
{
	g_engfuncs.pfnRunPlayerMove( entity, viewAngles, forwardMove, sideMove, upMove, buttons, impulse, msec );
}

int CASEngineFuncs::NumberOfEntities()
{
	return g_engfuncs.pfnNumberOfEntities();
}

CKeyValueBuffer *CASEngineFuncs::GetInfoKeyBuffer( edict_t *entity )
{
	if ( !entity )
		return NULL;

	char *buffer = g_engfuncs.pfnGetInfoKeyBuffer( entity );

	if ( !buffer )
		return NULL;

	return new CInfoKeyValueBuffer( entity, buffer );
}

CKeyValueBuffer *CASEngineFuncs::GetPhysicsKeyBuffer( edict_t *entity )
{
	if ( !entity )
		return NULL;

	return new CPhysicsKeyValueBuffer( entity );
}

bool CASEngineFuncs::IsMapValid( const CString &mapName ) const
{
	return g_engfuncs.pfnIsMapValid( (char *) mapName.Data() ) != 0;
}

void CASEngineFuncs::StaticDecal( const Vector &origin, int decalIndex, int entityIndex, int modelIndex )
{
	g_engfuncs.pfnStaticDecal( origin, decalIndex, entityIndex, modelIndex );
}

int CASEngineFuncs::GetPlayerUserId( edict_t *entity )
{
	return g_engfuncs.pfnGetPlayerUserId( entity );
}

bool CASEngineFuncs::IsDedicatedServer() const
{
	return g_engfuncs.pfnIsDedicatedServer() != 0;
}

const cvar_t *CASEngineFuncs::CVarGetPointer( const CString &name ) const
{
	return g_engfuncs.pfnCVarGetPointer( name.Data() );
}

void CASEngineFuncs::GetPlayerStats( const edict_t *entity, int &ping, int &packetLoss )
{
	g_engfuncs.pfnGetPlayerStats( entity, &ping, &packetLoss );
}

bool CASEngineFuncs::Voice_GetClientListening( int receiver, int sender )
{
	return g_engfuncs.pfnVoice_GetClientListening( receiver, sender ) != 0;
}

bool CASEngineFuncs::Voice_SetClientListening( int receiver, int sender, bool listen )
{
	g_GameMgrHelper.SetPlayerBanned( receiver, sender, listen );
	return listen;
}

CString CASEngineFuncs::GetPlayerAuthId( edict_t *entity )
{
	return CString( g_engfuncs.pfnGetPlayerAuthId( entity ) );
}
