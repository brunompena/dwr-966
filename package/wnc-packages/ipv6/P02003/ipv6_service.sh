#!/bin/sh
DHCPPD_FILE=/tmp/dhcppd_result

get_wan_prefix() {
	    if [ -e "$DHCPPD_FILE" ]; then
	        DHCPPD_PREFIX=`grep 'prefix addr ' $DHCPPD_FILE | sed 's/^.*addr //g'`
	        DHCPPD_PRELEN=`grep 'prefix plen ' $DHCPPD_FILE | sed 's/^.*plen //g'`
	        DHCPPD_PTIME=`grep 'prefix pltime ' $DHCPPD_FILE | sed 's/^.*pltime //g'`
	        DHCPPD_VTIME=`grep 'prefix vltime ' $DHCPPD_FILE | sed 's/^.*vltime //g'`
	        #gen ipv6 prefix from wan side with DHCP-PD if DHCP-PD enabled
	        PREFIX_FROM_WAN=$DHCPPD_PREFIX/64
	        DHCP_START_FROM_WAN=${DHCPPD_PREFIX}${LAN_DHCPV6START}
	        DHCP_END_FROM_WAN=${DHCPPD_PREFIX}${LAN_DHCPV6END}
	    else
	        PREFIX_FROM_WAN=""
	        DHCP_START_FROM_WAN=""
	        DHCP_END_FROM_WAN=""
	    fi
}

get_lan_prefix() {
	    if [ -n "$AP_IPADDR_V6_PREFIX" -a "${LAN_V6_CONFIG}" = "on" ]; then
	        PREFIX_FROM_LAN=$AP_IPADDR_V6_PREFIX/64
	        DHCP_START_FROM_LAN=${AP_IPADDR_V6_PREFIX}${LAN_DHCPV6START}
	        DHCP_END_FROM_LAN=${AP_IPADDR_V6_PREFIX}${LAN_DHCPV6END}
	        ifconfig br0 ${AP_IPADDR_V6}/64
	        LAN_DHCPPD_PTIME=30
			LAN_DHCPPD_VTIME=40
	    else
	        PREFIX_FROM_LAN=""
	        DHCP_START_FROM_LAN=""
	        DHCP_END_FROM_LAN=""
	    fi
}

start() {
	. /sysconfig/ipv6-cfg
	if [ "${IPV6_ENABLE}" = "off" ]; then
	    sysctl -w net.ipv6.conf.all.disable_ipv6=1
	    echo "IPv6 ENABLE is off" >> /var/log/messages
	    exit
	else
		if [ "${LAN_V6_CONFIG}" = "on" -o -e "$DHCPPD_FILE" ]; then
			sysctl -w net.ipv6.conf.all.disable_ipv6=0
			sysctl -w net.ipv6.conf.all.forwarding=1
	
			if [ "${LAN_V6_DNS}" = "on" ]; then
				if [ -e /tmp/ipv6_usb0.result ]; then
					PRIMARY_V6DNS=`grep 'dns ' /tmp/ipv6_usb0.result | cut -d' ' -f2 | tail -1`
					SECOND_V6DNS=`grep 'dns ' /tmp/ipv6_usb0.result | cut -d' ' -f3 | tail -1`
					if [ -n "$PRIMARY_V6DNS" ] && [ "$PRIMARY_V6DNS" = "0:0:0:0:0:0:0:0" ]; then
						PRIMARY_V6DNS=2001:4860:4860::8888
					fi
					if [ -n "$SECOND_V6DNS" ] && [ "$SECOND_V6DNS" = "0:0:0:0:0:0:0:0" ]; then
						SECOND_V6DNS=""
					fi
				else
					PRIMARY_V6DNS=""
					SECOND_V6DNS=""
				fi
			else
				PRIMARY_V6DNS=$WAN_IPV6PRIDNS
				SECOND_V6DNS=$WAN_IPV6SECDNS
			fi

			if [ -z "$PRIMARY_V6DNS" ]; then
				PRIMARY_V6DNS=2001:4860:4860::8888
			fi

			#if [ "${IPV6_TYPE}" != "6to4" ]; then
	        get_wan_prefix
	        get_lan_prefix
			#else
				#TODO: PREFIX_FROM_6to4=$(find_6to4_prefix ${usb0_ip})
			#fi
			. /usr/local/bin/radvd.sh
			run_radvd
	
			if [ "${LAN_V6_TYPE}" != "stateless" ]; then
				. /usr/local/bin/dhcpv6.sh
				run_dhcpv6s
			fi
		else
			sysctl -w net.ipv6.conf.all.disable_ipv6=1
			echo "IPv6 PREFIX_FROM_LAN and PREFIX_FROM_WAN is empty" >> /var/log/messages
			exit
		fi
	fi
}
stop() {
	count=`pidof radvd | wc -w`
	if [ $count -ne 0 ]; then
		killall radvd
	fi
	
	count=`pidof dhcp6s | wc -w`
	if [ $count -ne 0 ]; then
		killall dhcp6s
	fi
}

if [ $# == 0 ]; then
  CMD="restart"
else
  CMD=$1
fi

case $CMD in
  start)
    start $2
    ;;
  stop)
    stop
    ;;
  restart|reload)
    stop
    start $2
    ;;
  *)
    echo $"Usage: $0 {start|stop|restart}"
    exit 1
    ;;
esac
