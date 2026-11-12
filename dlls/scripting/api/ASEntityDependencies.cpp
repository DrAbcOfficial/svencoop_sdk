#include "scripting/api/ASEntityDependencies.h"
#include "scripting/core/CASDocumentation.h"

void RegisterASEntityReferenceTypes( CASDocumentation *documentation )
{
	static const struct
	{
		const char *description;
		const char *name;
	} types[] =
	{
		{ "All entities derive from this", "CBaseEntity" },
		{ "All entities that can trigger with a delay derive from this", "CBaseDelay" },
		{ "All entities that can animate derive from this", "CBaseAnimating" },
		{ "All entities that can be toggled derive from this", "CBaseToggle" },
		{ "Player item base class", "CBasePlayerItem" },
		{ "Player Weapon entity base class", "CBasePlayerWeapon" },
		{ "Player ammo class", "CBasePlayerAmmo" },
		{ "Item_inventory entity class\nCollection rules -- DENY/CAN'T have rules take precedence over REQUIRE/MUST\nAll string filters can be tokenized with a semi-colon ';'", "CItemInventory" },
		{ "Generic item class", "CItem" },
		{ "Grenade class", "CGrenade" },
		{ "Monster entity base class", "CBaseMonster" },
		{ "Cinematic monster entity base class", "CCineMonster" },
		{ "Player entity base class", "CBasePlayer" },
		{ "Sprite class", "CSprite" },
		{ "Path track class", "CPathTrack" },
		{ "Beam class", "CBeam" },
		{ "Laser class", "CLaser" },
		{ "Tank base class", "CBaseTank" },
		{ "Path condition entity", "CPathCondition" },
		{ "Button entity base class", "CBaseButton" },
		{ "Door entity base class", "CBaseDoor" },
		{ "Gib class", "CGib" },
	};

	for ( unsigned int i = 0; i < sizeof( types ) / sizeof( types[0] ); i++ )
	{
		documentation->RegisterObjectType( types[i].description, types[i].name,
		    0, asOBJ_REF | asOBJ_NOCOUNT );
	}
}
