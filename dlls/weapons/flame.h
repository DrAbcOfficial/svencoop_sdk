#ifndef FLAME_H
#define FLAME_H

class CFlame : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT FlameThink( void );
	void EXPORT FlameTouch( CBaseEntity *other );
	bool DoDamage( CBaseEntity *other );

	static CFlame *Create( const Vector &origin, const Vector &velocity, float damage,
	                       CBaseEntity *owner, float frameRate );

private:
	float m_flLastDamageTime;
};

#endif
