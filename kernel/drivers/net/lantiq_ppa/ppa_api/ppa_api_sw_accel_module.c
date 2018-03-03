/*******************************************************************************
**
** FILE NAME    : ppa_api_sw_accel_module.c
** PROJECT      : PPA
** MODULES      : PPA API (Module Wrapper for Software Acceleration)
**
** DATE         : 13 Mar 2014
** AUTHOR       : Lantiq
** DESCRIPTION  : WRAPPER FUNCTIONS TO MAKE SOFTWARE ACCELERATION A LODABLE 
**   		  KERNEL MODULE. ALL CODE AVAILABLE IN SOFTWARE ACCELERATION 
**		  MODULE IS LANTIQ PROPRIETARY AND NON GPL.
**
** COPYRIGHT    :              Copyright (c) 2013
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
*******************************************************************************/

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/atmdev.h>
#include <net/sock.h>

/*
 *  Chip Specific Head File
 */
#include <net/ppa_api.h>
#include "ppa_api_session.h"
#include "ppa_api_netif.h"
#include "ppa_api_misc.h"
#include "ppe_drv_wrapper.h"
#include <net/ppa_ppe_hal.h>
#include "../platform/ppa_datapath.h"
#include "ppa_api_sw_accel.h" 

extern int ppa_sw_init(void);
static int __init ppa_sw_init_local(void)
{
	ppa_sw_init();
}
extern void ppa_sw_exit(void);
static void __exit ppa_sw_exit_local(void)
{
	ppa_sw_exit();
}

module_init(ppa_sw_init_local);
module_exit(ppa_sw_exit_local);

MODULE_LICENSE("GPL");
