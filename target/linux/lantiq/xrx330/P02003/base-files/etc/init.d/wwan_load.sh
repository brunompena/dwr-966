#!/bin/sh /etc/rc.common

#START=20
#STOP=20

start() {
	echo "insmod lte modules ....."
	insmod /lib/modules/`uname -r`/usbnet.ko
	insmod /lib/modules/`uname -r`/rmnet_usb.ko
	insmod /lib/modules/`uname -r`/usbserial.ko
	insmod /lib/modules/`uname -r`/usb_wwan.ko
	insmod /lib/modules/`uname -r`/qcserial.ko

	#mount module ums
	insmod /lib/modules/`uname -r`/fat.ko
	insmod /lib/modules/`uname -r`/vfat.ko
	insmod /lib/modules/`uname -r`/nls_cp437.ko
	insmod /lib/modules/`uname -r`/nls_iso8859-1.ko

	Retry=0
	num=`lsusb | wc -l`
	while [ "$num" -lt 5 ] && [ $Retry -lt 5 ]; do
		# light blue led when error happen
		# echo 1 > /sys/class/leds/NW_ERR/brightness
		echo "Network error"
		sleep 2
		Retry=$(( $Retry + 1 ))
		echo "Retry: $Retry"
		num=`lsusb | wc -l`
	done
	if [ "$num" -lt 5 ]; then
		exit
	else
		# close blue led 
		# echo 0 > /sys/class/leds/NW_ERR/brightness
		echo "Network ok"
	fi
}

