/*
 * Hack.mkobj.c
 */

#include <string.h>
#include "hack.h"
#include "hack.vars.h"

static void savecalls (int fd, char *strings[], int max);
static void restcalls (int fd, char *strings[], int max);

static int
mkfood (void)
{
	register        FOOD fp;
	register int    i = rn2 (100);

	fp = &foods[0];
	while ((i -= fp -> prob) >= 0)
		fp++;
	return (fp - foods);
}

static int
mkarm (void)
{
	register        ARMOR ap;
	register int    i = rn2 (100);

	ap = &armors[0];
	while ((i -= ap -> prob) >= 0)
		ap++;
	return (ap - armors);
}

static int
mkwep (void)
{
	register        WEAPON wp;
	register int    i = rn2 (100);

	wp = &weapons[0];
	while ((i -= wp -> prob) >= 0)
		wp++;
	return (wp - weapons);
}

static char mkobjstr[] = "))[[!!!!????%%%%//=**";

void
mkobj (int let)
{
	register        OBJECT otmp;

	otmp = newobj ();
	otmp -> nobj = fobj;
	fobj = otmp;
	otmp -> known = 0;
	otmp -> cursed = 0;
	otmp -> spe = 0;
	otmp -> unpaid = 0;
	otmp -> quan = 1;
	if (!let)
		let = mkobjstr[rn2 (sizeof (mkobjstr) - 1)];
	otmp -> olet = let;
	switch (let) {

		case ')': 
			otmp -> otyp = mkwep ();
			if (otmp -> otyp <= W_AMMUNITION)
				otmp -> quan = rn1 (6, 6);
			if (!rn2 (11))
				otmp -> spe = rnd (3);
			else if (!rn2 (10)) {
				otmp -> cursed = 1;
				otmp -> spe = -rnd (3);
			}
			break;

		case '*': 
			otmp -> otyp = rn2 (SIZE (potcol));
			otmp -> quan = rn2 (6) ? 1 : 2;
			break;

		case '[': 
			otmp -> otyp = mkarm ();
			if (!rn2 (8))
				otmp -> cursed = 1;
			if (!rn2 (10))
				otmp -> spe = rnd (3);
			else if (!rn2 (9)) {
				otmp -> spe = -rnd (3);
				otmp -> cursed = 1;
			}
			otmp -> spe += 10 - armors[otmp -> otyp].a_ac;
			break;

		case '!': 
			otmp -> otyp = rn2 (SIZE (pottyp));
			break;

		case '?': 
			otmp -> otyp = rn2 (SIZE (scrtyp));
			break;

		case '%': 
			otmp -> otyp = mkfood ();
			otmp -> quan = rn2 (6) ? 1 : 2;
			break;

		case '/': 
			otmp -> otyp = rn2 (SIZE (wantyp));
			if (otmp -> otyp == Z_DEATH)
				otmp -> otyp = rn2 (SIZE (wantyp));
			otmp -> spe = rn1 (5, (otmp -> otyp <= Z_CREATE_MON) ?
					11 : 4);
		/* detection and light and create monster */
			break;

		case '=': 
			otmp -> otyp = rn2 (SIZE (ringtyp));
			if (otmp -> otyp >= R_GAIN_STR) {
				if (!rn2 (3)) {
					otmp -> spe = -rnd (2);
					otmp -> cursed = 1;
					break;
				}
				else
					otmp -> spe = rnd (2);
			}
			else if (otmp -> otyp == R_TELE ||
						otmp -> otyp == R_AGGRAV_MON ||
					otmp -> otyp == R_HUNGER)
				otmp -> cursed = 1;
			break;

		default: 
			panic (CORE, "Impossible mkobj");
	}
}

static void
shufl (char *base[], int num)
{
	char  **tmp, *tmp1;
	int     curnum;

	for (curnum = num - 1; curnum > 0; curnum--) {
		tmp = &base[rn2 (curnum)];
		tmp1 = *tmp;
		*tmp = base[curnum];
		base[curnum] = tmp1;
	}
}

void
shuffle (void)
{
	shufl (wannam, SIZE (wantyp));
	shufl (potcol, SIZE (potcol));
	shufl (rinnam, SIZE (ringtyp));
	shufl (scrnam, SIZE (scrtyp));
}

void
savenames (int fd)
{
	bwrite (fd, oiden, sizeof oiden);
	bwrite (fd, potcol, sizeof potcol);
	bwrite (fd, scrnam, sizeof scrnam);
	bwrite (fd, wannam, sizeof wannam);
	bwrite (fd, rinnam, sizeof rinnam);
}

void
restnames (int fd)
{
	mread (fd, oiden, sizeof oiden);
	mread (fd, potcol, sizeof potcol);
	mread (fd, scrnam, sizeof scrnam);
	mread (fd, wannam, sizeof wannam);
	mread (fd, rinnam, sizeof rinnam);
}

/* Restore the names we have given to things */
void
callsrestore (int fd)
{
	restcalls (fd, potcall, SIZE (pottyp));
	restcalls (fd, wandcall, SIZE (wantyp));
	restcalls (fd, ringcall, SIZE (ringtyp));
	restcalls (fd, scrcall, SIZE (scrtyp));
}

/* Save things we have given names to */
void
callssave (int fd)
{
	savecalls (fd, potcall, SIZE (pottyp));
	savecalls (fd, wandcall, SIZE (wantyp));
	savecalls (fd, ringcall, SIZE (ringtyp));
	savecalls (fd, scrcall, SIZE (scrtyp));
}

static void
savecalls (int fd, char *strings[], int max)
{
	register int    teller;

	for (teller = 0; teller < max; ++teller) {
		if (strings[teller])
			bwrite (fd, strings[teller],
					strlen (strings[teller]) + 1);
		else
			bwrite (fd, "\0", 1);
	}
}

static void
restcalls (int fd, char *strings[], int max)
{
	register int    teller;
	char   *str;
	int     cnt;
	char    buffer[BUFSZ];

	str = NULL;
	for (teller = 0; teller < max; ++teller) {
		cnt = -1;
		do {
			++cnt;
			mread (fd, str, 1);
			buffer[cnt] = *str;
		} while (*str != '\0');
		if (cnt) {
			strings[teller] = alloc (strlen (buffer) + 1) -> Val;
			strcpy (strings[teller], buffer);
		}
	}
}
