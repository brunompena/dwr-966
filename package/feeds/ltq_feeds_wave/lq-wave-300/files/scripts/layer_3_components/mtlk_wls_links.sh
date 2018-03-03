#!/bin/sh
# This is the script set wls links.
return_status=true

# Defines
if [ ! "$MTLK_INIT_PLATFORM" ]; then			
	. /tmp/mtlk_init_platform.sh
fi

print2log DBG "mtlk_wls_links.sh: args: $*"

command=$1

start_wls_links()
{
	print2log DBG "mtlk_wls_links.sh: start"
	ln -s $DRIVER_PATH/mtlk.ko /tmp/mtlk.ko

	cd $IMAGES_PATH
	for bin in `ls`
	do
		ln -s $IMAGES_PATH/$bin /tmp/$bin
	done
	
	# Check if need to work with FW debug bins.
	# if so, create symlinks from /tmp/ to debug bins.
	for bin in `ls ap_upper_*debug.bin sta_upper_*debug.bin contr_lm*debug.bin 2>/dev/null`
	do
		bin_name=`basename $bin`
		rm /tmp/$bin_name
	done
	FWDebugEnable=`host_api get $$ 0 FWDebugEnable`
	if [ "$FWDebugEnable" = "$YES" ]
	then
		print2log ATTENTION "#################################################"
		print2log ATTENTION "########## YOU ARE USING FW DEBUG BINS ##########"
		print2log ATTENTION "#################################################"
		rm /tmp/ap_upper* /tmp/sta_upper_* /tmp/contr_lm*
		for bin in `ls ap_upper_*debug.bin sta_upper_*debug.bin contr_lm*debug.bin`
		do
			bin_short=`basename $bin _debug.bin`
			bin_name=$bin_short.bin
			ln -s $IMAGES_PATH/$bin /tmp/$bin_name
		done
	fi
	cd - > /dev/null
	print2log DBG "mtlk_wls_links.sh: Done Start"
}

stop_wls_links()
{
	print2log DBG "mtlk_wls_links.sh: stop"

	cd /tmp
	rm /tmp/ap_upper*
	rm /tmp/ProgModel_*
	rm /tmp/mtlk.ko
	if [ -e /tmp/sta_upper* ]; then rm /tmp/sta_upper*; fi
	if [ -e /tmp/contr_lm.bin ]; then rm /tmp/contr_lm.bin; fi

	cd - > /dev/null
	
	print2log DBG "mtlk_wls_links.sh: Done stop"
}

create_config_wls_links()
{
	return
}

should_run_wls_links()
{
	return_status=`check_failure wlan0`
}

case $command in
	start)
		start_wls_links
	;;
	stop)
		stop_wls_links
	;;
	create_config)
		create_config_wls_links
	;;
	should_run)
		should_run_wls_links
	;;
esac

$return_status
