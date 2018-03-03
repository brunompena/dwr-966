/* umiLib.c - TeamF1's Universal Management Interface (UMI) Library */

/* Copyright (c) 2007, TeamF1, Inc. */
    
/*
modification history
--------------------
01i,16oct07,rnp  added check for umiInitialized in umiDeInit()
01h,17oct07,sdk updated umiProcessReq to account for UMI_CMD_IFDEV_EVENT
01g,28jul07,rks moved umiProcessQueuedRequests inside while(1) so that
                requests are processed in the order they were received.
01f,20jul07,rks remove umiRegisterDbHandler.
01e,28jun07,smv added umiRegisterDbHandler.
01d,10oct05,sam fixed endian issues.
01c,04aug05,sam changed debug levels
01b,08apr05,rks changes to remove UMI_MAX_REQ_SIZE limitation
01a,11feb05,rks written.
*/

/*
DESCRIPTION
TeamF1's Universal Management Interface (UMI) library implements a generic 
framework for communication in the following scenarios:
         1.) sender and the receiver are part of two different processes
         2.) sender is in the kernel space and receiver is in user space or 
         vice-versa
 
In the following description, a component is the end point of communication. A 
component is an entity, which has a UMI context. Any process/task can be a UMI
component by creating a context using the umiRegister() API. It is also 
possible for a process/task to use the framework for communication without 
having any UMI context. But there are certain limitations in this model, which 
are documented in the umiIoctl() API.

Any component that needs to use the UMI framework has to do the following:

 a.) Initialize the UMI library using umiInit().

 b.) The UMI library uses the adaptos I/O framework. So the adaptos I/O 
     framework needs to be installed with the following library call. 

        adpIoLibInit (adpLnxIoLibInstall, NULL);


 c.) Identify the component ID with which it wants to communicate. The list of 
     components that are currently registered with the UMI can be found by 
     querying the UMI framework. 
 
 d.) Optionally, create a context for itself. This can be done using the 
     umiRegister() API. 
   
 e.) Identify the data format that the destination component expects. The 
     destination component publishes the format in which it expects the data. 
     Please refer to the destination component's documentation to identify the
     data structure. 
   
 f.) In case of a duplex communication, a component must also be aware of the 
     data-format in which the destination component sends replies.
   
A component 'A' can send data to component 'B' using umiIoctl(). This call 
will be successful only if component 'B' has an ioctl handler registered with 
the UMI framework.

Any component that wants to reply to UMI requests from other components 
has to register a handler with the UMI framework using umiRegister(). The 
prototype of the handler is given below:

.CS
typedef int (* UMI_IOCTL_HANDLER)
   (
   void *           arg1,     -  UMI context
   UMI_REQ_INFO *   pUmiReq,  -  UMI Request info
   int              cmd,      -  UMI command
   void *           pBuf1,    -  pointer to data buffer
   unsigned int     bufLen1,  -  length of data buffer
   void *           pBuf2,    -  pointer to data buffer
   unsigned int *   bufLen2   -  length of data buffer
   );
.CE

A component has to call umiListen() API and wait for ioctl requests if
      1.) It wants to execute the ioctl handler in its own (process/task's)
          context.
      2.) The source and destination components are in different address spaces.
  
In above cases, the component has to register the handler with the
UMI_OPT_MY_TASK_CTX option.
 
On receiving a UMI request, the ioctl handler that has been registered using 
umiRegister() is invoked. The <cmd> argument in the ioctl handler specifies 
the type of command that the caller invoked. <pBuf1> and <pBuf2> are pointers
to the data that is being communicated. The data format of <pBuf1> and <pBuf2>
is left to the interpretation of the sender and the receiver. This data is not
interpreted by the UMI framework. 

INCLUDE FILES: umi/umi.h, umiP.h
*/

/* includes */

#include <stdio.h>
#include <umi.h>
#include <umiP.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <adaptos.h>
#include <adpListLib.h>
#include <adpMemLib.h>
#include <adpModDebug.h>

/* defines */

#ifndef UMI_DEBUG
#undef ADP_DEBUG2
#undef ADP_DEBUG
#undef ADP_ERROR
#define ADP_DEBUG(x...)
#define ADP_DEBUG2(x...)
#define ADP_ERROR(x...)
#endif

#define MICROSECS_IN_SECS   1000000
/* typedefs */  

typedef struct 
    {
    ADP_NODE        node;
    UMI_REQ_INFO *  pUmiReq;
    } UMI_REQ_BUF;

/* externs */ 


/* globals */

UMI_CTX *       pGlobalUmiCtx;
ADP_LIST        umiCtxList;
ADP_MUTEX_ID    umiLock;
int             umiInitialized = 0;

/* locals */

ADP_MOD_ID (UMI_COMP_UMI);

UMI_ID_NAME_MAP umiIdNameMap[] = { UMI_ID_NAME_MAP_LIST };

/* imports */


/* forward declarations */

static int          umiIoctlArgsToBuf ( char ** ppBuf, unsigned int bufLen,
                                        void * pInBuf, unsigned int inLen, 
                                        void * pOutBuf, unsigned int * pOutLen,
                                        unsigned int *  pBufSize );
static void         umiBufToIoctlArgs ( char * pUmiBuf, void ** ppInBuf, 
                                        unsigned int * pInLen, void ** ppOutBuf,
                                        unsigned int * pOutLen );
static int          umiRecvFrom ( int sockFd, char ** pBuf, unsigned int size, 
                                  int timeout );
static UMI_CTX *    umiFindCtx ( UMI_COMP_ID id );

static int          umiIoctlHandler ( void * arg1, UMI_REQ_INFO * pReqInfo,
                                      int cmd, void * pInBuf, 
                                      unsigned int inLen, void * pOutBuf, 
                                      unsigned int *  pOutLen );
static int          umiGenericIoctlHandler ( void * arg1, 
                                      UMI_REQ_INFO * pReqInfo,
                                      int cmd, void * pInBuf, 
                                      unsigned int inLen, void * pOutBuf, 
                                      unsigned int *  pOutLen );
static void         umiProcessReq (UMI_CTX * pSrcCtx, UMI_REQ_INFO * pUmiReq);
static void         umiProcessQueuedRequests (UMI_CTX * pSrcCtx);
static void         umiQueueRequest (UMI_CTX * pSrcCtx, UMI_REQ_INFO * pUmiReq,
                                     int reqSize);

/*******************************************************************************
*
* umiInit - initialize the UMI library
* 
* This routine initializes the UMI library. This function has to be called 
* before using any other function in this library.
*
* RETURNS: 0 for success , -1 for error
*/

int umiInit (void)
    {
    if (umiInitialized)
        {
        return (0);
        }
    
    if ((umiLock = adpMutexCreate (NULL, 0)) == (ADP_MUTEX_ID)NULL)
        {
        return -1;
        }
    
    adpListInit (&umiCtxList);

    pGlobalUmiCtx = umiRegister (UMI_COMP_UMI, UMI_OPT_ANY_TASK_CTX,
                                 (UMI_IOCTL_HANDLER)umiIoctlHandler, NULL);
    if (pGlobalUmiCtx == NULL)
        {
        return (-1);
        }

    umiInitialized = 1;

    return (0);
    }

/*******************************************************************************
*
* umiDeInit - de-initialize the UMI library
*
* This routine de-initializes the UMI library.
* 
* RETURNS: NONE
*/

void umiDeInit (void)
    {
    if (!umiInitialized)
        return;

    if (SIGTERM)
        {
        syslog(LOG_INFO, "caught signal to terminate\n");
        }
    umiUnRegister (pGlobalUmiCtx);
    adpMutexDelete (umiLock);
    }

/*******************************************************************************
*
* umiRegister - register a component's ioctl handler
*
* This routine creates a UMI context for the caller. The <myId> argument
* specifies the component ID of the caller and <handler> is the ioctl handler. 
* This handler is invoked whenever an ioctl request is received for this 
* component. The component can specify if the ioctl handler can be invoked
* from  the caller's task context or in its own task context. 
* If it is later, the component needs to call  umiListen() to accept ioctl 
* requests.
*
* umiOpt can take the following values:
*
* .iP "UMI_OPT_ANY_TASK_CTX"
* This option means that the ioctl handler can be invoked from any task context.
*
* .iP "UMI_OPT_MY_TASK_CTX"
* This option means that the ioctl handler should be invoked only from its 
* task context.
*
* RETURNS: pointer to UMI context
*/

void *  umiRegister 
    (
    UMI_COMP_ID         myId,       /* this component ID */
    int                 umiOpt,     /* UMI options */
    UMI_IOCTL_HANDLER   handler,    /* component's ioctl handler */
    void *              arg1        /* first argument of the handler */
    )
    {
    UMI_CTX *   pUmiCtx;

    pUmiCtx = (UMI_CTX *)adpMalloc (sizeof (UMI_CTX));

    if (pUmiCtx == NULL)
        {
        return (NULL);
        }
    
    pUmiCtx->myId = myId ;
    pUmiCtx->handler = handler;
    pUmiCtx->umiOpt = umiOpt;
    pUmiCtx->arg1 = arg1;

    pUmiCtx->sockFd = socket (AF_UMI, 0 , myId);

    if (pUmiCtx->sockFd == -1)
        {
        adpFree(pUmiCtx);
        return (NULL);
        }
    
    pUmiCtx->lock = adpMutexCreate (NULL, 0);

    adpListInit (&pUmiCtx->recvQ);
    
    if (pUmiCtx->lock == (ADP_MUTEX_ID)NULL)
        {
        close(pUmiCtx->sockFd);
        adpFree(pUmiCtx);
        return (NULL);
        }
    
    adpMutexTake (umiLock, ADP_MUTEX_WAIT_FOREVER, 0);
    adpListNodeAppend (&umiCtxList, (ADP_NODE *)pUmiCtx);
    adpMutexGive (umiLock);
    return (pUmiCtx);
    }

/*******************************************************************************
*
* umiUnRegister - delete a UMI context
* 
* This routine deletes the caller's UMI context and unregisters the component's
* ioctl handler from UMI.
*
* RETURNS: NONE
*/

void umiUnRegister
    (
    void *  pUmiCtx  /* UMI context */
    )
    {
    
    umiProcessQueuedRequests (pUmiCtx);
    
    if (((UMI_CTX *)pUmiCtx)->sockFd != -1)
        close (((UMI_CTX *)pUmiCtx)->sockFd);
    adpMutexTake (umiLock, ADP_MUTEX_WAIT_FOREVER, 0);
    adpListNodeRemove (&umiCtxList, (ADP_NODE *)pUmiCtx);
    adpMutexGive (umiLock);
    adpMutexDelete (((UMI_CTX *)pUmiCtx)->lock);
    adpFree (pUmiCtx);
    }

/*******************************************************************************
*
* umiIoctl - send an ioctl request to another component
*
* This routine passes the ioctl request to the destination component.
*
* <pUmiCtx> points to the context returned by umiRegister. A user application 
* which has not registered itself with the UMI framework has to set this to
* NULL. If this argument is NULL, the caller cannot use the
* `UMI_IOCTL_OPT_ASYNC_CALL' option.
*
* <destId> should be set to the component ID of the component to which 
* the ioctl request is being made. 
* 
* <cmd> should be set to UMI command that needs to be executed by the 
* destination component. 
* 
* <pInBuf> is pointer to a request structure specific to the ioctl command. The 
* request structure should not contain pointers to any internal memory if the 
* destination component is not in the same address space as the caller.
*
* <inLen> is the length of the input buffer.
*
* <pOutBuf> is an ioctl command specific data buffer. This buffer is generally
* used for output but can also be used as an additional input buffer.
*
* <pOutLen> points to the length of the output buffer. 
* 
* <reqOpt> takes following values:
*
* UMI_IOCTL_OPT_ASYNC_CALL:
* This option means that this ioctl request does not wait for the ioctl handler 
* execution result if it can not be executed directly. This can be used if the
* caller ignores the result or if the caller wishes to handle the result 
* asynchronously. To handle a result asynchronously, the caller has to 
* specify a request id (<reqId> argument) which UMI will pass back to
* caller when the result is received. The caller must have a UMI context to 
* use this option. 
*
* UMI_IOCTL_OPT_SYNC_CALL:
* This option means that the caller wants to wait for the result from the ioctl 
* handler. A timeout can be specified in this case to limit the wait time.
* 
* UMI_IOCTL_OPT_NO_RESULT:
* This option indicates to the UMI framework that the caller does not require
* the result of the ioctl call.
*
* <timeout> specifies the amount of time any synchronous call will wait for
* the reply. For infinite wait set this to 0.
*
* <reqId> identifies this UMI ioctl request call. This has to be set when an
* asynchronous call is made.
* 
* 
* 
* RETURNS: value returned by ioctl handler. 
*/

int umiIoctl
    (
    void *          pUmiCtx,    /* context returned by umiRegister or NULL */
    UMI_COMP_ID     destId,     /* destination component ID */
    int             cmd,        /* ioctl command */
    void *          pInBuf,     /* input buffer for ioctl request */
    unsigned int    inLen,      /* length of input buffer */
    void *          pOutBuf,    /* pointer to output buffer */
    unsigned int *  pOutLen,    /* pointer to output buffer length */
    int             reqOpt,     /* ioctl request option */
    int             timeout,    /* timeout in microseconds */
    int             reqId       /* request ID for asynchronous call */
    )
    {
    UMI_CTX *       pSrcCtx;
    UMI_CTX *       pDstCtx;
    UMI_REQ_INFO *  pUmiReq;
    unsigned int    reqBufSize; 
    int             error = -1;
    char            buf[UMI_MAX_REQ_SIZE];
    char *          pBuf = buf;
    struct timeval  startTime;
    struct timeval  curTime;
    int             options;
    int             outLen = 0;
    int             reqSize;
    
    gettimeofday (&startTime, NULL);

    if (pUmiCtx == NULL)
        {
        pSrcCtx = pGlobalUmiCtx;
        reqOpt &= ~UMI_IOCTL_OPT_ASYNC_CALL;
        }
    else
        {
        pSrcCtx = (UMI_CTX *) pUmiCtx;
        }

    if (timeout)
        {
        options = ADP_MUTEX_NO_WAIT;
        }
    else
        {
        options = ADP_MUTEX_WAIT_FOREVER;
        }

    ADP_DEBUG ("Making request from %d --> %d\n", pSrcCtx->myId, destId);
    pSrcCtx->stats.lockTake++;

    if (adpMutexTake (pSrcCtx->lock,options, 0) != ST_OK)
        {
        errno = UMI_LOCK_FAILED;
        pSrcCtx->stats.lockTakeFail++;
        ADP_ERROR ("failed to take lock for compId: %d \n",pSrcCtx->myId);
        return (error);
        }

    pDstCtx = umiFindCtx (destId);

    if (pOutLen == NULL)
        pOutLen = &outLen;
    
    if (pDstCtx == NULL || (pDstCtx->umiOpt & UMI_OPT_MY_TASK_CTX) ||
        (destId == UMI_COMP_UMI) )
        {
        /* 
         * Either destination component is in different address space or it does
         * not support its ioctl handler to be called from other than its own
         * task context. Also all the ioctl request for UMI component is 
         * directed to kernel UMI component. Later when UMI supports some of 
         * ioctl commands in user space we will need to add one more check here
         * (probably based on the cmd value).
         */
        
        error = umiIoctlArgsToBuf ( &pBuf, UMI_MAX_REQ_SIZE, pInBuf, inLen, 
                                    pOutBuf, pOutLen,  &reqBufSize);

        if (error)
            {
            adpMutexGive(pSrcCtx->lock);
            pSrcCtx->stats.argsToBufFail++;
            pSrcCtx->stats.lockGive++;
            ADP_ERROR ("failed to convert ioctl args to buffer for"
                       "compId: %d \n",pSrcCtx->myId);
            return (error);
            }

        pUmiReq = (UMI_REQ_INFO *)pBuf;
        pUmiReq->cmd    = cmd;
        pUmiReq->srcId  = pSrcCtx->myId;
        pUmiReq->reqOpt = reqOpt;
        pUmiReq->reqId  = reqId;
        pUmiReq->req    = 1;

        if (sendto (pSrcCtx->sockFd, pUmiReq, reqBufSize,
                    0, (struct sockaddr *) &destId, sizeof(UMI_COMP_ID)) == -1)
            {
            /* errno will be set by the sendto call */
            adpMutexGive(pSrcCtx->lock);
            if (pBuf != buf)
                adpFree (pBuf);
            pSrcCtx->stats.ioctlSendToFail++;
            pSrcCtx->stats.lockGive++;
            ADP_DEBUG2 ("failed to send ioctl request: dst(%d) <--- src(%d) \n",
                       destId, pSrcCtx->myId);
            return (-1);
            }
        
        if ( (reqOpt & UMI_IOCTL_OPT_ASYNC_CALL) ||
             (reqOpt & UMI_IOCTL_OPT_NO_RESULT) )
            {
            adpMutexGive(pSrcCtx->lock);
            if (pBuf != buf)
                adpFree (pBuf);
            pSrcCtx->stats.asyncOrNoResultIoctlSuccess++;
            pSrcCtx->stats.lockGive++;
            ADP_DEBUG ("sent request dst(%d) <-- src(%d) using option %d\n",
                       destId, pSrcCtx->myId, reqOpt);
            return (0);
            }

        if (pBuf != buf )
            {
            adpFree (pBuf);
            pBuf = buf;
            }
        /* 
         * wait for the result. During this wait we might get some requests
         * for this component. If that is the case we process those until
         * we receive the reply for correct request or we timeout.
         */

        while (1)
            {
            if ((reqSize = umiRecvFrom (pSrcCtx->sockFd, &pBuf, 
                                        UMI_MAX_REQ_SIZE, timeout )) == -1)
                {
                ADP_ERROR ("request timeout dst(%d) <-- src(%d) \n", 
                           destId, pSrcCtx->myId);
                pSrcCtx->stats.syncWaitExited++;
                pSrcCtx->stats.lockGive++;
                adpMutexGive(pSrcCtx->lock);
                return (-1);
                }

            pUmiReq = (UMI_REQ_INFO *)pBuf;

            if (pUmiReq->req == 0)
                {
                /* it is a reply. make sure it is for the request we sent. */
                if ( (pUmiReq->srcId == destId ) && (pUmiReq->cmd == cmd) &&
                     (pUmiReq->reqId == reqId) )
                    {
                    break;
                    }
                else
                    {
                    /* reply for either unknown request or previous request */
                    }
                }
            else
                {
                if ((pUmiReq->srcId == destId) &&
                    (pUmiReq->reqOpt == UMI_IOCTL_OPT_SYNC_CALL))
                    {
                    umiProcessReq(pSrcCtx, pUmiReq);
                    }
                 else
                    {
                     /* we dont process the request when waiting for reply */
                     umiQueueRequest (pSrcCtx, pUmiReq, reqSize);
                    }
                 }

            /* update the timeout */
            if (timeout)
                {
                gettimeofday (&curTime, NULL);
                timeout -= (curTime.tv_sec - startTime.tv_sec) 
                           * MICROSECS_IN_SECS +
                           (curTime.tv_usec - startTime.tv_usec);
                gettimeofday (&startTime, NULL);
                }
            /* We restore pBuf here before calling umiRecvFrom which might
             * replace pBuf and thus leaking the memory if pBuf earlier was
             * dynamically allocated.
             */
            if (pBuf != buf)
                {
                adpFree (pBuf);
                pBuf = buf;
                }
            }
        umiBufToIoctlArgs ((char *)pUmiReq, &pInBuf, &inLen, &pOutBuf, pOutLen);
        error = pUmiReq->status;

        if (pBuf != buf)
            adpFree (pBuf);
        }
    else
        {
        UMI_REQ_INFO    umiReqInfo;

        umiReqInfo.srcId = pSrcCtx->myId;

        error = pDstCtx->handler (pDstCtx->arg1, &umiReqInfo, cmd, pInBuf, 
                                  inLen, pOutBuf, pOutLen);
        }
    adpMutexGive(pSrcCtx->lock);
    pSrcCtx->stats.syncIoctlSuccess++;
    pSrcCtx->stats.lockGive++;
    return (error);
    }

/*******************************************************************************
*
* umiListen - wait and process UMI ioctl requests
*
* This routine listens for UMI ioctl requests from other components and
* processes them.
*
* RETURNS: NONE
*/

void umiListen
    (
    void *  pUmiCtx,    /* context returned by umiRegister */
    int     timeout     /* timeout in microsec to wait, 0 for infinite wait */
    )
    {
    char            buf[UMI_MAX_REQ_SIZE];
    char *          pBuf;
    UMI_CTX *       pSrcCtx;
    struct timeval  startTime;
    struct timeval  curTime;
    int             options;
    int             nBytes;

    pSrcCtx = (UMI_CTX *)pUmiCtx;

    if (timeout)
        {
        options = ADP_MUTEX_NO_WAIT;
        }
    else
        {
        options = ADP_MUTEX_WAIT_FOREVER;
        }

    pSrcCtx->stats.lockTake++;

    if (adpMutexTake (pSrcCtx->lock,options, 0) != ST_OK)
        {
        errno = UMI_LOCK_FAILED;
        pSrcCtx->stats.lockTakeFail++;
        ADP_ERROR ("failed to take lock for compId: %d \n",pSrcCtx->myId);
        return;
        }

    gettimeofday (&startTime, NULL);

    pBuf = buf;
    
    while (1)
        {

        /* process the queued requests first */ 
        umiProcessQueuedRequests (pSrcCtx);

        nBytes = umiRecvFrom (pSrcCtx->sockFd, &pBuf, UMI_MAX_REQ_SIZE, 
                              timeout);

        if (nBytes == -1)
            {
            pSrcCtx->stats.recvFromFailed++;
            adpMutexGive(pSrcCtx->lock);
            pSrcCtx->stats.lockGive++;
            return ;
            }
        else if (nBytes <= sizeof (UMI_REQ_INFO))
            {
            ADP_DEBUG ("received request too small!(%d bytes)\n",nBytes);
            pSrcCtx->stats.reqTooShort++;
            }

        ADP_DEBUG ("Received a UMI request from %d \n",pSrcCtx->myId);
        umiProcessReq(pSrcCtx,(UMI_REQ_INFO *)pBuf);
                          
        /* update the timeout */
        if (timeout)
            {
            gettimeofday (&curTime, NULL);
            timeout -= (curTime.tv_sec - startTime.tv_sec) * MICROSECS_IN_SECS +
                       (curTime.tv_usec - startTime.tv_usec);
            if ((timeout <= 0) ||
                (((curTime.tv_sec * MICROSECS_IN_SECS) + curTime.tv_usec)
                 <((startTime.tv_sec * MICROSECS_IN_SECS) + startTime.tv_usec)))
                break;
            gettimeofday (&startTime, NULL);
            }
        if (pBuf != buf)
            {
            adpFree (pBuf);
            pBuf = buf;
            }
        }
    adpMutexGive(pSrcCtx->lock);
    pSrcCtx->stats.lockGive++;
    }

/* LOCAL functions */
/*******************************************************************************
*
* umiIoctlArgsToBuf - encode ioctl arguments into a buffer
*
* This routine encode the ioctl arguments into a buffer. The buffer format
* is as follows.
*   ----------------------------------------------------------
*   |UMI_REQ_INFO struct | inLen | pInBuf | outLen | pOutBuf |
*   -----------------------------------------------------------
* 
* The reqOpt controls if pOutBuf/pInBuf are actually copied into the buffer
* or just memory is allocated.
*
* RETURNS:  0 or -1
*
* INTERNAL
*/

static int umiIoctlArgsToBuf
    (
    char **         ppBuf,      /* pointer to buffer to encode into */
    unsigned int    bufLen,     /* length of pBuf */
    void *          pInBuf,     /* input buffer for ioctl request */
    unsigned int    inLen,      /* length of input buffer */
    void *          pOutBuf,    /* pointer to output buffer */
    unsigned int *  pOutLen,    /* pointer to output buffer length */
    unsigned int *  pBufSize    /* pointer to hold the actual size of request */
    )
    {
    char *          pBuf;       
    unsigned int    reqBufSize;
    
    reqBufSize = sizeof (UMI_REQ_INFO);
    
    reqBufSize +=  sizeof (unsigned int);

    reqBufSize += inLen;

    reqBufSize +=  sizeof (unsigned int);
    
    reqBufSize += *pOutLen;
    
    if (reqBufSize > bufLen )
        {
        *ppBuf = adpMalloc (reqBufSize);

        if (*ppBuf == NULL)
            {
            ADP_ERROR("umiIoctlArgsToBuf: failed to allocate memory\n");
            return (-1);
            }
        }

    pBuf = *ppBuf;

    pBuf +=  sizeof (UMI_REQ_INFO);

    memcpy ((char *)pBuf, (char *)&inLen, sizeof (unsigned int));
    pBuf += sizeof (unsigned int);
    
    memcpy ((void *)pBuf, (void *)pInBuf, inLen);
    pBuf += inLen;
    
    memcpy ((char *)pBuf, (char *)pOutLen, sizeof (unsigned int)); 
    pBuf += sizeof (unsigned int);
   
    if (pOutBuf) 
        memcpy(pBuf , pOutBuf, *pOutLen);
        
    *pBufSize = reqBufSize;

    return 0;
    }

/*******************************************************************************
*
* umiBufToIoctlArgs - read and copy ioctl arguments from buffer
*
* This routine copies ioctl arguments encoded in the buffer to local variables.
*
* RETURNS:  NONE
*
* INTERNAL
*/

static void umiBufToIoctlArgs
    (
    char *          pUmiBuf,    /* buffer containing ioctl arguments */
    void **         ppInBuf,    /* input buffer for ioctl request */
    unsigned int *  pInLen,     /* length of input buffer */
    void **         ppOutBuf,   /* pointer to output buffer */
    unsigned int *  pOutLen     /* pointer to output buffer length */
    )
    {
    char * pBuf;

    pBuf = pUmiBuf + sizeof (UMI_REQ_INFO);

    memcpy ((void *)pInLen, pBuf, sizeof (unsigned int));
    pBuf += sizeof (unsigned int);
        
    if (*pInLen)
        {
        if (*ppInBuf == NULL)
            {
            *ppInBuf = pBuf;
            }
        else
            {
            memcpy (*ppInBuf, pBuf, *pInLen);
            }
        pBuf += *pInLen;
        }

    memcpy ((void *)pOutLen, pBuf, sizeof (unsigned int));
    pBuf += sizeof (unsigned int);
        
    if (*pOutLen)
        {
        if (*ppOutBuf == NULL)
            {
            *ppOutBuf = pBuf;
            }
        else
            {
            memcpy (*ppOutBuf, pBuf, *pOutLen);
            }
        }
    }

/*******************************************************************************
*
* umiRecvFrom - receive an ioctl request/reply
*
* This routine receives an ioctl request or reply from another component. A 
* timeout can be specified to limit the time to wait for the ioctl req/rep.
* 
* RETURNS:  -1 for error or number of bytes 
*
* INTERNAL
*/

static int umiRecvFrom
    (
    int             sockFd,     /* socket descriptor */
    char **         ppBuf,      /* receive buffer */
    unsigned int    size,       /* receive buffer size */
    int             timeout     /* time in microseconds to wait, 0 for inf. */
    )
    {
    fd_set              readset;
    struct timeval      tv;
    struct timeval *    pTv;
    struct timeval      startTime;
    struct timeval      curTime;
    int                 retVal;
    char *              pBuf;
    int                 numBytes;
    
restartSelect:
    FD_ZERO (&readset);
    FD_SET (sockFd, &readset);

    if (timeout < 0)
        {
        return -1;
        }
    if (timeout)
        {
        tv.tv_sec  = timeout / MICROSECS_IN_SECS;
        tv.tv_usec = timeout % MICROSECS_IN_SECS;
        pTv = &tv;
        gettimeofday (&startTime, NULL);
        }
    else
        {
        pTv = NULL;
        }

    retVal = select (sockFd + 1, &readset, NULL, NULL, pTv);

    if (retVal == 0)
        {
        /* timeout occured */
        errno = UMI_RECV_TIMEOUT;
        return (-1);
        }
    else if (retVal == -1 )
        {
        if (errno == EINTR)
            {
            if (pTv)
                {
                gettimeofday (&curTime, NULL);
                timeout -= (curTime.tv_sec - startTime.tv_sec) 
                           * MICROSECS_IN_SECS +
                           (curTime.tv_usec - startTime.tv_usec);
                }
            goto restartSelect;
            }
        return (-1);
        }
    else
        {
        /* find the number of bytes available to read */
        numBytes = 0;
        ioctl (sockFd, FIONREAD, &numBytes);
        if (numBytes > size)
            {
            *ppBuf = adpMalloc (numBytes);

            if (*ppBuf == NULL)
                {
                ADP_ERROR ("umiRecvFrom: could not allocate memory \n");
                return -1;
                }
            size = numBytes;
            }
        pBuf = *ppBuf;
        retVal = recv (sockFd, pBuf, size, 0);
        }
    return (retVal);
    }
     
/*******************************************************************************
*
* umiFindCtx - find UMI context
*
* This routine seraches for the UMI context for the given component id in the
* UMI context list.
*
* RETURNS: pointer to UMI_CTX or NULL
*
* INTERNAL
*/

static UMI_CTX * umiFindCtx
    (
    UMI_COMP_ID     id      /* UMI component ID */
    )
    {
    UMI_CTX *   pUmiCtx;

    adpMutexTake (umiLock, ADP_MUTEX_WAIT_FOREVER, 0);

    pUmiCtx = (UMI_CTX *) adpListFirst (&umiCtxList);

    while (pUmiCtx)
        {
        if (pUmiCtx->myId == id )
            {
            adpMutexGive (umiLock);
            return pUmiCtx;
            }
        pUmiCtx = (UMI_CTX *) adpListNext ((ADP_NODE *)pUmiCtx);
        }
    adpMutexGive (umiLock);
    return (NULL);
    }

/*******************************************************************************
*
* umiIoctlHandler - UMI ioctl handler
*
* This routine is an ioctl handler for user space UMI. 
*
* RETURNS: 0 for success , -1 for error
*
* INTERNAL
*/

static int umiIoctlHandler
    (
    void *          arg1,       /* argument (passed during umiRegister) */
    UMI_REQ_INFO *  pReqInfo,   /* information about request */
    int             cmd,        /* ioctl command */
    void *          pInBuf,     /* input buffer */
    unsigned int    inLen,      /* input buffer length */
    void *          pOutBuf,    /* output buffer */
    unsigned int *  pOutLen     /* pointer to output buffer length */
    )
    {
    /* no ioctl commands supported yet */
    return (-1);
    }

/*******************************************************************************
*
* umiGenericIoctlHandler - UMI generic ioctl handler
*
* This routine handles generic ioctl calls on behalf of each component.
*
* RETURNS: 0 for success , -1 for error
*
* INTERNAL
*/

static int umiGenericIoctlHandler
    (
    void *          pUmiCtx,    /* umi context of the destination comp 
                                   of the request */
    UMI_REQ_INFO *  pReqInfo,   /* information about request */
    int             cmd,        /* ioctl command */
    void *          pInBuf,     /* input buffer */
    unsigned int    inLen,      /* input buffer length */
    void *          pOutBuf,    /* output buffer */
    unsigned int *  pOutLen     /* pointer to output buffer length */
    )
    {
    switch (cmd)
        { 
        case UMI_CMD_ADP_DEBUG:
            {
            UMI_COMP_ID dest = ((UMI_CTX *)pUmiCtx)->myId;
            int         debugLevel;
            
            if ( pInBuf == NULL && inLen == sizeof (int))
                {
                return ST_ERROR;
                }
            debugLevel = *(int *)pInBuf;
            adpModuleDebugLevelOn (dest, debugLevel);
            }
        break;

        case UMI_CMD_STATS:
            {
            UMI_CTX * pCtx;
            pCtx = (UMI_CTX *) pUmiCtx;
            if (!pCtx) return (ST_ERROR);
            memcpy (pOutBuf, (char *)&pCtx->stats, sizeof(UMI_STATS));
            *pOutLen = sizeof(UMI_STATS);
            }
        break;

        case UMI_CMD_KUMI_DEINIT:
            if (((UMI_CTX *)pUmiCtx)->sockFd != -1)
                {
                close (((UMI_CTX *)pUmiCtx)->sockFd);
                ((UMI_CTX *)pUmiCtx)->sockFd = -1;
                }
            break;
        default:
            return (ST_ERROR);
        }

    return (ST_OK);
    }

/*******************************************************************************
*
* umiProcessReq - process an umi ioctl request
*
* This routine is executed when UMI receives an ioctl request from other
* components.
*
* RETURNS: NONE
*
* INTERNAL
*/

static void umiProcessReq 
    (
    UMI_CTX *       pSrcCtx,    /* umi context of the receiver of the request */
    UMI_REQ_INFO *  pUmiReq     /* pointer to UMI ioctl request */
    )
    {
    void *          pInBuf;
    void *          pOutBuf;
    unsigned int    inLen;
    unsigned int    outLen;
    UMI_COMP_ID     destId;
    unsigned int    reqSize;

    if (pSrcCtx->handler || (pUmiReq->cmd < UMI_PRIVATE_IOCTL_START))
        {
        pInBuf = pOutBuf = NULL;

        umiBufToIoctlArgs ((char *)pUmiReq, &pInBuf, &inLen, &pOutBuf, &outLen);
        
        if ((pUmiReq->cmd < UMI_PRIVATE_IOCTL_START) && 
            ((pUmiReq->cmd != UMI_CMD_DB_UPDATE) && 
             (pUmiReq->cmd != UMI_CMD_IFDEV_EVENT)))
            {
            pUmiReq->status = umiGenericIoctlHandler (pSrcCtx, pUmiReq, 
                                                      pUmiReq->cmd, pInBuf,
                                                      inLen, pOutBuf, &outLen);
            }
        else
            {
            pUmiReq->status = pSrcCtx->handler ( pSrcCtx->arg1, pUmiReq,
                                                 pUmiReq->cmd, pInBuf, inLen,
                                                 pOutBuf, &outLen);
            }
        if (pUmiReq->req == 0) 
            {
            /* 
             * either it was an async reply
             */
            pSrcCtx->stats.recvReply++;
            ADP_DEBUG2 ("processed a reply dst(%d) <-- src(%d) \n",
                       pSrcCtx->myId, pUmiReq->srcId);
            return;
            }
        
        if (pUmiReq->reqOpt & UMI_IOCTL_OPT_NO_RESULT)
            {
            /* src does not need reply */
            pSrcCtx->stats.reqWitNoReply++;
            ADP_DEBUG2 ("request with no result option dst(%d) <-- src(%d) \n",
                       pSrcCtx->myId, pUmiReq->srcId);
            return;
            }

        /* send the reply back */
        destId = pUmiReq->srcId;
        pUmiReq->srcId = pSrcCtx->myId;
        pUmiReq->req = 0;

        reqSize = sizeof (UMI_REQ_INFO) + 2 * sizeof (unsigned int) + inLen +
                  outLen;
        sendto (pSrcCtx->sockFd, pUmiReq, reqSize, 0, 
                (struct sockaddr *) &destId, sizeof(UMI_COMP_ID) ) ;
        ADP_DEBUG ("sent a reply src(%d) ---> dst(%d)\n",
                   pSrcCtx->myId, destId);
        pSrcCtx->stats.replySent++;
        }
    }

/*******************************************************************************
*
* umiQueueRequest - initializes the bridge library
*
* This function queues a request to the UMI queue.
* 
* RETURNS: OK or ERROR
*/

static void umiQueueRequest
    (
    UMI_CTX *       pSrcCtx,  /* pointer to the source context */
    UMI_REQ_INFO *  pUmiReq,  /* UMI request */
    int             reqSize   /* request size */  
    )
    {
    UMI_REQ_BUF *   pReqBuf;
    int             reqBufLen;
 
     reqBufLen = sizeof (UMI_REQ_BUF) + reqSize;
 
    pReqBuf = adpMalloc (reqBufLen);
 
     if (pReqBuf == NULL)
         {
         ADP_ERROR ("adpMalloc failed\n");
         umiProcessReq (pSrcCtx, pUmiReq);
         return;
         }
     pReqBuf->pUmiReq = (UMI_REQ_INFO *) (pReqBuf + 1);
     memcpy (pReqBuf->pUmiReq, pUmiReq, reqSize);
 
     adpListNodeAdd (&pSrcCtx->recvQ, (ADP_NODE *)pReqBuf);
 
     return;
     }
    
/*******************************************************************************
*
* umiProcessQueuedRequests - process queued requests
*
* This function processes all the queued UMI requests 
* 
* RETURNS: NONE
*/

static void umiProcessQueuedRequests
    (
    UMI_CTX * pUmiCtx  /* pointer to umi context */
    )
    {
    ADP_NODE *  pReqNode;
    ADP_NODE *  pNextReqNode;

    /* process and free the queued requests */
    pReqNode = adpListFirst (&pUmiCtx->recvQ);

    while (pReqNode)
        {
        pNextReqNode = adpListNext (pReqNode);
        adpListNodeRemove (&pUmiCtx->recvQ, pReqNode);
        umiProcessReq (pUmiCtx, ((UMI_REQ_BUF *)pReqNode)->pUmiReq);
        adpFree (pReqNode);
        pReqNode = pNextReqNode;
        }
    }
