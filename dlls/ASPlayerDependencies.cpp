#include "ASPlayerDependencies.h"
#include "ASObserver.h"
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "CString.h"
#include "CASDocumentation.h"
#include <stddef.h>

extern int TrainSpeed( int speed, int maximum );

static const size_t AS_MAX_AMMO_TYPES = 64;
static const size_t AS_MAX_ITEM_TYPES = 10;
static const float AS_AUTOAIM_2DEGREES = 0.03489949554204941f;
static const float AS_AUTOAIM_5DEGREES = 0.08715574443340302f;
static const float AS_AUTOAIM_8DEGREES = 0.13917310535907745f;
static const float AS_AUTOAIM_10DEGREES = 0.1736481785774231f;
static const float AS_PLAYERCOMMAND_WAIT = 4.0f;

int PlayerEntity_GetMaxAmmo( const CString &name, const CBasePlayer *player )
{
	return player->GetMaxAmmo( name.Data() );
}

void PlayerEntity_SetMaxAmmo( const CString &name, int maxAmmo, CBasePlayer *player )
{
	player->SetMaxAmmo( name.Data(), maxAmmo );
}

void PlayerEntity_ResetMaxAmmoToDefault( const CString &name, CBasePlayer *player )
{
	player->ResetMaxAmmoToDefault( name.Data() );
}

void PlayerEntity_RemoveExcessAmmo( const CString &name, CBasePlayer *player )
{
	player->RemoveExcessAmmo( name.Data() );
}

void PlayerEntity_ShowOverheadSprite( const CString &name, float zOffset, float lifeTime, CBasePlayer *player )
{
	string_t model = AllocStringLocal( name.Data() );

	player->ShowOverheadSprite( STRING( model ), zOffset, lifeTime );
}

void RegisterASPlayerAmmoMethods( CASDocumentation *documentation )
{
	documentation->RegisterObjectMethod( "", "CBasePlayer", "int GetMaxAmmo(size_t uiIndex) const",
		asMETHODPR( CBasePlayer, GetMaxAmmo, (unsigned int) const, int ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "int GetMaxAmmo(const string& in szName) const",
		asFUNCTION( PlayerEntity_GetMaxAmmo ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void SetMaxAmmo(size_t uiIndex, int iMaxAmmo)",
		asMETHODPR( CBasePlayer, SetMaxAmmo, (unsigned int, int), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void SetMaxAmmo(const string& in szName, int iMaxAmmo)",
		asFUNCTION( PlayerEntity_SetMaxAmmo ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void ResetMaxAmmoToDefault(size_t uiIndex)",
		asMETHODPR( CBasePlayer, ResetMaxAmmoToDefault, (unsigned int), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void ResetMaxAmmoToDefault(const string& in szName)",
		asFUNCTION( PlayerEntity_ResetMaxAmmoToDefault ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void ResetAllMaxAmmoToDefault()",
		asMETHOD( CBasePlayer, ResetAllMaxAmmoToDefault ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void RemoveExcessAmmo(size_t uiIndex)",
		asMETHODPR( CBasePlayer, RemoveExcessAmmo, (unsigned int), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void RemoveExcessAmmo(const string& in szName)",
		asFUNCTION( PlayerEntity_RemoveExcessAmmo ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "", "CBasePlayer", "void RemoveAllExcessAmmo()",
		asMETHOD( CBasePlayer, RemoveAllExcessAmmo ), asCALL_THISCALL );
}

void RegisterASPlayerHUDMethods( CASDocumentation *documentation )
{
	documentation->RegisterObjectMethod( "Creates a sprite above the player's head", "CBasePlayer",
	                                     "void ShowOverheadSprite(const string& in szSpriteName, const float flZOffset, const float flLifeTime)",
	                                     asFUNCTION( PlayerEntity_ShowOverheadSprite ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Set custom viewmodel position.", "CBasePlayer",
	                                     "void SetVModelPos( const Vector& in vecPos )", asMETHOD( CBasePlayer, SetVModelPos ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Reset viewmodel position.", "CBasePlayer",
	                                     "void ResetVModelPos()", asMETHOD( CBasePlayer, ResetVModelPos ), asCALL_THISCALL );
}

void RegisterPlayerKeyCodes( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Key Codes", "KeyCode", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "0", "KeyCode", "KEY_0", 0 );
	documentation->RegisterEnumValue( "1", "KeyCode", "KEY_1", 1 );
	documentation->RegisterEnumValue( "2", "KeyCode", "KEY_2", 2 );
	documentation->RegisterEnumValue( "3", "KeyCode", "KEY_3", 3 );
	documentation->RegisterEnumValue( "4", "KeyCode", "KEY_4", 4 );
	documentation->RegisterEnumValue( "5", "KeyCode", "KEY_5", 5 );
	documentation->RegisterEnumValue( "6", "KeyCode", "KEY_6", 6 );
	documentation->RegisterEnumValue( "7", "KeyCode", "KEY_7", 7 );
	documentation->RegisterEnumValue( "8", "KeyCode", "KEY_8", 8 );
	documentation->RegisterEnumValue( "9", "KeyCode", "KEY_9", 9 );
	documentation->RegisterEnumValue( "A", "KeyCode", "KEY_A", 10 );
	documentation->RegisterEnumValue( "B", "KeyCode", "KEY_B", 11 );
	documentation->RegisterEnumValue( "C", "KeyCode", "KEY_C", 12 );
	documentation->RegisterEnumValue( "D", "KeyCode", "KEY_D", 13 );
	documentation->RegisterEnumValue( "E", "KeyCode", "KEY_E", 14 );
	documentation->RegisterEnumValue( "F", "KeyCode", "KEY_F", 15 );
	documentation->RegisterEnumValue( "G", "KeyCode", "KEY_G", 16 );
	documentation->RegisterEnumValue( "H", "KeyCode", "KEY_H", 17 );
	documentation->RegisterEnumValue( "I", "KeyCode", "KEY_I", 18 );
	documentation->RegisterEnumValue( "J", "KeyCode", "KEY_J", 19 );
	documentation->RegisterEnumValue( "K", "KeyCode", "KEY_K", 20 );
	documentation->RegisterEnumValue( "L", "KeyCode", "KEY_L", 21 );
	documentation->RegisterEnumValue( "M", "KeyCode", "KEY_M", 22 );
	documentation->RegisterEnumValue( "N", "KeyCode", "KEY_N", 23 );
	documentation->RegisterEnumValue( "O", "KeyCode", "KEY_O", 24 );
	documentation->RegisterEnumValue( "P", "KeyCode", "KEY_P", 25 );
	documentation->RegisterEnumValue( "Q", "KeyCode", "KEY_Q", 26 );
	documentation->RegisterEnumValue( "R", "KeyCode", "KEY_R", 27 );
	documentation->RegisterEnumValue( "S", "KeyCode", "KEY_S", 28 );
	documentation->RegisterEnumValue( "T", "KeyCode", "KEY_T", 29 );
	documentation->RegisterEnumValue( "U", "KeyCode", "KEY_U", 30 );
	documentation->RegisterEnumValue( "V", "KeyCode", "KEY_V", 31 );
	documentation->RegisterEnumValue( "W", "KeyCode", "KEY_W", 32 );
	documentation->RegisterEnumValue( "X", "KeyCode", "KEY_X", 33 );
	documentation->RegisterEnumValue( "Y", "KeyCode", "KEY_Y", 34 );
	documentation->RegisterEnumValue( "Z", "KeyCode", "KEY_Z", 35 );
	documentation->RegisterEnumValue( "Numpad 0", "KeyCode", "KEY_PAD_0", 36 );
	documentation->RegisterEnumValue( "Numpad 1", "KeyCode", "KEY_PAD_1", 37 );
	documentation->RegisterEnumValue( "Numpad 2", "KeyCode", "KEY_PAD_2", 38 );
	documentation->RegisterEnumValue( "Numpad 3", "KeyCode", "KEY_PAD_3", 39 );
	documentation->RegisterEnumValue( "Numpad 4", "KeyCode", "KEY_PAD_4", 40 );
	documentation->RegisterEnumValue( "Numpad 5", "KeyCode", "KEY_PAD_5", 41 );
	documentation->RegisterEnumValue( "Numpad 6", "KeyCode", "KEY_PAD_6", 42 );
	documentation->RegisterEnumValue( "Numpad 7", "KeyCode", "KEY_PAD_7", 43 );
	documentation->RegisterEnumValue( "Numpad 8", "KeyCode", "KEY_PAD_8", 44 );
	documentation->RegisterEnumValue( "Numpad 9", "KeyCode", "KEY_PAD_9", 45 );
	documentation->RegisterEnumValue( "Numpad DIVIDE", "KeyCode", "KEY_PAD_DIVIDE", 46 );
	documentation->RegisterEnumValue( "Numpad MULTIPLY", "KeyCode", "KEY_PAD_MULTIPLY", 47 );
	documentation->RegisterEnumValue( "Numpad MINUS", "KeyCode", "KEY_PAD_MINUS", 48 );
	documentation->RegisterEnumValue( "Numpad PLUS", "KeyCode", "KEY_PAD_PLUS", 49 );
	documentation->RegisterEnumValue( "Numpad ENTER", "KeyCode", "KEY_PAD_ENTER", 50 );
	documentation->RegisterEnumValue( "Numpad DECIMAL", "KeyCode", "KEY_PAD_DECIMAL", 51 );
	documentation->RegisterEnumValue( "LBRACKET", "KeyCode", "KEY_LBRACKET", 52 );
	documentation->RegisterEnumValue( "RBRACKET", "KeyCode", "KEY_RBRACKET", 53 );
	documentation->RegisterEnumValue( "SEMICOLON", "KeyCode", "KEY_SEMICOLON", 54 );
	documentation->RegisterEnumValue( "APOSTROPHE", "KeyCode", "KEY_APOSTROPHE", 55 );
	documentation->RegisterEnumValue( "BACKQUOTE", "KeyCode", "KEY_BACKQUOTE", 56 );
	documentation->RegisterEnumValue( "COMMA", "KeyCode", "KEY_COMMA", 57 );
	documentation->RegisterEnumValue( "PERIOD", "KeyCode", "KEY_PERIOD", 58 );
	documentation->RegisterEnumValue( "SLASH", "KeyCode", "KEY_SLASH", 59 );
	documentation->RegisterEnumValue( "BACKSLASH", "KeyCode", "KEY_BACKSLASH", 60 );
	documentation->RegisterEnumValue( "MINUS", "KeyCode", "KEY_MINUS", 61 );
	documentation->RegisterEnumValue( "EQUAL", "KeyCode", "KEY_EQUAL", 62 );
	documentation->RegisterEnumValue( "ENTER", "KeyCode", "KEY_ENTER", 63 );
	documentation->RegisterEnumValue( "SPACE", "KeyCode", "KEY_SPACE", 64 );
	documentation->RegisterEnumValue( "BACKSPACE", "KeyCode", "KEY_BACKSPACE", 65 );
	documentation->RegisterEnumValue( "TAB", "KeyCode", "KEY_TAB", 66 );
	documentation->RegisterEnumValue( "CAPSLOCK", "KeyCode", "KEY_CAPSLOCK", 67 );
	documentation->RegisterEnumValue( "NUMLOCK", "KeyCode", "KEY_NUMLOCK", 68 );
	documentation->RegisterEnumValue( "ESCAPE", "KeyCode", "KEY_ESCAPE", 69 );
	documentation->RegisterEnumValue( "SCROLLLOCK", "KeyCode", "KEY_SCROLLLOCK", 70 );
	documentation->RegisterEnumValue( "INSERT", "KeyCode", "KEY_INSERT", 71 );
	documentation->RegisterEnumValue( "DELETE", "KeyCode", "KEY_DELETE", 72 );
	documentation->RegisterEnumValue( "HOME", "KeyCode", "KEY_HOME", 73 );
	documentation->RegisterEnumValue( "END", "KeyCode", "KEY_END", 74 );
	documentation->RegisterEnumValue( "PAGEUP", "KeyCode", "KEY_PAGEUP", 75 );
	documentation->RegisterEnumValue( "PAGEDOWN", "KeyCode", "KEY_PAGEDOWN", 76 );
	documentation->RegisterEnumValue( "BREAK", "KeyCode", "KEY_BREAK", 77 );
	documentation->RegisterEnumValue( "LSHIFT", "KeyCode", "KEY_LSHIFT", 78 );
	documentation->RegisterEnumValue( "RSHIFT", "KeyCode", "KEY_RSHIFT", 79 );
	documentation->RegisterEnumValue( "LALT", "KeyCode", "KEY_LALT", 80 );
	documentation->RegisterEnumValue( "RALT", "KeyCode", "KEY_RALT", 81 );
	documentation->RegisterEnumValue( "LCONTROL", "KeyCode", "KEY_LCONTROL", 82 );
	documentation->RegisterEnumValue( "RCONTROL", "KeyCode", "KEY_RCONTROL", 83 );
	documentation->RegisterEnumValue( "LWIN", "KeyCode", "KEY_LWIN", 84 );
	documentation->RegisterEnumValue( "RWIN", "KeyCode", "KEY_RWIN", 85 );
	documentation->RegisterEnumValue( "APP", "KeyCode", "KEY_APP", 86 );
	documentation->RegisterEnumValue( "UP", "KeyCode", "KEY_UP", 87 );
	documentation->RegisterEnumValue( "LEFT", "KeyCode", "KEY_LEFT", 88 );
	documentation->RegisterEnumValue( "DOWN", "KeyCode", "KEY_DOWN", 89 );
	documentation->RegisterEnumValue( "RIGHT", "KeyCode", "KEY_RIGHT", 90 );
	documentation->RegisterEnumValue( "F1", "KeyCode", "KEY_F1", 91 );
	documentation->RegisterEnumValue( "F2", "KeyCode", "KEY_F2", 92 );
	documentation->RegisterEnumValue( "F3", "KeyCode", "KEY_F3", 93 );
	documentation->RegisterEnumValue( "F4", "KeyCode", "KEY_F4", 94 );
	documentation->RegisterEnumValue( "F5", "KeyCode", "KEY_F5", 95 );
	documentation->RegisterEnumValue( "F6", "KeyCode", "KEY_F6", 96 );
	documentation->RegisterEnumValue( "F7", "KeyCode", "KEY_F7", 97 );
	documentation->RegisterEnumValue( "F8", "KeyCode", "KEY_F8", 98 );
	documentation->RegisterEnumValue( "F9", "KeyCode", "KEY_F9", 99 );
	documentation->RegisterEnumValue( "F10", "KeyCode", "KEY_F10", 100 );
	documentation->RegisterEnumValue( "F11", "KeyCode", "KEY_F11", 101 );
	documentation->RegisterEnumValue( "F12", "KeyCode", "KEY_F12", 102 );
	documentation->RegisterEnumValue( "The last number in the range of key codes.", "KeyCode", "KEY_LAST", 103 );
}

void RegisterPlayerFadeFlags( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Bit field values for entvars_t.buttons.\nUsed by players to determine which buttons have been pressed", "In_Buttons", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Primary attack", "In_Buttons", "IN_ATTACK", 1 );
	documentation->RegisterEnumValue( "Jump", "In_Buttons", "IN_JUMP", 2 );
	documentation->RegisterEnumValue( "Duck", "In_Buttons", "IN_DUCK", 4 );
	documentation->RegisterEnumValue( "Move forward", "In_Buttons", "IN_FORWARD", 8 );
	documentation->RegisterEnumValue( "Move backward", "In_Buttons", "IN_BACK", 16 );
	documentation->RegisterEnumValue( "Use", "In_Buttons", "IN_USE", 32 );
	documentation->RegisterEnumValue( "", "In_Buttons", "IN_CANCEL", 64 );
	documentation->RegisterEnumValue( "", "In_Buttons", "IN_LEFT", 128 );
	documentation->RegisterEnumValue( "", "In_Buttons", "IN_RIGHT", 256 );
	documentation->RegisterEnumValue( "Move left", "In_Buttons", "IN_MOVELEFT", 512 );
	documentation->RegisterEnumValue( "Move right", "In_Buttons", "IN_MOVERIGHT", 1024 );
	documentation->RegisterEnumValue( "Secondary attack", "In_Buttons", "IN_ATTACK2", 2048 );
	documentation->RegisterEnumValue( "Run/Walk", "In_Buttons", "IN_RUN", 4096 );
	documentation->RegisterEnumValue( "Reload", "In_Buttons", "IN_RELOAD", 0x2000 );
	documentation->RegisterEnumValue( "Tertiary attack", "In_Buttons", "IN_ALT1", 0x4000 );
	documentation->RegisterEnumValue( "Used by client.dll for when scoreboard is held down", "In_Buttons", "IN_SCORE", 0x8000 );
}

void RegisterSCScriptPlayerEntityDependencies( CASDocumentation *documentation )
{
	RegisterPlayerKeyCodes( documentation );
	documentation->RegisterEnum( "Observer modes", "ObserverMode", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Player in the process of entering observer mode", "ObserverMode", "OBS_ENTERING", -1 );
	documentation->RegisterEnumValue( "Player not in observer mode", "ObserverMode", "OBS_NONE", 0 );
	documentation->RegisterEnumValue( "Chase locked", "ObserverMode", "OBS_CHASE_LOCKED", 3 );
	documentation->RegisterEnumValue( "Free movement", "ObserverMode", "OBS_CHASE_FREE", 1 );
	documentation->RegisterEnumValue( "Roaming", "ObserverMode", "OBS_ROAMING", 2 );
	RegisterPlayerFadeFlags( documentation );
	documentation->RegisterEnum( "Train modes used by CBasePlayer::m_iTrain", "Train", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_ACTIVE", 128 );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_NEW", 192 );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_OFF", 0 );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_NEUTRAL", 1 );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_SLOW", 2 );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_MEDIUM", 3 );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_FAST", 4 );
	documentation->RegisterEnumValue( "", "Train", "TRAIN_BACK", 5 );
	documentation->RegisterEnum( "Player physics flags used in CBasePlayer::m_afPhysicsFlags", "PFLAG", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "PFLAG", "PFLAG_ONLADDER", 1 );
	documentation->RegisterEnumValue( "", "PFLAG", "PFLAG_ONSWING", 1 );
	documentation->RegisterEnumValue( "", "PFLAG", "PFLAG_ONTRAIN", 2 );
	documentation->RegisterEnumValue( "", "PFLAG", "PFLAG_ONBARNACLE", 4 );
	documentation->RegisterEnumValue( "In the process of ducking, but totally squatted yet", "PFLAG", "PFLAG_DUCKING", 8 );
	documentation->RegisterEnumValue( "Using a continuous entity", "PFLAG", "PFLAG_USING", 16 );
	documentation->RegisterEnumValue( "player is locked in stationary cam mode. Spectators can move, observers can't.", "PFLAG", "PFLAG_OBSERVER", 32 );
	documentation->RegisterEnumValue( "Toggle grapple use", "PFLAG", "PFLAG_GRAPPLE", 64 );
	documentation->RegisterEnumValue( "Looking through a trigger_camera", "PFLAG", "PFLAG_CAMERA", 128 );
	documentation->RegisterEnum( "Player animations, set using CBasePlayer::SetAnimation.", "PLAYER_ANIM", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_IDLE", 0 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_WALK", 1 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_JUMP", 2 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_SUPERJUMP", 3 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_DIE", 4 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_ATTACK1", 5 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_RELOAD", 6 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_DEPLOY", 7 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_BARNACLE_HIT", 8 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_BARNACLE_PULL", 9 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_BARNACLE_CHOMP", 10 );
	documentation->RegisterEnumValue( "", "PLAYER_ANIM", "PLAYER_BARNACLE_CHEW", 11 );
	documentation->RegisterEnum( "Player view mode.", "PlayerViewMode", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "PlayerViewMode", "ViewMode_FirstPerson", 0 );
	documentation->RegisterEnumValue( "", "PlayerViewMode", "ViewMode_ThirdPerson", 1 );
	documentation->RegisterEnum( "Player hook flags. Used to skip execution of code when a hook performs an action that overrides built-in behavior.", "PlayerHookFlag", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Don't run vehicle checks", "PlayerHookFlag", "PlrHook_SkipVehicles", 1 );
	documentation->RegisterEnumValue( "Don't run +use checks", "PlayerHookFlag", "PlrHook_SkipUse", 2 );

	RegisterSCScriptObserverClass( documentation );

	documentation->RegisterGlobalProperty( "Maximum number of ammo types", "const size_t MAX_AMMO_TYPES", (void *) &AS_MAX_AMMO_TYPES );
	documentation->RegisterGlobalProperty( "HUD item selection slots", "const size_t MAX_ITEM_TYPES", (void *) &AS_MAX_ITEM_TYPES );
	documentation->RegisterGlobalProperty( "Autoaim", "const float AUTOAIM_2DEGREES", (void *) &AS_AUTOAIM_2DEGREES );
	documentation->RegisterGlobalProperty( "Autoaim", "const float AUTOAIM_5DEGREES", (void *) &AS_AUTOAIM_5DEGREES );
	documentation->RegisterGlobalProperty( "Autoaim", "const float AUTOAIM_8DEGREES", (void *) &AS_AUTOAIM_8DEGREES );
	documentation->RegisterGlobalProperty( "Autoaim", "const float AUTOAIM_10DEGREES", (void *) &AS_AUTOAIM_10DEGREES );
	documentation->RegisterGlobalProperty( "Time between player vocal client commands", "const float PLAYERCOMMAND_WAIT", (void *) &AS_PLAYERCOMMAND_WAIT );
	documentation->RegisterGlobalFunction( "", "int TrainSpeed(int iSpeed, int iMax)", asFUNCTION( TrainSpeed ), asCALL_CDECL, NULL );
}
