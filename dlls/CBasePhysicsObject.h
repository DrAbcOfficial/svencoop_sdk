#ifndef CBASEPHYSICSOBJECT_H
#define CBASEPHYSICSOBJECT_H

class CBasePhysicsObject : public CBaseAnimating
{
public:
	void Spawn( void );
	void OnCreate( void );

	virtual void WorldInit( void );
	virtual void Materialize( void );
	void EXPORT CallMaterialize( void ) { Materialize(); }
	virtual void TouchedWorld( bool firstTouch );
	virtual void EXPORT Kill( void );
	void EXPORT CallKill( void ) { Kill(); }
	virtual const char *GetBounceSound( void );
	virtual void FallInit( void );

	void EXPORT FallThink( void );
	void EXPORT DefaultTouch( CBaseEntity *other );

protected:
	float m_flNextBounce;
	int   m_iBounceCount;
	bool  m_fTouchedWorld;
	float m_flWaterSpeed;
	int   m_iBounceMaterial;
};

#endif
