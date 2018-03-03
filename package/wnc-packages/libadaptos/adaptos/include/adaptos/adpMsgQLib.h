/* adpMsgQLib.h -  adaptos message queue library header */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01b,07may05,sam  restructured adaptos.
01a,12may03,ksn  created.
*/

#ifndef __INCadpMsgQLibH
#define __INCadpMsgQLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

#define ADP_MSG_Q_OK                (0)
#define ADP_MSG_Q_ERROR             (-1)

#define ADP_MSG_Q_FIFO              (0x00)
#define ADP_MSG_Q_PRIORITY          (0x01)

#define ADP_MSG_Q_NOWAIT            (0)
#define ADP_MSG_Q_WAITFOREVER       (-1)

#define ADP_MSG_PRIORITY_NORMAL     (0)
#define ADP_MSG_PRIORITY_URGENT     (-1)
    
#ifndef HAVE_ADP_MSG_Q_ID
typedef ULONG  ADP_MSG_Q_ID;
#define HAVE_ADP_MSG_Q_ID
#endif

IMPORT ADP_MSG_Q_ID adpMsgQCreate (char * pName, int maxMsgs,
                                   int maxMsgLength, int     options);
IMPORT STATUS       adpMsgQDelete (ADP_MSG_Q_ID msgQId);
IMPORT STATUS       adpMsgQSend (ADP_MSG_Q_ID msgQId, char * buffer,
                                 UINT nBytes, int timeout, int priority);
IMPORT int          adpMsgQReceive (ADP_MSG_Q_ID msgQId, char*  buffer,
                                    UINT buf_len, int timeout);
IMPORT int          adpmsgQNumMsgs (ADP_MSG_Q_ID msgQId);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCadpMsgQLibH */
