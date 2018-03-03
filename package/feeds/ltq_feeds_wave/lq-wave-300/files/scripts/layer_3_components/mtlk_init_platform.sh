#!/bin/sh

# This script sets platform specific settings for the UGW platforms.

if [ ! "$CONFIGLOADED" ]; then
	if [ -r /etc/rc.d/config.sh ]; then
		. /etc/rc.d/config.sh 2>/dev/null
		CONFIGLOADED="1"
	fi
fi

export RWFS_PATH=/mnt/jffs2
export ROOT_PATH=/root/mtlk
export ETC_PATH=/etc/rc.d
export IMAGES_PATH=/root/mtlk/images
export DRIVER_PATH=/lib/modules/$(uname -r)/net
export WEB_PATH=/root/mtlk/web
export CONFIGS_PATH=/ramdisk/flash
export BCL_PATH=/root/mtlk/bcl
export BINDIR=/bin
export SUPPLICANT_PARAMS_PATH=/tmp/supplicant_params.sh
export WAVE_MAP_CPEID=/tmp/wave_map_index
export WAVE_MAP_SECTION=wave_map
export WAVE_NEXT_SECTION=wave_next
export STATUS_OPER=/usr/sbin/status_oper

export log_level=1
#log_path="/dev/console"
# Uncomment to turn on timestamp based profiling:
#export WLAN_PROFILING=1

export AP=2
export VAP=3
export STA=0

#STATIC=1
export DHCPC=0

#VIDEO_BRIDGE=0
#LAN_PARTY=1

export BAND_5G=0
export BAND_24G=1
export BOTH_BAND=2


export ENABLE=1
export DISABLE=0
export YES=1
export NO=0

export MacCloning=3
export L2NAT=2
export FourAddresses=1
export ThreeAddresses=0

export open=1
export wep=2
export WPA_Personal=3
export WPA_Enterprise=4

export Enrollee=0
export Wirless_registrar=1
export Configured_AP=2
export Unconfigured_AP=1
export Ap_Proxy_registrar=1

export SSID_CHANGED=1
export SSID_NOT_CHANGED=0

export IF_UP=1
export IF_DOWN=0

export HOSTAPD_EVENTS_SCRIPT=$ETC_PATH/wave_wlan_hostapd_events
export HOSTAPD_PIN_REQ=/var/run/hostapd.pin-req
export WPS_PIN_TEMP_FILE=/tmp/wps_current_pin
export WPS_MAC_TEMP_FILE=/tmp/wps_current_mac
export LED_VARS_FILE=/tmp/wlan_leds_scan_results
export CLI_WPS_IN_PROCESS=/tmp/cli_wps_in_process

export wave_init_failure=/tmp/wave_init_failure
export wave_start_failure=/tmp/wave_start_failure
export wave_init_success=/tmp/wave_init_success

export WAVE_VENDOR_NAME=LANTIQ

export GENL_FAMILY_ID_FILE=/tmp/mtlk_genl_family_id_file

# Start the led manager on the dongle
#LEDMAN=0

# Dongle has no pushbutton for WPS
#WPS_PB=1

# Don't use lower memory usage - use default net queues
#LOWER_MEM_USAGE=1


# Don't mangle multicast packets (this is done on STAR to reduce CPU utilization)
#MULTICAST_MANGLING=0

# GPIO pins used for output LEDs
#GPIO_LEDS=0,3
# GPIO pins used for pushbuttons
#GPIO_PUSHBUTTONS=1,13

# TODO: This info is also available from HW.ini so maybe remove here, and use host_api get to read
# 1 = trigger on PBC release
export pbc_wps_trigger=1

print2log()
{
    case $1 in
	INFO)
		#if [ $log_level -ge 2 ]; then logger -t INFO "$2"; fi
		if [ $log_level -ge 2 ]; then echo "$2" > /dev/console ; fi
	;;
	DBG)
		#if [ $log_level -ge 3 ]; then logger -t DBG "$2"; fi
		if [ $log_level -ge 3 ]; then echo "$2" > /dev/console ; fi
	;;
	*)
		#logger -t $1 "$2"
		echo $1 $2 > /dev/console
	;;
    esac
}

# Get WPS on/off state: 0 off, 1 on.
# This is an optimization that reads wps_state (off/unconfigure/configured) from the config file to reduce host_api overhead.
# host_api is only called if the config file doesn't exist yet.
# To save ap index translation overhead, both the index and wlan interface name are passed as args. Ugly but works :-(
# TODO: This optimization is for AP, what about STA? Currently it will fall back to using host_api like before.
get_wps_on()
{
	apIndex=$1
	wlan=$2
	wps_on=$NO
	wps_state=`awk -F '=' '/wps_state/ {print $2}' $CONFIGS_PATH/hostapd_$wlan.conf`
	if [ -z $wps_state ]
	then
		wps_on=`host_api get $$ $apIndex NonProc_WPS_ActivateWPS`
	else
		if [ $wps_state != 0 ]
		then
			wps_on=$YES
		fi
	fi
	echo $wps_on
}

# timestamp function for profiling.
# Results added to:  /tmp/wlanprofiling.log
timestamp()
{
	if [ -z "$WLAN_PROFILING" ]
	then
		return
	fi
	SECS=`date +%s`
	if [ -n $1 ]
	then
		PREFIX="[$1]"
	fi
	echo ${PREFIX}${SECS} >> /tmp/wlanprofiling.log
}

# Converts ascii to hex
ascii2hex()
{
	ascii_X=$1
	ascii_LEN=`echo "${#ascii_X}"`
	i=0
	while [ $i -lt $ascii_LEN ]
	do
		ascii_char=${ascii_X:$i:1}
		printf '\\x%02x' "'$ascii_char" | sed 's/00/20/'
		let i=i+1
	done
}

# Get interface name from index
find_wave_if_from_index()
{
	apIndex=$1
	cpeId=`$STATUS_OPER -f $CONFIGS_PATH/rc.conf GET "wlan_main" "wlmn_${apIndex}_cpeId"`
	wlan=`$STATUS_OPER -f $WAVE_MAP_CPEID GET "$WAVE_MAP_SECTION" "wlmap_$cpeId"`
	echo $wlan
}

# Get index from interface name
find_index_from_wave_if()
{
	wlan=$1
	cpeId=`cat $WAVE_MAP_CPEID | grep wlmap_.*$wlan\" | cut -d "=" -f 1 | cut -d "_" -f2`
	# Safe read from rc.conf
	# Create script to run in order to read from rc.conf
	echo "#!/bin/sh" > /tmp/find_index_for_if_$$
	echo "apIndex=\`cat $CONFIGS_PATH/rc.conf | grep wlmn_.*_cpeId=\\\"${cpeId}\\\" | cut -d "_" -f2\`" >> /tmp/find_index_for_if_$$
	echo 'echo $apIndex' >> /tmp/find_index_for_if_$$
	chmod +x /tmp/find_index_for_if_$$
	# Lock rc.conf and execute script
	apIndex=`/usr/sbin/syscfg_lock $CONFIGS_PATH/rc.conf "/tmp/find_index_for_if_$$"`
	rm /tmp/find_index_for_if_$$
	echo $apIndex
}

# Check if the wlan interface is ready (eeprom/calibration file exist and ifconfig is working)
# If the interface is up, return 0, else return 1.
check_if_is_ready()
{
	wlan=$1
	parent_if=`echo $wlan | cut -d "." -f 1`
	# Check if the hw_wlan.ini file exists for this interface, file is created if eeprom/calibration file exist.
	if [ ! -e $CONFIGS_PATH/hw_$parent_if.ini ]
	then
		print2log DBG "mtlk_init_platform.sh: hw_$parent_if.ini is missing"
		echo 1
		return
	fi
	# Check if the wlan interface already exists.
	ifconfig_status=`ifconfig $wlan`
	if [ $? -ne 0 ]
	then
		print2log DBG "mtlk_init_platform.sh: $wlan interface not ready."
		echo 1
		return
	fi
	echo 0
}

# Check if failure files exist
check_failure()
{
	wlan=$1	
	if [ -e $wave_init_failure ] || [ -e /tmp/wave_${wlan}_start_failure ]
	then
		echo false
	else
		echo true
	fi
}

error_message()
{
	error_type=$1
	if [ "$error_type" = "FW_CRASH" ]
	then
		echo "ERROR API execution: $error_type error" > /dev/console
	fi
}

# wrap_file
# validate complete file configuration
# Assume:
#   dummy API at the end of file. It gives the time to get fw crash indication from driver on file send.
# Detect FW Crash and resend file if found one.
# params format:
#   $1= script
# stages before send file:
#   entry param: /tmp/set_driver_params_X.sh (where X=0/1...N)
#   index=X: remove file extension ".sh" and all before index, save it
#   read FW Crash count before
#   execute file /tmp/set_driver_params_X.sh
#   test for FW Crash: read FW Crash count and compare to previous value
wrap_file()
{
	FILE=$1
	ret=0
	count_errors=0

	# delete file extension (.sh), then remove all that is before '_' include, leave only index:
	# example: /tmp/set_driver_params_11.sh gives 11
	index=`echo $FILE | sed -r -e s/.sh// -e s/.+_//`

	# If VAP - get the parent interface (save it to phy_if)
	index_wlan=`find_wave_if_from_index $index`
	phy_if=`echo $index_wlan | cut -d "." -f 1`

	# In file wrapper must read numFwFails before exec of file
	fw_fails_before=`host_api get $$ $phy_if numFwFails`

	#execute file and read if error on last command:
	$FILE
	fw_fails=`host_api get $$ $phy_if numFwFails`

	if [ "$fw_fails" != "$fw_fails_before" ]
	then
		# Error occurred - initialize params needed for retries
		api_resend_max=`host_api get $$ $phy_if FwRecovery | awk '{sum=$1+$2} {print sum}'`

		# Is retry allowed?
		if [ "$api_resend_max" = "0" ]; then error_message "UNKNOWN"; return 1; fi
	fi

	while [ "$fw_fails" != "$fw_fails_before" ]
	do
		let count_errors=$count_errors+1
		if [ $count_errors -gt $api_resend_max ]
		then
			# Max Count ERROR
			error_message FW_CRASH
			ret=1
			break
		fi
		fw_fails_before=`host_api get $$ $phy_if numFwFails`

		#execute file again:
		$FILE

		fw_fails=`host_api get $$ $phy_if numFwFails`
	done
	return $ret
}

# wrap_param: handle param get/set requests
# Assume:
#	- no double quote on comamnd in the scripts
# $*= set/get command...
# stages before send command (example: ifconfig command):
#	set ifconfig wlan0 down - params
#	"ifconfig wlan0 down"	 - after remove type
wrap_param()
{
	##**********************************************#
	# TIME CRITICAL AREA							#
	TYPE=$1
	# Remove TYPE
	shift
	# Run command and test for error:
	ret=`eval "$*"`
	error=$?

	if [ "$error" != "0" ]
	then
		##**********************************************#
		# NON-TIME CRITICAL AREA						#
		# Send to handle error and API status
		ret=`wrap_error_handler $*`
		error=$?
	fi

	##**********************************************#
	# TIME CRITICAL AREA							#
	# Return data from API call:
	if [ "$TYPE" = "get" ] && [ "$error" = "0" ]; then echo "$ret"; fi
	return $error
}

# wrap_error_handler - handler API error
# Note- it regards wlan0 to always exist ! if command is not interface related, it uses wlan0 to get config using host_api 
wrap_error_handler()
{
	count_errors=0
	ret_error=0

	#################################################################
	# PRE-STATE														#
	#################################################################
	# One time initialization to do when error occurred:
	error_cause=NO_ERROR
	interface=$2
	# Delete vap extension, leave physical interface
	phy_if=`echo $interface | awk -F "." '{print $1}'`

	# Detect if interface given or a generic API, by checking if arg2 contains wlanX, 
	# then get FW Crash value before exec API, save it and compare to after API sent
	per_interface=`echo $phy_if | grep wlan`
	if [ -z "$per_interface" ]
	then
		# phy_if is empty - set it for other host_api execution
		phy_if=wlan0
		fw_fails_before=`host_api get $$ $phy_if numFwFails`
		num_interfaces=`host_api get $$ sys wlan_count`
		if [ "$num_interfaces" = "2" ]
		then
			fw_fails_before_if1=`host_api get $$ wlan1 numFwFails`
		fi
	else
		fw_fails_before=`host_api get $$ $phy_if numFwFails`
	fi
	# Read max retries (max=fast+full recovery limits), exit if equal current resends
	api_resend_max=`host_api get $$ $phy_if FwRecovery | awk '{sum=$1+$2} {print sum}'`
	# Config to first state
	sm_state=SEND_API
	if [ "$api_resend_max" == "0" ]
	then
		sm_state=DONE_ERROR_HANDLING
		ret_error=1
	fi

	#################################################################
	# STATE MACHINE - FW RECOVERY ERROR HANDLING					#
	#################################################################
	while [ true ]
	do
		case $sm_state in
		SEND_API)
			let count_errors=$count_errors+1
			ret=`eval "$*"`
			error=$?
			# Config to next state according to command status
			sm_state=DONE_ERROR_HANDLING
			if [ $error -gt 0 ]; then sm_state=RE_SEND_API; fi
		;;
		RE_SEND_API)
			let count_errors=$count_errors+1
			ret=`eval "$*"`
			error=$?
			# Config to next state according to command status
			sm_state=DONE_ERROR_HANDLING
			if [ $error -gt 0 ]; then sm_state=FW_CRASH_DETECT; fi
		;;
		FW_CRASH_DETECT)
			fw_crash=0
			## TEST: ################################
			#let inc=$inc+1
			#host_api set $$ $phy_if numFwFails $inc
			## TEST END #############################
			fw_fails=`host_api get $$ $phy_if numFwFails`
			if [ "$fw_fails_before" != "$fw_fails" ]; then fw_crash=1; fi
			
			if [ ! -z "$fw_fails_before_if1" ]
			then
				fw_fails_if1=`host_api get $$ wlan1 numFwFails`
				if [ "$fw_fails_before_if1" != "$fw_fails_if1" ]; then fw_crash=1; fi
			fi
				
			# Config to next state according to command status
			sm_state=MAX_RETRY_DETECT
			if [ "$fw_crash" = "0" ]
			then
				# Configuration error detected. Exit, make sure to report error type before
				error_cause=CONFIGURATION
				ret_error=1
				sm_state=DONE_ERROR_HANDLING
			fi
		;;
		MAX_RETRY_DETECT)
			# Save last read fails for next iteration
			fw_fails_before=$fw_fails
			fw_fails_before_if1=$fw_fails_if1
			# Config to next state according to command status
			sm_state=SEND_API
			if [ $count_errors -gt $api_resend_max ]
			then
				# Exit, make sure to report error type before
				error_cause=FW_CRASH
				ret_error=1
				sm_state=DONE_ERROR_HANDLING
			fi
		;;
		DONE_ERROR_HANDLING)
			# Report status
			error_message $error_cause
			break
		;;
		esac
	done

	# Return data from API call:
	echo "$ret"
	return $ret_error
}

# General on wrapper flow:
# command format: <type> <command> [<wlan interface/index> <processing>]
#   type=get/set/file, command=iwpriv/ifconfig etc. or set_driver_params_X.sh
#   command=iwpriv/ifconfig etc. or set_driver_params_X.sh [<wlanX/X>]
#   interface=wlanX
#   processing= | grep "string", etc. (processing only for 'get' type)
# stages over command handling:
#   handle=api_wrapper: "set ifconfig wlan0 down", save type
#          remove type from command line and send command to wrap_param().
#   handle=wrap_param: "ifconfig wlan0 down", get index from interface for use by handler,
#          exec command and handle error accordingly.
api_wrapper()
{
	#keep type for later use
	TYPE=$1
	#remove first parameter - type
	shift
	#execute handler according to command type
	if [ "$TYPE" = "file" ]
	then
		wrap_file $*
	else
		wrap_param $TYPE $*
	fi
	ret=$?
	return $ret
}

MTLK_INIT_PLATFORM="1"
