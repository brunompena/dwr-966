/* adpShmLib.c - Shared Memory Library. */

/* Copyright (c) 2006 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01b,29jun07,rnp fixed a warning in adpShmDetach()
01a,10aug06,smv written.
*/

/*
DESCRIPTION

This file implements APIs to use shared memory in linux environment.
*/


/* includes */

#include <adaptos.h>


/* defines */


/* typedefs */


/* globals */


/* locals */

LOCAL int errFlag = 0;

/* imports */


/* forward declarations */

LOCAL inline void adpShmErrShow (const char * pErrMsg);
LOCAL inline void adpShmWarnShow (const char * pWarnMsg);

/*******************************************************************************
*
* adpShmErrDisplay - sets the error display flag.
*
* This routine sets the error display flag. 
*         
* RETURNS: N/A
*/
void adpShmErrDisplay
    (
    int status /* 0 - off, 1 - on */
    )
    {
    errFlag = status;
    }

/*******************************************************************************
*
* adpShmErrShow - displays the error if error display is enabled.
*
* This routine displays the error if error display is enabled.
*         
* RETURNS: N/A.
*/

LOCAL inline void adpShmErrShow
    (
    const char * pErrMsg /* error string to display */
    )
    {
    if (!errFlag)
        return;
    perror (pErrMsg);
    return;
    }

/*******************************************************************************
*
* adpShmWarnShow - displays the warnings if error display is enabled.
*
* This routine displays the warnings if error display is enabled.
*         
* RETURNS: N/A.
*/

LOCAL inline void adpShmWarnShow
    (
    const char * pWarnMsg /* warning string to display */
    )
    {
    if (!errFlag || !pWarnMsg)
        return;
    fprintf (stderr, "%s\n", pWarnMsg);
    return;
    }

/*******************************************************************************
*
* adpShmKeyGet - generates a system V IPC key.
*
* This routine uses the identity of the specified file and the least significant
* 8 bits of projId to generate a key_t type system V IPC key.
*         
* RETURNS: generated ADP_KEY_T on success.
*          -1 on error.
*
* ERRNO: N/A
* 
* SEE_ALSO:
*/

ADP_KEY_T adpShmKeyGet
    (
    const char * pFilePath,  /* path to an existing, accessible file */
    int          projId      /* a non-zero value */
    )
    {
    ADP_KEY_T sysVIpcKey = -1;

    /* no need of pFilePath check. ftok() will return error. */
    
    /* use default ID if not specified */
    if (!projId)
        {
        projId = ADP_SHM_PROJ_ID;
        adpShmWarnShow ("Invalid project ID. Using default project ID");
        }
    
    sysVIpcKey = ftok (pFilePath, projId);
    if (sysVIpcKey == (key_t)-1)
        adpShmErrShow ("ftok() Failed");

    return ((ADP_KEY_T)sysVIpcKey);
    }

/*******************************************************************************
*
* adpShmIdGet - gets the shared memory segment identifier.
*
* This routine returns the identifier of the shared memory segment associated
* with the value of the sysVIpcKey argument. It creates shared memory segment
* with size equal to the value of segSize argument rounded up to a multiple of
* system PAGE_SIZE.
*         
* RETURNS: shared memory segment identifier.
*          -1 on error.
*
* ERRNO: N/A.
* 
* SEE_ALSO:
*/

int adpShmIdGet
    (
    ADP_KEY_T  sysVIpcKey,   /* valid System V IPC Key */
    int segSize,      /* segment size */
    int    shmFlags      /* segment creation flags */
    )
    {
    int shmId = -1;

    if (sysVIpcKey == (key_t) -1)
        return -1;
    
    if (segSize < 0)
        {
        /* use default buffer size */
        segSize = ADP_SHM_SIZE;
        adpShmWarnShow ("Invalid segment size. Using default segment size.");
        }
    
    if (shmFlags < 0)
        {
        /* use default permissioans and create flags */
        shmFlags = (ADP_SHM_PERM | ADP_SHM_CREAT_FLAGS);
        adpShmWarnShow ("Invalid flags. Using default flags.");
        }

    shmId = shmget (sysVIpcKey, segSize, shmFlags);
    if (shmId == -1)
        adpShmErrShow ("shmget() failed");

    return shmId;
    }

/*******************************************************************************
*
* adpShmAttach - attaches specified segment to specified address space.
*
* This routine attaches  the  shared  memory segment identified by shmid to the
* address space of the calling process.
*
* If shmaddr is NULL, the system chooses a suitable (unused) address
* at which to attach the segment.
*         
* RETURNS: address of the attached shared memory segment.
*          NULL on error.
*
* ERRNO: N/A
* 
* SEE_ALSO: adpShmDetach
*/

void * adpShmAttach
    (
    int          shmId,      /* shared memory segment identifier */
    const void * pShmAddr,   /* attaching address */
    int          shmFlags    /* attach controlling flags */
    )
    {
    void * pShmSegAddr = NULL;

    if (shmId == -1)
        {
        adpShmWarnShow ("Invalid shared memory identifier.");
        return pShmSegAddr;
        }

    pShmSegAddr = shmat (shmId, pShmAddr, shmFlags);
    if (pShmSegAddr == (void *)(-1))
        {
        adpShmErrShow ("shmat() failed");
        return NULL;
        }
    
    return pShmSegAddr;
    }

/*******************************************************************************
*
* adpShmDetach - detached specified shared memory segment.
*
* This routine detaches the shared memory segment located at the address
* specified by 'pShmAddr' from the address space of the calling process.
* The to-be-detached segment must be currently attached with  'pShmAddr' equal
* to the value returned by the attaching adpShmAttach () call.       
*
* RETURNS: 0 on success.
*         -1 on error.
*
* ERRNO: 
* 
* SEE_ALSO: adpShmAttach
*/

int adpShmDetach
    (
    const void * pShmAddr /* attached segment address */
    )
    {
    int result = -1;

    if (pShmAddr == NULL)
        {
        adpShmWarnShow ("Invalid shared memory segment address.");
        return -1;
        }

    result = shmdt (pShmAddr);
    if (result == -1)
        adpShmErrShow ("shmdt() failed");

    return result;
    }
