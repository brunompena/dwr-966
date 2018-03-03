/* af_umi.c - Implementation of AF_UMI socket family */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01e,05jan08,smv replaced spin_lock_bh and spin_unlock_bh with UMI_SPIN_LOCK
                and UMI_SPIN_UNLOCK to avoid kernel oops with 2.6.23 kernel
01d,10oct05,smv added support for 2.4.x.
01c,04aug05,smv changed debug levels.
01b,10may05,rks added umiSockIoctl
01a,11feb05,rks written.
*/

/*
DESCRIPTION
UMI library implements a framework to allow communication between a TeamF1's
product or sub-component to other TeamF1's products and sub-components. It
provides a ioctl-like interface to send request from one component to other
component.

A new socket family AF_UMI is used to pass ioctl request between components
running in different address space.

*/

/* includes files */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <asm/ioctls.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif


#include "umi.h"
#include "umiP.h"


MODULE_AUTHOR("TeamF1, Inc.");
MODULE_DESCRIPTION("kernel UMI");

/* defines */

#ifdef DEBUG
#define PRINTK(x...)   printk(x)
#else
#define PRINTK(x,...)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define sk_protinfo protinfo.destruct_hook
#endif

/* typedefs */

typedef struct {
    void *              pCookie;/* cookie returned by umiRegister */
    struct socket *     sock;   /* back pointer to socket */
    struct list_head    recvq;  /* receive q to queue umi ioctl requests */
    spinlock_t          sockLock;/* lock for syncronization */
    wait_queue_head_t * sleep;  /* wait q to sleep when there is no data */
}UMI_SOCK;

typedef struct {
    struct list_head    listEntry;
    char *              pBuf;
    int                 len;
}UMI_REQ_BUF;

/* locals */

static int          umiClose ( struct socket * sock);

static unsigned int umiPoll ( struct file * file, struct socket * sock,
                              poll_table * wait);
static int          umiSockIoctl (struct socket *, unsigned int, unsigned long);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
static int          umiSendMsg ( struct kiocb * iocb, struct socket * sock, 
                                struct msghdr * m, size_t totalLen );
static int          umiRecvMsg ( struct kiocb * iocb, struct socket * sock, 
                                struct msghdr * msg, size_t size, int flags);
#else
static int          umiSendMsg ( struct socket * sock, struct msghdr * m,
                                int totalLen, struct scm_cookie * scm);
static int          umiRecvMsg ( struct socket * sock, struct msghdr * msg,
                                 int size, int flags, struct scm_cookie * scm);
#endif

static int          umiCreate (struct net *pNet, struct socket *sock,
                               int protocol, int kern);

static int          umiSockHandler ( void * arg1, UMI_REQ_INFO * pUmiReq,
                                     int cmd, void * pInBuf,
                                     unsigned int inLen, void * pOutBuf,
                                     unsigned int *  pOutLen );

static struct proto_ops umi_proto_ops = {
    family:         AF_UMI,
    release:        umiClose,
    bind:           sock_no_bind,
    connect:        sock_no_connect,
    socketpair:     sock_no_socketpair,
    accept:         sock_no_accept,
    getname:        sock_no_getname,
    poll:           umiPoll,
    ioctl:          umiSockIoctl,
    listen:         sock_no_listen,
    shutdown:       sock_no_shutdown, 
    setsockopt:     sock_no_setsockopt,
    getsockopt:     sock_no_getsockopt,
    sendmsg:        umiSendMsg,
    recvmsg:        umiRecvMsg,
    mmap:           sock_no_mmap,
    sendpage:       sock_no_sendpage,
};

static struct net_proto_family umi_family_ops = {
    .family = AF_UMI,
    .create = umiCreate,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    .owner  = THIS_MODULE,
#endif
};
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,11)
static struct proto umi_proto = {
    .name     = "UMI",
    .owner    = THIS_MODULE,
    .obj_size = sizeof(struct sock),
};
#endif
/*******************************************************************************
*
* umiLoad - load UMI kernel module
*
* This function registers AF_UMI socket family with kernel and initialize kernel
* umi library.
*
* RETURNS: 0 for success or error value
*/
static int __init umiLoad (void)
    {
    int ret;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,11)
    ret = proto_register (&umi_proto, 0);
    if (ret != 0)
        {
        printk(KERN_ERR "UMI proto registration failed %d\n",ret);
        return ret;
        }
#endif
    
    ret = sock_register(&umi_family_ops);
    
    if (ret != 0)
        {
        printk(KERN_ERR "AF_UMI registration failed %d\n",ret);
        return ret;
        }

    ret = umiInit();

    if (ret != 0)
        {
        printk(KERN_ERR "umi initialization failed %d\n",ret);
        sock_unregister(AF_UMI);
        return ret;
        }
    
    printk(KERN_INFO "kernel UMI module loaded\n");

    return 0;
    }
/*******************************************************************************
*
* umiUnload - unload UMI module
*
* This function unregisters AF_UMI socket family and de-initialize kernel
* umi library.
*
* RETURNS: NONE
*/

static void __exit umiUnload (void)
    {
    umiDeInit();

    sock_unregister(AF_UMI);

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,11)
    proto_unregister(&umi_proto);
#endif    

    printk(KERN_INFO "kernel UMI module unloaded\n");

    }
/*******************************************************************************
*
* umiCreate - setup a new AF_UMI socket
*
* This function is called by generic socket library when a socket system called
* by the application. This routine setups UMI specific parameters in the socket.
*
* RETURNS: NONE
*/
static int umiCreate
    (
    struct net    * pNet,
    struct socket * pSocket,    /* pointer to socket */
    int             compId,      /* umi component Id */
    int	            kern
    )
    {
    UMI_SOCK *      pUmiSock;
    struct sock *   sk;

    PRINTK (KERN_DEBUG "%s (%x , %x)\n",
                       __func__, (unsigned int)pSocket, compId);

    pSocket->ops = &umi_proto_ops;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
    sk = sk_alloc (pNet, AF_UMI, GFP_KERNEL, &umi_proto);
#elif LINUX_VERSION_CODE > KERNEL_VERSION(2,6,11)
    sk = sk_alloc (AF_UMI, GFP_KERNEL, &umi_proto, 1);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    sk = sk_alloc(AF_UMI, GFP_KERNEL, 1, NULL);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
    sk = sk_alloc(AF_UMI, GFP_ATOMIC, 1);    
#endif
    if (sk == NULL)
        {
        return (-ENOMEM);
        }

    sock_init_data(pSocket, sk);

    pUmiSock = (UMI_SOCK *) kmalloc (sizeof(UMI_SOCK), GFP_ATOMIC);

    if (pUmiSock == NULL)
        {
        sk_free (sk);
        return -ENOMEM;
        }

    pUmiSock->pCookie = umiRegister ( compId, 
                                    UMI_OPT_ANY_TASK_CTX | UMI_OPT_RESERVED,
                                    umiSockHandler, pUmiSock);

    if (pUmiSock->pCookie == NULL)
        {
        sk_free (sk);
        kfree (pUmiSock);
        return (-1);
        }

    pUmiSock->sock = pSocket;
    pUmiSock->sleep = &pSocket->wq->wait;
    init_waitqueue_head (pUmiSock->sleep); 
    spin_lock_init (&pUmiSock->sockLock);
    INIT_LIST_HEAD (&pUmiSock->recvq);

    sk->sk_protinfo = pUmiSock;
    
    pSocket->sk = sk;
    
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)) &&  \
     (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12)))  
    sk_set_owner(sk, THIS_MODULE);
#endif 
    return 0;
    }
/*******************************************************************************
*
* umiPoll - check if data is available to read
*
* This function is invoked when user level umi library calls select on UMI
* socket to check for received data.
*
* RETURNS: 0 or POLLIN
*/
static unsigned int umiPoll 
    (
    struct file *   file,   /* socket file pointer */
    struct socket * sock,   /* socket pointer */
    poll_table *    wait    /* poll table */
    )
    {
    UMI_SOCK *  pUmiSock;

    pUmiSock = (UMI_SOCK *) sock->sk->sk_protinfo;

    poll_wait (file, pUmiSock->sleep, wait);

    if (list_empty(&pUmiSock->recvq))
        {
        return 0;
        }
    return (POLLIN);
    }

/*******************************************************************************
*
* umiSockIoctl - handle socket ioctl request
*
* This function handles ioctl requests for the UMI socket.
*
* RETURNS: OK or ERROR
*/
static int  umiSockIoctl 
    (
    struct socket * pSocket,   /* socket pointer */
    unsigned int    cmd,       /* ioctl command */
    unsigned long   arg        /* ioctl argument */
    )
    {
    UMI_SOCK *          pUmiSock;            
    struct list_head *  pTmp;
    int                 err = 0;
    
    switch (cmd)
        {
        case SIOCINQ:
            
            pUmiSock = (UMI_SOCK *)pSocket->sk->sk_protinfo;
            list_for_each (pTmp, &pUmiSock->recvq)
                {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))                
                err = put_user (((UMI_REQ_BUF *)pTmp)->len, (int __user *)arg);
#else                
                err = put_user (((UMI_REQ_BUF *)pTmp)->len, (int *)arg);
#endif
                break;
                }
            if (pTmp == NULL)
                {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))          
                err = put_user (0, (int __user *)arg);
#else                 
                err = put_user (0, (int *)arg);
#endif                
                }
            
            return err;
        }
    return sock_no_ioctl (pSocket, cmd, arg);
    }

/*******************************************************************************
*
* umiSendMsg - send a message to destination component 
*
* This function is invoked when user level umi library calls sendto on UMI 
* socket to pass an ioctl request to the destination component.
*
* RETURNS: number of bytes sent, negative for error
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
static int umiSendMsg 
    (
    struct kiocb *      iocb,   /* unused */
    struct socket *     pSocket,/* pointer to socket */ 
    struct msghdr *     m,      /* message containing ioctl request buffer */
    size_t              len     /* length of ioctl request buffer */
    )
#else    
static int umiSendMsg 
    (
    struct socket *     pSocket,/* pointer to socket */ 
    struct msghdr *     m,      /* message containing ioctl request buffer */
    int                 len,    /* length of ioctl request buffer */
    struct scm_cookie * scm     /* ignored */
    )
#endif
    {
    char *          pBuf;
    int             error;
    unsigned int    pOutLen = 0;
    UMI_SOCK *      pUmiSock;

    PRINTK (KERN_DEBUG "%s (%x,%x,%x)\n", __func__, (unsigned int)pSocket, 
            (unsigned int)m, len);

    pUmiSock = (UMI_SOCK *)pSocket->sk->sk_protinfo;

    if (m->msg_iovlen != 1)
        {
        pBuf = kmalloc (len, GFP_ATOMIC);

        if (pBuf == NULL)
            {
            return (-ENOMEM);
            }
        memcpy_fromiovec (pBuf, m->msg_iov, len);
        error = umiIoctl (pUmiSock->pCookie, *(UMI_COMP_ID *)m->msg_name, 0, 
                          pBuf, len, NULL, &pOutLen, 0, 0, 0);
        kfree (pBuf);
        return error;
        }

    return umiIoctl (pUmiSock->pCookie, *(UMI_COMP_ID *)m->msg_name, 0, 
                     m->msg_iov->iov_base, len, NULL, &pOutLen, 0, 0, 0);
    }

/*******************************************************************************
*
* umiRecvMsg - receive a message from a component
*
* This function is invoked when user level umi library calls recvfrom on UMI
* socket to receive an ioctl request from other component.
*
* RETURNS: number of bytes received, negative for error
*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
static int umiRecvMsg 
    ( 
    struct kiocb *      iocb,       /* unused */
    struct socket *     pSocket,    /* pointer to socket */
    struct msghdr *     pMsg,       /* message containing receive buffer */
    size_t              size,       /* size of receive buffer */
    int                 flags       /* flags : unused */
    )
#else
static int umiRecvMsg 
    ( 
    struct socket *     pSocket,    /* pointer to socket */
    struct msghdr *     pMsg,       /* message containing receive buffer */
    int                 size,       /* size of receive buffer */
    int                 flags,      /* flags : unused */
    struct scm_cookie * pScm        /* socket rights, ignored */
    )
#endif
    {
    UMI_SOCK *              pUmiSock;
    UMI_REQ_BUF *           pReqBuf;
    struct list_head *      pTmp;
    struct list_head *      pNextEntry;

    PRINTK (KERN_DEBUG "%s (%x,%x,%x)\n", __func__, (unsigned int)pSocket, 
                                         (unsigned int)pMsg, size);

    pUmiSock = (UMI_SOCK *)pSocket->sk->sk_protinfo;

    wait_event (*pUmiSock->sleep, !list_empty(&pUmiSock->recvq));

    UMI_SPIN_LOCK (pUmiSock->sockLock);

    pReqBuf = NULL;

    list_for_each_safe (pTmp, pNextEntry, &pUmiSock->recvq)
        {
        pReqBuf = (UMI_REQ_BUF *)pTmp;
        list_del (pTmp);
        break;
        }
    
    UMI_SPIN_UNLOCK (pUmiSock->sockLock);
    
    if (size > pReqBuf->len)
        {
        size = pReqBuf->len;
        }

    memcpy_toiovec (pMsg->msg_iov, pReqBuf->pBuf, size);

    kfree(pReqBuf);

    return (size);
    }
/*******************************************************************************
*
* umiSockHandler - process umi ioctl request destined to a user space component
*
* UMI registers this handler on behalf of all user space components. It queues
* the ioctl request and wake up the destination component process (if waiting).
*
* Note: cmd, pOutBuf, and pOutLen are ignored as the whole ioctl request is
* serialized within pInBuf.
* 
* RETURNS: -ENONMEM or 0
*/
static int umiSockHandler
    (
    void *          arg1,   /* argument passed during umiRegister */
    UMI_REQ_INFO *  pUmiReq,/* struct containting additional info about req */
    int             cmd,    /* ioctl command, ignored */
    void *          pInBuf, /* umi ioctl request buffer */
    unsigned int    inLen,  /* size of pInBuf */
    void *          pOutBuf,/* ignored */
    unsigned int *  pOutLen /* ignored */
    )
    {
    UMI_SOCK *      pUmiSock;
    UMI_REQ_BUF *   pReqBuf;

    PRINTK (KERN_DEBUG "%s (%x,%x,%x,%x,%x)\n", __func__, (unsigned int)arg1, 
                                               (unsigned int)pUmiReq, cmd,
                                               (unsigned int)pInBuf, inLen);

    pUmiSock = (UMI_SOCK *)arg1;

    pReqBuf = kmalloc (sizeof (UMI_REQ_BUF) + inLen, GFP_ATOMIC);

    if (pReqBuf == NULL)
        {
        return (-ENOMEM);
        }
    pReqBuf->pBuf = (char *)pReqBuf + sizeof (UMI_REQ_BUF);
    pReqBuf->len = inLen;
    
    memcpy(pReqBuf->pBuf, pInBuf, inLen);

    UMI_SPIN_LOCK (pUmiSock->sockLock);

    /* requests are added at the end while replies at the front */
    if (pUmiReq->req)
        {
        list_add_tail ((struct list_head *)pReqBuf, &pUmiSock->recvq);
        }
    else
        {
        list_add ((struct list_head *)pReqBuf, &pUmiSock->recvq);
        }

    UMI_SPIN_UNLOCK (pUmiSock->sockLock);

    wake_up (pUmiSock->sleep);

    return 0;
    }
            
/*******************************************************************************
*
* umiClose - close AF_UMI socket
*
* This function is invoked when user level umi library calls close on umi socket
* mainly when a component is unregistered or process is exited which causes
* system to close all the open descriptors.
*
* RETURNS: 0
*/
static int umiClose
    (
    struct socket * pSocket /* pointer to socket */
    )
    {
    UMI_SOCK *  pUmiSock;

    PRINTK (KERN_DEBUG "%s (%x)\n", __func__, (unsigned int)pSocket);

    pUmiSock = (UMI_SOCK *)pSocket->sk->sk_protinfo;
    umiUnRegister (pUmiSock->pCookie);
    kfree (pUmiSock);
    pSocket->sk->sk_protinfo = NULL;
    
    sock_orphan (pSocket->sk);
    sock_put (pSocket->sk);
    pSocket->sk = NULL;

    return 0;
    }

/* Module entry/exit declaration */
module_init(umiLoad);
module_exit(umiUnload);

