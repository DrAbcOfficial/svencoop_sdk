#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ASEntityDependencyLeaves.h"
#include "ASEntityDependencies.h"
#include "ASEntityInterfaces.h"
#include "ASEntityClassification.h"
#include "ASEntityHandle.h"
#include "ASMonsterDependencies.h"
#include "ASPlayerDependencies.h"
#include "ASRGBA.h"
#include "CASDocumentation.h"
#include "CCustomKeyvalue.h"
#include "CCustomKeyvalues.h"
#include "CString.h"
#include "server_support.h"
#include "decals.h"
#include <new>

static const float AS_VIEW_FIELD_FULL = -1.0f;
static const float AS_VIEW_FIELD_WIDE = -0.7f;
static const float AS_VIEW_FIELD_NARROW = 0.7f;
static const float AS_VIEW_FIELD_ULTRA_NARROW = 0.9f;

CString gDecals_GetName( decal_e index, DLL_DECALLIST *decals )
{
	if ( (unsigned int) index > DECAL_SPORESPLAT3 )
		return CString( "" );

	return CString( decals[index].name );
}

int gDecals_GetIndex( decal_e index, DLL_DECALLIST *decals )
{
	if ( (unsigned int) index > DECAL_SPORESPLAT3 )
		return 0;

	return decals[index].index;
}

void DefaultConstructCustomKeyvalue( void *memory )
{
	new ( memory ) CCustomKeyvalue;
}

void CopyConstructCustomKeyvalue( const CCustomKeyvalue &value, void *memory )
{
	new ( memory ) CCustomKeyvalue( value );
}

void DestructCustomKeyvalue( CCustomKeyvalue *value )
{
	value->~CCustomKeyvalue();
}

bool CustomKeyvalues_SetKeyvalue( const CString &name, const CString &value, CCustomKeyvalues *keyvalues )
{
	const char *text = STRING( AllocStringLocal( value.Data() ) );

	// The string overload passes character storage to the typed setter
	return keyvalues->SetKeyvalue( name.Data(), text, VAR_STRING, OWNERSHIP_DELETE );
}

bool CustomKeyvalues_SetKeyvalue( const CString &name, const Vector &value, CCustomKeyvalues *keyvalues )
{
	return keyvalues->SetKeyvalue( name.Data(), &value, VAR_VECTOR, OWNERSHIP_DELETE );
}

bool CustomKeyvalues_SetKeyvalue( const CString &name, float value, CCustomKeyvalues *keyvalues )
{
	return keyvalues->SetKeyvalue( name.Data(), &value, VAR_FLOAT, OWNERSHIP_DELETE );
}

bool CustomKeyvalues_SetKeyvalue( const CString &name, int value, CCustomKeyvalues *keyvalues )
{
	return keyvalues->SetKeyvalue( name.Data(), &value, VAR_INTEGER, OWNERSHIP_DELETE );
}

void CustomKeyvalues_InitializeKeyvalueWithDefault( const CString &name, CCustomKeyvalues *keyvalues )
{
	entvartype_s type;

	keyvalues->InitializeKeyvalueWithDefault( name.Data(), type );
}

bool CustomKeyvalues_HasKeyvalue( const CString &name, CCustomKeyvalues *keyvalues )
{
	return keyvalues->HasKeyvalue( name.Data() );
}

CCustomKeyvalue CustomKeyvalues_GetKeyvalue( const CString &name, CCustomKeyvalues *keyvalues )
{
	entvartype_s type;
	void *value = keyvalues->GetKeyvalue( name.Data(), type );

	return CCustomKeyvalue( value, type );
}

bool NullEdict( const edict_t *entity )
{
	return !entity || entity->free || g_engfuncs.pfnEntOffsetOfPEntity( entity ) == 0;
}

bool NullEntityVariables( entvars_t *variables )
{
	return !variables || NullEdict( variables->pContainingEntity );
}

void RegisterBulletTypes( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "bullet types", "Bullet", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_NONE", 0 );
	documentation->RegisterEnumValue( "Glock", "Bullet", "BULLET_PLAYER_9MM", 1 );
	documentation->RegisterEnumValue( "mp5", "Bullet", "BULLET_PLAYER_MP5", 2 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_PLAYER_SAW", 3 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_PLAYER_SNIPER", 4 );
	documentation->RegisterEnumValue( "Python", "Bullet", "BULLET_PLAYER_357", 5 );
	documentation->RegisterEnumValue( "Desert eagle", "Bullet", "BULLET_PLAYER_EAGLE", 6 );
	documentation->RegisterEnumValue( "Shotgun", "Bullet", "BULLET_PLAYER_BUCKSHOT", 7 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_PLAYER_CROWBAR", 8 );
	documentation->RegisterEnumValue( "normal bullet with custom damage", "Bullet", "BULLET_PLAYER_CUSTOMDAMAGE", 9 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_MONSTER_9MM", 10 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_MONSTER_MP5", 11 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_MONSTER_SAW", 12 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_MONSTER_12MM", 13 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_MONSTER_EAGLE", 14 );
	documentation->RegisterEnumValue( "", "Bullet", "BULLET_MONSTER_SNIPER", 15 );
	documentation->RegisterEnumValue( "Monster shotgun", "Bullet", "BULLET_MONSTER_BUCKSHOT", 16 );
	documentation->RegisterEnumValue( "", "Bullet", "DECAL_SCORCH_MARK", 17 );
}

void RegisterDamageTypes( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Damage types.", "DMG", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Generic damage.", "DMG", "DMG_GENERIC", 0 );
	documentation->RegisterEnumValue( "Crush damage. By falling or moving object.", "DMG", "DMG_CRUSH", 1 );
	documentation->RegisterEnumValue( "Bullet. Shot.", "DMG", "DMG_BULLET", 2 );
	documentation->RegisterEnumValue( "Slash. Cut, clawed, stabbed.", "DMG", "DMG_SLASH", 4 );
	documentation->RegisterEnumValue( "Burn. Heat burned.", "DMG", "DMG_BURN", 8 );
	documentation->RegisterEnumValue( "Freeze. Frozen.", "DMG", "DMG_FREEZE", 16 );
	documentation->RegisterEnumValue( "Fall. Fell too far.", "DMG", "DMG_FALL", 32 );
	documentation->RegisterEnumValue( "Blast. Explosive blast damage.", "DMG", "DMG_BLAST", 64 );
	documentation->RegisterEnumValue( "Club. Crowbar, punch, headbutt.", "DMG", "DMG_CLUB", 128 );
	documentation->RegisterEnumValue( "Shock. Electric shock.", "DMG", "DMG_SHOCK", 256 );
	documentation->RegisterEnumValue( "Sonic. Sound pulse shockwave.", "DMG", "DMG_SONIC", 512 );
	documentation->RegisterEnumValue( "Energy beam. Laser or other high energy beam.", "DMG", "DMG_ENERGYBEAM", 1024 );
	documentation->RegisterEnumValue( "Never gib. With this bit OR'd in, no damage type will be able to gib victims upon death.", "DMG", "DMG_NEVERGIB", 4096 );
	documentation->RegisterEnumValue( "Always gib. With this bit OR'd in, any damage type can be made to gib victims upon death.", "DMG", "DMG_ALWAYSGIB", 0x2000 );
	documentation->RegisterEnumValue( "Launch. Launch into air.", "DMG", "DMG_LAUNCH", 0x4000000 );
	documentation->RegisterEnumValue( "Drown. Drowning.", "DMG", "DMG_DROWN", 0x4000 );
	documentation->RegisterEnumValue( "Mash for time-based damage.", "DMG", "DMG_TIMEBASED", 16760832 );
	documentation->RegisterEnumValue( "Paralyze. Slows affected creature down.", "DMG", "DMG_PARALYZE", 0x8000 );
	documentation->RegisterEnumValue( "Nerve gas. Nerve toxins, very bad.", "DMG", "DMG_NERVEGAS", 0x10000 );
	documentation->RegisterEnumValue( "Poison. Blood poisoning.", "DMG", "DMG_POISON", 0x20000 );
	documentation->RegisterEnumValue( "Radiation. Radiation exposure.", "DMG", "DMG_RADIATION", 0x40000 );
	documentation->RegisterEnumValue( "Drown recover. Drowning recovery.", "DMG", "DMG_DROWNRECOVER", 0x80000 );
	documentation->RegisterEnumValue( "Acid. Toxic chemicals or acid burns.", "DMG", "DMG_ACID", 0x100000 );
	documentation->RegisterEnumValue( "Slow burn. In an oven.", "DMG", "DMG_SLOWBURN", 0x200000 );
	documentation->RegisterEnumValue( "Slow freeze. In a subzero freezer.", "DMG", "DMG_SLOWFREEZE", 0x400000 );
	documentation->RegisterEnumValue( "Mortar. Hit by air raid (done to distinguish grenade from mortar)", "DMG", "DMG_MORTAR", 0x800000 );
	documentation->RegisterEnumValue( "Sniper. Hit by a Sniper.", "DMG", "DMG_SNIPER", 0x1000000 );
	documentation->RegisterEnumValue( "Medkit heal. Medkit healing.", "DMG", "DMG_MEDKITHEAL", 0x2000000 );
	documentation->RegisterEnumValue( "Shock glow. Flag for shock glow effect.", "DMG", "DMG_SHOCK_GLOW", 0x8000000 );
	documentation->RegisterEnumValue( "Gib corpse. These are the damage types that are allowed to gib corpses.", "DMG", "DMG_GIB_CORPSE", 993 );
	documentation->RegisterEnumValue( "Shown on HUD. These are the damage types that have clien hud art.", "DMG", "DMG_SHOWNHUD", 7815448 );
}

void RegisterRenderFX( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Render FX modes (entvars_t::renderfx)", "RenderFX", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxNone", 0 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxPulseSlow", 1 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxPulseFast", 2 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxPulseSlowWide", 3 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxPulseFastWide", 4 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxFadeSlow", 5 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxFadeFast", 6 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxSolidSlow", 7 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxSolidFast", 8 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxStrobeSlow", 9 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxStrobeFast", 10 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxStrobeFaster", 11 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxFlickerSlow", 12 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxFlickerFast", 13 );
	documentation->RegisterEnumValue( "", "RenderFX", "kRenderFxNoDissipation", 14 );
	documentation->RegisterEnumValue( "Distort/scale/translate flicker", "RenderFX", "kRenderFxDistort", 15 );
	documentation->RegisterEnumValue( "kRenderFxDistort + distance fade", "RenderFX", "kRenderFxHologram", 16 );
	documentation->RegisterEnumValue( "kRenderAmt is the player index", "RenderFX", "kRenderFxDeadPlayer", 17 );
	documentation->RegisterEnumValue( "Scale up really big!", "RenderFX", "kRenderFxExplode", 18 );
	documentation->RegisterEnumValue( "Glowing Shell", "RenderFX", "kRenderFxGlowShell", 19 );
	documentation->RegisterEnumValue( "Keep this sprite from getting very small (SPRITES only!)", "RenderFX", "kRenderFxClampMinScale", 20 );
}

void RegisterCustomKeyvalue( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Custom keyvalue. Only use this as a local variable, do not store them!", "CustomKeyvalue", sizeof( CCustomKeyvalue ), asOBJ_VALUE | asOBJ_APP_CLASS_CDK );
	documentation->RegisterObjectBehaviour( "Constructor", "CustomKeyvalue", asBEHAVE_CONSTRUCT, "void CustomKeyvalue()", asFUNCTION( DefaultConstructCustomKeyvalue ), asCALL_CDECL_OBJLAST, NULL );
	documentation->RegisterObjectBehaviour( "Constructor", "CustomKeyvalue", asBEHAVE_CONSTRUCT, "void CustomKeyvalue(const CustomKeyvalue& in other)", asFUNCTION( CopyConstructCustomKeyvalue ), asCALL_CDECL_OBJLAST, NULL );
	documentation->RegisterObjectBehaviour( "Destructor", "CustomKeyvalue", asBEHAVE_DESTRUCT, "void DestructCustomKeyvalue()", asFUNCTION( DestructCustomKeyvalue ), asCALL_CDECL_OBJLAST, NULL );
	documentation->RegisterObjectMethod( "Returns whether the value exists.", "CustomKeyvalue", "bool Exists() const", asMETHOD( CCustomKeyvalue, Exists ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the type of the keyvalue.\nReturns VAR_INVALID if this value does not exist.", "CustomKeyvalue", "Entvartype GetType() const", asMETHOD( CCustomKeyvalue, GetType ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the value as a string.", "CustomKeyvalue", "string GetString() const", asMETHOD( CCustomKeyvalue, GetString ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the value as a vector.", "CustomKeyvalue", "Vector GetVector() const", asMETHOD( CCustomKeyvalue, GetVector ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the value as a float.", "CustomKeyvalue", "float GetFloat() const", asMETHOD( CCustomKeyvalue, GetFloat ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Gets the value as an integer.", "CustomKeyvalue", "int GetInteger() const", asMETHOD( CCustomKeyvalue, GetInteger ), asCALL_THISCALL );
}

void RegisterDecalMetadata( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Dynamic decals used by code to place decals like scorch marks in the world", "decal_e", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterObjectType( "Dynamic decal manager", "CDecals", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod( "Gets the name of the texture representing the given decal.", "CDecals", "string GetName(decal_e decal) const", asFUNCTION( gDecals_GetName ), asCALL_CDECL_OBJLAST );

	// The declaration returns a string while its callback returns an integer
	documentation->RegisterObjectMethod( "Gets the texture frame index of the given decal.", "CDecals", "string GetIndex(decal_e decal) const", asFUNCTION( gDecals_GetIndex ), asCALL_CDECL_OBJLAST );

	documentation->RegisterGlobalProperty( "Dynamic decal manager instance", "CDecals g_Decals", gDecals );
	documentation->RegisterEnumValue( "Gunshot 1", "decal_e", "DECAL_GUNSHOT1", 0 );
	documentation->RegisterEnumValue( "Gunshot 2", "decal_e", "DECAL_GUNSHOT2", 1 );
	documentation->RegisterEnumValue( "Gunshot 3", "decal_e", "DECAL_GUNSHOT3", 2 );
	documentation->RegisterEnumValue( "Gunshot 4", "decal_e", "DECAL_GUNSHOT4", 3 );
	documentation->RegisterEnumValue( "Gunshot 5", "decal_e", "DECAL_GUNSHOT5", 4 );
	documentation->RegisterEnumValue( "Lambda 1", "decal_e", "DECAL_LAMBDA1", 5 );
	documentation->RegisterEnumValue( "Lambda 2", "decal_e", "DECAL_LAMBDA2", 6 );
	documentation->RegisterEnumValue( "Lambda 3", "decal_e", "DECAL_LAMBDA3", 7 );
	documentation->RegisterEnumValue( "Lambda 4", "decal_e", "DECAL_LAMBDA4", 8 );
	documentation->RegisterEnumValue( "Lambda 5", "decal_e", "DECAL_LAMBDA5", 9 );
	documentation->RegisterEnumValue( "Lambda 6", "decal_e", "DECAL_LAMBDA6", 10 );
	documentation->RegisterEnumValue( "Scorch 1", "decal_e", "DECAL_SCORCH1", 11 );
	documentation->RegisterEnumValue( "Scorch 1", "decal_e", "DECAL_SCORCH2", 12 );
	documentation->RegisterEnumValue( "(Red) Blood 1", "decal_e", "DECAL_BLOOD1", 13 );
	documentation->RegisterEnumValue( "(Red) Blood 2", "decal_e", "DECAL_BLOOD2", 14 );
	documentation->RegisterEnumValue( "(Red) Blood 3", "decal_e", "DECAL_BLOOD3", 15 );
	documentation->RegisterEnumValue( "(Red) Blood 4", "decal_e", "DECAL_BLOOD4", 16 );
	documentation->RegisterEnumValue( "(Red) Blood 5", "decal_e", "DECAL_BLOOD5", 17 );
	documentation->RegisterEnumValue( "(Red) Blood 6", "decal_e", "DECAL_BLOOD6", 18 );
	documentation->RegisterEnumValue( "Yellow blood 1", "decal_e", "DECAL_YBLOOD1", 19 );
	documentation->RegisterEnumValue( "Yellow blood 2", "decal_e", "DECAL_YBLOOD2", 20 );
	documentation->RegisterEnumValue( "Yellow blood 3", "decal_e", "DECAL_YBLOOD3", 21 );
	documentation->RegisterEnumValue( "Yellow blood 4", "decal_e", "DECAL_YBLOOD4", 22 );
	documentation->RegisterEnumValue( "Yellow blood 5", "decal_e", "DECAL_YBLOOD5", 23 );
	documentation->RegisterEnumValue( "Yellow blood 6", "decal_e", "DECAL_YBLOOD6", 24 );
	documentation->RegisterEnumValue( "Glass break 1", "decal_e", "DECAL_GLASSBREAK1", 25 );
	documentation->RegisterEnumValue( "Glass break 2", "decal_e", "DECAL_GLASSBREAK2", 26 );
	documentation->RegisterEnumValue( "Glass break 3", "decal_e", "DECAL_GLASSBREAK3", 27 );
	documentation->RegisterEnumValue( "Big shot 1", "decal_e", "DECAL_BIGSHOT1", 28 );
	documentation->RegisterEnumValue( "Big shot 2", "decal_e", "DECAL_BIGSHOT2", 29 );
	documentation->RegisterEnumValue( "Big shot 3", "decal_e", "DECAL_BIGSHOT3", 30 );
	documentation->RegisterEnumValue( "Big shot 4", "decal_e", "DECAL_BIGSHOT4", 31 );
	documentation->RegisterEnumValue( "Big shot 5", "decal_e", "DECAL_BIGSHOT5", 32 );
	documentation->RegisterEnumValue( "Spit 1", "decal_e", "DECAL_SPIT1", 33 );
	documentation->RegisterEnumValue( "Spit 2", "decal_e", "DECAL_SPIT2", 34 );
	documentation->RegisterEnumValue( "Bulletproof glass decal", "decal_e", "DECAL_BPROOF1", 35 );
	documentation->RegisterEnumValue( "gargantua stomp crack", "decal_e", "DECAL_GARGSTOMP1", 36 );
	documentation->RegisterEnumValue( "Small scorch 1", "decal_e", "DECAL_SMALLSCORCH1", 37 );
	documentation->RegisterEnumValue( "Small scorch 2", "decal_e", "DECAL_SMALLSCORCH2", 38 );
	documentation->RegisterEnumValue( "Small scorch 3", "decal_e", "DECAL_SMALLSCORCH3", 39 );
	documentation->RegisterEnumValue( "Big momma birth splatter", "decal_e", "DECAL_MOMMABIRTH", 40 );
	documentation->RegisterEnumValue( "Big momma splat", "decal_e", "DECAL_MOMMASPLAT", 41 );
	documentation->RegisterEnumValue( "Opposing force scorch 1", "decal_e", "DECAL_OFSCORCH1", 42 );
	documentation->RegisterEnumValue( "Opposing force scorch 2", "decal_e", "DECAL_OFSCORCH2", 43 );
	documentation->RegisterEnumValue( "Opposing force scorch 3", "decal_e", "DECAL_OFSCORCH3", 44 );
	documentation->RegisterEnumValue( "Spore splat 1", "decal_e", "DECAL_SPORESPLAT1", 45 );
	documentation->RegisterEnumValue( "Spore splat 2", "decal_e", "DECAL_SPORESPLAT2", 46 );
	documentation->RegisterEnumValue( "Spore splat 3", "decal_e", "DECAL_SPORESPLAT3", 47 );
}

void RegisterSCScriptButtonCode( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Button codes used by CBaseButton", "ButtonCode", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Do nothing", "ButtonCode", "BUTTON_NOTHING", 0 );
	documentation->RegisterEnumValue( "Activate the button", "ButtonCode", "BUTTON_ACTIVATE", 1 );
	documentation->RegisterEnumValue( "Return the button to its default state", "ButtonCode", "BUTTON_RETURN", 2 );
}

void RegisterSCScriptButtonDependencies( CASDocumentation *documentation )
{
	RegisterSCScriptButtonCode( documentation );
}

void RegisterSCScriptDoorDependencies( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Door flags.", "DoorFlag", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_ROTATE_Y", 0 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_START_OPEN", 1 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_ROTATE_BACKWARDS", 2 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_PASSABLE", 8 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_ONEWAY", 16 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_NO_AUTO_RETURN", 32 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_ROTATE_Z", 64 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_ROTATE_X", 128 );
	documentation->RegisterEnumValue( "door must be opened by player's use button.", "DoorFlag", "SF_DOOR_USE_ONLY", 256 );
	documentation->RegisterEnumValue( "Monster can't open", "DoorFlag", "SF_DOOR_NOMONSTERS", 512 );
	documentation->RegisterEnumValue( "", "DoorFlag", "SF_DOOR_SILENT", 0x80000000 );
	documentation->RegisterEnumValue( "Don't make sure the doors are in-sync.", "DoorFlag", "SF_DOOR_NO_LINK_CHECKING", 1024 );
}

void RegisterSCScriptEntityDependencies( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Use type for entities. Not all entities support all use types.\nPassed to the Use method on entities.", "USE_TYPE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Turns off an entity", "USE_TYPE", "USE_OFF", 0 );
	documentation->RegisterEnumValue( "Turns on an entity", "USE_TYPE", "USE_ON", 1 );
	documentation->RegisterEnumValue( "Sets the state of an entity (use float parameter in Use)", "USE_TYPE", "USE_SET", 2 );
	documentation->RegisterEnumValue( "Toggles the state of an entity", "USE_TYPE", "USE_TOGGLE", 3 );
	documentation->RegisterEnumValue( "Kills an entity", "USE_TYPE", "USE_KILL", 4 );
	documentation->RegisterEnum( "Entity variable type", "Entvartype", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Invalid type", "Entvartype", "VAR_INVALID", 0 );
	documentation->RegisterEnumValue( "String type", "Entvartype", "VAR_STRING", 1 );
	documentation->RegisterEnumValue( "Vector type", "Entvartype", "VAR_VECTOR", 2 );
	documentation->RegisterEnumValue( "Float type", "Entvartype", "VAR_FLOAT", 3 );
	documentation->RegisterEnumValue( "Integer type", "Entvartype", "VAR_INTEGER", 4 );
	documentation->RegisterEnumValue( "Edict type", "Entvartype", "VAR_EDICT", 5 );
	documentation->RegisterEnumValue( "4 byte array type", "Entvartype", "VAR_BYTE4", 6 );
	documentation->RegisterEnumValue( "2 byte array type", "Entvartype", "VAR_BYTE2", 7 );
	documentation->RegisterEnumValue( "Custom type", "Entvartype", "VAR_CUSTOM", 8 );
	documentation->RegisterEnum( "CBaseEntity caps (CBaseEntity::ObjectCaps())", "FCAP", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Custom save method", "FCAP", "FCAP_CUSTOMSAVE", 1 );
	documentation->RegisterEnumValue( "Across transition", "FCAP", "FCAP_ACROSS_TRANSITION", 2 );
	documentation->RegisterEnumValue( "Must spawn", "FCAP", "FCAP_MUST_SPAWN", 4 );
	documentation->RegisterEnumValue( "Don't save", "FCAP", "FCAP_DONT_SAVE", 0x80000000 );
	documentation->RegisterEnumValue( "Impulse use", "FCAP", "FCAP_IMPULSE_USE", 8 );
	documentation->RegisterEnumValue( "Continuous use", "FCAP", "FCAP_CONTINUOUS_USE", 16 );
	documentation->RegisterEnumValue( "On/off use", "FCAP", "FCAP_ONOFF_USE", 32 );
	documentation->RegisterEnumValue( "Directional use", "FCAP", "FCAP_DIRECTIONAL_USE", 64 );
	documentation->RegisterEnumValue( "Is a master", "FCAP", "FCAP_MASTER", 128 );
	documentation->RegisterEnumValue( "Force transition", "FCAP", "FCAP_FORCE_TRANSITION", 128 );
	RegisterASEntityClassification( documentation );
	documentation->RegisterEnum( "Relationship between (monster) entities", "RELATIONSHIP", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Ally. Good alternative to R_NO when applicable.", "RELATIONSHIP", "R_AL", -2 );
	documentation->RegisterEnumValue( "No relationship. Disregard.", "RELATIONSHIP", "R_NO", 0 );
	documentation->RegisterEnumValue( "Dislike. Will attack.", "RELATIONSHIP", "R_DL", 1 );
	documentation->RegisterEnumValue( "Hate. Will attack this character instead of any DISLIKE'd characters.", "RELATIONSHIP", "R_HT", 2 );
	documentation->RegisterEnumValue( "Nemesis. A monster will ALWAYS attack its nemesis, no matter what.", "RELATIONSHIP", "R_NM", 3 );
	documentation->RegisterEnumValue( "Fear. Will attack.", "RELATIONSHIP", "R_FR", 4 );
	RegisterDamageTypes( documentation );
	documentation->RegisterEnum( "Toggle state for entities that can be toggled.", "TOGGLE_STATE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Entity is at top", "TOGGLE_STATE", "TS_AT_TOP", 0 );
	documentation->RegisterEnumValue( "Entity is at bottom", "TOGGLE_STATE", "TS_AT_BOTTOM", 1 );
	documentation->RegisterEnumValue( "Entity is going up", "TOGGLE_STATE", "TS_GOING_UP", 2 );
	documentation->RegisterEnumValue( "Entity is going down", "TOGGLE_STATE", "TS_GOING_DOWN", 3 );
	documentation->RegisterGlobalProperty( "Full view field", "const float VIEW_FIELD_FULL", (void *) &AS_VIEW_FIELD_FULL );
	documentation->RegisterGlobalProperty( "Wide view field", "const float VIEW_FIELD_WIDE", (void *) &AS_VIEW_FIELD_WIDE );
	documentation->RegisterGlobalProperty( "Narrow view field", "const float VIEW_FIELD_NARROW", (void *) &AS_VIEW_FIELD_NARROW );
	documentation->RegisterGlobalProperty( "Ultra narrow view field", "const float VIEW_FIELD_ULTRA_NARROW", (void *) &AS_VIEW_FIELD_ULTRA_NARROW );
	documentation->RegisterEnum( "Blood color", "BLOOD_COLOR", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Don't bleed", "BLOOD_COLOR", "DONT_BLEED", -1 );
	documentation->RegisterEnumValue( "Red blood", "BLOOD_COLOR", "BLOOD_COLOR_RED", 247 );
	documentation->RegisterEnumValue( "Yellow blood", "BLOOD_COLOR", "BLOOD_COLOR_YELLOW", 195 );
	documentation->RegisterEnumValue( "Green blood", "BLOOD_COLOR", "BLOOD_COLOR_GREEN", 195 );
	RegisterDecalMetadata( documentation );
	documentation->RegisterEnum( "when calling Killed(), a value that governs gib behavior is expected to be one of these three values", "GIB", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "gib if entity was overkilled", "GIB", "GIB_NORMAL", 0 );
	documentation->RegisterEnumValue( "never gib, no matter how much death damage is done ( freezing, etc )", "GIB", "GIB_NEVER", 1 );
	documentation->RegisterEnumValue( "always gib ( Houndeye Shock, Barnacle Bite )", "GIB", "GIB_ALWAYS", 2 );
	documentation->RegisterEnumValue( "No penalty applied to players that died", "GIB", "GIB_NOPENALTY", 3 );
	documentation->RegisterEnum( "Texture types used by the game.", "TextureType", CASDocumentation::ENUM_CHAR );
	documentation->RegisterEnumValue( "Concrete (Default)", "TextureType", "CHAR_TEX_CONCRETE", 67 );
	documentation->RegisterEnumValue( "Metal", "TextureType", "CHAR_TEX_METAL", 77 );
	documentation->RegisterEnumValue( "Dirt", "TextureType", "CHAR_TEX_DIRT", 68 );
	documentation->RegisterEnumValue( "Vent", "TextureType", "CHAR_TEX_VENT", 86 );
	documentation->RegisterEnumValue( "Grate", "TextureType", "CHAR_TEX_GRATE", 71 );
	documentation->RegisterEnumValue( "Tile", "TextureType", "CHAR_TEX_TILE", 84 );
	documentation->RegisterEnumValue( "Slosh", "TextureType", "CHAR_TEX_SLOSH", 83 );
	documentation->RegisterEnumValue( "Wood", "TextureType", "CHAR_TEX_WOOD", 87 );
	documentation->RegisterEnumValue( "Computer", "TextureType", "CHAR_TEX_COMPUTER", 80 );
	documentation->RegisterEnumValue( "Glass", "TextureType", "CHAR_TEX_GLASS", 89 );
	documentation->RegisterEnumValue( "Flesh", "TextureType", "CHAR_TEX_FLESH", 70 );
	documentation->RegisterEnumValue( "Snow", "TextureType", "CHAR_TEX_SNOW", 79 );
	documentation->RegisterEnum( "Break model defines for message SVC_TEMPENTITY::TE_BREAKMODEL", "BREAK", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Bit mask for all BREAK types.", "BREAK", "BREAK_TYPEMASK", 79 );
	documentation->RegisterEnumValue( "", "BREAK", "BREAK_GLASS", 1 );
	documentation->RegisterEnumValue( "", "BREAK", "BREAK_METAL", 2 );
	documentation->RegisterEnumValue( "", "BREAK", "BREAK_FLESH", 4 );
	documentation->RegisterEnumValue( "", "BREAK", "BREAK_WOOD", 8 );
	documentation->RegisterEnumValue( "Adds a smoke trail", "BREAK", "BREAK_SMOKE", 16 );
	documentation->RegisterEnumValue( "Model is transparent", "BREAK", "BREAK_TRANS", 32 );
	documentation->RegisterEnumValue( "", "BREAK", "BREAK_CONCRETE", 64 );
	documentation->RegisterEnumValue( "Not used", "BREAK", "BREAK_2", 128 );
	documentation->RegisterEnum( "Temp entity bounce sound types", "TE_BOUNCE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "TE_BOUNCE", "TE_BOUNCE_NULL", 0 );
	documentation->RegisterEnumValue( "", "TE_BOUNCE", "TE_BOUNCE_SHELL", 1 );
	documentation->RegisterEnumValue( "", "TE_BOUNCE", "TE_BOUNCE_SHOTSHELL", 2 );
	documentation->RegisterEnum( "Render modes used by entities (entvars_t::rendermode)", "RenderModes", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "src", "RenderModes", "kRenderNormal", 0 );
	documentation->RegisterEnumValue( "c*a+dest*(1-a)", "RenderModes", "kRenderTransColor", 1 );
	documentation->RegisterEnumValue( "src*a+dest*(1-a)", "RenderModes", "kRenderTransTexture", 2 );
	documentation->RegisterEnumValue( "src*a+dest -- No Z buffer checks", "RenderModes", "kRenderGlow", 3 );
	documentation->RegisterEnumValue( "src*srca+dest*(1-srca)", "RenderModes", "kRenderTransAlpha", 4 );
	documentation->RegisterEnumValue( "src*a+dest", "RenderModes", "kRenderTransAdd", 5 );
	RegisterRenderFX( documentation );
	RegisterASScriptClassInterface( documentation );
	documentation->RegisterEnum( "FireBullets draw mode options", "FireBulletsDrawMode", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Don't draw impact decal", "FireBulletsDrawMode", "FBDM_DONTDRAW", 0 );
	documentation->RegisterEnumValue( "Draw impact decal", "FireBulletsDrawMode", "FBDM_DRAW", 2 );
	documentation->RegisterEnum( "Common entity flags used by various entities.", "CommonEntityFlags", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Set this bit on guns and stuff that should never respawn.", "CommonEntityFlags", "SF_NORESPAWN", 1024 );
	documentation->RegisterEnumValue( "A weapon will be removed from the map after 60sec", "CommonEntityFlags", "SF_DELAYREMOVE", 2048 );
	documentation->RegisterEnumValue( "This flag indicates that this entity is a weapon created for players,\n"
	                                  "as opposed to being created by being placed by a mapper",
	                                  "CommonEntityFlags", "SF_CREATEDWEAPON", 4096 );
	documentation->RegisterEnumValue( "This flag indicates that a player thrown weapon should deal damage to things it hits", "CommonEntityFlags", "SF_DODAMAGE", 0x2000 );
	documentation->RegisterEnumValue( "This item is being given to the player -- can't fall out of map", "CommonEntityFlags", "SF_GIVENITEM", 0x4000 );
	documentation->RegisterEnumValue( "Don't allow default ammo to be set. Prevents players from getting too much ammo from cross map inventory", "CommonEntityFlags", "SF_NODEFAULTAMMO", 0x8000 );
	RegisterBulletTypes( documentation );
	documentation->RegisterEnum( "Return values for CBaseEntity::AddPlayerItem", "AddPlayerItemResult", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Player could not pick up this item", "AddPlayerItemResult", "APIR_NotAdded", 0 );
	documentation->RegisterEnumValue( "Player was able to add it as duplicate", "AddPlayerItemResult", "APIR_DuplicateAdd", 2 );
	documentation->RegisterEnumValue( "Player picked up this item", "AddPlayerItemResult", "APIR_Added", 3 );
	documentation->RegisterEnum( "Material types supported by breakables", "Materials", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "", "Materials", "matGlass", 0 );
	documentation->RegisterEnumValue( "", "Materials", "matWood", 1 );
	documentation->RegisterEnumValue( "", "Materials", "matMetal", 2 );
	documentation->RegisterEnumValue( "", "Materials", "matFlesh", 3 );
	documentation->RegisterEnumValue( "", "Materials", "matCinderBlock", 4 );
	documentation->RegisterEnumValue( "", "Materials", "matCeilingTile", 5 );
	documentation->RegisterEnumValue( "", "Materials", "matComputer", 6 );
	documentation->RegisterEnumValue( "", "Materials", "matUnbreakableGlass", 7 );
	documentation->RegisterEnumValue( "", "Materials", "matRocks", 8 );
	documentation->RegisterEnumValue( "", "Materials", "matNone", 9 );
	documentation->RegisterEnumValue( "", "Materials", "matLastMaterial", 10 );
	documentation->RegisterEnum( "How an item was dropped.", "DropTypes", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Default/unspecified.", "DropTypes", "DROP_DEFAULT", 0 );
	documentation->RegisterEnumValue( "By a player via drop/dropammo/dropsecammo command.", "DropTypes", "DROP_PLAYER_CMD", 1 );
	documentation->RegisterEnumValue( "By a player killed.", "DropTypes", "DROP_PLAYER_DEATH", 2 );
	documentation->RegisterEnumValue( "By an NPC killed.", "DropTypes", "DROP_NPC_DEATH", 3 );
	documentation->RegisterEnum( "How an item was collected.", "CollectTypes", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Touched", "CollectTypes", "COLLECT_TOUCH", 0 );
	documentation->RegisterEnumValue( "Used directly (+use)", "CollectTypes", "COLLECT_USE_DIRECT", 1 );
	documentation->RegisterEnumValue( "Used indirectly (trigger/etc)", "CollectTypes", "COLLECT_USE_INDIRECT", 2 );
	documentation->RegisterEnum( "Entity mask constants", "EFLAG", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Entity don't collide with friendly monsters/players", "EFLAG", "EFLAG_PROJECTILE", 2 );
	documentation->RegisterEnumValue( "Entity ignores trigger_gravity", "EFLAG", "EFLAG_IGNOREGRAVITY", 4 );
	RegisterSCScriptRGBA( documentation );
	RegisterSCScriptRGBAConstants( documentation );
	RegisterASEntityReferenceTypes( documentation );
	RegisterASEntityHandle( documentation );
	RegisterCustomKeyvalue( documentation );

	documentation->RegisterObjectType( "Custom keyvalues. Do not keep handles to instances of these objects.", "CustomKeyvalues", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Sets a custom keyvalue to the given string value.\nReturns true if the key existed and was overwritten.", "CustomKeyvalues",
	    "bool SetKeyvalue(const string& in szKey, const string& in szValue)",
	    asFUNCTIONPR( CustomKeyvalues_SetKeyvalue, (const CString &, const CString &, CCustomKeyvalues *), bool ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Sets a custom keyvalue to the given vector value.\nReturns true if the key existed and was overwritten.", "CustomKeyvalues",
	    "bool SetKeyvalue(const string& in szKey, const Vector& in vecValue)",
	    asFUNCTIONPR( CustomKeyvalues_SetKeyvalue, (const CString &, const Vector &, CCustomKeyvalues *), bool ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Sets a custom keyvalue to the given float value.\nReturns true if the key existed and was overwritten.", "CustomKeyvalues",
	    "bool SetKeyvalue(const string& in szKey, float flValue)",
	    asFUNCTIONPR( CustomKeyvalues_SetKeyvalue, (const CString &, float, CCustomKeyvalues *), bool ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod(
	    "Sets a custom keyvalue to the given integer value.\nReturns true if the key existed and was overwritten.", "CustomKeyvalues",
	    "bool SetKeyvalue(const string& in szKey, int iValue)",
	    asFUNCTIONPR( CustomKeyvalues_SetKeyvalue, (const CString &, int, CCustomKeyvalues *), bool ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod( "Initializes a keyvalue with the default value for its type.", "CustomKeyvalues", "void InitializeKeyvalueWithDefault(const string& in szKey)", asFUNCTION( CustomKeyvalues_InitializeKeyvalueWithDefault ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Returns whether the keyvalue with the given name exists.", "CustomKeyvalues", "bool HasKeyvalue(const string& in szKey) const", asFUNCTION( CustomKeyvalues_HasKeyvalue ), asCALL_CDECL_OBJLAST );
	documentation->RegisterObjectMethod( "Gets the keyvalue with the given name. Check if it exists by calling Exists.", "CustomKeyvalues", "CustomKeyvalue GetKeyvalue(const string& in szKey) const", asFUNCTION( CustomKeyvalues_GetKeyvalue ), asCALL_CDECL_OBJLAST );
	RegisterSCScriptMonsterEntityDependencies( documentation );
	RegisterSCScriptPlayerEntityDependencies( documentation );
	documentation->RegisterGlobalFunction( "Checks edict for nullity", "bool FNullEnt(const edict_t@ pEnt)", asFUNCTION( NullEdict ), asCALL_CDECL, NULL );
	documentation->RegisterGlobalFunction( "Checks entvars for nullity", "bool FNullEnt(entvars_t@ pev)", asFUNCTION( NullEntityVariables ), asCALL_CDECL, NULL );
	documentation->RegisterEnum( "Satchel codes", "SATCHELCODE", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Detonates all satchel charges owned by a player", "SATCHELCODE", "SATCHEL_DETONATE", 0 );
	documentation->RegisterEnumValue( "Release all satches charges owner by a player from their owner", "SATCHELCODE", "SATCHEL_RELEASE", 1 );
	RegisterSCScriptButtonDependencies( documentation );
	RegisterSCScriptDoorDependencies( documentation );
}
