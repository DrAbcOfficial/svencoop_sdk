#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "gamerules/skill.h"
#include "server/server_filesystem.h"
#include "server/server_support.h"
#include "server/CCfgFile.h"
#include "scripting/core/CASServerManager.h"
#include "gamerules/CClassicMode.h"
#include "server/CMap.h"
#include "server/CCVarSystem.h"
#include "sound/server_sound.h"
#include "scripting/core/CASCommand.h"
#include "scripting/core/CASConCommandSystem.h"
#include <ctype.h>

#define CFG_MAX_GRAVITY	32767
#define CFG_MIN_ZMAX	4096.0
#define CFG_MAX_ZMAX	454046.777344

extern DLL_GLOBAL int g_iSkillLevel;
extern void ServerCommandQueueCallback( ConCommandExecuteResult::Type result,
                                        const char *name, const CASCommand *command );

CCfgFile::CCfgFile()
{
	m_pCommands = NULL;
	m_bValid = false;
}

CCfgFile::CCfgFile(const char *fileName)
{
	m_pCommands = NULL;
	m_bValid = false;

	FileHandle_t file = g_pFileSystem->Open(fileName, "r", "GAME|DEFAULTGAME");

	if (!file)
	{
		ALERT(at_console, "Couldn't open %s, no config file parsed\n", fileName);
		return;
	}

	ALERT(at_console, "Parsing %s\n", fileName);

	char line[MAX_LINE_LENGTH];

	while (!g_pFileSystem->EndOfFile(file))
	{
		if (!g_pFileSystem->ReadLine(line, sizeof(line), file))
			continue;

		unsigned int length = strlen(line);

		if (!length)
			continue;

		for (unsigned int i = 0; i < length - 1; i++)
		{
			if ((line[i] == '/' && line[i + 1] == '/') || line[i] == '#')
			{
				line[i] = 0;
				length = i + 1;
				break;
			}
		}

		if (line[length - 1] == '\n')
			line[length - 1] = 0;

		if (line[0])
			ParseLine(line);
	}

	m_bValid = true;
	g_pFileSystem->Close(file);
}

CCfgFile::~CCfgFile()
{
	CCfgCommand *command = m_pCommands;

	while (command)
	{
		CCfgCommand *next = command->m_pNext;

		delete command;
		command = next;
	}
}

void CCfgFile::ParseLine(char *line)
{
	for (unsigned int i = 0; i < strlen(line); i++)
	{
		if (line[i] == '\n' || line[i] == '\r')
			line[i] = 0;
	}

	char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
	unsigned int token = 0;
	unsigned int length = 0;
	bool quoted = false;
	bool hasToken = false;

	for (unsigned int i = 0; i < MAX_TOKENS; i++)
		tokens[i][0] = 0;

	for (const char *cursor = line; *cursor; cursor++)
	{
		if ((*cursor == ' ' || *cursor == '\t') && !quoted)
		{
			if (hasToken)
				tokens[token++][length] = 0;

			hasToken = false;
			length = 0;
		}
		else
		{
			hasToken = true;

			if (*cursor == '"')
				quoted = !quoted;
			else
				tokens[token][length++] = *cursor;
		}
	}

	tokens[token][length] = 0;

	CCfgCommand *command = new CCfgCommand;

	command->SetCommand(tokens[0], tokens[1][0] ? tokens[1] : NULL,
		tokens[2][0] ? tokens[2] : NULL, tokens[3][0] ? tokens[3] : NULL,
		tokens[4][0] ? tokens[4] : NULL, tokens[5][0] ? tokens[5] : NULL);

	if (m_pCommands)
	{
		CCfgCommand *last = m_pCommands;

		while (last->m_pNext)
			last = last->m_pNext;

		command->m_pPrev = last;
		command->m_pNext = NULL;
		last->m_pNext = command;
	}
	else
	{
		m_pCommands = command;
		command->m_pNext = NULL;
		command->m_pPrev = NULL;
	}
}

CCfgCommand *CCfgFile::Find(const char *command)
{
	for (CCfgCommand *entry = m_pCommands; entry; entry = entry->m_pNext)
	{
		if (*entry == command)
			return entry;
	}

	return NULL;
}

void CCfgFile::GiveWeaponToPlayer( CBasePlayer *player, CCfgCommand *command, const char *name )
{
	player->GiveNamedItem( name, SF_CREATEDWEAPON | SF_GIVENITEM, 0 );
}

void CCfgFile::GiveAmmoToPlayer( CBasePlayer *player, CCfgCommand *command,
                                int amount, const char *name, int maxCarry )
{
	int count = atol( command->GetArg( 0 ) );

	if ( count < 1 )
		count = 1;

	do
	{
		player->GiveAmmo( amount, name, maxCarry, true, NULL );
		count--;
	} while ( count > 0 );
}

bool CCfgFile::ApplyCfgOnPlayer( CBasePlayer *player )
{
	if ( !player )
		return false;

	if ( !m_pCommands )
	{
		player->GiveNamedItem( "weapon_crowbar", 0, 0 );
		player->GiveNamedItem( "weapon_9mmhandgun", 0, 0 );
		return true;
	}

	static const char *weapons[] =
	{
		"weapon_crowbar", "weapon_pipewrench", "weapon_grapple", "weapon_9mmhandgun",
		"weapon_357", "weapon_uzi", "weapon_eagle", "weapon_9mmAR", "weapon_shotgun",
		"weapon_crossbow", "weapon_m16", "weapon_rpg", "weapon_gauss", "weapon_egon",
		"weapon_hornetgun", "weapon_handgrenade", "weapon_satchel", "weapon_tripmine",
		"weapon_snark", "weapon_saw", "weapon_sniperrifle", "weapon_sporelauncher",
		"weapon_shockrifle", "weapon_displacer"
	};

	struct AmmoGrant
	{
		const char *command;
		int         amount;
		const char *name;
		int         maxCarry;
	};

	static const AmmoGrant ammunition[] =
	{
		{ "ammo_9mm", 17, "9MM", 250 },
		{ "ammo_9mmbox", 200, "9MM", 250 },
		{ "ammo_357", 6, "357", 36 },
		{ "ammo_buckshot", 12, "buckshot", 125 },
		{ "ammo_crossbow", 5, "bolts", 50 },
		{ "ammo_556", 200, "556", 600 },
		{ "ammo_rpgclip", 1, "rockets", 5 },
		{ "ammo_gaussclip", 20, "uranium", 100 },
		{ "ammo_762", 5, "m40a1", 15 },
		{ "ammo_sporeclip", 1, "sporeclip", 30 }
	};

	for ( CCfgCommand *entry = m_pCommands; entry; entry = entry->m_pNext )
	{
		if ( !entry->m_bPlayerCommand )
			continue;

		if ( *entry == "weapon_mp5" || *entry == "weapon_9mmar" )
		{
			GiveWeaponToPlayer( player, entry, "weapon_9mmAR" );
			continue;
		}

		bool handled = false;

		for ( unsigned int index = 0; index < ARRAYSIZE( weapons ); index++ )
		{
			if ( *entry == weapons[index] )
			{
				GiveWeaponToPlayer( player, entry, entry->GetCommand() );
				handled = true;
				break;
			}
		}

		if ( handled )
			continue;

		if ( *entry == "weapon_glock" )
			GiveWeaponToPlayer( player, entry, "weapon_9mmhandgun" );
		else if ( *entry == "weapon_uziakimbo" )
		{
			GiveWeaponToPlayer( player, entry, "weapon_uzi" );
			GiveWeaponToPlayer( player, entry, "weapon_uzi" );
		}
		else if ( *entry == "weapon_desert_eagle" )
			GiveWeaponToPlayer( player, entry, "weapon_eagle" );
		else if ( *entry == "ammo_9mmAR" || *entry == "ammo_9mmar" )
			GiveAmmoToPlayer( player, entry, CClassicMode::GetInstance()->IsEnabled() ? 50 : 30, "9MM", 250 );
		else if ( *entry == "ammo_ARgrenades" || *entry == "ammo_argrenades" )
			GiveAmmoToPlayer( player, entry, 2, "ARgrenades", 10 );
		else
		{
			for ( unsigned int index = 0; index < ARRAYSIZE( ammunition ); index++ )
			{
				if ( *entry == ammunition[index].command )
				{
					GiveAmmoToPlayer( player, entry, ammunition[index].amount,
					                  ammunition[index].name, ammunition[index].maxCarry );
					handled = true;
					break;
				}
			}

			if ( handled )
				continue;

			if ( *entry == "item_longjump" )
			{
				player->m_fLongJump = TRUE;
				g_engfuncs.pfnSetPhysicsKeyValue( player->edict(), "slj", "1" );
			}
			else if ( *entry == "starthealth" )
			{
				int health = atol( entry->GetArg( 0 ) );

				if ( health > 0 )
				{
					player->pev->health = health;

					if ( health > 100 )
						player->pev->max_health = health;
				}
			}
			else if ( *entry == "startarmor" )
			{
				int armor = atol( entry->GetArg( 0 ) );

				if ( armor > 0 )
				{
					player->pev->armorvalue = armor;

					if ( armor > 100 )
						player->pev->armortype = armor;

					if ( player->pev->armorvalue <= 0 )
						player->pev->armorvalue = 0;
				}
			}
			else if ( *entry == "maxhealth" )
			{
				int health = atol( entry->GetArg( 0 ) );

				if ( health > 0 )
					player->pev->max_health = health;
			}
			else if ( *entry == "maxarmor" )
			{
				int armor = atol( entry->GetArg( 0 ) );

				if ( armor > 0 )
					player->pev->armortype = armor;
			}
			else if ( *entry == "nosuit" )
				player->SetHasSuit( false );
			else if ( *entry == "all_weapons" )
			{
				static const char *items[] =
				{
					"weapon_crowbar", "weapon_pipewrench", "weapon_grapple", "weapon_medkit",
					"weapon_9mmhandgun", "ammo_9mmclip", "weapon_357", "ammo_357",
					"weapon_uziakimbo", "ammo_9mmAR", "weapon_eagle", "ammo_357",
					"weapon_9mmAR", "ammo_9mmAR", "weapon_shotgun", "ammo_buckshot",
					"weapon_crossbow", "ammo_crossbow", "weapon_m16", "ammo_556", "ammo_ARgrenades",
					"weapon_rpg", "ammo_rpgclip", "weapon_gauss", "ammo_gaussclip",
					"weapon_egon", "ammo_gaussclip", "weapon_hornetgun", "weapon_handgrenade",
					"weapon_satchel", "weapon_tripmine", "weapon_snark", "weapon_saw", "ammo_556",
					"weapon_sniperrifle", "ammo_762", "weapon_sporelauncher",
					"ammo_sporeclip", "ammo_sporeclip", "ammo_sporeclip"
				};

				player->SetHasSuit( true );
				player->m_fLongJump = TRUE;
				g_engfuncs.pfnSetPhysicsKeyValue( player->edict(), "slj", "1" );

				for ( unsigned int index = 0; index < ARRAYSIZE( items ); index++ )
					player->GiveNamedItem( items[index], 0, 0 );
			}
			else if ( *entry == "weapon_medkit" )
				continue;
			else if ( !strncmp( entry->GetCommand(), "weapon_", 7 ) )
				GiveWeaponToPlayer( player, entry, entry->GetCommand() );
			else if ( !strncmp( entry->GetCommand(), "item_", 5 ) || !strncmp( entry->GetCommand(), "ammo_", 5 ) )
				player->GiveNamedItem( entry->GetCommand(), SF_GIVENITEM, 0 );
			else
			{
				char message[256];

				sprintf( message, "Unknown player cfg command: %s\n", entry->GetCommand() );
				g_engfuncs.pfnClientPrintf( player->edict(), print_console, message );
			}
		}
	}

	return true;
}

bool CCfgFile::ApplyCfgOnMap( void )
{
	CVAR_SET_STRING( "sv_allowdownload", "1" );
	CVAR_SET_STRING( "sv_send_resources", "1" );

	static const char *integerCvars[] =
	{
		"mp_monsterpoints", "mp_timelimit", "mp_fraglimit", "mp_telefrag", "mp_flashlight",
		"mp_suitpower", "mp_falldamage", "mp_weaponstay", "npc_dropweapons",
		"sv_accelerate", "sv_airaccelerate", "sv_airmove", "sv_friction", "sv_maxspeed",
		"sv_maxvelocity", "sv_wateraccelerate", "sv_wateramp", "sv_waterfriction",
		"mp_barnacle_paralyze", "weaponmode_9mmhandgun", "weaponmode_eagle",
		"weaponmode_shotgun", "weaponmode_mp5", "weaponmode_crossbow", "weaponmode_rpg",
		"weaponmode_displacer", "mp_banana", "mp_no_akimbo_uzis", "mp_respawndelay",
		"mp_multiplespawn", "mp_grapple_mode", "mp_disable_autoclimb",
		"mp_disable_pcbalancing", "mp_disable_medkit_points", "mp_disable_player_rappel",
		"sv_ai_enemy_detection_mode", "mp_ammo_droprules", "mp_weapon_droprules",
		"angelscript_compiler_treat_warnings_as_errors", "mp_hevsuit_voice", "mp_footsteps",
		"mp_survival_supported", "mp_survival_starton", "mp_survival_startdelay", "mp_survival_voteallow"
	};
	static const char *stringCvars[] =
	{
		"mp_pcbalancing_factorlist", "mp_weapon_respawndelay", "mp_ammo_respawndelay",
		"mp_item_respawndelay", "mp_observer_mode", "mp_observer_cyclic",
		"mp_weaponfadedelay", "mp_modelselection", "mp_survival_nextmap"
	};

	char message[128 + 1] = {};

	for ( CCfgCommand *entry = m_pCommands; entry; entry = entry->m_pNext )
	{
		const char *value = entry->GetArg( 0 );
		bool handled = false;

		for ( unsigned int index = 0; index < ARRAYSIZE( integerCvars ); index++ )
		{
			if ( *entry == integerCvars[index] )
			{
				CVAR_SET_FLOAT( integerCvars[index], (float) atol( value ) );
				handled = true;
				break;
			}
		}

		if ( handled )
			continue;

		for ( unsigned int index = 0; index < ARRAYSIZE( stringCvars ); index++ )
		{
			if ( *entry == stringCvars[index] )
			{
				CVAR_SET_STRING( stringCvars[index], value );
				handled = true;
				break;
			}
		}

		if ( handled )
			continue;

		if ( *entry == "forcerespawn" )
		{
			if ( atol( value ) )
				SvenForceRespawn = true;
		}
		else if ( *entry == "sv_gravity" )
		{
			int gravity = atol( value );

			if ( gravity < 1 )
				gravity = 1;
			else if ( gravity > CFG_MAX_GRAVITY )
				gravity = CFG_MAX_GRAVITY;

			CVAR_SET_FLOAT( "sv_gravity", (float) gravity );
		}
		else if ( *entry == "sv_zmax" )
		{
			double distance = atol( value );

			if ( distance <= CFG_MIN_ZMAX )
				distance = CFG_MIN_ZMAX;
			else if ( distance >= CFG_MAX_ZMAX )
				distance = CFG_MAX_ZMAX;

			CVAR_SET_FLOAT( "sv_zmax", (float) distance );
		}
		else if ( *entry == "mp_allowgaussjump" || *entry == "mp_disablegaussjump" )
			CVAR_SET_FLOAT( "mp_disablegaussjump", (float) atol( value ) );
		else if ( *entry == "nextmap" )
		{
			if ( CVAR_GET_FLOAT( "mp_nextmap_ignore_mapcfg" ) < 1 )
				CVAR_SET_STRING( "mp_nextmap", value );
		}
		else if ( *entry == "killnpc" || *entry == "mp_npckill" )
			CVAR_SET_FLOAT( "mp_npckill", (float) atol( value ) );
		else if ( *entry == "nomedkit" )
			CVAR_SET_FLOAT( "mp_nomedkit", 1 );
		else if ( *entry == "nomaptrans" )
			CVAR_SET_STRING( "sv_notransition", value );
		else if ( *entry == "plrstart_zoffset" )
			CVAR_SET_FLOAT( "mp_startoffset", (float) atol( value ) );
		else if ( *entry == "mp_dropweapons" || *entry == "sv_dropweapons" )
			CVAR_SET_FLOAT( "mp_dropweapons", (float) atol( value ) );
		else if ( *entry == "mp_noblastgibs" || *entry == "sv_noblastgibs" )
			CVAR_SET_FLOAT( "mp_noblastgibs", (float) atol( value ) );
		else if ( *entry == "mp_allowmonsterinfo" || *entry == "sv_allowmonsterinfo" )
			CVAR_SET_FLOAT( "mp_allowmonsterinfo", (float) atol( value ) );
		else if ( *entry == "mp_allowplayerinfo" || *entry == "sv_allowplayerinfo" )
			CVAR_SET_FLOAT( "mp_allowplayerinfo", (float) atol( value ) );
		else if ( *entry == "globalsoundlist" )
			g_szSoundReplacement = AllocStringLocal( value );
		else if ( *entry == "globalmodellist" )
			g_szModelReplacement = AllocStringLocal( value );
		else if ( *entry == "sentence_file" )
		{
			CServer_SoundEngine *sound = CServer_SoundEngine::GetSoundEngine();

			UTIL_CopyString( sound->m_szSentenceFile, value, sizeof( sound->m_szSentenceFile ) );
		}
		else if ( *entry == "materials_file" )
		{
			CServer_SoundEngine *sound = CServer_SoundEngine::GetSoundEngine();

			UTIL_CopyString( sound->m_szMaterialsFile, value, sizeof( sound->m_szMaterialsFile ) );
		}
		else if ( *entry == "forcepmodels" )
			CMap::GetInstance()->SetForcedPlayerModels( value );
		else if ( *entry == "map_script" )
			CASServerManager::GetInstance()->LoadMapScript( value );
		else if ( *entry == "as_command" )
		{
			CASCommand *command = new CASCommand( entry );

			CASConCommandSystem::GetInstance()->EnqueueCommand( command, ServerCommandQueueCallback );
			command->Release();
		}
		else if ( !strstr( entry->GetCommand(), "start" ) && !strstr( entry->GetCommand(), "weapon_" )
		          && !strstr( entry->GetCommand(), "ammo_" ) && !strstr( entry->GetCommand(), "item_" )
		          && !strstr( entry->GetCommand(), "weaponmode_" ) && !strstr( entry->GetCommand(), "" ) )
		{
			snprintf( message, sizeof( message ), "Unknown map cfg command: %s\n", entry->GetCommand() );
			ALERT( at_console, message );
		}
	}

	return true;
}

bool CCfgFile::ApplyCfgOnSkill(void)
{
	CVAR_SET_FLOAT("skill", SKILL_MEDIUM);
	g_iSkillLevel = SKILL_MEDIUM;

	for (CCfgCommand *entry = m_pCommands; entry; entry = entry->m_pNext)
	{
		const char *command = entry->GetCommand();

		if (entry->m_bSkillCommand)
		{
			unsigned int length = strlen(command);

			if (!CVAR_GET_POINTER(command) && isdigit(command[length - 1]) &&
				atoi(command + length - 1) == g_iSkillLevel)
			{
				char baseName[MAX_SKILL_NAME_LENGTH] = {};

				UTIL_CopyString(baseName, command, length);

				if (CVAR_GET_POINTER(baseName))
				{
					CVAR_SET_FLOAT(baseName, atof(entry->GetArg(0)));
					continue;
				}
			}

			if (CVAR_GET_POINTER(command))
				CVAR_SET_FLOAT(command, atof(entry->GetArg(0)));
		}
		else if (!strstr(command, ""))
		{
			char message[MAX_SKILL_NAME_LENGTH] = {};

			snprintf(message, sizeof(message), "Unknown map skill cfg command: %s\n", command);
			ALERT(at_console, message);
		}
	}

	return true;
}

void CCfgFile::LoadMapScripts( void )
{
	for ( CCfgCommand *entry = m_pCommands; entry; entry = entry->m_pNext )
	{
		if ( *entry == "map_script" )
			CASServerManager::GetInstance()->LoadMapScript( entry->GetArg( 0 ) );
	}
}
