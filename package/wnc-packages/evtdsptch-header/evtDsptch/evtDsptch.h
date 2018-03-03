/* evtDsptch.h - definitions for the event dispatcher library */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01b,11spe07,sdk added EVTDSPTCH_IFDEV_EVENT.
01a,21jul07,rks written.
*/

/*
DESCRIPTION
This file has definitions for implementation of the event dispatcher library.

INCLUDE FILES:
*/

#ifndef __INCevtDsptchh
#define __INCevtDsptchh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */

/* defines */

#define TBLNAME_MAX_LEN  64

/* typedefs */

typedef struct
    {
    char        tblName[TBLNAME_MAX_LEN];
    long long   rowId;    
    int         opCode;
    } EVTDSPTCH_DB_UPDATE_REQ;
    
enum
    {
    EVTDSPTCH_DB_UPDATE_EVENT   = UMI_PRIVATE_IOCTL_START,
    EVTDSPTCH_IFDEV_EVENT,
    EVTDSPTCH_DB_UPDATE_END
    };


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevtDsptchh */

