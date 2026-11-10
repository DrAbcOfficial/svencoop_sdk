#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "weapons.h"
#include "ASWeaponDependencies.h"
#include "CASDocumentation.h"

namespace
{
const int AS_LOUD_GUN_VOLUME = LOUD_GUN_VOLUME;
const int AS_NORMAL_GUN_VOLUME = NORMAL_GUN_VOLUME;
const int AS_QUIET_GUN_VOLUME = QUIET_GUN_VOLUME;

const int AS_BRIGHT_GUN_FLASH = BRIGHT_GUN_FLASH;
const int AS_NORMAL_GUN_FLASH = NORMAL_GUN_FLASH;
const int AS_DIM_GUN_FLASH = DIM_GUN_FLASH;

const int AS_BIG_EXPLOSION_VOLUME = BIG_EXPLOSION_VOLUME;
const int AS_NORMAL_EXPLOSION_VOLUME = NORMAL_EXPLOSION_VOLUME;
const int AS_SMALL_EXPLOSION_VOLUME = SMALL_EXPLOSION_VOLUME;
const int AS_WEAPON_ACTIVITY_VOLUME = WEAPON_ACTIVITY_VOLUME;

const Vector AS_VECTOR_CONE_1DEGREES = VECTOR_CONE_1DEGREES;
const Vector AS_VECTOR_CONE_2DEGREES = VECTOR_CONE_2DEGREES;
const Vector AS_VECTOR_CONE_3DEGREES = VECTOR_CONE_3DEGREES;
const Vector AS_VECTOR_CONE_4DEGREES = VECTOR_CONE_4DEGREES;
const Vector AS_VECTOR_CONE_5DEGREES = VECTOR_CONE_5DEGREES;
const Vector AS_VECTOR_CONE_6DEGREES = VECTOR_CONE_6DEGREES;
const Vector AS_VECTOR_CONE_7DEGREES = VECTOR_CONE_7DEGREES;
const Vector AS_VECTOR_CONE_8DEGREES = VECTOR_CONE_8DEGREES;
const Vector AS_VECTOR_CONE_9DEGREES = VECTOR_CONE_9DEGREES;
const Vector AS_VECTOR_CONE_10DEGREES = VECTOR_CONE_10DEGREES;
const Vector AS_VECTOR_CONE_15DEGREES = VECTOR_CONE_15DEGREES;
const Vector AS_VECTOR_CONE_20DEGREES = VECTOR_CONE_20DEGREES;

const int AS_WEAPON_NOCLIP = WEAPON_NOCLIP;

void RegisterWeaponIds( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Weapon id values", "WeaponIds", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue( "Weapon id used for invalid weapon ids.", "WeaponIds", "WEAPON_NONE", 0 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_CROWBAR", 1 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_GLOCK", 2 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_PYTHON", 3 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_MP5", 4 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_CHAINGUN", 5 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_CROSSBOW", 6 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_SHOTGUN", 7 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_RPG", 8 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_GAUSS", 9 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_EGON", 10 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_HORNETGUN", 11 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_HANDGRENADE", 12 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_TRIPMINE", 13 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_SATCHEL", 14 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_SNARK", 15 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_UZI", 17 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_MEDKIT", 18 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_CROWBAR_ELECTRIC", 19 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_PIPEWRENCH", 20 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_MINIGUN", 21 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_GRAPPLE", 22 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_SNIPERRIFLE", 23 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_M249", 24 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_M16", 25 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_SPORELAUNCHER", 26 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_DESERT_EAGLE", 27 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_SHOCKRIFLE", 28 );
	documentation->RegisterEnumValue( "", "WeaponIds", "WEAPON_DISPLACER", 29 );
	documentation->RegisterEnumValue( "Maximum number of weapons that the game supports", "WeaponIds", "MAX_WEAPONS", 256 );
}
}

void RegisterSCScriptWeaponDependencies( CASDocumentation *documentation )
{
	documentation->RegisterGlobalProperty(
	    "Parameter for CSoundEnt::InsertSound iVolume", "const int LOUD_GUN_VOLUME",
	    (void *) &AS_LOUD_GUN_VOLUME );
	documentation->RegisterGlobalProperty(
	    "Parameter for CSoundEnt::InsertSound iVolume", "const int NORMAL_GUN_VOLUME",
	    (void *) &AS_NORMAL_GUN_VOLUME );
	documentation->RegisterGlobalProperty(
	    "Parameter for CSoundEnt::InsertSound iVolume", "const int QUIET_GUN_VOLUME",
	    (void *) &AS_QUIET_GUN_VOLUME );
	documentation->RegisterGlobalProperty(
	    "Value used in CBasePlayer::m_iWeaponFlash", "const int BRIGHT_GUN_FLASH",
	    (void *) &AS_BRIGHT_GUN_FLASH );
	documentation->RegisterGlobalProperty(
	    "Value used in CBasePlayer::m_iWeaponFlash", "const int NORMAL_GUN_FLASH",
	    (void *) &AS_NORMAL_GUN_FLASH );
	documentation->RegisterGlobalProperty(
	    "Value used in CBasePlayer::m_iWeaponFlash", "const int DIM_GUN_FLASH",
	    (void *) &AS_DIM_GUN_FLASH );
	documentation->RegisterGlobalProperty(
	    "Parameter for CSoundEnt::InsertSound iVolume", "const int BIG_EXPLOSION_VOLUME",
	    (void *) &AS_BIG_EXPLOSION_VOLUME );
	documentation->RegisterGlobalProperty(
	    "Parameter for CSoundEnt::InsertSound iVolume", "const int NORMAL_EXPLOSION_VOLUME",
	    (void *) &AS_NORMAL_EXPLOSION_VOLUME );
	documentation->RegisterGlobalProperty(
	    "Parameter for CSoundEnt::InsertSound iVolume", "const int SMALL_EXPLOSION_VOLUME",
	    (void *) &AS_SMALL_EXPLOSION_VOLUME );
	documentation->RegisterGlobalProperty(
	    "Parameter for CSoundEnt::InsertSound iVolume", "const int WEAPON_ACTIVITY_VOLUME",
	    (void *) &AS_WEAPON_ACTIVITY_VOLUME );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_1DEGREES",
	    (void *) &AS_VECTOR_CONE_1DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_2DEGREES",
	    (void *) &AS_VECTOR_CONE_2DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_3DEGREES",
	    (void *) &AS_VECTOR_CONE_3DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_4DEGREES",
	    (void *) &AS_VECTOR_CONE_4DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_5DEGREES",
	    (void *) &AS_VECTOR_CONE_5DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_6DEGREES",
	    (void *) &AS_VECTOR_CONE_6DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_7DEGREES",
	    (void *) &AS_VECTOR_CONE_7DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_8DEGREES",
	    (void *) &AS_VECTOR_CONE_8DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_9DEGREES",
	    (void *) &AS_VECTOR_CONE_9DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_10DEGREES",
	    (void *) &AS_VECTOR_CONE_10DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_15DEGREES",
	    (void *) &AS_VECTOR_CONE_15DEGREES );
	documentation->RegisterGlobalProperty(
	    "", "const Vector VECTOR_CONE_20DEGREES",
	    (void *) &AS_VECTOR_CONE_20DEGREES );

	RegisterWeaponIds( documentation );

	documentation->RegisterGlobalProperty( "Used for clips that don't use ammo.", "const int WEAPON_NOCLIP", (void *) &AS_WEAPON_NOCLIP );
}
