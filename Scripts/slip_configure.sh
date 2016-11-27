#!/bin/sh

slattach -L -s 9600 -p slip /dev/ttyUSB0 -d &
sleep 1
ifconfig sl0 172.26.79.208 dstaddr 172.26.79.209 mtu 1500 
#ip address add dev sl0 172.26.79.208 peer 172.26.79.209
#ifconfig sl0 up
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 1 > /proc/sys/net/ipv4/conf/all/proxy_arp
#ip route add 172.26.79.209/32 dev sl0
socat UDP-RECVFROM:12345,fork,so-bindtodevice=sl0 UDP-DATAGRAM:172.26.79.255:12345,sp=54321,reuseaddr,broadcast &
