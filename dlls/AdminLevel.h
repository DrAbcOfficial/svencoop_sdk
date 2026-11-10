#ifndef ADMINLEVEL_H
#define ADMINLEVEL_H

enum AdminLevel_t : int
{
	ADMIN_INIT = -1,
	ADMIN_NO = 0,
	ADMIN_YES = 1,
	ADMIN_OWNER = 2
};

class CBasePlayer;
AdminLevel_t AdminLevel(CBasePlayer *player);
bool ValidateAdminAccess( CBasePlayer *invoker, CBasePlayer *target, const char *method, const char *failure );
const char *AdminLevelToString( AdminLevel_t level );
AdminLevel_t StringToAdminLevel(const char *level);

#endif
