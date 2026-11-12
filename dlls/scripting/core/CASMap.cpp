#include "extdll.h"
#include "util.h"
#include "server/server_support.h"
#include "scripting/core/CASMap.h"
#include "server/CMap.h"

CASMap::~CASMap()
{
}

bool CASMap::HasForcedPlayerModels( void ) const
{
	return CMap::GetInstance()->HasForcedPlayerModels();
}

bool CASMap::LoadMapSkillFile( void )
{
	return CMap::GetInstance()->LoadMapSkillFile();
}

bool CASMap::LoadGlobalSkillFile( void )
{
	return CMap::GetInstance()->LoadGlobalSkillFile();
}

bool CASMap::LoadSkillFile( const CString &fileName )
{
	if ( !fileName.Length() )
		return false;

	CMap *map = CMap::GetInstance();
	string_t pooledName = AllocStringLocal( fileName.Data() );

	return map->LoadSkillFile( STRING( pooledName ) );
}
