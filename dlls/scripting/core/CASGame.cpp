#include "extdll.h"
#include "SvenVersion.h"
#include "util.h"
#include "cbase.h"
#include "scripting/core/CASGame.h"
#include "scripting/api/ASString.h"
#include "server/server_support.h"

CASGame::~CASGame()
{
}

bool CASGame::AlertMessage( asIScriptGeneric *generic )
{
	ALERT_TYPE type = (ALERT_TYPE) generic->GetArgDWord( 0 );
	const CString &format = *(const CString *) generic->GetArgObject( 1 );
	char message[4096];
	bool result = ASSPrintf( message, sizeof( message ), format.Data(), 2, generic );

	if ( result )
		ALERT( type, "%s", message );

	return result;
}

int CASGame::PrecacheModel( const CString &fileName )
{
	string_t pooledName = AllocStringLocal( fileName.Data() );

	return PrecacheModel_Intercept( NULL, STRING( pooledName ) );
}

int CASGame::PrecacheModel( CBaseEntity *entity, const CString &fileName )
{
	string_t pooledName = AllocStringLocal( fileName.Data() );

	return PrecacheModel_Intercept( entity, STRING( pooledName ) );
}

void CASGame::PrecacheOther( const CString &className )
{
	string_t pooledName = AllocStringLocal( className.Data() );

	UTIL_PrecacheOther( STRING( pooledName ) );
}

void CASGame::PrecacheMonster( const CString &className, bool ally )
{
	string_t pooledName = AllocStringLocal( className.Data() );

	UTIL_PrecacheMonster( STRING( pooledName ), ally );
}

void CASGame::PrecacheGeneric( const CString &fileName )
{
	string_t pooledName = AllocStringLocal( fileName.Data() );

	PRECACHE_GENERIC( (char *) STRING( pooledName ) );
}

CString CASGame::GetGameName( void ) const
{
	return CString( "Sven Co-op" );
}

CString CASGame::GetGameVersionString( void ) const
{
	return CString( SVEN_GAME_VERSION_STRING );
}

unsigned int CASGame::GetGameVersion( void ) const
{
	return SVEN_GAME_VERSION_NUMBER;
}
