/* adpVxUnMap.h - post include for VxWorks */

/* Copyright (c) 2005 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01b,29jun07,rnp  added BLOCK_SIZE
01a,06sep05,rnp  written
*/

#ifndef __INCadpVxUnMaph
#define __INCadpVxUnMaph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* imports */
IMPORT int adpSocketAccept (ADP_SOCKET_ID s, struct sockaddr * pSAddr, 
                        int * pAddrLen);

#undef accept
#define accept(s, sa, al) adpSocketAccept((ADP_SOCKET_ID)s, sa, al)

/* 
 * vx 6.1 defines BLOCK_SIZE in memPartLibP.h which collides with aes.h in krb.
 * so it is undef'ed here
 */ 
#ifdef BLOCK_SIZE
#undef BLOCK_SIZE
#endif /* BLOCK_SIZE */
#ifdef __cplusplus
}
#endif

#endif /* __INCadpVxUnMaph */
