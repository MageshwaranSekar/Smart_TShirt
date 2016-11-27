/** fichier libcom.h **/

/******************************************************************/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions de gestion des sockets                     **/
/******************************************************************/

/**** Constantes ****/

/**** Fonctions ****/
/*-----------------------------------UDP-----------------------------------------------*/
int initialisationSocketUDP(char *service);
void serveurMessages(char *service, void(*traitement)(unsigned char *, int));
int envoiMessage(char *service,unsigned char *message, int packetsize);
int envoiMessageUnicast(char *hote, char *service,unsigned char *message, int packetsize);
void socketToClient(int s,char **hote,char **service);
void stopServerLoop();
/*-----------------------------------TCP-----------------------------------------------*/
int initialisationServeur(char *service);
int boucleServeur(int ecoute,void (*traitement)(int));
