/*
 * Hack.debug.c
 *
 * Debug function, only exists when DEBUG  if defined
 * Michiel and Fred
 */

#include "hack.h"

#ifdef DEBUG
debug () {
	nomove ();
	pline( "You're in the debug function!" );
}
#endif DEBUG
