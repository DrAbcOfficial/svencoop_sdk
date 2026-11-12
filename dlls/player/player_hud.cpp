#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "player/player_hud.h"
#include "server/user_messages.h"
#include <float.h>

#define HUD_CHANNELS_PER_GROUP	16
#define HUD_MAX_RECT_SIZE		512

int HudChannel( int group, unsigned char channel )
{
	if ( (unsigned int) group > 1 )
	{
		ALERT( at_warning, "HUD: Invalid channel group #%d", group );
		return -1;
	}

	if ( channel >= HUD_CHANNELS_PER_GROUP )
	{
		ALERT( at_warning, "HUD: Invalid channel #%d", channel );
		return -1;
	}

	return group * HUD_CHANNELS_PER_GROUP + channel;
}

void HudWriteFloat( float value )
{
	unsigned int bits;

	memcpy( &bits, &value, sizeof( bits ) );
	WRITE_BYTE( bits & 255 );
	WRITE_BYTE( ( bits >> 8 ) & 255 );
	WRITE_BYTE( ( bits >> 16 ) & 255 );
	WRITE_BYTE( bits >> 24 );
}

void HudWriteFloat( float value, float minimum, float maximum )
{
	float result = value < minimum ? minimum : value;

	if ( value > maximum )
		result = maximum;

	HudWriteFloat( result );
}

void HudWriteColor( const RGBA &color )
{
	WRITE_BYTE( color.r );
	WRITE_BYTE( color.g );
	WRITE_BYTE( color.b );
	WRITE_BYTE( color.a );
}

void HudWriteRectangle( unsigned char left, unsigned char top, short width, short height )
{
	WRITE_BYTE( left );
	WRITE_BYTE( top );
	WRITE_SHORT( width < 0 ? 0 : width > HUD_MAX_RECT_SIZE ? HUD_MAX_RECT_SIZE : width );
	WRITE_SHORT( height < 0 ? 0 : height > HUD_MAX_RECT_SIZE ? HUD_MAX_RECT_SIZE : height );
}

void HudWriteEffect( float fadein, float fadeout, float hold, float effectTime, unsigned char effect )
{
	HudWriteFloat( fadein, 0, FLT_MAX );
	HudWriteFloat( fadeout, 0, FLT_MAX );
	HudWriteFloat( hold, -1, FLT_MAX );
	HudWriteFloat( effectTime, 0, FLT_MAX );
	WRITE_BYTE( effect );
}

void UTIL_PrintKeyBindingString( CBasePlayer *player, const char *text )
{
	if ( !player )
		return;

	MESSAGE_BEGIN( MSG_ONE, gmsgKeyBindingString, NULL, player->edict() );
	WRITE_STRING( text );
	MESSAGE_END();
}

void UTIL_PrintKeyBindingStringAll( const char *text )
{
	for ( int index = 1; index <= gpGlobals->maxClients; index++ )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( index ) );

		if ( player )
			UTIL_PrintKeyBindingString( player, text );
	}
}

void UTIL_ConcussionEffect( CBaseEntity *entity, float amplitude, float frequency, float fadeTime )
{
	if ( !entity || !entity->IsNetClient() )
		return;

	float clampedAmplitude = amplitude >= 0 ? amplitude : 0;
	float clampedFrequency = frequency >= 0.001f ? frequency : 0.001f;
	float clampedFade = fadeTime >= 0 ? fadeTime : 0;

	if ( !( amplitude <= 180 ) )
		clampedAmplitude = 180;

	if ( !( frequency <= 1000 ) )
		clampedFrequency = 1000;

	if ( !( fadeTime <= 3600 ) )
		clampedFade = 3600;

	MESSAGE_BEGIN( MSG_ONE, gmsgConcuss, NULL, entity->edict() );
	HudWriteFloat( clampedAmplitude );
	HudWriteFloat( clampedFrequency );
	HudWriteFloat( clampedFade );
	MESSAGE_END();
}

void UTIL_HudToggleElement( CBaseEntity *entity, int group, unsigned char channel, bool visible )
{
	if ( entity && !entity->IsNetClient() )
		return;

	int wireChannel = HudChannel( group, channel );

	if ( wireChannel < 0 )
		return;

	MESSAGE_BEGIN( entity ? MSG_ONE : MSG_ALL, gmsgToggleElem, NULL, entity ? entity->edict() : NULL );
	WRITE_BYTE( wireChannel );
	WRITE_BYTE( visible );
	MESSAGE_END();
}

void UTIL_HudCustomSprite( CBaseEntity *entity, int group, const hudspriteparams_t &params )
{
	if ( entity && !entity->IsNetClient() )
		return;

	int channel = HudChannel( group, params.channel );

	if ( channel < 0 )
		return;

	MESSAGE_BEGIN( entity ? MSG_ONE : MSG_ALL, gmsgCustomSprite, NULL, entity ? entity->edict() : NULL );
	WRITE_BYTE( channel );
	WRITE_LONG( params.flags );
	WRITE_STRING( STRING( params.spritename ) );
	HudWriteRectangle( params.left, params.top, params.width, params.height );
	HudWriteFloat( params.x );
	HudWriteFloat( params.y );
	HudWriteColor( params.color1 );
	HudWriteColor( params.color2 );
	WRITE_BYTE( params.frame );
	WRITE_BYTE( params.numframes );
	HudWriteFloat( params.framerate, 0, FLT_MAX );
	HudWriteEffect( params.fadeinTime, params.fadeoutTime, params.holdTime, params.fxTime, params.effect );
	MESSAGE_END();
}

void HudNumberDisplay( CBaseEntity *entity, int group, const hudnumdisplayparams_t &params, bool timeDisplay )
{
	if ( entity && !entity->IsNetClient() )
		return;

	int channel = HudChannel( group, params.channel );

	if ( channel < 0 )
		return;

	MESSAGE_BEGIN( entity ? MSG_ONE : MSG_ALL, timeDisplay ? gmsgTimeDisplay : gmsgNumDisplay,
	               NULL, entity ? entity->edict() : NULL );
	WRITE_BYTE( channel );
	WRITE_LONG( params.flags );

	if ( timeDisplay )
		HudWriteFloat( gpGlobals->time );

	HudWriteFloat( params.value );

	if ( !timeDisplay )
	{
		WRITE_BYTE( params.defdigits );
		WRITE_BYTE( params.maxdigits );
	}

	HudWriteFloat( params.x );
	HudWriteFloat( params.y );
	HudWriteColor( params.color1 );
	HudWriteColor( params.color2 );
	WRITE_STRING( STRING( params.spritename ) );
	HudWriteRectangle( params.left, params.top, params.width, params.height );
	HudWriteEffect( params.fadeinTime, params.fadeoutTime, params.holdTime, params.fxTime, params.effect );
	MESSAGE_END();
}

void UTIL_HudNumDisplay( CBaseEntity *entity, int group, const hudnumdisplayparams_t &params )
{
	HudNumberDisplay( entity, group, params, false );
}

void UTIL_HudTimeDisplay( CBaseEntity *entity, int group, const hudnumdisplayparams_t &params )
{
	HudNumberDisplay( entity, group, params, true );
}

void HudUpdateValue( CBaseEntity *entity, int group, unsigned char channel, float value, bool timeDisplay )
{
	if ( entity && !entity->IsNetClient() )
		return;

	int wireChannel = HudChannel( group, channel );

	if ( wireChannel < 0 )
		return;

	MESSAGE_BEGIN( entity ? MSG_ONE : MSG_ALL, timeDisplay ? gmsgUpdateTime : gmsgUpdateNum,
	               NULL, entity ? entity->edict() : NULL );
	WRITE_BYTE( wireChannel );

	if ( timeDisplay )
		HudWriteFloat( gpGlobals->time );

	HudWriteFloat( value );
	MESSAGE_END();
}

void UTIL_HudUpdateNum( CBaseEntity *entity, int group, unsigned char channel, float value )
{
	HudUpdateValue( entity, group, channel, value, false );
}

void UTIL_HudUpdateTime( CBaseEntity *entity, int group, unsigned char channel, float value )
{
	HudUpdateValue( entity, group, channel, value, true );
}
