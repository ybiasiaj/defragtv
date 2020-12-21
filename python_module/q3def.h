#pragma once
/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/*
	too annoying to include q_shared.h etc directly
	some things are copied here for API use
	yes, this is the wrong way to do it

	TODO clean unused, add others required for client interaction
*/

typedef enum { qfalse, qtrue }	qboolean;
typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

enum cvar_flag_bits
{
	  ARCHIVE			= 0x0001	
	, USERINFO			= 0x0002
	, SERVERINFO		= 0x0004
	, SYSTEMINFO		= 0x0008
	, INIT				= 0x0010
	, LATCH				= 0x0020
	, ROM				= 0x0040
	, USER_CREATED		= 0x0080
	, TEMP				= 0x0100	
	, CHEAT				= 0x0200	
	, NORESTART			= 0x0400
	, SERVER_CREATED	= 0x0800
	, VM_CREATED		= 0x1000
	, PROTECTED			= 0x2000	
	, MODIFIED			= 0x40000000
	, NONEXISTENT		= 0x80000000
};

typedef void(*xcommand_t) (void);

#define	MAX_CONFIGSTRINGS	1024

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define	CS_SERVERINFO		0		// an info string with all the serverinfo cvars
#define	CS_SYSTEMINFO		1		// an info string for server system to client system configuration (timescale, etc)

#define	RESERVED_CONFIGSTRINGS	2	// game can't modify below this, only the system can

typedef unsigned char byte;
