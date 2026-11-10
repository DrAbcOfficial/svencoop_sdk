#ifndef SQUEAKGRENADE_H
#define SQUEAKGRENADE_H

class CSqueakGrenade : public CGrenade
{
	friend class CSqkNest;
	friend class CSqueak;
	void Spawn( void );
	void Precache( void );
	int Classify( void );
	void EXPORT SuperBounceTouch( CBaseEntity *pOther );
	void EXPORT HuntThink( void );
	int BloodColor( void ) { return BLOOD_COLOR_YELLOW; }
	void Killed( entvars_t *pevAttacker, int iGib );
	void GibMonster( void );

	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	static float m_flNextBounceSoundTime;

	float   m_flDie;
	Vector  m_vecTarget;
	float   m_flNextHunt;
	float   m_flNextHit;
	Vector  m_posPrev;
	bool    m_bNestAlly;
	EHANDLE m_hOwner;
	int     m_iMyClass;

public:
	CBaseEntity *GetOriginalOwner( void ) { return m_hOwner; }
};

#endif
