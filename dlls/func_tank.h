#ifndef FUNC_TANK_H
#define FUNC_TANK_H

class CBasePlayer;
class CBaseMonster;
class CASDocumentation;

#define SF_TANK_ACTIVE 0x0001
#define SF_TANK_PLAYER 0x0002
#define SF_TANK_HUMANS 0x0004
#define SF_TANK_ALIENS 0x0008
#define SF_TANK_LINEOFSIGHT 0x0010
#define SF_TANK_CANCONTROL 0x0020
#define SF_TANK_USERELATIONS 0x0200
#define SF_TANK_PLAYERCANTFIRE 0x0400
#define SF_TANK_SOUNDON 0x8000

enum TANKBULLET
{
	TANK_BULLET_NONE = 0,
	TANK_BULLET_9MM = 1,
	TANK_BULLET_MP5 = 2,
	TANK_BULLET_12MM = 3,
};

class CFuncTank : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void Think( void );
	void UpdateOnRemove( void );
	void TrackTarget( void );
	int Classify( void );
	int IRelationship( CBaseEntity *target, bool ignoreNoTarget = false );

	virtual void Fire( const Vector &barrelEnd, const Vector &forward, entvars_t *pevAttacker );
	void Fire( void );
	virtual Vector UpdateTargetPosition( CBaseEntity *pTarget )
	{
		return pTarget->BodyTarget( pev->origin );
	}
	virtual int BulletCount( void );

	void StartRotSound( void );
	void StopRotSound( void );

	// Bmodels don't go across transitions
	virtual int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	inline bool IsActive( void ) const { return ( pev->spawnflags & SF_TANK_ACTIVE ) != 0; }
	inline void TankActivate( void )
	{
		pev->spawnflags |= SF_TANK_ACTIVE;
		pev->nextthink = pev->ltime + 0.1;
		m_fireLast = 0;
	}
	inline void TankDeactivate( void )
	{
		pev->spawnflags &= ~SF_TANK_ACTIVE;
		m_fireLast = 0;
		StopRotSound();
	}
	inline bool CanFire( void ) const { return ( gpGlobals->time - m_lastSightTime ) < m_persist; }
	bool InRange( float range );

	void Look( int distance );
	CBaseEntity *FindTarget( void );
	bool PointAt( const Vector &destination );

	void TankTrace( const Vector &vecStart, const Vector &vecForward, const Vector &vecSpread, TraceResult &tr );

	Vector BarrelPosition( void )
	{
		Vector forward, right, up;

		UTIL_MakeVectorsPrivate( pev->angles, forward, right, up );
		return pev->origin + ( forward * m_barrelPos.x ) + ( right * m_barrelPos.y ) + ( up * m_barrelPos.z );
	}

	void AdjustAnglesForBarrel( Vector &angles, float distance );

	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	BOOL OnControls( entvars_t *pevTest );
	bool StartControl( CBasePlayer *pController );
	void StopControl( void );
	void ControllerPostFrame( void );
	void MonsterStartup( CBaseMonster *controller );
	void MonsterStopControl( void );
	const CBasePlayer *GetController( void );

protected:
	friend class CBaseMonster;
	friend class CAdvFuncVehicle;

	template <class T>
	friend void RegisterSCScriptBaseTankEntity( CASDocumentation *documentation, const char *name );

	EHANDLE			m_hTarget;
	float			m_flUseRadius;
	EHANDLE			m_hMonsterController;
	bool			m_bMonsterControl;
	EHANDLE			m_hUseProxy;

	EHANDLE			m_hController;
	float			m_flNextAttack;
	Vector			m_vecControllerUsePos;

	float			m_yawCenter;    // "Center" yaw
	float			m_yawRate;      // Max turn rate to track targets
	float			m_yawRange;     // Range of turning motion (one-sided: 30 is +/- 30 degress from center)
	                      // Zero is full rotation
	float			m_yawTolerance; // Tolerance angle

	float			m_pitchCenter;    // "Center" pitch
	float			m_pitchRate;      // Max turn rate on pitch
	float			m_pitchRange;     // Range of pitch motion as above
	float			m_pitchTolerance; // Tolerance angle

	float			m_fireLast;      // Last time I fired
	float			m_fireRate;      // How many rounds/second
	float			m_lastSightTime; // Last time I saw target
	float			m_persist;       // Persistence of firing (how long do I shoot when I can't see)
	float			m_minRange;      // Minimum range to aim/track
	float			m_maxRange;      // Max range to aim/track

	Vector			m_barrelPos;  // Length of the freakin barrel
	float			m_spriteScale; // Scale of any sprites we shoot
	int				m_iszSpriteSmoke;
	int				m_iszSpriteFlash;
	TANKBULLET		m_bulletType; // Bullet type
	int				m_iBulletDamage;     // 0 means use Bullet type's default damage

	Vector			m_sightOrigin; // Last sight of target
	int				m_spread;         // firing spread
	int				m_iszMaster;      // Master entity (game_team_master or multisource)

	int				m_iRelationships[14];
};

#endif
