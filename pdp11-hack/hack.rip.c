/*
 * Hack.rip.c
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "hack.h"

static void center (int line, char *text);

static char    *rip[] = {
	"                       ----------",
	"                      /          \\",
	"                     /    REST    \\",
	"                    /      IN      \\",
	"                   /     PEACE      \\",
	"                  /                  \\",
	"                  |                  |",
	"                  |                  |",
	"                  |   killed by a    |",
	"                  |                  |",
	"                  |       1001       |",
	"                 *|     *  *  *      | *",
	"        _________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______",
	0
};

void
outrip (void)
{
	register char **dp = rip;
	register struct tm     *lt;
	long    date;
	char    buffer[BUFSZ];

	time (&date);
	lt = localtime (&date);
	cls ();
	strcpy (buffer, plname);
	center (6, buffer);
	sprintf (buffer, "%ld Au", u.ugold);
	center (7, buffer);
	strcpy (buffer, killer);
	center (9, buffer);
	sprintf (buffer, "19%2d", lt -> tm_year);
	center (10, buffer);
	curs (1, 8);
	while (*dp)
		printf ("%s\n", *dp++);
	getret ();
}

static void
center (int line, char *text)
{
	register char  *ip, *op;

	ip = text;
	op = &rip[line][28 - ((strlen (text) + 1) >> 1)];
	while (*ip)
		*op++ = *ip++;
}
