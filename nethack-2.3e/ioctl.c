/*	SCCS Id: @(#)ioctl.c	2.0	87/09/18 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

/* This cannot be part of hack.tty.c (as it was earlier) since on some
   systems (e.g. MUNIX) the include files <termio.h> and <sgtty.h>
   define the same constants, and the C preprocessor complains. */
#include "config.h"
#include <stdio.h>
#ifdef BSD
#include <sgtty.h>
struct ltchars ltchars, ltchars0;
#else
#include <termio.h> /* also includes part of <sgtty.h> */
static struct termio termio;
#endif
#include "ioctls.h"

void
getioctls(void)
{
#ifdef BSD
    (void) ioctl(fileno(stdin), (int) TIOCGLTC, (char *) &ltchars);
    (void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars0);
#else
    (void) ioctl(fileno(stdin), (int) TCGETA, &termio);
#endif
}

void
setioctls(void)
{
#ifdef BSD
    (void) ioctl(fileno(stdin), (int) TIOCSLTC, (char *) &ltchars);
#else
    /* Now modified to run under Sys V R3.	- may have to be #ifdef'ed */
    (void) ioctl(fileno(stdin), (int) TCSETAW, &termio);
#endif
}

#ifdef SUSPEND /* implies BSD */
dosuspend(void)
{
#include <signal.h>
#ifdef SIGTSTP
    if (signal(SIGTSTP, SIG_IGN) == SIG_DFL) {
        settty((char *) 0);
        (void) signal(SIGTSTP, SIG_DFL);
        (void) kill(0, SIGTSTP);
        gettty();
        setftty();
        docrt();
    } else {
        pline("I don't think your shell has job control.");
    }
#else SIGTSTP
    pline("Sorry, it seems we have no SIGTSTP here. Try ! or S.");
#endif
    return (0);
}
#endif /* SUSPEND */
