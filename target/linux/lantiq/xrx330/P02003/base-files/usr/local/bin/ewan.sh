#! /bin/sh

echo "run script : ewan.sh"

# EWAN_TYPE --0: PPPoE, 1: Dynamic IP, 2: Static IP
EWAN_TYPE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select etype from ethWanSetting;"`

if [ "$EWAN_TYPE" = "0" ]; then
	PPPOE_username=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select username from ethWanSetting;"`
	PPPOE_password=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select password from ethWanSetting;"`
	PPPOE_service_name=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select service_name from ethWanSetting;"`
	PPPOE_ip=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select ip from ethWanSetting;"`
	PPPOE_dns1=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select dns1 from ethWanSetting;"`
	PPPOE_dns2=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select dns2 from ethWanSetting;"`
	PPPOE_mac=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select mac from ethWanSetting;"`

	PPPOE_username="\"username\":\"$PPPOE_username\""
	PPPOE_password=", \"password\":\"$PPPOE_password\""
	if [ -z "$PPPOE_service_name" ]; then
		PPPOE_service_name=""
	else	
		PPPOE_service_name=", \"service_name\":\"$PPPOE_service_name\""
	fi
	if [ -z "$PPPOE_ip" ]; then
		PPPOE_ip=""
	else	
		PPPOE_ip=", \"ip\":\"$PPPOE_ip\""
	fi
	if [ -z "$PPPOE_dns1" ]; then
		PPPOE_dns1=""
	else	
		PPPOE_dns1=", \"dns1\":\"$PPPOE_dns1\""
	fi
	if [ -z "$PPPOE_dns2" ]; then
		PPPOE_dns2=""
	else	
		PPPOE_dns2=", \"dns2\":\"$PPPOE_dns2\""
	fi
	if [ -z "$PPPOE_mac" ]; then
		PPPOE_mac=""
	else	
		PPPOE_mac=", \"mac\":\"$PPPOE_mac\""
	fi

	PPPOE_cmd={$PPPOE_username$PPPOE_password$PPPOE_service_name$PPPOE_ip$PPPOE_dns1$PPPOE_dns2$PPPOE_mac}
	
	JsonClient /tmp/cgi-2-sys set_ethwan_pppoe "$PPPOE_cmd"
elif [ "$EWAN_TYPE" = "1" ]; then
	DHCP_hostname=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select hostname from ethWanSetting;"`
	DHCP_dns1=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select dns1 from ethWanSetting;"`
	DHCP_dns2=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select dns2 from ethWanSetting;"`
	
	if [ -z "$DHCP_hostname" ]; then
		DHCP_hostname=""
	else	
		DHCP_hostname="\"hostname\":\"$DHCP_hostname\""
	fi
	if [ -z "$DHCP_dns1" ]; then
		DHCP_dns1=""
	else	
		DHCP_dns1=", \"dns1\":\"$DHCP_dns1\""
	fi
	if [ -z "$DHCP_dns2" ]; then
		DHCP_dns2=""
	else	
		DHCP_dns2=", \"dns2\":\"$DHCP_dns2\""
	fi
	
	DHCP_cmd={$DHCP_hostname$DHCP_dns1$DHCP_dns2}
	
	JsonClient /tmp/cgi-2-sys set_ethwan_dynamic_ip "$DHCP_cmd"	
elif [ "$EWAN_TYPE" = "2" ]; then
	STATIC_ip=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select ip from ethWanSetting;"`
	STATIC_subnet=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select subnet from ethWanSetting;"`
	STATIC_gw=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select gateway from ethWanSetting;"`
	STATIC_dns1=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select dns1 from ethWanSetting;"`
	STATIC_dns2=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select dns2 from ethWanSetting;"`
	STATIC_mtu=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select mtu from ethWanSetting;"`

	STATIC_ip="\"ip\":\"$STATIC_ip\""
	STATIC_subnet=", \"subnet\":\"$STATIC_subnet\""
	STATIC_gw=", \"gw\":\"$STATIC_gw\""
	STATIC_dns1=", \"dns1\":\"$STATIC_dns1\""
	if [ -z "$STATIC_dns2" ]; then
		STATIC_dns2=""
	else	
		STATIC_dns2=", \"dns2\":\"$STATIC_dns2\""
	fi
	STATIC_mtu=", \"mtu\":$STATIC_mtu"

	STATIC_cmd={$STATIC_ip$STATIC_subnet$STATIC_gw$STATIC_dns1$STATIC_dns2$STATIC_mtu} 
	
	JsonClient /tmp/cgi-2-sys set_ethwan_static_ip "$STATIC_cmd"
else
	echo "unknown EWAN_TYPE !!!"
fi

JsonClient /tmp/cgi-2-sys set_ethwan_connection_type "{\"type\": $EWAN_TYPE}"


WAN_PREF=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select etype from lteWanPriority;"`

JsonClient /tmp/cgi-2-sys set_network_connection_type "{\"type\": $WAN_PREF}"
