/** fichier libcom.c **/

/*****************************************/
/** Ce fichier contient des fonctions  **/
/**  concernant les sockets.           **/
/****************************************/

/**** Fichiers d'inclusion ****/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "libcom.h"

/**** Constantes ****/

#define MAX_TAMPON		1024
#define MAX_CLIENT 		20
#define MAX_UDP_MESSAGE 	100
#define MAX_CONNEXIONS		30

/**** Variables globales *****/
unsigned char ServerLoop=1;
int UDPsock;
char *broadcast_addr="172.26.79.255";
/**** Prototype des fonctions locales *****/

#ifdef DEBUG
static void afficheAdresse(FILE *flux,void *ip,int type);
void afficheAdresseSocket(FILE *flux,struct sockaddr_storage *padresse);
void afficheHote(FILE *flux,struct hostent *hote,int type);


/**** Fonctions de gestion des sockets ****/

/** Impression d'une adresse generale **/

static void afficheAdresse(FILE *flux,void *ip,int type){
	char adresse[MAX_TAMPON];
	inet_ntop(type,ip,adresse,MAX_TAMPON);
	fprintf(flux,adresse);
}

/** Impression d'une adresse de socket **/

void afficheAdresseSocket(FILE *flux,struct sockaddr_storage *padresse){
	void *ip;
	int port;
	if(padresse->ss_family==AF_INET){
		struct sockaddr_in *ipv4=(struct sockaddr_in *)padresse;
		ip=(void *)&ipv4->sin_addr;
		port=ipv4->sin_port;
	}
	if(padresse->ss_family==AF_INET6){
		struct sockaddr_in6 *ipv6=(struct sockaddr_in6 *)padresse;
		ip=(void *)&ipv6->sin6_addr;
		port=ipv6->sin6_port;
	}
	fprintf(flux,"Adresse IP%s : ",padresse->ss_family==AF_INET?"v4":"v6");
	afficheAdresse(flux,ip,padresse->ss_family);
	fprintf(flux,"\nPort de la socket : %d.\n",ntohs(port));
}

/** Impression des informations d'un hote **/

void afficheHote(FILE *flux,struct hostent *hote,int type){
	char **params;

	fprintf(flux,"Nom officiel : '%s'.\n",hote->h_name);
	fprintf(flux,"Surnoms: ");
	for(params=hote->h_aliases;*params!=NULL;params++){
		fprintf(flux,"%s",*params);
		if(*(params+1)==NULL) fprintf(flux,",");
	}
	fprintf(flux,"\n");
	fprintf(flux,"Type des adresses   : %d.\n",hote->h_addrtype);
	fprintf(flux,"Taille des adresses : %d.\n",hote->h_length);
	fprintf(flux,"Adresses: ");
	for(params=hote->h_addr_list;params[0]!=NULL;params++){
		afficheAdresse(flux,(struct in_addr *)params,type);
		if((*params+1)!=NULL) fprintf(flux,",");
	}
	fprintf(flux,"\n");
}
#endif
void socketToClient(int s,char **hote,char **service){
	struct sockaddr_storage adresse;
	socklen_t taille=sizeof adresse;
	int statut;

	/* Recupere l'adresse de la socket distante */
	statut=getpeername(s,(struct sockaddr *)&adresse,&taille);
	if(statut<0){ perror("socketToClient.getpeername"); exit(EXIT_FAILURE); }

	/* Recupere le nom de la machine */
	*hote=malloc(MAX_TAMPON);
	if(*hote==NULL){ perror("socketToClient.malloc"); exit(EXIT_FAILURE); }
	*service=malloc(MAX_TAMPON);
	if(*service==NULL){ perror("socketToClient.malloc"); exit(EXIT_FAILURE); }
	getnameinfo((struct sockaddr *)&adresse,sizeof adresse,*hote,MAX_TAMPON,*service,MAX_TAMPON,0);
}

void stopServerLoop(){
	ServerLoop=0;
}


int initialisationSocketUDP(char *service){
	struct addrinfo precisions,*resultat;
	int statut;
	int s;

	/* Construction de la structure adresse */
	memset(&precisions,0,sizeof precisions);
	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;
	precisions.ai_flags=AI_PASSIVE;
	statut=getaddrinfo(NULL,service,&precisions,&resultat);
	if(statut<0){ perror("initialisationSocketUDP.getaddrinfo"); exit(EXIT_FAILURE); }

	/* Creation d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

	/* Options utiles */
	int vrai=1;
	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
		perror("initialisationServeurUDP.setsockopt (REUSEADDR)");
		exit(-1);
	}
	
	if(setsockopt(s,SOL_SOCKET,SO_BROADCAST,&vrai,sizeof(vrai))<0){
		perror("initialisationServeurUDP.setsockopt (BROADCAST)");
		exit(-1);
	}

	/* Specification de l'adresse de la socket */
	statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
	if(statut<0) {perror("initialisationServeurUDP.bind"); exit(-1);}

	/* Liberation de la structure d'informations */
	freeaddrinfo(resultat);

	return s;
}


void serveurMessages(char *service, void(*traitement)(unsigned char *, int)){//UDP serveur
	
	UDPsock=initialisationSocketUDP(service);

	printf("(UDP) Listening on port : %s\n", service);

	
	while(ServerLoop){
		struct sockaddr_storage adresse;
		socklen_t taille=sizeof(adresse);
		unsigned char message[MAX_UDP_MESSAGE];
		int nboctets=recvfrom(UDPsock,message,MAX_UDP_MESSAGE,0,(struct sockaddr *)&(adresse),&taille);
		if(nboctets<0) exit(EXIT_FAILURE);			
		traitement(message, nboctets);
	}

	close(UDPsock);
}

int envoiMessage(char *service,unsigned char *message, int packetsize){//UDP broadcast
	struct addrinfo precisions,*resultat;
	int statut;

	/* Creation de l'adresse de socket */
	memset(&precisions,0,sizeof precisions);
	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;
	statut=getaddrinfo(broadcast_addr,service,&precisions,&resultat);
	if(statut<0){ perror("envoiMessage.getaddrinfo"); exit(EXIT_FAILURE); }

	/* Envoi du message */
	int nboctets=sendto(UDPsock,message,packetsize,0,resultat->ai_addr,resultat->ai_addrlen);
	if(nboctets<0){ perror("envoiMessage.sendto"); exit(EXIT_FAILURE); }

	/* Liberation de la structure d'informations */
	freeaddrinfo(resultat);

	return 0;
}

int envoiMessageUnicast(char *hote, char *service,unsigned char *message, int packetsize){//UDP unicast
	struct addrinfo precisions,*resultat;	
	int statut;
	
	/* Creation de l'adresse de socket */
	memset(&precisions,0,sizeof precisions);
	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;
	statut=getaddrinfo(hote,service,&precisions,&resultat);
	if(statut<0){ perror("envoiMessageUnicast.getaddrinfo"); exit(EXIT_FAILURE); }

	/* Envoi du message */
	int nboctets=sendto(UDPsock,message,packetsize,0,resultat->ai_addr,resultat->ai_addrlen);
	if(nboctets<0){ perror("envoiMessageUnicast.sendto"); exit(EXIT_FAILURE); }

	/* Liberation de la structure d'informations */
	freeaddrinfo(resultat);

	return 0;
}

/*---------------------------------------------TCP-------------------------------------------------------------*/
int initialisationServeur(char *service){
	struct addrinfo precisions,*resultat;
	int statut;
	int s;

	/* Construction de la structure adresse */
	memset(&precisions,0,sizeof precisions);
	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_STREAM;
	precisions.ai_flags=AI_PASSIVE;
	statut=getaddrinfo(NULL,service,&precisions,&resultat);
	if(statut<0){ perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }

	/* Creation d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }

	/* Options utiles */
	int vrai=1;
	if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
	  perror("initialisationServeur.setsockopt (REUSEADDR)");
	  exit(EXIT_FAILURE);
	  }
	if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
	  perror("initialisationServeur.setsockopt (NODELAY)");
	  exit(EXIT_FAILURE);
	  }

	/* Specification de l'adresse de la socket */
	statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
	if(statut<0) return -1;

	/* Liberation de la structure d'informations */
	freeaddrinfo(resultat);

	/* Taille de la queue d'attente */
	statut=listen(s,MAX_CONNEXIONS);
	if(statut<0) return -1;


	printf("(TCP) Listening on port : %s\n", service);
	return s;
}

int boucleServeur(int ecoute,void (*traitement)(int)){
	int dialogue;
	while(ServerLoop){
		/* Attente d'une connexion */
		if((dialogue=accept(ecoute,NULL,NULL))<0){ perror("accept");exit(EXIT_FAILURE);}

		/* Passage de la socket de dialogue a la fonction de traitement */
		traitement(dialogue);
	}

	close(ecoute);

	return 0;
}
