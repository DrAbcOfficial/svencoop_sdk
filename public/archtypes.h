//
// Word size dependent definitions
// DAL 1/03
//
#ifndef ARCHTYPES_H
#define ARCHTYPES_H

#include "steam/steamtypes.h"

#ifndef _WIN32
#define MAX_PATH 260
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <stddef.h>
#define _S_IREAD S_IREAD
#define _S_IWRITE S_IWRITE
typedef unsigned long ulong;	// matches the system <sys/types.h> definition
#endif

#endif // ARCHTYPES_H
