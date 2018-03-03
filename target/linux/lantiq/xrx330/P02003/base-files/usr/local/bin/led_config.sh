#!/bin/sh
echo "RUN led_config.sh"
checked=0
while [ "_$checked" = "_0" ]
do
	#echo 0 > /sys/class/leds/power/delay_on
	echo 0 > /sys/class/leds/power/delay_off
	echo 1 > /sys/class/leds/power/brightness
	sleep 1
	#dly_on=`cat /sys/class/leds/power/delay_on`
	dly_off=`cat /sys/class/leds/power/delay_off`
	brtness=`cat /sys/class/leds/power/brightness`
	echo "Power LED : delay_on=$dly_on, delay_off=$dly_off, brightness=$brtness"
	if [ "_$dly_off" = "_0" ]; then
		if [ "_$brtness" = "_1" ]; then
			checked=1
		fi
	fi
done
