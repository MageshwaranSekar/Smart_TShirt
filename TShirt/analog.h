/** Definitions for the analog converter **/
#include <stdbool.h>

void ad_init(unsigned char channel);
unsigned int ad_sample(void);
unsigned int ad_read(char channel);
bool detect_chute(int adn, int adnprev);
void mydelay(uint8_t centiseconds);
