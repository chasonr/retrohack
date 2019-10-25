/*
 * Hack.office.c
 */

#include <time.h>

#define SUNDAY 0
#define SATURDAY 6

kantoor () {
	struct tm      *s;
	long    clock;

	time (&clock);
	s = localtime (&clock);
	return (s -> tm_wday != SUNDAY && s -> tm_wday != SATURDAY &&
			s -> tm_hour <= 17 && s -> tm_hour > 9);
}
