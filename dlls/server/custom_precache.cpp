#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "server/custom_precache.h"

LINK_ENTITY_TO_CLASS( custom_precache, CCustomPrecache );

CCustomPrecache::CCustomPrecache()
    : m_pModels( NULL ), m_pSounds( NULL ), m_pSkies( NULL ),
      m_iModelCount( 0 ), m_iSoundCount( 0 ), m_iSkyCount( 0 )
{
}

void CCustomPrecache::AddResource( string_t *&resources, int &count, string_t value )
{
	string_t *updated = new string_t[count + 1];

	for ( int index = 0; index < count; index++ )
		updated[index] = resources[index];

	updated[count] = value;
	delete[] resources;
	resources = updated;
	count++;
}

void CCustomPrecache::AddModel( string_t model )
{
	AddResource( m_pModels, m_iModelCount, model );
}

void CCustomPrecache::AddSound( string_t sound )
{
	AddResource( m_pSounds, m_iSoundCount, sound );
}

void CCustomPrecache::AddSky( string_t sky )
{
	AddResource( m_pSkies, m_iSkyCount, sky );
}

bool CCustomPrecache::ParseKeyValue( const char *name, const char *value )
{
	if ( !name || !value )
		return false;

	if ( strstr( name, "model_" ) )
		AddModel( AllocStringLocal( value ) );
	else if ( strstr( name, "sound_" ) )
		AddSound( AllocStringLocal( value ) );
	else if ( strstr( name, "sky_" ) )
		AddSky( AllocStringLocal( value ) );
	else
		return false;

	return true;
}

void CCustomPrecache::KeyValue( KeyValueData *pkvd )
{
	if ( ParseKeyValue( pkvd->szKeyName, pkvd->szValue ) )
		pkvd->fHandled = TRUE;

	// Forwarding is unconditional, including recognized resource keys
	CBaseEntity::KeyValue( pkvd );
}

void CCustomPrecache::FreePointers( void )
{
	delete[] m_pModels;
	delete[] m_pSounds;
	delete[] m_pSkies;
	m_pModels = NULL;
	m_pSounds = NULL;
	m_pSkies = NULL;
}

void CCustomPrecache::Spawn( void )
{
	Precache();
	FreePointers();
	UTIL_Remove( this );
}

void CCustomPrecache::Precache( void )
{
	for ( int index = 0; index < m_iModelCount; index++ )
	{
		ALERT( at_aiconsole, "Custom precache [model]: %s %d\n", STRING( m_pModels[index] ), index );
		UTIL_PrecacheModel( this, STRING( m_pModels[index] ) );
	}

	for ( int index = 0; index < m_iSoundCount; index++ )
	{
		ALERT( at_aiconsole, "Custom precache [sound]: %s %d\n", STRING( m_pSounds[index] ), index );
		PrecacheSound_Intercept( this, STRING( m_pSounds[index] ) );
	}

	for ( int index = 0; index < m_iSkyCount; index++ )
	{
		ALERT( at_aiconsole, "Custom precache [sky]: %s %d\n", STRING( m_pSkies[index] ), index );
		UTIL_PrecacheSkymap( STRING( m_pSkies[index] ) );
	}
}
