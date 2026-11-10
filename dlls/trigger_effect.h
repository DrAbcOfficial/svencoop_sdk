#ifndef TRIGGER_EFFECT_H
#define TRIGGER_EFFECT_H

class CTriggerEffect : public CBaseDelay
{
public:
	CTriggerEffect();
	void Spawn( void );
	void KeyValue( KeyValueData *data );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	int ObjectCaps( void ) { return 0; }
	int Save( CSave &save );
	int Restore( CRestore &restore );
	void SetEffects( CBaseEntity *entity );

	static TYPEDESCRIPTION m_SaveData[];

private:
	int			m_iEffectGlowMode;
	Vector		m_vecEffectGlowColor;

	int			m_iEffectBlockWeaponsMode;
	bool		m_fEffectBlockWeapons;

	int			m_iEffectInvulnerableMode;
	bool		m_fEffectInvulnerable;

	int			m_iEffectInvisibleMode;
	bool		m_fEffectInvisible;

	int			m_iEffectNonSolidMode;
	bool		m_fEffectNonSolid;

	int			m_iEffectRespirationMode;
	float		m_flEffectRespiration;

	int			m_iEffectGravityMode;
	float		m_flEffectGravity;

	int			m_iEffectFrictionMode;
	float		m_flEffectFriction;

	int			m_iEffectSpeedMode;
	float		m_flEffectSpeed;

	int			m_iEffectDamageMode;
	float		m_flEffectDamage;
};

#endif
