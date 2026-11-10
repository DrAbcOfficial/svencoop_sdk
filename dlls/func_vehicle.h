#ifndef FUNC_VEHICLE_H
#define FUNC_VEHICLE_H

class CPathTrack;
class CBasePlayer;

class CFuncVehicle : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *key );
	void Blocked( CBaseEntity *other );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	BOOL OnControls( entvars_t *test );
	int Classify( void );
	int ObjectCaps( void ) { return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | FCAP_DIRECTIONAL_USE; }
	void OverrideReset( void );

	void EXPORT Next( void );
	void EXPORT Find( void );
	void EXPORT NearestPath( void );
	void EXPORT DeadEnd( void );
	void Restart( void );
	void SetControls( entvars_t *controls );
	CBasePlayer *GetDriver( void );
	void SetDriver( CBasePlayer *player );
	void StopSound( void );
	void UpdateSound( void );
	void TerrainFollowing( void );
	void CollisionDetection( void );
	void CheckTurning( void );

	int Save( CSave &save );
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	CPathTrack	*m_ppath;
	float		m_length;
	float		m_width;
	float		m_height;
	float		m_speed;
	float		m_dir;
	float		m_startSpeed;
	Vector		m_controlMins;
	Vector		m_controlMaxs;
	int			m_soundPlaying;
	int			m_sounds;
	int			m_acceleration;
	float		m_flVolume;
	float		m_flBank;
	float		m_oldSpeed;

	int				m_iTurnAngle;
	float			m_flSteeringTime;
	float			m_flAcceleratorTime;
	float			m_flTurnStartTime;
	float			m_flLaunchTime;
	float			m_flPreviousGroundZ;
	float			m_flSteeringDelay;
	float			m_flNextSoundTime;
	Vector			m_vecFrontLeft;
	Vector			m_vecFront;
	Vector			m_vecFrontRight;
	Vector			m_vecBackLeft;
	Vector			m_vecBack;
	Vector			m_vecBackRight;
	Vector			m_vecSurfaceNormal;
	Vector			m_vecDirection;
	EHANDLE			m_hDriver;
	unsigned short	m_usAdjustPitch;
};

class CFuncVehicleControls : public CBaseEntity
{
public:
	void Spawn( void );
	int ObjectCaps( void ) { return 0; }
	void EXPORT Find( void );
};

#endif
