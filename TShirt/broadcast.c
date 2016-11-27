#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>

#include "serial.h"
#include "ethernet.h"
#include "analog.h"
//#include "socket.h"

#define MAC_SIZE	6
#define IPV4_SIZE	4
#define DELAY		1000
#define PACKET_SIZE 33

uint8_t data[5];
uint8_t udp[13];
uint8_t ip[33];
int acked=0;
unsigned int flashFreq;
ISR(USART_RX_vect){
	unsigned int frq;
	uint8_t recvd[PACKET_SIZE];
	recv_packet(recvd, PACKET_SIZE);
	if((frq=read_packet(recvd))>0){
		flashFreq=frq;
		acked=1;
	}

}

int main(void){
	DDRB=0x01; //init LED in portB
	initParityTable();
	init_printf();
	sei();
	int mouvement=0;
	int team=0x40;
	int id=0x00;
	int adX=ad_read('x');
	int adY=ad_read('y');
	int adZ=ad_read('z');
	int adYprev=adY;
	int adXprev=adX;
	int adZprev=adZ;
	int temp;
	bool chute=false;
	int temp_read;
	
	
	while(1){

		_delay_ms(DELAY);
		
		adXprev=adX;	    
		adYprev=adY;
		adZprev=adZ;
		adX=ad_read('x');
		adY=ad_read('y');
	 	adZ=ad_read('z');
	 	temp_read=ad_read('t');
	 	temp=temp_read*1.176 - 50; //convert temperature ::: (temp_read*(3/255) - 0.5)*100
	 	chute=detect_chute(adY, adYprev) | detect_chute(adX, adXprev) | detect_chute(adZ, adZprev);
		


		if(chute){
			id=team|0x0F;
			data_packet(id,0xFF,0xFF,0xFF,0xFF,data);
			udp_packet(udp,data);
		  	ip_packet(ip,udp);
		  	while(acked==0){
					
					_delay_ms(DELAY);
					send_packet(ip,(33*sizeof(uint8_t)));				
					//acked=read_packet();
			}
			acked=0;
			adXprev=adX;	    
			adYprev=adY;
			adZprev=adZ;
			adX=ad_read('x');
			adY=ad_read('y');
			adZ=ad_read('z');
			mouvement=detect_chute(adX, adXprev) | detect_chute(adY, adYprev) | detect_chute(adZ, adZprev);
			while(!mouvement){
				PORTB^=0x01;
				mydelay(flashFreq);//mydelay function				
				adXprev=adX;	    
				adYprev=adY;
				adZprev=adZ;
				adX=ad_read('x');
				adY=ad_read('y');
				adZ=ad_read('z');
				mouvement=detect_chute(adX, adXprev) | detect_chute(adY, adYprev) | detect_chute(adZ, adZprev);				
			}
		}
		else {
			id = team | checkParity(adX,adY,adZ,temp);
			data_packet(id,adX,adY,adZ,temp,data);
	  
	  		udp_packet(udp,data);
	  		ip_packet(ip,udp);
	  		send_packet(ip,(33*sizeof(uint8_t)));
		}
		
		
	
	}
	return 0;
}
