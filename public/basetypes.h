//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef BASETYPES_H
#define BASETYPES_H

#ifdef _WIN32
#pragma once
#endif

// stdio.h
#ifndef NULL
#define NULL 0
#endif

// In case this ever changes
#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


typedef int BOOL;
typedef int qboolean;
typedef unsigned long ULONG;
typedef unsigned char BYTE;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint32;
typedef unsigned short ushort;

#ifdef _WIN32
typedef wchar_t ucs2; // under windows wchar_t is ucs2
#else
typedef unsigned short ucs2;
#endif

enum ThreeState_t
{
	TRS_FALSE,
	TRS_TRUE,
	TRS_NONE,
};

typedef float vec_t;

// FIXME: this should move
#ifndef __cplusplus
#define true 1
#define false 0
typedef char bool;
#endif

typedef struct vrect_s
{
	int				x,y,width,height;
	struct vrect_s			*pnext;
} vrect_t;


//-----------------------------------------------------------------------------
// MaterialRect_t struct - used for DrawDebugText
//-----------------------------------------------------------------------------
struct Rect_t
{
    int x, y;
	int width, height;
};


//-----------------------------------------------------------------------------
// Interval, used by soundemittersystem + the game
//-----------------------------------------------------------------------------
struct interval_t
{
	float start;
	float range;
};


// @TODO: Find a better home for this
#if !defined(_STATIC_LINKED) && !defined(PUBLISH_DLL_SUBSYSTEM)
// for platforms built with dynamic linking, the dll interface does not need spoofing
#define PUBLISH_DLL_SUBSYSTEM()
#endif

#endif // BASETYPES_H
