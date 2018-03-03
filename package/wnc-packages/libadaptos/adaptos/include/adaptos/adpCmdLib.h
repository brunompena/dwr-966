/* adpCmdLib.h - adaptos command execution library header file */

/* Copyright (c) 2009, TeamF1, Inc. */

/*
modification history
--------------------
01d,15apr09,sdk  added facility to reap a process.
01c,19dec08,pnm  added prototype for adpCmdExecArgv.
01b,20jun06,chd  added declaration for adpCmdExecStatus
01a,12feb06,rnp  created.
*/

#ifndef __INCadpCmdLibH
#define __INCadpCmdLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* defines */
/* two modes to execute a command */
#define ADP_CMD_EXEC        (0)
#define ADP_CMD_SPAWN       (1)

/* two modes to kill a command */
#define ADP_CMD_KILL_WAIT        (0)
#define ADP_CMD_KILL_DONTWAIT    (1)

/* two modes to reap a process */
#define ADP_CMD_REAP_WAIT        (0)
#define ADP_CMD_REAP_DONTWAIT    (1)

/* ADPCMD_ERR codes */
#define ADPCMD_ERR_INVALID_ARGUMENT     (M_adpCmdLib | 0x0)
#define ADPCMD_ERR_NO_TOKENS            (M_adpCmdLib | 0x1)
#define ADPCMD_ERR_NO_MEM               (M_adpCmdLib | 0x2)
#define ADPCMD_ERR_CMD_EXEC             (M_adpCmdLib | 0x3)
#define ADPCMD_ERR_CMD_ERROR            (M_adpCmdLib | 0x4)
#define ADPCMD_ERR_CMD_ABORTED          (M_adpCmdLib | 0x5)
#define ADPCMD_ERR_INVALID_CMDID        (M_adpCmdLib | 0x6)

#define adpCmdSpawn(cmd, output)        adpCmdExec(cmd, output, ADP_CMD_SPAWN)
#define adpCmdSpawnPid                  adpCmdExecStatus

/* exports */
EXPORT STATUS adpCmdExec (char *, char *, int);
EXPORT STATUS adpCmdExecArgv (char **, char *, int);
EXPORT STATUS adpCmdKill (int, int);
EXPORT STATUS adpCmdReap (int);
EXPORT int    adpCmdExecStatus;

#ifdef __cplusplus
}
#endif

#endif /* __INCadpCmdLibH */
