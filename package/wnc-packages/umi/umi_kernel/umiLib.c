/* umiLib.c - Implementation of UMI api for kernel space components */

/* Copyright (c) 2005, TeamF1, Inc. */
    
/*
modification history
--------------------
01g,05jan08,smv replaced spin_lock_bh and spin_unlock_bh with UMI_SPIN_LOCK
                and UMI_SPIN_UNLOCK to avoid kernel oops with 2.6.23 kernel
01f,06dec07,smv changing kmalloc flags based on interrupt context in
                umiIoctlArgsToBuf ().
01e,10oct05,smv fixed endian issues.
01d,04aug05,smv changed debug levels.
01c,05may05,kb  added UMI_CMD_FIND_COMP in umiIoctlHandler.
01b,08apr05,rks changed umiIoctlArgsToBuf to remove UMI_MAX_REQ_SIZE limitation
01a,25feb05,rks written.
*/

/*
DESCRIPTION
UMI library implements a framework to allow communication between a TeamF1's 
product or sub-component to other TeamF1's products and sub-components. It 
provides a ioctl-like interface to send request from one component to other 
component.
   
INCLUDE FILES: umi.h

*/

/* includes */

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif
#include <linux/interrupt.h>
#include <linux/spinlock.h>

#include "umi.h"
#include "umiP.h"
/* defines */

#ifdef DEBUG
#define PRINTK(x...)   printk(x)
#else
#define PRINTK(x,...)
#endif

int compIdMax = UMI_COMP_PNAC_USR;


/* typedefs */


/* externs */


/* globals */

UMI_ID_NAME_MAP umiIdNameMap[] = {
UMI_ID_NAME_MAP_LIST
};

/* locals */

static LIST_HEAD (umiCtxList);
static DEFINE_SPINLOCK(umiLock);
static UMI_CTX *    pKernUmiCtx;
static UMI_COMP_ID  lastUmiId = UMI_COMP_ID_MAX;
static int          lastUmiIdOverflow = 0;

/* imports */


/* forward declarations */

static int          umiIoctlHandler (void * arg1, UMI_REQ_INFO * pUmiReq,
                                     int cmd, void * pInBuf, unsigned int inLen,
                                     void * pOutBuf, unsigned int * pOutLen) ;

static int          umiIoctlArgsToBuf ( char ** ppBuf, unsigned int bufLen, 
                                        void * pInBuf, unsigned int inLen,
                                        void * pOutBuf, unsigned int * pOutLen,
                                        unsigned int *  pBufSize );
static UMI_CTX *    umiFindCtx ( UMI_COMP_ID id );

/*******************************************************************************
*
* umiInit - initialize UMI library
*
* This routine initializes UMI global variables.
*
* RETURNS: 0 for success , -1 for error
*/
int umiInit (void)
    {
    pKernUmiCtx = umiRegister (UMI_COMP_UMI, UMI_OPT_ANY_TASK_CTX, 
                               umiIoctlHandler, NULL);
    if (pKernUmiCtx == NULL)
        {
        printk(KERN_ERR "kernel UMI registration failed!\n");
        return -1;
        }
    return (0);
    }
/*******************************************************************************
*
* umiRegister - register component's ioctl handler
*
* This routine lets the component register its ioctl handler with UMI. This 
* handler is invoked whenever a ioctl request is received for this component.
* The component can specify if the ioctl handler can be invoked from any task
* context or in its task context. If it is later, the component need to call 
* umiListen to accept ioctl requests.
*
* Currently umiOpt can take following values:
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

    PRINTK (KERN_DEBUG "%s (%x,%x)\n", __func__, myId, umiOpt);

    pUmiCtx = (UMI_CTX *) kmalloc (sizeof (UMI_CTX), GFP_KERNEL);

    if (pUmiCtx == NULL)
        {
        return (NULL);
        }
    

    if (umiOpt & UMI_OPT_RESERVED)
        {
        pUmiCtx->compType = UMI_COMP_TYPE_USER;

        /* 
         * User level UMI instance is registered with a reserved id so that
         * request and reply (using UMI context) could be routed to correct
         * instance.
         */
        if (myId == UMI_COMP_UMI)
            {
reselectId:
            myId = ++lastUmiId;
            if (myId == 0)
                {
                /* Here we take care of the possibility of lastUmiId getting
                 * overflowed
                 */
                myId = lastUmiId = UMI_COMP_ID_MAX + 1;
                lastUmiIdOverflow = 1;
                }
            /* if lastUmiIdOverflow is 1, make sure this id is not used.*/
            if (lastUmiIdOverflow && umiFindCtx(myId))
                {
                goto reselectId;
                }
            }
        }
    else
        {
        pUmiCtx->compType = UMI_COMP_TYPE_KERNEL;
        }
    
    pUmiCtx->myId = myId ;
    pUmiCtx->handler = handler;
    pUmiCtx->umiOpt = umiOpt;
    pUmiCtx->arg1 = arg1;

    UMI_SPIN_LOCK (umiLock);
    
    if (pUmiCtx->compType == UMI_COMP_TYPE_USER ||
        myId == UMI_COMP_UMI)
        {
        /* 
         * user components (and kernel UMI) are added at the end of the list 
         * to speed up the lookup for kernel components.
         */
        list_add_tail ((struct list_head *)pUmiCtx, &umiCtxList);
        }
    else
        {
        list_add ((struct list_head *)pUmiCtx, &umiCtxList);
        }
    UMI_SPIN_UNLOCK (umiLock);

    return (pUmiCtx);
    }

/*******************************************************************************
*
* umiIoctlHandler - kernel UMI ioctl handler
*
* This is kernel UMI's ioctl handler. Currently it supports UMI_CMD_LIST_COMP 
* which returns component ids of the registerd components and UMI_CMD_FIND_COMP
* ioctl command for checking whether the component is already registered or not
*
* RETURNS: 0 for success , -1 for error
*/
static int umiIoctlHandler 
    (
    void *          arg1,       /* argument passed during umiRegister */
    UMI_REQ_INFO *  pUmiReq,    /* pointer to UMI_REQ_INFO */
    int             cmd,        /* ioctl command */
    void *          pInBuf,     /* pointer to input buffer */
    unsigned int    inLen,      /* input buffer length */
    void *          pOutBuf,    /* pointer to output buffer */
    unsigned int *  pOutLen     /* pointer to output buffer length */
    )
    {
    UMI_CTX *           pUmiCtx;
    struct list_head *  pTmp;   
    int                 len;
    int *id;

    switch (cmd)
        {
        case UMI_CMD_LIST_COMP:
            len = 0;
            UMI_SPIN_LOCK (umiLock);
            list_for_each (pTmp, &umiCtxList)
                {
                pUmiCtx = (UMI_CTX *)pTmp;
                if (len + sizeof (UMI_COMP_ID) > *pOutLen)
                    {
                    break;
                    }
                *(UMI_COMP_ID *)(pOutBuf) = pUmiCtx->myId;

                pOutBuf = (char *)pOutBuf + sizeof (UMI_COMP_ID);
                len    += sizeof (UMI_COMP_ID);
                }
            UMI_SPIN_UNLOCK (umiLock);
            *pOutLen = len;
            break;

        case UMI_CMD_FIND_COMP:
            UMI_SPIN_LOCK (umiLock);
            *(UMI_COMP_ID *)(pOutBuf) = -1;
            list_for_each (pTmp, &umiCtxList)
                {
                pUmiCtx = (UMI_CTX *)pTmp;
                id = (int *)pInBuf;
                if (pUmiCtx->myId == *id)
                    {
                    *(UMI_COMP_ID *)(pOutBuf) = 0;
                    break;
                    }
                }
            len = sizeof (UMI_COMP_ID);
            UMI_SPIN_UNLOCK (umiLock);
            *pOutLen = len;
            break;

        default:
            *pOutLen = 0;
            return -1;
        }
    return 0;
    }

/*******************************************************************************
*
* umiIoctl - send a ioctl request to other component
*
* This routine passes the ioctl request to the destination component.
*
* pUmiCtx points to the context returned by umiRegister or NULL.
* 
* pInBuf points to two different type of buffer based on if the request is from
* a user level component or kernel level component. In case of first it points
* to a buffer which has ioctl arguments encoded in it along with UMI_REQ_INFO.
* In later case it is one argument of ioctl handler.
* 
* reqOpt, timeout and reqId is ignored in kernel UMI.
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
    UMI_REQ_INFO    reqInfo;
    UMI_REQ_INFO *  pUmiReq;
    unsigned int    reqBufSize; 
    int             error = -1;
    int             outLen = 0;
    char *          srcCompName="UNKNOWN";
    char *          destCompName = srcCompName;

    if (pUmiCtx == NULL)
        {
        pUmiCtx = pKernUmiCtx;
        }

    pSrcCtx = pUmiCtx;
    if (pOutLen == NULL)
        pOutLen = &outLen;

    if (pSrcCtx->myId <= compIdMax) 
        {
        srcCompName = umiIdToName (pSrcCtx->myId);
        }
    else if (pSrcCtx->myId > UMI_COMP_ID_MAX)
        {
        srcCompName = umiIdToName (UMI_COMP_UMI);
        }
    if (destId <= compIdMax)
        {
        destCompName = umiIdToName (destId);
        }
    else if (destId > UMI_COMP_ID_MAX)
        {
        destCompName = umiIdToName(UMI_COMP_UMI);
        }
    /*
     * Following check is to avoid too many printk because of timer handler 
     * queuing timeout events to its own component.
     */
    if (pSrcCtx->myId != destId)
        {
        PRINTK (KERN_DEBUG "%s srcId=%d(%s) --> destId=%d(%s) "
                "cmd=%d inLen=%d outLen=%d\n",
                __func__, pSrcCtx->myId, srcCompName, 
                destId, destCompName, cmd, inLen, *pOutLen);
        }

    if (pSrcCtx->compType == UMI_COMP_TYPE_USER)
        {
        pUmiReq = (UMI_REQ_INFO *)pInBuf;
        if (pUmiReq->srcId != pSrcCtx->myId)
            {
            PRINTK (KERN_DEBUG "setting srcId from %d to %d\n",pUmiReq->srcId, 
                    pSrcCtx->myId);
            }
        pUmiReq->srcId = pSrcCtx->myId;
        }
    else
        {
        reqInfo.srcId = pSrcCtx->myId;
        reqInfo.reqOpt = reqOpt;
        reqInfo.reqId = reqId;
        reqInfo.cmd = cmd;
        pUmiReq = &reqInfo;
        }

    pDstCtx = umiFindCtx (destId);

    if (pDstCtx == NULL)
        {
        PRINTK (KERN_ERR "destination component (%d) not found.\n",destId);
        return error;
        }

    if (pSrcCtx->compType == pDstCtx->compType) 
        {
        /* 
         * both components are in same space , no need to build/convert
         * ioctl args. just execute the handler.
         */
        error = pDstCtx->handler (pDstCtx->arg1, pUmiReq, cmd, pInBuf, inLen, 
                                  pOutBuf, pOutLen);
        }
    else if (pSrcCtx->compType == UMI_COMP_TYPE_USER)
        {
        /* convert from umi request buffer to ioctl args */

        cmd = pUmiReq->cmd;
        reqId = pUmiReq->reqId;

        pInBuf = (char *)pInBuf + sizeof (UMI_REQ_INFO);
        inLen = *(unsigned int *) (pInBuf);
        pInBuf = (char *)pInBuf + sizeof (unsigned int);
        pOutLen =(unsigned int *)((char *)pInBuf + inLen);

        if (*pOutLen == 0)
            {
            pOutBuf = NULL;
            }
        else
            {
            pOutBuf = pOutLen + 1;
            }
        error = pDstCtx->handler (pDstCtx->arg1, pUmiReq, cmd, pInBuf, inLen, 
                                  pOutBuf, pOutLen);

        if (pUmiReq->reqOpt & UMI_IOCTL_OPT_NO_RESULT)
            {
            return (error);
            }

        pUmiReq->req = 0;
        pUmiReq->reqId = reqId;
        pUmiReq->cmd = cmd;
        pUmiReq->srcId = pDstCtx->myId;
        pUmiReq->status = error;

        reqBufSize = sizeof(UMI_REQ_INFO) + 2 * sizeof (unsigned int) + inLen +
            *pOutLen;

        error = pSrcCtx->handler (pSrcCtx->arg1, pUmiReq , 0, (void *)pUmiReq,
                                  reqBufSize, 0, 0);

        }
    else 
        {
        /* convert from ioctl args to umi request buffer */
        UMI_REQ_INFO *  pUmiReq;
        char            buf[512]; /* UMI_MAX_REQ_SIZE */
        char *          pBuf = buf;

        error = umiIoctlArgsToBuf ( &pBuf, 512, pInBuf, inLen, 
                                    pOutBuf, pOutLen, &reqBufSize);
        if (error)
            {
            return (error);
            }

        pUmiReq = (UMI_REQ_INFO *)pBuf;
        pUmiReq->cmd    = cmd;
        pUmiReq->srcId  = pSrcCtx->myId;
        pUmiReq->reqId  = 0;
        pUmiReq->req    = UMI_IOCTL_REQUEST;
        pUmiReq->reqOpt = UMI_IOCTL_OPT_NO_RESULT;

        pInBuf = (char *) pUmiReq;
        inLen = reqBufSize;
        pOutBuf = NULL;
        *pOutLen = 0;

        error = pDstCtx->handler (pDstCtx->arg1, pUmiReq, 0 , pInBuf, inLen, 
                                  pOutBuf, pOutLen);
        if (pBuf != buf)
            {
            kfree (pBuf);
            }
        }
    return (error);
    }

/*******************************************************************************
*
* umiIoctlArgsToBuf - serialize ioctl arguments into a buffer
*
* This routine serializes the ioctl arguments into a buffer. The buffer format
* is as follows.
*   ----------------------------------------------------------
*   |UMI_REQ_INFO struct | inLen | pInBuf | outLen | pOutBuf |
*   -----------------------------------------------------------
* 
* The reqOpt controls if pOutBuf/pInBuf are actually copied into the buffer
* or just memory is allocated.
*
* RETURNS:  pointer to the allocated buffer or NULL
*/
static int umiIoctlArgsToBuf
    (
    char **         ppBuf,       /* pointer to buffer to encode into */
    unsigned int    bufLen,     /* length of pBuf */
    void *          pInBuf,     /* input buffer for ioctl request */
    unsigned int    inLen,      /* length of input buffer */
    void *          pOutBuf,    /* pointer to output buffer */
    unsigned int *  pOutLen,    /* pointer to output buffer length */
    unsigned int *  pBufSize    /* pointer to hold the actual size of request */
    )
    {
    unsigned int    reqBufSize;
    char *          pBuf;
    
    reqBufSize = sizeof (UMI_REQ_INFO);
    
    reqBufSize +=  sizeof (unsigned int);

    reqBufSize += inLen;

    reqBufSize +=  sizeof (unsigned int);

    reqBufSize += *pOutLen;

    if (reqBufSize > bufLen )
        {
        *ppBuf = kmalloc ( reqBufSize, (in_interrupt() ? GFP_ATOMIC : GFP_KERNEL));
        if (*ppBuf == NULL)
            {
            return (-1);
            }
        }

    pBuf = *ppBuf;

    pBuf +=  sizeof (UMI_REQ_INFO);

    memcpy ((char *)pBuf, (char *)&inLen, sizeof(unsigned int));
    pBuf += sizeof (unsigned int);
    
    memcpy (pBuf, pInBuf, inLen);
    pBuf += inLen;

    memcpy ((char *)pBuf, (char *)pOutLen, sizeof(unsigned int));
    pBuf += sizeof (unsigned int);
    
    memcpy(pBuf , pOutBuf, *pOutLen);

    *pBufSize = reqBufSize;
    return 0;
    }

/*******************************************************************************
*
* umiFindCtx - find UMI context
*
* This routine seraches for the UMI context for the given component id in the
* UMI context list.
*
* RETURNS: pointer to UMI_CTX or NULL
*/

static UMI_CTX * umiFindCtx
    (
    UMI_COMP_ID     id   /* UMI component ID */
    )
    {
    UMI_CTX *           pUmiCtx;
    struct list_head *  pTmp;   

    UMI_SPIN_LOCK (umiLock);
    list_for_each (pTmp, &umiCtxList)
        {
        pUmiCtx = (UMI_CTX *)pTmp;

        if (pUmiCtx->myId == id )
            {
            UMI_SPIN_UNLOCK (umiLock);
            return pUmiCtx;
            }
        }
    UMI_SPIN_UNLOCK (umiLock);
    return (NULL);
    }
/*******************************************************************************
*
* umiShowCtx - displays list of component registered with UMI
*
* This routine displays the component ids and there handler registered with 
* kernel UMI.
*
* RETURNS: NONE
*/
void umiShowCtx (void)
    {
    UMI_CTX *           pUmiCtx;
    struct list_head *  pTmp;   

    UMI_SPIN_LOCK (umiLock);
    list_for_each (pTmp, &umiCtxList)
        {
        pUmiCtx = (UMI_CTX *)pTmp;

        PRINTK (KERN_INFO "id=%d handler=%x\n",pUmiCtx->myId, 
                                            (unsigned int)pUmiCtx->handler);
        }
    UMI_SPIN_UNLOCK (umiLock);
    }

/*******************************************************************************
*
* umiUnRegister - unregister a component
*
* This routine removes component's context from global context list.
*
* RETURNS: NONE
*/
void umiUnRegister 
    (
    void *  pUmiCtx /* UMI context */
    )
    {
    PRINTK (KERN_DEBUG "%s (%x)\n", __func__, ((UMI_CTX *) pUmiCtx)->myId);
    UMI_SPIN_LOCK (umiLock);
    list_del ((struct list_head *)pUmiCtx);
    UMI_SPIN_UNLOCK (umiLock);
    kfree (pUmiCtx);
    }

/*******************************************************************************
*
* umiDeInit - de-initalize UMI
*
* This routine de-initializes the UMI. It cleans up all the UMI contexts that
* are registered with it.
*
* RETURNS: NONE
*/

void umiDeInit (void)
    {
    struct list_head *  pTmp;   
    struct list_head *  pNext;   
    UMI_CTX *           pUmiCtx;
    
    UMI_SPIN_LOCK (umiLock);

    list_for_each_safe (pTmp, pNext, &umiCtxList)
        {
        pUmiCtx = (UMI_CTX *)pTmp;

        if ( pUmiCtx->compType == UMI_COMP_TYPE_USER)
            {
            umiIoctl (pKernUmiCtx, pUmiCtx->myId, UMI_CMD_KUMI_DEINIT, 
                      NULL, 0, NULL, NULL, 0, 0, 0);
            }
        else
            {
            umiUnRegister (pUmiCtx);
            }
        }
    INIT_LIST_HEAD (&umiCtxList);
    UMI_SPIN_UNLOCK (umiLock);
    }

EXPORT_SYMBOL(umiRegister);
EXPORT_SYMBOL(umiIoctl);
EXPORT_SYMBOL(umiUnRegister);
