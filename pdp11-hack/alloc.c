/*
 * Alloc.c
 *
 * Added new return values Michiel and Fred
 *
 */

/*
 * The strange return value of alloc is implemented to shut lint up
 */

#include <stdlib.h>
#include "hack.h"

static union PTRS ptrs;

union PTRS *
alloc (num)
register int    num;
{
	register char  *val;

	if (!(val = malloc (num)))
		panic (CORE, "Cannot get %d bytes", num);
	ptrs.Val = val;
	return (&ptrs);
}
