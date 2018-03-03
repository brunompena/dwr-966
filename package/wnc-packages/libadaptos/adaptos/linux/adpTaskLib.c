/* adpTaskLib.c - adaptos task library for Linux */

/* Copyright (C) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,05jan05,k_p  created.
*/

/*
DESCRIPTION

This library provides all the task-related support for Linux

INCLUDES: adaptos.h, adptaskLib.h

SEE ALSO: 
*/

/* includes */

#include "adaptos.h"
#include "adpTaskLib.h"
#include "adpMemLib.h"
#include <pthread.h>
#include <string.h>


typedef struct adpTaskParam
    {
    ADP_FUNCPTR pEntryPoint;  /* task entry point */
    ULONG       arg1;         /* arg1-arg4 are required task arguments */
    ULONG       arg2;
    ULONG       arg3;
    ULONG       arg4;    
    } ADP_TASK_PARAM;


LOCAL void * adpLnxTaskEntryPoint (void * params);

#define  ADP_THRD_KEY pthread_key_t

/*******************************************************************************
*
* adpTaskSpawn - spawn a task
*
* This routine packs the entry point information such as entry point
* arguments and entry routine into one argument pTaskParam and spawns
* a new task by passing this argument. It returns the task Id of the
* spawned task.
*
* RETURNS: The task Id, or ERROR otherwise
*/



int adpTaskSpawn
    (
    char *      pName,          /* task name */
    ULONG       priority,       /* task priority */
    ULONG       options,        /* task options */
    ULONG       mode,           /* initial task attribute */
    ULONG       sStackSize,     /* supervisor task stack size */
    ULONG       uStackSize,     /* size of stack (in bytes) */
    ADP_FUNCPTR pFunc,          /* task entry point */
    ULONG       arg1,           /* arg1-arg4 are required task arguments */
    ULONG       arg2,           /* arg1-arg4 are required task arguments */
    ULONG       arg3,           /* arg1-arg4 are required task arguments */
    ULONG       arg4            /* arg1-arg4 are required task arguments */
    )
    {
    pthread_t       taskId = 0;
    ADP_TASK_PARAM  * pTaskParam = NULL;
    int retCode;

    pTaskParam = (ADP_TASK_PARAM  *) adpMalloc(sizeof(ADP_TASK_PARAM));

    if (pTaskParam == NULL)
        {
        return (ST_ERROR);
        }
    
    /* pack task parameters */
    
    pTaskParam->pEntryPoint  =  pFunc;
    pTaskParam->arg1         =  arg1;
    pTaskParam->arg2         =  arg2;
    pTaskParam->arg3         =  arg3;
    pTaskParam->arg4         =  arg4;
   
    /* create task/thread */
    
    retCode = pthread_create (&taskId, NULL, adpLnxTaskEntryPoint,
                              (void *) pTaskParam);

    /* EAGAIN : not enough system resources to create a process for the new
     *          thread, or more than PTHREAD_THREADS_MAX threads are already
     *          active.
     */
  
    if (retCode == EAGAIN)
        {
        return (ST_ERROR);
        }

    return ((int)taskId);
    }


/*******************************************************************************
*
* adpLnxTaskEntryPoint - adaptos task entry point
*
* This routine is the adaptos task entry point for Linux. It unpacks
* information such as arguments and entry routine from <params> and
* calls entry routine with unpacked arguments.
* 
* RETURNS:NULL
*
*/

LOCAL void * adpLnxTaskEntryPoint
    (
    void * params /* task parameters */
    )
    {

    ADP_TASK_PARAM * pTaskParam = (ADP_TASK_PARAM *)params;
    ADP_TASK_PARAM   tParamCopy;

    
    /* task may need these parameter throughout its life, so copy
     * parameters to local strcucture.
     */
    
    tParamCopy.pEntryPoint = pTaskParam->pEntryPoint;
    tParamCopy.arg1        = pTaskParam->arg1;
    tParamCopy.arg2        = pTaskParam->arg2;
    tParamCopy.arg3        = pTaskParam->arg3;
    tParamCopy.arg4        = pTaskParam->arg4;

    /* free  params resource */
   
    adpFree (params);
   
    /* call entry point with arguements */
    
    tParamCopy.pEntryPoint (tParamCopy.arg1, tParamCopy.arg2,
                            tParamCopy.arg3, tParamCopy.arg4); 
    return ((void *) NULL);
    }

/*******************************************************************************
*
* adpTaskDelete - delete a task
*
* It deletes a task from the system.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adpTaskDelete
    (
    ADP_TASK_ID     tId     /* task Id to delete */
    )
    {
    ADP_TASK_ID tmpId = tId;

    /* check if calling task itself */
    
    if ( tmpId == 0)
        {
        pthread_exit(PTHREAD_CANCELED);
        /* tmpId = adpTaskIdSelf();*/
        }
    
    return ( pthread_cancel(tmpId));
    }

/*******************************************************************************
*
* adpTaskNameToId -  look up the task Id associated with a task name
*
* It takes the name of the task as an argument and returns it's Id.
*
* RETURNS: The task Id, or ERROR on failure
*/

ADP_TASK_ID adpTaskNameToId
    (
    char *  pTaskName        /* task Name */
    )
    {

    /* not implemented for Linux */
    return (ST_ERROR);
    }

/*******************************************************************************
*
* adpTaskIdSelf - get the task Id of a running task
*
* It returns the task Id of the calling task.
*
* RETURNS: The task Id
*/

int adpTaskIdSelf ()
    {
    
    return ((int) pthread_self());
    }

/*******************************************************************************
*
* adpTaskIdVerify - verify the existence of a task.
*
* It verifies whether the task give by the task Id <tId> exists in the system
* or not.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adpTaskIdVerify
    (
    ADP_TASK_ID     tId     /* task Id */
    )
    {
    struct sched_param param;
    int policy;
    
    if (tId == NULL)
      {
    return (ST_ERROR);
      }
      
    if (pthread_getschedparam ((ULONG)tId, &policy, &param) != ST_OK)
        {
        return (ST_ERROR);
        }
    
    return (ST_OK);
    }


/*******************************************************************************
*
* adpTaskDelay - delay a task from executing
*
* This routine delays a task from executing by a specified number of ticks.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adpTaskDelay
    (
    ULONG   ticks       /* no. of ticks to wake the task after */
    )
    {
    
    long sleeptime;

    if (ticks > 0)
        {
        /* convert ticks in to micro seconds */    
        sleeptime = ((ticks * 1000000)/sysconf(_SC_CLK_TCK));

        /* sleep for <sleeptime> micro seconds */
        usleep(sleeptime);
        }
    
    return(ST_OK);
    }

/*******************************************************************************
*
* adpTaskDeleteHookRegister - add a routine to be called at every task delete
*
* This routine adds the routine pointed to by the argument <pCallOutRtn> to
* the list of routines that are called when a task is deleted.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

ADP_TASK_CO_ID adpTaskDeleteHookRegister
    (
    ULONG               type,           /* type of call out */
    ADP_TASK_CO_HOOK    pCallOutRtn,    /* pointer to function to call */
    void *              pArg1           /* argument given to the function */
    )
    {
    /* function not implemented */
    
    return(ADP_TASK_ERROR);
    }

/*******************************************************************************
*
* adpTaskDeleteHookDeRegister - (function not defined)
*
* This function is not yet implemented.
*
* RETURNS: ERROR
*/

int adpTaskDeleteHookDeRegister
    (
    ADP_TASK_CO_ID  coId,   /* call out Id */
    ULONG           flags,  /* flags */
    ULONG           timeout /* timeout */
    )
    {

    return (ADP_TASK_ERROR);
    }

/*******************************************************************************
*
* adpTaskInfoGet - get information about a task
*
* This routine retrieves information on a running task. This information is
* added to the ADP_TASK_INFO structure pointer <pTaskInfo>.
*
* RETURNS: ST_OK on success, or ERROR otherwise
*/

STATUS adpTaskInfoGet
    (
    ADP_TASK_ID     tId,        /* task Id */
    ADP_TASK_INFO * pTaskInfo   /* task info */
    )
    {
    
    return (ADP_TASK_OK);
    }

/*******************************************************************************
*
* adpTaskSuspend - suspend a given task.
*
* This routine suspends a given task with the taskId <taskId>.
* 
* RETURNS: ST_OK on success, or ERROR otherwise 
*/

STATUS adpTaskSuspend
    (
    ADP_TASK_ID     tId     /* task Id */
    )
    {
    /* not implemented */
    return (ST_ERROR);
    }

