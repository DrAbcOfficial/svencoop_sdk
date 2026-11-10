#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weather.h"

#define SF_WEATHER_START_ON 1

static unsigned short g_usNextWeatherID;

float WeatherMinimum( const char *text, float minimum )
{
	double value = atof( text );

	return (float) ( value > minimum ? value : minimum );
}

LINK_ENTITY_TO_CLASS( env_weather, CWeather );

void CWeather::Spawn( void )
{
	Precache();
	SetModel_Intercept( edict(), STRING( pev->model ) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	m_usWeatherID = g_usNextWeatherID++;

	if ( pev->spawnflags & SF_WEATHER_START_ON )
	{
		m_bActive = true;
		SetThink( &CWeather::MessageThink );
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		SetThink( NULL );
		m_bActive = false;
	}
}

void CWeather::Precache( void )
{
	m_iDropModelIndex = UTIL_PrecacheModel( this, STRING( m_iszDropModel ) );

	if ( m_iszSplashModel )
		m_iSplashModelIndex = UTIL_PrecacheModel( this, STRING( m_iszSplashModel ) );
}

void CWeather::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	switch ( useType )
	{
	case USE_OFF:
		m_bActive = false;
		break;

	case USE_ON:
		m_bActive = true;
		break;

	case USE_TOGGLE:
		m_bActive = !m_bActive;
		break;

	default:
		break;
	}

	if ( m_bActive )
	{
		SetThink( &CWeather::MessageThink );
		pev->nextthink = gpGlobals->time;
	}
	else
		SetThink( NULL );
}

void CWeather::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "drop_model" ) )
	{
		m_iszDropModel = AllocStringLocal( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}

	// The drop_model branch also reaches the inherited handler
	if ( FStrEq( pkvd->szKeyName, "drop_scale" ) )
		m_flDropScale = WeatherMinimum( pkvd->szValue, 0.01f );
	else if ( FStrEq( pkvd->szKeyName, "drop_vp_type" ) )
		m_iDropVPType = atoi( pkvd->szValue );
	else if ( FStrEq( pkvd->szKeyName, "splash_model" ) )
		m_iszSplashModel = AllocStringLocal( pkvd->szValue );
	else if ( FStrEq( pkvd->szKeyName, "splash_scale" ) )
		m_flSplashScale = WeatherMinimum( pkvd->szValue, 0.01f );
	else if ( FStrEq( pkvd->szKeyName, "splash_vp_type" ) )
		m_iSplashVPType = atoi( pkvd->szValue );
	else if ( FStrEq( pkvd->szKeyName, "density" ) )
		m_flDensity = WeatherMinimum( pkvd->szValue, 0.01f );
	else if ( FStrEq( pkvd->szKeyName, "angle_variance" ) )
		m_flAngleVariance = WeatherMinimum( pkvd->szValue, 0.0f );
	else if ( FStrEq( pkvd->szKeyName, "speed_variance" ) )
		m_flSpeedVariance = WeatherMinimum( pkvd->szValue, 0.0f );
	else
	{
		CBaseEntity::KeyValue( pkvd );
		return;
	}

	pkvd->fHandled = TRUE;
}

void CWeather::MessageThink( void )
{
}
