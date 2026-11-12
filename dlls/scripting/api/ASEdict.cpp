#include "extdll.h"
#include "util.h"
#include "scripting/api/ASEdict.h"
#include "scripting/core/CASDocumentation.h"
#include "CString.h"

bool EntityVars_ClassNameIs(const CString &className, entvars_t *variables)
{
	return strcmp(STRING(variables->classname), className.Data()) == 0;
}

unsigned char EntityVars_GetController(unsigned int index, entvars_t *variables)
{
	return variables->controller[index];
}

void EntityVars_SetController(unsigned int index, unsigned char value, entvars_t *variables)
{
	variables->controller[index] = value;
}

unsigned char EntityVars_GetBlending(unsigned int index, entvars_t *variables)
{
	return variables->blending[index];
}

void EntityVars_SetBlending(unsigned int index, unsigned char value, entvars_t *variables)
{
	variables->blending[index] = value;
}

bool EntityVars_SpawnFlagBitSet(int flags, entvars_t *variables)
{
	return (variables->spawnflags & flags) != 0;
}

bool EntityVars_FlagBitSet(int flags, entvars_t *variables)
{
	return (variables->flags & flags) != 0;
}

edict_t *EntityVars_GetContainingEntity(entvars_t *variables)
{
	return variables->pContainingEntity;
}

short Edict_GetLeafNums(unsigned int index, edict_t *edict)
{
	if (index >= MAX_ENT_LEAFS)
		return 0;

	return edict->leafnums[index];
}

void RegisterSCScriptEdict(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Entity dictionary structure", "edict_t", 0, asOBJ_REF | asOBJ_NOCOUNT);

	documentation->RegisterEnum("entvars_t::flags constants", "EdictFlags", CASDocumentation::ENUM_UNSIGNED);
	documentation->RegisterEnumValue("Changes the SV_Movestep() behavior to not need to be on ground", "EdictFlags", "FL_FLY", 1);
	documentation->RegisterEnumValue("Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)", "EdictFlags", "FL_SWIM", 2);
	documentation->RegisterEnumValue("If set, entities standing on this entity are moved as if this were a conveyor", "EdictFlags", "FL_CONVEYOR", 4);
	documentation->RegisterEnumValue("If set, this is a player", "EdictFlags", "FL_CLIENT", 8);
	documentation->RegisterEnumValue("If set, this entity is in water. Only applies to players", "EdictFlags", "FL_INWATER", 16);
	documentation->RegisterEnumValue("If set, this is a monster", "EdictFlags", "FL_MONSTER", 32);
	documentation->RegisterEnumValue("If set, this entity is invincible. Only applies to players", "EdictFlags", "FL_GODMODE", 64);
	documentation->RegisterEnumValue("If set, this entity will not be targeted by monster AI", "EdictFlags", "FL_NOTARGET", 128);
	documentation->RegisterEnumValue("Don't send entity to local host, it's predicting this entity itself", "EdictFlags", "FL_SKIPLOCALHOST", 256);
	documentation->RegisterEnumValue("At rest / on the ground", "EdictFlags", "FL_ONGROUND", 512);
	documentation->RegisterEnumValue("not all corners are valid", "EdictFlags", "FL_PARTIALGROUND", 1024);
	documentation->RegisterEnumValue("player jumping out of water", "EdictFlags", "FL_WATERJUMP", 2048);
	documentation->RegisterEnumValue("Player is frozen for 3rd person camera", "EdictFlags", "FL_FROZEN", 4096);
	documentation->RegisterEnumValue("JAC: fake client, simulated server side; don't send network messages to them", "EdictFlags", "FL_FAKECLIENT", 0x2000);
	documentation->RegisterEnumValue("Player flag -- Player is fully crouched", "EdictFlags", "FL_DUCKING", 0x4000);
	documentation->RegisterEnumValue("Apply floating force to this entity when in water", "EdictFlags", "FL_FLOAT", 0x8000);
	documentation->RegisterEnumValue("worldgraph has this ent listed as something that blocks a connection", "EdictFlags", "FL_GRAPHED", 0x10000);
	documentation->RegisterEnumValue("If set, players don't take drown damage", "EdictFlags", "FL_IMMUNE_WATER", 0x20000);
	documentation->RegisterEnumValue("If set, players don't take damage from slime", "EdictFlags", "FL_IMMUNE_SLIME", 0x40000);
	documentation->RegisterEnumValue("If set, players don't take damage from lava", "EdictFlags", "FL_IMMUNE_LAVA", 0x80000);
	documentation->RegisterEnumValue("This is a spectator proxy", "EdictFlags", "FL_PROXY", 0x100000);
	documentation->RegisterEnumValue(
		"Brush model flag -- call think every frame regardless of nextthink - ltime (for constantly changing "
		"velocity/path)",
		"EdictFlags", "FL_ALWAYSTHINK", 0x200000);
	documentation->RegisterEnumValue("Base velocity has been applied this frame (used to convert base velocity into momentum)",
		"EdictFlags", "FL_BASEVELOCITY", 0x400000);
	documentation->RegisterEnumValue("Only collide in with monsters who have FL_MONSTERCLIP set", "EdictFlags", "FL_MONSTERCLIP", 0x800000);
	documentation->RegisterEnumValue("Player is _controlling_ a train, so movement commands should be ignored on client during prediction.",
		"EdictFlags", "FL_ONTRAIN", 0x1000000);
	documentation->RegisterEnumValue(
		"Not moveable/removeable brush entity (really part of the world, but represented as an entity for "
		"transparency or something)",
		"EdictFlags", "FL_WORLDBRUSH", 0x2000000);
	documentation->RegisterEnumValue("This client is a spectator, don't run touch functions, etc.", "EdictFlags", "FL_SPECTATOR", 0x4000000);
	documentation->RegisterEnumValue("This client is being denied weapon usage", "EdictFlags", "FL_NOWEAPONS", 0x8000000);
	documentation->RegisterEnumValue("This is a custom entity", "EdictFlags", "FL_CUSTOMENTITY", 0x20000000);
	documentation->RegisterEnumValue("This entity is marked for death -- This allows the engine to kill ents at the appropriate time",
		"EdictFlags", "FL_KILLME", 0x40000000);
	documentation->RegisterEnumValue("Entity is dormant, no updates to client", "EdictFlags", "FL_DORMANT", (int)0x80000000u);

	documentation->RegisterEnum("entvars_t::movetype constants", "MOVETYPE", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("never moves (forced by map)", "MOVETYPE", "MOVETYPE_NONE_EXPLICIT", -1);
	documentation->RegisterEnumValue("never moves", "MOVETYPE", "MOVETYPE_NONE", 0);
	documentation->RegisterEnumValue("Player only - moving on the ground", "MOVETYPE", "MOVETYPE_WALK", 3);
	documentation->RegisterEnumValue("gravity, special edge handling -- monsters use this", "MOVETYPE", "MOVETYPE_STEP", 4);
	documentation->RegisterEnumValue("No gravity, but still collides with stuff", "MOVETYPE", "MOVETYPE_FLY", 5);
	documentation->RegisterEnumValue("gravity/collisions", "MOVETYPE", "MOVETYPE_TOSS", 6);
	documentation->RegisterEnumValue("no clip to world, push and crush", "MOVETYPE", "MOVETYPE_PUSH", 7);
	documentation->RegisterEnumValue("No gravity, no collisions, still do velocity/avelocity", "MOVETYPE", "MOVETYPE_NOCLIP", 8);
	documentation->RegisterEnumValue("extra size to monsters", "MOVETYPE", "MOVETYPE_FLYMISSILE", 9);
	documentation->RegisterEnumValue("Just like Toss, but reflect velocity when contacting surfaces", "MOVETYPE", "MOVETYPE_BOUNCE", 10);
	documentation->RegisterEnumValue("bounce w/o gravity", "MOVETYPE", "MOVETYPE_BOUNCEMISSILE", 11);
	documentation->RegisterEnumValue("track movement of aiment", "MOVETYPE", "MOVETYPE_FOLLOW", 12);
	documentation->RegisterEnumValue("BSP model that needs physics/world collisions (uses nearest hull for world collision)",
		"MOVETYPE", "MOVETYPE_PUSHSTEP", 13);

	documentation->RegisterEnum(
		"entvars_t::solid constants.\nNOTE: Some movetypes will cause collisions independent of "
		"SOLID_NOT/SOLID_TRIGGER when the entity moves\nSOLID only effects OTHER entities colliding with this "
		"one when they move - UGH!",
		"SOLID", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("no interaction with other objects (forced by map)", "SOLID", "SOLID_NOT_EXPLICIT", -1);
	documentation->RegisterEnumValue("no interaction with other objects", "SOLID", "SOLID_NOT", 0);
	documentation->RegisterEnumValue("touch on edge, but not blocking", "SOLID", "SOLID_TRIGGER", 1);
	documentation->RegisterEnumValue("touch on edge, block", "SOLID", "SOLID_BBOX", 2);
	documentation->RegisterEnumValue("touch on edge, but not an onground", "SOLID", "SOLID_SLIDEBOX", 3);
	documentation->RegisterEnumValue("bsp clip, touch on edge, block", "SOLID", "SOLID_BSP", 4);

	documentation->RegisterEnum("Entity entvars_t::deadflag values", "DEAD", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("Alive", "DEAD", "DEAD_NO", 0);
	documentation->RegisterEnumValue("Playing death animation or still falling off of a ledge waiting to hit ground", "DEAD", "DEAD_DYING", 1);
	documentation->RegisterEnumValue("Dead, lying still.", "DEAD", "DEAD_DEAD", 2);
	documentation->RegisterEnumValue("The player is in a state where they can be respawned.", "DEAD", "DEAD_RESPAWNABLE", 3);
	documentation->RegisterEnumValue("No direct use of this", "DEAD", "DEAD_DISCARDBODY", 4);

	documentation->RegisterEnum("entvars_t::takedamage values", "DAMAGE", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("Don't allow damage", "DAMAGE", "DAMAGE_NO", 0);
	documentation->RegisterEnumValue("Allow damage", "DAMAGE", "DAMAGE_YES", 1);
	documentation->RegisterEnumValue("Allow auto-aim", "DAMAGE", "DAMAGE_AIM", 2);

	documentation->RegisterEnum("entvars_t::effects constants", "EFFECTS", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("Swirling cloud of particles", "EFFECTS", "EF_BRIGHTFIELD", 1);
	documentation->RegisterEnumValue("Single frame ELIGHT on entity attachment 0", "EFFECTS", "EF_MUZZLEFLASH", 2);
	documentation->RegisterEnumValue("DLIGHT centered at entity origin", "EFFECTS", "EF_BRIGHTLIGHT", 4);
	documentation->RegisterEnumValue("Player flashlight", "EFFECTS", "EF_DIMLIGHT", 8);
	documentation->RegisterEnumValue("Get lighting from ceiling", "EFFECTS", "EF_INVLIGHT", 16);
	documentation->RegisterEnumValue("Don't interpolate the next frame", "EFFECTS", "EF_NOINTERP", 32);
	documentation->RegisterEnumValue("Rocket flare glow sprite", "EFFECTS", "EF_LIGHT", 64);
	documentation->RegisterEnumValue("Don't draw entity", "EFFECTS", "EF_NODRAW", 128);
	documentation->RegisterEnumValue("Don't animate brush entities", "EFFECTS", "EF_NOANIMTEXTURES", 256);
	documentation->RegisterEnumValue("Use the value in entvars_t::frame to set the texture on a brush entity", "EFFECTS", "EF_FRAMEANIMTEXTURES", 512);
	documentation->RegisterEnumValue("Allow a map to override a sprite viewport type (from entvars_t::sequence)", "EFFECTS", "EF_SPRITE_CUSTOM_VP", 1024);
	documentation->RegisterEnumValue("Don't draw decals on this entity", "EFFECTS", "EF_NODECALS", 2048);
	documentation->RegisterEnumValue("DonRender the entity at full brightness", "EFFECTS", "EF_FULLBRIGHT", 4096);
	documentation->RegisterEnumValue("Don't render a studio shadow for this entity", "EFFECTS", "EF_NOSHADOW", 0x2000);

	documentation->RegisterEnum(
		"Sprite viewport types. When overriden stored in entvars_t::sequence with EF_SPRITE_CUSTOM_VP set in "
		"entvars_t::effects for engine use.",
		"VP_TYPE", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("Parallel: Always face the player.", "VP_TYPE", "VP_PARALLEL", 2);
	documentation->RegisterEnumValue("Parallel upright: Always face the player on horizontal axis only, fixed vertically.",
		"VP_TYPE", "VP_PARALLEL_UPRIGHT", 0);
	documentation->RegisterEnumValue("Parallel orientated: Always face the player, but use angles to rotate the sprite.",
		"VP_TYPE", "VP_PARALLEL_ORITENTATED", 4);
	documentation->RegisterEnumValue("Facing upright: ???", "VP_TYPE", "VP_FACING_UPRIGHT", 1);
	documentation->RegisterEnumValue("Orientated: Angled according to the world instead of player viewport.", "VP_TYPE", "VP_ORIENTATED", 3);

	documentation->RegisterEnum("Map sprite viewport types.", "MAP_VP_TYPE", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("Default. (Do not override the sprite's QC choice.)", "MAP_VP_TYPE", "MAP_VP_DEFAULT", 0);
	documentation->RegisterEnumValue("Parallel: Always face the player.", "MAP_VP_TYPE", "MAP_VP_PARALLEL", 3);
	documentation->RegisterEnumValue("Parallel upright: Always face the player on horizontal axis only, fixed vertically.",
		"MAP_VP_TYPE", "MAP_VP_PARALLEL_UPRIGHT", 1);
	documentation->RegisterEnumValue("Parallel orientated: Always face the player, but use angles to rotate the sprite.",
		"MAP_VP_TYPE", "MAP_VP_PARALLEL_ORITENTATED", 5);
	documentation->RegisterEnumValue("Facing upright: ???", "MAP_VP_TYPE", "MAP_VP_FACING_UPRIGHT", 2);
	documentation->RegisterEnumValue("Orientated: Angled according to the world instead of player viewport.", "MAP_VP_TYPE", "MAP_VP_ORIENTATED", 4);

	documentation->RegisterEnum("entvars_t waterlevel values", "WATERLEVEL", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("Not in water", "WATERLEVEL", "WATERLEVEL_DRY", 0);
	documentation->RegisterEnumValue("Feet underwater", "WATERLEVEL", "WATERLEVEL_FEET", 1);
	documentation->RegisterEnumValue("Waist underwater", "WATERLEVEL", "WATERLEVEL_WAIST", 2);
	documentation->RegisterEnumValue("Head underwater (completely submerged)", "WATERLEVEL", "WATERLEVEL_HEAD", 3);

	documentation->RegisterEnum("entvars_t::fixangle modes", "FixAngleMode", CASDocumentation::ENUM_SIGNED);
	documentation->RegisterEnumValue("Do nothing", "FixAngleMode", "FAM_NOTHING", 0);
	documentation->RegisterEnumValue("Force player view angles to entvars_t::angles", "FixAngleMode", "FAM_FORCEVIEWANGLES", 1);
	documentation->RegisterEnumValue("Add avelocity to player view angles", "FixAngleMode", "FAM_ADDAVELOCITY", 2);

	documentation->RegisterObjectType("Networked entity variables", "entvars_t", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectProperty("The entity class name", "entvars_t", "const string_t classname", asOFFSET(entvars_t, classname));
	documentation->RegisterObjectMethod("Returns whether the given class name is this entity's class name",
		"entvars_t", "bool ClassNameIs(const string& in szClassName)", asFUNCTION(EntityVars_ClassNameIs), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("The entity global name", "entvars_t", "string_t globalname", asOFFSET(entvars_t, globalname));
	documentation->RegisterObjectProperty("Entity origin. Must be set using g_EntityFuncs.SetOrigin.",
		"entvars_t", "Vector origin", asOFFSET(entvars_t, origin));
	documentation->RegisterObjectProperty(
		"Old entity origin. Used by some entities to store off their original origin before moving to a "
		"different location.",
		"entvars_t", "Vector oldorigin", asOFFSET(entvars_t, oldorigin));
	documentation->RegisterObjectProperty("Entity velocity", "entvars_t", "Vector velocity", asOFFSET(entvars_t, velocity));
	documentation->RegisterObjectProperty(
		"Entity base velocity. Used when standing on another entity that applies velocity to entities "
		"standing on it, for instance conveyors.",
		"entvars_t", "Vector basevelocity", asOFFSET(entvars_t, basevelocity));
	documentation->RegisterObjectProperty(
		"Entity move direction. Used by some entities for movement directions, but some entities use it for "
		"other purposes.",
		"entvars_t", "Vector movedir", asOFFSET(entvars_t, movedir));
	documentation->RegisterObjectProperty("Entity angles, in degrees.", "entvars_t", "Vector angles", asOFFSET(entvars_t, angles));
	documentation->RegisterObjectProperty("Entity angular velocity, in degrees/sec. Applied to the entity's angles.",
		"entvars_t", "Vector avelocity", asOFFSET(entvars_t, avelocity));
	documentation->RegisterObjectProperty(
		"Player punch angle. These angles are applied to players when they are 'punched'.These angles are "
		"automatically interpolated back to the null vector over time.",
		"entvars_t", "Vector punchangle", asOFFSET(entvars_t, punchangle));
	documentation->RegisterObjectProperty(
		"Player view angle. These are the angles used for the player's view, and differ from the regular "
		"angles variable.",
		"entvars_t", "Vector v_angle", asOFFSET(entvars_t, v_angle));
	documentation->RegisterObjectProperty("Entity end position. Reserved for future use.", "entvars_t", "Vector endpos", asOFFSET(entvars_t, endpos));
	documentation->RegisterObjectProperty("Entity start position. Reserved for future use.", "entvars_t", "Vector startpos", asOFFSET(entvars_t, startpos));
	documentation->RegisterObjectProperty("Entity impact time. Reserved for future use.", "entvars_t", "float impacttime", asOFFSET(entvars_t, impacttime));
	documentation->RegisterObjectProperty("Entity start time. Reserved for future use.", "entvars_t", "float starttime", asOFFSET(entvars_t, starttime));
	documentation->RegisterObjectProperty(
		"Entity fix angle. Engine uses this value to change player view angles, and then resets it to "
		"FAM_NOTHING. See FixAngleMode enum.",
		"entvars_t", "int fixangle", asOFFSET(entvars_t, fixangle));
	documentation->RegisterObjectProperty(
		"Entity ideal pitch. Used in conjunction with pitch_speed to interpolate the entity's current pitch "
		"angle to this value.",
		"entvars_t", "float idealpitch", asOFFSET(entvars_t, idealpitch));
	documentation->RegisterObjectProperty("Entity pitch speed. See idealpitch.", "entvars_t", "float pitch_speed", asOFFSET(entvars_t, pitch_speed));
	documentation->RegisterObjectProperty(
		"Entity ideal yaw. Used in conjunction with yaw_speed to interpolate the entity's current yaw angle "
		"to this value.",
		"entvars_t", "float ideal_yaw", asOFFSET(entvars_t, ideal_yaw));
	documentation->RegisterObjectProperty("Entity yaw speed. See ideal_yaw.", "entvars_t", "float yaw_speed", asOFFSET(entvars_t, yaw_speed));
	documentation->RegisterObjectProperty("Entity model index. If this entity has a model, is the unique index of the model. Otherwise, is 0.",
		"entvars_t", "const int modelindex", asOFFSET(entvars_t, modelindex));
	documentation->RegisterObjectProperty(
		"The entity model name. If this entity has a model, is the name of that model. Otherwise, is an empty "
		"string.Use g_EntityFuncs.SetModel to set this variable.",
		"entvars_t", "string_t model", asOFFSET(entvars_t, model));
	documentation->RegisterObjectProperty(
		"Player view model index. If this player has a visible weapon, is the name of that view (v_) model. "
		"Otherwise, is an empty string.",
		"entvars_t", "string_t viewmodel", asOFFSET(entvars_t, viewmodel));
	documentation->RegisterObjectProperty(
		"Entity weapon model index. If this player has a visible weapon, is the name of that third person "
		"(p_) model. Otherwise, is an empty string.",
		"entvars_t", "string_t weaponmodel", asOFFSET(entvars_t, weaponmodel));
	documentation->RegisterObjectProperty(
		"Entity absolute minimum size. Is an absolute position in the world. Automatically updated by the "
		"engine.",
		"entvars_t", "Vector absmin", asOFFSET(entvars_t, absmin));
	documentation->RegisterObjectProperty(
		"Entity absolute maximum size. Is an absolute position in the world. Automatically updated by the "
		"engine.",
		"entvars_t", "Vector absmax", asOFFSET(entvars_t, absmax));
	documentation->RegisterObjectProperty("Entity minimum size. Relative to the player's origin.", "entvars_t", "Vector mins", asOFFSET(entvars_t, mins));
	documentation->RegisterObjectProperty("Entity maximum size. Relative to the player's origin.", "entvars_t", "Vector maxs", asOFFSET(entvars_t, maxs));
	documentation->RegisterObjectProperty("Entity size. Equivalent to maxs - mins", "entvars_t", "Vector size", asOFFSET(entvars_t, size));
	documentation->RegisterObjectProperty("Entity ltime. The last time this entity has executed its think function. Otherwise, is 0.",
		"entvars_t", "float ltime", asOFFSET(entvars_t, ltime));
	documentation->RegisterObjectProperty(
		"Entity next think time. The next time this entity will execute its think function, as an absolute "
		"time value. Otherwise, is 0.",
		"entvars_t", "float nextthink", asOFFSET(entvars_t, nextthink));
	documentation->RegisterObjectProperty("Entity movetype\nSee MOVETYPE enum.", "entvars_t", "int movetype", asOFFSET(entvars_t, movetype));
	documentation->RegisterObjectProperty("Entity solid flag\nSee SOLID enum.", "entvars_t", "int solid", asOFFSET(entvars_t, solid));
	documentation->RegisterObjectProperty("Entity skin index. Used by entities with studio models to set which skin to use.",
		"entvars_t", "int skin", asOFFSET(entvars_t, skin));
	documentation->RegisterObjectProperty("Entity body index. Used by entities with studio models to set which body to use.",
		"entvars_t", "int body", asOFFSET(entvars_t, body));
	documentation->RegisterObjectProperty("Entity effects\nSee EFFECTS enum.", "entvars_t", "int effects", asOFFSET(entvars_t, effects));
	documentation->RegisterObjectProperty("Entity gravity. Is a scalar between 0 and 1, inclusive.",
		"entvars_t", "float gravity", asOFFSET(entvars_t, gravity));
	documentation->RegisterObjectProperty("Entity friction. Is a scalar between 0 and 1, inclusive.",
		"entvars_t", "float friction", asOFFSET(entvars_t, friction));
	documentation->RegisterObjectProperty(
		"Player only light level. This is a single value between 0 and 255 inclusive. Use "
		"g_EngineFuncs.GetEntityIllum to retrieve this value.",
		"entvars_t", "int light_level", asOFFSET(entvars_t, light_level));
	documentation->RegisterObjectProperty("Entity sequence. Used by entities with studio models to set which sequence to use.",
		"entvars_t", "int sequence", asOFFSET(entvars_t, sequence));
	documentation->RegisterObjectProperty("Entity gait sequence. Used by entities with studio models for walking sequences.",
		"entvars_t", "int gaitsequence", asOFFSET(entvars_t, gaitsequence));
	documentation->RegisterObjectProperty(
		"Entity frame. Used by entities with studio models to set the current frame that a sequence is using. "
		"Automatically incremented.Used by brush models for animated textures: 1 for animated, 0 for off.",
		"entvars_t", "float frame", asOFFSET(entvars_t, frame));
	documentation->RegisterObjectProperty("Entity animation time. Used for internal operations.",
		"entvars_t", "float animtime", asOFFSET(entvars_t, animtime));
	documentation->RegisterObjectProperty("Entity framerate. used by entities with studio models to control sequence frame rate.",
		"entvars_t", "float framerate", asOFFSET(entvars_t, framerate));
	documentation->RegisterObjectMethod(
		"Get entity controller by index (0..3). Used by entities with studio models to get bone controller "
		"settings.",
		"entvars_t", "int8 get_controller(uint)", asFUNCTION(EntityVars_GetController), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod(
		"Set entity controller by index (0..3, 0..255) Used by entities with studio models to set bone "
		"controller settings.",
		"entvars_t", "void set_controller(uint, int8)", asFUNCTION(EntityVars_SetController), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Get entity blending by index (0..1). Used by entities with models to get x or y axis blending.",
		"entvars_t", "int8 get_blending(uint)", asFUNCTION(EntityVars_GetBlending), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod(
		"Set entity blending by index (0..1, 0..255). Used by entities with models to set x or y axis "
		"blending.",
		"entvars_t", "void set_blending(uint, int8)", asFUNCTION(EntityVars_SetBlending), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Returns whether the given flag(s) are set on the entvars spawnflags variable",
		"entvars_t", "bool SpawnFlagBitSet(int iFlags)", asFUNCTION(EntityVars_SpawnFlagBitSet), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Returns whether the given flag(s) are set on the entvars flags variable",
		"entvars_t", "bool FlagBitSet(int iFlags)", asFUNCTION(EntityVars_FlagBitSet), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("Entity rendering scale (0..255). Applies to studio and sprite models.",
		"entvars_t", "float scale", asOFFSET(entvars_t, scale));
	documentation->RegisterObjectProperty("Entity render mode. See RenderModes enum.", "entvars_t", "int rendermode", asOFFSET(entvars_t, rendermode));
	documentation->RegisterObjectProperty("Entity render amount (0..255)", "entvars_t", "float renderamt", asOFFSET(entvars_t, renderamt));
	documentation->RegisterObjectProperty("Entity render color (rgb)", "entvars_t", "Vector rendercolor", asOFFSET(entvars_t, rendercolor));
	documentation->RegisterObjectProperty("Entity render fx", "entvars_t", "int renderfx", asOFFSET(entvars_t, renderfx));
	documentation->RegisterObjectProperty("Entity health. Do not set directly unless you are absolutely sure you know what you are doing.",
		"entvars_t", "float health", asOFFSET(entvars_t, health));
	documentation->RegisterObjectProperty("Entity frags. Usually used for the number of kills or score.",
		"entvars_t", "float frags", asOFFSET(entvars_t, frags));
	documentation->RegisterObjectProperty(
		"Entity weapons bit vector. Largely used by monsters to track which weapons they have, and by players "
		"for internal purposes.",
		"entvars_t", "int weapons", asOFFSET(entvars_t, weapons));
	documentation->RegisterObjectProperty("Entity take damage flag\nSee DAMAGE enum.", "entvars_t", "float takedamage", asOFFSET(entvars_t, takedamage));
	documentation->RegisterObjectProperty("Entity dead flag\nSee DEAD enum.", "entvars_t", "int deadflag", asOFFSET(entvars_t, deadflag));
	documentation->RegisterObjectProperty("Entity view offset. Used to set an eye offset.", "entvars_t", "Vector view_ofs", asOFFSET(entvars_t, view_ofs));
	documentation->RegisterObjectProperty("Entity button bit vector. Usually used to track which buttons the player is currently pressing.",
		"entvars_t", "int button", asOFFSET(entvars_t, button));
	documentation->RegisterObjectProperty("Entity impulse bit vector. Used for various internal operations.",
		"entvars_t", "int impulse", asOFFSET(entvars_t, impulse));
	documentation->RegisterObjectProperty("Entity pointer when linked into a linked list. Used for temporary lists.",
		"entvars_t", "edict_t@ chain", asOFFSET(entvars_t, chain));
	documentation->RegisterObjectProperty("Entity damage inflictor", "entvars_t", "edict_t@ dmg_inflictor", asOFFSET(entvars_t, dmg_inflictor));
	documentation->RegisterObjectProperty("Entity enemy", "entvars_t", "edict_t@ enemy", asOFFSET(entvars_t, enemy));
	documentation->RegisterObjectProperty("Entity pointer when MOVETYPE_FOLLOW", "entvars_t", "edict_t@ aiment", asOFFSET(entvars_t, aiment));
	documentation->RegisterObjectProperty("Entity owner", "entvars_t", "edict_t@ owner", asOFFSET(entvars_t, owner));
	documentation->RegisterObjectProperty("Entity ground entity", "entvars_t", "edict_t@ groundentity", asOFFSET(entvars_t, groundentity));
	documentation->RegisterObjectProperty("Entity spawn flags", "entvars_t", "int spawnflags", asOFFSET(entvars_t, spawnflags));
	documentation->RegisterObjectProperty("Entity flags\nSee EdictFlags enum.", "entvars_t", "int flags", asOFFSET(entvars_t, flags));
	documentation->RegisterObjectProperty("Entity color map. Used for player model colors.", "entvars_t", "int colormap", asOFFSET(entvars_t, colormap));
	documentation->RegisterObjectProperty("Entity team. Reserved for future use.", "entvars_t", "int team", asOFFSET(entvars_t, team));
	documentation->RegisterObjectProperty("Entity maximum health", "entvars_t", "float max_health", asOFFSET(entvars_t, max_health));
	documentation->RegisterObjectProperty("Entity teleport time. Intended for internal operations.",
		"entvars_t", "float teleport_time", asOFFSET(entvars_t, teleport_time));
	documentation->RegisterObjectProperty("Entity armor type. Represents maximum armor (similar to max_health).",
		"entvars_t", "float armortype", asOFFSET(entvars_t, armortype));
	documentation->RegisterObjectProperty("Entity armor value", "entvars_t", "float armorvalue", asOFFSET(entvars_t, armorvalue));
	documentation->RegisterObjectProperty("Entity water level\nSee WATERLEVEL enum.", "entvars_t", "int waterlevel", asOFFSET(entvars_t, waterlevel));
	documentation->RegisterObjectProperty("Player only. Contains the type of water contents the player is currently in. See CONTENTS enum.",
		"entvars_t", "int watertype", asOFFSET(entvars_t, watertype));
	documentation->RegisterObjectProperty("The entity target. If set, is the name of one or more targets to trigger. Otherwise, is empty.",
		"entvars_t", "string_t target", asOFFSET(entvars_t, target));
	documentation->RegisterObjectProperty("The entity target name", "entvars_t", "string_t targetname", asOFFSET(entvars_t, targetname));
	documentation->RegisterObjectProperty("The entity net name. Used for various things, such as the player's name.",
		"entvars_t", "string_t netname", asOFFSET(entvars_t, netname));
	documentation->RegisterObjectProperty("The entity message. Used for various things.", "entvars_t", "string_t message", asOFFSET(entvars_t, message));
	documentation->RegisterObjectProperty("Entity damage take. Used for internal operations.", "entvars_t", "float dmg_take", asOFFSET(entvars_t, dmg_take));
	documentation->RegisterObjectProperty("Entity damage save. Used for internal operations.", "entvars_t", "float dmg_save", asOFFSET(entvars_t, dmg_save));
	documentation->RegisterObjectProperty("Entity damage. Usually stores how much damage this entity can do to other entities.",
		"entvars_t", "float dmg", asOFFSET(entvars_t, dmg));
	documentation->RegisterObjectProperty(
		"Entity damage time. Time at which this entity last took damage. Usually used to limit damage effects "
		"from being shown too often.",
		"entvars_t", "float dmgtime", asOFFSET(entvars_t, dmgtime));
	documentation->RegisterObjectProperty("Entity noise. Usually used to store a sound name.", "entvars_t", "string_t noise", asOFFSET(entvars_t, noise));
	documentation->RegisterObjectProperty("Entity noise 1. Usually used to store a sound name.", "entvars_t", "string_t noise1", asOFFSET(entvars_t, noise1));
	documentation->RegisterObjectProperty("Entity noise 2. Usually used to store a sound name.", "entvars_t", "string_t noise2", asOFFSET(entvars_t, noise2));
	documentation->RegisterObjectProperty("Entity noise 3. Usually used to store a sound name.", "entvars_t", "string_t noise3", asOFFSET(entvars_t, noise3));
	documentation->RegisterObjectProperty("Entity speed", "entvars_t", "float speed", asOFFSET(entvars_t, speed));
	documentation->RegisterObjectProperty("Player air finished. Time at which the player runs out of air when underwater.",
		"entvars_t", "float air_finished", asOFFSET(entvars_t, air_finished));
	documentation->RegisterObjectProperty("Player pain finished. Time at which the player can take damage again while drowning.",
		"entvars_t", "float pain_finished", asOFFSET(entvars_t, pain_finished));
	documentation->RegisterObjectProperty("Entity radiation suit finished. Reserved for future use.",
		"entvars_t", "float radsuit_finished", asOFFSET(entvars_t, radsuit_finished));
	documentation->RegisterObjectMethod("Entity containing entity. pContainingEntity.vars equals this entvars instance.",
		"entvars_t", "edict_t@ get_pContainingEntity() const", asFUNCTION(EntityVars_GetContainingEntity), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("Entity player class. Reserved for future use.", "entvars_t", "int playerclass", asOFFSET(entvars_t, playerclass));
	documentation->RegisterObjectProperty("Entity maximum speed", "entvars_t", "float maxspeed", asOFFSET(entvars_t, maxspeed));
	documentation->RegisterObjectProperty("Player field of view", "entvars_t", "float fov", asOFFSET(entvars_t, fov));
	documentation->RegisterObjectProperty("Player weapon animation", "entvars_t", "int weaponanim", asOFFSET(entvars_t, weaponanim));
	documentation->RegisterObjectProperty("Entity push milliseconds. Used for internal operations.",
		"entvars_t", "int pushmsec", asOFFSET(entvars_t, pushmsec));
	documentation->RegisterObjectProperty("Entity ducking flag. Used for internal operations.", "entvars_t", "int bInDuck", asOFFSET(entvars_t, bInDuck));
	documentation->RegisterObjectProperty("Entity time of step sound. Used for internal operations.",
		"entvars_t", "int flTimeStepSound", asOFFSET(entvars_t, flTimeStepSound));
	documentation->RegisterObjectProperty("Entity swim time. Used for internal operations.", "entvars_t", "int flSwimTime", asOFFSET(entvars_t, flSwimTime));
	documentation->RegisterObjectProperty("Entity duck time. Used for internal operations.", "entvars_t", "int flDuckTime", asOFFSET(entvars_t, flDuckTime));
	documentation->RegisterObjectProperty("Entity step left flag. Used for internal operations.",
		"entvars_t", "int iStepLeft", asOFFSET(entvars_t, iStepLeft));
	documentation->RegisterObjectProperty("Player fall velocity.", "entvars_t", "float flFallVelocity", asOFFSET(entvars_t, flFallVelocity));
	documentation->RegisterObjectProperty("Entity gamestate. Reserved for future use.", "entvars_t", "int gamestate", asOFFSET(entvars_t, gamestate));
	documentation->RegisterObjectProperty("Player old buttons. Previous frame's contents of buttons.",
		"entvars_t", "int oldbuttons", asOFFSET(entvars_t, oldbuttons));
	documentation->RegisterObjectProperty("Entity group info. Used for internal operations.", "entvars_t", "int groupinfo", asOFFSET(entvars_t, groupinfo));
	documentation->RegisterObjectProperty("User variable 1 (integer)", "entvars_t", "int iuser1", asOFFSET(entvars_t, iuser1));
	documentation->RegisterObjectProperty("User variable 2 (integer)", "entvars_t", "int iuser2", asOFFSET(entvars_t, iuser2));
	documentation->RegisterObjectProperty("User variable 3 (integer)", "entvars_t", "int iuser3", asOFFSET(entvars_t, iuser3));
	documentation->RegisterObjectProperty("User variable 4 (integer)", "entvars_t", "int iuser4", asOFFSET(entvars_t, iuser4));
	documentation->RegisterObjectProperty("User variable 1 (float)", "entvars_t", "float fuser1", asOFFSET(entvars_t, fuser1));
	documentation->RegisterObjectProperty("User variable 2 (float)", "entvars_t", "float fuser2", asOFFSET(entvars_t, fuser2));
	documentation->RegisterObjectProperty("User variable 3 (float)", "entvars_t", "float fuser3", asOFFSET(entvars_t, fuser3));
	documentation->RegisterObjectProperty("User variable 4 (float)", "entvars_t", "float fuser4", asOFFSET(entvars_t, fuser4));
	documentation->RegisterObjectProperty("User variable 1 (Vector)", "entvars_t", "Vector vuser1", asOFFSET(entvars_t, vuser1));
	documentation->RegisterObjectProperty("User variable 2 (Vector)", "entvars_t", "Vector vuser2", asOFFSET(entvars_t, vuser2));
	documentation->RegisterObjectProperty("User variable 3 (Vector)", "entvars_t", "Vector vuser3", asOFFSET(entvars_t, vuser3));
	// This binding places vuser4 at euser4, past the real vuser4 member
	documentation->RegisterObjectProperty("User variable 4 (Vector)", "entvars_t", "Vector vuser4", asOFFSET(entvars_t, euser4));
	documentation->RegisterObjectProperty("User variable 1 (Edict)", "entvars_t", "edict_t@ euser1", asOFFSET(entvars_t, euser1));
	documentation->RegisterObjectProperty("User variable 2 (Edict)", "entvars_t", "edict_t@ euser2", asOFFSET(entvars_t, euser2));
	documentation->RegisterObjectProperty("User variable 3 (Edict)", "entvars_t", "edict_t@ euser3", asOFFSET(entvars_t, euser3));
	documentation->RegisterObjectProperty("User variable 4 (Edict)", "entvars_t", "edict_t@ euser4", asOFFSET(entvars_t, euser4));
	documentation->RegisterObjectProperty("Flag telling whether this edict is free", "edict_t", "const int free", asOFFSET(edict_t, free));
	documentation->RegisterObjectProperty("Serial number", "edict_t", "const int serialnumber", asOFFSET(edict_t, serialnumber));
	documentation->RegisterObjectProperty("Head node", "edict_t", "const int headnode", asOFFSET(edict_t, headnode));
	documentation->RegisterObjectProperty("Number of leafs", "edict_t", "const int num_leafs", asOFFSET(edict_t, num_leafs));
	documentation->RegisterObjectMethod("get leafnums", "edict_t", "int16 get_leafnums(uint)", asFUNCTION(Edict_GetLeafNums), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("When this edict was last freed", "edict_t", "const float freetime", asOFFSET(edict_t, freetime));
	documentation->RegisterObjectProperty("Entity variables instance", "edict_t", "entvars_t vars", asOFFSET(edict_t, v));
}
