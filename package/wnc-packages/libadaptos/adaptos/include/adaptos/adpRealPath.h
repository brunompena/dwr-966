/* adpRealPath.h - interface header for realpath.c */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01c,05sep05,rnp  added MAXPATHLEN
01b,30jan02,mno  fixed prototype of unx_realpath
01a,26jan02,ksn  written.
*/

#ifndef _REALPATH_H
#define _REALPATH_H

/* includes */
#include "adaptos.h"

#ifndef PATH_DEPTH_MAX
#define PATH_DEPTH_MAX  50
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN  PATH_MAX
#endif

char * adp_realpath (char * pathname, char * resolvedname, char * root);

#endif /* _REALPATH_H */
