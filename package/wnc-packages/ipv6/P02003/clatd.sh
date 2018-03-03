#!/bin/sh
PLAT_DEV=$2
WAN_PREFIX=$3
#CLAT_V6_ADDR=$4
LOG_FILE=/var/log/messages
TAYGA_CONFIG_FILE=/tmp/tayga.conf
CLAT_DEV="clat"
CLAT_V4_ADDR="192.0.0.1"
TAYGA_V4_ADDR="192.0.0.2"
#V4_ConnCheck_ENABLE="true"
V4_DefaultRoute_ENABLE="true"
IP6TABLEs_ENABLE="true"
#V4_DefaultRoute_METRIC="2048"
#V4_DefaultRoute_MTU="1260"

#create_tayga_conf() {
#  echo "tun-device" $CLAT_DEV > $TAYGA_CONFIG_FILE
#  echo "prefix" $PLAT_PREFIX >> $TAYGA_CONFIG_FILE
#  echo "ipv4-addr" $TAYGA_V4_ADDR >> $TAYGA_CONFIG_FILE
#  echo "map" $CLAT_V4_ADDR $CLAT_V6_ADDR >> $TAYGA_CONFIG_FILE
#}

start() {
#
# Step 1: Detect if there is an IPv4 default route on the system from before.
# If so we have no need for 464XLAT, and we can just exit straight away
#
  if [ "${V4_ConnCheck_ENABLE}" = "true" ]; then
    ConnCheck=`ip -4 route list default | grep -c default`
    if [ "${ConnCheck}" != "0" ]; then
      echo "clatd: This system already has IPv4 connectivity; no need for a CLAT." >> $LOG_FILE
      exit
    fi
  fi

#
# Write out the TAYGA config file
#
  rm -f $TAYGA_CONFIG_FILE
  /usr/local/bin/create_tayga_conf -p $WAN_PREFIX >> $LOG_FILE
  if [ -f "${TAYGA_CONFIG_FILE}" ]; then
    cat $TAYGA_CONFIG_FILE >> $LOG_FILE
	CLAT_V6_ADDR=`cat /tmp/tayga.conf | grep map | cut -d' ' -f3`
  else
    echo "clatd: Can not create tayga config file" >> $LOG_FILE
    exit
  fi
#
# Add ip6tables rules permitting traffic between the PLAT and the CLAT
#
  if [ "${IP6TABLEs_ENABLE}" = "true" ]; then
    ip6tables -I FORWARD -i $CLAT_DEV -o $PLAT_DEV -j ACCEPT
    ip6tables -I FORWARD -i $PLAT_DEV -o $CLAT_DEV -j ACCEPT
	#iptables -t nat -I NAT_SNAT -o clat -j MASQUERADE
    echo "clatd: Adding ip6tables rules allowing traffic between the CLAT and PLAT devices" >> $LOG_FILE
  fi

#
# Create the CLAT tun interface, add the IPv4 address to it as well as the
# route to the corresponding IPv6 address, and possibly an IPv4 default route
#
  tayga --config $TAYGA_CONFIG_FILE --mktun

  ip link set up dev $CLAT_DEV
  ip -4 address add $CLAT_V4_ADDR dev $CLAT_DEV
  ip -6 route add $CLAT_V6_ADDR dev $CLAT_DEV

  if [ "${V4_DefaultRoute_ENABLE}" = "true" ]; then
    if [ -n "${V4_DefaultRoute_METRIC}" -a -n "${V4_DefaultRoute_MTU}" ]; then
      ip -4 route add default dev $CLAT_DEV metric $V4_DefaultRoute_METRIC mtu $V4_DefaultRoute_MTU
    else
      if [ -n "${V4_DefaultRoute_METRIC}" ]; then
        ip -4 route add default dev $CLAT_DEV metric $V4_DefaultRoute_METRIC
      else
        if [ -n "${V4_DefaultRoute_MTU}" ]; then
          ip -4 route add default dev $CLAT_DEV mtu $V4_DefaultRoute_MTU
        else
          ip -4 route add default dev $CLAT_DEV
        fi
      fi
    fi
    echo "clatd: Adding IPv4 default route via the CLAT" >> $LOG_FILE
  fi
#
# All preparation done! We can now start TAYGA, which will handle the actual
# translation of IP packets.
#
  tayga --config $TAYGA_CONFIG_FILE --nodetach &
}

stop() {
  count=`pidof tayga | wc -w`
  if [ $count -ne 0 ]; then
    killall tayga
    tayga --config $TAYGA_CONFIG_FILE --rmtun
  fi
  if [ "${IP6TABLEs_ENABLE}" = "true" ]; then
    ip6tables -D FORWARD -i $CLAT_DEV -o $PLAT_DEV -j ACCEPT
    ip6tables -D FORWARD -i $PLAT_DEV -o $CLAT_DEV -j ACCEPT
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
