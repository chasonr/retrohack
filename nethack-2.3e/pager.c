/*	SCCS Id: @(#)pager.c	2.3	87/12/12 */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */

/* This file contains the command routine dowhatis() and a pager. */
/* Also readmail() and doshell(), and generally the things that
   contact the outside world. */

#include "hack.h"
#include <signal.h>
#include <stdio.h>

void page_more(/*void*/);
void set_pager(/*void*/);

extern int CO, LI; /* usually COLNO and ROWNO+2 */
extern char *CD;
extern char quitchars[];
extern char *getenv(), *getlogin();
extern xchar curx;
int done1();
extern int getpos();

int
dowhatis()
{
    FILE *fp;
    char bufr[BUFSZ + 6];
    register char *buf = &bufr[6], *ep, q;
    extern char readchar();

    if (!(fp = fopen(DATAFILE, "r")))
        pline("Cannot open data file!");
    else {
#ifndef GRAPHICS
        pline("Specify what? ");
        q = readchar();
#else
        coord cc;
        char r;

        pline("Specify unknown object by cursor ? [ynq] ");
        while (!index("yYnNqQ", (q = readchar())) && !index(quitchars, q))
            bell();

        if (q == 'n' || q == 'N') {
            pline("Specify what? ");
            r = readchar();
        } else if (index(quitchars, q))
            r = q;
        else {
            pline("Please move the cursor to the unknown object.");
            getpos(&cc, TRUE, "the unknown object");
            r = levl[cc.x][cc.y].scrsym;
        }

        if (r == showsyms.stone)
            q = defsyms.stone;
        else if (r == showsyms.vwall)
            q = defsyms.vwall;
        else if (r == showsyms.hwall)
            q = defsyms.hwall;
        else if (r == showsyms.tlcorn)
            q = defsyms.tlcorn;
        else if (r == showsyms.trcorn)
            q = defsyms.trcorn;
        else if (r == showsyms.blcorn)
            q = defsyms.blcorn;
        else if (r == showsyms.brcorn)
            q = defsyms.brcorn;
        else if (r == showsyms.door)
            q = defsyms.door;
        else if (r == showsyms.room)
            q = defsyms.room;
        else if (r == showsyms.corr)
            q = defsyms.corr;
        else if (r == showsyms.upstair)
            q = defsyms.upstair;
        else if (r == showsyms.dnstair)
            q = defsyms.dnstair;
        else if (r == showsyms.trap)
            q = defsyms.trap;
#ifdef FOUNTAINS
        else if (r == showsyms.pool)
            q = defsyms.pool;
        else if (r == showsyms.fountain)
            q = defsyms.fountain;
#endif
#ifdef NEWCLASS
        else if (r == showsyms.throne)
            q = defsyms.throne;
#endif
#ifdef SPIDERS
        else if (r == showsyms.web)
            q = defsyms.web;
#endif
#ifdef SINKS
        else if (r == showsyms.sink)
            q = defsyms.sink;
#endif
        else
            q = r;
#endif /* GRAPHICS */
#ifdef DGKMOD
        if (index(quitchars, q)) {
            (void) fclose(fp); /* sweet@scubed */
            return (0);
        }
#endif
#ifdef KJSMODS
        if (q == '%') {
            pline("%%       a piece of food");
            (void) fclose(fp);
            return (0);
        }
#endif
        if (q != '\t')
            while (fgets(buf, BUFSZ, fp))
                if (*buf == q) {
                    ep = index(buf, '\n');
                    if (ep)
                        *ep = 0;
                    /* else: bad data file */
                    /* Expand tab 'by hand' */
                    if (buf[1] == '\t') {
                        buf = bufr;
#ifdef GRAPHICS
                        buf[0] = r;
#else
                        buf[0] = q;
#endif
                        (void) strncpy(buf + 1, "       ", 7);
                    }
                    pline(buf);
                    if (ep[-1] == ';') {
                        pline("More info? ");
                        if (readchar() == 'y') {
                            page_more(fp, 1); /* does fclose() */
                            return (0);
                        }
                    }
                    (void) fclose(fp); /* kopper@psuvax1 */
                    return (0);
                }
        pline("I've never heard of such things.");
        (void) fclose(fp);
    }
    return (0);
}

/* make the paging of a file interruptible */
static int got_intrup;

void
intruph()
{
    got_intrup++;
}

/* simple pager, also used from dohelp() */
void
page_more(fp, strip)
FILE *fp;
int strip; /* nr of chars to be stripped from each line (0 or 1) */
{
    register char *bufr, *ep;
#ifdef DGK
    /* There seems to be a bug in ANSI.SYS  The first tab character
     * after a clear screen sequence is not expanded correctly.  Thus
     * expand the tabs by hand -dgk
     */
    int tabstop = 8, spaces;
    char buf[BUFSIZ], *bufp, *bufrp;

    set_pager(0);
    bufr = (char *) alloc((unsigned) CO);
    while (fgets(buf, BUFSIZ, fp) && (!strip || *buf == '\t')) {
        bufp = buf;
        bufrp = bufr;
        while (*bufp && *bufp != '\n') {
            if (*bufp == '\t') {
                spaces = tabstop - (bufrp - bufr) % tabstop;
                while (spaces--)
                    *bufrp++ = ' ';
                bufp++;
            } else
                *bufrp++ = *bufp++;
        }
        *bufrp = '\0';
#else
    int (*prevsig)() = signal(SIGINT, intruph);

    set_pager(0);
    bufr = (char *) alloc((unsigned) CO);
    bufr[CO - 1] = 0;
    while (fgets(bufr, CO - 1, fp) && (!strip || *bufr == '\t')) {
        ep = index(bufr, '\n');
        if (ep)
            *ep = 0;
#endif /* DGK */
        if (page_line(bufr + strip)) {
            set_pager(2);
            goto ret;
        }
    }
    set_pager(1);
ret:
    free(bufr);
    (void) fclose(fp);
#ifndef DGK
    (void) signal(SIGINT, prevsig);
    got_intrup = 0;
#endif
}

static boolean whole_screen = TRUE;
#define PAGMIN 12 /* minimum # of lines for page below level map */

void
set_whole_screen()
{ /* called in termcap as soon as LI is known */
    whole_screen = (LI - ROWNO - 2 <= PAGMIN || !CD);
}

#ifdef NEWS
int
readnews()
{
    register int ret;

    whole_screen = TRUE; /* force a docrt(), our first */
    ret = page_file(NEWS, TRUE);
    set_whole_screen();
    return (ret); /* report whether we did docrt() */
}
#endif

void
set_pager(mode)
register int mode; /* 0: open  1: wait+close  2: close */
{
    static boolean so;
    if (mode == 0) {
        if (!whole_screen) {
            /* clear topline */
            clrlin();
            /* use part of screen below level map */
            curs(1, ROWNO + 4);
        } else {
            cls();
        }
        so = flags.standout;
        flags.standout = 1;
    } else {
        if (mode == 1) {
            curs(1, LI);
            more();
        }
        flags.standout = so;
        if (whole_screen)
            docrt();
        else {
            curs(1, ROWNO + 4);
            cl_eos();
        }
    }
}

int
page_line(s) /* returns 1 if we should quit */
register char *s;
{
    extern char morc;

    if (cury == LI - 1) {
        if (!*s)
            return (0); /* suppress blank lines at top */
        putchar('\n');
        cury++;
        cmore("q\033");
        if (morc) {
            morc = 0;
            return (1);
        }
        if (whole_screen)
            cls();
        else {
            curs(1, ROWNO + 4);
            cl_eos();
        }
    }
#ifdef TERMINFO
    xputs(s);
    xputc('\n');
#else
    puts(s);
#endif
    cury++;
    return (0);
}

/*
 * Flexible pager: feed it with a number of lines and it will decide
 * whether these should be fed to the pager above, or displayed in a
 * corner.
 * Call:
 *	cornline(0, title or 0)	: initialize
 *	cornline(1, text)	: add text to the chain of texts
 *	cornline(2, morcs)	: output everything and cleanup
 *	cornline(3, 0)		: cleanup
 */

void
cornline(mode, text)
int mode;
char *text;
{
    static struct line {
        struct line *next_line;
        char *line_text;
    } * texthead, *texttail;
    static int maxlen;
    static int linect;
    register struct line *tl;

    if (mode == 0) {
        texthead = 0;
        maxlen = 0;
        linect = 0;
        if (text) {
            cornline(1, text); /* title */
            cornline(1, "");   /* blank line */
        }
        return;
    }

    if (mode == 1) {
        register int len;

        if (!text)
            return; /* superfluous, just to be sure */
        linect++;
        len = strlen(text) + 1; /* allow for an extra leading space */
        if (len > maxlen)
            maxlen = len;
        tl =
            (struct line *) alloc((unsigned) (len + sizeof(struct line) + 1));
        tl->next_line = 0;
        tl->line_text = (char *) (tl + 1);
        tl->line_text[0] = ' ';
        tl->line_text[1] = '\0';
        (void) strcat(tl->line_text, text);
        if (!texthead)
            texthead = tl;
        else
            texttail->next_line = tl;
        texttail = tl;
        return;
    }

    /* --- now we really do it --- */
    if (mode == 2 && linect == 1) /* topline only */
        pline(texthead->line_text);
    else if (mode == 2) {
        register int curline, lth;

        if (flags.toplin == 1)
            more(); /* ab@unido */
        remember_topl();

        lth = CO - maxlen - 2;          /* Use full screen width */
        if (linect < LI && lth >= 10) { /* in a corner */
            home();
            cl_end();
            flags.toplin = 0;
            curline = 1;
            for (tl = texthead; tl; tl = tl->next_line) {
#ifdef MSDOS
                cmov(lth, curline);
#else
                curs(lth, curline);
#endif
                if (curline > 1)
                    cl_end();
                xputs(tl->line_text);
                curx = curx + strlen(tl->line_text);
                curline++;
            }
#ifdef MSDOS
            cmov(lth, curline);
#else
            curs(lth, curline);
#endif
            cl_end();
            cmore(text);
            home();
            cl_end();
            docorner(lth, curline - 1);
        } else { /* feed to pager */
            set_pager(0);
            for (tl = texthead; tl; tl = tl->next_line) {
                if (page_line(tl->line_text)) {
                    set_pager(2);
                    goto cleanup;
                }
            }
            if (text) {
                cgetret(text);
                set_pager(2);
            } else
                set_pager(1);
        }
    }

cleanup:
    while (tl = texthead) {
        texthead = tl->next_line;
        free((char *) tl);
    }
}

int
dohelp()
{
    char c;

    pline("Long or short help? ");
    while (((c = readchar()) != 'l') && (c != 's') && !index(quitchars, c))
        bell();
    if (!index(quitchars, c))
        (void) page_file((c == 'l') ? HELP : SHELP, FALSE);
    return (0);
}

int
page_file(fnam, silent) /* return: 0 - cannot open fnam; 1 - otherwise */
register char *fnam;
boolean silent;
{
#ifdef DEF_PAGER /* this implies that UNIX is defined */
    {
        /* use external pager; this may give security problems */

        register int fd = open(fnam, 0);

        if (fd < 0) {
            if (!silent)
                pline("Cannot open %s.", fnam);
            return (0);
        }
        if (child(1)) {
            extern char *catmore;

            /* Now that child() does a setuid(getuid()) and a chdir(),
               we may not be able to open file fnam anymore, so make
               it stdin. */
            (void) close(0);
            if (dup(fd)) {
                if (!silent)
                    printf("Cannot open %s as stdin.\n", fnam);
            } else {
                execl(catmore, "page", (char *) 0);
                if (!silent)
                    printf("Cannot exec %s.\n", catmore);
            }
            exit(1);
        }
        (void) close(fd);
    }
#else
    {
        FILE *f; /* free after Robert Viduya */

        if ((f = fopen(fnam, "r")) == (FILE *) 0) {
            if (!silent) {
                home();
                perror(fnam);
                flags.toplin = 1;
                pline("Cannot open %s.", fnam);
            }
            return (0);
        }
        page_more(f, 0);
    }
#endif /* DEF_PAGER */

    return (1);
}

#ifdef UNIX
#ifdef SHELL
int
dosh()
{
    register char *str;
    if (child(0)) {
        if (str = getenv("SHELL"))
            execl(str, str, (char *) 0);
        else
            execl("/bin/sh", "sh", (char *) 0);
        pline("sh: cannot execute.");
        exit(1);
    }
    return (0);
}
#endif /* SHELL */

int
child(wt)
int wt;
{
    register int f = fork();
    if (f == 0) {           /* child */
        settty((char *) 0); /* also calls end_screen() */
        (void) setuid(getuid());
        (void) setgid(getgid());
#ifdef CHDIR
        (void) chdir(getenv("HOME"));
#endif
        return (1);
    }
    if (f == -1) { /* cannot fork */
        pline("Fork failed. Try again.");
        return (0);
    }
    /* fork succeeded; wait for child to exit */
    (void) signal(SIGINT, SIG_IGN);
    (void) signal(SIGQUIT, SIG_IGN);
    (void) wait((int *) 0);
    gettty();
    setftty();
    (void) signal(SIGINT, done1);
#ifdef WIZARD
    if (wizard)
        (void) signal(SIGQUIT, SIG_DFL);
#endif
    if (wt)
        getret();
    docrt();
    return (0);
}
#endif /* UNIX */
