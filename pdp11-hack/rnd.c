/*
 * Rnd.c
 */

#define RND(x)	((rand() >> 3) % x)

rn1 (x, y)
register        x, y;
{
	return (RND (x) + y);
}

rn2 (x)
register        x;
{
	return (RND (x));
}

rnd (x)
register        x;
{
	return (RND (x) + 1);
}

d (n, x)
register        n, x;
{
	register        tmp = n;

	while (n--)
		tmp += RND (x);
	return tmp;
}
