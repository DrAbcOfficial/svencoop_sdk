// port.h: portability helper
//
//////////////////////////////////////////////////////////////////////

#if !defined PORT_H
#define PORT_H

#include "archtypes.h"     // DAL

#ifdef _WIN32

// Insert your headers here
#include "tier0/platform.h"
#include "basetypes.h"
#include <sys/stat.h>
#include <stdarg.h>

#else // _WIN32

#include "tier0/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // exit()
#include <string.h> // strncpy()
#include <ctype.h> // tolower()
#include <limits.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifndef u_long
#define u_long unsigned long
#endif

// PATH_MAX is optional in POSIX; the engine uses a fixed-size buffer anyway.
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef unsigned char BYTE;
typedef short int WORD;
typedef unsigned int DWORD;
//typedef int32 LONG;
//typedef uint32 ULONG;
#ifndef ARCHTYPES_H
typedef uint32 ULONG;
#endif
typedef void *HANDLE;
#ifndef HMODULE
typedef void *HMODULE;
#endif
typedef char * LPSTR;

#define __cdecl


//const int MAX_PATH = PATH_MAX;
#define MAX_PATH 260

#ifdef LINUX
typedef struct POINT_s
{
	int x;
	int y;
} POINT;
typedef void *HINSTANCE;
typedef void *HWND;
typedef void *HDC;
typedef void *HGLRC;

typedef struct RECT_s
{
	int left;
	int right;
	int top;
	int bottom;
} RECT;
#endif


#ifdef __cplusplus

//#undef FALSE
//#undef TRUE

#if defined(__APPLE__)
//#else
//const bool	FALSE		= false;
//const bool	TRUE		= true;
#endif
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif

#ifdef __cplusplus
inline int		ioctlsocket( int d, int cmd, uint32 *argp )
{
	// ioctl() is variadic and expects a pointer to a long-sized value; copy
	// through a correctly sized object instead of passing a uint32*.
	unsigned long value = argp ? (unsigned long) *argp : 0;
	int result = ioctl( d, cmd, &value );

	if ( argp )
		*argp = (uint32) value;

	return result;
}
inline int		closesocket( int fd )								{ return close( fd ); }
inline char *	GetCurrentDirectory( size_t size, char * buf )		{ return getcwd( buf, size ); }
inline int		WSAGetLastError()									{ return errno; }

inline void		DebugBreak( void ) { exit( 1 ); }
#endif

extern char g_szEXEName[ 4096 ];

#define _snprintf snprintf

// Define min/max macros only for C to avoid conflicts with std::min and std::max
#if !defined( __cplusplus )
#ifndef min
#define min( a, b )		( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif
#ifndef max
#define max( a, b )		( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif
#ifndef __min
#define __min( a, b )	min( a, b )
#endif
#ifndef __max
#define __max( a, b )	max( a, b )
#endif
#endif // !__cplusplus

#if defined(__APPLE__)
#define SO_ARCH_SUFFIX ".dylib"
#elif defined(__x86_64__) || defined(_M_X64)
#define SO_ARCH_SUFFIX "_amd64.so"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define SO_ARCH_SUFFIX "_arm64.so"
#else
#define SO_ARCH_SUFFIX ".so"
#endif
#endif

#endif // PORT_H
