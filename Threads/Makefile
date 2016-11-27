#
# Makefile pour generer la bibliotheque de threads
#
CC= gcc
CLIB= ar cq
CFLAGS = -g -Wall
LIBFILE= libthrd.a


all: $(LIBFILE)

debug: CFLAGS += -DDEBUG
debug: $(LIBFILE)
#
# La cible de nettoyage
#

clean: 
	rm -f core *.o *~ $(LIBFILE)

#
# Les cibles pour la bibliotheque
#

libthrd.o: libthrd.c libthrd.h
 
libthrd.a: libthrd.o
	rm -f $@
	$(CLIB) $@ $+


