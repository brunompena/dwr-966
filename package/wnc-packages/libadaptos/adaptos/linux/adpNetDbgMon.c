/* adpNetDbgMon.c - net daemon for logging debug messages */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01c,25oct05,sam  mapped adpNetDbgmon to main for documentation.
01b,01oct05,sam  improved documentation
01a,09apr05,rks  written
*/

/*
DESCRIPTION
adpNetDbgMon() prints the debug and log messages from a target over the network.
To use adpNetDbgMon(), the adaptos library has to be compiled with the 
ADP_NETLOG flag and the adpNetDbgMon() must be started on a Linux host 
machine which is in the same network as the target being monitored.

.LP    
This debug info is received by subscribing to the following multicast address
and port:
.TS
tab(|);
  Address/port          | value
-
ADB_DEBUG_MCAST_IP_STR  | 224.1.1.1

ADP_DEBUG_PORT          | 9876

.TE
*/

/* includes */
#include "adaptos.h"

#define adpNetDbgMon main
/*******************************************************************************
*
* adpNetDbgMon - prints the debug info received from the target
* 
* adpNetDbgMon() joins the multicast group whose IP address is given by
* ADB_DEBUG_MCAST_IP_STR, receives the debug information that is being
* multicasted by the target and prints it on the console. 
* 
* RETURNS: ST_OK for success or ST_ERROR otherwise
*/

int adpNetDbgMon ()
    {
    int             sockFd;
    char            buff [ADP_DEBUG_MAX_SOCK_BUF];
    struct          sockaddr_in myaddr;
    struct ip_mreq  mreq;

    sockFd = socket (PF_INET, SOCK_DGRAM, 0);

    bzero ((void *)&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons (ADP_DEBUG_PORT);
    myaddr.sin_addr.s_addr = htonl (INADDR_ANY);

    if (bind (sockFd, (struct sockaddr *)&myaddr, sizeof (myaddr)) < 0)
        {
        perror ("bind");
        return (ST_ERROR);
        }
    mreq.imr_multiaddr.s_addr = inet_addr(ADB_DEBUG_MCAST_IP_STR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sockFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,
                   sizeof(mreq)) < 0) 
        {
        perror("setsockopt");
        return (ST_ERROR);
        }
     while (1) 
        {
        if (recv (sockFd, buff, ADP_DEBUG_MAX_SOCK_BUF, 0) < 0)
            {
            perror("recvfrom");
            return (ST_ERROR);
            }
        printf("%s",buff);
        bzero(buff, sizeof (buff));
        }
     
     return (ST_OK);
    }

