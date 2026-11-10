#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "ASPlayerEntity.h"
#include "CASLog.h"
#include "server_support.h"

CBasePlayerItem *PlayerEntity_Get_m_rgpPlayerItems( unsigned int index, CBasePlayer *player )
{
	if ( index < MAX_ITEM_TYPES )
		return player->m_rgpPlayerItems[index];

	CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CBasePlayer::m_rgpPlayerItems: index is invalid!\n" );
	return NULL;
}

int PlayerEntity_Get_m_rgAmmo( unsigned int index, CBasePlayer *player )
{
	if ( index < MAX_AMMO_SLOTS )
		return player->m_rgAmmo[index];

	CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CBasePlayer::m_rgAmmo: index is invalid!\n" );
	return 0;
}

void PlayerEntity_Set_m_rgAmmo( unsigned int index, int value, CBasePlayer *player )
{
	if ( index < MAX_AMMO_SLOTS )
	{
		player->m_rgAmmo[index] = value;
		return;
	}

	CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CBasePlayer::m_rgAmmo: index is invalid!\n" );
}

CString CBasePlayer_GetAnimExtension( const CBasePlayer *player )
{
	return CString( player->m_szAnimExtention );
}

void CBasePlayer_SetAnimExtension( CBasePlayer *player, const CString &extension )
{
	UTIL_CopyString( player->m_szAnimExtention, extension.Data(), sizeof( player->m_szAnimExtention ) );
}

CBaseEntity *PlayerEntity_DropPlayerItem( const CString &name, CBasePlayer *player )
{
	return player->DropPlayerItem( (char *) name.Data() );
}

CBasePlayerItem *PlayerEntity_DropPlayerAmmo( const CString &name, CBasePlayer *player )
{
	return player->DropPlayerAmmo( name.Data(), false, false );
}

CBasePlayerItem *PlayerEntity_HasNamedPlayerItem( const CString &name, CBasePlayer *player )
{
	return player->FindItem( name.Data() );
}

void PlayerEntity_SelectItem( const CString &name, CBasePlayer *player )
{
	player->SelectItem( name.Data() );
}

void PlayerEntity_GiveNamedItem( const CString &name, int flags, int ammoCount, CBasePlayer *player )
{
	string_t stableName = AllocStringLocal( name.Data() );

	player->GiveNamedItem( STRING( stableName ), flags, ammoCount );
}

void PlayerEntity_SetSuitUpdate( const CString &name, bool group, int noRepeat, CBasePlayer *player )
{
	player->SetSuitUpdate( name.Data(), group, noRepeat );
}
