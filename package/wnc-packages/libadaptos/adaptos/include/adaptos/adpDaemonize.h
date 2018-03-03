/* adpDaemonize.h - API to daemonize a program */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01a,20jul07,rnp  written
*/

/*
DESCRIPTION
This library provides API to daemonize a program
*/

/* includes */

#include <adaptos.h>

/*******************************************************************************
*
* adpDaemonize - puts program into background(daemonizes it)
* 
* On Linux this API will invoke daemon(0,0) and thereafter invoke the daemon
* entry point <pEntry>
*
* On VxWorks, this API will spawn a new task with <pEntry> as the entry point.
* Options to control the spawned task can be set in adpDaemonOpt before calling
* adpDaemonize()
*
* The <args> will typically be local stack variables that are needed in the
* <pEntry> routine. If there are pointers on the local stack, they must point
* valid heap memory and cannot point to stack memory. For ex: you cannot pass an
* array on the stack as a pointer argument to adpDaemonize().
*
* This API is implemented as a macro, to allow the use of variable number of
* arguments. Atleast 1 argument must be specified.
* 
* RETURNS: N/A
*
* NOMANUAL
*/
#ifdef HAVE_ADP_DAEMON_API
#define adpDaemonize(pEntryRtn, args...) \
    do { \
        daemon(0,0); \
        /* set_proc_title(pName); */ \
        pEntryRtn(args); \
    } while(0)
#endif /* HAVE_ADP_DAEMON_API */

#ifdef HAVE_ADP_DAEMON_TASK
#ifdef ADAPTOS_VXWORKS_DIAB
#define adpDaemonize(pEntryRtn, ...) \
    do { \
        /* this array will ensure taskSpawn() will access valid stack mem */ \
        int dummy[10]; \
        /* dummy statement to avoid warning */ \
        dummy[0] = adpDaemonOpt.vxoptions; \
        adpDaemonTask(adpDaemonOpt.name, adpDaemonOpt.vxpriority, \
                  adpDaemonOpt.vxoptions, adpDaemonOpt.vxstacksize, \
                  (FUNCPTR)pEntryRtn, __VA_ARGS__); \
    } while(0)
#else /* !ADAPTOS_VXWORKS_DIAB */
#define adpDaemonize(pEntryRtn, args...) \
    do { \
        /* this array will ensure taskSpawn() will access valid stack mem */ \
        int dummy[10]; \
        /* dummy statement to avoid warning */ \
        dummy[0] = adpDaemonOpt.vxoptions; \
        adpDaemonTask(adpDaemonOpt.name, adpDaemonOpt.vxpriority, \
                  adpDaemonOpt.vxoptions, adpDaemonOpt.vxstacksize, \
                  (FUNCPTR)pEntryRtn, ##args); \
    } while(0)
#endif /* ADAPTOS_VXWORKS_DIAB */
IMPORT int adpDaemonTask (char * pName, ULONG priority, ULONG options, 
                          ULONG stackSize, ADP_FUNCPTR pEntryRtn, ...);
#endif /* HAVE_ADP_DAEMON_TASK */

#ifndef HAVE_ADP_DAEMON_OPT
#define ADP_DAEMON_NAME_SIZE            (10)
#define ADP_DAEMON_DEFAULT_VXPRIO       (100)
#define ADP_DAEMON_DEFAULT_VXOPTIONS    (ADP_TASK_OPT_FP)
#define ADP_DAEMON_DEFAULT_VXSTACKSIZE  (8192)

typedef struct adpDaemonOpt
    {
    char name[ADP_DAEMON_NAME_SIZE];
    int  vxpriority;
    int  vxoptions;
    int  vxstacksize;
    } ADP_DAEMON_OPT;

IMPORT ADP_DAEMON_OPT adpDaemonOpt;
#define HAVE_ADP_DAEMON_OPT
#endif /* HAVE_ADP_DAEMON_OPT */
