//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:	All of our code is completely Unicode.  Instead of char, you should
//			use wchar, uint8, or char8, as explained below.
//
// $NoKeywords: $
//=============================================================================//


#ifndef WCHARTYPES_H
#define WCHARTYPES_H
#ifdef _WIN32
#pragma once
#endif

#ifdef _INC_TCHAR
#error ("Must include tier0 type headers before tchar.h")
#endif

// Temporarily turn off Valve defines
#include "tier0/valve_off.h"

#ifdef _WIN32

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#else // _LINUX

#include <stddef.h>
#include <wchar.h>

#endif 

// char8
// char8 is equivalent to char, and should be used when you really need a char
// (for example, when calling an external function that's declared to take
// chars).
typedef char char8;

// uint8
// uint8 is equivalent to byte (but is preferred over byte for clarity).  Use this
// whenever you mean a byte (for example, one byte of a network packet).
typedef unsigned char uint8;
typedef unsigned char BYTE;
typedef unsigned char byte;

// wchar
// wchar is a single character of text (currently 16 bits, as all of our text is
// Unicode).  Use this whenever you mean a piece of text (for example, in a string).
typedef wchar_t wchar;
//typedef char wchar;

// __WFILE__
// This is a Unicode version of GOLDSRCPLUS_FILE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(GOLDSRCPLUS_FILE)

#ifdef _WIN32
#ifdef STEAM
#ifndef _UNICODE
#define FORCED_UNICODE
#endif
#define _UNICODE
#endif
#endif

#ifdef _WIN32
#include <tchar.h>
#else
#if defined(_UNICODE)
#define _tcsstr wcsstr
#define _tcsicmp wcscasecmp
#define _tcscmp wcscmp
#define _tcscpy wcscpy
#define _tcsncpy wcsncpy
#define _tcsrchr wcsrchr
#define _tcslen wcslen
#define _tfopen _wfopen
#define _stprintf swprintf
#define _ftprintf fwprintf
#define _vsntprintf vswprintf
#define _tprintf wprintf
#define _sntprintf _snwprintf
#define _T(s) L##s
#else
#define _tcsstr strstr
#define _tcsicmp stricmp
#define _tcscmp strcmp
#define _tcscpy strcpy
#define _tcsncpy strncpy
#define _tcsrchr strrchr
#define _tcslen strlen
#define _tfopen fopen
#define _stprintf sprintf 
#define _ftprintf fprintf
#define _vsntprintf _vsnprintf
#define _tprintf printf
#define _sntprintf _snprintf
#define _T(s) s
#endif
#endif

#if defined(_UNICODE)
typedef wchar tchar;
#define tstring wstring
#define __TFILE__ __WFILE__
#define TCHAR_IS_WCHAR
#else
typedef char tchar;
#define tstring string
#define __TFILE__ GOLDSRCPLUS_FILE
#define TCHAR_IS_CHAR
#endif

#ifdef FORCED_UNICODE
#undef _UNICODE
#endif

// Turn valve defines back on
#include "tier0/valve_on.h"


#endif // WCHARTYPES


