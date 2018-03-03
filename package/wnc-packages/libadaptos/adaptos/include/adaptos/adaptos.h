/* adaptos.h -  adaptos process library header */

/* Copyright (c) 2002 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01v,02jan08,rnp  renamed ADAPTOS_VERSION as TF1_VERSION_ADAPTOS(reset it to 1)
                 removed adpSupportLib.h
01u,07dec07,rnp  added adpPasswd.h
01t,06oct07,san  added adpBuffIoTypes.h & adpKliteGlobalMap.h.
01s,24jul07,rnp  added adpDaemonize.h
                 added adpMemTrkLib.h and placed adpListLib.h earlier in the
                 include order to allow use in adpMemLib.h
01r,12jul07,rnp  added adpU64Struct.h
01q,05jul07,rnp  removed 6X code(defines for snprintf and vsnprintf)
01p,19aug06,chd  moved adpShmLib.h to adpLnxBase.h
01o,11aug06,rnp  added adpShmLib.h
01n,02aug06,rc   added support for VxWorks 6.x
01m,20jun06,chd  added support for Vx6x.
01l,05sep05,rnp  added adpTgtosXXX header files
                 added ADAPTOS_VERSION
01k,07may05,sam  restructured adaptos.
01j,09mar05,uge  added OS independent changes
01i,20aug03,gnm  added stddef.h for psos 2x
01h,16aug03,ksn  added sys/times.h for vxWorks.
01g,02may03,ksn  removed adpNetLib.h
01f,08mar03,ksn  added support for vxWorks
01e,08jul02,mno  added pSOS-2x support
01d,12jun02,mno  added include files -- pna.h, sys/socket.h for pSOS
01c,06jun02,mk   added min() and max()
01b,01jun02,mno  added os specific includes; cleanups
01a,08may02,vnk  created.
*/

#ifndef __INCadaptosH
#define __INCadaptosH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* version of define must appear right at the beginning */
/* hex. version */
#define TF1_VERSION_ADAPTOS 0x00000001

/* base os includes */
#include "adpTgtosBase.h"

#if defined(CONFIG_ADP_KLITE_GLOBAL_MAP)
#include "adpKliteGlobalMap.h"
#endif /* CONFIG_ADP_KLITE_GLOBAL_MAP */

/* adaptos includes */
#include "adpU64Struct.h"
#include "adpListLib.h"
#ifdef CONFIG_BUFFER_IO 
#include "adpBuffIoTypes.h"
#endif /* CONFIG_BUFFER_IO */     
#include "adpIoTypes.h"
#include "adpTypes.h"   
#include "adpStdLib.h"
#include "adpModCodes.h"
#include "adpModDebug.h" 
#include "adpMemLib.h"
#include "adpCryptoApi.h"
#include "adpErrnoLib.h"
#include "adpHostLib.h"
#include "adpIoLib.h"
#include "adpMsgQLib.h"
#include "adpNetdb.h"
#include "adpNetLib.h"
#include "adpRandLib.h"
#include "adpRealPath.h"
#include "adpSemLib.h"
#include "adpSocketIoLib.h"
#include "adpStat.h"
#include "adpSysLog.h"
#include "adpTimerLib.h"
#include "adpTaskLib.h"
#include "adpGetOpt.h"
#include "adpCmdLib.h"
#include "adpEtherLib.h"
#include "adpDaemonize.h"
#include "adpMemTrkLib.h"
#include "adpPasswd.h"

/* os includes for adaptos compatibility */
#include "adpTgtosCompat.h"

#ifdef __cplusplus
}
#endif

#endif /* __INCadaptosH */
