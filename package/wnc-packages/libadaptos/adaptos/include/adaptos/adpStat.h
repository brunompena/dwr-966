/* adpStat.h - file stat support */

/* Copyright (c) 2002, TeamF1, Inc. */

/*
modification history
--------------------
01a,04jun02,mno  created.
*/

#ifndef __INCadpStatH
#define __INCadfStatH

#ifdef __cplusplus
extern "C" {
#endif

#include "adaptos.h"

IMPORT STATUS adpStat (const char * file, struct stat * pStat);

#ifdef __cplusplus
}
#endif
#endif /* __INCadfStatH */
