#!/bin/sh
return_status=true

# Defines
if [ ! "$MTLK_INIT_PLATFORM" ]; then
	. /tmp/mtlk_init_platform.sh
fi

command=$1
apIndex=$2

# Get corresponding wlan network interface from mapping file
wlan=`find_wave_if_from_index $apIndex`

start_mtlk_init_rdlim()
{
	print2log DBG "mtlk_init_rdlim.sh Start"
	
	if [ ! -e /tmp/ini.tcl ]; then ln -s $ETC_PATH/ini.tcl /tmp/ini.tcl; fi
	if [ ! -e /tmp/rdlim.ini ]; then ln -s $ETC_PATH/rdlim.ini /tmp/rdlim.ini; fi
	print2log DBG "mtlk_init_rdlim.sh: $ETC_PATH/rdlim.tcl $wlan"
	$ETC_PATH/rdlim.tcl $wlan
	if [ $? != 0  ]
	then
		print2log DBG "mtlk_init_rdlim.sh: rdlim.tcl failed for $wlan"
		echo "mtlk_init_rdlim.sh: rdlim.tcl failed for $wlan" >> /tmp/wave_${wlan}_start_failure
		exit 1
	fi
	if [ -e $CONFIGS_PATH/hw_wlan0.ini ] && [ ! -e /tmp/HW.ini ]
	then
		#Workaround: create link in the tmp until fix drvhlpr and more for dual concurrent"
		# TODO: Needs a fix when we have a new drvhlpr that supports dual concurrent
		ln -s $CONFIGS_PATH/hw_wlan0.ini  /tmp/HW.ini
	fi
	#Make sure defaults CoC related parameters are according to real HW
	coc_power=`host_api get $$ $apIndex CoCPower` 
	sw_num_antennas=`echo $coc_power | awk '{print $2}'` 
	hw_Support_3TX=`host_api get $$ hw_$wlan Support_3TX` 
	if [ $sw_num_antennas -gt 2 ] && [ $hw_Support_3TX -eq 0 ]
	then
		#hw doesn't support 3X3, overwrite with 2X2
		coc_power_new=`echo $coc_power | awk '{print $1" 2 2"}'`
		host_api set $$ $apIndex CoCPower "$coc_power_new"
	fi
	#config_save.sh
	print2log DBG "start mtlk_init_rdlim.sh: Done Start"
}

stop_mtlk_init_rdlim()
{
	return
}

create_config_mtlk_init_rdlim()
{
	return
}

should_run_mtlk_init_rdlim()
{
	print2log DBG "mtlk_init_rdlim.sh: should_run"	
	return_status=`check_failure $wlan`
	if [ "$return_status" = "false" ]
	then
		return
	fi

	# Check if hw ini file for the current interface already exists
	if [ -e $CONFIGS_PATH/hw_$wlan.ini ]
	then
		print2log DBG "mtlk_init_rdlim.sh:should_run: hw_$wlan.ini already exists"
		return_status=false
	fi
}

case $command in
	start)
		start_mtlk_init_rdlim
	;;
	stop)
		stop_mtlk_init_rdlim
	;;
	create_config)
		create_config_mtlk_init_rdlim
	;;
	should_run)
		should_run_mtlk_init_rdlim
	;;
esac

$return_status
