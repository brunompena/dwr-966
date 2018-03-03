#!/bin/sh /etc/rc.common
START=99

CPUFREQROOT="/sys/devices/system/cpu/cpu0/cpufreq"

start() {
	if [ A"$CONFIG_PACKAGE_KMOD_LANTIQ_CPUFREQ_SCALING_ENA" = "A1" ]; then 
		if [ -f $CPUFREQROOT/ltq_stats/ltq_control ]; then
			echo on/ > $CPUFREQROOT/ltq_stats/ltq_control
			echo performance > $CPUFREQROOT/scaling_governor
			echo ondemand > $CPUFREQROOT/scaling_governor
			echo "CPU Frequency Scaling Enabled"
		fi
	fi
}

stop() {
	if [ A"$CONFIG_PACKAGE_KMOD_LANTIQ_CPUFREQ_SCALING_ENA" = "A1" ]; then 
		if [ -f $CPUFREQROOT/ltq_stats/ltq_control ]; then
			echo performance > $CPUFREQROOT/scaling_governor
			echo off/ > $CPUFREQROOT/ltq_stats/ltq_control
			echo "CPU Frequency Scaling Disabled"
		fi
	fi
}
