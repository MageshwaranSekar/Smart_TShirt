#!/bin/sh
ip route del 172.26.79.209/32 dev sl0
ifconfig sl0 down
killall socat
killall slattach 
