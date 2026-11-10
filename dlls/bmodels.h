#ifndef BMODELS_H
#define BMODELS_H

class CFuncWall : public CBaseEntity
{
public:
	void Spawn( void );
	void EXPORT MoveThink( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	// Bmodels don't go across transitions
	virtual int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};

class CFuncWallToggle : public CFuncWall
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void TurnOff( void );
	void TurnOn( void );
	BOOL IsOn( void );
};

#endif
