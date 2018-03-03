/* adpModCodes.h - adaptos module codes */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01g,16sep05,rnp  added M_adpX509Lib
01f,16sep03,ws   added M_wmwSslLib.
01e,28jul03,ws   added M_sshdLib.
01d,03jul03,ws   added M_adpCryptoLib.
01c,19apr03,ksn  updated with module information.
01b,12jun02,mno  added ADP_SOCKET_IO_LIB
01a,08may02,mno  created.
*/

#ifndef __INCadpModCodesH
#define __INCadpModCodesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* module codes */
#define TEAMF1_ERRNO_LIB            0xF1  << 24
#define ADP_MODULE_ERRNO_SET(X)     (TEAMF1_ERRNO_LIB | (X << 16))

#define M_adpTaskLib                ADP_MODULE_ERRNO_SET(0x1)
#define M_adpSemLib                 ADP_MODULE_ERRNO_SET(0x2)
#define M_adpTimerLib               ADP_MODULE_ERRNO_SET(0x3)
#define M_adpMemLib                 ADP_MODULE_ERRNO_SET(0x4)
#define M_adpListLib                ADP_MODULE_ERRNO_SET(0x5)
#define M_adpIoLib                  ADP_MODULE_ERRNO_SET(0x6)
#define M_adpSocketIoLib            ADP_MODULE_ERRNO_SET(0x7)
#define M_adpRandLib                ADP_MODULE_ERRNO_SET(0x8)
#define M_adpX509Lib                ADP_MODULE_ERRNO_SET(0x9)
#define M_adpCmdLib                 ADP_MODULE_ERRNO_SET(0xA)

#define M_sftpClientLib             ADP_MODULE_ERRNO_SET(0x60)
#define M_adpCryptoLib              ADP_MODULE_ERRNO_SET(0x61)
#define M_sshdLib                   ADP_MODULE_ERRNO_SET(0x62)
#define M_umiLib                    ADP_MODULE_ERRNO_SET(0x63)

/* error module number for wind manage httpsSsl application */
#define M_wmwSslLib                 ADP_MODULE_ERRNO_SET(0x63) 

/* mgmt modules */
#define M_mgmtDemoLib               ADP_MODULE_ERRNO_SET(0xFF)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCadpModCodesH */
