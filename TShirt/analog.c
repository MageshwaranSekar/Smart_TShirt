#include <avr/io.h>		// for the input/output register
#include <util/delay.h>
#include <stdbool.h>
#include "analog.h"

/** Functions for the analog converter **/

void ad_init(unsigned char channel){
	ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);   
	ADMUX|=(1<<REFS0)|(1<<ADLAR);
	ADMUX=(ADMUX&0xf0)|channel;   
	ADCSRA|=(1<<ADEN);
}   

unsigned int ad_sample(void){
	ADCSRA|=(1<<ADSC);
	while(bit_is_set(ADCSRA, ADSC));
	return ADCH;
}

unsigned int ad_read(char channel){
	if(channel=='x'){
	    ad_init(0);
	}
  	if(channel=='y'){
    	ad_init(1);
  	}
  	if(channel=='z'){
    	ad_init(2);
  	}
  	if(channel=='t'){
    	ad_init(3);
  	}
  	return ad_sample();
}

bool detect_chute(int adn, int adnprev){
	int oldmanisdown=adnprev-adn;
	if (oldmanisdown>80|| oldmanisdown<-80) return true;
	else return false;
}

void mydelay(uint8_t centiseconds){
	while (centiseconds>0){
		_delay_ms(10);
		centiseconds--;
	}

}
