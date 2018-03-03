/* adpTimerLib.h -  adaptos Timer library header */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01d,09sep05,rnp  removed vxworks/psos references
01c,19apr03,ksn  modified error macros
01b,01jun02,mno  added clock and tickGet; cleanups.
01a,08may02,vnk  created.
*/

#ifndef __INCadpTimerLibH
#define __INCadpTimerLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "adaptos.h"

/* error codes */
#define ADP_TIMER_OK        0
#define ADP_TIMER_ERROR     -1
#define S_adpTimer_NO_MEM   (M_adpTimerLib, 1);
#define TIME_STR_SIZE       128

/* typedefs */
#ifndef HAVE_ADP_TIME_SPEC
typedef struct timespec ADP_TIME_SPEC;
#define HAVE_ADP_TIME_SPEC
#endif

#ifndef HAVE_ADP_TIMER_NODE
typedef struct adpTimerNode
    {
    ADP_NODE        node;
    ADP_TIME_SPEC   ts;
    ADP_FUNCPTR     funcptr;
    int             arg;
    } ADP_TIMER_NODE;
#define HAVE_ADP_TIMER_NODE
#endif

#ifndef HAVE_ADP_TIMER_ID
typedef ADP_TIMER_NODE *  ADP_TIMER_ID;
#endif

/* prototypes */
STATUS adpTimerStart (ADP_TIMER_ID timerId, ADP_TIME_SPEC ts,
                      ADP_FUNCPTR funcptr, int arg);

ADP_TIMER_ID    adpTimerCreate (void);
STATUS          adpTimerCancel (ADP_TIMER_ID timerId);
void            adpTimerDelete (ADP_TIMER_ID    timerId);
STATUS          adpTimeSet (int year, int month, int day, 
                        int hour, int mins, int sec);
int             adpTicksPerSec (void);
ULONG           adpTickGet32 (void);
ULONG           adpClock (void);

#ifdef __cplusplus
}
#endif

#endif /* __INCadpTimerLibh */


