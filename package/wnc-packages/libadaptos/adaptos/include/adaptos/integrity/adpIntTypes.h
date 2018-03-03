/* adpIntTypes.h - include files for Integrity OS */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01b,10may08,san  define type ether_addr for fixing compiler warning 
                 'type ether_addr is not globally visible'.
01a,08apr08,san  written (based on linux/adpXXX.*).
*/

#ifndef __INCadpIntTypesH
#define __INCadpIntTypesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "adpIntBase.h"

/* typedefs */

#ifndef HAVE_ADP_STATUS
typedef enum status
{
    ST_OK = 0, 
    ST_ERROR = -1
} ADP_STATUS;
#define STATUS ADP_STATUS
#define HAVE_ADP_STATUS
#endif

#ifndef HAVE_ADP_BOOL
typedef enum boolean
{
    B_FALSE = 0,
    B_TRUE = 1
} ADP_BOOL;
#define BOOL ADP_BOOL
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

#ifndef PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX 
#endif 

#ifndef KEY_T
#define KEY_T
typedef int key_t;
#endif /* KEY_T */

#ifndef HAVE_ADP_KEY_T
#define HAVE_ADP_KEY_T
typedef key_t ADP_KEY_T;
#endif /* HAVE_ADP_KEY_T */

#ifdef CONFIG_NO_LONG_LONG 
#ifndef HAVE_ADP_UINT64_T 
#define uint64_t UINT64_STRUCT
#define HAVE_ADP_UINT64_T 
#endif /* HAVE_ADP_UINT64_T */
#ifndef HAVE_ADP_INT64_T
#define int64_t  UINT64_STRUCT
#define HAVE_ADP_INT64_T
#endif /* HAVE_ADP_INT64_T */    
#endif /* CONFIG_NO_LONG_LONG */

/* added for fixing warning type ether_addr is not globally visible */ 
struct ether_addr
    {
    char addr[6];
    };

#ifdef __cplusplus
}
#endif

#endif /* __INCadpIntTypesH */
