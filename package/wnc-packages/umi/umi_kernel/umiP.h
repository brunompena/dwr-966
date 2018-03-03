/* umiP.h - private definitions for the UMI library*/

/* Copyright (c) 2005 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01e,04sep08,rnp  changed AF_UMI to 31 to address error when inserting umi.ko on
                 2.6.10
                 removed LINUX KERNEL version protection around UMI_SPIN_XXX(),
                 to address "BUG in local_bh_enable()" message
01f,04jan08,smv  added UMI_SPIN_LOCK and UMI_SPIN_UNLOCK.
01d,20jul07,rks  removed UMI_DB_HANDLER from UMI_CTX as now
                 request is directly passed to component.
01c,28jun07,smv  added UMI_DB_HANDLER variable in UMI_CTX.
01b,31aug05,sam  added lock to gaurd recvQ.
01a,11feb05,rks  written.
*/

#ifndef __INCumiPh
#define __INCumiPh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */

#ifdef __KERNEL__
#include <linux/list.h>
#elif defined(ADAPTOS_LINUX)
#include <adaptos.h>
#include <adpListLib.h>
#include <adpSemLib.h>
#else
#include <vxWorks.h>
#include <msgQLib.h> 
#endif /* __KERNEL__*/


/* defines */

#define AF_UMI  31

#if defined (__KERNEL__)


#define	UMI_SPIN_LOCK(lock)	        \
	do {                            \
	     if (irqs_disabled()) {		\
		    spin_lock(&lock);		\
	     } else {					\
     		spin_lock_bh(&lock);	\
	     }                          \
     } while(0)

#define	UMI_SPIN_UNLOCK(lock)	    \
	do {                            \
	     if (irqs_disabled()) {		\
		     spin_unlock(&lock);	\
	     } else {					\
		     spin_unlock_bh(&lock);	\
	     }                          \
     } while(0)

#endif /* __KERNEL__ */

/* typedefs */

typedef enum 
    {
    UMI_COMP_TYPE_USER = 1,
    UMI_COMP_TYPE_KERNEL = 2
    } UMI_COMP_TYPE;

typedef struct
    {
    unsigned int lockTake;
    unsigned int lockTakeFail;
    unsigned int argsToBufFail;
    unsigned int lockGive;
    unsigned int ioctlSendToFail;
    unsigned int asyncOrNoResultIoctlSuccess;
    unsigned int syncWaitExited;
    unsigned int syncIoctlSuccess;
    unsigned int recvFromFailed;
    unsigned int reqTooShort;
    unsigned int recvReply;
    unsigned int reqWitNoReply;
    unsigned int replySent;
    } UMI_STATS;

typedef struct 
    {
#if defined (__KERNEL__)
    struct list_head    node;
    int                 sockFd;
#elif defined (ADAPTOS_LINUX)
    ADP_NODE            node;   
    ADP_MUTEX_ID        lock;
    int                 sockFd;
    ADP_LIST            recvQ;
#else
    NODE                node;
    SEM_ID              lock;
    SEM_ID              recvSem;
    LIST                recvQ;
    SEM_ID              recvQLock;
#endif /* __KERNEL__ */
    UMI_COMP_ID         myId;
    UMI_COMP_TYPE       compType;
    UMI_IOCTL_HANDLER   handler;
    void *              arg1;
    unsigned int        umiOpt;
    UMI_STATS           stats;
    } UMI_CTX;

/* imports */


/* function declarations */ 


#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* __INCumiPh */
