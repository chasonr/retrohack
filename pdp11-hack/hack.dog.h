/*
 * Hack.dog.h
 */

struct edog {
	unsigned        hungrytime;	/* At this time dog gets hungry */
	unsigned        eattime;	/* Dog is eating */
	unsigned        droptime;	/* Moment dog dropped object */
	unsigned        dropdist;	/* Dist of drpped obj from @ */
	unsigned        apport;		/* Amount of training */
	unsigned        carry:	1;	/* The dog is carrying sth */
};
