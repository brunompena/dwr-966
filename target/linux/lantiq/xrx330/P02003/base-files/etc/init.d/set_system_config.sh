#!/bin/sh /etc/rc.common

#START=07

#
# This routine configures memory tuning parameters
#
TuneMemoryParameters()
{

  cmdLine=`cat /proc/cmdline`
  cmdLine=${cmdLine#*'mem='}
  ramSize=${cmdLine%%'M '*}
  [ ${#ramSize} -lt 0 ] && MEM=64 # Error checking. Default MEM is 64
  
  min_free_kbytes=1024 # Default minimum free bytes

  [ $ramSize -gt 100 ] && { # for 128M and above
 	if [ -n "$CONFIG_PACKAGE_KMOD_SMVP" -a "$CONFIG_PACKAGE_KMOD_SMVP" = "1" ]; then
		min_free_kbytes=8192
	else
    	min_free_kbytes=4096
	fi
  }


  echo ${min_free_kbytes} > /proc/sys/vm/min_free_kbytes
  # memory tunning for all platform lowmem_reserve_ratio is a MUST
  echo 250 >  /proc/sys/vm/lowmem_reserve_ratio
  echo 2 > /proc/sys/vm/dirty_background_ratio
  echo 250 > /proc/sys/vm/dirty_writeback_centisecs
  echo 10 > /proc/sys/vm/dirty_ratio
  echo 16384 > /proc/sys/vm/max_map_count
#  echo 2 > /proc/sys/vm/page-cluster
#  echo 70 > /proc/sys/vm/swappiness

  #
  # Following parameters ensure that user space applications are not allowed to
  # consume more than 95% of the system RAM and when system is heavily used &
  # under low memory, instead of OOM, user space application requesting for more
  # momory will suffer.
  #   
  echo 1 > /proc/sys/vm/scan_unevictable_pages
#  echo 95 > /proc/sys/vm/overcommit_ratio
  echo 0 > /proc/sys/vm/overcommit_memory
}

#
# This routine configures the network parameters
#
TuneNetworkParameters()
{
  echo 4096 > /proc/sys/net/ipv4/route/max_size

  # Optimize Routing & Conntrack cache - needs individual tuning based on model
  if [ "$CONFIG_FEATURE_LOW_FOOTPRINT" = "1" ]; then
    echo 512 > /proc/sys/net/netfilter/nf_conntrack_max
    echo 50 > /proc/sys/net/netfilter/nf_conntrack_expect_max
  else
    echo 4096 > /proc/sys/net/netfilter/nf_conntrack_max
    echo 100 > /proc/sys/net/netfilter/nf_conntrack_expect_max
  fi

}

start()
{
  TuneMemoryParameters
  TuneNetworkParameters

  # WAN specific configuration to be made available before doing WAN startup
  # Can't be put in DSL startup script since it doesn't exist on all models
  # Can't be put in generic WAN startup script, since DSL starts before WAN and might trigger WAN start before WAN startup script
  # names are misnomer !
  /usr/sbin/status_oper SET "wan_link_status" "link_up_once" "0"
}
