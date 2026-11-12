#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "player/observer.h"
#include "world/effects.h"
#include "server/user_messages.h"

#define MENU_FROM_ANGELSCRIPT 0x80 // ShowMenu "need more" byte

void CBasePlayer::SetVModelPos( const Vector &position )
{
	m_vecViewModelPos = position;
	m_bViewModelPos = true;
}

void CBasePlayer::ResetVModelPos( void )
{
	m_bViewModelPos = false;
}

void CBasePlayer::SendVModelPos( int mode, const Vector &position )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgVModelPos, NULL, pev );
		WRITE_BYTE( mode );

		if ( mode == 1 )
		{
			WRITE_COORD( position.x );
			WRITE_COORD( position.y );
			WRITE_COORD( position.z );
		}
	MESSAGE_END();
}

bool CBasePlayer::ShowOverheadSprite( const char *spriteName, float zOffset, float lifeTime )
{
	if ( !IsObserver() )
	{
		int model = PrecacheModel_Intercept( NULL, spriteName );

		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_PLAYERATTACHMENT );
			WRITE_BYTE( entindex() & 0xff );
			WRITE_COORD( zOffset );
			WRITE_SHORT( model );
			WRITE_SHORT( (int) ( lifeTime * 10.0f ) );
		MESSAGE_END();

		return true;
	}

	CBaseEntity *body = GetObserver()->GetDeadBody();

	if ( !body )
		return false;

	Vector position = body->pev->origin;

	position.z += zOffset;

	const char *model = CWorld::GetInstance()->MODELREPLACEMENT_Find( spriteName );
	CSprite *sprite = CSprite::SpriteCreate( model, position, false, 10.0f );

	sprite->LiveForTime( lifeTime );
	return true;
}

bool CBasePlayer::IntervalHUDMessage( int destination, const char *message )
{
	if ( !message || m_flLastHUDMessageTime + 1.0f > gpGlobals->time )
		return false;

	m_flLastHUDMessageTime = gpGlobals->time;
	ClientPrint( pev, destination, message );
	return true;
}

void CBasePlayer::ShowMenu( int validSlots, int displayTime, bool needMore, char *text )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pev );
		WRITE_SHORT( validSlots );
		WRITE_CHAR( displayTime );
		WRITE_BYTE( MENU_FROM_ANGELSCRIPT | (int) needMore );
		WRITE_STRING( text );
	MESSAGE_END();
}

void CBasePlayer::ShowMenu( int validSlots, int displayTime, char *text, int menuId, unsigned int page )
{
	m_bTextMenuInputReceived = false;

	if ( !text || !*text )
		return;

	unsigned int textLength = strlen( text );

	m_iTextMenuValidSlots = validSlots;
	m_iTextMenuId = menuId;
	m_uiTextMenuPage = page;
	m_flTextMenuExpireTime = displayTime <= 0 ? -1.0f : (float) displayTime + gpGlobals->time;

	UTIL_SendTextMenu( &pev, validSlots, displayTime, text, textLength );
}
