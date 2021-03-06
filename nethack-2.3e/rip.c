/*	SCCS Id: @(#)rip.c	2.3	88/02/11 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

#include "hack.h"
#include <stdio.h>
#ifdef GENIX
#define void int /* jhn - mod to prevent compiler from bombing */
#endif

static void center(int line, char *text);

static char rip[][50] = {
    "                       ----------",
    "                      /          \\",
    "                     /    REST    \\",
    "                    /      IN      \\",
    "                   /     PEACE      \\",
    "                  /                  \\",
    "                  |                  |",
    "                  |                  |",
    "                  |                  |",
    "                  |                  |",
    "                  |                  |",
    "                  |       1001       |",
    "                 *|     *  *  *      | *",
    "        _________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______\n",
    ""
};

void
outrip(void)
{
    register char const *dpx;
    char buf[BUFSZ];
    register int x, y;

    cls();
    (void) sprintf(buf, "%s%s", (Badged) ? "Officer " : "", plname);
    buf[16] = 0;
    center(6, buf);
    (void) sprintf(buf, "%ld AU", u.ugold);
    center(7, buf);
    (void) sprintf(
        buf, "killed by%s",
        !strncmp(killer, "the ", 4)
            ? ""
            : !strcmp(killer, "starvation")
                  ? ""
                  : !strncmp(killer, "Mr.", 3)
                        ? ""
                        : !strncmp(killer, "Ms.", 3)
                              ? ""
                              :
#ifdef STOOGES
                              !strcmp(killer, "Larry")
                                  ? ""
                                  : !strcmp(killer, "Curly")
                                        ? ""
                                        : !strcmp(killer, "Moe")
                                              ? ""
                                              :
#endif
                                              index(vowels, *killer) ? " an"
                                                                     : " a");
    center(8, buf);
    (void) strcpy(buf, killer);
    if (strlen(buf) > 16) {
        register int i, i0, i1;
        i0 = i1 = 0;
        for (i = 0; i <= 16; i++)
            if (buf[i] == ' ')
                i0 = i, i1 = i + 1;
        if (!i0)
            i0 = i1 = 16;
        buf[i1 + 16] = 0;
        center(10, buf + i1);
        buf[i0] = 0;
    }
    center(9, buf);
    (void) sprintf(buf, "%4d", getyear());
    center(11, buf);
    for (y = 0; rip[y][0] != '\0'; y++) {
        x = 0;
        dpx = rip[y];
        while (dpx[x]) {
            while (dpx[x] == ' ')
                x++;
            curs(x, y + 8);
            while (dpx[x] && dpx[x] != ' ') {
                if (done_stopprint)
                    return;
                curx++;
                (void) putchar(dpx[x++]);
            }
        }
    }
    getret();
}

static void
center(int line, char *text)
{
    register char *ip, *op;
    ip = text;
    op = &rip[line][28 - ((strlen(text) + 1) / 2)];
    while (*ip)
        *op++ = *ip++;
}
