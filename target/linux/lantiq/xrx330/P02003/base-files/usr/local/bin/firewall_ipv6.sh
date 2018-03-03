#!/bin/sh

echo "run script : firewall_ipv6.sh"

if [ "_$1" = "_init" ];then
	echo "firewall_ipv6 init"
else
	logger -t [IP6TABLES] -p local2.info firewall restart
fi

EXTIF=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select interfaceName from networkInterface where LogicalIfName = 'LTE-WAN1';"`
NATENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select Enable from FirewallNatConfig;"`

ip6tables -F 
ip6tables -X 
ip6tables -t mangle -F
ip6tables -t mangle -X 

# bridge mode
if [ "$NATENABLE" = "5" ]; then 
	ip6tables -P INPUT ACCEPT
	ip6tables -P OUTPUT ACCEPT
	ip6tables -P FORWARD ACCEPT
	return
fi

ip6tables -A INPUT -i lo -j ACCEPT
ip6tables -A OUTPUT -o lo -j ACCEPT

ip6tables -A OUTPUT -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu
ip6tables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS  --clamp-mss-to-pmtu

# LOG packets, then DROP.
ip6tables -N DROPLOG
#iptables -A DROPLOG -j $LOG $RLIMIT --log-level "info" --log-prefix "[IPTABLES_DROP] "
ip6tables -A DROPLOG -j DROP

# GARYeh 20120505: load firewall rule
/usr/local/bin/ip6tables_fw.sh init

# GARYeh 20120407: output ipv6 filter rule to FORWARD ip6tables
/usr/local/bin/filter_ipv6.sh init

#adding the UPNP chain for filter with ipv6
ip6tables -N UPNP
ip6tables -A FORWARD -i $EXTIF ! -o $EXTIF -j UPNP

