/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* makedefs.c - NetHack version 2.3 */

/*static char SCCS_Id[] = "@(#)makedefs.c	2.3\t88/02/18";*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"
#include "alloc.h"
#include "panic.h"

#ifdef MSDOS
#undef exit
#define alloc malloc
#define RDMODE "r"
#define WRMODE "w"
#else
#define RDMODE "r+"
#define WRMODE "w+"
#endif

static int do_objs(void);
static int skipuntil(const char *s);
static int getentry(int *skip);
static int duplicate(void);
static int specprop(FILE *out_file, char *name, int count);
static int letter(char ch);
static int digit(char ch);
static char *limit(char *name);

/* construct definitions of object constants */
#define OBJ_FILE "objects.h"
#define ONAME_FILE "onames.h"
#define TRAP_IN_FILE "trap_in.h"
#define TRAP_FILE "trap.h"
#define DATE_IN_FILE "date_in.h"
#define DATE_FILE "date.h"
#define RUMOR_FILE "rumors"
#define DATA_FILE "data"

static char in_line[256];

static void do_traps(void);
static void do_rumors(void);
static void do_date(void);
static void do_data(void);
static void newobj(void);
static void capitalize(char *sp);

int
main(int argc, char *argv[])
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

static void
do_traps(void)
{
    int ntrap;
    FILE *inp_file, *out_file;

    if ((out_file = fopen(TRAP_FILE, WRMODE)) == NULL) {
        perror(TRAP_FILE);
        exit(1);
    }
    if ((inp_file = fopen(TRAP_IN_FILE, RDMODE)) == NULL) {
        perror(TRAP_IN_FILE);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), inp_file) != NULL) {
        fputs(in_line, out_file);
        if (!strncmp(in_line, "/* DO NOT REMOVE THIS LINE */", 29))
            break;
    }
    ntrap = 10;
    fprintf(out_file, "\n");
#ifdef NEWTRAPS
    fprintf(out_file, "#define\tMGTRP\t\t%d\n", ntrap++);
    fprintf(out_file, "#define\tSQBRD\t\t%d\n", ntrap++);
#endif
#ifdef SPIDERS
    fprintf(out_file, "#define\tWEB\t\t%d\n", ntrap++);
#endif
#ifdef NEWCLASS
    fprintf(out_file, "#define\tSPIKED_PIT\t%d\n", ntrap++);
    fprintf(out_file, "#define\tLEVEL_TELEP\t%d\n", ntrap++);
#endif
#ifdef SPELLS
    fprintf(out_file, "#define\tANTI_MAGIC\t%d\n", ntrap++);
#endif
#ifdef KAA
    fprintf(out_file, "#define\tRUST_TRAP\t%d\n", ntrap++);
#ifdef RPH
    fprintf(out_file, "#define\tPOLY_TRAP\t%d\n", ntrap++);
#endif
#endif
#ifdef SAC
    fprintf(out_file, "#define\tLANDMINE\t%d\n", ntrap++);
#endif /* SAC */
    fprintf(out_file, "\n#define\tTRAPNUM\t%d\n", ntrap);
    fclose(inp_file);
    fclose(out_file);
}

static void
do_rumors(void)
{
    char infile[30];
    FILE *inp_file, *out_file;

    if ((out_file = fopen(RUMOR_FILE, WRMODE)) == NULL) {
        perror(RUMOR_FILE);
        exit(1);
    }
#ifdef MSDOS
    sprintf(infile, "%s.bas", RUMOR_FILE);
#else
    sprintf(infile, "%s.base", RUMOR_FILE);
#endif
    if ((inp_file = fopen(infile, RDMODE)) == NULL) {
        perror(infile);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), inp_file) != NULL)
        fputs(in_line, out_file);

#ifdef KAA
    fclose(inp_file);
    sprintf(infile, "%s.kaa", RUMOR_FILE);
    if ((inp_file = fopen(infile, RDMODE)) == NULL)
        perror(infile);

    while (fgets(in_line, sizeof(in_line), inp_file) != NULL)
        fputs(in_line, out_file);
#endif

#ifdef NEWCLASS
    sprintf(infile, "%s.mrx", RUMOR_FILE);
    fclose(inp_file);
    if ((inp_file = fopen(infile, RDMODE)) == NULL)
        perror(infile);

    while (fgets(in_line, sizeof(in_line), inp_file) != NULL)
        fputs(in_line, out_file);
#endif
    fclose(inp_file);
    fclose(out_file);
}

static void
do_date(void)
{
    long clock;
    char cbuf[30], *c;
    FILE *inp_file, *out_file;

    if ((out_file = fopen(DATE_FILE, WRMODE)) == NULL) {
        perror(DATE_FILE);
        exit(1);
    }
    if ((inp_file = fopen(DATE_IN_FILE, RDMODE)) == NULL) {
        perror(DATE_IN_FILE);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), inp_file) != NULL) {
        if (!strncmp(in_line, "char datestring[] = ", 20))
            break;
        fputs(in_line, out_file);
    }
    time(&clock);
    strcpy(cbuf, ctime(&clock));
    for (c = cbuf; *c != '\n'; c++)
        ;
    *c = 0; /* strip off the '\n' */
    fprintf(out_file, "char datestring[] = %c%s%c;\n", '"', cbuf, '"');

    fclose(inp_file);
    fclose(out_file);
}

static void
do_data(void)
{
    char tmpfile[30];
    FILE *inp_file, *out_file;

    sprintf(tmpfile, "%s.base", DATA_FILE);
    if ((inp_file = fopen(tmpfile, RDMODE)) == NULL) {
        perror(tmpfile);
        exit(1);
    }
    if ((out_file = fopen(DATA_FILE, WRMODE)) == NULL) {
        perror(DATA_FILE);
        exit(1);
    }

    while (fgets(in_line, sizeof(in_line), inp_file) != NULL) {
#ifdef KOPS
        if (!strcmp(in_line, "K	a kobold\n"))
            fprintf(out_file, "K\ta Keystone Kop\n");
        else
#endif
#ifdef KAA
            if (!strcmp(in_line, "Q	a quasit\n"))
            fprintf(out_file, "Q\ta quantum mechanic\n");
        else
#endif
#ifdef ROCKMOLE
            if (!strcmp(in_line, "r	a giant rat\n"))
            fprintf(out_file, "r\ta rockmole\n");
        else
#endif
#ifdef SPIDERS
            if (!strcmp(in_line, "s	a scorpion\n"))
            fprintf(out_file, "s\ta giant spider\n");
        else if (!strcmp(in_line, "\"	an amulet\n"))
            fprintf(out_file, "\"\tan amulet (or a web)\n");
        else
#endif
#ifdef SINKS
            if (!strcmp(in_line, "#	a corridor\n"))
            fprintf(out_file, "#\ta corridor (or a kitchen sink)\n");
        else
#endif
#ifdef SPELLS
            if (!strcmp(in_line, "+	a door\n"))
            fprintf(out_file, "+\ta door (or a spell book)\n");
        else
#endif
#ifdef FOUNTAINS
            if (!strcmp(in_line, "}	water filled area\n")) {
            fputs(in_line, out_file);
            fprintf(out_file, "{\ta fountain\n");
        } else
#endif
#ifdef NEWCLASS
            if (!strcmp(in_line, "^	a trap\n")) {
            fputs(in_line, out_file);
            fprintf(out_file, "\\\tan opulent throne.\n");
        } else
#endif
            fputs(in_line, out_file);
    }
#ifdef SAC
    fprintf(out_file, "3\ta soldier;\n");
    fprintf(out_file, 
        "\tThe soldiers  of Yendor are  well-trained in the art of war,\n");
    fprintf(out_file, 
        "\tmany  trained by  the wizard himself.  Some say the soldiers\n");
    fprintf(out_file, 
        "\tare explorers  who were  unfortunate enough  to be captured,\n");
    fprintf(out_file, 
        "\tand  put under the wizard's spell.  Those who have  survived\n");
    fprintf(out_file, 
        "\tencounters  with  soldiers   say  they  travel  together  in\n");
    fprintf(out_file, 
        "\tplatoons,  and are fierce fighters.  Because of the  load of\n");
    fprintf(out_file, 
        "\ttheir  combat gear,  however,  one can usually run away from\n");
    fprintf(out_file, "\tthem, and doing so is considered a wise thing.\n");
#endif
#ifdef RPH
    fprintf(out_file, "8\tthe medusa;\n");
    fprintf(out_file, 
        "\tThis hideous  creature from  ancient Greek myth was the doom\n");
    fprintf(out_file, 
        "\tof many a valiant adventurer.  It is said that one gaze from\n");
    fprintf(out_file, 
        "\tits eyes  could turn a man to stone.  One bite from the nest\n");
    fprintf(out_file, 
        "\tof  snakes which  crown its head could  cause instant death.\n");
    fprintf(out_file, 
        "\tThe only  way to kill this  monstrosity is to turn its  gaze\n");
    fprintf(out_file, "\tback upon itself.\n");
#endif
#ifdef KAA
    fprintf(out_file, "9\ta giant;\n");
    fprintf(out_file, 
        "\tGiants have always walked the earth, though they are rare in\n");
    fprintf(out_file, 
        "\tthese times.  They range in size from  little over nine feet\n");
    fprintf(out_file, 
        "\tto a towering twenty feet or more.  The larger ones use huge\n");
    fprintf(out_file, 
        "\tboulders as weapons, hurling them over large distances.  All\n");
    fprintf(out_file, 
        "\ttypes of giants share a love for men  -  roasted, boiled, or\n");
    fprintf(out_file, "\tfried.  Their table manners are legendary.\n");
#endif
    fclose(inp_file);
    fclose(out_file);
}

#define LINSZ 1000
#define STRSZ 40

static int fd;
static struct objdef {
    struct objdef *next;
    char string[STRSZ];
} * more, *current;

static int
do_objs(void)
{
    register int index = 0;
    register int propct = 0;
#ifdef SPELLS
    register int nspell = 0;
#endif
    register char *sp;
    int skip;
    FILE *out_file;

    fd = open(OBJ_FILE, 0);
    if (fd < 0) {
        perror(OBJ_FILE);
        exit(1);
    }

    if ((out_file = fopen(ONAME_FILE, WRMODE)) == NULL) {
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
                propct = specprop(out_file, current->string + 4, propct);
            for (sp = current->string; *sp; sp++)
                capitalize(sp);
            /* avoid trouble with stupid C preprocessors */
            if (!strncmp(current->string, "WORTHLESS_PIECE_OF_", 19))
                fprintf(out_file, "/* #define\t%s\t%d */\n", current->string, index++);
            else {
#ifdef SPELLS
                if (!strncmp(current->string, "SPE_", 4))
                    nspell++;
                fprintf(out_file, "#define\t%s\t%d\n", limit(current->string), index++);
#else
                if (strncmp(current->string, "SPE_", 4))
                    fprintf(out_file, "#define\t%s\t%d\n", limit(current->string),
                           index++);
#endif
            }
            newobj();
        }
    }
    fprintf(out_file, "\n#define	CORPSE		DEAD_HUMAN\n");
#ifdef KOPS
    fprintf(out_file, "#define	DEAD_KOP		DEAD_KOBOLD\n");
#endif
#ifdef SPIDERS
    fprintf(out_file, "#define	DEAD_GIANT_SPIDER	DEAD_GIANT_SCORPION\n");
#endif
#ifdef ROCKMOLE
    fprintf(out_file, "#define	DEAD_ROCKMOLE		DEAD_GIANT_RAT\n");
#endif
#ifndef KAA
    fprintf(out_file, "#define DEAD_QUASIT		DEAD_QUANTUM_MECHANIC\n");
    fprintf(out_file, "#define DEAD_VIOLET_FUNGI	DEAD_VIOLET_FUNGUS\n");
#endif
    fprintf(out_file, "#define	LAST_GEM	(JADE+1)\n");
    fprintf(out_file, "#define	LAST_RING	%d\n", propct);
#ifdef SPELLS
    fprintf(out_file, "#define MAXSPELL	%d\n", nspell + 1);
#endif
    fprintf(out_file, "#define	NROFOBJECTS	%d\n", index - 1);
    fclose(out_file);
    exit(0);
}

static char temp[32];

static char *
limit(char *name) /* limit a name to 30 characters length */
{
    strncpy(temp, name, 30);
    temp[30] = 0;
    return (temp);
}

static void
newobj(void)
{
    more = current;
    current = (struct objdef *) alloc(sizeof(struct objdef));
    current->next = more;
}

static struct inherent {
    const char *attrib, *monsters;
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
specprop(FILE *out_file, char *name, int count)
{
    int i;
    char *tname;

    tname = limit(name);
    capitalize(tname);
    for (i = 0; strlen(abilities[i].attrib); i++)
        if (!strcmp(abilities[i].attrib, tname)) {
            fprintf(out_file, "#define\tH%s\tu.uprops[%d].p_flgs\n", tname, count);
            fprintf(out_file, "#define\t%s\t((H%s) || index(\"%s\", u.usym))\n", tname,
                   tname, abilities[i].monsters);
            return (++count);
        }

    fprintf(out_file, "#define\t%s\tu.uprops[%d].p_flgs\n", tname, count);
    return (++count);
}

static char line[LINSZ], *lp = line, *lp0 = line, *lpe = line;
static int xeof;

static void
readline(void)
{
    register int n = read(fd, lp0, (line + LINSZ) - lp0);
    if (n < 0) {
        fprintf(stderr, "Input error.\n");
        exit(1);
    }
    if (n == 0)
        xeof++;
    lpe = lp0 + n;
}

static char
nextchar(void)
{
    if (lp == lpe) {
        readline();
        lp = lp0;
    }
    return ((lp == lpe) ? 0 : *lp++);
}

static int
skipuntil(const char *s)
{
    const char *sp0;
    char *sp1;
loop:
    while (*s != nextchar())
        if (xeof) {
            fprintf(stderr, "Cannot skipuntil %s\n", s);
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
            fprintf(stderr, "error in skipuntil");
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
getentry(int *skip)
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
                fprintf(stderr, "too many ) ?");
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
                fprintf(stderr, "unexpected ,\n");
                exit(1);
            }
            commaseen++;
            continue;
        case '\'':
            if ((ch = nextchar()) == '\\')
                ch = nextchar();
            if (nextchar() != '\'') {
                fprintf(stderr, "strange character denotation?\n");
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
duplicate(void)
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

static void
capitalize(register char *sp)
{
    if ('a' <= *sp && *sp <= 'z')
        *sp += 'A' - 'a';
}

static int
letter(register char ch)
{
    return (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'));
}

static int
digit(register char ch)
{
    return ('0' <= ch && ch <= '9');
}

/* a copy of the panic code from hack.pri.c, edited for standalone use */

static boolean panicking = 0;

void
panic(const char *str, ...)
{
    va_list args;

    if (panicking++)
        exit(1); /* avoid loops - this should never happen*/
    fputs(" ERROR:  ", stderr);
    va_start(args, str);
    vfprintf(stderr, str, args);
    va_end(args);
#ifdef DEBUG
#ifdef UNIX
    if (!fork())
        abort(); /* generate core dump */
#endif
#endif
    exit(1);
}
