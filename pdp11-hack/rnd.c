/*
 * Rnd.c
 */

#include <stdlib.h>

#define RND(x)	((rand() >> 3) % x)

int
rn1 (x, y)
register int    x, y;
{
	return (RND (x) + y);
}

int
rn2 (x)
register int    x;
{
	return (RND (x));
}

int
rnd (x)
register int    x;
{
	return (RND (x) + 1);
}

int
d (n, x)
register int    n, x;
{
	register int    tmp = n;

	while (n--)
		tmp += RND (x);
	return tmp;
}
