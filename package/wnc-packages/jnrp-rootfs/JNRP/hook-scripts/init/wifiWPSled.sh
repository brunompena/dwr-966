#!/bin/sh
command=$1
LED_PATH=/sys/class/leds/wps
echo "control WPS led"

#define WPS_LED_OFF         0 
#define WPS_LED_ON          1
#define WPS_LED_BLINK_ACT   2 // light LED 0.3 sec every 1 sec
#define WPS_LED_BLINK_ERR   3 // light LED 0.3 sec every 2 sec

wps_led_off () {
    echo "wps led off"
    echo "none" > $LED_PATH/trigger
}

wps_led_on () {
    echo "wps led on"
    echo "timer" > $LED_PATH/trigger
    echo 0 > $LED_PATH/delay_off
}

wps_led_active () {
    echo "wps led active"
    echo "timer" > $LED_PATH/trigger
    echo 300 > $LED_PATH/delay_on    
    echo 700 > $LED_PATH/delay_off    
}

wps_led_err () {
    echo "wps led active"
    echo "timer" > $LED_PATH/trigger
    echo 300 > $LED_PATH/delay_on
    echo 1700 > $LED_PATH/delay_off
}

case $command in 
    0 )
        wps_led_off
    ;;
    1 )
        wps_led_on
    ;;
    2 )
        wps_led_active
    ;;
    3 )
        wps_led_err
    ;;
    * )
        echo "unknow wps led state"
    ;;
esac
