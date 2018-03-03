/* adpSocketIoLib.c - adaptos socket I/O library for Integrity */

/* Copyright (C) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,08apr08,san  created (based on linux/adpxxx).
*/

/*
DESCRIPTION

This module implements the adaptos socket I/O routines for Integrity.

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

