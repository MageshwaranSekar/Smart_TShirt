#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "libthrd.h"
#define MAX_MUTX 30
static pthread_once_t do_once = PTHREAD_ONCE_INIT;
pthread_mutex_t mutexes[MAX_MUTX];
unsigned char nbThread=0;

void init_mutex(){
	int i;
	for(i=0; i<MAX_MUTX; i++)	
		pthread_mutex_init(mutexes+i, NULL);
}


void P(int sem){
	pthread_once(&do_once, init_mutex);//initialised only once
	pthread_mutex_lock(mutexes+sem);
}

void V(int sem){
	pthread_mutex_unlock(mutexes+sem);//available for next operation
}

void *clientThread(void *arg){
	int status;
	P(0);
	nbThread++;
	V(0);
	threadStruct *thrStr;
   	thrStr=(threadStruct *)arg;
    	thrStr->callback(thrStr->arg);
	free(thrStr->arg);
    	free(arg);
	P(0);
	nbThread--;
	V(0);
	pthread_exit((void *)&status);
}

int lanceThread(void (*function)(void *), void *arg, int taille){
	pthread_t pthr;
	pthread_attr_t attr;

	threadStruct *thrStr = (threadStruct *)malloc(sizeof(threadStruct));
	if(thrStr==NULL) {perror("struct malloc");exit(EXIT_FAILURE);}
	thrStr->callback=function;
	if(taille>0){
    		thrStr->arg=(threadStruct *)malloc(taille);
		if(thrStr->arg==NULL) {perror("arg malloc");exit(EXIT_FAILURE);}
		memcpy((void *) (thrStr->arg), arg, taille);
		
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	int err = pthread_create(&pthr, &attr, clientThread, (void *)thrStr);
	if(err){perror("pthread_create");exit(EXIT_FAILURE);}
	return pthr;
}

int getThreadCount(){
	return nbThread;
}
