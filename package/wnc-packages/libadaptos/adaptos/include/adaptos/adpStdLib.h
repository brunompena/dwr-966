/* adpStdLib.h - declaration for standard C library functions */

/* Copyright (c) 2005 - 2006, TeamF1, Inc. */

/*
modification history
--------------------
01b,14jul06,rks  added adp_strtok_r
01a,05may05,sam  written
*/

#ifndef __INCadpStdLibH
#define __INCadpStdLibH

#ifdef __cplusplus
extern "C" {
#endif

/* includes */
#include "adaptos.h"    

IMPORT int adp_mkstemp (char *);
IMPORT int adp_mkstemps (char *, int);
IMPORT char * adp_mkdtemp (char *);
IMPORT int adp_strncasecmp (const char *s1, const char *s2, int n);
IMPORT int adp_strcasecmp (const char *s1, const char *s2);
IMPORT char * adp_strtok_r (char * s1, const char *sep, char ** ppLast);
IMPORT int adp_gettimeofday (struct timeval *tp, void *ignore);

#ifdef __cplusplus
}
#endif
#endif /* __INCadpStdLibH */
