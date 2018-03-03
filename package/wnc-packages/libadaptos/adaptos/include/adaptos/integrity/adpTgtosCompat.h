/* adpTgtosCompat.h - post include for Integrity */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,08apr08,san  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpTgtosCompatH
#define __INCadpTgtosCompatH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adpIntUnMap.h"

/* defines */
#ifndef ADP_IO_API_FUNCTION_TABLE
#define ADP_IO_API_FUNCTION_TABLE   adpIntIoFuncTbl
#endif /* ADP_IO_API_FUNCTION_TABLE */

#ifndef ADP_IO_API_LIB_INIT_RTN
#define ADP_IO_API_LIB_INIT_RTN     adpIntIoLibInstall
#endif /* ADP_IO_API_LIB_INIT_RTN */

#ifndef ADP_IO_API_LIB_INIT_ARG
#define ADP_IO_API_LIB_INIT_ARG     (NULL)
#endif /* ADP_IO_API_LIB_INIT_ARG */

/* imports */
IMPORT ADP_IO_FUNCTBL * adpIntIoLibInstall (void *);
IMPORT ADP_IO_FUNCTBL adpIntIoFuncTbl;

#ifdef __cplusplus
}
#endif

#endif /* __INCadpTgtosCompatH */
