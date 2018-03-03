#!/bin/sh

#/etc/rc.d/invoke_upgrade.sh image image_type expand saveenv reboot &
#Modify the LED display before decryp imaige file
echo 0 > /sys/class/leds/power/brightness
echo "timer" > /sys/class/leds/power_error/trigger
echo 500 > /sys/class/leds/power_error/delay_on
# Added ap fw encrypted
mv -f $1 /tmp/og4610_fw.img
rm /mnt/data/upfwlog
mv /mnt/data/upfwlog1 /mnt/data/upfwlog
echo "version information===============================================================" >> /mnt/data/upfwlog
version.sh >> /mnt/data/upfwlog

sync
echo 3 > /proc/sys/vm/drop_caches
sleep 2
date >> /mnt/data/upfwlog
echo "before decryt image file===============================================================" >> /mnt/data/upfwlog
ls -al /tmp/og* >> /mnt/data/upfwlog
ls -al /tmp/fullima* >> /mnt/data/upfwlog
ls -al /tmp/cgi* >> /mnt/data/upfwlog
ls -al /tmp/*.dwn >> /mnt/data/upfwlog
ps | grep cgi >> /mnt/data/upfwlog
cat /proc/meminfo >> /mnt/data/upfwlog 

cd /tmp && ./ap_fw_upgrade.sh og4610_fw.img
sync
echo 3 > /proc/sys/vm/drop_caches
sleep 2
date >> /mnt/data/upfwlog
echo "after decryt image file===============================================================" >> /mnt/data/upfwlog
ls -al /tmp/og* >> /mnt/data/upfwlog
ls -al /tmp/fullima* >> /mnt/data/upfwlog
ls -al /tmp/cgi* >> /mnt/data/upfwlog
ls -al /tmp/*.dwn >> /mnt/data/upfwlog
ps | grep cgi >> /mnt/data/upfwlog
cat /proc/meminfo >> /mnt/data/upfwlog

rm /tmp/og4610_fw.img
sync
echo 3 > /proc/sys/vm/drop_caches
sleep 2

CheckResult=`cat /tmp/ap_fw_check`
if [ "_$CheckResult" = "_1" ]; then
	echo "FW check failed! Abort!"
	#LED display
	echo 0 > /sys/class/leds/power_error/brightness
	echo 1 > /sys/class/leds/power/brightness
	date >> /mnt/data/upfwlog
	echo "FW check failed! Abort!" >> /mnt/data/upfwlog
	exit
fi

date >> /mnt/data/upfwlog
echo "FW check PASS!" >> /mnt/data/upfwlog

#LED display
echo 0 > /sys/class/leds/power_error/brightness
echo 1 > /sys/class/leds/power/brightness

UpgradeTarget=`cat /tmp/ap_fw_target`
if [ "_$UpgradeTarget" = "_1" ]; then
	echo "@@@ cpt fw"
	UpgradePara="/tmp/fullimage.img fullimage 1 saveenv reboot"
else
	echo "@@@ img fw"
	UpgradePara="/tmp/og4610_fw.img fullimage 1 saveenv reboot"
fi

if [ "_$7" = "_webui" ]; then
CheckWebui=`cat /tmp/ap_fw_webui`
while [ "_$CheckWebui" = "_255" ]
do
        sleep 1
        CheckWebui=`cat /tmp/ap_fw_webui`
done
fi

echo "@@@ UpgradePara=[$UpgradePara] @@@"
if [ "$5" = "reboot" ]; then
	. /etc/rc.d/free_memory.sh
	killall br2684ctld >&- 2>&-
	killall syslogd >&- 2>&-
fi

count=`pidof malmanager | wc -w`
if [ $count -ne 0 ]; then
  lte_set -O 0x03
fi

echo "timer" > /sys/class/leds/power/trigger
echo "timer" > /sys/class/leds/power_error/trigger
echo 500 > /sys/class/leds/power/delay_on
echo 500 > /sys/class/leds/power/delay_off
echo 500 > /sys/class/leds/power_error/delay_on
echo 500 > /sys/class/leds/power_error/delay_off

date >> /mnt/data/upfwlog
echo "start to upgrade FW===============================================================" >> /mnt/data/upfwlog
ls -al /tmp/og* >> /mnt/data/upfwlog
ls -al /tmp/fullima* >> /mnt/data/upfwlog
ls -al /tmp/cgi* >> /mnt/data/upfwlog
ls -al /tmp/*.dwn >> /mnt/data/upfwlog
ps | grep cgi >> /mnt/data/upfwlog
cat /proc/meminfo >> /mnt/data/upfwlog

cd /mnt/data/ && {
#	upgrade $@ &
	upgrade $UpgradePara &
	sed -i 's/^config.patch = .*/config.patch = \"true\"/g' /sysconfig/jnr-cfg.ascii
}
echo "===================FW upgrade done!!!============================================" >> /mnt/data/upfwlog
