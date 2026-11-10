/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
#include "../hud.h"
#include "../cl_util.h"

int iModelAlienGibs;
int iModelHumanGibs;
int iModelGlassGibs;

int CL_PrecacheModel(const char *name)
{
	if (!name)
		return 0;

	int index = 0;

	gEngfuncs.CL_LoadModel(name, &index);
	return index;
}

/*
=====================
Game_AddObjects

Add game specific, client-side objects here
=====================
*/
void Game_AddObjects( void )
{
	iModelAlienGibs = CL_PrecacheModel("models/agibs.mdl");
	iModelHumanGibs = CL_PrecacheModel("models/hgibs.mdl");
	iModelGlassGibs = CL_PrecacheModel("models/glassgibs.mdl");
}