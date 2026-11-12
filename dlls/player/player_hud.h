#ifndef PLAYER_HUD_H
#define PLAYER_HUD_H

#include "utils/RGBA.h"

class CBaseEntity;
class CBasePlayer;

struct hudspriteparams_t
{
	unsigned char channel;
	int           flags;
	string_t      spritename;
	unsigned char left;
	unsigned char top;
	short         width;
	short         height;
	float         x;
	float         y;
	RGBA          color1;
	RGBA          color2;
	unsigned char frame;
	unsigned char numframes;
	float         framerate;
	float         fadeinTime;
	float         fadeoutTime;
	float         holdTime;
	float         fxTime;
	unsigned char effect;
};

struct hudnumdisplayparams_t
{
	unsigned char channel;
	int           flags;
	float         value;
	unsigned char defdigits;
	unsigned char maxdigits;
	float         x;
	float         y;
	RGBA          color1;
	RGBA          color2;
	string_t      spritename;
	unsigned char left;
	unsigned char top;
	short         width;
	short         height;
	float         fadeinTime;
	float         fadeoutTime;
	float         holdTime;
	float         fxTime;
	unsigned char effect;
};

void UTIL_PrintKeyBindingString( CBasePlayer *player, const char *text );
void UTIL_PrintKeyBindingStringAll( const char *text );
void UTIL_ConcussionEffect( CBaseEntity *entity, float amplitude, float frequency, float fadeTime );
void UTIL_HudToggleElement( CBaseEntity *entity, int group, unsigned char channel, bool visible );
void UTIL_HudCustomSprite( CBaseEntity *entity, int group, const hudspriteparams_t &params );
void UTIL_HudNumDisplay( CBaseEntity *entity, int group, const hudnumdisplayparams_t &params );
void UTIL_HudUpdateNum( CBaseEntity *entity, int group, unsigned char channel, float value );
void UTIL_HudTimeDisplay( CBaseEntity *entity, int group, const hudnumdisplayparams_t &params );
void UTIL_HudUpdateTime( CBaseEntity *entity, int group, unsigned char channel, float value );

#endif
