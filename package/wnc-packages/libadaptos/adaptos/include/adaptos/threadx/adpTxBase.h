/* adpTxBase.h - include files for ThreadX */

/* Copyright (c) 2006 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01c,27mar07,san  added header file from USNET under macro 
                 CONFIG_USNET.
01b,17may06,pkk  adopted from LynxOs and modified for ThreadX
01a,31aug05,rnp  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpTxBaseH
#define __INCadpTxBaseH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <time.h>
#include <signal.h>

#include <tx_api.h>  /* this is from TheadX os */

#if defined (CONFIG_USNET)
#include <types.h>     /* From generic include for HP */
#include "SOCKET.H"    /* USNet specific */
#include "MTMACRO.H"   /* USNet specific */
#include <inline.h>
#include "dhcp.h"      /* USNet specific */ 
#else
#include "nptypes.h" /* this is from GH tcpip stack */
#include "netbuf.h"  /* this is from GH tcpip stack */
#include "ipport.h"  /* this is from GH tcpip stack */
#include "bsdsock.h"
#include "net.h"     /* this is from GH tcpip stack */
#include "ip.h"      /* this is from GH tcpip stack */
#include "socket.h"  /* this is from GH tcpip stack */ 
#include "dns.h"     /* this is from GH tcpip stack */
#include <stdint.h>
#endif /* CONFIG_USNET */

#include <limits.h>
#include "adpTxTypes.h"
#include "adpTxMisc.h"
#include "adpTxNetIf.h"

#ifdef __cplusplus
}
#endif

#endif /* __INCadpTxBaseH */
