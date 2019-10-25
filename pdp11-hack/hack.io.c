/*
 * Hack.io.c
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "hack.h"

short   ospeed;			/* Used by tputs */

void
getlin (str)
register char  *str;
{
	register char  *ostr = str;

	flags.topl = 0;
	flush ();
	for (;;) {
		*str = getchar ();
		if (*str == '\b') {
			if (str != ostr) {
				str--;
				write (1, "\b \b", 3);
			}
			else
				write (1, "\7", 1);
		}
		else if (*str == '\n') {
			*str = 0;
			return;
		}
		else if (*str >= ' ') {
			write (1, str, 1);
			str++;
		}
	}
}

void
getret () {
	printf ("\nHit space to continue: ");
	flush ();
	while (getchar () != ' ');
}
/*
 * Put together cbreak-mode and echo --- Michiel
 */

void
hackmode (x)
register        x;
{
	struct termios ttyp;
	static struct termios tty_save;
	static int tty_saved = 0;

	if (x) {
		tcgetattr(1, &ttyp);
		if (!tty_saved) {
			tty_save = ttyp;
			tty_saved = 1;
		}
		ospeed = cfgetospeed(&ttyp);
		cfmakeraw(&ttyp);
		tcsetattr(1, TCSAFLUSH, &ttyp);
	}
	else {
		if (tty_saved) {
			tcsetattr(1, TCSAFLUSH, &tty_save);
		}
	}
}

void
more () {
	printf (" --More--");
	flush ();
	while (getchar () != ' ');
	flags.topl = 0;
}

#ifndef NORMAL_IO
getchar () {
	char    c;

	read (0, &c, 1);
	return (c);
}
#endif NORMAL_IO
