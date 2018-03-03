/* adpErrnoLib.c - adaptos errno library for Integrity */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,08apr08,san  created (based on linux\adpDebug.c).
*/

/*
DESCRIPTION

This module provides adaptos errno routines for Integrity.

INCLUDES: adaptos.h

SEE ALSO: 

*/

/* includes */
#include "adaptos.h"

/* globals */
const char * pAdpErrStr;
int          adpErrno;
char         adpErrBuf[ADP_ERR_BUF_SIZE];
const char * adpSysOpStrTbl[] = ADP_SYSOP_STRINGS;
const UINT   adpSysOpStrTblSize = sizeof(adpSysOpStrTbl);

/*******************************************************************************
*
* adpErrnoSet - set the error value
*
* This routine sets the error value.
*
* RETURNS: ST_OK on success, ERROR otherwise
*/
STATUS adpErrnoSet
    (
    int         errValue   /* error value to be set */
    )
    {
    errno = errValue;
    adpErrno = errValue;
    return (ST_OK); 
    }

/*******************************************************************************
*
* adpErrnoGet - get the error value
*
* This routine gets the error value.
*
* RETURNS: Error value
*/
int adpErrnoGet (void)
    {
    return errno;
    }

/*******************************************************************************
*
* adpErrStrSet - set the error string
*
* This routine sets the error string.
*
* RETURNS: ST_OK on success, ERROR otherwise
*/
STATUS adpErrStrSet
    (
    const char * pErrStr   /* error string to be set */
    )
    {
    pAdpErrStr = pErrStr;
    return (ST_OK); 
    }

/*******************************************************************************
*
* adpErrStrGet - get the error string
*
* This routine gets the error string.
*
* RETURNS: Error string
*/
const char * adpErrStrGet (void)
    {
    if (pAdpErrStr == NULL)
        {
        snprintf(adpErrBuf, ADP_ERR_BUF_SIZE, "0x%08x", adpErrno);
        pAdpErrStr = adpErrBuf; 
        }
    return pAdpErrStr;
    }

/*******************************************************************************
*
* adpErrnoClear - clear the error value
*
* This routine clears the error value.
*
* RETURNS: N/A
*/
void adpErrnoClear (void)
    {
    errno = 0;
    }

/*******************************************************************************
*
* adpErrStrClear - clear the error string
*
* This routine clears the error string.
*
* RETURNS: N/A
*/
void adpErrStrClear(void)
    {
    pAdpErrStr = "ADPOK";
    }

/*******************************************************************************
*
* adpErrnoStr - get the errno string
*
* This routine gets the errno string.
*
* RETURNS: Errno string
*/
const char * adpErrnoStr (void)
    {
    return strerror(errno);
    }

/*******************************************************************************
*
* adpPanic - stop processing
*
* This function suspends the execution of the current process.
*
* RETURNS: N/A
*/
void adpPanic (void)
    {
    abort();   
    }
