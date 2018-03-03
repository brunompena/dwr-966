#! /bin/sh

EXTIF=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select interfaceName from networkInterface where LogicalIfName = 'LTE-WAN1';"`

# Create filter chains
ip6tables -N BAD_PACKETS
ip6tables -N FWFlood
ip6tables -N INBOUND

ip6tables -N BAD_TCP_PACKETS
ip6tables -N IPv6BadAddrs

ip6tables -N FW_SYN_FLOOD
ip6tables -N FWICMPFlood

# Chain: BAD_PACKETS
ip6tables -A BAD_PACKETS -p tcp -j BAD_TCP_PACKETS
ip6tables -A BAD_PACKETS -p ALL -j IPv6BadAddrs
ip6tables -A BAD_PACKETS -j RETURN

# Chain: FWFlood
ip6tables -A FWFlood -p tcp --syn -j FW_SYN_FLOOD
ip6tables -A FWFlood -p icmpv6 -j FWICMPFlood

# Chain: BAD_TCP_PACKETS
# Stealth scan
ip6tables -A BAD_TCP_PACKETS -p tcp --tcp-flags ALL NONE -j DROP
ip6tables -A BAD_TCP_PACKETS -p tcp --tcp-flags ALL ALL -j DROP
ip6tables -A BAD_TCP_PACKETS -p tcp --tcp-flags ALL FIN,URG,PSH -j DROP
ip6tables -A BAD_TCP_PACKETS -p tcp --tcp-flags ALL SYN,RST,ACK,FIN,URG -j DROP
ip6tables -A BAD_TCP_PACKETS -p tcp --tcp-flags SYN,RST SYN,RST -j DROP
ip6tables -A BAD_TCP_PACKETS -p tcp --tcp-flags SYN,FIN SYN,FIN -j DROP

# add rule in IPv6BadAddrs chain
ip6tables -A IPv6BadAddrs -s ::1 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d ::1 -j DROP
ip6tables -A IPv6BadAddrs -s ::224.0.0.0/100 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d ::224.0.0.0/100 -j DROP
ip6tables -A IPv6BadAddrs -s ::127.0.0.0/104 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d ::127.0.0.0/104 -j DROP
#ip6tables -A IPv6BadAddrs -s ::/104 -d ::/0 -j DROP
#ip6tables -A IPv6BadAddrs -s ::/0 -d ::/104 -j DROP
ip6tables -A IPv6BadAddrs -s ::255.0.0.0/104 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d ::255.0.0.0/104 -j DROP
ip6tables -A IPv6BadAddrs -s 2002:e000::/20 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d 2002:e000::/20 -j DROP
ip6tables -A IPv6BadAddrs -s 2002:7f00::/24 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d 2002:7f00::/24 -j DROP
ip6tables -A IPv6BadAddrs -s 2002::/24 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d 2002::/24 -j DROP
ip6tables -A IPv6BadAddrs -s 2002:ff00::/24 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d 2002:ff00::/24 -j DROP
ip6tables -A IPv6BadAddrs -s 2002:a00::/24 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d 2002:a00::/24 -j DROP
ip6tables -A IPv6BadAddrs -s 2002:ac00::/24 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d 2002:ac00::/24 -j DROP
ip6tables -A IPv6BadAddrs -s 2002:c0a8::/32 -d ::/0 -j DROP
ip6tables -A IPv6BadAddrs -s ::/0 -d 2002:c0a8::/32 -j DROP

# Syn Flooding
# Should likely tuned based on expected limit of desired connections/second.
ip6tables -A FW_SYN_FLOOD -p tcp --syn -m limit --limit 128/second --limit-burst 128 -j RETURN
ip6tables -A FW_SYN_FLOOD -p tcp --syn -j DROP
ip6tables -A FW_SYN_FLOOD -p tcp -j RETURN

# Chain: FWICMPFlood ###
# Echo replies
ip6tables -A FWICMPFlood -p icmpv6 --icmpv6-type 128 -m limit --limit 100/s --limit-burst 100 -j RETURN
ip6tables -A FWICMPFlood -p icmpv6 --icmpv6-type 128 -j DROP
ip6tables -A FWICMPFlood -p icmpv6 -j RETURN


###
# Create filter chains
###
ip6tables -N BLACKLISTED_INBOUND
ip6tables -N ICMP_INBOUND

###
# Chain: INBOUND
###
ip6tables -A INBOUND -j BLACKLISTED_INBOUND
ip6tables -A INBOUND -p icmpv6 -j ICMP_INBOUND
ip6tables -A INBOUND -j RETURN

###
# Chain: Blacklisted INBOUND
###
ip6tables -A BLACKLISTED_INBOUND -m pkttype --pkt-type broadcast -j DROP
ip6tables -A BLACKLISTED_INBOUND -j RETURN

###
# Chain: ICMP INBOUND
###
ip6tables -A ICMP_INBOUND -p icmpv6 --icmpv6-type echo-request -m limit --limit 900/min -j RETURN
ip6tables -A ICMP_INBOUND -p icmpv6 --icmpv6-type echo-reply -m limit --limit 900/min -j RETURN
# Allow router advertisements on local network segments
for icmptype in 133 134 135 136 137 141 142 151
do
	ip6tables -A ICMP_INBOUND -p icmpv6 --icmpv6-type $icmptype -m hl --hl-eq 255 -j RETURN
done
# Allow RFC 4890 but with rate-limiting
for icmptype in 1 2 3/0 3/1 4/0 4/1 4/2 130 131 132 135 136 141 142 143 148 149 151 152
do
	ip6tables -A ICMP_INBOUND -p icmpv6 --icmpv6-type $icmptype -m limit --limit 900/min -j RETURN
done
# Mobile prefix discovery replies
for icmptype in 145 147
do
	ip6tables -A ICMP_INBOUND -p icmpv6 --icmpv6-type $icmptype -m limit --limit 900/min -j RETURN
done
ip6tables -A ICMP_INBOUND -p icmpv6 -j DROP

# Chains
ip6tables -A INPUT -j BAD_PACKETS
ip6tables -A FORWARD -j BAD_PACKETS
ip6tables -A INPUT -j FWFlood
ip6tables -A FORWARD -j FWFlood
ip6tables -A INPUT -i $EXTIF -j INBOUND


ip6tables -N ICMP_FORWARD
ip6tables -A FORWARD -p icmpv6 -j ICMP_FORWARD

# RFC 4890
for icmptype in 1 2 3/0 3/1 4/0 4/1 4/2 128 129 130 131 132 133 135 136 141 142 143 148 149 151 152 153
do
	ip6tables -A ICMP_FORWARD -p icmpv6 --icmpv6-type $icmptype -j RETURN
done
# Mobile prefix discovery
for icmptype in 144 145 146 147
do
	ip6tables -A ICMP_FORWARD -p icmpv6 --icmpv6-type $icmptype -j RETURN
done
ip6tables -A ICMP_FORWARD -j DROP


# forbbden Forward bad ipv6 address
ip6tables -N IPv6ForwardBadAddrs

#ip6tables -A FWInputFilter -i $EXTIF -p tcp --syn -j DROP
#ip6tables -A FWForwardFilter -i $EXTIF -p tcp -m tcp --dport 32768:61000 --syn -j DROP
ip6tables -A IPv6ForwardBadAddrs -s fec0::/16 -d ::/0 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::/0 -d fec0::/16 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::ffff:0:0/96 -d ::/0 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::/0 -d ::ffff:0:0/96 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::192.168.0.0/112 -d ::/0 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::/0 -d ::192.168.0.0/112 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ff02::/16 -d ::/0 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::/0 -d ff02::/16 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s 2001:db8::/32 -d ::/0 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::/0 -d 2001:db8::/32 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s 2001:10::/32 -d ::/0 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::/0 -d 2001:10::/32 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ff0e::/16 -d ::/0 -j DROP
ip6tables -A IPv6ForwardBadAddrs -s ::/0 -d ff0e::/16 -j DROP
#ip6tables -A FWInputFilter -i $EXTIF -p udp ! --dport 32768:60999 -j DROP
#ip6tables -A FWForwardFilter -i $EXTIF -p udp ! --dport 32768:60999 -j DROP 

#ip6tables -A INPUT -j FWInputFilter
ip6tables -A FORWARD -j IPv6ForwardBadAddrs
