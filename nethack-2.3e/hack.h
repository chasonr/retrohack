/*	SCCS Id: @(#)hack.h	2.3	87/12/12 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#ifndef HACK_H
#define HACK_H

#ifdef __TURBOC__
/* work around the case-insensitivity of the DOS linker */
#define Amonnam Amonnam_
#define Xmonnam Xmonnam_
#define Monnam Monnam_
#define POISONOUS POISONOUS_
#define Doname Doname_
#define Tmp_at Tmp_at_
/* rename the next two functions because they clash with the Turbo C library
 */
#define getdate getdate_
#define itoa itoa_
#endif

#include "global.h"
#include "extern.h"

#define Null(type) ((struct type *) 0)

#include "objclass.h"
#include "coord.h"

extern coord bhitpos; /* place where thrown weapon falls to the ground */

#include "flag.h"
#include "gold.h"
#include "monst.h" /* uses coord */
#include "trap.h"

#define plur(x) (((x) == 1) ? "" : "s")
#define min(x, y) ((x) < (y) ? (x) : (y))

#define BUFSZ 256  /* for getlin buffers */

#include "rm.h"

#define Inhell (dlevel >= 30)
#define newstring(x) (char *) alloc((unsigned) (x))

#ifdef SPELLS
#define NO_SPELL 0
#endif

#define TELL 1
#define NOTELL 0

#define ON 1
#define OFF 0

#ifdef GENIX
#define DIST jhndist
/*	genix compiler chokes on DIST macro below - jhn*/
#else
#define DIST(x1, y1, x2, y2) \
    (((x1) - (x2)) * ((x1) - (x2)) + ((y1) - (y2)) * ((y1) - (y2)))
#endif

#ifdef HARD
#define MAX_CARR_CAP 120 /* so that boulders can be heavier */
#else
#define MAX_CARR_CAP 500
#endif
#ifdef RPH
#define MAXLEVEL 60
#else
#define MAXLEVEL 40
#endif
#define FAR (COLNO + 2) /* position outside screen */

#endif /* HACK_H */
