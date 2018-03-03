/* adpSemLib.c - adaptos semaphore library for POSIX  based OSes */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,01apr08,san  created (based on linux/adpxxx) .
*/

/*
DESCRIPTION

This library provides the adaptos mutual exclusion and binary semaphore
implementation for POSIX based operating systems.

INCLUDES: adaptos.h, adpSemLib.h

*/
#include "adaptos.h"

/* externs */

extern int pthread_mutexattr_settype (pthread_mutexattr_t * attr, int kind);

/*******************************************************************************
*
* adpMutexCreate - create and initialize a mutual exclusion semaphore
*
* This routine can be used to allocate and initialize a mutual exclusion
* semaphore. The <options> argument can have the following values:
*
* SEM_Q_PRIORITY (0x1) - pended tasks queued on the basis of priority.
* SEM_Q_FIFO (0x0) - pended tasks queued on FIFO basis.
* SEM_DELETE_SAFE (0x4) - task with semaphore is protected from unexpected
*                         deletion.
* SEM_INVERSION_SAFE (0x8) - protect system from priority inversion.
*
* RETURNS: The semaphore ID, or NULL on failure
*/

ADP_MUTEX_ID adpMutexCreate 
    (
    char *      pName,      /* dummy */
    ULONG       options     /* mutual exclusion semaphore options */
    )
    {
    pthread_mutex_t * mutex;
    pthread_mutexattr_t mutattr;

    if ((mutex = (pthread_mutex_t *)adpMalloc(sizeof(pthread_mutex_t))) == NULL)
        {
        return ((ADP_MUTEX_ID)NULL);
        }

    pthread_mutexattr_init(&mutattr);
    pthread_mutexattr_settype (&mutattr, PTHREAD_MUTEX_RECURSIVE);

    if(pthread_mutex_init(mutex, &mutattr) != 0)
        {
        return ((ADP_MUTEX_ID)NULL);
        }
    return ((ADP_MUTEX_ID)mutex);
    }
/*******************************************************************************
*
* adpMutexDelete - delete a mutual exclusion semaphore
*
* This routine can be used to delete a mutual exclusion semaphore. All the
* memory associated with the semaphore is deallocated on this call.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adpMutexDelete 
    (
    ADP_MUTEX_ID mutexId    /* mutual exclusion ID */
    )
    {
    if (!mutexId)
        return (-1);
    pthread_mutex_destroy((pthread_mutex_t *)mutexId);
    adpFree((void *)mutexId);
    return (0);
    }
/*******************************************************************************
*
* adpMutexTake - take a mutual exclusion semaphore
*
* This routine does a take on a mutual exclusion semaphore. The <timeout>
* argument is ignored as there is no way to specify timeouts in POSIX.
* option can have the following values:
*
* ADP_MUTEX_WAIT_FOREVER (0) - wait forever to take the mutex semaphore
* ADP_MUTEX_NO_WAIT (1) - return immediately whether or not mutex is taken
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adpMutexTake 
    (
    ADP_MUTEX_ID    mutexId,    /* mutual exclusion ID */
    ULONG           options,    /* mutual exclusion semaphore options */
    ULONG           timeout     /* timeout in ticks */
    )
    {
    if (options == ADP_MUTEX_NO_WAIT)
        {
        return (pthread_mutex_trylock((pthread_mutex_t *)mutexId));
        }
    else
        {
        return (pthread_mutex_lock((pthread_mutex_t *)mutexId));
        }
    }
/*******************************************************************************
*
* adpMutexGive - give a mutual exclusion semaphore
*
* This routine does a give operation on a mutual exclusion semaphore.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adpMutexGive 
    (
    ADP_MUTEX_ID mutexId    /* mutual exclusion ID */
    )
    {
    return (pthread_mutex_unlock((pthread_mutex_t *)mutexId));
    }

/*******************************************************************************
*
* adpSemBCreate - create and initialize a binary semaphore
*
* This routine allocates a binary semaphore and initializes it. The <options>
* and <name> is ignored.
*
* The <bFull> argument can have the following values:
*
* ADP_SEM_FULL (1) - initialize semphore are full
* ADP_SEM_EMPTY (0) - initialize semaphore are empty
*
* RETURNS: The semaphore Id, or NULL on failure
*/

ADP_SEM_ID adpSemBCreate 
    (
    char *  name,           /* future use */
    ULONG   options,        /* mutual exclusion semaphore options */
    BOOL    pFull           /* initial state */
    )
    { 
    sem_t * sem;
    int     value = 0;

    if ((sem = (sem_t *)adpMalloc(sizeof(sem_t))) == NULL)
        return (ADP_SEM_ID)(NULL);

    if (pFull)
        value = 1;

    if (sem_init(sem, 0, value) != 0)
        {
        return ((ADP_SEM_ID)NULL);
        }
    else
        {
        return (ADP_SEM_ID)(sem);
        }
    }
        
/*******************************************************************************
*
* adpSemBDelete - delete a binary semaphore
*
* This routine deletes a binary semaphore and deallocates any memory
* associated with it.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/
int adpSemBDelete 
    (
    ADP_SEM_ID  semId       /* semaphore Id */
    )
    {
    if(!semId)
        return (-1);
    sem_destroy((sem_t *)semId);
    adpFree ((void *)semId);
    return (0);
    }
/*******************************************************************************
*
* adpSemTake - take a semaphore
*
* This routine does a take on a semaphore. The <timeout> argument is ignored.
* options can  have the  following values:
*
* ADP_SEM_WAIT_FOREVER (-1) - wait forever to take the mutex semaphore
* ADP_SEM_NO_WAIT (0) - return immediately whether or not mutex is taken
*
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adpSemTake 
    (
    ADP_SEM_ID  semId,      /* semaphore Id */
    ULONG       options,    /* semaphore options */
    ULONG       timeout     /* timeout in ticks */
    )
    {
    if (options == ADP_SEM_NO_WAIT)
        {
        return (sem_trywait((sem_t *)semId));
        }
    else
        {
        return (sem_wait((sem_t *)semId));
        }
    }

/*******************************************************************************
*
* adpSemGive - give a semaphore
*
* This routine does a give on a semaphore.
*
* RETURNS: ST_OK on success, or ERROR
*/

int adpSemGive 
    (
    ADP_SEM_ID semId        /* semaphore Id */
    )
    {
    return (sem_post((sem_t *)semId));
    }
