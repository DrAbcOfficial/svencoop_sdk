#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include "TypedEntityHandle.h"

class CSprite;

class CHyperCube : public CBaseEntity
{
public:
	CHyperCube();
	~CHyperCube();
	void Spawn( void );
	void Think( void );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void CreateBeam( int startEntity, int endEntity, Vector color );
	void Randomize( void );

private:
	float m_vecPosition[4];
	float m_vecVelocity[4];
	float m_flAngles[6];
	float m_flAngularVelocity[6];
	float m_flLastThinkTime;

	CEntityHandle<CSprite> m_hSprites[16];
};

#endif
