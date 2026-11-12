#ifndef C_ADV_FUNC_VEHICLE_H
#define C_ADV_FUNC_VEHICLE_H

class CBasePlayer;

struct PlayerSlot
{
	PlayerSlot();

	edict_t    *m_pUser;
	Vector     *m_pData;
	PlayerSlot *m_pNext;
	float       m_flEnterTime;
	float       m_flExitTime;
	edict_t *m_pTank;
	int      m_iControls;
	int      m_iIndex;
};

struct VehicleAttachmentEntity
{
	EHANDLE                  m_hEntity;
	VehicleAttachmentEntity *m_pNext;
};

struct AnythingSlot
{
	AnythingSlot();

	VehicleAttachmentEntity *m_pEntities;
	Vector                  *m_pData;
	AnythingSlot            *m_pNext;
	int                      m_iFlags;
	unsigned int             m_Unresolved10;
	unsigned int             m_Unresolved14;
};

struct TireSlot
{
	TireSlot();

	edict_t      *m_pEntity;
	Vector       *m_pData;
	TireSlot     *m_pNext;
	unsigned int  m_Unresolved0C;
	unsigned int  m_Unresolved10;
};

class CAdvFuncVehicle : public CBaseEntity
{
public:
	CAdvFuncVehicle();

	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *key );
	void Touch( CBaseEntity *other );
	void Blocked( CBaseEntity *other );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void TraceAttack( entvars_t *attacker, float damage, Vector direction, TraceResult *trace, int damageType );
	int TakeDamage( entvars_t *inflictor, entvars_t *attacker, float damage, int damageType );
	bool TakeHealth( float health, int damageType, int healthCap = 0 );
	bool TakeArmor( float armor, int damageType, int armorCap = 0 );
	int ObjectCaps( void ) { return FCAP_CONTINUOUS_USE; }

	void EXPORT RealSpawn( void );
	void EXPORT VehicleThink( void );
	float GetAcceleration( void );
	float GetDeceleration( bool braking );
	float GetTurnSpeed( bool alternate );
	void ControlMovement( void );
	void DirectSteer( void );
	void ControlSteer( void );
	bool CanRepairVehicle( CBaseEntity *entity );
	CBasePlayer *GetDriver( CBasePlayer *player = NULL );
	bool CanExit( PlayerSlot *slot );
	bool CanUse( void *slot );
	PlayerSlot *FindFreeSlot( CBasePlayer *player );
	void PlayerEnter( CBasePlayer *player );
	void PlayerExit( CBasePlayer *player, bool force );
	void PlayerUse( CBasePlayer *player, int useType, int buttons );
	void PlayerSwitchSlot( CBasePlayer *player );
	void SendUserExitMessage( edict_t *player );
	void SendUserSlotMessage( PlayerSlot *slot );
	void MakeRelativeWorld( Vector *world, Vector *relative );
	bool PlayerNearSlot( Vector *position, Vector *relative );
	void PlaceAttachements( AnythingSlot *slot );
	void PlaceTank( PlayerSlot *slot );
	void PlaceTire( TireSlot *slot );
	void PlaceUser( PlayerSlot *slot );
	void PlaceAll( void );
	void AddPlayerSlot( PlayerSlot *slot );
	void RegisterSlot( entvars_t *slot );

	int Save( CSave &save );
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	bool          m_bSpawned;
	float         m_flMass;
	float         m_flPower;
	float         m_flBreakPower;
	float         m_flCurrentForwardSpeed;
	float         m_flCurrentSideSpeed;
	float         m_flCurrentTurnSpeed;
	unsigned int  m_Unresolved160;

	float m_flForwardSpeed;
	float m_flBackwardSpeed;
	float m_flSideSpeed;
	float m_flUpSpeed;
	float m_flDownSpeed;
	float m_flRotSpeed;
	float m_flArmor;
	float m_flFloorHeight;
	float m_flFloatHeight;
	float m_flHealth;

	TireSlot     *m_pTires;
	PlayerSlot   *m_pPlayerSlots;
	AnythingSlot *m_pAttachments;
	int           m_iButtons;

	Vector m_Unresolved19C;
	Vector m_Unresolved1A8;
};

#endif
