/* adpSocketIoLib.c - adaptos socket I/O library for Linux */

/* Copyright (C) 2003 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01f,30aug08,rnp  added adpSocketConnectWithTO()
01e,13may05,k_p  modified for Linux
01d,24jul03,gnm  modified all routine descriptions,
                 modified module description,
                 modified module header,
                 added files to INCLUDES list.
01c,23jul03,gnm  modified all routine descriptions.
01b,17jun03,gnm  code cleanup: added headers for all functions, added argument
                 descriptions for all function arguments.
01a,06feb03,ksn  created.
*/

/*
DESCRIPTION

This module implements the adaptos socket I/O routines for Linux.

INCLUDES: adaptos.h, adpSocketIoLib.h

SEE ALSO: 

*/

/* includes */

#include "adaptos.h"

/*******************************************************************************
*
* adpSocketCreate - open a socket
*
* This routine will open a socket and returns a socket descriptor. The <type>
* argument can have the following values:
*
* SOCK_STREAM,
* SOCK_DGRAM, or
* SOCK_RAW
* SOCK_SEQPACKET
* SOCK_PACKET
*
* RETURNS: Socket descriptor, or ERROR (-1) on failure and errno is set
* appropriately.
*
*/

ADP_SOCKET_ID adpSocketCreate
    (
    int         domain,    /* address family (eg. AF_INET) */
    int         type,      /* socket type */
    int         protocol   /* protocol used with socket */
    )
    {
    return socket (domain, type, protocol);
    }

/*******************************************************************************
*
* adpSocketRead - read from a socket
*
* This routine reads a number of bytes from the socket <sock> and places the
* data read in <pBuf>.
*
* RETURNS: The number of bytes read or ERROR (-1) on failure and errno is
* set appropriately.
*
*/

int adpSocketRead
    (
    ADP_SOCKET_ID       sock,   /* socket from which to read */
    char *              pBuf,   /* buffer to store bytes read */
    int                 bytes   /* maximum bytes to be read into buffer */
    )
    {

    return (read(sock, pBuf, bytes));
    }
    
/*******************************************************************************
*
* adpSocketWrite - write bytes to a socket
*
* This routine writes <bytes> bytes from the buffer <pBuf> to the specified
* socket <sock>.
*
* RETURNS: The number of bytes written, or ERROR (-1) on failure  and errno
* is set appropriately.
*
*/

int adpSocketWrite
    (
    ADP_SOCKET_ID       sock,  /* socket on which to write */
    char *              pBuf,  /* read buffer */
    int                 bytes  /* number of bytes to write */
    )
    {

    return (write (sock, pBuf, bytes));
    }

/*******************************************************************************
*
* adpSocketClose - close a socket
*
* This routine closes the specified socket.
*
* RETURNS: Status of the driver close routine, or ERROR on failure and
* errno is set appropriately.
*/

int adpSocketClose
    (
    ADP_SOCKET_ID       sock  /* socket to be closed */
    )
    {
    return (close(sock));
    }

/*******************************************************************************
*
* adpSocketIoctl - perform an I/O control function
*
* This routine performs an I/O control function on the socket.
*
* RETURNS: Return value of the driver, or ERROR (-1) on failure and
*  and errno is set appropriately.
*/

int adpSocketIoctl
    (
    ADP_SOCKET_ID       sock,  /* socket */
    int                 cmd,   /* function code */
    void *              pBuf   /* argument to pass to ioctl call */
    )
    {
    return (ioctl(sock, cmd, pBuf));
    }

/*******************************************************************************
*
* adpSocketConnectWithTO - connect with timeout
*
* This routine performs a connect with timeout
*
* RETURNS: Return value of the driver, or ERROR (-1) on failure and
*  and errno is set appropriately.
*/
int adpSocketConnectWithTO
    (
    ADP_SOCKET_ID       sock,  /* socket */
    struct sockaddr *   pAddr,
    int                 addrSize,
    struct timeval *    timeout
    )
    {
    long flags;
    int  setblocking = 1;
    int  val = 0;
    int  valsize = sizeof(int);
    fd_set wfds;

    flags = fcntl(sock, F_GETFL, NULL);
    if (flags & O_NONBLOCK)
        setblocking = 0;
    else
        {
        flags |= O_NONBLOCK;
        if (fcntl(sock, F_SETFL, flags) < 0)
            goto adpSocketConnectWithTOError;
        }
    if (connect(sock, pAddr, addrSize) < 0 &&
        errno != EINPROGRESS)
        goto adpSocketConnectWithTOError;

    FD_ZERO(&wfds);
    FD_SET(sock, &wfds);
    if (select(sock+1, NULL, &wfds, NULL, timeout) <=0)
        goto adpSocketConnectWithTOError;

    getsockopt(sock, SOL_SOCKET, SO_ERROR, &val, &valsize);
    if (val)
        goto adpSocketConnectWithTOError;
    if (setblocking)
        flags &= ~O_NONBLOCK;
    return ST_OK;
    adpSocketConnectWithTOError:
        {
        if (setblocking)
            flags &= ~O_NONBLOCK;
        return ST_ERROR;
        }
    }
