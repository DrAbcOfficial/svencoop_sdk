#ifndef SERVER_SUPPORT_H
#define SERVER_SUPPORT_H

struct RGBA;
class CHashStringPool;
class CBasePlayer;
struct edict_s;

extern CHashStringPool g_StringPool;

bool UTIL_StringToRGBA(RGBA &color, const char *text, char separator = ' ');
void WRITE_RGBA(const RGBA &color);

void UTIL_GetCircularGaussianSpread( float &x, float &y );

char *EntityLogString( const char *name, const char *userID, const char *authID, const char *team );
char *PlayerLogString( const char *name, const char *userID, const char *authID );
char *PlayerLog( edict_s *entity );
bool UTIL_IsPlayerConnected( CBasePlayer *player );
CBasePlayer *UTIL_CBasePlayerByName( const char *name, bool caseSensitive );

int UTIL_FormatString(char *destination, unsigned int size, const char *format, ...);
char *UTIL_FixSlashes(char *path);
char *UTIL_CopyString(char *destination, const char *source, unsigned int size);
char *UTIL_AppendString(char *destination, const char *source, unsigned int size);
unsigned int UTIL_TrimFloatTrailingZeroes(char *text);
char *UTIL_GetMapName(void);
const char *GetIpAddr(void);
void UTIL_HostError(const char *message, int status);
int AllocStringLocal(const char *text);

#endif
