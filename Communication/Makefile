#
# Makefile pour generer la bibliotheque de fonctions de communication
#
CC= gcc
LIBFILE= libcom.a
CLIB= ar cq
CFLAGS = -g -Wall

all: $(LIBFILE)
debug: CFLAGS += -DDEBUG
debug: $(LIBFILE)
#
# La cible de nettoyage
#

clean: 
	rm -f core *.o $(LIBFILE) *~

#
# Les cibles pour la bibliotheque
#

libcom.o: libcom.c libcom.h
 
libcom.a: libcom.o
	rm -f $@
	$(CLIB) $@ $+
