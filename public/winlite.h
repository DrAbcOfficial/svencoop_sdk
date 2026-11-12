//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef WINLITE_H
#define WINLITE_H
#pragma once

#ifdef _WIN32
// 
// Prevent tons of unused windows definitions
//
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#ifdef _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable: 4005)
#endif
#include <windows.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#undef PostMessage

#ifdef _MSC_VER
#pragma warning( disable: 4800 )	// forcing value to bool 'true' or 'false' (performance warning)
#endif

#endif // WIN32
#endif // WINLITE_H
