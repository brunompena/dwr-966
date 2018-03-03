#!/bin/sh
WIFI_SETTING=`cat /tmp/wifisetting_11n`
Debug=`cat /tmp/wifiDebug`
[ "$Debug" != "" ] && echo "Debug mode on"
# call from wifiInit, set this flag.
INIT=$1

# define
WLANIf_1="wlan0"
WLANIf_2="wlan0.0"
WLANCONF_1="/tmp/hostapd_1.conf"
WLANCONF_2="/tmp/hostapd_2.conf"
# import LTE band info
if [ -f /tmp/bandinfo ]; then
    source /tmp/bandinfo
    #mv -f /tme/bandinfo /tme/bandinfo.bk
    [ "$Debug" != "" ] && echo "LTEcoex=$LTEcoex"
    [ "$Debug" != "" ] && echo "CHcoex=$CHcoex"
    [ "$Debug" != "" ] && echo "CHSETcoex=$CHSETcoex"
    if [ "$LTEcoex" == "0" ]; then
        echo "LTEcoex is not set"
        exit 0
    fi
else 
    echo "cant get the LTE bandinfo"
    exit 0
fi

if [ "$INIT" == "1" ]; then
    # This part is called from wifiInit
    if [ "$CHcoex" != "0" ]; then
        # non-interference channel availabe.
        iwpriv $WLANIf_1 sFortyMHzOpMode 0
        iwconfig $WLANIf_1 channel $CHcoex
        iwconfig $WLANIf_2 channel $CHcoex
    fi
else 
    # Trigger from LTE(Reserved, security will mismatch)
    WLANCh=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select configuredChannel from wifiRadio where radioNo = '1';"`
    [ "$Debug" != "" ] && echo "WLANCh=$WLANCh"
    if [ "$WLANCh" != "0" ]; then
        echo "WIFI channel specific"
        exit 0
    fi 

    if [ -f /tmp/BootOK ]; then
        echo "LTE coex triggered"
    else
        echo "LTE coex called in Boot phase"
        exit 0
    fi

    if [ "$WIFI_SETTING" == "1" ]; then
        echo "WIFI 11n setting is running"
        exit 0
    else
        echo 1 > /tmp/wifisetting_11n
    fi

    WLANEn_1=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select wlanEnabled from wifiAP where wlanName = 'ap1';"`
    WLANEn_2=`sqlite3 -cmd ".timeout 1000" /tmp/system.db "select wlanEnabled from wifiAP where wlanName = 'ap2';"`
    [ "$Debug" != "" ] && echo "WLANEn_1=$WLANEn_1"
    [ "$Debug" != "" ] && echo "WLANEn_2=$WLANEn_2"
    killall hostapd

    if [ "$WLANEn_1" == "1" ]; then
        ifconfig $WLANIf_1 down
        [ "$Debug" != "" ] && echo "disable $WLANIf_1"
    fi

    if [ "$WLANEn_2" == "1" ]; then
        ifconfig $WLANIf_2 down
        [ "$Debug" != "" ] && echo "disable $WLANIf_2"
    fi

    iwpriv $WLANIf_1 sFortyMHzOpMode 0
    iwconfig $WLANIf_1 channel $CHcoex
    iwconfig $WLANIf_2 channel $CHcoex

    if [ "$WLANEn_1" == "1" ]; then
        ifconfig $WLANIf_1 up
        [ "$Debug" != "" ] && echo "enable $WLANIf_1"
        hostapd $WLANCONF_1 &
    fi

    if [ "$WLANEn_2" == "1" ]; then
        ifconfig $WLANIf_2 up
        [ "$Debug" != "" ] && echo "enable $WLANIf_2"
        hostapd $WLANCONF_2 &
    fi

    rm /tmp/wifisetting_11n

fi

