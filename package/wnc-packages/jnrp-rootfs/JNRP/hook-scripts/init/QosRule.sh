#! /bin/sh
# WLD6_QoS
# QosRule
# QOS_NAME : QoS rule name
# QOS_PRIORITY : 1, 2, 3, 4
# QOS_RULE_INDEX : 1~32
# QOS_FLOW : uplink, dlink
# QOS_UP_RULE : ip, ssid, mac, dscp
# UP_RULE_IP : 192.168.1.20
# UP_RULE_MAC : XX:XX:XX:XX:XX:XX
# UP_RULE_SSID : 0, 1, 2, 3
# UP_RULE_DSCP : 0~63
# QOS_DOWN_RULE : ip, ssid, mac, dscp
# DOWN_RULE_IP : 192.168.1.20
# DOWN_RULE_SSID : 0, 1, 2, 3
# DOWN_RULE_DSCP : 0~63


###Test Only : start###
#QOS_DOWN_RULE=ip
#DOWN_RULE_IP=192.168.2.2
#QOS_PRIORITY=1
#QOS_RULE_INDEX=5
#QOS_FLOW=dlink
#QOS_NAME=QoS_Highest_Level
###Test Only : end###

CONFIG_FILE=/mnt/data/QoS/QoS_Rule

if [ ! -e $CONFIG_FILE ]
then
        i=1
        while [ $i -le 32 ]
        do
        echo "#" >> $CONFIG_FILE
        i=$(($i+1))
        done
        echo "" >> $CONFIG_FILE
fi


# sqlite3 -cmd ".timeout 1000" /tmp/system.db "update QosRule set Enable='1', Name='$QOS_NAME', Priority='$QOS_PRIORITY', Rule='$QOS_RULE', Flow='$QOS_FLOW', Content='$QOS_CONTENT' where _ROWID_='$QOS_RULE_INDEX'";
# read config info from DB
QOS_NAME=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Name from QosRule where _ROWID_='$1'";`
QOS_FLOW=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Flow from QosRule where _ROWID_='$1'";`
QOS_RULE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Rule from QosRule where _ROWID_='$1'";`
QOS_PRIORITY=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Priority from QosRule where _ROWID_='$1'";`
QOS_CONTENT=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Content from QosRule where _ROWID_='$1'";`


# uplink cmd
mangle_ip_ul="iptables -t mangle -A QoS_UL -s"
mangle_ssid_ul="iptables -t mangle -A QoS_UL -m physdev --physdev-in ath"
mangle_mac_ul="iptables -t mangle -A QoS_UL -m mac --mac-source"
mangle_dscp_ul="iptables -t mangle -A QoS_UL -m dscp --dscp"

# downlink cmd
mangle_ip_dl="iptables -t mangle -A QoS_DL -d"
mangle_ssid_dl="iptables -t mangle -A QoS_DL -m physdev --physdev-out ath"
mangle_dscp_dl="iptables -t mangle -A QoS_DL -m dscp --dscp"

# iptables mark parameter
mangle_mark_para="-j MARK --set-mark"

# iptables comment parameter
mangle_cmt_para="-m comment --comment"

# iptables comment content
mangle_cmt_cnt="QoS~$1~~$QOS_NAME"

mangle_index=$(($1))"c"

# replace column
if [ $QOS_FLOW = "uplink" ];then

	echo "Configure UpLink Rule"
	
	if [ $QOS_RULE = "ip" ];then	
		# filter : IP
		COMMAND_UL="$mangle_ip_ul $QOS_CONTENT $mangle_mark_para 2$QOS_PRIORITY $mangle_cmt_para $mangle_cmt_cnt"
	else
#		if [ $QOS_RULE = "ssid" ];then		
			# filter : SSID
#			COMMAND_UL="$mangle_ssid_ul$QOS_CONTENT $mangle_mark_para 2$QOS_PRIORITY $mangle_cmt_para $mangle_cmt_cnt"
#		else
			if [ $QOS_RULE = "mac" ];then
				# filter : MAC
				COMMAND_UL="$mangle_mac_ul $QOS_CONTENT $mangle_mark_para 2$QOS_PRIORITY $mangle_cmt_para $mangle_cmt_cnt"
			else
				if [ $QOS_RULE = "dscp" ];then
					# filter : DSCP
					COMMAND_UL="$mangle_dscp_ul $QOS_CONTENT $mangle_mark_para 2$QOS_PRIORITY $mangle_cmt_para $mangle_cmt_cnt"
				fi
			fi
#		fi
	fi
	iptables -t mangle -F QoS_UL
	iptables -t mangle -F QoS_DL
	sed -i "$mangle_index $COMMAND_UL" $CONFIG_FILE	
else

	echo "Configure DownLink Rule"
	
	if [ $QOS_RULE = "ip" ];then
		# filter : IP
		COMMAND_DL="$mangle_ip_dl $QOS_CONTENT $mangle_mark_para 1$QOS_PRIORITY $mangle_cmt_para $mangle_cmt_cnt"
	else
#		if [ $QOS_RULE = "ssid" ];then
			# filter : SSID
#			COMMAND_DL="$mangle_ssid_dl$QOS_CONTENT $mangle_mark_para 1$QOS_PRIORITY $mangle_cmt_para $mangle_cmt_cnt"
#		else
			if [ $QOS_RULE = "dscp" ];then
				# filter : DSCP
				COMMAND_DL="$mangle_dscp_dl $QOS_CONTENT $mangle_mark_para 1$QOS_PRIORITY $mangle_cmt_para $mangle_cmt_cnt"
			fi
#		fi
	fi
	iptables -t mangle -F QoS_UL
	iptables -t mangle -F QoS_DL
	sed -i "$mangle_index $COMMAND_DL" $CONFIG_FILE
fi

# Write to DB : Start
# QOS_NAME : user define 
# QOS_PRIORITY : 1~5
# QOS_RULE : ip, mac, dscp, ssid(not supported yet)
# QOS_FLOW : uplink, downlink
# QOS_RULE_INDEX : 1~32
# sqlite3 -cmd ".timeout 1000" /tmp/system.db "update QosRule set Enable='1', Name='$QOS_NAME', Priority='$QOS_PRIORITY', Rule='$QOS_RULE', Flow='$QOS_FLOW' where _ROWID_='$QOS_RULE_INDEX'";
# sqlite3 -cmd ".timeout 1000" /tmp/system.db "update QosRule set Enable='1', Name='$QOS_NAME', Priority='$QOS_PRIORITY', Rule='$QOS_RULE', Flow='$QOS_FLOW', Content='$QOS_CONTENT' where _ROWID_='$QOS_RULE_INDEX'";
# Write to DB : End

chmod +x $CONFIG_FILE
$CONFIG_FILE

# Woody : NO need to redirect to WLD6 QoS web page
# ./QoSPolicy
