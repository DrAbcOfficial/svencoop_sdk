#ifndef CASENGINEFUNCS_H
#define CASENGINEFUNCS_H

#include "extdll.h"
#include "CASServerScriptGlobal.h"
#include "CString.h"

class CBasePlayer;
class CKeyValueBuffer;

class CASEngineFuncs : public CASServerScriptGlobal<CASEngineFuncs>
{
public:
	virtual ~CASEngineFuncs();

	int ModelIndex( const CString &modelName );
	int ModelFrames( int modelIndex );
	void ChangeLevel( const CString &levelName );
	float VecToYaw( const Vector &vector );
	void VecToAngles( const Vector &vector, Vector &angles );
	void MoveToOrigin( edict_t *entity, const Vector &goal, float distance, int moveType );
	void ChangeYaw( edict_t *entity );
	void ChangePitch( edict_t *entity );
	int GetEntityIllum( edict_t *entity );
	edict_t *FindClientInPVS( edict_t *startEntity );
	edict_t *EntitiesInPVS( edict_t *startEntity );
	void MakeVectors( const Vector &vector );
	void AngleVectors( const Vector &angles, Vector &forward, Vector &right, Vector &up );
	void MakeStatic( edict_t *entity );
	int EntIsOnFloor( edict_t *entity );
	int DropToFloor( edict_t *entity );
	int WalkMove( edict_t *entity, float yaw, float distance, int move );
	void GetAimVector( edict_t *entity, float speed, Vector &result );
	void ServerCommand( const CString &command );
	void ServerExecute();
	void ParticleEffect( const Vector &origin, const Vector &direction, float color, float count );
	void LightStyle( int style, const CString &value );
	int DecalIndex( const CString &name );
	int PointContents( const Vector &point );
	float CVarGetFloat( const CString &name );
	CString CVarGetString( const CString &name );
	void CVarSetFloat( const CString &name, float value );
	void CVarSetString( const CString &name, const CString &value );
	entvars_t *GetVarsOfEnt( edict_t *entity );
	int IndexOfEdict( const edict_t *entity );
	edict_t *PEntityOfEntIndex( int index );
	edict_t *FindEntityByVars( entvars_t *vars );
	void GetBonePosition( const edict_t *entity, int bone, Vector &origin, Vector &angles );
	void ClientPrintf( CBasePlayer *player, PRINT_TYPE printType, const CString &message );
	void ServerPrint( const CString &message );
	CString Cmd_Args();
	CString Cmd_Argv( int index );
	int Cmd_Argc();
	void GetAttachment( const edict_t *entity, int attachment, Vector &origin, Vector &angles );
	void SetView( const edict_t *entity, const edict_t *viewEntity );
	float Time();
	void CrosshairAngle( const edict_t *entity, float pitch, float yaw );
	CString GetGameDir();
	void RunPlayerMove( edict_t *entity, const Vector &viewAngles, float forwardMove, float sideMove,
	                    float upMove, unsigned short buttons, unsigned char impulse, unsigned char msec );
	int NumberOfEntities();
	CKeyValueBuffer *GetInfoKeyBuffer( edict_t *entity );
	CKeyValueBuffer *GetPhysicsKeyBuffer( edict_t *entity );
	bool IsMapValid( const CString &mapName ) const;
	void StaticDecal( const Vector &origin, int decalIndex, int entityIndex, int modelIndex );
	int GetPlayerUserId( edict_t *entity );
	bool IsDedicatedServer() const;
	const cvar_t *CVarGetPointer( const CString &name ) const;
	void GetPlayerStats( const edict_t *entity, int &ping, int &packetLoss );
	bool Voice_GetClientListening( int receiver, int sender );
	bool Voice_SetClientListening( int receiver, int sender, bool listen );
	CString GetPlayerAuthId( edict_t *entity );

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASEngineFuncs>;

	CASEngineFuncs();
};

#endif
