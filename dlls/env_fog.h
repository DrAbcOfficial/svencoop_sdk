#ifndef ENV_FOG_H
#define ENV_FOG_H

class CEnvFog : public CBaseEntity
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void Think( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void OnCreate( void );
	void OnDestroy( void );
	void UpdatePlayer( CBaseEntity *player );
	bool IsActive( void ) const { return m_fActive; }

private:
	int		m_iID;
	bool	m_fActive;
	bool	m_fLastActive;

	Vector	m_vecLastOrigin;
	int		m_iLastRadius;
	Vector	m_vecLastColor;
	int		m_iLastStartDist;
	int		m_iLastEndDist;
};

#endif
