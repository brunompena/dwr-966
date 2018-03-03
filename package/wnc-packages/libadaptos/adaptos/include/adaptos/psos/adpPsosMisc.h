/* adpPsosMisc.h -- pSOS utility routines and definitions */

/* Copyright (c) 2003, TeamF1, Inc. */

/*
modification history
--------------------
01i,02oct03,ksn  added definition for sleep for psos 2.x
01h,03sep03,ksn  added routine adp_gmtime_r.
01g,20aug03,gnm  made inet_ntoa available for psos 2.5
01f,20aug03,ksn  added support for psos 2.5
01e,19aug03,gnm  added F_GETFL, F_SETFD and ENOTSUP macros nad redefined
                 S_SETFL, changed prototype of strtok_r
01d,16aug03,ksn  added getuid, getpid, psosSocketShare and
                 psosSocketShareAndClose
01c,25jul03,ksn  fixed FSETFL value.
01b,08jul03,ksn  removed macro for inet_aton
01a,12jan03,ksn  created from vxcompat.h
*/

#ifndef _ADP_PSOSCOMPAT_H
#define _ADP_PSOSCOMPAT_H

#include "adaptos.h"

/* defines */

#ifdef  SIG_ERR
#undef  SIG_ERR
#define SIG_ERR         (void (*)())-1
#endif /* SIG_ERR */

#ifdef ADAPTOS_PSOS_2X
/* sys/socket.h */

#define AF_UNIX          1        /* local to host (pipes, portals) */
#define AF_MAX          22
#define IPPORT_RESERVED 1024 /* Ports < IPPORT_RESERVED are reserved. */
#define FIOREAD         0x4004667F  /* Get bytes in socket read buffer */
#define FIONREAD    FIOREAD
#define hostent         adp_hostent

/* fcntl.h */

#define F_SETFL         0x0005    /* Set file status flags */
#define F_GETFL         0x0004
#define F_SETFD         0x0003
#define O_RDWR          0x0002
#define O_RDONLY    0x0000   /* Open for reading only */
#define O_WRONLY    0x0001   /* Open for writing only */
#define O_CREAT     0x0200   /* Create file if it does not exist  */
#define O_EXCL      0x0100   /* Exclusive use flag */
#define O_TRUNC     0x0800   /* Truncate flag */

/* errno.h */

#define EAGAIN      EWOULDBLOCK
#define ENAMETOOLONG    0x506A  /* File name too long                  */
#define ENOTDIR         0x310b  /* POSIX                               */
#define ENOENT          0x506F  /* POSIX                               */
#define ENOTSUP         0x310d  /* POSIX                               */
#define EACCESS         0x3005  /* File not open for operation         */
#define EACCES          0x5063  /* Permission denied                   */
#define ENOSPC          0x6B    /* No space left on device             */
#define EBADF           0x65    /* Bad stream descriptor number        */
#define EPERM           0x6D    /* Not owner                           */
#define EFAULT          0x68    /* Bad address                         */

#endif /* ADAPTOS_PSOS_2X */

#define IN_LOOPBACKNET  127

/* prototypes */
extern char *  adp_getenv(char *);

#ifdef ADAPTOS_PSOS_2X
char *strtok_r(char *, const char *, char **);
#endif /* ADAPTOS_PSOS_2X */

#if defined(ADAPTOS_PSOS_2X) || defined(ADAPTOS_PSOS_2X_ADV)
#define inet_ntoa(X)    adp_inet_ntoa(X)
#endif /* defined(ADAPTOS_PSOS_2X) || defined(ADAPTOS_PSOS_2X_ADV) */

#ifdef  ADAPTOS_PSOS_2X
ULONG
#else /* ADAPTOS_PSOS_2X */  
uid_t
#endif /* ADAPTOS_PSOS_2X */
    getuid(void);

#ifdef  ADAPTOS_PSOS_2X
ULONG
#else /* ADAPTOS_PSOS_2X */
pid_t
#endif /* ADAPTOS_PSOS_2X */    
    getpid(void);

#ifdef ADAPTOS_PSOS_2X_ADV
#undef  STATUS
#define FIONREAD    FIOREAD
#endif 

extern int psosSocketShare (int sock, int tId);
extern int psosSocketShareAndClose (int sock, int tId);
#ifdef ADAPTOS_PSOS_2X
#define gmtime_r    adp_gmtime_r
extern struct tm * adp_gmtime_r (const time_t *, struct tm *);
#endif /* ADAPTOS_PSOS_2X */
#if defined(ADAPTOS_PSOS_2X) || defined(ADAPTOS_PSOS_2X_ADV)
extern unsigned int sleep (unsigned int secs);  
#endif /* defined(ADAPTOS_PSOS_2X) || defined(ADAPTOS_PSOS_2X_ADV) */

ADP_INADDR_T    adp_inet_addr(register const char *cp);

#ifdef HAVE_ADP_GETOPT
#define getopt(X,Y,Z)       BSDgetopt(X,Y,Z)
#define HAVE_ADP_GETOPT
#endif

#ifndef HAVE_ADP_OPTERR
#define opterr      getoptObjGet()->BSD_opterr
#define HAVE_ADP_OPTERR
#endif

#ifndef HAVE_ADP_OPTIND
#define optind      getoptObjGet()->BSD_optind
#define HAVE_ADP_OPTIND
#endif

#ifndef HAVE_ADP_OPTOPT
#define optopt      getoptObjGet()->BSD_optopt
#define HAVE_ADP_OPTOPT
#endif

#ifndef HAVE_ADP_OPTRESET
#define optreset    getoptObjGet()->BSD_optreset
#define HAVE_ADP_OPTRESET
#endif

#ifndef HAVE_ADP_OPTARG
#define optarg      getoptObjGet()->BSD_optarg
#define HAVE_ADP_OPTARG
#endif

#define MAX_ADDR_LIST       10
#define SIZE_MAX_HOST_NAME  256

#define S_IFMT      _IFMT
#define S_IFDIR     _IFDIR
#define S_IFCHR     _IFCHR
#define S_IFBLK     _IFBLK
#define S_IFREG     _IFREG
#define S_IFLNK     _IFLNK
#define S_IFSOCK    _IFSOCK  
#define S_IFIFO     _IFIFO

#endif /* _ADP_PSOS_COMP_H */


