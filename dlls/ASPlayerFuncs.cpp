#include "extdll.h"
#include "util.h"
#include "ASPlayerFuncs.h"
#include "AdminLevel.h"
#include "CASPlayerFuncs.h"
#include "CASDocumentation.h"
#include "player_hud.h"

void ConstructHUDTextParams( hudtextparms_t *params )
{
	params->x = -1;
	params->y = -1;
	params->effect = 0;
	params->r1 = 100;
	params->g1 = 100;
	params->b1 = 100;
	params->r2 = 240;
	params->g2 = 110;
	params->b2 = 0;
	params->a2 = 0;
	params->fadeinTime = 1.5f;
	params->fadeoutTime = 0.5f;
	params->holdTime = 1.2f;
	params->fxTime = 0.25f;
	params->channel = 1;
}

void ConstructHUDSpriteParams( hudspriteparams_t *params )
{
	memset( params, 0, 18 );
	params->x = -1;
	params->y = -1;
	params->color1 = RGBA();
	params->color2 = RGBA();
	params->frame = 0;
	params->numframes = 1;
	params->framerate = 10;
	params->fadeinTime = 0;
	params->fadeoutTime = 0;
	params->holdTime = -1;
	params->fxTime = 0;
	params->effect = 0;
}

void ConstructHUDNumDisplayParams( hudnumdisplayparams_t *params )
{
	memset( params, 0, 14 );
	params->x = -1;
	params->y = -1;
	params->color1 = RGBA();
	params->color2 = RGBA();
	params->spritename = 0;
	params->left = 0;
	params->top = 0;
	params->width = 0;
	params->height = 0;
	params->fadeinTime = 0;
	params->fadeoutTime = 0;
	params->holdTime = -1;
	params->fxTime = 0;
	params->effect = 0;
}

void CopyConstructHUDTextParams( const hudtextparms_t &other, hudtextparms_t *params )
{
	memcpy( params, &other, sizeof( *params ) );
}

void DestructHUDTextParams( hudtextparms_t *params )
{
}

hudtextparms_t &AssignHUDTextParams( const hudtextparms_t &other, hudtextparms_t *params )
{
	memcpy( params, &other, sizeof( *params ) );
	return *params;
}

void RegisterSCScriptHUDTextParams( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Parameters for text output to the HUD.", "HUDTextParams", sizeof( hudtextparms_t ), asOBJ_VALUE );
	documentation->RegisterObjectBehaviour( "Constructor", "HUDTextParams", asBEHAVE_CONSTRUCT,
	                                        "void HUDTextParams()", asFUNCTION( ConstructHUDTextParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectBehaviour( "Copy constructor", "HUDTextParams", asBEHAVE_CONSTRUCT,
	                                        "void HUDTextParams(const HUDTextParams& in other)",
	                                        asFUNCTION( CopyConstructHUDTextParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectBehaviour( "Destructor", "HUDTextParams", asBEHAVE_DESTRUCT,
	                                        "void DestructHUDTextParams()", asFUNCTION( DestructHUDTextParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Assignment operator", "HUDTextParams",
	                                     "HUDTextParams& opAssign(const HUDTextParams& in other)",
	                                     asFUNCTION( AssignHUDTextParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty( "Horizontal position on the screen.\n<0, 1.0> = left to right\n(-1.0, 0) = right to left\n-1.0 = centered", "HUDTextParams", "float x",
	                                       asOFFSET( hudtextparms_t, x ) );
	documentation->RegisterObjectProperty( "Vertical position on the screen.\n<0, 1.0> = top to bottom\n(-1.0, 0) = bottom to top\n-1.0 = centered", "HUDTextParams", "float y",
	                                       asOFFSET( hudtextparms_t, y ) );
	documentation->RegisterObjectProperty( "Effects.\n0 : Fade In/Out\n1 : Credits\n2 : Scan Out", "HUDTextParams", "int effect",
	                                       asOFFSET( hudtextparms_t, effect ) );
	documentation->RegisterObjectProperty( "Red 1", "HUDTextParams", "uint8 r1",
	                                       asOFFSET( hudtextparms_t, r1 ) );
	documentation->RegisterObjectProperty( "Green 1", "HUDTextParams", "uint8 g1",
	                                       asOFFSET( hudtextparms_t, g1 ) );
	documentation->RegisterObjectProperty( "Blue 1", "HUDTextParams", "uint8 b1",
	                                       asOFFSET( hudtextparms_t, b1 ) );
	documentation->RegisterObjectProperty( "Alpha 1", "HUDTextParams", "uint8 a1",
	                                       asOFFSET( hudtextparms_t, a1 ) );
	documentation->RegisterObjectProperty( "Red 2", "HUDTextParams", "uint8 r2",
	                                       asOFFSET( hudtextparms_t, r2 ) );
	documentation->RegisterObjectProperty( "Green 2", "HUDTextParams", "uint8 g2",
	                                       asOFFSET( hudtextparms_t, g2 ) );
	documentation->RegisterObjectProperty( "Blue 2", "HUDTextParams", "uint8 b2",
	                                       asOFFSET( hudtextparms_t, b2 ) );
	documentation->RegisterObjectProperty( "Alpha 2", "HUDTextParams", "uint8 a2",
	                                       asOFFSET( hudtextparms_t, a2 ) );
	documentation->RegisterObjectProperty( "Fade in time", "HUDTextParams", "float fadeinTime",
	                                       asOFFSET( hudtextparms_t, fadeinTime ) );
	documentation->RegisterObjectProperty( "Fade out time", "HUDTextParams", "float fadeoutTime",
	                                       asOFFSET( hudtextparms_t, fadeoutTime ) );
	documentation->RegisterObjectProperty( "Hold time", "HUDTextParams", "float holdTime",
	                                       asOFFSET( hudtextparms_t, holdTime ) );
	documentation->RegisterObjectProperty( "Effect time (scan effect only)", "HUDTextParams", "float fxTime",
	                                       asOFFSET( hudtextparms_t, fxTime ) );
	documentation->RegisterObjectProperty( "Channel. 1-4.", "HUDTextParams", "int channel",
	                                       asOFFSET( hudtextparms_t, channel ) );
}

void CopyConstructHUDSpriteParams( const hudspriteparams_t &other, hudspriteparams_t *params )
{
	memcpy( params, &other, sizeof( *params ) );
}

void DestructHUDSpriteParams( hudspriteparams_t *params )
{
}

hudspriteparams_t &AssignHUDSpriteParams( const hudspriteparams_t &other, hudspriteparams_t *params )
{
	memcpy( params, &other, sizeof( *params ) );
	return *params;
}

void RegisterSCScriptHUDSpriteParams( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Holds parameters for custom sprite.", "HUDSpriteParams", sizeof( hudspriteparams_t ), asOBJ_VALUE );
	documentation->RegisterObjectBehaviour( "Constructor", "HUDSpriteParams", asBEHAVE_CONSTRUCT,
	                                        "void HUDSpriteParams()", asFUNCTION( ConstructHUDSpriteParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectBehaviour( "Copy constructor", "HUDSpriteParams", asBEHAVE_CONSTRUCT,
	                                        "void HUDSpriteParams(const HUDSpriteParams& in other)",
	                                        asFUNCTION( CopyConstructHUDSpriteParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectBehaviour( "Destructor", "HUDSpriteParams", asBEHAVE_DESTRUCT,
	                                        "void DestructHUDTextParams()", asFUNCTION( DestructHUDSpriteParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Assignment operator", "HUDSpriteParams",
	                                     "HUDSpriteParams& opAssign(const HUDSpriteParams& in other)",
	                                     asFUNCTION( AssignHUDSpriteParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty( "Channel.\nRange: 0-15 (each module type has its own channel group).", "HUDSpriteParams", "uint8 channel",
	                                       asOFFSET( hudspriteparams_t, channel ) );
	documentation->RegisterObjectProperty( "Flags\nSee HUD_ELEM and HUD_SPR enums.", "HUDSpriteParams", "int flags",
	                                       asOFFSET( hudspriteparams_t, flags ) );
	documentation->RegisterObjectProperty( "Sprite name", "HUDSpriteParams", "string_t spritename",
	                                       asOFFSET( hudspriteparams_t, spritename ) );
	documentation->RegisterObjectProperty( "Sprite left offset\nRange: 0-255", "HUDSpriteParams", "uint8 left",
	                                       asOFFSET( hudspriteparams_t, left ) );
	documentation->RegisterObjectProperty( "Sprite top offset\nRange: 0-255", "HUDSpriteParams", "uint8 top",
	                                       asOFFSET( hudspriteparams_t, top ) );
	documentation->RegisterObjectProperty( "Sprite width\nRange: 0-512 (0: auto; use total width of the sprite)", "HUDSpriteParams", "int16 width",
	                                       asOFFSET( hudspriteparams_t, width ) );
	documentation->RegisterObjectProperty( "Sprite height\nRange: 0-512 (0: auto; use total height of the sprite)", "HUDSpriteParams", "int16 height",
	                                       asOFFSET( hudspriteparams_t, height ) );
	documentation->RegisterObjectProperty( "Horizontal position on the screen.\n<0, 1.0> = left to right\n(-1.0, 0) = right to left\n-1.0 = centered", "HUDSpriteParams", "float x",
	                                       asOFFSET( hudspriteparams_t, x ) );
	documentation->RegisterObjectProperty( "Vertical position on the screen.\n<0, 1.0> = top to bottom\n(-1.0, 0) = bottom to top\n-1.0 = centered", "HUDSpriteParams", "float y",
	                                       asOFFSET( hudspriteparams_t, y ) );
	documentation->RegisterObjectProperty( "Color 1", "HUDSpriteParams", "RGBA color1",
	                                       asOFFSET( hudspriteparams_t, color1 ) );
	documentation->RegisterObjectProperty( "Color 2", "HUDSpriteParams", "RGBA color2",
	                                       asOFFSET( hudspriteparams_t, color2 ) );
	documentation->RegisterObjectProperty( "Frame", "HUDSpriteParams", "uint8 frame",
	                                       asOFFSET( hudspriteparams_t, frame ) );
	documentation->RegisterObjectProperty( "Number of frames", "HUDSpriteParams", "uint8 numframes",
	                                       asOFFSET( hudspriteparams_t, numframes ) );
	documentation->RegisterObjectProperty( "Framerate", "HUDSpriteParams", "float framerate",
	                                       asOFFSET( hudspriteparams_t, framerate ) );
	documentation->RegisterObjectProperty( "Fade in time", "HUDSpriteParams", "float fadeinTime",
	                                       asOFFSET( hudspriteparams_t, fadeinTime ) );
	documentation->RegisterObjectProperty( "Fade out time", "HUDSpriteParams", "float fadeoutTime",
	                                       asOFFSET( hudspriteparams_t, fadeoutTime ) );
	documentation->RegisterObjectProperty( "Hold time", "HUDSpriteParams", "float holdTime",
	                                       asOFFSET( hudspriteparams_t, holdTime ) );
	documentation->RegisterObjectProperty( "Effect time", "HUDSpriteParams", "float fxTime",
	                                       asOFFSET( hudspriteparams_t, fxTime ) );
	documentation->RegisterObjectProperty( "Effect\nSee HUD_EFFECT enum.", "HUDSpriteParams", "uint8 effect",
	                                       asOFFSET( hudspriteparams_t, effect ) );
}

void CopyConstructHUDNumDisplayParams( const hudnumdisplayparams_t &other, hudnumdisplayparams_t *params )
{
	memcpy( params, &other, sizeof( *params ) );
}

void DestructHUDNumDisplayParams( hudnumdisplayparams_t *params )
{
}

hudnumdisplayparams_t &AssignHUDNumDisplayParams( const hudnumdisplayparams_t &other, hudnumdisplayparams_t *params )
{
	memcpy( params, &other, sizeof( *params ) );
	return *params;
}

void RegisterSCScriptHUDNumDisplayParams( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Holds parameters for custom numeric/time display.", "HUDNumDisplayParams", sizeof( hudnumdisplayparams_t ), asOBJ_VALUE );
	documentation->RegisterObjectBehaviour( "Constructor", "HUDNumDisplayParams", asBEHAVE_CONSTRUCT,
	                                        "void HUDNumDisplayParams()", asFUNCTION( ConstructHUDNumDisplayParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectBehaviour( "Copy constructor", "HUDNumDisplayParams", asBEHAVE_CONSTRUCT,
	                                        "void HUDNumDisplayParams(const HUDNumDisplayParams& in other)",
	                                        asFUNCTION( CopyConstructHUDNumDisplayParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectBehaviour( "Destructor", "HUDNumDisplayParams", asBEHAVE_DESTRUCT,
	                                        "void DestructHUDNumDisplayParams()", asFUNCTION( DestructHUDNumDisplayParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Assignment operator", "HUDNumDisplayParams",
	                                     "HUDNumDisplayParams& opAssign(const HUDNumDisplayParams& in other)",
	                                     asFUNCTION( AssignHUDNumDisplayParams ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectProperty( "Channel.\nRange: 0-15 (each module type has its own channel group).", "HUDNumDisplayParams", "uint8 channel",
	                                       asOFFSET( hudnumdisplayparams_t, channel ) );
	documentation->RegisterObjectProperty( "Flags\nSee HUD_ELEM, HUD_NUM (numeric display only) and HUD_TIME (time display only) enums.", "HUDNumDisplayParams", "int flags",
	                                       asOFFSET( hudnumdisplayparams_t, flags ) );
	documentation->RegisterObjectProperty( "Value", "HUDNumDisplayParams", "float value",
	                                       asOFFSET( hudnumdisplayparams_t, value ) );
	documentation->RegisterObjectProperty( "Default number of digits (numeric display only)", "HUDNumDisplayParams", "uint8 defdigits",
	                                       asOFFSET( hudnumdisplayparams_t, defdigits ) );
	documentation->RegisterObjectProperty( "Maximum number of digits (numeric display only)", "HUDNumDisplayParams", "uint8 maxdigits",
	                                       asOFFSET( hudnumdisplayparams_t, maxdigits ) );
	documentation->RegisterObjectProperty( "Horizontal position on the screen.\n<0, 1.0> = left to right\n(-1.0, 0) = right to left\n-1.0 = centered", "HUDNumDisplayParams", "float x",
	                                       asOFFSET( hudnumdisplayparams_t, x ) );
	documentation->RegisterObjectProperty( "Vertical position on the screen.\n<0, 1.0> = top to bottom\n(-1.0, 0) = bottom to top\n-1.0 = centered", "HUDNumDisplayParams", "float y",
	                                       asOFFSET( hudnumdisplayparams_t, y ) );
	documentation->RegisterObjectProperty( "Color 1", "HUDNumDisplayParams", "RGBA color1",
	                                       asOFFSET( hudnumdisplayparams_t, color1 ) );
	documentation->RegisterObjectProperty( "Color 2", "HUDNumDisplayParams", "RGBA color2",
	                                       asOFFSET( hudnumdisplayparams_t, color2 ) );
	documentation->RegisterObjectProperty( "Fade in time", "HUDNumDisplayParams", "float fadeinTime",
	                                       asOFFSET( hudnumdisplayparams_t, fadeinTime ) );
	documentation->RegisterObjectProperty( "Fade out time", "HUDNumDisplayParams", "float fadeoutTime",
	                                       asOFFSET( hudnumdisplayparams_t, fadeoutTime ) );
	documentation->RegisterObjectProperty( "Hold time", "HUDNumDisplayParams", "float holdTime",
	                                       asOFFSET( hudnumdisplayparams_t, holdTime ) );
	documentation->RegisterObjectProperty( "Effect time", "HUDNumDisplayParams", "float fxTime",
	                                       asOFFSET( hudnumdisplayparams_t, fxTime ) );
	documentation->RegisterObjectProperty( "Effect\nSee HUD_EFFECT enum.", "HUDNumDisplayParams", "uint8 effect",
	                                       asOFFSET( hudnumdisplayparams_t, effect ) );
	documentation->RegisterObjectProperty( "Sprite name", "HUDNumDisplayParams", "string_t spritename",
	                                       asOFFSET( hudnumdisplayparams_t, spritename ) );
	documentation->RegisterObjectProperty( "Sprite left offset\nRange: 0-255", "HUDNumDisplayParams", "uint8 left",
	                                       asOFFSET( hudnumdisplayparams_t, left ) );
	documentation->RegisterObjectProperty( "Sprite top offset\nRange: 0-255", "HUDNumDisplayParams", "uint8 top",
	                                       asOFFSET( hudnumdisplayparams_t, top ) );
	documentation->RegisterObjectProperty( "Sprite width\nRange: 0-512 (0: auto; use total width of the sprite)", "HUDNumDisplayParams", "int16 width",
	                                       asOFFSET( hudnumdisplayparams_t, width ) );
	documentation->RegisterObjectProperty( "Sprite height\nRange: 0-512 (0: auto; use total height of the sprite)", "HUDNumDisplayParams", "int16 height",
	                                       asOFFSET( hudnumdisplayparams_t, height ) );
}

void RegisterSCScriptAdminLevels( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Admin levels", "AdminLevel_t" );
	documentation->RegisterEnumValue( "Level on connect, tells functions not to use cached level",
	                                  "AdminLevel_t", "ADMIN_INIT", ADMIN_INIT );
	documentation->RegisterEnumValue( "Not an administrator", "AdminLevel_t", "ADMIN_NO", ADMIN_NO );
	documentation->RegisterEnumValue( "Server administrator", "AdminLevel_t", "ADMIN_YES", ADMIN_YES );
	documentation->RegisterEnumValue( "Server owner (applies to a listenserver host player)",
	                                  "AdminLevel_t", "ADMIN_OWNER", ADMIN_OWNER );
}

void RegisterSCScriptPlayerFuncs( CASDocumentation *documentation )
{
	RegisterSCScriptAdminLevels( documentation );

	documentation->RegisterEnum( "Fade flags for CPlayerFuncs::ScreenFade and ScreenFadeAll", "FFADE" );
	documentation->RegisterEnumValue( "Just here so we don't pass 0 into the function", "FFADE", "FFADE_IN", 0 );
	documentation->RegisterEnumValue( "Fade out (not in)", "FFADE", "FFADE_OUT", 1 );
	documentation->RegisterEnumValue( "Modulate (don't blend)", "FFADE", "FFADE_MODULATE", 2 );
	documentation->RegisterEnumValue( "ignores the duration, stays faded out until new ScreenFade message received", "FFADE", "FFADE_STAYOUT", 4 );

	documentation->RegisterEnum( "Used to dictate where client print commands print the given text.", "HUD" );
	documentation->RegisterEnumValue( "Print notify", "HUD", "HUD_PRINTNOTIFY", 1 );
	documentation->RegisterEnumValue( "Print to console", "HUD", "HUD_PRINTCONSOLE", 2 );
	documentation->RegisterEnumValue( "Print to chat", "HUD", "HUD_PRINTTALK", 3 );
	documentation->RegisterEnumValue( "Print to center of screen", "HUD", "HUD_PRINTCENTER", 4 );
	RegisterSCScriptHUDTextParams( documentation );
	RegisterSCScriptHUDSpriteParams( documentation );
	RegisterSCScriptHUDNumDisplayParams( documentation );

	documentation->RegisterEnum( "HUD element effects.", "HUD_EFFECT" );
	documentation->RegisterEnumValue( "No effect.", "HUD_EFFECT", "HUD_EFFECT_NONE", 0 );
	documentation->RegisterEnumValue( "Linear ramp up from color1 to color2.", "HUD_EFFECT", "HUD_EFFECT_RAMP_UP", 1 );
	documentation->RegisterEnumValue( "Linear ramp down from color2 to color1.", "HUD_EFFECT", "HUD_EFFECT_RAMP_DOWN", 2 );
	documentation->RegisterEnumValue( "Linear ramp up and ramp down from color1 through color2 back to color1.", "HUD_EFFECT", "HUD_EFFECT_TRIANGLE", 3 );
	documentation->RegisterEnumValue( "Cosine ramp up from color1 to color2.", "HUD_EFFECT", "HUD_EFFECT_COSINE_UP", 4 );
	documentation->RegisterEnumValue( "Cosine ramp down from color2 to color1.", "HUD_EFFECT", "HUD_EFFECT_COSINE_DOWN", 5 );
	documentation->RegisterEnumValue( "Cosine ramp up and ramp down from color1 through color2 back to color1.", "HUD_EFFECT", "HUD_EFFECT_COSINE", 6 );
	documentation->RegisterEnumValue( "Toggle between color1 and color2.", "HUD_EFFECT", "HUD_EFFECT_TOGGLE", 7 );
	documentation->RegisterEnumValue( "Sine pulse from color1 through zero to color2.", "HUD_EFFECT", "HUD_EFFECT_SINE_PULSE", 8 );

	documentation->RegisterEnum( "Common flags used by all custom HUD elements.", "HUD_ELEM" );
	documentation->RegisterEnumValue( "X position in pixels.", "HUD_ELEM", "HUD_ELEM_ABSOLUTE_X", 1 );
	documentation->RegisterEnumValue( "Y position in pixels.", "HUD_ELEM", "HUD_ELEM_ABSOLUTE_Y", 2 );
	documentation->RegisterEnumValue( "X position relative to the center of the screen.", "HUD_ELEM", "HUD_ELEM_SCR_CENTER_X", 4 );
	documentation->RegisterEnumValue( "Y position relative to the center of the screen.", "HUD_ELEM", "HUD_ELEM_SCR_CENTER_Y", 8 );
	documentation->RegisterEnumValue( "Ignore the client-side HUD border (hud_bordersize).", "HUD_ELEM", "HUD_ELEM_NO_BORDER", 16 );
	documentation->RegisterEnumValue( "Create a hidden element.", "HUD_ELEM", "HUD_ELEM_HIDDEN", 32 );
	documentation->RegisterEnumValue( "Play the effect only once.", "HUD_ELEM", "HUD_ELEM_EFFECT_ONCE", 64 );
	documentation->RegisterEnumValue( "Use the default client-side HUD alpha (hud_defaultalpha).", "HUD_ELEM", "HUD_ELEM_DEFAULT_ALPHA", 128 );
	documentation->RegisterEnumValue( "Use the default client-side HUD alpha and flash the element when updated.", "HUD_ELEM", "HUD_ELEM_DYNAMIC_ALPHA", 256 );

	documentation->RegisterEnum( "Flags used by custom HUD sprites.", "HUD_SPR" );
	documentation->RegisterEnumValue( "Draw opaque sprite.", "HUD_SPR", "HUD_SPR_OPAQUE", 0x10000 );
	documentation->RegisterEnumValue( "Draw masked sprite.", "HUD_SPR", "HUD_SPR_MASKED", 0x20000 );
	documentation->RegisterEnumValue( "Play the animation only once.", "HUD_SPR", "HUD_SPR_PLAY_ONCE", 0x40000 );
	documentation->RegisterEnumValue( "Hide the sprite when the animation stops.", "HUD_SPR", "HUD_SPR_HIDE_WHEN_STOPPED", 0x80000 );

	documentation->RegisterEnum( "Flags used by custom numeric displays.", "HUD_NUM" );
	documentation->RegisterEnumValue( "Draw right aligned element.", "HUD_NUM", "HUD_NUM_RIGHT_ALIGN", 0x10000 );
	documentation->RegisterEnumValue( "Draw separator.", "HUD_NUM", "HUD_NUM_SEPARATOR", 0x20000 );
	documentation->RegisterEnumValue( "Hide the element if the value is zero.", "HUD_NUM", "HUD_NUM_DONT_DRAW_ZERO", 0x40000 );
	documentation->RegisterEnumValue( "Draw leading zeros.", "HUD_NUM", "HUD_NUM_LEADING_ZEROS", 0x80000 );
	documentation->RegisterEnumValue( "Allow negative values.", "HUD_NUM", "HUD_NUM_NEGATIVE_NUMBERS", 0x100000 );
	documentation->RegisterEnumValue( "Draw sign for positive values.", "HUD_NUM", "HUD_NUM_PLUS_SIGN", 0x200000 );

	documentation->RegisterEnum( "Flags used by custom time displays.", "HUD_TIME" );
	documentation->RegisterEnumValue( "Draw right aligned element.", "HUD_TIME", "HUD_TIME_RIGHT_ALIGN", 0x10000 );
	documentation->RegisterEnumValue( "Draw hours.", "HUD_TIME", "HUD_TIME_HOURS", 0x20000 );
	documentation->RegisterEnumValue( "Draw minutes.", "HUD_TIME", "HUD_TIME_MINUTES", 0x40000 );
	documentation->RegisterEnumValue( "Draw seconds.", "HUD_TIME", "HUD_TIME_SECONDS", 0x80000 );
	documentation->RegisterEnumValue( "Draw milliseconds.", "HUD_TIME", "HUD_TIME_MILLISECONDS", 0x100000 );
	documentation->RegisterEnumValue( "Draw hours even if the value is zero.", "HUD_TIME", "HUD_TIME_ZERO_HOURS", 0x200000 );
	documentation->RegisterEnumValue( "Freeze the displayed value.", "HUD_TIME", "HUD_TIME_FREEZE", 0x400000 );
	documentation->RegisterEnumValue( "Count down.", "HUD_TIME", "HUD_TIME_COUNT_DOWN", 0x800000 );

	documentation->RegisterEnum( "Flags used in CBasePlayer m_iHideHUD", "HideHUDFlags" );
	documentation->RegisterEnumValue( "Hides Weapon Selection Menu and Crosshair elements", "HideHUDFlags", "HIDEHUD_WEAPONS", 1 );
	documentation->RegisterEnumValue( "Hides the Flashlight element", "HideHUDFlags", "HIDEHUD_FLASHLIGHT", 2 );
	documentation->RegisterEnumValue( "Hides all HUD elements", "HideHUDFlags", "HIDEHUD_ALL", 4 );
	documentation->RegisterEnumValue( "Hides the Health element", "HideHUDFlags", "HIDEHUD_HEALTH", 8 );
	documentation->RegisterEnumValue( "Hides the Weapon Selection Menu element", "HideHUDFlags", "HIDEHUD_WEAPON_SELECT", 16 );
	documentation->RegisterEnumValue( "Hides the Battery(armor) element", "HideHUDFlags", "HIDEHUD_SUITPOWER", 32 );
	documentation->RegisterEnumValue( "Hides Custom HUD elements from map scripts", "HideHUDFlags", "HIDEHUD_CUSTOM_MAPS", 64 );
	documentation->RegisterEnumValue( "Hides Custom HUD elements from plugins", "HideHUDFlags", "HIDEHUD_CUSTOM_PLUGINS", 128 );
	documentation->RegisterEnumValue( "Hides only the Crosshair element", "HideHUDFlags", "HIDEHUD_CROSSHAIR", 256 );
	documentation->RegisterEnumValue( "Hides only the Ammo element", "HideHUDFlags", "HIDEHUD_AMMO", 512 );

	documentation->RegisterObjectType( "Global player functions", "CPlayerFuncs", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod( "Says text to a specific player", "CPlayerFuncs",
	                                     "void SayText(CBasePlayer@ pTargetPlayer, const string& in szText)",
	                                     asMETHOD( CASPlayerFuncs, SayText ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Says text to all players", "CPlayerFuncs",
	                                     "void SayTextAll(CBasePlayer@ pOriginatingPlayer, const string& in szText)",
	                                     asMETHOD( CASPlayerFuncs, SayTextAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Prints one or more messages on the HUD of the given player.\nSee HUD enum.", "CPlayerFuncs",
	                                     "void ClientPrint(CBasePlayer@ pTargetPlayer, HUD iMsgDest, const string& in szMessage, const string& in szLine2 = \"\", const string& in szLine3 = \"\", const string& in szLine4 = \"\", const string& in szLine5 = \"\")",
	                                     asMETHOD( CASPlayerFuncs, ClientPrint ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Prints one or more messages on the HUD of all players.\nSee HUD enum.", "CPlayerFuncs",
	                                     "void ClientPrintAll(HUD iMsgDest, const string& in szMessage, const string& in szLine2 = \"\", const string& in szLine3 = \"\", const string& in szLine4 = \"\", const string& in szLine5 = \"\")",
	                                     asMETHOD( CASPlayerFuncs, ClientPrintAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Prints one or more messages centered on the HUD of all players.", "CPlayerFuncs",
	                                     "void CenterPrintAll(const string& in szMessage, const string& in szLine2 = \"\", const string& in szLine3 = \"\", const string& in szLine4 = \"\", const string& in szLine5 = \"\")",
	                                     asMETHOD( CASPlayerFuncs, CenterPrintAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a message to the given player.", "CPlayerFuncs",
	                                     "void ShowMessage(CBasePlayer@ pTargetPlayer, const string& in szString)",
	                                     asMETHOD( CASPlayerFuncs, ShowMessage ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a message to all players.", "CPlayerFuncs",
	                                     "void ShowMessageAll(const string& in szString)",
	                                     asMETHOD( CASPlayerFuncs, ShowMessageAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a HUD message to a given player.", "CPlayerFuncs",
	                                     "void HudMessage(CBasePlayer@ pTargetPlayer, const HUDTextParams& in textParams, const string& in szMessage)",
	                                     asMETHOD( CASPlayerFuncs, HudMessage ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a HUD message to all players.", "CPlayerFuncs",
	                                     "void HudMessageAll(const HUDTextParams& in textParams, const string& in szMessage)",
	                                     asMETHOD( CASPlayerFuncs, HudMessageAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Prints a string containing key bindings to the screen of the given player.", "CPlayerFuncs",
	                                     "void PrintKeyBindingString(CBasePlayer@ pPlayer, const string& in szString)",
	                                     asMETHOD( CASPlayerFuncs, PrintKeyBindingString ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Prints a string containing key bindings to the screen of all players.", "CPlayerFuncs",
	                                     "void PrintKeyBindingStringAll(const string& in szString)",
	                                     asMETHOD( CASPlayerFuncs, PrintKeyBindingStringAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows or hides HUD element occupying a given channel.", "CPlayerFuncs",
	                                     "void HudToggleElement(CBasePlayer@ pTargetPlayer, uint8 iChannel, bool fVisible)",
	                                     asMETHOD( CASPlayerFuncs, HudToggleElement ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a custom HUD sprite to a given player or to all players if pTargetPlayer is not specified.", "CPlayerFuncs",
	                                     "void HudCustomSprite(CBasePlayer@ pTargetPlayer, const HUDSpriteParams& in params)",
	                                     asMETHOD( CASPlayerFuncs, HudCustomSprite ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a custom numeric display to a given player or to all players if pTargetPlayer is not specified.", "CPlayerFuncs",
	                                     "void HudNumDisplay(CBasePlayer@ pTargetPlayer, const HUDNumDisplayParams& in params)",
	                                     asMETHOD( CASPlayerFuncs, HudNumDisplay ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Updates value of a custom numberic display.", "CPlayerFuncs",
	                                     "void HudUpdateNum(CBasePlayer@ pTargetPlayer, uint8 iChannel, float flValue)",
	                                     asMETHOD( CASPlayerFuncs, HudUpdateNum ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a custom time display to a given player or to all players if pTargetPlayer is not specified.", "CPlayerFuncs",
	                                     "void HudTimeDisplay(CBasePlayer@ pTargetPlayer, const HUDNumDisplayParams& in params)",
	                                     asMETHOD( CASPlayerFuncs, HudTimeDisplay ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Updates value of a custom time display.", "CPlayerFuncs",
	                                     "void HudUpdateTime(CBasePlayer@ pTargetPlayer, uint8 iChannel, float flTime)",
	                                     asMETHOD( CASPlayerFuncs, HudUpdateTime ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Finds a player by index", "CPlayerFuncs",
	                                     "CBasePlayer@ FindPlayerByIndex(int index)",
	                                     asMETHOD( CASPlayerFuncs, FindPlayerByIndex ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Finds a player by name", "CPlayerFuncs",
	                                     "CBasePlayer@ FindPlayerByName(const string& in szName, bool bCaseSensitive = true)",
	                                     asMETHOD( CASPlayerFuncs, FindPlayerByName ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the number of players that are currently connected to the server.", "CPlayerFuncs",
	                                     "int GetNumPlayers() const",
	                                     asMETHOD( CASPlayerFuncs, GetNumPlayers ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shakes the screen for players near a certain location", "CPlayerFuncs",
	                                     "void ScreenShake(const Vector& in center, float amplitude, float frequency, float duration, float radius)",
	                                     asMETHOD( CASPlayerFuncs, ScreenShake ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shakes the screen for all players", "CPlayerFuncs",
	                                     "void ScreenShakeAll(const Vector& in center, float amplitude, float frequency, float duration)",
	                                     asMETHOD( CASPlayerFuncs, ScreenShakeAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a message to a given player.", "CPlayerFuncs",
	                                     "void ScreenFade(CBaseEntity@ pEntity, const Vector& in color, float fadeTime, float fadeHold, int alpha, int flags)",
	                                     asMETHOD( CASPlayerFuncs, ScreenFade ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Shows a message to all players.", "CPlayerFuncs",
	                                     "void ScreenFadeAll(const Vector& in color, float fadeTime, float fadeHold, int alpha, int flags)",
	                                     asMETHOD( CASPlayerFuncs, ScreenFadeAll ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Applies concussion effect to a given player.", "CPlayerFuncs",
	                                     "void ConcussionEffect(CBaseEntity@ pEntity, float amplitude, float frequency, float fadeTime)",
	                                     asMETHOD( CASPlayerFuncs, ConcussionEffect ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Makes the given player select the next best weapon available, based on the current weapon.", "CPlayerFuncs",
	                                     "bool GetNextBestWeapon(CBasePlayer@ pPlayer, CBasePlayerItem@ pCurrentWeapon)",
	                                     asMETHOD( CASPlayerFuncs, GetNextBestWeapon ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether cheats are enabled for the given player.", "CPlayerFuncs",
	                                     "bool CheatsAllowed(CBasePlayer@ pPlayer, const string& in szCheatName, bool fMustBeAlive = true, bool fNoMessage = false) const",
	                                     asMETHOD( CASPlayerFuncs, CheatsAllowed ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns the admin level for a given player.", "CPlayerFuncs",
	                                     "AdminLevel_t AdminLevel(CBasePlayer@ pPlayer) const",
	                                     asMETHOD( CASPlayerFuncs, AdminLevel ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Converts the admin level to a string. If the level is an invalid value, an empty string is returned", "CPlayerFuncs",
	                                     "string AdminLevelToString(const AdminLevel_t adminLevel) const",
	                                     asMETHOD( CASPlayerFuncs, AdminLevelToString ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Converts a string containing one of AdminLevelToString's return values to an admin level.Returns ADMIN_NO if the string cannot be converted to a suitable constant", "CPlayerFuncs",
	                                     "AdminLevel_t StringToAdminLevel(const string& in szString) const",
	                                     asMETHOD( CASPlayerFuncs, StringToAdminLevel ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Applies the map configuration on the given player.\nWill stack if called multiple times, unless fReEquip is true, in which case the player is stripped of all weapons and ammo first.", "CPlayerFuncs",
	                                     "void ApplyMapCfgToPlayer(CBasePlayer@ pPlayer, bool fReEquip = false)",
	                                     asMETHOD( CASPlayerFuncs, ApplyMapCfgToPlayer ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Relocates the given player to an active spawn point. If the player is dead and fRespawnDeadPlayers is true, the player is respawned.", "CPlayerFuncs",
	                                     "void RespawnPlayer(CBasePlayer@ pPlayer, bool fMoveLivingPlayers = true, bool fRespawnDeadPlayers = false)",
	                                     asMETHOD( CASPlayerFuncs, RespawnPlayer ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Relocates all players to active spawn points. If a player is dead and fRespawnDeadPlayers is true, the player is respawned.", "CPlayerFuncs",
	                                     "void RespawnAllPlayers(bool fMoveLivingPlayers = true, bool fRespawnDeadPlayers = false)",
	                                     asMETHOD( CASPlayerFuncs, RespawnAllPlayers ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether the given player passes the given spawn point's filter", "CPlayerFuncs",
	                                     "bool SpawnPointFilterPasses(CBaseEntity@ pSpawnEnt, CBaseEntity@ pPlayer)",
	                                     asMETHOD( CASPlayerFuncs, SpawnPointFilterPasses ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether the given spawn point is valid for the given player", "CPlayerFuncs",
	                                     "bool IsSpawnPointValid(CBaseEntity@ pSpawnEnt, CBaseEntity@ pPlayer)",
	                                     asMETHOD( CASPlayerFuncs, IsSpawnPointValid ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether the given spawn point entity is occupied", "CPlayerFuncs",
	                                     "bool IsSpawnPointOccupied(CBaseEntity@ pSpawnEnt)",
	                                     asMETHOD( CASPlayerFuncs, IsSpawnPointOccupied ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets a random long whose value is the same on both the client and server.", "CPlayerFuncs",
	                                     "int SharedRandomLong(uint iRandomSeed, int iLow, int iHigh)",
	                                     asMETHOD( CASPlayerFuncs, SharedRandomLong ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets a random float whose value is the same on both the client and server.", "CPlayerFuncs",
	                                     "float SharedRandomFloat(uint iRandomSeed, float iLow, float iHigh)",
	                                     asMETHOD( CASPlayerFuncs, SharedRandomFloat ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the index of the given ammo, or -1 if it is invalid", "CPlayerFuncs",
	                                     "int GetAmmoIndex(const string& in szAmmoName)",
	                                     asMETHOD( CASPlayerFuncs, GetAmmoIndex ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Creates a bot with the given name.", "CPlayerFuncs",
	                                     "CBasePlayer@ CreateBot(const string& in szName)",
	                                     asMETHOD( CASPlayerFuncs, CreateBot ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Disconnects the bot. This will call ClientDisconnect and removes the entity. Do not use the entity after this call.", "CPlayerFuncs",
	                                     " void BotDisconnect(CBasePlayer@ pBot)",
	                                     asMETHOD( CASPlayerFuncs, BotDisconnect ), asCALL_THISCALL );
	documentation->RegisterGlobalProperty( "Global player functions instance", "CPlayerFuncs g_PlayerFuncs",
	                                       CASPlayerFuncs::GetInstance() );
}
