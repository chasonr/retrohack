/*	SCCS Id: @(#)alloc.c	1.4	87/08/08 */
/* alloc.c - version 1.0.2 */
#ifdef LINT

/*
   a ridiculous definition, suppressing
        "possible pointer alignment problem" for (long *) malloc()
        "enlarg defined but never used"
        "ftell defined (in <stdio.h>) but never used"
   from lint
*/
#include <stdio.h>
long *
alloc(unsigned n)
{
    long dummy = ftell(stderr);
    if (n)
        dummy = 0; /* make sure arg is used */
    return (&dummy);
}

#else

#include <stdlib.h>
#include "alloc.h"
#include "panic.h"

long *
alloc(register unsigned lth)
{
    register char *ptr;

    if (!(ptr = malloc(lth)))
        panic("Cannot get %d bytes", lth);
    return ((long *) ptr);
}

#endif /* LINT */
