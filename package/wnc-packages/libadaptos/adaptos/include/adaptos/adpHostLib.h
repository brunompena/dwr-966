/* adpHostLib.h -  adaptos header file to abstract OS dependent host routines */

/* Copyright (c) 2004, TeamF1, Inc. */

/*
modification history
--------------------
01a,18aug04,rks  created.
*/

#ifndef __INCadpHostLibH
#define __INCadpHostLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include <adaptos.h>


/* prototypes */
IMPORT int adp_gethostname (char * name, int len);

#ifdef __cplusplus
}
#endif

#endif /* __INCadpHostLibH */
