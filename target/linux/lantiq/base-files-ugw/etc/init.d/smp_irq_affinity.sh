#!/bin/sh /etc/rc.common

START=998

        if [ ! "$CONFIGLOADED" ]; then
        	if [ -r /etc/rc.d/config.sh ]; then
                	. /etc/rc.d/config.sh 2>/dev/null
                	CONFIGLOADED="1"
        	fi
        fi


start(){
	if [ -n "$CONFIG_PACKAGE_KMOD_SMVP" -a "$CONFIG_PACKAGE_KMOD_SMVP" = "1" ]; then
		# irq assigned to CPU0
		for irq_name in dma-core ltqusb2_oc
		do
			grep -w $irq_name /proc/interrupts | awk '{print $1}' | sed  's/://' | while read irq ;do echo 1 > /proc/irq/$irq/smp_affinity; done
		done

		# irq assigned to CPU1
		for irq_name in  mtlk ltqusb_hcd_1 ltqusb_hcd_2 ltqusb1_oc a5_mailbox0_isr a5_mailbox_isr d5_mailbox_isr e5_mailbox_isr DFEIR wifi0 wifi1
		do
			grep -w $irq_name /proc/interrupts | awk '{print $1}' | sed  's/://' | while read irq ;do echo 2 > /proc/irq/$irq/smp_affinity; done
		done
	fi
}
