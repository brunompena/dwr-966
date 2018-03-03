/* adpTaskLib.h -  adaptos process library header */

/* Copyright (c) 2003 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01i,06oct07,san  change adpTaskInfoGet prototype for fixing warning.
01h,16aug03,ksn  added adpPsosTaskCreate and adpPsosTaskStart.
01g,30jul03,ksn  added comments
01f,20may03,ksn  added adpTaskVarDelete.
01e,19apr03,ksn  modified error macros.
01d,08mar03,ksn  added support for vxWorks
01c,18jun02,mno  added adpTaskInfo, updated task flags and mode
01b,06jun02,mno  cleanups.
01a,08may02,vnk  created.
*/

#ifndef __INCadpTaskLibH
#define __INCadpTaskLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* error codes */

#define ADP_TASK_OK             (0)
#define ADP_TASK_ERROR          (-1)
#define S_adpTaskLib_NO_MEM     (M_adpTaskLib | 0x1)
#define S_adpTaskLib_ID_INVALID (M_adpTaskLib | 0x2)

/* task flags or options */

#define ADP_TASK_OPT_NO_BREAK       0x0002
#define ADP_TASK_OPT_DEALLOC_STACK  0x0004
#define ADP_TASK_OPT_FP         0x0008      
#define ADP_TASK_OPT_PRI_ENV        0x0080      
#define ADP_TASK_OPT_NO_STACK_FILL  0x0100

/* task mode */

#define ADP_TASK_MODE_NO_PREEMPT    0x0001
#define ADP_TASK_MODE_SLICE     0x0002
#define ADP_TASK_MODE_NO_ASR        0x0004
#define ADP_TASK_MODE_SUPV      0x0008
#define ADP_TASK_MODE_USER      0x0000

/* callout type */

#define ADP_TASK_CO_START   0
#define ADP_TASK_CO_DELETE  1
#define ADP_TASK_CO_RESTART 2

#define ADP_TASK_NAME_MAX   8

/* typedefs */

#ifndef HAVE_ADP_TASK_ID
typedef int ADP_TASK_ID;
#define HAVE_ADP_TASK_ID
#endif

#ifndef HAVE_ADP_TASK_CO_ID
typedef int ADP_TASK_CO_ID;
#define HAVE_ADP_TASK_CO_ID
#endif

#ifndef HAVE_ADP_TASK_INFO
typedef struct adpTaskInfo
    {
    ULONG   id;             /* task ID */
    char    name[ADP_TASK_NAME_MAX];    /* task name */
    ULONG   priority;           /* task priority */
    ULONG   status;             /* task status */
    ULONG   options;            /* task options */
    ULONG   entryPoint;         /* entry function */
    ULONG   errorStatus;            /* error status */
    ULONG   delay;              /* task delay */
    ULONG   usrStackPtr;            /* pointer to user stack */
    ULONG   usrStackBase;           /* user stack base value */
    ULONG   usrStackLimit;          /* user stack limit */
    ULONG   usrStackEnd;            /* user stack end */
    ULONG   usrStackSize;           /* user stack size */
    ULONG   usrStackCurrent;        /* user stack current */
    ULONG   usrStackHigh;           /* user stack high */
    ULONG   usrStackMargin;         /* user stack margin */
    ULONG   superStackPtr;          /* superviser stack pointer */
    ULONG   superStackBase;         /* superviser stack base */
    ULONG   superStackLimit;        /* superviser stack limit */
    ULONG   superStackSize;         /* superviser stack size */
    } ADP_TASK_INFO;
#define HAVE_ADP_TASK_INFO
#endif

#ifndef HAVE_ADP_TASK_DESC
typedef struct          /* TASK_DESC - information structure */
    {
    int         td_id;      /* task id */
    char *      td_name;    /* name of task */
    int         td_priority;    /* task priority */
    int         td_status;  /* task status */
    int         td_options; /* task option bits (see below) */
    ADP_FUNCPTR     td_entry;   /* original entry point of task */
    char *      td_sp;      /* saved stack pointer */
    char *      td_pStackBase;  /* the bottom of the stack */
    char *      td_pStackLimit; /* the effective end of the stack */
    char *      td_pStackEnd;   /* the actual end of the stack */
    int         td_stackSize;   /* size of stack in bytes */
    int         td_stackCurrent;/* current stack usage in bytes */
    int         td_stackHigh;   /* maximum stack usage in bytes */
    int         td_stackMargin; /* current stack margin in bytes */
    int         td_errorStatus; /* most recent task error status */
    int         td_delay;   /* delay/timeout ticks */
    /* add psos related stuff */
    } ADP_TASK_DESC;
#define HAVE_ADP_TASK_DESC
#endif

/* Prototypes */
ADP_TASK_ID adpTaskSpawn
    (
    char *          pName,      /* 4 letter name of the task */
    ULONG       priority,   /* priority of the task */
    ULONG       options,    /* task options */
    ULONG       mode,       /* task mode */
    ULONG       sStackSize, /* supervisor stack size */
    ULONG       uStackSize, /* user stack size */
    ADP_FUNCPTR     pFunc,      /* entry point of the task */
    ULONG       arg1,       /* argument 1 */
    ULONG       arg2,       /* argument 2 */
    ULONG       arg3,       /* argument 3 */
    ULONG       arg4        /* argument 4 */
    );

IMPORT int adpTaskVarAdd (ADP_TASK_ID   tId, void ** pPtrVar);
IMPORT int adpTaskVarDelete (ADP_TASK_ID tId, void **   pPtrVar);
IMPORT int adpTaskDelete (ADP_TASK_ID tId);
IMPORT ADP_TASK_ID adpTaskIdSelf (void);
IMPORT ADP_TASK_ID adpTaskNameToId (char *taskName);
IMPORT STATUS adpTaskInfoGet (ADP_TASK_ID tId, ADP_TASK_INFO *pTaskInfo);
IMPORT int adpTaskIdVerify (ADP_TASK_ID tId);
IMPORT int adpTaskDelay (ULONG  ticks);
IMPORT ADP_TASK_CO_ID adpTaskDeleteHookRegister (ULONG  type,
                                                 ADP_TASK_CO_HOOK pCallOutRtn,
                                                 void * arg1);
IMPORT int adpTaskDeleteHookDeRegister (ADP_TASK_CO_ID  coId, ULONG flags,
                                        ULONG timeout);
IMPORT void adpTaskStdSet (int taskId, int stdFd, int newFd);
IMPORT int adpTaskStdGet (int taskId, int stdFd);
IMPORT int adpGlobalStdGet (int stdFd);
IMPORT void adpGlobalStdSet (int stdFd, int newFd);
IMPORT STATUS adpTaskSuspend (ADP_TASK_ID   tId);
IMPORT int adpPsosTaskCreate (char *    pName, ULONG priority, ULONG options,
                              ULONG sStackSize, ULONG   uStackSize);
IMPORT STATUS adpPsosTaskStart (ULONG   taskId, ULONG   mode,
                                ADP_FUNCPTR pFunc, ULONG    arg1, ULONG arg2,
                                ULONG   arg3, ULONG arg4);
#ifdef __cplusplus
}
#endif

#endif /* __INCadpTaskLibH */
