/* adpEtherLib.h -  adaptos ether header */

/* Copyright (c) 2006 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01c,17aug07,rnp  removed snprintf prototype and included adaptos.h instead.
01b,24aug06,chd added fixed compilation warnings.
01a,14may06,rnp created
*/

#ifndef __INCadpEtherLibH
#define __INCadpEtherLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "adaptos.h"

/* defines */
#define ether_ntoa   adpEtherNToA
#define ether_ntoa_r adpEtherNToAR

#define ADP_ETHER_BUF_SIZE 18

LOCAL ADP_INLINE char * adpEtherNToAR
    (
    struct ether_addr * pEtherAddr, /* network order ethernet address */
    char * pEtherBuf                /* buffer to store ascii form of ether addr
                                     */
    )
    {
    unsigned char * pData = (unsigned char *)pEtherAddr;

    memset(pEtherBuf, 0, ADP_ETHER_BUF_SIZE);
    snprintf(pEtherBuf, ADP_ETHER_BUF_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
             pData[0], pData[1], pData[2], pData[3], pData[4], pData[5]);       
    return pEtherBuf;
    }

/* 
 * This routine is not return and returns a statis buffer. If you need a retrant
 * version of this routine use adpEtherNtoAR
 */
LOCAL ADP_INLINE char * adpEtherNToA
    (
    struct ether_addr * pEtherAddr /* network order ethernet address */
    )
    {
    static char etherBuf[ADP_ETHER_BUF_SIZE];
    return adpEtherNToAR(pEtherAddr, etherBuf);
    }

#ifdef __cplusplus
}
#endif

#endif /* __INCadpEtherLibH */
