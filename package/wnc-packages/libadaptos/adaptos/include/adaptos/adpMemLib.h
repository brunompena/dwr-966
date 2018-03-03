/* adpMemLib.h -  adaptos memory partition library header */

/* Copyright (c) 2002 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01m,12sep07,rnp  changed ADP_SYS_MEM_PART to adpSysMemPart
01l,26jul07,rnp  added memory-tracking variables to ADP_PARTITION
                 added ADP_SYS_MEM_PART prototype
                 added prototypes for adpMemPartCalloc and adpMemPartStrDup
01k,17sep05,rnp  changed type of ADP_PART_ID to structure pointer
01j,07may05,sam  restructured adaptos.
01i,16aug03,ksn  fixed adpMemPartDelete prototype, and removed adpBzero
01h,16jun03,gnm  changed adpMemPartFree declaration: return type
                 changed from int to STATUS
01g,20apr03,ksn  modified ADP_MEM_ERR_MEM to S_adpMemLib_NO_MEM
01f,08mar03,ksn  fixed ADP_MEM_PART_ID_OS for vxWorks.
01e,27dec02,mk   added adpBezro
01d,12jun02,mno  added preset memory partition IDs
01c,07jun02,mno  fixed adpMemPartCreate API
01b,06jun02,mno  cleanup.
01a,08may02,vnk  created.
*/

#ifndef __INCadpMemLibH
#define __INCadpMemLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* error codes */

#define ADP_MEM_OK          (0)
#define ADP_MEM_ERROR       (-1)
#define S_adpMemLib_NO_MEM  (M_adpMemLib | 1)

#define ADP_MEM_TIMEOUT_WAIT_FOREVER    (-1)

/* flags for adpMemPartAllocWithTimeout() */

#define ADP_MEM_WAIT            (1)
#define ADP_MEM_NO_WAIT         (0)

/* typedefs */

#ifndef HAVE_ADP_PART_ID
typedef struct 
    {
    void *    adpPartId;
    void *    adpPartMemPool;
    int       maxMemUsage;
    int       curMemUsage;
    int       curOverHead;
    ADP_LIST  adpMemList;
    int       memCounter; /* number of nodes in list */
    } ADP_PARTITION;

typedef ADP_PARTITION * ADP_PART_ID;
#define HAVE_ADP_PART_ID
#endif

/* exports */
IMPORT ADP_PARTITION adpSysMemPart;

/* prototypes */
IMPORT void adpBzero (void *pBuf, size_t len);
/* adpMemPartXXX prototypes */
IMPORT ADP_PART_ID adpMemPartCreate (char * pName,  char * pMemPool,
                                     ULONG partSize);
IMPORT STATUS adpMemPartDelete (ADP_PART_ID memPartId);
IMPORT void * adpMemPartAlloc (ADP_PART_ID memPartId, ULONG size);
IMPORT void * adpMemPartAllocWithTimeout (ADP_PART_ID memPartId, ULONG size,
                                          ULONG flags, ULONG timeout);
IMPORT STATUS adpMemPartFree (ADP_PART_ID  memPartId, void * pBlock);
IMPORT void * adpMemPartRealloc (ADP_PART_ID memPartId, void * pBlock,
                                 ULONG nBytes);
IMPORT STATUS adpMemPartDestroy (ADP_PART_ID memPartId);

IMPORT void * adpMemPartCalloc (ADP_PART_ID memPartId, size_t nelem,
                                size_t size);
IMPORT char * adpMemPartStrDup (ADP_PART_ID memPartId, const char * str);

#ifdef __cplusplus
}
#endif

#endif /* __INCadpMemLibh */
