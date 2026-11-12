#ifndef SHOCK_H
#define SHOCK_H

class CShockEffect : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void Touch( CBaseEntity *other );
	void EXPORT IgniteThink( void );

	static void Shoot( entvars_t *owner, Vector origin, Vector velocity );

private:
	EHANDLE m_hBeams[2];
};

#endif
