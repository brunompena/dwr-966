/* adpIntUnMap.h - unmap list for Integrity OS */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,08apr08,san  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpIntUnMapH
#define __INCadpIntUnMapH

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

#endif /* __INCadpIntUnMapH */
