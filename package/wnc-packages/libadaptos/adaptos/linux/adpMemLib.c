/* adpMemLib.c - adaptos memory allocation library for linux */

/* Copyright (c) 2006 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01c,12sep07,rnp  changed ADP_SYS_MEM_PART to adpSysMemPart
01b,26jul07,rnp  removed all memory debugging routines and added adpMemPartXXX
                 routines
01a,01mar06,rnp  created.
*/

/*
DESCRIPTION

This library provides for all the memory allocation debugging related support
for linux.

INCLUDES: adaptos.h

SEE ALSO: 

*/
/* includes */
#include "adaptos.h"

/* defines */
#undef malloc
#undef free
#undef realloc
#undef adpMemPartAlloc
#undef adpMemPartRealloc
#undef adpMemPartFree

/* globals */
ADP_PARTITION adpSysMemPart={NULL, NULL};

/*******************************************************************************
*
* adpMemPartCreate - create a new memory partition
*
* Linux doesnt have memory partitions. So we always uses system-wide memory
* partition
*
* RETURNS: The partition Id, or NULL on failure
*/
ADP_PART_ID adpMemPartCreate
    (
    char *  pName,      /* name of the region */
    char *  pMemPool,   /* pointer to the memory pool */
    ULONG   partSize    /* size of the partition (in bytes) */
    )
    {
    return &adpSysMemPart;
    }

/*******************************************************************************
*
* adpMemPartAlloc - allocate a block of memory from a particular partition
*
* This routine allocates a block of memory from a partition. The partition to
* allocate the block from is specified by the argument <memPartId>.
*
* RETURNS: Pointer to block, or NULL on failure
*/
void * adpMemPartAlloc
    (
    ADP_PART_ID     memPartId,  /* partition ID */
    ULONG           size        /* size of block (in bytes) */
    )
    {
    return (malloc(size));
    }

/*******************************************************************************
*
* adpMemPartFree - free a block of memory from a particular partition
*
* This routine can be used to reclaim a block of memory from a partition
* previously allocated via a call to adpMemPartAlloc(). This block of memory
* is added to the partition's free memory list.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/
STATUS adpMemPartFree
    (
    ADP_PART_ID     memPartId,  /* partition ID */
    void *          pBuf        /* pointer to block of memory to free */
    )
    {
    free(pBuf);
    return ST_OK;
    }

/*******************************************************************************
*
* adpMemPartRealloc - reallocate a block of memory from a particular partition
*
* This routine can be used to change the size of a specified block of memory
* in a partition. The partition Id and the block Id are values returned by
* earlier calls to adpMemPartCreate() and adpMemPartAlloc() respectively. The
* contents of the block remain unchanged if the size specified by the argument
* <nBytes> is greated or same as the earlier block size.
*
* RETURNS: Pointer to a new block of memory, or NULL on failure
*/
void * adpMemPartRealloc
    (
    ADP_PART_ID     memPartId,  /* partition Id */
    void *          pBuf,       /* pointer to block */
    ULONG           nBytes      /* size of new block (in bytes) */
    )
    {
    return (realloc(pBuf, nBytes));
    }

/*******************************************************************************
*
* adpMemPartDestroy - destroys a memory partition
*
* This routine destroys a memory partition created using adpMemPartCreate()
*
* RETURNS: ST_OK or ERROR
*/
STATUS adpMemPartDestroy
    (
    ADP_PART_ID     memPartId  /* partition Id */
    )
    {
    /* nothing to destroy */
    return ST_OK;
    }

/*******************************************************************************
*
* adpMemPartShow - displays a memory partition
*
* This routine displays a memory partition.
*
* RETURNS: ST_OK or ERROR
*/
STATUS  adpMemPartShow
    (
    ADP_PART_ID     memPartId,   /* partition Id */
    int             verbose      /* verbose */
    )
    {
    /* nothing to show */
    return ST_OK;
    }
