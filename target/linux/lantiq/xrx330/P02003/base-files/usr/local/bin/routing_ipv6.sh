#!/bin/sh
#static routing
STATICRT_RULE=/tmp/staticrt_rule_ipv6

NAME=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select name from system;"`
SRENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select StaticRoutingEnable from RoutingConfig;"`
EXTIF=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select interfaceName from networkInterface where LogicalIfName = 'LTE-WAN1';"`
LANIF=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select interfaceName from networkInterface where LogicalIfName = 'LAN1';"`

if [ -e $STATICRT_RULE ];then
	# GARYeh 20120420: clear static routing table
	cat $STATICRT_RULE | while read line
	do
		echo $line | grep "route"
		if [ $? == 0 ];then
			delete=`echo $line | sed 's/add/del/g' | sed 's/ gw_index.*$//g'`
			`$delete`
		fi
	done 
fi
rm -f $STATICRT_RULE

if [ "$SRENABLE" = "0" ]; then
	if [ "$DRENABLE" = "0" ]; then
		return 0
	fi
fi

#For static routing
if [ "$SRENABLE" = "1" ]; then
	SRCOUNT=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select count(*) from IPv6StaticRouting;"`
	if [ "$SRCOUNT" -ne 0 ]; then
		i=1
		while [ $i -le $SRCOUNT ]
		do
			#SREN=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Enable from StaticRoute where _ROWID_='$i';"`
			#if [ "$SREN" = "1" ]; then
				DIP=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select DesNetwork from IPv6StaticRouting where _ROWID_='$i';"`
				DPREFIX=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select PreLength from IPv6StaticRouting where _ROWID_='$i';"`
				NETWORK=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select NetworkType from IPv6StaticRouting where _ROWID_='$i';"`
				GW=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Gateway from IPv6StaticRouting where _ROWID_='$i';"`

				add_route="route -A inet6 add "
				
				dest_net="$DIP/$DPREFIX "
				
				if [ $NETWORK = "0" ];then
					interface="dev $LANIF "
				else
					interface="dev $EXTIF "
				fi
				
				if [ -z $GW ];then
					gw=""
				else
					gw="gw $GW "
				fi
				
				routerule=$add_route$dest_net$interface$gw
				
				echo "$routerule" >> $STATICRT_RULE
			#fi
			i=`expr $i + 1`
		done	
	fi
	
	if [ -e $STATICRT_RULE ];then
	# GARYeh 20120420: clear static routing table
		cat $STATICRT_RULE | while read line
		do
			echo $line | grep "route"
			if [ $? == 0 ];then
				`$line`
			fi
		done 
	fi
fi
