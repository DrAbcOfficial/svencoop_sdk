#ifndef CLIENT_WEATHER_H
#define CLIENT_WEATHER_H

#include "clientobject.h"

struct tempent_s;

class CClientWeather : public CClientObject
{
public:
	virtual void Spawn(void);

	void Init(void);
	void TurnOn(void);
	void TurnOff(void);
	void WeatherThink(void);

	short	m_iWeatherID;
	Vector	m_vecMins;
	Vector	m_vecMaxs;
	Vector	m_vecAngles;
	int		m_iModel;
	float	m_flScale;
	int		m_iVPType;
	int		m_iSplashModel;
	float	m_flSplashScale;
	int		m_iSplashVPType;
	float	m_flRenderAmt;
	int		m_iRenderMode;
	int		m_iRenderFx;
	Vector	m_vecRenderColor;
	float	m_flFrequency;
	float	m_flSpeed;
	float	m_flAngleVariance;
	float	m_flSpeedVariance;
	Vector	m_vecCenter;
	Vector	m_vecHalfSize;
	float	m_flVolume;
};

void ClientWeatherSpriteHitCallback(tempent_s *pTemp, pmtrace_s *pTrace);
void TE_CreateWeatherEffect(int iWeatherID, Vector vecMins, Vector vecMaxs, Vector vecAngles,
	int iModel, float flScale, int iVPType, int iSplashModel, float flSplashScale, int iSplashVPType,
	int iRenderMode, float flRenderAmt, Vector vecRenderColor, int iRenderFx, float flFrequency,
	float flSpeed, float flAngleVariance, float flSpeedVariance);

#endif
