/* adpLynUnMap.h - unmap list for LynxOS */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,31aug05,rnp  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpLynUnMapH
#define __INCadpLynUnMapH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define adpMalloc(x)                        malloc(x)
#define adpCalloc(a,b)                      calloc(a,b)
#define adpFree(x)                          free(x)
#define adpMemPartCreate(a,b,c)             ((ADP_PART_ID)1)
#define adpMemPartDestroy(a)                ((ADP_PART_ID)1)
#define adpMemPartDelete(a)                 ((ADP_PART_ID)1)
#define adpMemPartAlloc(a,b)                malloc(b)
#define adpMemPartFree(a,b)                 (free(b),0)
#define adpMemPartRealloc(a,b,c)            realloc(b,c)
#define adpMemPartAllocWithTimeout(a,b,c,d) malloc (b)   

/* 
 * LynxOS 3.1.0 has a broken definition which causes compiler warnings.
 * Following is the correct definition which overrides the errneous one.
 */
#ifdef PTHREAD_MUTEX_INITIALIZER
#undef PTHREAD_MUTEX_INITIALIZER
#define PTHREAD_MUTEX_INITIALIZER \
    { \
    __MUTEX_MAGIC|__MUTEX_P_PINH|__MUTEX_T_NORM, \
    0, \
    {0}, \
    0, \
    0, \
    0, \
    0}
#endif /* PTHREAD_MUTEX_INITIALIZER */

/* 
 * LynxOS 3.1.0 still not POSIX regarding the spelling of the following macro.
 * Hence the need for the following #define (POSIX has only one L).
 */
#ifndef PTHREAD_CANCELED
#define PTHREAD_CANCELED PTHREAD_CANCELLED
#endif /* PTHREAD_CANCELED */

#ifdef __cplusplus
}
#endif

#endif /* __INCadpLynUnMapH */
