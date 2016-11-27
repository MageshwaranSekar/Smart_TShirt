void writeToFile(unsigned char capteurID);
void saveInList(unsigned char capteurID, unsigned char x, unsigned char y, unsigned char z, unsigned char t);
int setValue(unsigned char *ch, unsigned char *capteurID, unsigned char *x, unsigned char *y, unsigned char *z, unsigned char *t);
void getValue(unsigned char capteurID, unsigned char *x, unsigned char *y, unsigned char *z, unsigned char *t);
void processUDPMessage(void *arg);
void gestionMessage(unsigned char *msg, int taille);
