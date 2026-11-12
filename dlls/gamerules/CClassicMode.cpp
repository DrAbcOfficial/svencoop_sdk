#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "gamerules/game.h"
#include "player/player.h"
#include "weapons/weapons.h"
#include "gamerules/CClassicMode.h"
#include "server/CCommand.h"
#include "server/CConCommand.h"
#include "server/CServer.h"
#include "scripting/core/CASLog.h"
#include "../../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"

#define CLASSIC_MODE_MAPINIT_PHASE 2

class CClassicMode::CPlayerItemInfo
{
public:
	CPlayerItemInfo() : m_hPlayer(), m_flClipRatio( 0 ) {}

	void Initialize( CBasePlayer *player );
	void RestoreActiveItem( void );
	bool HasPlayer( void )
	{
		edict_t *player = m_hPlayer.Get();

		return player && !player->free && GET_PRIVATE( player ) != NULL;
	}

private:
	EHANDLE m_hPlayer;
	CString m_szMappedActiveItem;
	float   m_flClipRatio;
};

void CClassicMode::CPlayerItemInfo::Initialize( CBasePlayer *player )
{
	m_hPlayer = NULL;

	if ( !player || !player->pev )
		return;

	edict_t *playerEdict = player->edict();

	if ( !playerEdict || playerEdict->free )
		return;

	m_hPlayer.Set( playerEdict );

	if ( !HasPlayer() )
		return;

	player = (CBasePlayer *) (CBaseEntity *) m_hPlayer;

	if ( !player->m_pActiveItem )
		return;

	CBasePlayerItem *item = player->m_pActiveItem;

	if ( item->edict()->free || CBaseEntity::Instance( item->edict() ) != item )
		return;

	m_szMappedActiveItem = CClassicMode::GetInstance()->FindMappedItem( STRING( item->pev->classname ) );

	if ( m_szMappedActiveItem.IsEmpty() )
		return;

	CBasePlayerWeapon *weapon = (CBasePlayerWeapon *) item->GetWeaponPtr();

	if ( weapon )
		m_flClipRatio = (float) weapon->m_iClip / weapon->iMaxClip();
}

void CClassicMode::CPlayerItemInfo::RestoreActiveItem( void )
{
	if ( !HasPlayer() || m_szMappedActiveItem.IsEmpty() )
		return;

	CBasePlayer *player = (CBasePlayer *) (CBaseEntity *) m_hPlayer;

	player->SelectItem( m_szMappedActiveItem.Data() );

	CBasePlayerItem *item = player->m_pActiveItem;

	if ( !item || item->edict()->free || CBaseEntity::Instance( item->edict() ) != item )
		return;

	if ( m_szMappedActiveItem != STRING( item->pev->classname ) )
		return;

	CBasePlayerWeapon *weapon = (CBasePlayerWeapon *) item->GetWeaponPtr();

	if ( weapon )
		weapon->m_iClip = (int) ( weapon->iMaxClip() * m_flClipRatio );
}

CClassicMode::CClassicMode()
	: m_pItemMappings( NULL ), m_iState( -1 ), m_iNextState( -1 )
{
}

CClassicMode::~CClassicMode()
{
}

void CClassicMode::Initialize( void )
{
	m_bMapSupport = false;
	m_bShouldRestartOnChange = true;
}

void CClassicMode::Shutdown( void )
{
	if ( m_pItemMappings )
		m_pItemMappings->Release();

	m_pItemMappings = NULL;
}

void CClassicMode::PreMapInit( void )
{
	if ( m_iNextState == -1 )
		return;

	m_iState = m_iNextState;
	CVAR_SET_FLOAT( mp_classic_mode.name, (float) m_iState );
	m_iNextState = -1;
}

void CClassicMode::PostMapInit( bool success )
{
	if ( !m_bMapSupport )
		ResetState();
}

void CClassicMode::PostMapActivate( bool success )
{
	if ( IsEnabled() )
		UpdateWorld( false );
}

bool CClassicMode::IsStateDefined( void ) const
{
	return mp_classic_mode.value != -1.0f;
}

void CClassicMode::ResetState( void )
{
	m_iState = -1;
	CVAR_SET_FLOAT( mp_classic_mode.name, -1 );
}

bool CClassicMode::IsEnabled( void ) const
{
	return m_iState == 1;
}

void CClassicMode::ForceItemRemap( bool enabled )
{
	m_bForceItemRemap = enabled;
}

void CClassicMode::SetShouldRestartOnChange( bool restart )
{
	m_bShouldRestartOnChange = restart;
}

bool CClassicMode::ShouldRestartOnChange( void ) const
{
	return m_bShouldRestartOnChange;
}

void CClassicMode::Toggle( void )
{
	if ( !m_bMapSupport || m_iNextState != -1 )
		return;

	const char *stateName = IsEnabled() ? "Disabled" : "Enabled";
	const char *message;

	if ( m_bShouldRestartOnChange )
	{
		message = UTIL_VarArgs( "Classic Mode %s. Restarting map in %d seconds.\n", stateName, 5 );
		CServer::GetInstance()->m_flRestartTime = gpGlobals->time;
	}
	else
		message = UTIL_VarArgs( "Classic Mode %s. The change will be applied on the next map change.\n", stateName );

	UTIL_ClientPrintAll( HUD_PRINTTALK, message );
	m_iNextState = IsEnabled() ? 0 : 1;
}

void CClassicMode::SetEnabled( bool enabled )
{
	if ( !m_bMapSupport || m_iNextState != -1 )
		return;

	if ( IsEnabled() != enabled )
	{
		Toggle();
		return;
	}

	if ( !IsStateDefined() )
	{
		m_iState = IsEnabled();
		CVAR_SET_FLOAT( mp_classic_mode.name, (float) m_iState );
	}
}

void CClassicMode::Think( void )
{
	if ( (float) m_iState == mp_classic_mode.value )
		return;

	int state = (int) mp_classic_mode.value;

	mp_classic_mode.value = (float) m_iState;
	SetEnabled( state >= 1 );

	if ( state <= -1 )
		ResetState();
}

void CClassicMode::EnableMapSupport( void )
{
	if ( CASServerManager::GetInstance()->m_iPhase >= CLASSIC_MODE_MAPINIT_PHASE )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CClassicMode::EnableMapSupport: MapInit already ended!\n" );
		return;
	}

	m_bMapSupport = true;
}

void CClassicMode::SetItemMappings( CScriptArray *mappings )
{
	if ( m_pItemMappings )
		m_pItemMappings->Release();

	m_pItemMappings = NULL;

	if ( !mappings )
		return;

	asITypeInfo *type = CASServerManager::GetInstance()->m_pEngine->GetTypeInfoByDecl( "array<ItemMapping@>" );

	if ( mappings->GetArrayObjectType() != type )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CClassicMode::SetItemMappings: Invalid array type passed!\n" );
		mappings->Release();
		return;
	}

	m_pItemMappings = mappings;
}

CClassicMode::CItemMapping *CClassicMode::ItemMappingAt( unsigned int index ) const
{
	if ( !m_pItemMappings || index >= m_pItemMappings->GetSize() )
		return NULL;

	void *mapping = m_pItemMappings->At( index );

	return mapping ? *(CItemMapping **) mapping : NULL;
}

const CString &CClassicMode::FindMappedItem( const char *name ) const
{
	if ( !name || !*name || !m_pItemMappings )
		return CString::EMPTY_STRING;

	for ( unsigned int index = 0; index < m_pItemMappings->GetSize(); index++ )
	{
		CItemMapping *mapping = ItemMappingAt( index );

		if ( !mapping )
			return CString::EMPTY_STRING;

		if ( IsEnabled() || m_bForceItemRemap )
		{
			if ( mapping->GetFrom() == name )
				return mapping->GetTo();
		}
		else if ( mapping->GetTo() == name )
			return mapping->GetFrom();
	}

	return CString::EMPTY_STRING;
}

const CString &CClassicMode::FindMappedItem( const CString &name ) const
{
	return FindMappedItem( name.Data() );
}

void CClassicMode::UpdateWorld( bool includePlayers )
{
	if ( !m_pItemMappings )
		return;

	CPlayerItemInfo players[32];

	for ( int index = 1; index <= gpGlobals->maxClients; index++ )
	{
		CBasePlayer *player = (CBasePlayer *) UTIL_PlayerByIndex( index );

		if ( player )
			players[index - 1].Initialize( player );
	}

	for ( unsigned int index = 0; index < m_pItemMappings->GetSize(); index++ )
	{
		CItemMapping *mapping = ItemMappingAt( index );

		if ( !mapping )
			return;

		const CString &from = IsEnabled() ? mapping->GetFrom() : mapping->GetTo();
		const CString &to = IsEnabled() ? mapping->GetTo() : mapping->GetFrom();
		CBaseEntity *entity = NULL;

		while ( ( entity = UTIL_FindEntityByClassname( entity, from.Data() ) ) != NULL )
		{
			CBasePlayerItem *item = entity->MyItemPointer();

			if ( !item || ( item->m_hPlayer && !includePlayers ) )
				continue;

			Vector origin = item->pev->origin;
			Vector angles = item->pev->angles;
			Vector velocity = item->pev->velocity;
			string_t target = item->pev->target;
			string_t targetname = item->pev->targetname;
			const char *className = STRING( AllocStringLocal( to.Data() ) );
			CBaseEntity *replacement = CBaseEntity::Create( className, origin, angles, false, NULL );

			ASSERT( replacement != NULL );

			if ( replacement )
			{
				replacement->pev->velocity = velocity;
				replacement->pev->targetname = targetname;
				replacement->pev->target = target;
			}

			CBasePlayerItem *replacementItem = replacement->MyItemPointer();

			ASSERT( replacementItem != NULL );

			replacementItem->m_RespawnDelay = item->m_RespawnDelay;
			replacementItem->m_bIsNotAmmoItem = item->m_bIsNotAmmoItem;

			CBasePlayer *owner = item->m_hPlayer;

			if ( owner )
			{
				owner->RemovePlayerItem( item );
				replacementItem->Collect( owner, COLLECT_USE_INDIRECT );

				if ( !replacementItem->m_hPlayer )
					UTIL_Remove( replacementItem );
			}

			UTIL_Remove( item );
		}
	}

	for ( int index = 0; index < ARRAYSIZE( players ); index++ )
	{
		if ( players[index].HasPlayer() )
			players[index].RestoreActiveItem();
	}
}

CClassicMode::CItemMapping::CItemMapping( const CString &from, const CString &to )
	: m_From( from ), m_To( to )
{
}

const CString &CClassicMode::CItemMapping::GetFrom( void ) const
{
	return m_From;
}

const CString &CClassicMode::CItemMapping::GetTo( void ) const
{
	return m_To;
}

void CClassicMode::CItemMapping::Release( void ) const
{
	if ( InternalRelease() )
		delete this;
}

CClassicMode::CItemMapping *ConstructItemMapping( const CString &from, const CString &to )
{
	return new CClassicMode::CItemMapping( from, to );
}

void toggle_classic_mode_Callback( const CCommand &command )
{
	CClassicMode::GetInstance()->Toggle();
}

CConCommand toggle_classic_mode( "toggle_classic_mode", toggle_classic_mode_Callback, ConCommandFlag::AdminOnly, "Toggles Classic Mode" );
