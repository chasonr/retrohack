# hack makefile.
# layout en verdere verbeteringen Michiel en Fred

PLAYDIR = /usr/games/playdir
SAVEDIR = /usr/games/playdir/save

CFLAGS = -O -i
LIB = -ltermlib

TARGET = hack

HFILES = hack.c hack.lev.c hack.pri.c hack.do.c hack.do1.c hack.main.c\
hack.mon.c hack.dog.c hack.invent.c hack.eat.c hack.save.c\
hack.do_wear.c hack.shk.c hack.io.c hack.end.c hack.rip.c\
hack.bones.c hack.mkobj.c hack.monst.c hack.invinit.c hack.worm.c\
hack.debug.c hack.move.c hack.do.misc.c hack.mon.do.c hack.str.c\
hack.office.c

MFILES = mklev.c mklev.make.c mklev.mkobj.c mklev.svlev.c 

GFILES = rnd.c alloc.c

HOBJ = hack.o hack.lev.o hack.pri.o hack.do.o hack.do1.o hack.main.o\
hack.mon.o hack.dog.o hack.invent.o hack.eat.o hack.save.o\
hack.do_wear.o hack.shk.o hack.io.o hack.end.o hack.rip.o\
hack.bones.o hack.mkobj.o hack.monst.o hack.invinit.o hack.worm.o\
hack.debug.o hack.move.o hack.do.misc.o hack.mon.do.o hack.str.o\
hack.office.o

GOBJ = rnd.o alloc.o

MKOBJ = mklev.o mklev.make.o mklev.mkobj.o

all: hack mklev show

$(TARGET)   : $(HOBJ) $(GOBJ)
	@echo Loading...
	@$(CC) $(CFLAGS) -o $(TARGET) $(GOBJ) $(HOBJ) $(LIB)
	@size $(TARGET)
	@echo 'Done Hack'

$(HOBJ) $(MKOBJ) :	hack.h
hack.do.misc.o:		hack.do.vars.h envir.h
hack.vars.h:		hack.foods.h hack.armors.h hack.weapons.h
hack.lev.o:		hack.savelev.c
hack.dog.o hack.monst.o:        hack.dog.h
hack.mkobj.o mklev.mkobj.o: 	hack.vars.h
hack.main.o hack.lev.o hack.debug.o \
hack.do.misc.o hack.end.o hack.invinit.o: envir.h

hack.invinit.o:
	cc $(CFLAGS) -c hack.invinit.c

mklev.o: hack.h envir.h
	cc $(CFLAGS) -c mklev.c

mklev:  $(MKOBJ) $(GOBJ) mklev.svlev.c
	cc $(CFLAGS) $(MKOBJ) $(GOBJ) -o mklev
	@echo 'Done mklev'

show: show.c hack.h envir.h
	cc -O show.c -o show
	@echo 'Done show'

cleanup:
	rm -f $(HOBJ) $(GOBJ) $(PLAYDIR)/show $(PLAYDIR)/makelev \
	   $(PLAYDIR)/bones_* mklev.mkobj.o mklev.o

install: all 
	rm -f $(PLAYDIR)/bones_*
	install -c -m 4711 $(TARGET) $(PLAYDIR)/$(TARGET)
	install -c -m 711 mklev $(PLAYDIR)/mklev
	install -c -m 700 show $(PLAYDIR)/show

new: dirs install

dirs:
	mkdir $(PLAYDIR)
	mkdir $(SAVEDIR)
	mv help $(PLAYDIR)
	cp /dev/null $(PLAYDIR)/record
	chmod 700 $(PLAYDIR)/record

lint:
	lint -phbxac mk*.c rnd.c alloc.c -lc
