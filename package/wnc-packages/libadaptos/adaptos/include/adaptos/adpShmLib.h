/* adpShmLib.h - declarations and definitions for shared memory library. */

/* Copyright (c) 2006, TeamF1, Inc. */

/*
modification history
--------------------
01a,10aug06,smv written.
*/

/*
DESCRIPTION

*/

#ifndef __INCadpShmLibH
#define __INCadpShmLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */

#include <adaptos.h>


/* defines */

/* default values */
#define ADP_SHM_PROJ_ID           1
#define ADP_SHM_CREAT_FLAGS      (IPC_CREAT)
#define ADP_SHM_PERM             (0644)
#define ADP_SHM_SIZE             (1024 * 4)

/* typedefs */

/* locals */

/* globals */

/* imports */

IMPORT void adpShmErrDisplay (int status);
IMPORT ADP_KEY_T adpShmKeyGet (const char * pFilePath, int projId);
IMPORT int adpShmIdGet (ADP_KEY_T sysVIpcKey, int segSize, int shmFlags);
IMPORT void * adpShmAttach (int shmId, const void * pShmAddr, int shmFlags);
IMPORT int adpShmDetach (const void * pShmAddr);

#ifdef __cplusplus
}
#endif

#endif /* __INCadpShmLibH */
