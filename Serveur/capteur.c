#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <time.h>
#include "capteur.h"
#include "libthrd.h"
#include "libcom.h"
#include "serveur.h"

char *lilypadPort="54321";
valCapteur valueList[SIZE];
static pthread_once_t do_once = PTHREAD_ONCE_INIT;
unsigned char parityTab[256];

unsigned char evenParity(unsigned char ch){//returns false:0 if odd, true:1 if even
	int i;
	for(i=4;i>0;i>>=1) ch ^= (ch >> i);
	return ~ch&0x01;
}

void initParityTable(){
	unsigned int i;
	for(i=0; i<256; i++)
		parityTab[i]=evenParity((unsigned char)i);
		
}

unsigned char checkParity(unsigned char x, unsigned char y, unsigned char z, unsigned char t){
	return (((parityTab[x]<<3) | (parityTab[y]<<2) | (parityTab[z]<<1) | (parityTab[t])) & 0x0F);
}


void writeToFile(unsigned char capteurID){
	char buffer[3];
	char fileName[25]={"tsv_files/capteur"};
	snprintf(buffer, sizeof(buffer), "%02d", capteurID);
	strcat(fileName, buffer);
	strcat(fileName, ".tsv");
	time_t t = time(NULL);
	struct tm mytm = *localtime(&t);

	P(capteurID+3);
	FILE *fp=fopen(fileName, "r+");
	if(fp==NULL){ 
		fp=fopen(fileName, "w");
		if(fp==NULL){ perror("fopen"); exit(EXIT_FAILURE); }
		fprintf(fp, "date\tx\ty\tz\tt\n");
		P(2);
		FILE *fp2=fopen("tsv_files/allcapteurs.bin", "ab+");
		fprintf(fp2, "capteur%s\n", buffer);
		fclose(fp2);
		V(2);
		
	}
	fseek(fp, 0, SEEK_END);
	fprintf(fp, "%d-%02d-%02dT%02d:%02d:%02d\t", mytm.tm_year + 1900, mytm.tm_mon + 1, mytm.tm_mday, mytm.tm_hour, mytm.tm_min, mytm.tm_sec);
	//fprintf(fp, "%s%c;%c;%c;%c\n", timenow, valueList[capteurID].x, valueList[capteurID].y,valueList[capteurID].z, valueList[capteurID].t);
	fprintf(fp, "%d\t%d\t%d\t%d\n", valueList[capteurID].x, valueList[capteurID].y,valueList[capteurID].z, valueList[capteurID].t);//test
	fclose(fp);
	V(capteurID+3);
}

void saveInList(unsigned char capteurID, unsigned char x, unsigned char y, unsigned char z, unsigned char t){
	P(1);//mutex[0] for the nbThread variable access,  mutex[1] for list access and others(>1) for the files' access
	valueList[capteurID].x=x;
	valueList[capteurID].y=y;
	valueList[capteurID].z=z;
	valueList[capteurID].t=t;
	V(1);
}

int temp_convert(unsigned char temp){
	float voltage=(((float)temp*0.0196)-0.5)*100;
	//actual_temp=(((((float)actual_temp)*(0.01196))-0.5)*100);
	//return actual_temp;
	return (int)voltage;
}

int setValue(unsigned char *ch, unsigned char *capteurID, unsigned char *x, unsigned char *y, unsigned char *z, unsigned char *t){
	unsigned char val;
	int i=1;
	if((val=checkParity(ch[1], ch[2], ch[3], ch[4]))==(ch[0]&0x0F)){ 
		if(ch[2]==0 && ch[3]==0 && (ch[0]==0 || ch[4]==0)) {
			printf("0x%02x : Not for me !\n", ch[0]);
			return 2;
		}//packet from other server to arduino ::: not our concern
		printf("Right parity ! 0x%02x. ", ch[0]); 
		*capteurID=((ch[0]>>4)&0x0F);
		while( i<5 && ch[i]!=0xFF)//0xFF === emergency value from sensor
			i++;
		if(i!=5){printf("Chute !\n"); return 1;} //emergency ==>true
		printf("\n");
		
		*x=ch[1];
		*y=ch[2];
		*z=ch[3];		
		*t=ch[4];
		return 0;
	}

#ifdef DEBUG
	printf("Wrong parity ! Expected 0x%02x instead of 0x%02x\n", val| (ch[0]&0xF0)  , ch[0]); 
#endif

	return -1;//if parity error
}

void getValue(unsigned char capteurID, unsigned char *x, unsigned char *y, unsigned char *z, unsigned char *t){
	P(1);
	*x=valueList[capteurID].x;
	*y=valueList[capteurID].y;
	*z=valueList[capteurID].z;
	*t=valueList[capteurID].t;
	V(1);	
}

void processUDPMessage(void *arg){
	int ret,i ;
	unsigned char x, y, z, t, nbcapteur, message[5];
	unsigned char *chaine=((unsigned char *) arg);
	pthread_once(&do_once, initParityTable);//initialised only once	
	if((ret=setValue(chaine, &nbcapteur, &x, &y, &z, &t))==0){
		
		saveInList(nbcapteur, x, y, z, t);		
		writeToFile(nbcapteur);//write value to file
	}
	else if(ret==1){
		for(i=1; i<4; i++) message[i]=0x00;
		message[4]=0x32; //50cs
		message[0]=((nbcapteur<<4)&0xF0) | checkParity(0,0,0,message[4]) ;		
		
		envoiMessage(lilypadPort,message, 5); //sendACK to source
		//envoiMessageUnicast("172.26.79.208", "9001", message, 5);
	}
}

void gestionMessage(unsigned char *msg, int taille){
	lanceThread((void *)processUDPMessage, msg, taille);
}
