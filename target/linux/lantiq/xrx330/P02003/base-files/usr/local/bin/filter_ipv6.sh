#! /bin/sh
#set -x

echo "run script : filter_ipv6.sh"

if [ "_$1" = "_init" ];then
	echo "ipv6 filter init"
	ip6tables -N USER_FILTER
	ip6tables -A FORWARD -j USER_FILTER
else
	logger -t [IP6TABLES] -p local2.info filter restart
fi

# GARYeh 20120423: clear iptables to avoid save network rule
ip6tables -F USER_FILTER
ip6tables -Z USER_FILTER

#Access Control
ACCOUNT=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select count(*) from IPv6Firewall;"`
if [ "$ACCOUNT" -ne 0 ]; then
	i=1
	while [ $i -le $ACCOUNT ]
	do
		ACSERVICE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select ServiceName from IPv6Firewall where _ROWID_='$i';"`
		ACHOST=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select HostName from IPv6Firewall where _ROWID_='$i';"`
		
		# check is there any ServiceName and Cname for Access Control in Services tables
		SERVICE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select ServiceName from Services where ServiceName='$ACSERVICE';"`
		HOST=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Cname from hostTbl where Cname='$ACHOST';"`
		
		if [ "$SERVICE" != "" -a "$HOST" != "" ]; then
			PFSERVICE_PROTOCOL=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Protocol from Services where ServiceName='$SERVICE';"`
			PORTSTART=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select DestinationPortStart from Services where ServiceName='$SERVICE';"`
			PORTEND=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select DestinationPortEnd from Services where ServiceName='$SERVICE';"`
			
			HOSTIP=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Ipv6Addr from hostTbl where Cname='$HOST';"`
			
			ACSTATUS=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Status from IPv6Firewall where _ROWID_='$i';"`
			
			if [ "$ACSTATUS" = "0" ]; then
				ip6tables -A USER_FILTER -s $HOSTIP/128 -p $PFSERVICE_PROTOCOL --dport $PORTSTART:$PORTEND -j ACCEPT
			else
				ip6tables -A USER_FILTER -s $HOSTIP/128 -p $PFSERVICE_PROTOCOL --dport $PORTSTART:$PORTEND -j DROPLOG
			fi
		else	
			echo "No service and host to create IPv6 Access Control rule!!!"
		fi
		i=`expr $i + 1`
	done
fi
#end Access Control

