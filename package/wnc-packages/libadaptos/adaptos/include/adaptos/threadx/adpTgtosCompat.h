/* adpTgtosCompat.h - post include for ThreadX */

/* Copyright (c) 2006, TeamF1, Inc. */

/*
modification history
--------------------
01b,17may06,pkk  adopted from LynxOs
01a,06sep05,rnp  written
*/

#ifndef __INCadpTgtosCompatH
#define __INCadpTgtosCompatH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adpTxUnMap.h"

/* defines */
#ifndef ADP_IO_API_FUNCTION_TABLE
#define ADP_IO_API_FUNCTION_TABLE   adpTxIoFuncTbl
#endif /* ADP_IO_API_FUNCTION_TABLE */

#ifndef ADP_IO_API_LIB_INIT_RTN
#define ADP_IO_API_LIB_INIT_RTN     adpTxIoLibInstall
#endif /* ADP_IO_API_LIB_INIT_RTN */

#ifndef ADP_IO_API_LIB_INIT_ARG
#define ADP_IO_API_LIB_INIT_ARG     (NULL)
#endif /* ADP_IO_API_LIB_INIT_ARG */

/* imports */
IMPORT ADP_IO_FUNCTBL * adpTxIoLibInstall (void *);
IMPORT ADP_IO_FUNCTBL adpTxIoFuncTbl;

#ifdef __cplusplus
}
#endif

#endif /* __INCadpTgtosCompatH */
