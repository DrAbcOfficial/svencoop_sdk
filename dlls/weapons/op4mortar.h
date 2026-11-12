#ifndef OP4MORTAR_H
#define OP4MORTAR_H

class COp4MortarController;

class COp4Mortar : public CBaseMonster
{
public:
	COp4Mortar();
	~COp4Mortar() override;

	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData *data ) override;
	int Classify( void ) override;
	int Save( CSave &save ) override;
	int Restore( CRestore &restore ) override;

	static TYPEDESCRIPTION m_SaveData[];

	void EXPORT TrackTarget( void );
	void EXPORT FireCannonUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );

private:
	friend class COp4MortarController;

	bool InRange( float distance );
	void Scan( int distance );
	CBaseEntity *FindTarget( void );
	void FireCannon( CBaseEntity *activator );
	void UpdateControllers( void );

	float m_flHorizontalCenter;
	float m_flHorizontalMax;
	float m_flHorizontalMin;
	float m_flMinDist;
	float m_flMaxDist;
	float m_flMortarVelocity;
	float m_flDelay;
	bool  m_bActive;

	Vector m_vecVelocity;
	Vector m_vecController;
	float  m_flNextFire;
	Vector m_vecIdealAngles;
};

class COp4MortarController : public CBaseToggle
{
public:
	COp4MortarController();
	~COp4MortarController() override;

	void Spawn( void ) override;
	void KeyValue( KeyValueData *data ) override;
	int ObjectCaps( void ) override;

	void EXPORT FollowThink( void );
	void EXPORT ControllerUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );

private:
	COp4Mortar *m_pMortar;
	bool        m_bStopped;
	float       m_flStopTime;
	int         m_iDirection;
	float       m_flNextMove;
	int         m_iAxis;
};

#endif
