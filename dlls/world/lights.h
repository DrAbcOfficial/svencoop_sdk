#ifndef LIGHTS_H
#define LIGHTS_H

class CLight : public CPointEntity
{
public:
	virtual void	KeyValue( KeyValueData* pkvd );
	virtual void	Spawn( void );
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

private:
	friend class CGlobalLightControl;

	int		m_iStyle;
	int		m_iszPattern;
};

#endif
