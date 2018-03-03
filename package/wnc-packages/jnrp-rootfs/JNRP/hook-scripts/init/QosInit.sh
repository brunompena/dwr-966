#!/bin/sh

init=`iptables -t mangle -L | grep QoS_DL | awk 'NR==1' | awk '{print $1}'`

if [ "_$init" = "_QoS_DL" ]; then
echo "QoS_DL and QoS_UL chain exist, exit..."
exit 0
fi

iptables -t mangle -N QoS_UL
iptables -t mangle -N QoS_DL
iptables -t mangle -A FORWARD -j QoS_UL
iptables -t mangle -A FORWARD -j QoS_DL

test -d /mnt/data/QoS && echo QoS_Folder_Exist || mkdir /mnt/data/QoS

QoS_Enable=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Enable from QoS;"`

if [ $QoS_Enable = "1" ];then
	echo "QoS Enable : disable PPA"
	ppacmd control --disable-lan --disable-wan
	if [ -e /mnt/data/QoS/QoS_Policy ];then
		chmod +x /mnt/data/QoS/QoS_Policy
		/mnt/data/QoS/QoS_Policy
	fi
fi

if [ ! -e /mnt/data/QoS/QoS_Rule ]; then
	i=1
	while [ $i -le 32 ]
	do
	echo "#" >> /mnt/data/QoS/QoS_Rule
	i=$(($i+1))
	done
	echo "" >> /mnt/data/QoS/QoS_Rule
else
	chmod +x /mnt/data/QoS/QoS_Rule
	/mnt/data/QoS/QoS_Rule	
fi
