#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "http.h"
#include "libcom.h"
#include "capteur.h"
#include "libthrd.h"
#include "serveur.h"

char *serviceUDP="12345";

/*Prototypes*/
void handle(int sig);
void aff_usage();
void gestionOpt(int argc, char **argv, char *service);
unsigned char threadCount;
struct sigaction action;
char *service;
int s;
int main(int argc, char **argv){
	
	service=(char *)malloc(BUFSIZE*sizeof(char));
	gestionOpt(argc, argv, service);
	
	action.sa_handler=handle;
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);	

	lanceThread((void *) TCPServeurThread, (void *) service, BUFSIZE);
	serveurMessages(serviceUDP, gestionMessage);
	
	return 0;
}

void handle(int sig){
	
	if(sig==SIGINT || sig==SIGQUIT){
		printf("Interruption\n");
		stopServerLoop();
		
		while((threadCount=getThreadCount())>0){			
			printf("THR count : %d\n",getThreadCount());
			if(threadCount==1) break;//if and only if the TCPServeurThread remains => dont wait
			sleep(1);		
		}
		free(service);	
		exit(sig);
	}
}

void aff_usage(){
	printf("Lancer avec option -p <num_port> ou --port <num_port>\n");
}

void gestionOpt(int argc, char **argv, char *service){
	static struct option long_options[] = {
        {"port",required_argument,0,'p'},
        {0,0,0,0}
   	};
	int port=-1;
	int long_index =0, opt=0;
	while ((opt = getopt_long(argc, argv,"p:", long_options, &long_index )) != -1) {
		switch (opt) {
			case 'p' : 
				snprintf(service,BUFSIZE,"%s",optarg);
				port = atoi(service);
				break;
			default: 
				aff_usage(); 
				exit(EXIT_FAILURE);
		}
	}

	if(opt == -1 && port==-1){
		aff_usage(); 
		exit(EXIT_FAILURE);
	}

}
