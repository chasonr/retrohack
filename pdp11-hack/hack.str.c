/*
 * Hack.str.c
 */

/*
 * strprint() : Cheap alternative for sprintf() and printf(), by
 * Fred and Michiel. If you want the normal io just define NORMAL_IO
 */


#include "hack.h"

#ifndef NORMAL_IO

#define ARINDEX		512
static char output_buffer[ARINDEX];
static arcount = 0;

char *litoa();

/*VARARGS10*/
strprint( dest, src, argptr )
register char *dest, *src;
register int *argptr;
{
	register kar;
	register char *string;
	register field;
	int negated;
	int printflag = 0;
	static char extra[512];

	if ( dest == STDOUT ) {
		dest = extra;
		printflag++;
	}

	while( *src ) {
		if ( *src == '%' ) {
			field = 0;
			negated = 0;
			if ( *++src == '-' )
				++negated;
			else
				--src;
			while( *++src & 060 && *src <= '9' )
				field = 10*field + ( *src & 017 );
			if ( (kar = *src) == '%' )
				*dest++ = '%';
			else if ( kar == 'c' )
				*dest++ = (char)*argptr++;
			else {
				switch ( kar ) {
					
				case 'd' :
					string = litoa( (long)(*argptr));
					break;
				case 'U' :
				case 'D' :
					string = litoa( *((long *)argptr++) );
					break;
				case 'u' :
					string =
					litoa( (long)( *((unsigned *)argptr)) );
					break;
				case 's' :
					string = (char *)(*argptr);
					break;
				default :
					strcpy( string, "%'" );
				}
				if ( field )
					field -= strlen( string );
				if ( field > 0 && !negated )
					while( --field >= 0 )
						*dest++ = ' ';
				while( *dest++ = *string++ )
					;
				dest--;  /* Remove '\0' character */
				if ( field > 0 && negated )
					while( --field >= 0 )
						*dest++ = ' ';
				argptr++;
			}
			src++;
		}	
		else
			*dest++ = *src++;
	}
	*dest = 0;
	if ( printflag ) {
		WRITE( extra, strlen(extra) );
	}
}

char *
litoa( l_arg )
long l_arg;
{
	static char string[20];
	register int i = 0;
	register char *str2 = string;
	char pipo[20];

	if ( !l_arg )
		return( "0" );
	if ( l_arg < 0L ) {
		*str2++ = '-';
		l_arg = -l_arg;
	}

	while( l_arg ) {
		pipo[i++] = (int)( l_arg % 10L + '0');
		l_arg /= 10L;
	}
	while( --i >= 0 ) /* Michiel */
		*str2++ = pipo[i];
	*str2 = '\0';
	return( string );
}

WRITE( string , length )
register char *string;
register length;
{
	if ( arcount + length >= ARINDEX )
		flush();
	strcat( output_buffer, string );
	arcount += length;
}

flush()
{
	write( 1, output_buffer, arcount );
	arcount = 0;
	output_buffer[0] = 0;
}

putchar ( c )
{
	WRITE( &c, 1 );
}

printf( fmt, args )
{
	strprint( STDOUT, fmt, &args );
}

sprintf( buffer, fmt, args )
{
	strprint( buffer, fmt, &args );
}
#endif NORMAL_IO
