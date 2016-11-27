#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "http.h"
#include "libthrd.h"
#include "capteur.h"
#include "serveur.h"
#include "libcom.h" 
#define JS 0
#define CSS 1
#define HTML 2
#define TSV 3
#define PLAIN 4

char *ok_response = "HTTP/1.1 200 OK\r\nDate :";
char *ok_response_partial="\r\nContent-type: ";
char *ok_response_end="\r\nServer: C Web server\r\n\r\n";


void TCPServeurThread(void *arg){
	char *service=(char *) arg;
	int s=initialisationServeur(service);
	boucleServeur(s,gestionThreadTCP);
}


void setHTTPRespondHeader(char *getch, int type){
	char ch[50];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(ch, sizeof(ch), "%a, %d %b %Y %H:%M:%S %Z", &tm);
#ifdef DEBUG
	printf("%s\n", ch);
#endif
	strcpy(getch, ok_response);
	strcat(getch, ch);
	strcat(getch, ok_response_partial);
	
	switch(type){
		case 0:
			strcat(getch, "application/javascript");
			break;
		case 1:
			strcat(getch, "text/css");
			break;
		case 2:
			strcat(getch, "text/html");
			break;
		case 3:
			strcat(getch, "text/tab-separated-values");
			break;

		case 4:
			strcat(getch, "text/plain");
			break;
	}

	strcat(getch, ok_response_end);
}
void decodeHTTPRequest(char *path, FILE *fd){
	char protocol[15], method[10], ligne[MAX_LIGNE];
	if((fgets(ligne,MAX_LIGNE,fd)!=NULL) && (strcmp(ligne, "\r\n")!= 0)){
		sscanf (ligne, "%s %s %s", method, path, protocol);
		do{	
#ifdef DEBUG
			printf("> %s",ligne);
#endif
		}while((fgets(ligne,MAX_LIGNE,fd)!=NULL) && (strcmp(ligne, "\r\n")!= 0) );
	}

}


void respondHTTPRequest(char *path, FILE *dialogue){
	FILE *fp;
	char line[3000];
	int i;	
	unsigned char groupNum, x=0, y=0, z=0, t=0;
	char ch[200], groupe[3]={"\0\0\0"};
	
	

	char *end=path+strlen(path);


	if((strcmp(path, "/valeurs.html")==0) || (strcmp(path, "/")==0)){
		if((fp = fopen("../webroot/valeurs.html", "r"))!=NULL){
			setHTTPRespondHeader(ch, HTML);
			fprintf(dialogue , "%s", ch);
			while(fgets(line,300,fp)!=NULL)
				fprintf(dialogue , "%s", line);
			fclose(fp);
		}
	}
	
	else if(strncmp(path, "/valeurs.html", 13*sizeof(char))==0){
		if(strncmp(path+13, "?value=instant&group=", 21*sizeof(char))==0){
			setHTTPRespondHeader(ch, PLAIN);
			for(i=34; i<strlen(path)&&i<36;i++)
				groupe[i-34]=path[i];
			
			groupNum=atoi(groupe);
			if(groupNum<11){ //total groups of 11 : 0 - 10
				getValue(groupNum, &x, &y, &z, &t);
				fprintf(dialogue , "%s", ch);				
				fprintf(dialogue , "<canvas id=\"display\"><script type=\"text/javascript\">disp(\"X:%03u\", display)</script></canvas>\n", x);
				fprintf(dialogue , "<br/><canvas id=\"display2\" ><script type=\"text/javascript\">disp(\"Y:%03u\", display2)</script></canvas>\n",y);
				fprintf(dialogue , "<br/><canvas id=\"display3\"><script type=\"text/javascript\">disp(\"Z:%03u\", display3)</script></canvas>\n",z);
				fprintf(dialogue , "<br/><canvas id=\"display4\" ><script type=\"text/javascript\">disp(\"T:%02u C\", display4)</script></canvas>\n", t);
				
			}
		}
	}

	else if(strcmp(path, "/graphes.html")==0){
		if((fp = fopen("../webroot/graphes.html", "r"))!=NULL){
			setHTTPRespondHeader(ch, HTML);
			fprintf(dialogue , "%s", ch);
			while(fgets(line,300,fp)!=NULL)
				fprintf(dialogue , "%s", line);
			fclose(fp);
		}
	}

	

	

	else if(strcmp(end-4, ".tsv")==0){
		int found=0;
		char tsvdir[15]={"tsv_files"};
		for(i=8; i<strlen(path)&&i<10;i++)
				groupe[i-8]=path[i];
		groupNum=atoi(groupe);
		strcat(tsvdir, path);
		P(groupNum+3);
		if((fp = fopen(tsvdir, "r"))!=NULL){
			setHTTPRespondHeader(ch, TSV);
			fprintf(dialogue , "%s", ch);
			while(fgets(line,3000,fp)!=NULL)
				fprintf(dialogue , "%s", line);
			fclose(fp);
			found=1;
		}
		V(groupNum+3);

		if(found==0){//if the file is not found or not yet created
			setHTTPRespondHeader(ch, TSV);
			fprintf(dialogue , "%s", ch);
			fprintf(dialogue , "date\tx\ty\tz\tt"); //send only the field => no data
		}
	}

	else if(strcmp(end-4, ".css")==0){
		setHTTPRespondHeader(ch,CSS);
		if(strcmp(path, "/css/mainstyle.css")==0){
			if((fp = fopen("../webroot/css/mainstyle.css", "r"))!=NULL){
				
				fprintf(dialogue , "%s", ch);
				while(fgets(line,3000,fp)!=NULL)
					fprintf(dialogue , "%s", line);
				fclose(fp);
			}
		}
	}

	else if(strcmp(end-3, ".js")==0){
		setHTTPRespondHeader(ch, JS);
		if(strcmp(path, "/js/page.js")==0){
			if((fp = fopen("../webroot/js/page.js", "r"))!=NULL){
				fprintf(dialogue , "%s", ch);
				while(fgets(line,3000,fp)!=NULL)
					fprintf(dialogue , "%s", line);
				fclose(fp);
			}
		}

		else if(strcmp(path, "/js/segmentdisplay.js")==0){
			if((fp = fopen("../webroot/js/segmentdisplay.js", "r"))!=NULL){
				fprintf(dialogue , "%s", ch);
				while(fgets(line,3000,fp)!=NULL)
					fprintf(dialogue , "%s", line);
				fclose(fp);
			}
		}

		else if(strcmp(path, "/js/svg.js")==0){
			if((fp = fopen("../webroot/js/svg.js", "r"))!=NULL){
				fprintf(dialogue , "%s", ch);
				while(fgets(line,3000,fp)!=NULL)
					fprintf(dialogue , "%s", line);
				fclose(fp);
			}
		}
	}

	else{
		if((fp = fopen("../webroot/redirect.html", "r"))!=NULL){
			setHTTPRespondHeader(ch, HTML);
			fprintf(dialogue , "%s", ch);
			while(fgets(line,300,fp)!=NULL)
				fprintf(dialogue , "%s", line);
			fclose(fp);
		}

	}	
}

void processHTTPRequest(void *arg){	
	int s=*(int *)arg;
	char path[MAX_LIGNE];
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("processHTTPRequest.fdopen"); exit(EXIT_FAILURE); }	

	decodeHTTPRequest(path, dialogue);
	respondHTTPRequest(path, dialogue);
	
	fclose(dialogue);
}


void gestionThreadTCP(int fd){
	lanceThread((void *)processHTTPRequest, (void *) &fd, sizeof(int));
}

