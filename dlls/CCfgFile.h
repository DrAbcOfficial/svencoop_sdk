#ifndef CCFGFILE_H
#define CCFGFILE_H

#include "CCfgCommand.h"

class CBasePlayer;

class CCfgFile
{
public:
	enum
	{
		MAX_LINE_LENGTH = 512,
		MAX_TOKEN_LENGTH = 64,
		MAX_TOKENS = CCfgCommand::MAX_ARGUMENTS + 1,
		MAX_SKILL_NAME_LENGTH = 256
	};

	CCfgFile();
	CCfgFile(const char *fileName);
	~CCfgFile();

	void ParseLine(char *line);
	CCfgCommand *Find(const char *command);
	bool ApplyCfgOnSkill(void);
	bool ApplyCfgOnMap(void);
	void LoadMapScripts(void);
	bool ApplyCfgOnPlayer( CBasePlayer *player );
	void GiveWeaponToPlayer( CBasePlayer *player, CCfgCommand *command, const char *name );
	void GiveAmmoToPlayer( CBasePlayer *player, CCfgCommand *command,
	                       int amount, const char *name, int maxCarry );

	CCfgCommand	*m_pCommands;
	bool		m_bValid;
};

#endif
