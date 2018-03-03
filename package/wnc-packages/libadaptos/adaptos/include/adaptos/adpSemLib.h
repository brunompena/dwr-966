/* adpSemLib.h -  adaptos semaphore library header */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01f,07may05,sam  restructured adaptos.
01e,19apr03,ksn  modified error macros.
01d,08mar03,ksn  added support for vxWorks.
01c,09aug02,mno  added adpSem APIs
01b,06jun02,mno  fixed adpMutexCreate API
01a,08may02,vnk  created.
*/

#ifndef __INCadpSemLibH
#define __INCadpSemLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* error codes */

#define ADP_SEM_OK      (0)
#define ADP_SEM_ERROR       (-1)
#define S_adpSemLib_NO_MEM  (M_adpSemLib | 1);

#define ADP_SEM_NAME_MAX    4

#define ADP_MUTEX_Q_FIFO    0x0000
#define ADP_MUTEX_Q_PRIORITY    0x0001
#define ADP_MUTEX_DEL_SAFE  0x0004
#define ADP_MUTEX_INV_SAFE  0x0008

#define ADP_MUTEX_WAIT_FOREVER  0

#define ADP_MUTEX_NO_WAIT   0x0001
#define ADP_MUTEX_WAIT      0x0002

#define ADP_SEM_Q_FIFO      0x0000
#define ADP_SEM_Q_PRIORITY  0x0001
#define ADP_SEM_DEL_SAFE    0x0004

#define ADP_SEM_WAIT_FOREVER    0

#define ADP_SEM_NO_WAIT     0x0001
#define ADP_SEM_WAIT        0x0002

#define ADP_SEM_EMPTY       0x0
#define ADP_SEM_FULL        0x1
/* typedefs */

#ifndef HAVE_ADP_MUTEX_ID
typedef ULONG ADP_MUTEX_ID;
#define HAVE_ADP_MUTEX_ID
#endif

#ifndef HAVE_ADP_SEM_ID
typedef ULONG ADP_SEM_ID;
#define HAVE_ADP_SEM_ID
#endif

/* prototypes */

IMPORT ADP_MUTEX_ID adpMutexCreate (char *pName, ULONG options);
IMPORT int adpMutexDelete (ADP_MUTEX_ID mutexId);
IMPORT int adpMutexTake (ADP_MUTEX_ID mutexId, ULONG flags, ULONG timeout);
IMPORT int adpMutexGive (ADP_MUTEX_ID mutexId);
IMPORT ADP_SEM_ID adpSemBCreate (char *name, ULONG options, BOOL pFull);
IMPORT int adpSemBDelete (ADP_SEM_ID);
IMPORT int adpSemTake (ADP_SEM_ID semId, ULONG options, ULONG timeout);
IMPORT int adpSemGive (ADP_SEM_ID semId);

#ifdef __cplusplus
}
#endif

#endif /* __INCadpSemLibH */
