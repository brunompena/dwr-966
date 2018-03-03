#!/bin/sh /etc/rc.common
#
# Install Voice TAPI subsystem high level driver


#START=30

KERNEL_VERSION=`uname -r`
KERNEL_MAJOR=`uname -r | cut -f1,2 -d. | sed -e 's/\.//'`

# check for Linux 2.6 or higher
if [ $KERNEL_MAJOR -ge 26 ]; then
	MODEXT=.ko
fi

modules_dir=/lib/modules/${KERNEL_VERSION}
drv_obj_file_name=drv_tapi$MODEXT

start() {
	# load IFXOS if not already loaded
	[ -e ${modules_dir}/drv_ifxos$MODEXT ] &&
	[ -z `cat /proc/modules | grep drv_ifxos` ] && {
		insmod ${modules_dir}/drv_ifxos$MODEXT
	}
	# check for loading the eventlogger driver
	[ -e ${modules_dir}/drv_event_logger$MODEXT ] &&
	[ -z `cat /proc/modules | grep event_logger` ] && {
		insmod ${modules_dir}/drv_event_logger$MODEXT
	}
	[ -e ${modules_dir}/${drv_obj_file_name} ] && {
		insmod ${modules_dir}/${drv_obj_file_name};
 	}
}

