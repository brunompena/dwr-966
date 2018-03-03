/* adpErrnoLib.h - adaptos error library header file */

/* Copyright (c) 2009, TeamF1, Inc. */

/*
modification history
--------------------
01f,15apr09,sdk  added facility to reap a process.
01e,06jul07,rnp  removed trailing comma after ADP_RECV to avoid warning
01d,02aug06,sam  added File lock.
01c,10oct05,sam  added adpPanic
01b,07may05,sam  restructured adaptos.
01a,15apr03,ksn  created.
*/

#ifndef __INCadpErrnoLibH
#define __INCadpErrnoLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include <adaptos.h>
    
/* defines */
#define ADP_RET_ERROR(errCode, errLabel)                             \
        do {                                                         \
            adpErrnoSet(errCode);                                    \
            adpErrStrSet(NULL);                                      \
            if ((errCode & 0xFFFF) < errLabel##StrTbl##Size)         \
                adpErrStrSet(errLabel##StrTbl[errCode & 0xFFFF]);    \
            ADP_PRINTF("At %s():%s:%d -> %s\n",                      \
                       __FUNC__, __FILE__, __LINE__,adpErrStrGet()); \
            goto errLabel;                                           \
        } while(0)

#define ADP_RET_SYSTEM_ERROR(sysCode, errLabel)                 \
        do {                                                    \
            adpErrStrSet("System");                             \
            if ((sysCode & 0xFFFF) < adpSysOpStrTblSize)        \
                adpErrStrSet(adpSysOpStrTbl[sysCode & 0xFFFF]); \
            ADP_PRINTF("At %s():%s:%d -> %s(%s)\n", __FUNC__,   \
                       __FILE__, __LINE__,                      \
                       adpErrStrGet(), adpErrnoStr());          \
            goto errLabel;                                      \
        } while(0)

#define ADP_RET_NESTED_ERROR(errLabel)                    \
        do {                                              \
            ADP_PRINTF("At %s():%s:%d -> Nested Error\n", \
                       __FUNC__, __FILE__, __LINE__);     \
            goto errLabel;                                \
        } while(0)

#define ADP_RET_OK(okLabel)                                                   \
        do {                                                                  \
            adpErrnoClear();                                                  \
            adpErrStrClear();                                                 \
            ADP_PRINTF("At %s():%s:%d -> OK\n", __FUNC__, __FILE__,__LINE__); \
            goto okLabel;                                                     \
        } while(0)

#define ADP_DEFINE_ERROR_TABLE(errLabel, errorStrings)            \
    const char * errLabel##StrTbl[] = errorStrings;               \
    const UINT errLabel##StrTbl##Size = sizeof(errLabel##StrTbl);

#define ADP_DECLARE_ERROR_TABLE(errLabel)     \
    IMPORT const char * errLabel##StrTbl[];   \
    IMPORT const UINT errLabel##StrTbl##Size;

#define ADP_ERR_BUF_SIZE (256)

#define ADP_SYSOP_STRINGS \
    {                     \
    "File Open",          \
    "File Read",          \
    "File Write",         \
    "I/O Control",        \
    "Process Fork",       \
    "Program Exec",       \
    "Wait on Process",    \
    "Process Kill",       \
    "Process Reap",       \
    "Local System Time",  \
    "File Number",        \
    "File Scan",          \
    "Socket Open",        \
    "File lock",          \
    "File seek",          \
    "Failed to open shared memory segment",       \
    "Failed to write to shared memory segment",   \
    "Failed to read from shared memory segment",  \
    "Failed to send message through socket",        \
    "Failed to receive message through socket",     \
    }

/* typedefs */
/* adp system opcodes */
typedef enum adpSysOpCodes
    {
    ADP_FOPEN = 0,
    ADP_FREAD,
    ADP_FWRITE,
    ADP_IOCTL,
    ADP_FORK,
    ADP_EXEC,
    ADP_WAIT,
    ADP_KILL,
    ADP_REAP,
    ADP_LOCALTIME,
    ADP_FILENO,
    ADP_FSCANF,
    ADP_SOCKET,
    ADP_FLOCK,
    ADP_LSEEK,
    ADP_SHM_OPEN_FAIL,
    ADP_SHM_WRITE_FAIL,
    ADP_SHM_READ_FAIL,
    ADP_SEND,
    ADP_RECV
    } ADP_SYSOP_CODES;

/* exports */
IMPORT STATUS         adpErrnoSet (int);
IMPORT int            adpErrnoGet (void);
IMPORT void           adpErrnoClear (void);
IMPORT STATUS         adpErrStrSet (const char *);
IMPORT const char *   adpErrStrGet (void);
IMPORT void           adpErrStrClear (void);
IMPORT const char *   adpErrnoStr (void);
IMPORT void           adpPanic (void);
IMPORT const char *   adpSysOpStrTbl[];
IMPORT const UINT     adpSysOpStrTblSize;

/* adpErr global variables */
EXPORT const char * pAdpErrStr;
EXPORT int          adpErrno;

#ifdef __cplusplus
}
#endif

#endif /* __INCadpErrnoLibH */
