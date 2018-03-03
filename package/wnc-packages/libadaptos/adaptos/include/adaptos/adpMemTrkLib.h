/* adpMemTrkLib.h - memory tracking library */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01a,26jul07,rnp  written
*/

#ifndef __INCadpMemTrkLibh
#define __INCadpMemTrkLibh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* defines */
/* define ADP_MEM_TRACK in your module before it includes adaptos.h */
#ifdef ADP_MEM_TRACK
#ifdef ADP_MEM_TRACK_LITE
#define adpMemPartAlloc(P,s)      adpMemTrkAlloc(P,s)
#define adpMemPartRealloc(P,n,s)  adpMemTrkRealloc(P,n,s)
#define adpMemPartFree(P,p)       adpMemTrkFree(P,p)
#define adpMemPartCalloc(P,n,s)   adpMemTrkCalloc(P,n,s)
#define adpMemPartStrDup(P,s)     adpMemTrkStrDup(P,s)
/* 
 * the use of following mappings is discouraged, because, it uses the system
 * partition. use one of the above mappings instead.
 */ 
#define adpStrDup(S)              adpMemTrkStrDup(NULL,S)
#define adpCalloc(n,s)            adpMemTrkCalloc(NULL,n,s)
#define adpMalloc(s)              adpMemTrkAlloc(NULL,s)
#define adpRealloc(p,s)           adpMemTrkRealloc(NULL,p,s)
#define adpFree(p)                adpMemTrkFree(NULL,p)
#else /* !ADP_MEM_TRACK_LITE */
#define adpMemPartAlloc(P,s)      adpMemTrkAlloc(P,s,__FILE__,__LINE__)
#define adpMemPartRealloc(P,n,s)  adpMemTrkRealloc(P,n,s,__FILE__,__LINE__)
#define adpMemPartFree(P,p)       adpMemTrkFree(P,p,__FILE__,__LINE__)
#define adpMemPartCalloc(P,n,s)   adpMemTrkCalloc(P,n,s,__FILE__,__LINE__)
#define adpMemPartStrDup(P,s)     adpMemTrkStrDup(P,s,__FILE__,__LINE__)
/* 
 * the use of following mappings is discouraged, because, it uses the system
 * partition. use one of the above mappings instead.
 */ 
#define adpStrDup(S)              adpMemTrkStrDup(NULL,S,__FILE__,__LINE__)
#define adpCalloc(n,s)            adpMemTrkCalloc(NULL,n,s,__FILE__,__LINE__)
#define adpMalloc(s)              adpMemTrkAlloc(NULL,s,__FILE__,__LINE__)
#define adpRealloc(p,s)           adpMemTrkRealloc(NULL,p,s,__FILE__,__LINE__)
#define adpFree(p)                adpMemTrkFree(NULL,p,__FILE__,__LINE__)
#endif /* ADP_MEM_TRACK_LITE */
#endif /* ADP_MEM_TRACK */
             
/* adpMemTrkXXX prototypes */
IMPORT STATUS adpMemTrkInit (ADP_PART_ID memPartId);
IMPORT void * adpMemTrkAlloc (ADP_PART_ID memPartId, size_t size, ...);
IMPORT void * adpMemTrkAllocWithTimeout (ADP_PART_ID memPartId, size_t size,
                                         ULONG flags, ULONG timeout, ...);
IMPORT STATUS adpMemTrkFree (ADP_PART_ID memPartId, void * pBlock, ...);
IMPORT void * adpMemTrkRealloc (ADP_PART_ID memPartId, void * pBlock,
                                size_t nBytes, ...);
IMPORT void * adpMemTrkCalloc (ADP_PART_ID memPartId, size_t nelem,
                               size_t size, ...);
IMPORT char * adpMemTrkStrDup (ADP_PART_ID memPartId, const char * str, ...);
IMPORT STATUS adpMemTrkShow (ADP_PART_ID memPartId, int verbose);
#ifdef __cplusplus
}
#endif

#endif /* __INCadpMemTrkLibh */
