/* adpTypes.h - adaptos types */

/* Copyright (c) 2002 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01t,01jul07,rnp  changed ADP___P to HAVE_ADP___P
                 added NEED_ADP_XOPEN_SOURCE
01s,16aug06,rnp  added ADP_KEY_T
01r,09sep05,rnp  removed vxworks references
                 added __P define
01q,12mar05,uge  added sys/times.h 
01p,19feb05,chd  added string.h, sys/time.h and arap/inet.h 
                 and used #ifndef ADAPTOS_LINUX for supporting linux.
01o,19aug03,gnm  removed adpBzero declaration.
01n,16aug03,ksn  fixed function prototypes.
01m,23jul03,ws   defined mkstemp, mkstemps, mkdtemp, strcasecmp, strncasecmp
                 and gettimeofday to their adaptos equivalents.
01l,14jul03,ws   undefined NULL to fix problems in Tornado 2.2.
01k,08jul03,ksn  added macro for inet_aton
01j,08jul03,ws   changed return type of adpAlarm from int to unsigned int.
01i,02apr03,ksn  removed lstat mapping and declaration of
                 strcase funcstions.
01h,08mar03,ksn  added support for vxWorks
01g,27dec02,mk   exposed in_addr_t for pSOS
01f,07aug02,ksn  added stat, fstat, lstat mapping
01e,31jul02,mk   exposed u_int32_t for pSOS
01d,29jun02,ksn  added definitions for pSOS-2x.
01c,06jun02,mno  removed SEM_ID compatibility definition
01b,25may02,mk   added code for psos
01a,15may02,mno  created.
*/

#ifndef __INCadpTypesH
#define __INCadpTypesH

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include "adaptos.h"

#ifndef NULL_STR
#define NULL_STR        ((char *)0)
#endif

#ifndef NULL_CHAR
#define NULL_CHAR       ('\0')
#endif

#ifndef ADAPTOS_BUFSIZE
#define ADAPTOS_BUFSIZE     64
#endif

#undef NULL
#ifndef NULL
#define NULL            0
#endif /* NULL */

/* Integer types for 32 bit CPU */

#ifndef HAVE_ADP_INT_8
typedef char            INT_8;
#define HAVE_ADP_INT_8
#endif

#ifndef HAVE_ADP_UINT_8
typedef unsigned char   UINT_8;
#define HAVE_ADP_UINT_8
#endif

#ifndef HAVE_ADP_INT_16
typedef short           INT_16;
#define HAVE_ADP_INT_16
#endif

#ifndef HAVE_ADP_UINT_16
typedef unsigned short  UINT_16;
#define HAVE_ADP_UINT_16
#endif

#ifndef HAVE_ADP_INT_32
typedef long            INT_32;
#define HAVE_ADP_INT_32
#endif

#ifndef HAVE_ADP_UINT_32
typedef unsigned long   UINT_32;
#define HAVE_ADP_UINT_32
#endif

#ifndef HAVE_ADP_FUNCPTR
typedef int             (*ADP_FUNCPTR)();
#define HAVE_ADP_FUNCPTR
#endif

#ifndef HAVE_ADP_ADP_VOIDFUNCPTR
typedef void            (*ADP_VOIDFUNCPTR)();
#define HAVE_ADP_ADP_VOIDFUNCPTR
#endif

#ifndef HAVE_ADP_VOIDPTRFUNCPTR
typedef void *          (*ADP_VOIDPTRFUNCPTR)();
#define HAVE_ADP_VOIDPTRFUNCPTR
#endif

#ifndef HAVE_ADP_MALLOC_FUNCPTR
typedef void *          (*ADP_MALLOC_FUNCPTR) (size_t);
#define HAVE_ADP_MALLOC_FUNCPTR
#endif

#ifndef HAVE_FREE_FUNCPTR
typedef void            (*ADP_FREE_FUNCPTR) (void *);
#define HAVE_FREE_FUNCPTR
#endif
    
#ifndef HAVE_ADP_KEY_T
typedef int ADP_KEY_T;
#define HAVE_ADP_KEY_T
#endif
    
#ifndef NONE
#define  NONE           (-1)
#endif /* NONE */

#ifndef STD_IN
#define STD_IN          0
#define STD_OUT         1
#define STD_ERR         2
#endif /* STD_IN */

#ifndef HAVE_ADP___P
#define HAVE_ADP___P
#undef __P
#define __P(x) x    /* ANSI only */
#endif /* HAVE_ADP___P */

/* used in klite/rand/randfile.c - perhaps can undef'ed for vxworks 5.x also */
#define NEED_ADP_XOPEN_SOURCE
#ifdef ADAPTOS_VXWORKS_6X
#undef NEED_ADP_XOPEN_SOURCE
#endif /* ADAPTOS_VXWORKS_6X */

#ifdef __cplusplus
}
#endif
#endif /* __INCadpTypesH */

