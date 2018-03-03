#!/bin/sh

# Defines
if [ ! "$MTLK_INIT_PLATFORM" ]; then			
	. /tmp/mtlk_init_platform.sh
fi

. /$CONFIGS_PATH/rc.conf 2> /dev/null

command=$1

drvhlpr_dut="drvhlpr_dut"
TMP=/tmp

start_dut_helper()
{
	print2log DBG "wave_wlan_dut_drvctrl.sh: Start DUT Helper"
	echo "f_saver = $ETC_PATH/wave_wlan_dut_file_saver.sh" > $TMP/$drvhlpr_dut.config
	if [ ! -e $TMP/$drvhlpr_dut ]
	then
		ln -s $BINDIR/drvhlpr $TMP/$drvhlpr_dut
	fi
	$TMP/$drvhlpr_dut --dut -p $TMP/$drvhlpr_dut.config &
	print2log DBG "wave_wlan_dut_drvctrl.sh: Start DUT Helper Done"
}

stop_dut_helper()
{
	print2log DBG "wave_wlan_dut_drvctrl.sh: Stop DUT Helper"
	killall $drvhlpr_dut
	print2log DBG "wave_wlan_dut_drvctrl.sh: Stop DUT Helper Done"
}

start_dut_drvctrl()
{
	print2log DBG "wave_wlan_dut_drvctrl.sh: Start"
	(. $ETC_PATH/mtlk_insmod_wls_driver.sh start dut)
	start_dut_helper
	touch /tmp/dut_mode_on
	print2log DBG "wave_wlan_dut_drvctrl.sh: Start done"
}

first_stop_dut_drvctrl()
{
    print2log DBG "wave_wlan_dut_drvctrl.sh: First Stop"
    # We have to stop all the wave AP/VAPs
    eval ap_count='$'wlan_main_Count
    j=`expr $ap_count - 1`
    while [ $j -ge 0 ]
    do
        echo "stop: j = $j, ap_num = $ap_count"
        eval radioCpeId='$'wlmn_$j'_radioCpeId'
        if [ "$radioCpeId" = "1" ]; then
            radioPrefix=0
        elif [ "$radioCpeId" = "2" ]; then
           radioPrefix=1
        fi
        #do if wlss_X_prefixScript rc.conf param is 'wave':
        eval vendor_prefix='$'wlss_$radioPrefix'_prefixScript'
        if [ "$vendor_prefix" = "wave" ]; then
           print2log DBG "wave_wlan_dut_drvctrl.sh: Stoping Wave interface $j"
           /etc/rc.d/wave_wlan_stop $j
        fi
        j=`expr $j - 1`
    done

    $ETC_PATH/mtlk_insmod_wls_driver.sh stop
    print2log DBG "wave_wlan_dut_drvctrl.sh: First Stop done"
}

stop_dut_drvctrl()
{
	print2log DBG "wave_wlan_dut_drvctrl.sh: Stop"
	if [ -e /tmp/dut_mode_on ]; then rm /tmp/dut_mode_on; fi
	$ETC_PATH/mtlk_insmod_wls_driver.sh stop
	stop_dut_helper
	print2log DBG "wave_wlan_dut_drvctrl.sh: Stop done"
}

case $command in
	start )
		start_dut_drvctrl
	;;
	stop )
		if [ -e /tmp/dut_mode_on ]
		then
			stop_dut_drvctrl
		else
			first_stop_dut_drvctrl
		fi
	;;
	start_helper ) 	
		start_dut_helper	
	;;
	stop_helper )		
		stop_dut_helper	
	;;
	* )
		print2log WARNING "wave_wlan_dut_drvctrl.sh: Unknown command=$command"
    ;;
esac
