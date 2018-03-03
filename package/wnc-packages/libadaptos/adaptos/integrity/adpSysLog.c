/* adpSysLog.c - unix style system log facility */
   
/* Copyright (c) 2005 - 2008, TeamF1, Inc. */

/*
modification history
--------------------

01g,23apr08,san  adopted from adaptos\thredax.
01f,05sep05,rnp  removed VXWORKS references
01e,30jul03,ksn  added comments
01d,18jun03,gnm  code cleanups: added module description, added function
                 descriptions to all functions.
01c,17jun03,gnm  changed copyright information, changed module file name at top
                 of this module from syslog.c to adp_syslog.c
01b,02jun02,mno  ported to adaptos framework
01a,08nov00,mno  created.
*/

/*
DESCRIPTION

Thsi module implements the logging routines for the adaptos framework.

INCLUDES: syslog.h

SEE ALSO:
*/

/* includes */

#include "adaptos.h"

/* globals */

LOCAL char *priorityNames[] =
    {
        "EMERGENCY",                         /* LOG_EMERG */
        "ALERT",                             /* LOG_ALERT */
        "CRITICAL",                          /* LOG_CRIT */
        "ERROR",                             /* LOG_ERR */
        "WARNING",                           /* LOG_WARNING */
        "NOTICE",                            /* LOG_NOTICE */
        "INFO",                              /* LOG_INFO */
        "DEBUG"                              /* LOG_DEBUG */
    };

/* name of facility for which log has to ge generated */
LOCAL char *facilityNames[] =
    {
        "Kernel",
        "User",
        "Mail",
        "Daemon",
        "Auth",
        "Syslog",
        "Lpr",
        "News",
        "Uucp",
        "Cron",
        "AuthPriv",
        "Ftp",
        "Unknown0",
        "Unknown1",
        "Unknown2",
        "Unknown3",
        "Local0",
        "Local1",
        "Local2",
        "Local3",
        "Local4",
        "Local5",
        "Local6",
        "Local7",
        "None"
    };

/*******************************************************************************
*
* openlog - dummy routine
*
* This is a dummy implementation of openlog
*
* RETURNS: ST_OK in all cases. 
*/

int openlog
    (
    char *  ident,
    int     option,
    int     facility
    )
    {
    return (ST_OK);
    }

/*******************************************************************************
*
* closelog - dummy routine
*
* This is a dummy implementation of closelog
*
* RETURNS: N/A.
*/
void closelog()
    {
    return;
    }

/*******************************************************************************
*
* syslog - log a formatted error message
*
* This routine logs a specified message via the logging task.
*
* RETURNS: N/A.
*/
void syslog
    (
    int     pri,
    char *  fmt,
    ...
    )
    {
    va_list ap;
    char    buffer[512];
    char *  pBuf;
    int     facilityIndex;
    int     priorityIndex;

    facilityIndex=(pri >> 3);
    if (facilityIndex > LOG_NFACILITIES)
        facilityIndex = LOG_NFACILITIES;

    priorityIndex=(pri & 0x7);
    
    sprintf (buffer, "(%s) %s: ",
             facilityNames[facilityIndex],
             priorityNames[priorityIndex]);

    pBuf = buffer;
    pBuf += strlen(pBuf);

    va_start(ap, fmt);
    vsprintf (pBuf, fmt, ap);
    va_end(ap);
       
    printf("%s\n",buffer); 
    }
