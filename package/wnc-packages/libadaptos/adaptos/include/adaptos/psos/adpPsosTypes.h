/* adpPsosTypes.h - definitions for PSOS specific data types */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,07may05,sam  moved all PSOS defn. from adpTypes.h.
*/

#ifndef __INCadpPsosTypesH
#define __INCadpPsosTypesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(ADAPTOS_PSOS_2X) || defined(ADAPTOS_PSOS_2X_ADV)
#undef STATUS
#endif /* ADAPTOS_PSOS_2X */

typedef enum status
{
    ST_OK = 0, 
    ST_ERROR = -1
} STATUS;


typedef enum boolean
{
    B_FALSE = 0,
    B_TRUE = 1
} BOOL;

#define FALSE   0
#define TRUE    1


/* typedefs */
typedef void                (*VOIDFUNCPTR)();
typedef unsigned char       u_char;
typedef unsigned short      u_short;
typedef unsigned long       u_long;
typedef unsigned long int   fd_mask;
typedef unsigned long       in_addr_t;
typedef in_addr_t           ADP_INADDR_T
#ifndef ADAPTOS_PSOS_2X
typedef unsigned char       UCHAR;
typedef unsigned short      USHORT;
typedef unsigned long       ULONG;
typedef unsigned int        UINT;
#endif /* ADAPTOS_PSOS_2X */

typedef struct  adpTaskCo
    {
    ADP_TASK_ID     targetTid;
    ADP_TASK_ID     sourceTid;
    void *      reserve;
    } ADP_TASK_CO;

typedef void    (*ADP_TASK_CO_HOOK)(void* arg1,
                                    ADP_TASK_CO * pTaskCoInfo);

/* defines */
#define UINT8               UINT_8
#define INT8                INT_8
#define UINT16              UINT_16
#define INT16               INT_16
#define UINT32              UINT_32
#define INT32               UINT_32

#define INADDR_NONE         ((unsigned long int) 0xffffffff)

#ifndef PATH_MAX
#define PATH_MAX            64
#endif /* PATH_MAX */

#define bzero(x,y)          adpBzero(x,y)
#define inet_addr(x)        adp_inet_addr(x)

#define ADP_TICKS_PER_SEC   CLOCKS_PER_SEC

#define strncasecmp         adp_strncasecmp
#define strcasecmp          adp_strcasecmp
#define gettimeofday        adp_gettimeofday

#ifdef __cplusplus
}
#endif

#endif /* __INCadpPsosTypesH */
