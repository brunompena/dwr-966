/* adpSocketIoLib.h -  adaptos socket IO library header */

/* Copyright (c) 2003 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01g,30aug08,rnp  added adpSocketConnectWithTO() prototype
01f,08mar03,ksn  renamed unx_inet_addr to adp_inet_addr
01e,27dec02,mk   added adp_inet_addr
01d,22jun02,mno  added adpSocketPrintf
01c,20jun02,mno  added parameter echo to adpSocketReadString()
01b,18jun02,mno  added adpSocket*String() routines.
01a,01jun02,mno  created
*/

#ifndef __INCadpSocketIoLibH
#define __INCadpSocketIoLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

#ifndef HAVE_ADP_SOCKET_ID
typedef int ADP_SOCKET_ID;
#define HAVE_ADP_SOCKET_ID
#endif

IMPORT ADP_SOCKET_ID   adpSocketCreate(int protocol, int type, int flags);
IMPORT int             adpSocketClose(ADP_SOCKET_ID sock);
IMPORT int             adpSocketWrite(ADP_SOCKET_ID sock, char *pBuf, int len);
IMPORT int             adpSocketRead(ADP_SOCKET_ID sock, char *pBuf, int len);
IMPORT int             adpSocketIoctl(ADP_SOCKET_ID sock, int cmd, void *pBuf);
IMPORT char *          adpSocketReadString (ADP_SOCKET_ID sock, char *pBuf,
                                            int len, BOOL echo);
IMPORT int             adpSocketWriteString (ADP_SOCKET_ID sock, char *str);
IMPORT void            adpSocketPrintf(ADP_SOCKET_ID sock, char *fmt, ...);
IMPORT int             adpSocketConnectWithTO(ADP_SOCKET_ID sock, 
                                              struct sockaddr *, int, 
                                              struct timeval *);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __INCadpSocketIoLibH */
