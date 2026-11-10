#include "extdll.h"
#include "util.h"
#include "server_support.h"
#include "CCfgCommand.h"

CCfgCommand::CCfgCommand()
{
	m_pNext = NULL;
	m_pPrev = NULL;
	m_pszCommand = NULL;
	memset(m_Arguments, 0, sizeof(m_Arguments));
	m_bPlayerCommand = false;
	m_bMapCommand = false;
	m_bSkillCommand = false;
	m_bUnknownCommand = true;
}

CCfgCommand::~CCfgCommand()
{
	SetCommand(NULL);
}

bool CCfgCommand::operator==(const char *command)
{
	if (command == m_pszCommand)
		return true;

	return command && m_pszCommand && !stricmp(command, m_pszCommand);
}

bool CCfgCommand::operator!=(const char *command)
{
	return !(*this == command);
}

void CCfgCommand::ClassifyCommand(void)
{
	static const char *playerCommands[] =
	{
		"weapon_crowbar", "weapon_pipewrench", "weapon_glock", "weapon_357",
		"weapon_uzi", "weapon_uziakimbo", "weapon_eagle", "weapon_desert_eagle",
		"weapon_mp5", "weapon_9mmar", "weapon_9mmAR", "weapon_shotgun",
		"weapon_m16", "weapon_crossbow", "weapon_rpg", "weapon_egon",
		"weapon_gauss", "weapon_hornetgun", "weapon_handgrenade", "weapon_satchel",
		"weapon_tripmine", "weapon_snark", "weapon_grapple", "weapon_saw",
		"weapon_sniperrifle", "weapon_shockrifle", "weapon_sporelauncher", "weapon_displacer",
		"ammo_9mm", "ammo_9mmAR", "ammo_9mmar", "ammo_mp5clip", "ammo_9mmbox",
		"ammo_357", "ammo_buckshot", "ammo_556", "ammo_ARgrenades", "ammo_argrenades",
		"ammo_crossbow", "ammo_rpgclip", "ammo_gaussclip", "ammo_762", "ammo_sporeclip",
		"item_longjump", "starthealth", "startarmor", "maxhealth", "maxarmor", "nosuit", NULL
	};
	static const char *mapCommands[] =
	{
		"mp_monsterpoints", "mp_timelimit", "mp_fraglimit", "mp_telefrag",
		"mp_flashlight", "mp_suitpower", "mp_falldamage", "forcerespawn",
		"mp_weaponstay", "mapcyclefile", "sv_accelerate", "sv_airaccelerate",
		"sv_airmove", "sv_friction", "sv_gravity", "sv_maxspeed",
		"sv_wateraccelerate", "skill", "sv_waterfriction", "mp_allowgaussjump",
		"mp_disablegaussjump", "nextmap", "killnpc", "nomedkit",
		"weaponmode_9mmhandgun", "weaponmode_eagle", "weaponmode_shotgun", "weaponmode_mp5",
		"weaponmode_crossbow", "weaponmode_rpg", "nomaptrans", "plrstart_zoffset",
		"mp_banana", "mp_npckill", "mp_dropweapons", "npc_dropweapons",
		"mp_respawndelay", "mp_multiplespawn", "mp_no_akimbo_uzis", "mp_grapple_mode",
		"mp_disable_autoclimb", "mp_disable_pcbalancing", "mp_disable_medkit_points",
		"mp_pcbalancing_factorlist", "mp_barnacle_paralyze", "mp_hevsuit_voice",
		"mp_modelselection", "sv_zmax", NULL
	};

	m_bUnknownCommand = true;

	if (!m_pszCommand)
		return;

	m_bPlayerCommand = false;
	m_bMapCommand = false;
	m_bSkillCommand = false;

	for (unsigned int i = 0; playerCommands[i]; i++)
	{
		if (!stricmp(m_pszCommand, playerCommands[i]))
		{
			m_bPlayerCommand = true;
			break;
		}
	}

	for (unsigned int i = 0; mapCommands[i]; i++)
	{
		if (!stricmp(m_pszCommand, mapCommands[i]))
		{
			m_bMapCommand = true;
			break;
		}
	}

	if (!strncmp(m_pszCommand, "sk_", sizeof("sk_") - 1))
		m_bSkillCommand = true;

	if (!strncmp(m_pszCommand, "weapon_", sizeof("weapon_") - 1) ||
		!strncmp(m_pszCommand, "ammo_", sizeof("ammo_") - 1) ||
		!strncmp(m_pszCommand, "item_", sizeof("item_") - 1))
		m_bPlayerCommand = true;

	if (m_bMapCommand || m_bPlayerCommand || m_bSkillCommand)
		m_bUnknownCommand = false;
}

void CCfgCommand::SetCommand(const char *command, const char *arg0, const char *arg1,
	const char *arg2, const char *arg3, const char *arg4)
{
	delete[] m_pszCommand;
	m_pszCommand = NULL;

	if (command)
	{
		unsigned int size = strlen(command) + 1;

		m_pszCommand = new char[size];
		UTIL_CopyString(m_pszCommand, command, size);
	}

	SetArg(arg0, 0);
	SetArg(arg1, 1);
	SetArg(arg2, 2);
	SetArg(arg3, 3);
	SetArg(arg4, 4);
	ClassifyCommand();
}

const char *CCfgCommand::GetCommand(void)
{
	return m_pszCommand;
}

const char *CCfgCommand::GetArg(int index)
{
	if ((unsigned int)index >= MAX_ARGUMENTS || !m_Arguments[index])
		return "";

	return m_Arguments[index];
}

void CCfgCommand::SetArg(const char *argument, unsigned int index)
{
	if (index >= MAX_ARGUMENTS)
		return;

	delete[] m_Arguments[index];
	m_Arguments[index] = NULL;

	if (argument)
	{
		unsigned int size = strlen(argument) + 1;

		m_Arguments[index] = new char[size];
		UTIL_CopyString(m_Arguments[index], argument, size);
	}
}
