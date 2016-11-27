void TCPServeurThread(void *arg);
void setHTTPRespondHeader(char *getch, int type);
void decodeHTTPRequest(char *path, FILE *fd);
void respondHTTPRequest(char *path, FILE *dialogue);
void processHTTPRequest(void *arg);
void gestionThreadTCP(int fd);
