#!/bin/sh
return_status=true

# Defines
if [ ! "MTLK_INIT_PLATFORM" ]; then
	. /tmp/mtlk_init_platform.sh
fi

command=$1
apIndex=$2

# Get corresponding wlan network interface from mapping file
wlan=`find_wave_if_from_index $apIndex`

# Physical AP to which the VAP belongs.
phyAP=`echo $wlan | cut -f1 -d "."`
# VAP index of the VAP.
vapIdx=`echo $wlan | cut -f2 -d "."`

start_mtlk_init_mbss()
{
	print2log DBG "mtlk_init_mbss.sh: Start"
	
	print2log DBG "mtlk_init_mbss.sh: phyAP = $phyAP"
	print2log DBG "mtlk_init_mbss.sh: vapIdx $vapIdx"
	api_wrapper set iwpriv $phyAP sAddVap $vapIdx 
	res=$?
	# verify that VAP was added successful
	count=0
	while [ $res != 0 ]
	do
		if [ $count -lt 3 ]
		then
			sleep 1
	                api_wrapper set iwpriv $phyAP sAddVap $vapIdx
			res=$?
		else
			print2log ALERT "mtlk_init_mbss.sh: add of $wlan to $phyAP failed"
			echo "mtlk_init_mbss.sh: add of $wlan to $phyAP failed" >> /tmp/wave_${wlan}_start_failure
			exit 1
		fi
		let count=$count+1
	done
	
	print2log DBG "mtlk_init_mbss.sh: BSS $wlan created. Done Start"
}

stop_mtlk_init_mbss()
{
	print2log DBG "mtlk_init_mbss.sh: Start stop_mtlk_init_mbss"
	print2log DBG "mtlk_init_mbss.sh: vapIdx $vapIdx"
	api_wrapper set iwpriv $phyAP sDelVap $vapIdx
	res=$?
	# verify that VAP was removed successful
	count=0
	while [ $res != 0 ]
	do
		if [ $count -lt 4 ]
		then
			sleep 1
	                api_wrapper set iwpriv $phyAP sDelVap $vapIdx
			res=$?
		else
			print2log ALERT "Removing VAP $wlan failed by driver after 5 tries"
			exit
		fi
		let count=$count+1
	done
	
	print2log DBG "mtlk_init_mbss.sh: BSS $wlan removed"
}

create_config_mtlk_init_mbss()
{
	return
}

should_run_mtlk_init_mbss()
{
#	mbss=`host_api get $$ $apIndex MBSSEnabled`
#	print2log DBG "mtlk_init_mbss.sh: should_run: mbss = $mbss"
#	if [ $mbss ]
#	then
#		true
#	else
#		print2log DBG "mtlk_init_mbss.sh: should_run: MBSS not enabled"
#		false
#	fi		
#	return
	print2log DBG "mtlk_init_mbss.sh: should_run"
	return_status=`check_failure $wlan`
	if [ "$return_status" = "false" ]
	then
		return
	fi
	# Check if the physical AP failed and if so, don't start the VAP.
	if [ -e $wave_start_failure ]
	then
		if [ `cat $wave_start_failure | grep "$phyAP failed:" -c` -gt 0 ]
		then
			print2log ALERT "mtlk_init_mbss.sh: add of $wlan to $phyAP failed. Since $phyAP failed before that."
			echo "mtlk_init_mbss.sh: add of $wlan to $phyAP failed. Since $phyAP failed before that." >> /tmp/wave_${wlan}_start_failure
			return_status=false
		fi
	fi
}


case $command in
	start)
		start_mtlk_init_mbss
	;;
	stop)
		stop_mtlk_init_mbss
	;;
	create_config)
		create_config_mtlk_init_mbss
	;;
	should_run)
		should_run_mtlk_init_mbss
	;;
esac

$return_status
