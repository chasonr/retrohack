/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* makedefs.c - NetHack version 2.3 */

/*static char SCCS_Id[] = "@(#)makedefs.c	2.3\t88/02/18";*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"

#ifdef MSDOS
#undef exit
#define freopen _freopen
#define alloc malloc
#define RDMODE "r"
#define WRMODE "w"
#else
#define RDMODE "r+"
#define WRMODE "w+"
#endif

static int do_objs();
static int skipuntil();
static int getentry();
static int duplicate();
static int specprop();
static int letter();
static int digit();

/* construct definitions of object constants */
#define OBJ_FILE "objects.h"
#define ONAME_FILE "onames.h"
#define TRAP_FILE "trap.h"
#define DATE_FILE "date.h"
#define RUMOR_FILE "rumors"
#define DATA_FILE "data"

char in_line[256], outline[256];

void do_traps(/*void*/);
void do_rumors(/*void*/);
void do_date(/*void*/);
void do_data(/*void*/);
void newobj(/*void*/);
void capitalize(/*void*/);

int
main(argc, argv)
int argc;
char *argv[];
{
    char *option;

    if (argc == 2) {
        option = argv[1];
        switch (option[1]) {
        case 'o':
        case 'O':
            do_objs();
            break;
        case 't':
        case 'T':
            do_traps();
            break;
        case 'r':
        case 'R':
            do_rumors();
            break;

        case 'd':
            do_data();
            break;

        case 'D':
            do_date();
            break;
        default:
            fprintf(stderr, "Unknown option '%c'.\n", option[1]);
            exit(1);
        }
        exit(0);
    } else
        fprintf(stderr, "Bad arg count (%d).\n", argc - 1);
    exit(1);
}

void
do_traps()
{
    int ntrap, getpid();
    char tmpfile[30];
    FILE *freopen();

    sprintf(tmpfile, "makedefs.%d", getpid());
    if (freopen(tmpfile, WRMODE, stdout) == NULL) {
        perror(tmpfile);
        exit(1);
    }
    if (freopen(TRAP_FILE, RDMODE, stdin) == NULL) {
        perror(TRAP_FILE);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), stdin) != NULL) {
        fputs(in_line, stdout);
        if (!strncmp(in_line, "/* DO NOT REMOVE THIS LINE */", 29))
            break;
    }
    ntrap = 10;
    printf("\n");
#ifdef NEWTRAPS
    printf("#define\tMGTRP\t\t%d\n", ntrap++);
    printf("#define\tSQBRD\t\t%d\n", ntrap++);
#endif
#ifdef SPIDERS
    printf("#define\tWEB\t\t%d\n", ntrap++);
#endif
#ifdef NEWCLASS
    printf("#define\tSPIKED_PIT\t%d\n", ntrap++);
    printf("#define\tLEVEL_TELEP\t%d\n", ntrap++);
#endif
#ifdef SPELLS
    printf("#define\tANTI_MAGIC\t%d\n", ntrap++);
#endif
#ifdef KAA
    printf("#define\tRUST_TRAP\t%d\n", ntrap++);
#ifdef RPH
    printf("#define\tPOLY_TRAP\t%d\n", ntrap++);
#endif
#endif
#ifdef SAC
    printf("#define\tLANDMINE\t%d\n", ntrap++);
#endif /* SAC */
    printf("\n#define\tTRAPNUM\t%d\n", ntrap);
    fclose(stdin);
    fclose(stdout);
#ifdef MSDOS
    remove(TRAP_FILE);
#endif
    rename(tmpfile, TRAP_FILE);
}

struct hline {
    struct hline *next;
    char *line;
} * f_line;

void
do_rumors()
{
    char infile[30];
    FILE *freopen();

    if (freopen(RUMOR_FILE, WRMODE, stdout) == NULL) {
        perror(RUMOR_FILE);
        exit(1);
    }
#ifdef MSDOS
    sprintf(infile, "%s.bas", RUMOR_FILE);
#else
    sprintf(infile, "%s.base", RUMOR_FILE);
#endif
    if (freopen(infile, RDMODE, stdin) == NULL) {
        perror(infile);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), stdin) != NULL)
        fputs(in_line, stdout);

#ifdef KAA
    sprintf(infile, "%s.kaa", RUMOR_FILE);
    if (freopen(infile, RDMODE, stdin) == NULL)
        perror(infile);

    while (fgets(in_line, sizeof(in_line), stdin) != NULL)
        fputs(in_line, stdout);
#endif

#ifdef NEWCLASS
    sprintf(infile, "%s.mrx", RUMOR_FILE);
    if (freopen(infile, RDMODE, stdin) == NULL)
        perror(infile);

    while (fgets(in_line, sizeof(in_line), stdin) != NULL)
        fputs(in_line, stdout);
#endif
    fclose(stdin);
    fclose(stdout);
}

void
do_date()
{
    int getpid();
    long clock, time();
    char tmpfile[30], cbuf[30], *c, *ctime();
    FILE *freopen();

    sprintf(tmpfile, "makedefs.%d", getpid());
    if (freopen(tmpfile, WRMODE, stdout) == NULL) {
        perror(tmpfile);
        exit(1);
    }
    if (freopen(DATE_FILE, RDMODE, stdin) == NULL) {
        perror(DATE_FILE);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), stdin) != NULL) {
        if (!strncmp(in_line, "char datestring[] = ", 20))
            break;
        fputs(in_line, stdout);
    }
    time(&clock);
    strcpy(cbuf, ctime(&clock));
    for (c = cbuf; *c != '\n'; c++)
        ;
    *c = 0; /* strip off the '\n' */
    printf("char datestring[] = %c%s%c;\n", '"', cbuf, '"');

    fclose(stdin);
    fclose(stdout);
#ifdef MSDOS
    remove(DATE_FILE);
#endif
    rename(tmpfile, DATE_FILE);
}

void
do_data()
{
    int getpid();
    char tmpfile[30];
    FILE *freopen();

    sprintf(tmpfile, "%s.base", DATA_FILE);
    if (freopen(tmpfile, RDMODE, stdin) == NULL) {
        perror(tmpfile);
        exit(1);
    }
    if (freopen(DATA_FILE, WRMODE, stdout) == NULL) {
        perror(DATA_FILE);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), stdin) != NULL) {
#ifdef KOPS
        if (!strcmp(in_line, "K	a kobold\n"))
            printf("K\ta Keystone Kop\n");
        else
#endif
#ifdef KAA
            if (!strcmp(in_line, "Q	a quasit\n"))
            printf("Q\ta quantum mechanic\n");
        else
#endif
#ifdef ROCKMOLE
            if (!strcmp(in_line, "r	a giant rat\n"))
            printf("r\ta rockmole\n");
        else
#endif
#ifdef SPIDERS
            if (!strcmp(in_line, "s	a scorpion\n"))
            printf("s\ta giant spider\n");
        else if (!strcmp(in_line, "\"	an amulet\n"))
            printf("\"\tan amulet (or a web)\n");
        else
#endif
#ifdef SINKS
            if (!strcmp(in_line, "#	a corridor\n"))
            printf("#\ta corridor (or a kitchen sink)\n");
        else
#endif
#ifdef SPELLS
            if (!strcmp(in_line, "+	a door\n"))
            printf("+\ta door (or a spell book)\n");
        else
#endif
#ifdef FOUNTAINS
            if (!strcmp(in_line, "}	water filled area\n")) {
            fputs(in_line, stdout);
            printf("{\ta fountain\n");
        } else
#endif
#ifdef NEWCLASS
            if (!strcmp(in_line, "^	a trap\n")) {
            fputs(in_line, stdout);
            printf("\\\tan opulent throne.\n");
        } else
#endif
            fputs(in_line, stdout);
    }
#ifdef SAC
    printf("3\ta soldier;\n");
    printf(
        "\tThe soldiers  of Yendor are  well-trained in the art of war,\n");
    printf(
        "\tmany  trained by  the wizard himself.  Some say the soldiers\n");
    printf(
        "\tare explorers  who were  unfortunate enough  to be captured,\n");
    printf(
        "\tand  put under the wizard's spell.  Those who have  survived\n");
    printf(
        "\tencounters  with  soldiers   say  they  travel  together  in\n");
    printf(
        "\tplatoons,  and are fierce fighters.  Because of the  load of\n");
    printf(
        "\ttheir  combat gear,  however,  one can usually run away from\n");
    printf("\tthem, and doing so is considered a wise thing.\n");
#endif
#ifdef RPH
    printf("8\tthe medusa;\n");
    printf(
        "\tThis hideous  creature from  ancient Greek myth was the doom\n");
    printf(
        "\tof many a valiant adventurer.  It is said that one gaze from\n");
    printf(
        "\tits eyes  could turn a man to stone.  One bite from the nest\n");
    printf(
        "\tof  snakes which  crown its head could  cause instant death.\n");
    printf(
        "\tThe only  way to kill this  monstrosity is to turn its  gaze\n");
    printf("\tback upon itself.\n");
#endif
#ifdef KAA
    printf("9\ta giant;\n");
    printf(
        "\tGiants have always walked the earth, though they are rare in\n");
    printf(
        "\tthese times.  They range in size from  little over nine feet\n");
    printf(
        "\tto a towering twenty feet or more.  The larger ones use huge\n");
    printf(
        "\tboulders as weapons, hurling them over large distances.  All\n");
    printf(
        "\ttypes of giants share a love for men  -  roasted, boiled, or\n");
    printf("\tfried.  Their table manners are legendary.\n");
#endif
    fclose(stdin);
    fclose(stdout);
}

#define LINSZ 1000
#define STRSZ 40

int fd;
struct objdef {
    struct objdef *next;
    char string[STRSZ];
} * more, *current;

static int
do_objs()
{
    register int index = 0;
    register int propct = 0;
#ifdef SPELLS
    register int nspell = 0;
#endif
    FILE *freopen();
    register char *sp;
    char *limit();
    int skip;

    fd = open(OBJ_FILE, 0);
    if (fd < 0) {
        perror(OBJ_FILE);
        exit(1);
    }

    if (freopen(ONAME_FILE, WRMODE, stdout) == NULL) {
        perror(ONAME_FILE);
        exit(1);
    }

    current = 0;
    newobj();
    skipuntil("objects[] = {");

    while (getentry(&skip)) {
        if (!*(current->string)) {
            if (skip)
                index++;
            continue;
        }
        for (sp = current->string; *sp; sp++)
            if (*sp == ' ' || *sp == '\t' || *sp == '-')
                *sp = '_';

        /* Do not process duplicates caused by #ifdef/#else pairs. */
        /* M. Stephenson					   */
        if (!duplicate()) {
            if (!strncmp(current->string, "RIN_", 4))
                propct = specprop(current->string + 4, propct);
            for (sp = current->string; *sp; sp++)
                capitalize(sp);
            /* avoid trouble with stupid C preprocessors */
            if (!strncmp(current->string, "WORTHLESS_PIECE_OF_", 19))
                printf("/* #define\t%s\t%d */\n", current->string, index++);
            else {
#ifdef SPELLS
                if (!strncmp(current->string, "SPE_", 4))
                    nspell++;
                printf("#define\t%s\t%d\n", limit(current->string), index++);
#else
                if (strncmp(current->string, "SPE_", 4))
                    printf("#define\t%s\t%d\n", limit(current->string),
                           index++);
#endif
            }
            newobj();
        }
    }
    printf("\n#define	CORPSE		DEAD_HUMAN\n");
#ifdef KOPS
    printf("#define	DEAD_KOP		DEAD_KOBOLD\n");
#endif
#ifdef SPIDERS
    printf("#define	DEAD_GIANT_SPIDER	DEAD_GIANT_SCORPION\n");
#endif
#ifdef ROCKMOLE
    printf("#define	DEAD_ROCKMOLE		DEAD_GIANT_RAT\n");
#endif
#ifndef KAA
    printf("#define DEAD_QUASIT		DEAD_QUANTUM_MECHANIC\n");
    printf("#define DEAD_VIOLET_FUNGI	DEAD_VIOLET_FUNGUS\n");
#endif
    printf("#define	LAST_GEM	(JADE+1)\n");
    printf("#define	LAST_RING	%d\n", propct);
#ifdef SPELLS
    printf("#define MAXSPELL	%d\n", nspell + 1);
#endif
    printf("#define	NROFOBJECTS	%d\n", index - 1);
    exit(0);
}

static char temp[32];

char *limit(name) /* limit a name to 30 characters length */
char *name;
{
    strncpy(temp, name, 30);
    temp[30] = 0;
    return (temp);
}

void
newobj()
{
    extern long *alloc();

    more = current;
    current = (struct objdef *) alloc(sizeof(struct objdef));
    current->next = more;
}

struct inherent {
    char *attrib, *monsters;
} abilities[] = {
    { "Regeneration", "TVi" },
    { "See_invisible", "I" },
    { "Poison_resistance", "abcghikqsuvxyADFQSVWXZ&" },
    { "Fire_resistance", "gD&" },
    { "Cold_resistance", "gFY" },
    { "Shock_resistance", "g;" },
    { "Teleportation", "LNt" },
    { "Teleport_control", "t" },
    { "", "" }
};

static int
specprop(name, count)
char *name;
int count;
{
    int i;
    char *tname, *limit();

    tname = limit(name);
    capitalize(tname);
    for (i = 0; strlen(abilities[i].attrib); i++)
        if (!strcmp(abilities[i].attrib, tname)) {
            printf("#define\tH%s\tu.uprops[%d].p_flgs\n", tname, count);
            printf("#define\t%s\t((H%s) || index(\"%s\", u.usym))\n", tname,
                   tname, abilities[i].monsters);
            return (++count);
        }

    printf("#define\t%s\tu.uprops[%d].p_flgs\n", tname, count);
    return (++count);
}

char line[LINSZ], *lp = line, *lp0 = line, *lpe = line;
int xeof;

void
readline()
{
    register int n = read(fd, lp0, (line + LINSZ) - lp0);
    if (n < 0) {
        printf("Input error.\n");
        exit(1);
    }
    if (n == 0)
        xeof++;
    lpe = lp0 + n;
}

char
nextchar()
{
    if (lp == lpe) {
        readline();
        lp = lp0;
    }
    return ((lp == lpe) ? 0 : *lp++);
}

static int
skipuntil(s)
char *s;
{
    register char *sp0, *sp1;
loop:
    while (*s != nextchar())
        if (xeof) {
            printf("Cannot skipuntil %s\n", s);
            exit(1);
        }
    if (strlen(s) > lpe - lp + 1) {
        register char *lp1, *lp2;
        lp2 = lp;
        lp1 = lp = lp0;
        while (lp2 != lpe)
            *lp1++ = *lp2++;
        lp2 = lp0; /* save value */
        lp0 = lp1;
        readline();
        lp0 = lp2;
        if (strlen(s) > lpe - lp + 1) {
            printf("error in skipuntil");
            exit(1);
        }
    }
    sp0 = s + 1;
    sp1 = lp;
    while (*sp0 && *sp0 == *sp1)
        sp0++, sp1++;
    if (!*sp0) {
        lp = sp1;
        return (1);
    }
    goto loop;
}

static int
getentry(skip)
int *skip;
{
    int inbraces = 0, inparens = 0, stringseen = 0, commaseen = 0;
    int prefix = 0;
    char ch;
#define NSZ 10
    char identif[NSZ], *ip;
    current->string[0] = current->string[4] = 0;
    /* read until {...} or XXX(...) followed by ,
       skip comment and #define lines
       deliver 0 on failure
     */
    while (1) {
        ch = nextchar();
    swi:
        if (letter(ch)) {
            ip = identif;
            do {
                if (ip < identif + NSZ - 1)
                    *ip++ = ch;
                ch = nextchar();
            } while (letter(ch) || digit(ch));
            *ip = 0;
            while (ch == ' ' || ch == '\t')
                ch = nextchar();
            if (ch == '(' && !inparens && !stringseen)
                if (!strcmp(identif, "WAND") || !strcmp(identif, "RING")
                    || !strcmp(identif, "POTION") || !strcmp(identif, "SPELL")
                    || !strcmp(identif, "SCROLL"))
                    (void) strncpy(current->string, identif, 3),
                        current->string[3] = '_', prefix = 4;
        }
        switch (ch) {
        case '/':
            /* watch for comment */
            if ((ch = nextchar()) == '*')
                skipuntil("*/");
            goto swi;
        case '{':
            inbraces++;
            continue;
        case '(':
            inparens++;
            continue;
        case '}':
            inbraces--;
            if (inbraces < 0)
                return (0);
            continue;
        case ')':
            inparens--;
            if (inparens < 0) {
                printf("too many ) ?");
                exit(1);
            }
            continue;
        case '\n':
            /* watch for #define at begin of line */
            if ((ch = nextchar()) == '#') {
                register char pch;
                /* skip until '\n' not preceded by '\\' */
                do {
                    pch = ch;
                    ch = nextchar();
                } while (ch != '\n' || pch == '\\');
                continue;
            }
            goto swi;
        case ',':
            if (!inparens && !inbraces) {
                if (prefix && !current->string[prefix]) {
#ifndef SPELLS
                    *skip = strncmp(current->string, "SPE_", 4);
#else
                    *skip = 1;
#endif
                    current->string[0] = 0;
                }
                if (stringseen)
                    return (1);
                printf("unexpected ,\n");
                exit(1);
            }
            commaseen++;
            continue;
        case '\'':
            if ((ch = nextchar()) == '\\')
                ch = nextchar();
            if (nextchar() != '\'') {
                printf("strange character denotation?\n");
                exit(1);
            }
            continue;
        case '"': {
            register char *sp = current->string + prefix;
            register char pch;
            register int store =
                (inbraces || inparens) && !stringseen++ && !commaseen;
            do {
                pch = ch;
                ch = nextchar();
                if (store && sp < current->string + STRSZ)
                    *sp++ = ch;
            } while (ch != '"' || pch == '\\');
            if (store)
                *--sp = 0;
            continue;
        }
        }
    }
}

static int
duplicate()
{
    char s[STRSZ];
    register char *c;
    register struct objdef *testobj;

    strcpy(s, current->string);
    for (c = s; *c != 0; c++)
        capitalize(c);

    for (testobj = more; testobj != 0; testobj = testobj->next)
        if (!strcmp(s, testobj->string))
            return (1);

    return (0);
}

void
capitalize(sp)
register char *sp;
{
    if ('a' <= *sp && *sp <= 'z')
        *sp += 'A' - 'a';
}

static int
letter(ch)
register char ch;
{
    return (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'));
}

static int
digit(ch)
register char ch;
{
    return ('0' <= ch && ch <= '9');
}

/* a copy of the panic code from hack.pri.c, edited for standalone use */

boolean panicking = 0;

int
panic(str, a1, a2, a3, a4, a5, a6)
char *str;
int a1, a2, a3, a4, a5, a6;
{
    if (panicking++)
        exit(1); /* avoid loops - this should never happen*/
    fputs(" ERROR:  ", stdout);
    printf(str, a1, a2, a3, a4, a5, a6);
#ifdef DEBUG
#ifdef UNIX
    if (!fork())
        abort(); /* generate core dump */
#endif
#endif
    exit(1);
}

#if defined(SYSV) || defined(GENIX)
int
rename(oldname, newname)
const char *oldname, *newname;
{
    int rc = 0;
    if (strcmp(oldname, newname)) {
        rc = unlink(newname);
        if (rc == 0) {
            rc = link(oldname, newname);
        }
        if (rc == 0) {
            unlink(oldname);
        }
    }
    return rc;
}
#endif

#ifdef MSDOS
/* Get around bug in freopen when opening for writing	*/
/* Supplied by Nathan Glasser (nathan@mit-eddie)	*/
#undef freopen
FILE *_freopen(fname, fmode, fp)
char *fname, *fmode;
FILE *fp;
{
    if (!strncmp(fmode, "w", 1)) {
        FILE *tmpfp;

        if ((tmpfp = fopen(fname, fmode)) == NULL)
            return (NULL);
        if (dup2(fileno(tmpfp), fileno(fp)) < 0)
            return (NULL);
        fclose(tmpfp);
        return (fp);
    } else
        return (freopen(fname, fmode, fp));
}

#ifdef __TURBOC__
int
getpid()
{
    return (1);
}
#endif
#endif
