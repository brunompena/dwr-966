/* adpMsgQLib.c - adaptos message queue library for Linux */

/* Copyright (C) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,20may05,k_p  created.
*/

/*
DESCRIPTION

This module implements the adaptos message queuing related support for
Linux.

INCLUDES: adaptos.h

SEE ALSO: 
*/

/* includes */

#include "adaptos.h"


/* defines used only in this file */

#define ADP_MSG_SIZ_MAX    4056 /* max msg size supported by linux */
#define ADP_MSG_Q_PERMS    0660
#define ADP_MSG_TYPE       0x800

/* message buffer structure, used only in this file */

typedef struct adpMessageBuf
    {
    long messageType;               /* message type, must be > 0 */
    char message [ADP_MSG_SIZ_MAX]; /* message data */
    } ADP_LNX_MSG;

 
/*******************************************************************************
*
* adpMsgQCreate - create and initialize a message queue
*
* This routine creates a message queue. This queue can hold a maximum number
* of <maxMsgs> message, and each message can be a maximum of <maxMsgLength>
* bytes in length.
*
* The <options> argument can have the following values:
*
* MSG_Q_FIFO (0x00) - pended tasks queued in FIFO order. 
* MSG_Q_PRIORITY (0x01) - pended tasks queued in priority order. 
*
* RETURNS: Message queue Id, or NULL on failure
*/

ADP_MSG_Q_ID adpMsgQCreate
    (
    char *  pName,          /* name of message queue */
    int     maxMsgs,        /* maximum no. of message in queue */
    int     maxMsgLength,   /* maximum length of a message (in bytes) */
    int     options         /* message queue options */
    )
    {
    ADP_MSG_Q_ID    msgQId;
    
    if ((msgQId = msgget (IPC_PRIVATE, options | ADP_MSG_Q_PERMS)) == -1)
        {
        return (NULL);
        }
    
    return (msgQId);
    }

/*******************************************************************************
*
* adpMsgQDelete - delete a message queue
*
* This routine deletes a message queue. The message queue to be deleted is
* specified by the argument <msgQId>.
*
* RETURNS: ST_OK on success, or ST_ERROR otherwise
*/

STATUS adpMsgQDelete
    (
    ADP_MSG_Q_ID    msgQId  /* message queue Id */
    )
    {

    if (msgctl((int)msgQId, IPC_RMID, 0) == 0)
        {
        return (ST_OK);
        }
    
    return (ST_ERROR);
    }


/*******************************************************************************
*
* adpMsgQSend - send a message to a message queue
*
* This routine sends a message to a message queue. The message is read from a
* buffer pointed to by the argument <pBuffer>. The <timeout> argument can have
* the following values among others:
* 
* ADP_MSG_Q_NOWAIT - If the msg queue is full, the routine returns immediately
* WAIT_FOREVER - Wait forever to send the message
* 
* The <priority> argument does not have nay effect in Linux.
*
*
* RETURNS: ST_OK on success, or ST_ERROR otherwise
*/

STATUS adpMsgQSend
    (
    ADP_MSG_Q_ID    msgQId,  /* message queue Id */
    char *          pBuffer,  /* message to send */
    UINT            nBytes,  /* length of message (in bytes) */
    int             timeout, /* ticks to wait */
    int             priority /* message priority NORMAL or URGENT */
    )
    {
    ADP_LNX_MSG sndMsg;
    
    int msgFlags = 0; /* wait forever */

    
    if ((timeout == ADP_MSG_Q_NOWAIT) ||
        (timeout == 0) /* ticks to wait is zero */ )
        {
        msgFlags = IPC_NOWAIT;
        }
    
    /* prepare message to be sent */

    sndMsg.messageType = ADP_MSG_TYPE;
    
    bcopy ((const void *) pBuffer, sndMsg.message, nBytes);
    
    /*  send  a message */
    
    if (msgsnd ((int) msgQId, &sndMsg, nBytes, msgFlags) == 0)
        {
        return (ST_OK);
        }
    
    return (ST_ERROR);
    }


/*******************************************************************************
*
* adpMsgQReceive - receive a message from a message queue
*
* This routine reads a message from a message queue. The message is stored in
* the buffer pointed to by <pBuffer>. The <timeout> argument can have the
* following special values among others:
* 
* ADP_MSG_Q_NOWAIT - If the msg queue is full, the routine returns immediately
* WAIT_FOREVER - Wait forever to send the message
*
* RETURNS: Total number of bytes copied to buffer, or ST_ERROR otherwise
*/

int adpMsgQReceive
    (
    ADP_MSG_Q_ID    msgQId,  /* message queue Id */
    char *          pBuffer,  /* receive buffer */
    UINT            msgSize, /* message size (in bytes) */
    int             timeout  /* ticks to wait */
    )
    {
    int retCode;
    ADP_LNX_MSG rcvMsg;
    int msgFlags = 0; /* wait forever */
    
    rcvMsg.messageType = ADP_MSG_TYPE;   
   
    if ((timeout == ADP_MSG_Q_NOWAIT) ||
        (timeout == 0) /* ticks to wait is zero */ )
        {
        msgFlags = IPC_NOWAIT;
        }

    /* truncate the message if longer than msgsz bytes */
    
    msgFlags |= MSG_NOERROR;
     
    retCode = msgrcv ((int)msgQId, &rcvMsg, msgSize, rcvMsg.messageType,
                      msgFlags);

    bcopy (rcvMsg.message, (void *)pBuffer, msgSize);
    
    return (retCode);
    }

/*******************************************************************************
*
* adpmsgQNumMsgs - number of messages queued in a message queue
*
* This routine returns the total number of messages which are currently queued
* for a message queue. 
*
* RETURNS: Number of messages queued, or ST_ERROR otherwise.
*/

int adpmsgQNumMsgs
    (
    ADP_MSG_Q_ID    msgQId  /* message queue Id */    
    )
    {
    struct msqid_ds msgStats;

    /* get statistics for the message queue which is
     * associated with <msgQId>
     */
    
    if (msgctl(msgQId, IPC_STAT, &msgStats) == 0)
        {
        
        /* return number of messages currently on queue */
        
        return ((int)msgStats.msg_qnum);
        }
    
    return (ST_ERROR);
    }
