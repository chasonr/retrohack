#!/bin/sh
#	SCCS Id: @(#)nethack.sh	1.4	87/08/08
HACKDIR=/usr/games/lib/nethack23dir
HACK=$HACKDIR/nethack23
MAXNROFPLAYERS=4

cd $HACKDIR
case $1 in
	-s*)
		exec $HACK $@
		;;
	*)
		exec $HACK $@ $MAXNROFPLAYERS
		;;
esac
