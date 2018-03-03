/* adpGetTimeOfDay.c - get the time of day. */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,22feb05,rks  written
*/

#include "adaptos.h"

#ifdef gettimeofday
#undef gettimeofday
#endif

/*******************************************************************************
*
* adp_gettimeofday - get the time of the day.
*
* This function calls the gettimeofday function of the C library in the Linux
* operating system.
* 
* RETURNS: ST_OK on success, or ERROR otherwise
*/

int adp_gettimeofday (struct timeval *tp, void *ignore)
    {
    return gettimeofday (tp, (struct timezone *)ignore);
    }
