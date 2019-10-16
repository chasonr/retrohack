/*	SCCS Id: @(#)topl.c	2.0	87/09/15 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include <stdio.h>
#include <stdlib.h>
#include "hack.h"
#ifdef GENIX
#define void int /* jhn - mod to prevent compiler from bombing */
#endif

static void redotoplin(/*unknown*/);
void more(/*void*/);
void putsym(/*void*/);
void putstr(/*void*/);
static void xmore(/*unknown*/);

static char toplines[BUFSIZ];
static xchar tlx, tly; /* set by pline; used by addtopl */

static struct topl {
    struct topl *next_topl;
    char *topl_text;
} * old_toplines, *last_redone_topl;
#define OTLMAX 20 /* max nr of old toplines remembered */

int
doredotopl()
{
    if (last_redone_topl)
        last_redone_topl = last_redone_topl->next_topl;
    if (!last_redone_topl)
        last_redone_topl = old_toplines;
    if (last_redone_topl) {
        (void) strcpy(toplines, last_redone_topl->topl_text);
    }
    redotoplin();
    return (0);
}

static void
redotoplin()
{
    home();
    if (index(toplines, '\n'))
        cl_end();
    putstr(toplines);
    cl_end();
    tlx = curx;
    tly = cury;
    flags.toplin = 1;
    if (tly > 1)
        more();
}

void
remember_topl()
{
    register struct topl *tl;
    register int cnt = OTLMAX;
    if (last_redone_topl && !strcmp(toplines, last_redone_topl->topl_text))
        return;
    if (old_toplines && !strcmp(toplines, old_toplines->topl_text))
        return;
    last_redone_topl = 0;
    tl = (struct topl *) alloc(
        (unsigned) (strlen(toplines) + sizeof(struct topl) + 1));
    tl->next_topl = old_toplines;
    tl->topl_text = (char *) (tl + 1);
    (void) strcpy(tl->topl_text, toplines);
    old_toplines = tl;
    while (cnt && tl) {
        cnt--;
        tl = tl->next_topl;
    }
    if (tl && tl->next_topl) {
        free((char *) tl->next_topl);
        tl->next_topl = 0;
    }
}

void
addtopl(s)
char *s;
{
    curs(tlx, tly);
    if (tlx + strlen(s) > CO)
        putsym('\n');
    putstr(s);
    tlx = curx;
    tly = cury;
    flags.toplin = 1;
}

static void
xmore(s)
char *s; /* allowed chars besides space/return */
{
    if (flags.toplin) {
        curs(tlx, tly);
        if (tlx + 8 > CO)
            putsym('\n'), tly++;
    }

    if (flags.standout)
        standoutbeg();
    putstr("--More--");
    if (flags.standout)
        standoutend();

    xwaitforspace(s);
    if (flags.toplin && tly > 1) {
        home();
        cl_end();
        docorner(1, tly - 1);
    }
    flags.toplin = 0;
}

void
more()
{
    xmore("");
}

void
cmore(s)
register char *s;
{
    xmore(s);
}

void
clrlin()
{
    if (flags.toplin) {
        home();
        cl_end();
        if (tly > 1)
            docorner(1, tly - 1);
        remember_topl();
    }
    flags.toplin = 0;
}

/*VARARGS1*/
/* Because the modified mstatusline has 9 arguments KAA */
void
pline(line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
register char *line,
    *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8, *arg9;
{
    char pbuf[BUFSZ];
    register char *bp = pbuf, *tl;
    register int n, n0;

    if (!line || !*line)
        return;
    if (!index(line, '%'))
        (void) strcpy(pbuf, line);
    else
        (void) sprintf(pbuf, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7,
                       arg8, arg9);
    if (flags.toplin == 1 && !strcmp(pbuf, toplines))
        return;
    nscr(); /* %% */

    /* If there is room on the line, print message on same line */
    /* But messages like "You die..." deserve their own line */
    n0 = strlen(bp);
    if (flags.toplin == 1 && tly == 1 && n0 + strlen(toplines) + 3 < CO - 8
        && /* leave room for --More-- */
        strncmp(bp, "You ", 4)) {
        (void) strcat(toplines, "  ");
        (void) strcat(toplines, bp);
        tlx += 2;
        addtopl(bp);
        return;
    }
    if (flags.toplin == 1)
        more();
    remember_topl();
    toplines[0] = 0;
    while (n0) {
        if (n0 >= CO) {
            /* look for appropriate cut point */
            n0 = 0;
            for (n = 0; n < CO; n++)
                if (bp[n] == ' ')
                    n0 = n;
            if (!n0)
                for (n = 0; n < CO - 1; n++)
                    if (!letter(bp[n]))
                        n0 = n;
            if (!n0)
                n0 = CO - 2;
        }
        (void) strncpy((tl = eos(toplines)), bp, n0);
        tl[n0] = 0;
        bp += n0;

        /* remove trailing spaces, but leave one */
        while (n0 > 1 && tl[n0 - 1] == ' ' && tl[n0 - 2] == ' ')
            tl[--n0] = 0;

        n0 = strlen(bp);
        if (n0 && tl[0])
            (void) strcat(tl, "\n");
    }
    redotoplin();
}

void
putsym(c)
char c;
{
    switch (c) {
    case '\b':
        backsp();
        return;
    case '\n':
        curx = 1;
        cury++;
        if (cury > tly)
            tly = cury;
        break;
    default:
        if (curx == CO)
            putsym('\n'); /* 1 <= curx <= CO; avoid CO */
        else
            curx++;
    }
    (void) putchar(c);
}

void
putstr(s)
register char *s;
{
    while (*s)
        putsym(*s++);
}
