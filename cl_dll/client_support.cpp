#include "extdll.h"
#include "hud_iface.h"
#include "client_messagebox.h"
#include "client_support.h"
#include "netadr.h"
#include "parsemsg.h"
#include <ctype.h>

#ifdef _WIN32
#include <winsock.h>
#include <delayimp.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

void AlertMessage(ALERT_TYPE type, char *format, ...);

char *UTIL_CopyString(char *destination, const char *source, unsigned int size)
{
	if (!source || !destination || !size)
		return NULL;

	memset(destination, 0, size);

	unsigned int length = strlen(source);

	if (!length)
		return destination;

	return strncpy(destination, source, length < size ? size : size - 1);
}

int UTIL_FormatString(char *destination, unsigned int size, const char *format, ...)
{
	if (!format || !destination)
		return 0;

	unsigned int formatLength = strlen(format);

	if (!size)
		return 0;

	memset(destination, 0, size);

	if (!formatLength)
		return 0;

	va_list arguments;

	va_start(arguments, format);
	int result = vsnprintf(destination, size, format, arguments);
	va_end(arguments);

	destination[size - 1] = 0;
	return result;
}

char *UTIL_FixSlashes(char *path)
{
	if (!path)
		return NULL;

	for (char *cursor = path; *cursor; cursor++)
	{
		if (*cursor == '\\')
			*cursor = '/';
	}

	return path;
}

char *UTIL_AppendString(char *destination, const char *source, unsigned int size)
{
	if (!source)
		return destination;

	if (!destination)
		return NULL;

	unsigned int length = strlen(destination);

	if (length + 1 >= size)
		return destination;

	unsigned int remaining = size - length - 1;

	memset(destination + length, 0, size - length);

	if (!*source)
		return destination;

	return strncat(destination, source, strlen(source) <= remaining ? remaining : remaining - 1);
}

char *READ_ALLOC_STRING(void)
{
	const char *source = READ_STRING();
	unsigned int size = strlen(source) + 1;
	char *result = new char[size];

	UTIL_CopyString(result, source, size);
	return result;
}

char *UTIL_GetMapName(void)
{
	static char mapName[260];
	const char *level = gEngfuncs.pfnGetLevelName();
	const char *map = level ? strstr(level, "maps/") : NULL;

	if (map && strlen(map) > 5)
	{
		UTIL_CopyString(mapName, map + 5, sizeof(mapName));

		char *extension = strstr(mapName, ".bsp");

		if (extension)
			*extension = 0;
	}
	else
	{
		memset(mapName, 0, sizeof(mapName));
	}

	return mapName;
}

void UTIL_HostError(const char *message, int status)
{
	unsigned int length = strlen(message);
	char *text = new char[length + 2];

	snprintf(text, length + 1, "%s\n", message);
	AlertMessage(at_error, text);

#ifdef _WIN32
	MessageBoxA(NULL, text, "Critical Error", MB_SETFOREGROUND | MB_ICONERROR);
#else
	UTIL_MessageBox(text, "Critical Error", MSGBOX_ICONERROR, MSGBOX_OK);
#endif

	delete[] text;

	if (status)
		exit(status);

	gEngfuncs.pfnClientCmd("disconnect");
}

#ifdef _WIN32
HMODULE LoadGameLibrary(const char *name)
{
	char directory[MAX_PATH];
	char path[MAX_PATH];

	UTIL_CopyString(directory, gEngfuncs.pfnGetGameDirectory(), sizeof(directory));
	snprintf(path, sizeof(path), "%s/%s", directory, name);

	HMODULE module = LoadLibraryA(path);

	if (!module)
	{
		char message[772];

		snprintf(message, sizeof(message), "Error delay loading library \"%s\". Exiting\n", path);
		UTIL_HostError(message, -1);
	}

	return module;
}

// fmodex.dll ships in the game directory, outside the default DLL search path
FARPROC WINAPI DelayLoadNotifyHook(unsigned notification, PDelayLoadInfo info)
{
	if (notification != dliNotePreLoadLibrary || strcmp(info->szDll, "fmodex.dll"))
		return NULL;

	return (FARPROC)LoadGameLibrary(info->szDll);
}

extern "C" const PfnDliHook __pfnDliNotifyHook2 = DelayLoadNotifyHook;
#endif

char *TrimString(char *text)
{
	if (!text || !*text)
		return NULL;

	while (*text && isspace(*text))
		text++;

	char *end = text + strlen(text);

	while (end > text && isspace(end[-1]))
		*--end = 0;

	return text;
}

void StripMapFromPFN(const char *source, char *destination)
{
	if (!source)
		return;

	char name[44];

	UTIL_CopyString(name, source, 42);

	char *extension = strchr(name, '.');

	if (extension)
		*extension = 0;

	char *directory = strchr(name, '/');

	if (directory)
		memmove(name, directory + 1, strlen(directory) + 1);

	memset(name + strlen(name), 0, 42 - strlen(name));
	UTIL_CopyString(destination, name, 32);
	memset(destination + strlen(destination), 0, 33 - strlen(destination));
}

char *GetTopSubdir(const char *path)
{
#ifdef _WIN32
	static char buffer[260];
#else
	static char buffer[4096];
#endif
	buffer[0] = 0;

	unsigned int length = strlen(path);

	if (!length)
		return buffer;

	UTIL_CopyString(buffer, path, sizeof(buffer));

	if (buffer[length - 1] == '/' || buffer[length - 1] == '\\')
		buffer[length - 1] = 0;

	char *directory = buffer;

	for (char *cursor = buffer; *cursor; cursor++)
	{
		if (*cursor == '/' || *cursor == '\\')
			directory = cursor + 1;
	}

	return directory;
}

const char *GetIpAddr(void)
{
	char name[64];
#ifdef _WIN32
	WSADATA data;

	if (WSAStartup(MAKEWORD(1, 1), &data) != 0)
	{
		WSACleanup();
		return "NULL";
	}
#endif
	if (gethostname(name, sizeof(name)) != 0)
	{
#ifdef _WIN32
		WSACleanup();
#endif
		return "NULL";
	}

	name[sizeof(name) - 1] = 0;

	hostent *host = gethostbyname(name);

	if (!host)
	{
#ifdef _WIN32
		WSACleanup();
#endif
		return "NULL";
	}

	const char *address = NULL;

	for (int i = 0; host->h_addr_list[i]; i++)
	{
		address = inet_ntoa(*(in_addr *)host->h_addr_list[i]);

		if (strncmp(address, "127.", 4) && strcmp(address, "::1"))
			break;
	}

#ifdef _WIN32
	WSACleanup();
#endif
	return address;
}

void UTIL_FormatNetAdr(char *destination, unsigned int size, netadr_s *address)
{
	char text[33];

	memset(text, 0, sizeof(text));

	switch (address->type)
	{
	case NA_IP:
		UTIL_FormatString(text, "%u.%u.%u.%u", address->ip[0], address->ip[1], address->ip[2], address->ip[3]);
		break;

	case NA_IPX:
		UTIL_FormatString(text, "%x%x%x%x%x%x%x%x%x%x",
			address->ipx[0], address->ipx[1], address->ipx[2], address->ipx[3], address->ipx[4],
			address->ipx[5], address->ipx[6], address->ipx[7], address->ipx[8], address->ipx[9]);
		break;

	case NA_LOOPBACK:
		UTIL_CopyString(destination, "Loopback", size);
		break;

	default:
		UTIL_CopyString(destination, "Unknown", size);
		break;
	}

	if (text[0])
		UTIL_FormatString(destination, size, "%s:%d", text, ((address->port & 0xFF) << 8) | (address->port >> 8));
}
