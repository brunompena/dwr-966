#!/bin/sh

NATENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Enable from FirewallNatConfig;"`

if [ "$NATENABLE" != "5" ]; then 
	/usr/local/bin/ipv6_service.sh start &
fi