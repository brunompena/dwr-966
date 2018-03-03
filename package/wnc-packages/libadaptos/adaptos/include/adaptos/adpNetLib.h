/* adpNetLib.h - generic network wrapper routines and definitions */

/* Copyright (c) 2003 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01i,30jun07,rnp  changed arg2 of adp_getservbyname() to const char *
01h,12sep05,rnp  added adp_getservbyport() prototype
01g,08apr05,sam  changed netdb.h to adpNetdb.h
01f,18sep03,mk   fixed warnings
01e,16aug03,ksn  fixed fuction prototypes.
01d,23jul03,ws   defined netent, hostent, servent and protent to their
                 adaptos equivalents.
01c,08jul03,ksn  fixed prototypes, and added adp_inet_aton
01b,02may03,ksn  changed function prototype of hostbyaddr, hostbyname,
                 added prototype of sockpair_duplex.
01a,12jan03,ksn  created from vxcompat.h
*/

#ifndef _ADPNETLIB_H
#define _ADPNETLIB_H

/* includes */
#include "adaptos.h"

/* prototypes */

struct adp_protoent *   adp_getprotobyname (char *name);
struct adp_servent *    adp_getservbyname (const char *name, const char *proto);
struct adp_hostent *    adp_gethostbyname (const char *name,
                                           ADP_MALLOC_FUNCPTR _mallocRtn);
struct adp_hostent *    adp_gethostbyaddr (const char *addr, int len, int type,
                                           ADP_MALLOC_FUNCPTR _mallocRtn);
struct adp_servent *    adp_getservbyport (int port, char * proto);
void                    adp_hostentBufFree (void *pBuf,
                                            ADP_FREE_FUNCPTR _freeRtn);
int sockpair_duplex (int sv[2]);
extern int adp_inet_aton (const char *cp, struct in_addr *addr);
extern char *adp_inet_ntoa(struct in_addr in);

#endif /* _ADPNETLIB_H */
