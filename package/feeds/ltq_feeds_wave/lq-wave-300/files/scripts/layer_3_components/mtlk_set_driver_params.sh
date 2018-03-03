#!/bin/sh
return_status=true

# Defines
if [ ! "$MTLK_INIT_PLATFORM" ]; then			
	. /tmp/mtlk_init_platform.sh
fi

print2log DBG "mtlk_set_driver_params.sh: args: $*"

command=$1
apIndex=$2

# Get corresponding wlan network interface from mapping file
wlan=`find_wave_if_from_index $apIndex`

# Optional parameter, used to update all parameters for the requested object (i.e. wlan_main, wlan_security etc.)
object_name=$3
PARAMS_LIST_FILE=/tmp/driver_api_params_to_set_$apIndex

timestamp "mtlk_set_driver_params.sh:$command:$apIndex:begin"

IWPRIV_SET ()
{
	api_wrapper set iwpriv $1 "s$2" $3
}

assignee ()
{
	if [ `expr $1 : $2` != 0 ]
	then
		echo $3
	else
		echo $4
	fi
}

start_mtlk_set_driver_params()
{
	print2log DBG "mtlk_set_driver_params.sh: Start"
	if [ ! -e /tmp/set_driver_params_${apIndex}.sh ]
	then
		print2log ALERT "/tmp/set_driver_params_${apIndex}.sh was not found."
		echo "/tmp/set_driver_params_${apIndex}.sh was not found." >> /tmp/wave_${wlan}_start_failure
		exit 1
	fi
			
	api_wrapper file  /tmp/set_driver_params_${apIndex}.sh
	
	#TODO: how control commands order (such as wep). maybe differens queries
	print2log DBG "start mtlk_set_driver_params.sh: Done Start"
}

stop_mtlk_set_driver_params()
{
	return
}

create_config_mtlk_set_driver_params()
{
	print2log DBG "mtlk_set_driver_params.sh: create_config"
	if [ ! -e /tmp/ini.tcl ]; then ln -s $ETC_PATH/ini.tcl /tmp/ini.tcl; fi
	if [ ! -e /tmp/driver_api.ini ]; then ln -s $ETC_PATH/driver_api.ini /tmp/driver_api.ini; fi
	#get_query | awk -F "/" '
	#{
	#	a="'"'"'";
	#	gsub("NULL","\\N",$0)
	#	print $3, $2, $1, a $4 a
	#}' > /tmp/set_driver_params.sh
	
	#host_api commit $$
	#config_save.sh
	
	host_api get_all $$ $apIndex $object_name > $PARAMS_LIST_FILE
	$ETC_PATH/driver_api.tcl DriverSetAll $apIndex $PARAMS_LIST_FILE
	# Check if debug mode of saving all the driver configurations is on.
	Debug_save_conf=`host_api get $$ $apIndex Debug_save_conf`
	if [ "$Debug_save_conf" = "$YES" ]
	then
		Debug_save_conf_dest=`host_api get $$ $apIndex Debug_save_conf_dest`
		cat /tmp/set_driver_params_${apIndex}.sh >> $Debug_save_conf_dest
	fi
	print2log DBG "mtlk_set_driver_params.sh: Done create_config ${apIndex}"
}

should_run_mtlk_set_driver_params()
{
	return_status=`check_failure $wlan`
}


case $command in
	start)
		start_mtlk_set_driver_params
	;;
	stop)
		stop_mtlk_set_driver_params
	;;
	create_config)
		create_config_mtlk_set_driver_params
	;;
	should_run)
		should_run_mtlk_set_driver_params
	;;
	reconfigure)
		stop_mtlk_set_driver_params
		create_config_mtlk_set_driver_params
		start_mtlk_set_driver_params
	;;
esac

timestamp "mtlk_set_driver_params.sh:$command:$apIndex:done"
$return_status
