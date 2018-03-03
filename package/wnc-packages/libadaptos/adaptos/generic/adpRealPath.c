/* adpRealPath.c - realpath implementation */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01k,15oct05,rnp  merged a bug fix from RC(TF1-HOTFIX-SSH-UNXRLPTH)
01j,19aug03,gnm  removed adp_strtok_r prototype
01i,08jul03,ksn  fixed warning for diab
01h,17jun03,gnm  code cleanup: added module description, added adp_realpath()
                 function header.
01g,17jun03,gnm  modified copyright information, changed module file
                 name at top of module from realpath.c to adp_realpath.c
01f,08jul02,mno  added prototype for strtok_r for pSOS-2x
01e,27jun02,vnk  fixed relative path issues.
01d,01may02,mno  ported to adaptos framework
01c,12feb02,ksn  fixed strncmp calls.
01b,30jan02,mno  fixed the core translation algo
01a,26jan02,ksn  written.
*/

/*
DESCRIPTION

This module implements functionality to determine the resolved path name by
removing all symbolic components from a given path.

INCLUDES: 
*/

/* includes */
   
#include "adaptos.h"


/* externs */

/*******************************************************************************
*
* adp_realpath - find the real path name
*
* Find the real name of path, by removing all ".", ".." and symlink components.
* 
* RETURNS: The resolved path name.
*/

char * adp_realpath
    (
    char * pathname,        /* path name */
    char * resolvedname,    /* resolved name */
    char * root             /* root dir */
    )
    {
    char   ctx[PATH_MAX];
    char * pPath[PATH_DEPTH_MAX];
    int    depth = 0;
    int    i;

    resolvedname[0] = '\0';
    pPath[0] = strtok_r (pathname, "/", (char **)&ctx);

    if ((pPath[0] == NULL) ||
        (strcmp (pPath[0], ".") == ST_OK))
        {
        strcpy (resolvedname, root);
        return (resolvedname);
        }
   
        sprintf (resolvedname, "%s", root);
    
    depth++;
    
    while ((pPath[depth] = strtok_r (NULL, "/", (char **)&ctx)) != NULL)
        {
        if (strcmp (pPath[depth], "..") == ST_OK)
            depth--;
        else if (strcmp (pPath[depth], ".") != ST_OK)
            depth++;

        if (depth < 0)
            depth=0;
        }

    for (i = 1; i < depth; i++)
        {
        sprintf (resolvedname, "%s/%s", resolvedname, pPath[i]);
        }
    return (resolvedname);
    }
