#include <stdint.h>


uint8_t evenParity(uint8_t ch);
void initParityTable();
uint8_t checkParity(uint8_t x, uint8_t y, uint8_t z, uint8_t t);
void data_packet(unsigned int teamid, unsigned int x, unsigned int y, unsigned int z, unsigned int t, uint8_t packet[5]);
void udp_packet(uint8_t udpp[13], uint8_t packet[5]);
void ip_packet(uint8_t ipp[33], uint8_t udpp[13]);
void send_packet(unsigned char *p, int len);
unsigned int recv_packet(unsigned char *p, int len);
uint8_t read_packet(uint8_t dat[5]);
