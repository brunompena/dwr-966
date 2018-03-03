/* adpDaemonize.c - API to daemonize a program */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01a,20jul07,rnp  written
*/

/*
DESCRIPTION
This library provides API to daemonize a program

INCLUDE FILES: adpDaemonize.h
*/

/* includes */

#include <adaptos.h>

/* defines */

/* typedefs */

/* externs */

/* locals */

/* globals */
ADP_DAEMON_OPT adpDaemonOpt =
    {
    {0}, ADP_DAEMON_DEFAULT_VXPRIO, ADP_DAEMON_DEFAULT_VXOPTIONS,
    ADP_DAEMON_DEFAULT_VXSTACKSIZE
    };

#ifdef HAVE_ADP_DAEMON_TASK
int adpDaemonTask
    (
    char *      pName,          /* task name */
    ULONG       priority,       /* task priority */
    ULONG       options,        /* task options */
    ULONG       stackSize,      /* task stack size */
    ADP_FUNCPTR pEntryRtn,      /* task entry point */
    ...
    )
    {
    int i = 0;
    va_list args;
    ULONG arg[10];
    va_start (args, pEntryRtn);
    for (i = 0; i < 10; i++)
        arg[i] = va_arg(args, ULONG);
    va_end (args);
    taskSpawn(pName, priority, options, stackSize, pEntryRtn,
              arg[0], arg[1], arg[2], arg[3], arg[4],
              arg[5], arg[6], arg[7], arg[8], arg[9]);

    return 0;
    }
#endif /* HAVE_ADP_DAEMON_TASK */
