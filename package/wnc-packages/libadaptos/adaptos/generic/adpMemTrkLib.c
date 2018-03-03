/* adpMemTrkLib.c - adaptos memory allocation tracking library */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01b,12sep07,rnp  added null-checks for filename
                 added adpMemSysPartGet()
01a,26jul07,rnp  written
*/

/*
DESCRIPTION

This library provides for all the memory allocation tracking

INCLUDES: adaptos.h

SEE ALSO: 

*/
/* includes */
#include "adaptos.h"

/* defines */
#undef adpMemPartAlloc
#undef adpMemPartFree
#undef adpMemPartRealloc

/* globals */

/* externs */

/* typedefs */
typedef struct adpMemNode
    {
    ADP_NODE    slNode;
    int         ctxId;
    void *      ptr;
    int         size;
    char *      fileName;
    int         lineNumber;
    int         tstamp;
    } ADP_MEM_NODE;

/*******************************************************************************
*
* adpMemTrkInit - initialize memory tracking
*
* This routine initializes memory tracking information
* 
* RETURNS: ST_OK on success and ERROR otherwise.
*/
STATUS adpMemTrkInit
    ( 
    ADP_PART_ID     memPartId   /* partition ID */
    )
    {
    adpListInit (&memPartId->adpMemList);
    memPartId->curMemUsage = 0;
    memPartId->maxMemUsage = 0;
    memPartId->curOverHead = 0;
    memPartId->memCounter = 0;
    return (ST_OK);
    }

/*******************************************************************************
*
* adpMemSysPartGet - initialize memory tracking
*
* This routine initializes memory tracking information
* 
* RETURNS: ST_OK on success and ERROR otherwise.
*/
ADP_PART_ID adpMemSysPartGet (void)
    {
    /* 
     * adpSysMemPart.adpPartId is initialized here, because ADP_MEM_PART_ID_OS
     * is not a compile-time constant and so the initialization cant be
     * performed during compilation. This is also the main reason for the
     * existence of this routine.
     */ 
    if (adpSysMemPart.adpPartId == NULL)
        adpSysMemPart.adpPartId = ADP_MEM_PART_ID_OS;
    return &adpSysMemPart;
    }
/*******************************************************************************
*
* adpMemTrkShow - show memory tracking info.
*
* This routine displays information on all allocated blocks that includes the
* task ID, a ptr to the allocated block, its size, as well as the file and line
* number where that block was allocated
*
* RETURNS: ST_OK
*/
STATUS adpMemTrkShow
    ( 
    ADP_PART_ID     memPartId,  /* partition ID */
    int             verbose
    )
    {
    ADP_MEM_NODE *cursor;
    int i = 1;

    if (memPartId == NULL)
        memPartId = adpMemSysPartGet();

    if (verbose)
        {
        for (cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_FIRST(&memPartId->adpMemList);
             (cursor != NULL);
             cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_NEXT((ADP_NODE *)cursor))
            {
            printf("%8d#[%s:%d]<%d>: %d bytes at %p\n",
                   i, cursor->fileName, cursor->lineNumber, cursor->tstamp,
                    cursor->size, cursor->ptr);
            i++;
            }
        }

    printf("curMemUsage: %d, maxMemUsage: %d, curOverHead: %d, "
            "memCounter: %d\n",
            memPartId->curMemUsage, memPartId->maxMemUsage,
            memPartId->curOverHead, memPartId->memCounter);
    
    return (ST_OK);
    }

/*******************************************************************************
*
* adpMemTrkAlloc - allocated memory which will be tracked
* 
* This is allocates memory which will be tracked.
*
* RETURNS: a pointer to the allocate block and NULL on failure.
*/
void * adpMemTrkAlloc
    ( 
    ADP_PART_ID     memPartId,  /* partition ID */
    size_t          size,       /* size of block (in bytes) */
    ...
    )
    {
    void *ptr;
    char *file = "(unknown file)";
    int line = 0;

#ifndef ADP_MEM_TRACK_LITE
    va_list arg;
    va_start (arg, size);
    file = va_arg (arg, char *);
    if (file == NULL)
        file = "(null file)";
    line = va_arg (arg, int);
    va_end (arg);
#endif /* ADP_MEM_TRACK_LITE */

    if (memPartId == NULL)
        memPartId = adpMemSysPartGet();

    ptr = adpMemPartAlloc (memPartId, size);
    /* Add new node corresponding to ptr to list */ 
    if (ptr)
        {
        ADP_MEM_NODE *pNode = NULL;
        pNode = adpMemPartAlloc (memPartId, sizeof(ADP_MEM_NODE));
        if (pNode == NULL)
            {
            adpMemPartFree (memPartId, ptr);
            return NULL;
            }

        /* increment mem usage by size of ptr allocated */
        memPartId->curMemUsage += size; 
        /* increment mem usage by size of list node */
        memPartId->curOverHead += sizeof(ADP_MEM_NODE);  

        if (memPartId->curMemUsage > memPartId->maxMemUsage)
            memPartId->maxMemUsage = memPartId->curMemUsage;

        pNode->ctxId = adpTaskIdSelf();
        pNode->ptr = ptr;
        pNode->size = size;
        pNode->fileName = file;
        pNode->lineNumber = line;
        pNode->tstamp = time(0);

#ifdef FIMPLE
        /* maintain stats such as multiple allocations from same location */
        for (cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_FIRST(&memPartId->adpMemList);
             ((cursor != NULL) && (cursor->ptr != ptr));
             cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_NEXT((ADP_NODE *)cursor))
            {
            continue;
            }
#endif /* FIMPLE */

        adpListNodeAppend (&memPartId->adpMemList, (ADP_NODE *)pNode);
        ++(memPartId->memCounter);
        }

#ifdef ADP_MEM_TRACK_VERBOSE
    fprintf (stderr, "adpMemTrkAlloc(%s:%d): %d bytes alloc'ed at %p\n",
             file, line, size, ptr);
#endif /* ADP_MEM_TRACK_VERBOSE */

    return ptr;
    }

/*******************************************************************************
* adpMemTrkRealloc - realloc tracked memory
*
* This routine realloc's tracked memory
*
* RETURNS: a pointer to the allocate block and NULL on failure.
*/
void * adpMemTrkRealloc
    (
    ADP_PART_ID  memPartId,   /* partition Id              */
    void *       ptr,         /* pointer to be reallocated */
    size_t       size,        /* size of new pointer       */
    ...
    )
    {
    void *newPtr;
    ADP_MEM_NODE *cursor;
    size_t oldSize = 0;
    char *file = "(unknown file)";
    int line = 0;

#ifndef ADP_MEM_TRACK_LITE
    va_list arg;
    va_start (arg, size);
    file = va_arg (arg, char *);
    if (file == NULL)
        file = "(null file)";
    line = va_arg (arg, int);
    va_end (arg);
#endif /* ADP_MEM_TRACK_LITE */

    if (memPartId == NULL)
        memPartId = adpMemSysPartGet();

   /* 
    * null pointer wont be in memList, so use Alloc() to allocate
    * the memory                     
    */
   if (ptr == NULL)                  
#ifdef ADP_MEM_TRACK_LITE
       return adpMemTrkAlloc(memPartId, size);
#else /* !ADP_MEM_TRACK_LITE */             
       return adpMemTrkAlloc(memPartId, size, file, line);
#endif /* ADP_MEM_TRACK_LITE */            

    newPtr = adpMemPartRealloc (memPartId, ptr, size);
    if (newPtr)
        {
        for (cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_FIRST(&memPartId->adpMemList);
             ((cursor != NULL) && (cursor->ptr != ptr));
             cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_NEXT((ADP_NODE *)cursor))
            {
            continue;
            }

        if (cursor != NULL)
            {
            oldSize = cursor->size;
            /* decrement mem current usage */
            memPartId->curMemUsage -= cursor->size;
            /* increment mem usage by size allocated */
            memPartId->curMemUsage += size;
            if (memPartId->curMemUsage > memPartId->maxMemUsage)
                memPartId->maxMemUsage = memPartId->curMemUsage;

            cursor->ctxId = adpTaskIdSelf();
            cursor->ptr = newPtr;
            cursor->size = size;
            cursor->fileName = file;
            cursor->lineNumber = line;
            }
#ifdef ADP_MEM_TRACK_VERBOSE
        else
            {
            fprintf (stderr, "adpMemTrkRealloc(%s:%d): failed to locate "
                     "node for %p\n", 
                     file, line, ptr);
            }
#endif /* ADP_MEM_TRACK_VERBOSE */
        }
    
#ifdef ADP_MEM_TRACK_VERBOSE
    fprintf (stderr, "adpMemTrkReAlloc(%s:%d): [%p:%d bytes] realloc'ed to "
             "[%p:%d bytes]\n",
             file, line, oldSize, ptr, size, newPtr);
#endif /* ADP_MEM_TRACK_VERBOSE */
    return newPtr;
    }

/*******************************************************************************
*
* adpMemTrkFree - deallocate a tracked memory block
*
* This routine deallocates a tracked memory blocks.
*
* RETURNS: ST_OK on success and ERROR otherwise
*/
STATUS adpMemTrkFree
    (
    ADP_PART_ID  memPartId,   /* partition ID    */
    void *       ptr,         /* pointer to free */
    ...
    )
    {
    ADP_MEM_NODE *pNodePrev, *cursor;
    size_t  size;
    char *file = "(unknown file)";
    int line = 0;

#ifndef ADP_MEM_TRACK_LITE
    va_list arg;
    va_start (arg, ptr);
    file = va_arg (arg, char *);
    if (file == NULL)
        file = "(null file)";
    line = va_arg (arg, int);
    va_end (arg);
#endif /* ADP_MEM_TRACK_LITE */

    if (memPartId == NULL)
        memPartId = adpMemSysPartGet();

    pNodePrev = NULL;
    for (cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_FIRST(&memPartId->adpMemList);
         ((cursor != NULL) && (cursor->ptr != ptr));
         cursor=(ADP_MEM_NODE *)ADP_LIST_NODE_NEXT((ADP_NODE *)cursor))
        {
        pNodePrev=cursor;
        }

    if ((cursor != NULL) && (cursor->ptr == ptr))
        {
        size = cursor->size;
        memPartId->curMemUsage -= cursor->size;
        memPartId->curOverHead -= sizeof(ADP_MEM_NODE);

        adpMemPartFree (memPartId, ptr);
        
        adpListNodeRemove (&memPartId->adpMemList, (ADP_NODE *)cursor);
        --(memPartId->memCounter);
        adpMemPartFree (memPartId, cursor);
#ifdef ADP_MEM_TRACK_VERBOSE
        fprintf(stderr, "adpMemTrkFree(%s:%d): freed %d bytes at %p\n",
                file, line, size, ptr);
#endif /* ADP_MEM_TRACK_VERBOSE */
        ptr = NULL;
        }
    else
        {
        fprintf(stderr, "adpMemTrkFree(%s:%d): ptr %p not found\n",
                file, line, ptr);
        }
    
    return (ST_OK);
    }

/*******************************************************************************
*
* adpMemTrkCalloc - memory clear allocation
* 
* This routine allocates and zeros out a block of memory.
*
* RETURNS: a pointer to the allocated block and NULL on failure.
*/
void * adpMemTrkCalloc
    (
    ADP_PART_ID  memPartId,   /* partition ID       */
    size_t       nelem,       /* number of elements */
    size_t       size,        /* size of element    */
    ...
    )
    {
    void *ptr;
    char *file = "(unknown file)";
    int line = 0;

#ifndef ADP_MEM_TRACK_LITE
    va_list arg;
    va_start (arg, size);
    file = va_arg (arg, char *);
    if (file == NULL)
        file = "(null file)";
    line = va_arg (arg, int);
    va_end (arg);
#endif /* ADP_MEM_TRACK_LITE */

    if (memPartId == NULL)
        memPartId = adpMemSysPartGet();

    ptr = adpMemTrkAlloc (memPartId, nelem*size, file, line);

    if (ptr)
        bzero(ptr, nelem*size);

    return ptr;
    }

/*******************************************************************************
*
* adpMemTrkStrDup - duplicates a string
*
* This routine allocates memory for a string, then duplicates a string in the
* allocated memory.
*
* RETURNS: pointer to duplicate string
*/
char * adpMemTrkStrDup
    (
    ADP_PART_ID  memPartId,   /* partition ID        */
    const char * str,         /* string to duplicate */
    ...
    )
    {
    size_t len;
    char *cp;
    char *file = "(unknown file)";
    int line = 0;

#ifndef ADP_MEM_TRACK_LITE
    va_list arg;
    va_start (arg, str);
    file = va_arg (arg, char *);
    if (file == NULL)
        file = "(null file)";
    line = va_arg (arg, int);
    va_end (arg);
#endif /* ADP_MEM_TRACK_LITE */

    if (memPartId == NULL)
        memPartId = adpMemSysPartGet();

    len = strlen(str) + 1;

    cp = adpMemTrkAlloc(memPartId, len, file, line);

    if (cp)
        strcpy(cp, str);

    return cp;
    }

/* 
 * the following 2 memPart routines are provided for convenience and they use
 * other memPart routines that OS-specific. These are OS-neutral and so
 * defined here.
 */ 

/*******************************************************************************
*
* adpMemPartCalloc - memory clear allocation
* 
* This routine allocates and zeros out a block of memory.
*
* RETURNS: a pointer to the allocated block and NULL on failure.
*/
void * adpMemPartCalloc
    (
    ADP_PART_ID  memPartId,   /* partition ID       */
    size_t       nelem,       /* number of elements */
    size_t       size         /* size of element    */
    )
    {
    void *ptr;

    ptr = adpMemPartAlloc (memPartId, nelem*size);

    if (ptr)
        bzero(ptr, nelem*size);

    return ptr;
    }

/*******************************************************************************
*
* adpMemPartStrDup - duplicates a string
*
* This routine allocates memory for a string, then duplicates a string in the
* allocated memory.
*
* RETURNS: pointer to duplicate string
*/
char * adpMemPartStrDup
    (
    ADP_PART_ID  memPartId,   /* partition ID        */
    const char * str          /* string to duplicate */
    )
    {
    size_t len;
    char *cp;

    len = strlen(str) + 1;

    cp = adpMemPartAlloc(memPartId, len);

    if (cp)
        strcpy(cp, str);

    return cp;
    }
