/* adpDebug.c - set/unset debug levels */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,08apr08,san  written (based on linux\adpDebug.c)
*/

/*
DESCRIPTION
This file provides the binary to get/set log levels in the components that are
registered with the TeamF1's Universal Management Interface (UMI).
*/

/* includes */

#include "adaptos.h"


/* globals */

/* imports */
IMPORT int adpDebug (char * , int compId, char *  arg2);

/*******************************************************************************
*
* printUsage - print usage for adpDebug binary
* 
* DESCRIPTION:
* This function prints the usage for the adpDebug binary.
* 
* RETURNS: N/A
*
* NOMANUAL
*/

static void printUsage (char * progName)
    {
    printf ("Usage:\n");
    printf ("     :%s on <compId> <levelNo>\n", progName);
    printf ("     :%s off <compId> <levelNo>\n", progName);
    }

/*******************************************************************************
*
* main - main routine for adpDebug calls.
* 
* This routine is used to call the adpDebug calls.
*
* NOMANUAL
*
* RETURNS: 0 for success , -1 for error
*  
*/

int main 
    (
    int argc,    /* number of arguments */
    char ** argv /* argument list */
    )
    {
    if (argc < 3 )
        {
        printUsage (argv[0]);
        return (ST_ERROR);
        }

    if (adpDebug (argv[1], atoi(argv[2]), argv[3]) == ST_ERROR)
        {
        printUsage (argv[0]);
        return (ST_ERROR);
        }

    return (ST_OK);
    }
