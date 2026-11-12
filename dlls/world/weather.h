#ifndef WEATHER_H
#define WEATHER_H

class CWeather : public CBaseEntity
{
public:
	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;

	void EXPORT MessageThink( void );

private:
	string_t		m_iszDropModel;
	int				m_iDropModelIndex;
	float			m_flDropScale;
	int				m_iDropVPType;
	string_t		m_iszSplashModel;
	int				m_iSplashModelIndex;
	float			m_flSplashScale;
	int				m_iSplashVPType;
	float			m_flDensity;
	float			m_flAngleVariance;
	float			m_flSpeedVariance;
	unsigned short	m_usWeatherID;
	bool			m_bActive;
};

#endif
