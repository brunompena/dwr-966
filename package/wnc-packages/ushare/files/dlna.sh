#!/bin/sh

USHARE_PID=`pidof ushare`
if [ -n "$USHARE_PID" ]; then
killall ushare
fi

ENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select enable from dlnaConfig;"`;
logger -t [DLNA] -p local4.info "dlnaConfig enable=$ENABLE"
`echo $ENABLE > /tmp/dlnaStatus`

if [ "$ENABLE" = "0" ]; then
logger -t [DLNA] -p local4.info dlna stop
fi

if [ "$ENABLE" = "1" ]; then
`ushare -f /etc/ushare.conf -D`
logger -t [DLNA] -p local4.info dlna start
fi
