/*
 * Hack.office.c
 */

struct tm {			/* see ctime(3) */
	int     tm_sec;
	int     tm_min;
	int     tm_hour;
	int     tm_mday;
	int     tm_mon;
	int     tm_year;
	int     tm_wday;
	int     tm_yday;
	int     tm_isdst;
};


#define SUNDAY 0
#define SATURDAY 6

extern struct tm       *localtime ();

kantoor () {
	struct tm      *s;
	long    clock;

	time (&clock);
	s = localtime (&clock);
	return (s -> tm_wday != SUNDAY && s -> tm_wday != SATURDAY &&
			s -> tm_hour <= 17 && s -> tm_hour > 9);
}
