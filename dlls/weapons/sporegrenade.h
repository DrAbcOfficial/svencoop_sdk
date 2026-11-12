#ifndef SPOREGRENADE_H
#define SPOREGRENADE_H

class CSporeGrenade : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void OnDestroy( void );
	int BloodColor( void ) { return DONT_BLEED; }
	void Killed( entvars_t *attacker, int gib );

	void Explode( TraceResult *trace, int damageType );
	void Explode( Vector source, Vector direction );
	void EXPORT Detonate( void );
	void EXPORT DetonateUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void EXPORT Smoke( void );
	void EXPORT TumbleThink( void );
	void EXPORT IgniteThink( void );
	void EXPORT DangerSoundThink( void );
	void EXPORT BounceTouch( CBaseEntity *other );
	void EXPORT ExplodeTouch( CBaseEntity *other );
	virtual void BounceSound( void );
	void SetClientEffectsEnabled( bool enabled );

	static CSporeGrenade *ShootTimed( entvars_t *owner, Vector start, Vector velocity, float time );
	static CSporeGrenade *ShootContact( entvars_t *owner, Vector start, Vector velocity, float damage );
	static CSporeGrenade *ShootStraight( entvars_t *owner, Vector start, Vector velocity, float damage );
	static CSporeGrenade *ShootLob( entvars_t *owner, Vector start, Vector velocity, bool randomize, float damage );

private:
	int		m_iSporeSprite;
	float	m_flBounceSoundTime;
	bool	m_fRegisteredSound;
	bool	m_fClientEffectsEnabled;
};

#endif
