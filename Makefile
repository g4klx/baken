BINDIR=/usr/bin
MAPDIR=/usr/share/baken
RCDIR=/etc
VARDIR=/var/baken

CC=gcc

CFLAGS=-O2 -Wall -DGTK_CAN_DEFAULT -DMAPDIR=\"$(MAPDIR)\" -DRCDIR=\"$(RCDIR)\" -DVARDIR=\"$(VARDIR)\" -I /usr/include/gtk-3.0 -I /usr/include/glib-2.0
LDFLAGS=

LIBS=

all:		baken iaru old2new rslist

baken:		baken.o choose.o colour.o data_ed.o detail_map.o distance.o get_data.o list.o main_map.o reflection.o utils.o
		$(CC) $(LDFLAGS) baken.o choose.o colour.o data_ed.o detail_map.o distance.o get_data.o list.o main_map.o reflection.o utils.o -o baken `pkg-config gtk+-3.0 --libs` $(LIBS)

iaru:		iaru.o
		$(CC) $(LDFLAGS) iaru.o -o iaru

old2new:	old2new.o
		$(CC) $(LDFLAGS) old2new.o -o old2new

rslist:		rslist.o
		$(CC) $(LDFLAGS) rslist.o -o rslist

baken.o:	baken.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c baken.c

choose.o:	choose.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c choose.c

colour.o:	colour.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c colour.c

data_ed.o:	data_ed.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c data_ed.c

detail_map.o:	detail_map.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c detail_map.c

distance.o:	distance.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c distance.c

get_data.o:	get_data.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c get_data.c

iaru.o:		iaru.c
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c iaru.c

list.o:		list.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c list.c

main_map.o:	main_map.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c main_map.c

old2new.o:	old2new.c
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c old2new.c

reflection.o:	reflection.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c reflection.c

utils.o:	utils.c global.h
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c utils.c

rslist.o:	rslist.c
		$(CC) $(CFLAGS) `pkg-config gtk+-3.0 --cflags` -c rslist.c

clean:
		rm -f core baken iaru old2new rslist *.o *~ *.bak

install:	install-map install-data install-prog

install-map:
		install -d -g bin -o root -m 0775 $(MAPDIR)
		install -g bin -o root -m 0664 mapfile $(MAPDIR)

install-data:
		install -d -g bin -o root -m 0775 $(VARDIR)
		install -d -g bin -o root -m 0775 $(MAPDIR)
		install -g bin -o root -m 0664 iarufile $(VARDIR)
		install -g bin -o root -m 0664 noniarufile $(VARDIR)
		install -g bin -o root -m 0664 repeaterfile $(VARDIR)
		install -g bin -o root -m 0664 linearfile $(VARDIR)
		install -g bin -o root -m 0664 mailboxfile $(VARDIR)
		install -g bin -o root -m 0664 tvfile $(VARDIR)
		install -g bin -o root -m 0664 dxclusterfile $(VARDIR)
		install -g bin -o root -m 0664 nodefile $(VARDIR)
		install -g bin -o root -m 0664 miscfile $(VARDIR)
		install -g bin -o root -m 0664 rsfile $(VARDIR)
		install -g bin -o root -m 0664 cityfile $(MAPDIR)
		install -g bin -o root -m 0664 bakenrc $(RCDIR)

install-prog:
		install -g bin -o root -m 0775 baken $(BINDIR)

uninstall:
		rm -f $(BINDIR)/baken
		rm -rf $(MAPDIR)
		rm -f $(RCDIR)/bakenrc
		rm -rf $(VARDIR)
