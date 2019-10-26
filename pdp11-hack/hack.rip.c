/*
 * Hack.rip.c
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "hack.h"

static char *center (const char *line, const char *text);

static const char * const rip[] = {
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
	unsigned dp;
	struct tm *lt;
	time_t  date;
	char    buffer[BUFSZ];

	time (&date);
	cls ();
	curs (1, 8);
	for (dp = 0; rip[dp] != NULL; ++dp) {
		switch (dp) {
		case 6:
			sprintf (buffer, "%s", plname);
			break;
		case 7:
			sprintf (buffer, "%ld Au", u.ugold);
			break;
		case 9:
			sprintf (buffer, "%s", killer);
			break;
		case 10:
			lt = localtime (&date);
			sprintf (buffer, "%04d", lt -> tm_year + 1900);
			break;
		default:
			buffer[0] = '\0';
			break;
		}
		printf ("%s\n", center(rip[dp], buffer));
	}
	getret ();
}

static char *
center (const char *line, const char *text)
{
	static char buf[80];
	char *op;

	sprintf (buf, "%s", line);
	op = &buf[28 - ((strlen (text) + 1) >> 1)];
	memcpy (op, text, strlen (text));
	return buf;
}
