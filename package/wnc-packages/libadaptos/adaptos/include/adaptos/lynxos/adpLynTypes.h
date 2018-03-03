/* adpLynTypes.h - include files for LynxOS */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,31aug05,rnp  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpLynTypesH
#define __INCadpLynTypesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "adpLynBase.h"

/* typedefs */

#ifndef HAVE_ADP_STATUS
typedef enum status
{
    ST_OK = 0, 
    ST_ERROR = -1
} STATUS;
#define HAVE_ADP_STATUS
#endif

#ifndef HAVE_ADP_BOOL
typedef enum boolean
{
    B_FALSE = 0,
    B_TRUE = 1
} BOOL;
#define HAVE_ADP_BOOL
#endif

#ifndef HAVE_ADP_UCHAR
typedef unsigned char    UCHAR;
#define HAVE_ADP_UCHAR
#endif

#ifndef HAVE_ADP_USHORT
typedef unsigned short   USHORT;
#define HAVE_ADP_USHORT
#endif

#ifndef HAVE_ADP_ULONG
typedef unsigned long    ULONG;
#define HAVE_ADP_ULONG
#endif

#ifndef HAVE_ADP_UINT
typedef unsigned int     UINT;
#define HAVE_ADP_UINT
#endif

#ifndef HAVE_ADP_VOIDFUNCPTR
typedef void             (*VOIDFUNCPTR)();
#define HAVE_ADP_VOIDFUNCPTR
#endif

#ifndef HAVE_ADP_TASK_CO_HOOK
typedef void    (*ADP_TASK_CO_HOOK)(void* arg1);
#define HAVE_ADP_TASK_CO_HOOK
#endif

#ifndef HAVE_ADP_TIMEVAL
typedef struct timeval  ADP_TIMEVAL;
#define HAVE_ADP_TIMEVAL
#endif
/* defines */
#ifndef HAVE_ADP_UINT8
#define UINT8            UINT_8
#define HAVE_ADP_UINT8
#endif

#ifndef HAVE_ADP_INT8
#define INT8             INT_8
#define HAVE_ADP_INT8
#endif

#ifndef HAVE_ADP_UINT16
#define UINT16           UINT_16
#define HAVE_ADP_UINT16
#endif

#ifndef HAVE_ADP_INT16
#define INT16            INT_16
#define HAVE_ADP_INT16
#endif

#ifndef HAVE_ADP_UINT32
#define UINT32           UINT_32
#define HAVE_ADP_UINT32
#endif

#ifndef HAVE_ADP_INT32
#define INT32            UINT_32
#define HAVE_ADP_INT32
#endif

#ifndef HAVE_ADP_UINT_32
#define UINT_32          ULONG
#define HAVE_ADP_UINT_32
#endif

#ifndef HAVE_ADP_UINT_16
#define UINT_16          USHORT
#define HAVE_ADP_UINT_16
#endif

#ifndef HAVE_ADP_BYTE
#define BYTE             UINT_8 
#define HAVE_ADP_BYTE
#endif

#ifndef HAVE_ADP_LOCAL
#define LOCAL            static
#define HAVE_ADP_LOCAL
#endif

#ifndef HAVE_ADP_IMPORT
#define IMPORT           extern
#define HAVE_ADP_IMPORT
#endif

#ifndef HAVE_ADP_EXPORT
#define EXPORT           extern
#define HAVE_ADP_EXPORT
#endif

#ifndef HAVE_ADP_INLINE
#define ADP_INLINE       inline
#define HAVE_ADP_INLINE
#endif

#ifndef HAVE_ADP_FALSE
#define FALSE            0
#define HAVE_ADP_FALSE
#endif

#ifndef HAVE_ADP_TRUE
#define TRUE             1
#define HAVE_ADP_TRUE
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCadpLynTypesH */
