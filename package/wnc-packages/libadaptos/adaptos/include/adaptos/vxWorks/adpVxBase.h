/* adpVxBase.h - include files for VxWorks OS */

/* Copyright (c) 2005 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01j,26nov08,rnp  added defines for BIG_ENDIAN and LITTLE_ENDIAN if not defined
01i,04sep07,rnp  corrected ADAPTOS_VXWORKS_54x to ADAPTOS_VXWORKS_54
01h,27jul07,rnp  added adpVxRamFs.h
01g,24aug06,chd  added ethernet.h for vx6x.
01f,02aug06,rc   added support for VxWorks 6.x
01e,20jun06,chd  added support for Vx54.
01d,15nov05,chd  added adpGlob.h
01c,19oct05,smv  added endLib.h
01b,18sep05,rnp  added assert.h
01a,07may05,sam  moved all VxWorks defn. from adaptos.h.
*/

#ifndef __INCadpVxBaseH
#define __INCadpVxBaseH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* base os includes */

#include <vxWorks.h>
#include <ioLib.h>
#include <fcntl.h>
#include <sockLib.h>
#include <memLib.h>
#ifndef ADAPTOS_VXWORKS_54
#include <memPartLib.h>
#endif /* ADAPTOS_VXWORKS_54 */
#ifdef ADAPTOS_VXWORKS_6X
#include <net/ethernet.h>    
#else
#include <netinet/if_ether.h>    
#endif /* ADAPTOS_VXWORKS_6X */
#include <msgQLib.h>
#include <time.h>
#include <utime.h>
#include <selectLib.h>
#include <taskLib.h>
#include <tickLib.h>
#include <taskVarLib.h>
#include <taskHookLib.h>
#include <limits.h>
#include <sys/times.h>
#include <private/taskLibP.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <wdLib.h>
#include <sntpcLib.h>
#include <symLib.h>
#include <semLib.h>
#include <errno.h>
#include <errnoLib.h>
#include <ctype.h>
#include <hostLib.h>
#include <inetLib.h>
#include <logLib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <semaphore.h>
#ifndef ADAPTOS_VXWORKS_54
#include <pthread.h> 
#endif /* ADAPTOS_VXWORKS_54 */
#include <drv/timer/timerDev.h>
#include <netdb.h>
#include <unistd.h>
#include <ptyDrv.h>
#include <assert.h>
#include <endLib.h>    

#define NO_SYS_PARAM_H
#ifndef NO_ASM
#define NO_ASM
#endif   
#define NO_INLINE_ASM
#undef  SSIZE_MAX
#define SSIZE_MAX   _POSIX_SSIZE_MAX

/* define byte-order */
#ifndef BIG_ENDIAN
#define BIG_ENDIAN  _BIG_ENDIAN
#endif
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN  _LITTLE_ENDIAN
#endif
#if     (_BYTE_ORDER == _BIG_ENDIAN)
#define B_ENDIAN
#undef  L_ENDIAN
#ifndef BYTE_ORDER
#define BYTE_ORDER BIG_ENDIAN
#endif
#elif   (_BYTE_ORDER == _LITTLE_ENDIAN)
#undef  B_ENDIAN
#define L_ENDIAN
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#else
#error  "ENDIAN not set"
#endif

#include "adpVxTypes.h"
#include "adpVxMisc.h"
#include "adpGlob.h"
#include "adpVxRamFs.h"

#ifdef __cplusplus
}
#endif

#endif /* __INCadpVxBaseH */
