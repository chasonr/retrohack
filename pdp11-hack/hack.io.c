/*
 * Hack.io.c
 */

#include "hack.h"
#include <sgtty.h>

short   ospeed;			/* Used by tputs */

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

getret () {
	printf ("\nHit space to continue: ");
	flush ();
	while (getchar () != ' ');
}
/*
 * Put together cbreak-mode and echo --- Michiel
 */

hackmode (x)
register        x;
{
	struct sgttyb   ttyp;

	ioctl (1, TIOCGETP, &ttyp);/* Use of basic functions */
	ospeed = ttyp.sg_ospeed;
	if (x) {
		ttyp.sg_flags &= ~ECHO;
		ttyp.sg_flags |= CBREAK;
	}
	else {
		ttyp.sg_flags |= ECHO;
		ttyp.sg_flags &= ~CBREAK;
	}
	if (ioctl (1, TIOCSETP, &ttyp) < 0) {
		printf ("ERROR: Cannot change tty");
		exit (1);
	}
}

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
