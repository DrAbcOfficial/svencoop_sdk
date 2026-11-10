#ifndef CCVARSYSTEM_H
#define CCVARSYSTEM_H

class CCVarSystem
{
public:
	enum SetType_t
	{
		SET_ENGINE_CVAR = 1,
		SET_AS_CVAR = 2,
		SET_ALL_CVARS = 3
	};

	enum SetResult_t
	{
		SET_RESULT_ERROR = -1,
		SET_RESULT_NO_NAME = 1,
		SET_RESULT_NAME_TOO_LONG = 2,
		SET_RESULT_DISALLOWED = 3,
		SET_RESULT_UNKNOWN_SKILL = 4,
		SET_RESULT_OK = 5
	};

	CCVarSystem();

	static CCVarSystem *GetInstance( void );
	int SetCVar( const char *name, const char *value, SetType_t type );
	int SetEngineCVar( const char *name, const char *value );
	int SetASCVar( const char *name, const char *value );
};

extern bool SvenForceRespawn;
extern float g_plCountBalance;

#endif
