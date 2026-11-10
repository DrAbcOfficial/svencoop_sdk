#ifndef CHARGER_H
#define CHARGER_H

class CBasePlayer;

struct ChargerSounds
{
	const char *denied;
	const char *start;
	const char *loop;
};

class CBaseCharger : public CBaseToggle
{
public:
	CBaseCharger() : m_pszDeniedSound( NULL ), m_pszStartSound( NULL ), m_pszLoopSound( NULL ) {}

	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *data );
	int Save( CSave &save );
	int Restore( CRestore &restore );
	int ObjectCaps( void ) { return FCAP_CONTINUOUS_USE; }
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );

	void EXPORT Off( void );
	void EXPORT Recharge( void );
	void Recharge( bool fromSpawn );

	virtual bool ChargeDenied( CBasePlayer *player ) { return false; }
	virtual void Charge( CBasePlayer *player ) = 0;
	virtual int GetDefaultCapacity( void ) const = 0;
	virtual float GetRechargeTime( void ) const = 0;
	virtual ChargerSounds GetChargerSounds( void ) const = 0;

	static TYPEDESCRIPTION m_SaveData[];

protected:
	enum
	{
		STATE_RECHARGING = -1,
		STATE_OFF = 0,
		STATE_STARTING,
		STATE_CHARGING
	};

	int   m_iJuice;
	float m_flNextCharge;
	int   m_iReactivate;
	int   m_State;
	float m_flSoundTime;

	const char *m_pszDeniedSound;
	const char *m_pszStartSound;
	const char *m_pszLoopSound;

	int      m_iCustomJuice;
	int      m_iCustomRechargeTime;
	string_t m_iszTriggerOnEmpty;
	string_t m_iszTriggerOnRecharged;
};

class CFuncCharger : public CBaseCharger
{
public:
	bool ChargeDenied( CBasePlayer *player );
	void Charge( CBasePlayer *player );
	int GetDefaultCapacity( void ) const;
	float GetRechargeTime( void ) const;
	ChargerSounds GetChargerSounds( void ) const;
};

class CFuncHealthCharger : public CBaseCharger
{
public:
	bool ChargeDenied( CBasePlayer *player );
	void Charge( CBasePlayer *player );
	int GetDefaultCapacity( void ) const;
	float GetRechargeTime( void ) const;
	ChargerSounds GetChargerSounds( void ) const;
};

#endif
