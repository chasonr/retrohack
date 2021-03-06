/*	SCCS Id: @(#)mfndpos.h	3.0	89/11/22 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */
/* mfndpos.h - version 1.0.2 */
 
#ifndef MFNDPOS_H
#define MFNDPOS_H

#define ALLOW_TRAPS	0x0002FFFFL	/* can enter 18 kinds of traps */
#define ALLOW_U 	0x00040000L	/* can attack you */
#define ALLOW_M 	0x00080000L	/* can attack other monsters */
#define ALLOW_TM	0x00100000L	/* can attack tame monsters */
#define ALLOW_ALL	(ALLOW_U | ALLOW_M | ALLOW_TM | ALLOW_TRAPS)
#define NOTONL		0x00200000L	/* avoids direct line to player */
#define OPENDOOR	0x00400000L	/* opens closed doors */
#define UNLOCKDOOR	0x00800000L	/* unlocks locked doors */
#define BUSTDOOR	0x01000000L	/* breaks any doors */
#define ALLOW_ROCK	0x02000000L	/* pushes rocks */
#define ALLOW_WALL	0x04000000L	/* walks thru walls */
#define ALLOW_DIG	0x08000000L	/* digs */
#define ALLOW_SANCT	0x20000000L	/* enters temples */
#define ALLOW_SSM	0x40000000L	/* ignores scare monster */
#define NOGARLIC	0x80000000L	/* hates garlic */

#endif /* MFNDPOS_H */
