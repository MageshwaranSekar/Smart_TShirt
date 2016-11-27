CC= gcc
OBJS_SERVEUR=serveur.o capteur.o http.o
HEADER_FILES=serveur.h http.h capteur.h
CFLAGS = -g -Wall -I../Communication -I../Threads
LDFLAGS = -lpthread -L../Threads -lthrd -L../Communication -lcom
EXEC = serveur

all: $(EXEC)

debug: CFLAGS += -DDEBUG
debug: $(EXEC)

clean: 
	rm -f core *.o *~ $(EXEC)

#Inclure les fichiers capteurs dans rm
cleaner:
	rm -rf core *.o *~ $(EXEC) tsv_files/*
#
# Les cibles
#


serveur: $(OBJS_SERVEUR)
	$(CC) $(CFLAGS) -o $@ $(OBJS_SERVEUR) $(LDFLAGS)


%.o : %.c $(HEADER_FILES)
	$(CC) $(CFLAGS) -c $<

#http.o: http.c $(HEADER_FILES)
#capteur.o: capteur.c $(HEADER_FILES)
#serveur.o: serveur.c $(HEADER_FILES)


