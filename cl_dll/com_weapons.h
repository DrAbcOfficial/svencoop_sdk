//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// com_weapons.h
// Shared weapons common function prototypes
#if !defined( COM_WEAPONSH )
#define COM_WEAPONSH
#ifdef _WIN32
#pragma once
#endif

#include "hud_iface.h"
#include "Exports.h"

void			COM_Log( const char *pszFile, const char *fmt, ...);
int				CL_IsDead( void );

float			UTIL_SharedRandomFloat( unsigned int seed, float low, float high );
int				UTIL_SharedRandomLong( unsigned int seed, int low, int high );

int				HUD_GetWeaponAnim( void );
int				HUD_GetWeaponBody( void );
void			HUD_SendWeaponAnim( int iAnim, int body, int force );
void			HUD_PlaySound( char *sound, float volume );
void			HUD_PlaybackEvent( int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );
void			HUD_SetMaxSpeed( edict_t *ed, float speed );
void			Game_GetSequence(int *sequence, int *gaitsequence);
void			Game_SetSequence(int sequence, int gaitsequence);
void			Game_GetOrientation(float *origin, float *angles);
void			Game_SetOrientation(Vector origin, Vector angles);
int				stub_PrecacheModel( char* s );
int				stub_PrecacheSound( char* s );
unsigned short	stub_PrecacheEvent( int type, const char *s );
const char		*stub_NameForFunction	( uint32 function );
void			stub_SetModel			( edict_t *e, const char *m );


extern cvar_t *cl_lw;

extern bool g_runfuncs;
extern vec3_t v_angles;
extern float g_lastFOV;
extern local_state_t* g_finalstate;

#endif