/* adpTxTypes.h - include files for ThreadX */

/* Copyright (c) 2006 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01c,27mar07,san  modified data type for threadx 
01b,17may06,pkk  adopted from adpLynTypes.h and modified for threadX
01a,31aug05,rnp  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpTxTypesH
#define __INCadpTxTypesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "adpTxBase.h"
#include "adpU64Struct.h"    

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

#ifndef HAVE_ADP_USHORT
typedef unsigned short   USHORT;
#define HAVE_ADP_USHORT
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

#ifndef PATH_MAX
/* PATH_MAX is not difined in limits.h of GH */
#define PATH_MAX 1024  
#endif 

#if defined(CONFIG_GHNET)

/* following is being defined in Threadx/GHNET env */ 
#define HAVE_ADP_CADDR_T
#define HAVE_ADP_U_INT
#define HAVE_ADP_U_LONG
#define HAVE_ADP_UINT8_T
#define HAVE_ADP_INT64_T
#define HAVE_ADP_UINT64_T 
#define HAVE_ADP_INT32_T

#ifdef CONFIG_NO_LONG_LONG
#define int64_t  UINT64_STRUCT
#define uint64_t UINT64_STRUCT
#endif /* CONFIG_NO_LONG_LONG */ 

/* add following typedefs which are not available in Threadx/GHNET */
#ifndef HAVE_ADP_UINT32 
typedef uint32_t UINT32; 
#define HAVE_ADP_UINT32 
#endif 

#ifndef HAVE_ADP_INT32
typedef int32_t INT32;
#define HAVE_ADP_INT32
#endif

#ifndef HAVE_ADP_UINT64
#ifdef CONFIG_NO_LONG_LONG
typedef UINT64_STRUCT UINT64;
#else
typedef uint64_t UINT64; 
#endif /* CONFIG_NO_LONG_LONG */
#define HAVE_ADP_UINT64
#endif

#ifndef HAVE_ADP_INT64
#ifdef CONFIG_NO_LONG_LONG
typedef UINT64_STRUCT INT64;
#else
typedef int64_t INT64; 
#endif /* CONFIG_NO_LONG_LONG */
#define HAVE_ADP_INT64
#endif

#ifndef HAVE_ADP_UINT8
typedef uint8_t UINT8; 
#define HAVE_ADP_UINT8
#endif

#ifndef HAVE_ADP_UINT16
typedef uint16_t UINT16; 
#define HAVE_ADP_UINT16
#endif

#endif /* CONFIG_GHNET */

#ifndef HAVE_ADP_CADDR_T
typedef unsigned char * caddr_t;
#define HAVE_ADP_CADDR_T
#endif 

#ifndef HAVE_ADP_DIR
typedef  UINT32 DIR;
#define HAVE_ADP_DIR
#endif 

#ifndef HAVE_ADP_SSIZE_T
typedef INT32 ssize_t;
#define HAVE_ADP_SSIZE_T
#endif 

#ifndef HAVE_ADP_OFF_T
typedef ULONG off_t;
#define HAVE_ADP_OFF_T
#endif 

#ifndef HAVE_ADP_U_LONG
typedef UINT32 u_long;
#define HAVE_ADP_U_LONG
#endif 

#ifndef HAVE_ADP_U_CHAR
typedef UINT8 u_char;
#define HAVE_ADP_U_CHAR
#endif 

#ifndef HAVE_ADP_U_SHORT
typedef UINT16 u_short;
#define HAVE_ADP_U_SHORT
#endif 

#ifndef HAVE_ADP_U_INT
typedef UINT32 u_int;
#define HAVE_ADP_U_INT
#endif 

#ifdef CONFIG_NO_LONG_LONG
#ifndef HAVE_ADP_UINT64
typedef UINT64_STRUCT UINT64;
#define HAVE_ADP_UINT64
#endif 
#ifndef HAVE_ADP_INT64
typedef UINT64_STRUCT INT64;
#define HAVE_ADP_INT64
#endif 
#endif /* CONFIG_NO_LONG_LONG */

#ifndef HAVE_ADP_U_INT64_T
typedef UINT64 u_int64_t; 
#define HAVE_ADP_U_INT64_T
#endif

#ifndef HAVE_ADP_U_INT32_T
typedef UINT32 u_int32_t;
#define HAVE_ADP_U_INT32_T 
#endif	

#ifndef HAVE_ADP_INT32_T
typedef INT32 int32_t;
#define HAVE_ADP_INT32_T 
#endif	

#ifndef HAVE_ADP_U_INT16_T
typedef UINT16 u_int16_t;
#define HAVE_ADP_U_INT16_T
#endif	

#ifndef HAVE_ADP_U_INT8_T
typedef UINT8  u_int8_t;
#define HAVE_ADP_U_INT8_T
#endif	

#ifndef HAVE_ADP_UINT64_T
typedef UINT64  uint64_t;
#define HAVE_ADP_UINT64_T
#endif	

#ifndef HAVE_ADP_INT64_T
typedef INT64  int64_t;
#define HAVE_ADP_INT64_T
#endif 

#ifndef HAVE_ADP_UINT32_T
typedef UINT32 uint32_t;
#define HAVE_ADP_UINT32_T
#endif	

#ifndef HAVE_ADP_UINT16_T
typedef UINT16  uint16_t;
#define HAVE_ADP_UINT16_T
#endif	

#ifndef HAVE_ADP_UINT8_T
typedef UINT8  uint8_t;
#define HAVE_ADP_UINT8_T
#endif	

#ifndef HAVE_ADP_PID_T
typedef INT32 pid_t;
#define HAVE_ADP_PID_T
#endif 

#ifndef HAVE_ADP_STRUCT_TIMESPEC
struct timespec
    {
    long tv_sec;
    long tv_nsec;
    };
#define HAVE_ADP_STRUCT_TIMESPEC
#endif 

#ifndef HAVE_ADP_STRUCT_STAT
/* space holder for stat structure */
struct stat
    {
    ULONG  st_mode;
    UINT32 st_uid;
    UINT32 st_gid;
    off_t  st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
    };
#define HAVE_ADP_STRUCT_STAT
#endif 

#ifndef HEADER  /* this structure is defined in dns.h of threadx tcpip stack */
typedef struct dns_hdr HEADER;
#endif 

#if defined (CONFIG_GHNET)
/* structure in bits/uio.h */
struct iovec 
    {
    void *iov_base;
    size_t iov_len;
    };
#define MAX_IOVEC 3
#endif /* CONFIG_GHNET */

/* added for fixing warning type ether_addr is not globally visible */ 
struct ether_addr
    {
    char addr[6];
    };

/* end of add following typedefs which are not available in threadx */
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
#if !defined(__cplusplus) && !defined(inline)
#ifdef __GNUC__
#define inline __inline
#endif /* __GNUC__ */
#endif      
#define HAVE_ADP_INLINE
#endif

#ifndef HAVE_ADP_FALSE
#undef  FALSE
#define FALSE            0
#define HAVE_ADP_FALSE
#endif

#ifndef HAVE_ADP_TRUE
#undef  TRUE
#define TRUE             1
#define HAVE_ADP_TRUE
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCadpTxTypesH */
