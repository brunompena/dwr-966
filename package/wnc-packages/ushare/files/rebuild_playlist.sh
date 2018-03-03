#!/bin/sh

ENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select enable from dlnaConfig"`;
USHARE_PID=`pidof ushare | awk '{print $NF}'`
if [ "$ENABLE" = "1" ]; then
  if [ -n "$USHARE_PID" ]
  then
	logger -t [DLNA] -p local4.info "send signal to ushare to rebuild list"
	`kill -USR1 $USHARE_PID`
  else
	logger -t [DLNA] -p local4.info "restart ushare to build the list"
	`ushare -f /etc/ushare.conf -D`
  fi
fi


