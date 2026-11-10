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

// netadr.h

#ifndef NETADR_H
#define NETADR_H

#ifdef _WIN32
#pragma once
#endif

typedef enum
{
	NA_UNUSED,			// 0
	NA_LOOPBACK,		// 1
	NA_BROADCAST,		// 2
	NA_IP,				// 3
	NA_IP6,				// 4  Sven Co-op replaced the legacy NA_IPX with IPv6 here
	NA_IPX,				// 5
	NA_BROADCAST_IPX,	// 6
	NA_P2P,				// 7  SVEN P2P - Steam P2P networking, 64-bit SteamID in 'steamid'
} netadrtype_t;


typedef struct netadr_s
{
	netadrtype_t	type;			// 0
	unsigned char	ip[4];			// 4   IPv4
	unsigned char	ip6[16];		// 8   IPv6 (reserved)
	union
	{
		unsigned char ipx[10];
		struct
		{
			unsigned char  steamid[8];
			unsigned short reserved;
		};
	};
	unsigned short	port;			// 34
} netadr_t;							// 36 bytes

#endif // NETADR_H
