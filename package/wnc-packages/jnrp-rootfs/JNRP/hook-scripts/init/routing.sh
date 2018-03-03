#!/bin/sh
#static routing

killall -9 zebra
killall -9 ripd

ZEBRA_CONFIG_FILE=/tmp/zebra.conf
RIPD_CONFIG_FILE=/tmp/ripd.conf

rm -f $ZEBRA_CONFIG_FILE
rm -f $RIPD_CONFIG_FILE

NAME=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select name from system;"`
SRENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select StaticRoutingEnable from RoutingConfig;"`
DRENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select DynamicRoutingEnable from RoutingConfig;"`

if [ "$SRENABLE" = "0" ]; then
	if [ "$DRENABLE" = "0" ]; then
		return 0
	fi
fi

# create zebra.conf
echo "hostname $NAME" >> $ZEBRA_CONFIG_FILE
echo "password zebra" >> $ZEBRA_CONFIG_FILE
echo "enable password zebra" >> $ZEBRA_CONFIG_FILE
echo "interface br0" >> $ZEBRA_CONFIG_FILE
echo "interface usb0" >> $ZEBRA_CONFIG_FILE
echo "log syslog" >> $ZEBRA_CONFIG_FILE

if [ "$SRENABLE" = "1" ]; then
	SRCOUNT=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select count(*) from StaticRoute;"`
	if [ "$SRCOUNT" -ne 0 ]; then
		i=1
		while [ $i -le $SRCOUNT ]
		do
			SREN=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Enable from StaticRoute where _ROWID_='$i';"`
			if [ "$SREN" = "1" ]; then
				DIP=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select DestineNetwork from StaticRoute where _ROWID_='$i';"`
				DMASK=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select DestineNetmask from StaticRoute where _ROWID_='$i';"`
				#DEV=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select device from StaticRoute where _ROWID_='$i';"`
				GW=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select LocalGateway from StaticRoute where _ROWID_='$i';"`
				
				if [ -z $GW ];then
					gw=""
				else
					gw="$GW "
					echo "ip route $DIP $DMASK $gw" >> $ZEBRA_CONFIG_FILE
					i=`expr $i + 1`
					continue 
				fi
				
				#if [ $DEV = "LAN" ]; then
				#	echo "ip route $DIP $DMASK dev br0 $gw" >> $ZEBRA_CONFIG_FILE
				#fi
				#if [ $DEV = "APN1" ]; then
				#	echo "ip route $DIP $DMASK dev usb0 $gw" >> $ZEBRA_CONFIG_FILE 
				#fi
				#if [ $DEV = "APN2" ]; then
				#	echo "ip route $DIP $DMASK dev usb1 $gw" >> $ZEBRA_CONFIG_FILE 
				#fi
			fi
				i=`expr $i + 1`
		done
	fi
fi

zebra -f $ZEBRA_CONFIG_FILE -d -i /tmp/zebra.pid


if [ "$DRENABLE" != "1" ]; then
	# create ripd.conf
	echo "hostname $HOSTNAME" >> $RIPD_CONFIG_FILE
	echo "password ripd" >> $RIPD_CONFIG_FILE
	echo "interface br0" >> $RIPD_CONFIG_FILE
	echo "interface usb0" >> $RIPD_CONFIG_FILE	
	echo "router rip" >> $RIPD_CONFIG_FILE
	echo " network br0" >> $RIPD_CONFIG_FILE
	echo " network usb0" >> $RIPD_CONFIG_FILE

	if [ $DRENABLE = "2" ];then
		echo " version 1" >> $RIPD_CONFIG_FILE
	fi
	if [ $DRENABLE = "2" ];then
		echo " version 2" >> $RIPD_CONFIG_FILE
	fi
	#echo "interface $EXTIF" >> $RIPD_CONFIG_FILE
	#echo " no ip rip authentication mode" >> $RIPD_CONFIG_FILE
	echo "log syslog" >> $RIPD_CONFIG_FILE
	
	ripd -f $RIPD_CONFIG_FILE -d -i /tmp/ripd.pid
fi
