/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//=========================================================
// skill.cpp - code for skill level concerns
//=========================================================
#include	"extdll.h"
#include	"util.h"
#include	"gamerules/skill.h"


skilldata_t	gSkillData;


//=========================================================
// return the value of a skill Cvar
//=========================================================
float GetSkillCvar( const char *szBuffer)
{
	float flValue = CVAR_GET_FLOAT ( szBuffer );

	if ( flValue <= 0 )
	{
		ALERT (at_aiconsole, "\n\n** GetSkillCVar: Got a zero for \"%s\" **\n\n", szBuffer);
	}
	else
	{
		ALERT(at_aiconsole, "** GetSkillCVar: Value of \"%s\" is %f **\n", szBuffer, flValue);
	}

	return flValue;
}
