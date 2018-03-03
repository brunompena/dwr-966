#!/bin/sh

NTPPID=`pidof ntpclient`
if [ -n "$NTPPID" ]; then
killall ntpclient
fi

ENABLE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select enabled from ntpConfig";`
if [ "$ENABLE" = "1" ]; then
SERVER1=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select server1 from ntpConfig";`
#SERVER2=`sqlite3 /tmp/system.db "select server2 from ntpConfig";`
#SERVER3=`sqlite3 /tmp/system.db "select server3 from ntpConfig";`

SYNC_INTERVAL=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select reSyncNtpVal from ntpConfig";`
TIME_ZONE=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select timezone from ntpConfig";`

if [ "$SERVER1" != "" ]; then
    /usr/sbin/ntpclient  -h $SERVER1 -s -o "$TIME_ZONE" -l -i "$SYNC_INTERVAL" &
fi
#if [ "$SERVER2" != "" ]; then
#    ntpclient  -h $SERVER2 -s -e "$TIME_ZONE" -l -i "$SYNC_INTERVAL" &
#fi
#if [ "$SERVER3" != "" ]; then
#    ntpclient  -h $SERVER3 -s -e "$TIME_ZONE" -l -i "$SYNC_INTERVAL" &
#fi
fi
