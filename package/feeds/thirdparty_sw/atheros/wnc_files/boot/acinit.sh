#!/bin/sh /etc/rc.common

START=68

start() 
{
# In bridge mode, do not init wifi
	NATENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Enable from FirewallNatConfig;"`
	if [ "$NATENABLE" = "5" ]; then 
		return
	fi	
#temp sol : put the wifi AC intial script here
    if [ ! "$MTLK_INIT_PLATFORM" ]; then
        . /etc/rc.d/mtlk_init_platform.sh
        echo "no platform found"
    fi
    /etc/rc.d/rc.bringup_wlan initac 
    /etc/rc.d/rc.bringup_wlan start 2 4

    [ -e /usr/local/bin/wifiACInit ]  && /usr/local/bin/wifiACInit
}



