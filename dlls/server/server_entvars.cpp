#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "server/CCustomKeyvalues.h"
#include "server/server_entvars.h"
#include "in_buttons.h"
#include <stddef.h>

const char *ChangeValueTypeStr( int type )
{
	switch ( type )
	{
	case 0: return "REPLACE";
	case 1: return "ADD";
	case 2: return "MULTIPLY";
	case 3: return "SUBTRACT";
	case 4: return "DIVIDE";
	case 5: return "LOGIC AND";
	case 6: return "LOGIC OR";
	case 7: return "BITWISE NEGATED LOGIC AND";
	case 8: return "BITWISE NEGATED LOGIC OR";
	case 9: return "DIRECTION TO ANGLES";
	case 10: return "ANGLES TO DIRECTION";
	case 11: return "CONCATENATE";
	case 12: return "MODULO";
	case 13: return "EXCLUSIVE OR";
	case 14: return "BITWISE NEGATED EXCLUSIVE OR";
	case 15: return "STRING PRINT FORMAT";
	case 16: return "POWER";
	case 17: return "SINE";
	case 18: return "COSINE";
	case 19: return "TANGENT";
	case 20: return "ARCSINE";
	case 21: return "ARCCOSINE";
	case 22: return "ARCTANGENT";
	case 23: return "COTANGENT";
	case 24: return "ARCCOTANGENT";
	default: return "UNKNOWN";
	}
}

struct EntvarField
{
	const char		*m_pszName;
	unsigned int	m_uiOffset;
	entvartype_s	m_Type;
};

#define ENTVAR_FIELD(name, type) { #name, offsetof(entvars_t, name), type }

static const EntvarField g_EntvarFields[ENTVAR_FIELD_COUNT] =
{
	ENTVAR_FIELD(classname, VAR_STRING),
	ENTVAR_FIELD(globalname, VAR_STRING),
	ENTVAR_FIELD(origin, VAR_VECTOR),
	ENTVAR_FIELD(oldorigin, VAR_VECTOR),
	ENTVAR_FIELD(velocity, VAR_VECTOR),
	ENTVAR_FIELD(basevelocity, VAR_VECTOR),
	ENTVAR_FIELD(clbasevelocity, VAR_VECTOR),
	ENTVAR_FIELD(movedir, VAR_VECTOR),
	ENTVAR_FIELD(angles, VAR_VECTOR),
	ENTVAR_FIELD(avelocity, VAR_VECTOR),
	ENTVAR_FIELD(punchangle, VAR_VECTOR),
	ENTVAR_FIELD(v_angle, VAR_VECTOR),
	ENTVAR_FIELD(endpos, VAR_VECTOR),
	ENTVAR_FIELD(startpos, VAR_VECTOR),
	ENTVAR_FIELD(impacttime, VAR_FLOAT),
	ENTVAR_FIELD(starttime, VAR_FLOAT),
	ENTVAR_FIELD(fixangle, VAR_INTEGER),
	ENTVAR_FIELD(idealpitch, VAR_FLOAT),
	ENTVAR_FIELD(pitch_speed, VAR_FLOAT),
	ENTVAR_FIELD(ideal_yaw, VAR_FLOAT),
	ENTVAR_FIELD(yaw_speed, VAR_FLOAT),
	ENTVAR_FIELD(modelindex, VAR_INTEGER),
	ENTVAR_FIELD(model, VAR_STRING),
	ENTVAR_FIELD(viewmodel, VAR_INTEGER),
	ENTVAR_FIELD(weaponmodel, VAR_INTEGER),
	ENTVAR_FIELD(absmin, VAR_VECTOR),
	ENTVAR_FIELD(absmax, VAR_VECTOR),
	ENTVAR_FIELD(mins, VAR_VECTOR),
	ENTVAR_FIELD(maxs, VAR_VECTOR),
	ENTVAR_FIELD(size, VAR_VECTOR),
	ENTVAR_FIELD(ltime, VAR_FLOAT),
	ENTVAR_FIELD(nextthink, VAR_FLOAT),
	ENTVAR_FIELD(movetype, VAR_INTEGER),
	ENTVAR_FIELD(solid, VAR_INTEGER),
	ENTVAR_FIELD(skin, VAR_INTEGER),
	ENTVAR_FIELD(body, VAR_INTEGER),
	ENTVAR_FIELD(effects, VAR_INTEGER),
	ENTVAR_FIELD(gravity, VAR_FLOAT),
	ENTVAR_FIELD(friction, VAR_FLOAT),
	ENTVAR_FIELD(light_level, VAR_INTEGER),
	ENTVAR_FIELD(sequence, VAR_INTEGER),
	ENTVAR_FIELD(gaitsequence, VAR_INTEGER),
	ENTVAR_FIELD(frame, VAR_FLOAT),
	ENTVAR_FIELD(animtime, VAR_FLOAT),
	ENTVAR_FIELD(framerate, VAR_FLOAT),
	ENTVAR_FIELD(controller, VAR_BYTE4),
	ENTVAR_FIELD(blending, VAR_BYTE2),
	ENTVAR_FIELD(scale, VAR_FLOAT),
	ENTVAR_FIELD(rendermode, VAR_INTEGER),
	ENTVAR_FIELD(renderamt, VAR_FLOAT),
	ENTVAR_FIELD(rendercolor, VAR_VECTOR),
	ENTVAR_FIELD(renderfx, VAR_INTEGER),
	ENTVAR_FIELD(health, VAR_FLOAT),
	ENTVAR_FIELD(frags, VAR_FLOAT),
	ENTVAR_FIELD(weapons, VAR_INTEGER),
	ENTVAR_FIELD(takedamage, VAR_FLOAT),
	ENTVAR_FIELD(deadflag, VAR_INTEGER),
	ENTVAR_FIELD(view_ofs, VAR_VECTOR),
	ENTVAR_FIELD(button, VAR_INTEGER),
	ENTVAR_FIELD(impulse, VAR_INTEGER),
	ENTVAR_FIELD(chain, VAR_EDICT),
	ENTVAR_FIELD(dmg_inflictor, VAR_EDICT),
	ENTVAR_FIELD(enemy, VAR_EDICT),
	ENTVAR_FIELD(aiment, VAR_EDICT),
	ENTVAR_FIELD(owner, VAR_EDICT),
	ENTVAR_FIELD(groundentity, VAR_EDICT),
	ENTVAR_FIELD(spawnflags, VAR_INTEGER),
	ENTVAR_FIELD(flags, VAR_INTEGER),
	ENTVAR_FIELD(colormap, VAR_INTEGER),
	ENTVAR_FIELD(team, VAR_INTEGER),
	ENTVAR_FIELD(max_health, VAR_FLOAT),
	ENTVAR_FIELD(teleport_time, VAR_FLOAT),
	ENTVAR_FIELD(armortype, VAR_FLOAT),
	ENTVAR_FIELD(armorvalue, VAR_FLOAT),
	ENTVAR_FIELD(waterlevel, VAR_INTEGER),
	ENTVAR_FIELD(watertype, VAR_INTEGER),
	ENTVAR_FIELD(target, VAR_STRING),
	ENTVAR_FIELD(targetname, VAR_STRING),
	ENTVAR_FIELD(netname, VAR_STRING),
	ENTVAR_FIELD(message, VAR_STRING),
	ENTVAR_FIELD(dmg_take, VAR_FLOAT),
	ENTVAR_FIELD(dmg_save, VAR_FLOAT),
	ENTVAR_FIELD(dmg, VAR_FLOAT),
	ENTVAR_FIELD(dmgtime, VAR_FLOAT),
	ENTVAR_FIELD(noise, VAR_STRING),
	ENTVAR_FIELD(noise1, VAR_STRING),
	ENTVAR_FIELD(noise2, VAR_STRING),
	ENTVAR_FIELD(noise3, VAR_STRING),
	ENTVAR_FIELD(speed, VAR_FLOAT),
	ENTVAR_FIELD(air_finished, VAR_FLOAT),
	ENTVAR_FIELD(pain_finished, VAR_FLOAT),
	ENTVAR_FIELD(radsuit_finished, VAR_FLOAT),
	ENTVAR_FIELD(pContainingEntity, VAR_EDICT),
	ENTVAR_FIELD(playerclass, VAR_INTEGER),
	ENTVAR_FIELD(maxspeed, VAR_FLOAT),
	ENTVAR_FIELD(fov, VAR_FLOAT),
	ENTVAR_FIELD(weaponanim, VAR_INTEGER),
	ENTVAR_FIELD(pushmsec, VAR_INTEGER),
	ENTVAR_FIELD(bInDuck, VAR_INTEGER),
	ENTVAR_FIELD(flTimeStepSound, VAR_INTEGER),
	ENTVAR_FIELD(flSwimTime, VAR_INTEGER),
	ENTVAR_FIELD(flDuckTime, VAR_INTEGER),
	ENTVAR_FIELD(iStepLeft, VAR_INTEGER),
	ENTVAR_FIELD(flFallVelocity, VAR_FLOAT),
	ENTVAR_FIELD(gamestate, VAR_INTEGER),
	ENTVAR_FIELD(oldbuttons, VAR_INTEGER),
	ENTVAR_FIELD(groupinfo, VAR_INTEGER),
	ENTVAR_FIELD(iuser1, VAR_INTEGER),
	ENTVAR_FIELD(iuser2, VAR_INTEGER),
	ENTVAR_FIELD(iuser3, VAR_INTEGER),
	ENTVAR_FIELD(iuser4, VAR_INTEGER),
	ENTVAR_FIELD(fuser1, VAR_FLOAT),
	ENTVAR_FIELD(fuser2, VAR_FLOAT),
	ENTVAR_FIELD(fuser3, VAR_FLOAT),
	ENTVAR_FIELD(fuser4, VAR_FLOAT),
	ENTVAR_FIELD(vuser1, VAR_VECTOR),
	ENTVAR_FIELD(vuser2, VAR_VECTOR),
	ENTVAR_FIELD(vuser3, VAR_VECTOR),
	ENTVAR_FIELD(vuser4, VAR_VECTOR),
	ENTVAR_FIELD(euser1, VAR_EDICT),
	ENTVAR_FIELD(euser2, VAR_EDICT),
	ENTVAR_FIELD(euser3, VAR_EDICT),
	ENTVAR_FIELD(euser4, VAR_EDICT)
};

#undef ENTVAR_FIELD

enum { MAX_ENTVAR_INDEX_TEXT = 4 };

CCustomKeyvalues *gEntVarList = NULL;

void RemoveGlobalEntvarsList(void)
{
	delete gEntVarList;
	gEntVarList = NULL;
}

void CWorld::SetupEntVars( void )
{
	RemoveGlobalEntvarsList();
	gEntVarList = new CCustomKeyvalues( DEFAULT_CUSTOM_KEYVALUES_MAX_HEIGHT );

	for ( int index = 0; index < ENTVAR_FIELD_COUNT; index++ )
	{
		char text[MAX_ENTVAR_INDEX_TEXT];

		snprintf( text, sizeof( text ), "%i", index );
		gEntVarList->SetKeyvalue( g_EntvarFields[index].m_pszName, text, VAR_CUSTOM, OWNERSHIP_BY_TYPE );
	}
}

entvartype_s GetEntvarField(entvars_t *vars, const char *name, void **value, int &index)
{
	entvartype_s type = VAR_INVALID;

	index = INVALID_ENTVAR_INDEX;

	int *field = (int *)gEntVarList->GetKeyvalue(name, type);

	if (type != VAR_INTEGER)
		return VAR_INVALID;

	index = *field;

	if (index >= 0 && index < ENTVAR_FIELD_COUNT)
	{
		*value = (char *)vars + g_EntvarFields[index].m_uiOffset;
		return g_EntvarFields[index].m_Type;
	}

	return VAR_INTEGER;
}

struct EntvarNamedValue
{
	const char *name;
	int        value;
};

enum
{
	ENTVAR_INDEX_MOVETYPE = 32,
	ENTVAR_INDEX_SOLID = 33,
	ENTVAR_INDEX_EFFECTS = 36,
	ENTVAR_INDEX_TAKEDAMAGE = 55,
	ENTVAR_INDEX_DEADFLAG = 56,
	ENTVAR_INDEX_BUTTON = 58,
	ENTVAR_INDEX_FLAGS = 67,
	ENTVAR_INDEX_WATERLEVEL = 74
};

#define ENTVAR_PREFIX_LENGTH( prefix ) ( sizeof( prefix ) - 1 )

static const EntvarNamedValue g_MovetypeValues[] =
{
	{ "NONE", MOVETYPE_NONE }, { "WALK", MOVETYPE_WALK }, { "STEP", MOVETYPE_STEP }, { "FLY", MOVETYPE_FLY },
	{ "TOSS", MOVETYPE_TOSS }, { "PUSH", MOVETYPE_PUSH }, { "NOCLIP", MOVETYPE_NOCLIP }, { "FLYMISSILE", MOVETYPE_FLYMISSILE },
	{ "BOUNCE", MOVETYPE_BOUNCE }, { "BOUNCEMISSILE", MOVETYPE_BOUNCEMISSILE }, { "FOLLOW", MOVETYPE_FOLLOW }, { "PUSHSTEP", MOVETYPE_PUSHSTEP },
};

static const EntvarNamedValue g_SolidValues[] =
{
	{ "NOT", SOLID_NOT }, { "TRIGGER", SOLID_TRIGGER }, { "BBOX", SOLID_BBOX }, { "SLIDEBOX", SOLID_SLIDEBOX }, { "BSP", SOLID_BSP },
};

static const EntvarNamedValue g_EffectValues[] =
{
	{ "BRIGHTFIELD", EF_BRIGHTFIELD }, { "MUZZLEFLASH", EF_MUZZLEFLASH }, { "BRIGHTLIGHT", EF_BRIGHTLIGHT },
	{ "DIMLIGHT", EF_DIMLIGHT }, { "INVLIGHT", EF_INVLIGHT }, { "NOINTERP", EF_NOINTERP }, { "LIGHT", EF_LIGHT },
	{ "NODRAW", EF_NODRAW }, { "NOANIMTEXTURES", 256 }, { "FRAMEANIMTEXTURES", 512 },
};

static const EntvarNamedValue g_TakeDamageValues[] =
{
	{ "NO", DAMAGE_NO }, { "YES", DAMAGE_YES }, { "AIM", DAMAGE_AIM },
};

static const EntvarNamedValue g_DeadFlagValues[] =
{
	{ "NO", DEAD_NO }, { "DYING", DEAD_DYING }, { "DEAD", DEAD_DEAD }, { "RESPAWNABLE", DEAD_RESPAWNABLE }, { "DISCARDBODY", DEAD_DISCARDBODY },
};

static const EntvarNamedValue g_ButtonValues[] =
{
	{ "ATTACK", IN_ATTACK }, { "JUMP", IN_JUMP }, { "DUCK", IN_DUCK }, { "FORWARD", IN_FORWARD },
	{ "BACK", IN_BACK }, { "USE", IN_USE }, { "CANCEL", IN_CANCEL }, { "LEFT", IN_LEFT },
	{ "RIGHT", IN_RIGHT }, { "MOVELEFT", IN_MOVELEFT }, { "MOVERIGHT", IN_MOVERIGHT },
	{ "ATTACK2", IN_ATTACK2 }, { "RUN", IN_RUN }, { "RELOAD", IN_RELOAD },
	{ "ALT1", IN_ALT1 }, { "SCORE", IN_SCORE },
};

static const EntvarNamedValue g_EntityFlagValues[] =
{
	{ "FLY", FL_FLY }, { "SWIM", FL_SWIM }, { "CONVEYOR", FL_CONVEYOR }, { "CLIENT", FL_CLIENT },
	{ "INWATER", FL_INWATER }, { "MONSTER", FL_MONSTER }, { "GODMODE", FL_GODMODE }, { "NOTARGET", FL_NOTARGET },
	{ "SKIPLOCALHOST", FL_SKIPLOCALHOST }, { "ONGROUND", FL_ONGROUND }, { "PARTIALGROUND", FL_PARTIALGROUND },
	{ "WATERJUMP", FL_WATERJUMP }, { "FROZEN", FL_FROZEN }, { "FAKECLIENT", FL_FAKECLIENT },
	{ "DUCKING", FL_DUCKING }, { "FLOAT", FL_FLOAT },
};

static const EntvarNamedValue g_WaterlevelValues[] =
{
	{ "DRY", 0 }, { "SWIM", 1 }, { "WAIST", 2 }, { "HEAD", 3 },
};

int ValueForPEVFlag( const char *name, int index, bool &valid )
{
	const bool invert = name[0] == '~';
	const char *valueName = name + ( invert ? 1 : 0 );
	const EntvarNamedValue *values = NULL;
	unsigned int count = 0;
	unsigned int prefixLength = 0;

	valid = true;

	switch ( index )
	{
	case ENTVAR_INDEX_MOVETYPE:
		values = g_MovetypeValues;
		count = ARRAYSIZE( g_MovetypeValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "MOVETYPE_" );
		break;
	case ENTVAR_INDEX_SOLID:
		values = g_SolidValues;
		count = ARRAYSIZE( g_SolidValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "SOLID_" );
		break;
	case ENTVAR_INDEX_EFFECTS:
		values = g_EffectValues;
		count = ARRAYSIZE( g_EffectValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "EF_" );
		break;
	case ENTVAR_INDEX_TAKEDAMAGE:
		values = g_TakeDamageValues;
		count = ARRAYSIZE( g_TakeDamageValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "DAMAGE_" );
		break;
	case ENTVAR_INDEX_DEADFLAG:
		values = g_DeadFlagValues;
		count = ARRAYSIZE( g_DeadFlagValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "DEAD_" );
		break;
	case ENTVAR_INDEX_BUTTON:
		values = g_ButtonValues;
		count = ARRAYSIZE( g_ButtonValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "IN_" );
		break;
	case ENTVAR_INDEX_FLAGS:
		values = g_EntityFlagValues;
		count = ARRAYSIZE( g_EntityFlagValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "FL_" );
		break;
	case ENTVAR_INDEX_WATERLEVEL:
		values = g_WaterlevelValues;
		count = ARRAYSIZE( g_WaterlevelValues );
		prefixLength = ENTVAR_PREFIX_LENGTH( "WATERLEVEL_" );
		break;
	default:
		valid = false;
		return invert ? ~0 : 0;
	}

	// This skips a fixed prefix by length, without checking the prefix text
	if ( strlen( valueName ) >= prefixLength )
		valueName += prefixLength;

	for ( unsigned int i = 0; i < count; i++ )
	{
		if ( strcmp( valueName, values[i].name ) == 0 )
			return invert ? ~values[i].value : values[i].value;
	}

	// Unknown waterlevel identifiers retain the success flag
	if ( index != ENTVAR_INDEX_WATERLEVEL )
		valid = false;

	return invert ? ~0 : 0;
}
