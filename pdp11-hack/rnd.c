/*
 * Rnd.c
 */

#include <stdlib.h>

#define RND(x)	((rand() >> 3) % x)

int
rn1 (int x, int y)
{
	return (RND (x) + y);
}

int
rn2 (int x)
{
	return (RND (x));
}

int
rnd (int x)
{
	return (RND (x) + 1);
}

int
d (int n, int x)
{
	register int    tmp = n;

	while (n--)
		tmp += RND (x);
	return tmp;
}
