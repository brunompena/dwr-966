/* adpLnxUnMap.h - unmap list for Linux OS */

/* Copyright (c) 2005 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01c,26jul07,rnp  removed ADP_MEM_DEBUG related code.
                 removed adpMemPartXXX macros(they are now routines)
                 obsoleted adpMemPartDelete and adpMemPartAllocWithTimeout
01b,31aug05,rnp  typecast the Create and Delete macros
                 removed adpTaskSuspend and adpTaskIdSelf defines
01a,07may05,sam  moved mappings from adpMemLib.h.
*/

#ifndef __INCadpLnxUnMapH
#define __INCadpLnxUnMapH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef ADP_MEM_TRACK
/* define these mappings only if memory tracking is not enabled */
#define adpMalloc(x)                        malloc(x)
#define adpCalloc(a,b)                      calloc(a,b)
#define adpRealloc(a,b)                     realloc(a,b)
#define adpStrDup(a)                        strdup(a)
#define adpFree(x)                          free(x)
#endif /* ADP_MEM_TRACK */
#ifdef FIMPLE
/* These are probably obsolete */
#define adpMemPartDelete(a)                 ((ADP_PART_ID)1)
#define adpMemPartAllocWithTimeout(a,b,c,d) adpMalloc (b)   
#endif /* FIMPLE */

#ifdef __cplusplus
}
#endif

#endif /* __INCadpLnxUnMapH */
