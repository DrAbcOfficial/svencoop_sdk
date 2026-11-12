#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

void AddCustomServerCommands();
bool AddPlayerBan( const char *authId, float seconds );
bool RemovePlayerBan( const char *authId );
void RemoveAllBans();
bool IsPlayerBanned( const char *authId );

#endif
