/* adpGetTimeOfDay.c - get the time of day. */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,08apr08,san  written
*/

#include "adaptos.h"

/*******************************************************************************
*
* adp_gettime  - get the current time 
*
* This function gets the current time measured in seconds from 
* 1970 jan 1st hours 00:00
* 
* RETURNS: time_t time in seconds.
*/

int adp_time()
    {
    Time t;
    Error result;
    long seconds;

    result = GetClockTime(PrimaryClock, &t);
    if (result != Success)
        {
        return (ST_ERROR);
        }
    seconds = t.Seconds;
    return (seconds);
    }


/*******************************************************************************
*
* adp_gettimeofday - get the time of the day.
*
* This function calls the gettimeofday function of the C library in the 
* Integrity operating system.
* 
* RETURNS: ST_OK on success, or ST_ERROR otherwise
*/

int adp_gettimeofday 
    (
    struct timeval *tp, 
    void *ignore
    )
    {
    Time t;
    Error result;

    result = GetClockTime(PrimaryClock, &t);
    if (result != Success)
        {
        return (ST_ERROR);
        }

    tp->tv_sec  = t.Seconds;
    tp->tv_usec = 0;
    return (ST_OK); 
    }
