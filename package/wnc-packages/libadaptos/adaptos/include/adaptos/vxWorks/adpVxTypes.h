/* adpVxTypes.h - data types for VxWorks OS */

/* Copyright (c) 2005 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01p,30aug08,rnp  defined HAVE_ADP_CONNECT_WITH_TO
01o,07dec07,rnp  added in_addr_t
                 added NEED_ADP___XXX under ADAPTOS_VXWORKS_54
01n,06oct07,san  added CONFIG_NO_LONG_LONG,HAVE_ADP_NO_VAR_NAME_VAR_ARG 
                 and adpU64Struct.h.
01m,20sep07,rnp  added __func__ definition
01l,13sep07,rnp  added snprintf prototype
01k,21aug07,san  fixed compilation error for Vx54. 
01j,17aug07,rnp  moved HAVE_ADP_VX_XBD under 63 and 65 macros.
01i,27jul07,rnp  added HAVE_ADP_VX_XBD for 6.x
01h,12jul07,rnp  added HAVE_ADP_QUAD_T_DECLARED
01g,30jun07,rnp  added HAVE_ADP_VSNPRINTF_PROTOTYPE
                 added HAVE_ADP_GETADDRINFO, HAVE_ADP___P under 6X
                 added HAVE_ADP_SIZE_T under DIAB
01f,23jan07,ncs  added support for PNE 2.2 or above version
01e,02aug06,rc   added support for VxWorks 6.x
01d,25jun06,chd  added support for Vx54.
01c,25jun06,chd  mapped inline to __inline for vxWorks.
01b,25aug05,rnp  u_int types are now defines instead of typedefs
01a,07may05,sam  moved all VxWorks datatypes from adpTypes.h.
*/

#ifndef __INCadpVxTypesH
#define __INCadpVxTypesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "adpU64Struct.h"   
#include "adaptos.h"

#ifdef ADAPTOS_VXWORKS_PNE
#define HAVE_ADP_U_INT64_T_DECLARED
#define HAVE_ADP_QUAD_T_DECLARED
#define HAVE_ADP_U_QUAD_T_DECLARED
#define HAVE_ADP_GETADDRINFO
#endif /* ADAPTOS_VXWORKS_PNE */

#if defined(ADAPTOS_VXWORKS_6X)
#define HAVE_ADP_U_INT64_T_DECLARED
#define HAVE_ADP_QUAD_T_DECLARED
#define HAVE_ADP_U_QUAD_T_DECLARED
#define HAVE_ADP_GETADDRINFO
#define HAVE_ADP___P
#define HAVE_ADP_IN_ADDR_T_DECLARED
#if (defined ADAPTOS_VXWORKS_63) || (defined ADAPTOS_VXWORKS_65)
#define HAVE_ADP_VX_XBD
#endif /* (defined ADAPTOS_VXWORKS_63) || (defined ADAPTOS_VXWORKS_65) */
#ifdef ADAPTOS_VXWORKS_DIAB
#define HAVE_ADP_SIZE_T
#endif /* ADIABPTOS_VXWORKS_DIAB */
#endif /* ADAPTOS_VXWORKS_6X */
    
#ifdef ADAPTOS_VXWORKS_54
/* 
 * vx 5.4 has long long and unsigned long long types, but it doesnt have the
 * compiler routines to handle 64-bit arithmetic. So we add these routines(from
 * adpU64Struct.c)
 */ 
#define NEED_ADP___DIVDI3
#define NEED_ADP___MODDI3
#define NEED_ADP___CMPDI2
#endif /* ADAPTOS_VXWORKS_54 */

#ifndef ADAPTOS_VXWORKS_54
 #ifdef CONFIG_NO_LONG_LONG 
  #define uint64_t UINT64_STRUCT
  #define int64_t  UINT64_STRUCT
 #endif /* CONFIG_NO_LONG_LONG  */	
#else /* ADAPTOS_VXWORKS_54 */
 /* for vx 5.4 declare ull and ll types */
 #ifndef HAVE_ADP_UINT64_T	
  #ifdef CONFIG_NO_LONG_LONG 
   typedef UINT64_STRUCT uint64_t;
  #else
   typedef unsigned long long uint64_t;
  #endif /* CONFIG_NO_LONG_LONG */
  #define HAVE_ADP_UINT64_T
 #endif /* HAVE_ADP_UINT64_T */
 #ifndef HAVE_ADP_INT64_T
  #ifdef CONFIG_NO_LONG_LONG 
   typedef UINT64_STRUCT int64_t;
  #else
   typedef signed long long int64_t;
  #endif /* CONFIG_NO_LONG_LONG */
  #define HAVE_ADP_INT64_T
 #endif /* HAVE_ADP_INT64_T */
#endif /* ADAPTOS_VXWORKS_54 */

/* typedefs */
#ifndef HAVE_ADP_U_INT32_T
#define u_int32_t   UINT32
#define HAVE_ADP_U_INT32_T
#endif

#ifndef HAVE_ADP_U_INT16_T
#define u_int16_t   UINT16
#define HAVE_ADP_U_INT16_T
#endif
#ifndef HAVE_ADP_U_INT
#define u_int   UINT
#define HAVE_ADP_U_INT
#endif

#ifndef HAVE_ADP_SIZE_T
#ifndef _SIZE_T_
/* _SIZE_T_ is typically defined  in the compiler header file */
typedef unsigned long       size_t;
#endif /* _SIZE_T_ */
#define HAVE_ADP_SIZE_T
#endif /* HAVE_ADP_SIZE_T */

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

#ifndef HAVE_ADP_TASK_CO_HOOK
typedef void    (*ADP_TASK_CO_HOOK)(WIND_TCB* arg1);
#define HAVE_ADP_TASK_CO_HOOK
#endif

#ifndef HAVE_ADP_TIMEVAL
typedef struct timeval  ADP_TIMEVAL;
#define HAVE_ADP_TIMEVAL
#endif

#ifndef HAVE_ADP_U_INT64_T_DECLARED
#define HAVE_ADP_U_INT64_T_DECLARED
#ifdef CONFIG_NO_LONG_LONG 
typedef UINT64_STRUCT u_int64_t;
#else
typedef unsigned long long u_int64_t;
#endif /* CONFIG_NO_LONG_LONG */
#endif /* HAVE_ADP_U_INT64_T_DECLARED */

#ifndef HAVE_ADP_U_QUAD_T_DECLARED
#define HAVE_ADP_U_QUAD_T_DECLARED
typedef unsigned long long u_quad_t;
#endif /* HAVE_ADP_U_QUAD_T_DECLARED */

#ifndef HAVE_ADP_QUAD_T_DECLARED
#define HAVE_ADP_QUAD_T_DECLARED
typedef long long quad_t;
#endif /* HAVE_ADP_QUAD_T_DECLARED */

#ifndef HAVE_ADP_INT8_T_DECLARED
#define HAVE_ADP_INT8_T_DECLARED
#define u_int8_t   UINT_8
#endif

#ifndef HAVE_ADP_IN_ADDR_T_DECLARED
#define HAVE_ADP_IN_ADDR_T_DECLARED
typedef u_int32_t  in_addr_t;
#endif

#ifndef HAVE_ADP_VSNPRINTF_PROTOTYPE
#define HAVE_ADP_VSNPRINTF_PROTOTYPE
extern int vsnprintf (char *, size_t, const char *, va_list);
#endif /* HAVE_ADP_VSNPRINTF_PROTOTYPE */

#ifndef HAVE_ADP_SNPRINTF_PROTOTYPE
#define HAVE_ADP_SNPRINTF_PROTOTYPE
extern int snprintf (char *, size_t, const char *, ...);
#endif /* HAVE_ADP_SNPRINTF_PROTOTYPE */

#ifndef HAVE_ADP___FUNC__
#define HAVE_ADP___FUNC__
/* 
 * ideally __func__ shouldnt be defined, if HAVE_ADP___FUNC__ is not defined,
 * but on vx6.3 __func__ is defined to NULL, so undef that first to avoid
 * warning 
 */
#undef __func__
#define __func__    __FILE__
#endif /* HAVE_ADP___FUNC__ */
    
/* used in adpModDebug.h */
#if defined(ADAPTOS_VXWORKS_DIAB)
/* diab supports variable args in macro, but w/o a variable name(unlike gnu) */
#define HAVE_ADP_NO_VAR_NAME_VAR_ARG 
#endif /* ADAPTOS_VXWORKS_DIAB */

#ifndef HAVE_ADP_CONNECT_WITH_TO
#define HAVE_ADP_CONNECT_WITH_TO
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCadpVxTypesH */
