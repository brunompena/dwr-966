#!/bin/sh
return_status=true

# Defines
if [ ! "$MTLK_INIT_PLATFORM" ]; then			
	. /tmp/mtlk_init_platform.sh
fi

command=$1
driver_mode=$2
if [ -z "$driver_mode" ]; then driver_mode="ap"; fi

insmod_device()
{
	cd /tmp	
	insmod $@
	# verify that insmod was successful
	if [ `lsmod | grep "mtlk " -c` -eq 0  ]
	then
		print2log ALERT "mtlk_insmod_wls_driver: driver insmod is failed"
		echo "mtlk_insmod_wls_driver: driver insmod is failed" >> $wave_init_failure
		exit
        fi

	cd - > /dev/null
}

rmmod_device()
{
	device=$1

	# In mtlkroot.ko insmod we pass to the driver the Netlink ID to reuse it on Netlink create.
	# Therefore, before removing driver, use iwpriv to read the ID that the kernel allocated, save it in file.
	mtlk_genl_family_id=`api_wrapper get iwpriv wlan0 gGenlFamilyId \| awk -F \":\" \'{print \\$2}\'`
	print2log DBG "mtlk_insmod_wls_driver:  after rmmod, mtlk_genl_family_id=$mtlk_genl_family_id"
	if [ "$mtlk_genl_family_id" != "" ]
	then
		echo $mtlk_genl_family_id > $GENL_FAMILY_ID_FILE
	fi

	rmmod $device	
	res=$?
	# verify that rmmod was successful
	count=0
	while [ $res != 0 ]
	do
		if [ $count -lt 3 ]
		then
			rmmod $device
			res=$?
			sleep 2
		else
			print2log ALERT "mtlk_insmod_wls_driver: rmmod of $device is failed"
			print2log INFO "mtlk_insmod_wls_driver: Kill drvhlpr process before rmmod of $device"
			break
		fi
		count=`expr $count + 1`
	done		
}

check_wave400()
{
	if [ `ls /sys/bus/platform/devices/ | grep mtlk -c` -eq 0 ]
	then
		return_status=false
	fi
}

start_insmod_wls_driver()
{	
	print2log DBG "mtlk_insmod_wls_driver: Start"
	# Get the index of wlan0
	apIndex=`find_index_from_wave_if wlan0`
	wlan_count=`host_api get $$ sys wlan_count`	
	NETWORK_TYPE=`host_api get $$ $apIndex network_type`
	wlanm_cmd=""
	check_wave400
	if [ $return_status == true ]
	then
		#wlanm is retrieved from u-boot and represents the MAC FW start address in DDR.
		wlanm=`uboot_env --get --name mem | cut -d M -f 0`
		wlanm_cmd="bb_cpu_ddr_mb_number=$wlanm"
	fi

	if [ "$NETWORK_TYPE" = "$AP" ]
	then
		if [ $wlan_count = 1 ]
		then
			insmod_device mtlk.ko $driver_mode=1 $wlanm_cmd
		else
			insmod_device mtlk.ko $driver_mode=1,1 $wlanm_cmd
		fi
	else
		insmod_device mtlk.ko $wlanm_cm
	fi
	
	awk '/mtlk/ {print $1" module loaded in address: " $6}' /proc/modules > /dev/console
	print2log DBG "mtlk_insmod_wls_driver: Done Start"
}

stop_insmod_wls_driver()
{
	print2log DBG "mtlk_insmod_wls_driver: Stop"

	if [ `lsmod | grep "mtlk " -c` -gt 0  ]
	then
		rmmod_device mtlk
	else
		print2log DBG "mtlk_insmod_wls_driver: mtlk driver wasn't loaded"
	fi
	
	print2log DBG "mtlk_insmod_wls_driver: Done Stop"
}

create_config_insmod_wls_driver()
{	
	return	
}

should_run_insmod_wls_driver()
{
	print2log DBG "mtlk_insmod_wls_driver: should_run"
	return_status=`check_failure wlan0`
}

case $command in
	start)
		start_insmod_wls_driver
	;;
	stop)
		stop_insmod_wls_driver
	;;
	create_config)
		create_config_insmod_wls_driver
	;;
	should_run)
		should_run_insmod_wls_driver
	;;
esac

$return_status
