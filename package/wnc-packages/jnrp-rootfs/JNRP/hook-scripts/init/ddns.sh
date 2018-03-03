#!/bin/sh

NOIPID=`pidof noip2`
if [ -n NOIPID ]; then
killall noip2
fi

ENABLE=`sqlite3 /tmp/system.db "select Enable from ddnsConfig"`
if [ "$ENABLE" = "1" ]; then
    TYPE=`sqlite3 /tmp/system.db "select TypeName from ddnsConfig"`
    USER=`sqlite3 /tmp/system.db "select userName from ddns where typeName='$TYPE'"`
    PASS=`sqlite3 /tmp/system.db "select pass from ddns where typeName='$TYPE'"`
    HOST=`sqlite3 /tmp/system.db "select hostName from ddns where typeName='$TYPE'"`
    IF=`sqlite3 /tmp/system.db "select interface from ddns where typeName='$TYPE'"`

    if [ "$TYPE" = "qdns" ]; then
        echo "service-type=qdns" > /tmp/$TYPE.conf
    elif [ "$TYPE" = "dyndns" ]; then
        echo "service-type=dyndns" > /tmp/$TYPE.conf
    elif [ "$TYPE" = "noip" ]; then
        noip2 -C -I "$IF" -u "$USER" -p "$PASS" -U 30
        noip2
    fi

    if [ "$TYPE" != "noip" ]; then
        echo "user=$USER:$PASS" >> /tmp/"$TYPE".conf
        echo "host=$HOST" >> /tmp/$TYPE.conf
        echo "interface=$IF" >> /tmp/$TYPE.conf
        ez-ipupdate -c /tmp/$TYPE.conf
    fi
fi
