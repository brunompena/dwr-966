/* adpTimerLib.c - adaptos timer library implementation using posix api*/

/* Copyright (c) 2009, TeamF1, Inc. */

/*
modification history
--------------------
01e,22sep09,sam  fixed race conditions - add/del timer to/from list
				 before setting the timer.
01d,20sep09,chd  added memset in adpTimerCreate() function.
01c,19sep08,chd  fixed issue in adpTimerCancel() at return value check 
                 for timer_settime().
01b,03sep05,rnp  removed VXWORKS references.
01a,29jul04,rks  created.
*/

/*
DESCRIPTION

This module implements adaptos timer library using posix timer api.

INCLUDES: adaptos.h, adpTimerLib.h

SEE ALSO: 
*/

/* includes */

#include "adaptos.h"

ADP_LIST    adpTimerList;

static timer_t          masterTimer = (timer_t)-1;
static struct sigevent  sigev;
static struct sigaction sigact;

/*******************************************************************************
*
* adpTimerHandler - common handler for all timer nodes
*
* This routine is called when a timer (node with smallest time value) is 
* expired. The routine calls the handler associated with this time node and
* removoes it from the adpTimerList.
*
* RETURNS: NONE
*/

static void adpTimerHandler
    (
    int     sigNo           /* dummy */
    )
    {
    ADP_TIMER_NODE *    pNode;
    ADP_TIMER_NODE *    pNextNode;
    struct itimerspec    its;

    pNode = (ADP_TIMER_NODE *)adpListFirst(&adpTimerList);

    if (pNode == NULL) 
        return;

    pNextNode =(ADP_TIMER_NODE *) adpListNext((ADP_NODE *)pNode);
    adpListNodeRemove (&adpTimerList, (ADP_NODE *)pNode);

    if (pNextNode)
        {
        its.it_value = pNextNode->ts;
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;

        timer_settime (masterTimer, 0, &its, NULL); 
        }
 
    pNode->funcptr (pNode->arg);
    }
    
/*******************************************************************************
*
* adpTimerCreate - create a timer 
*
* This routine allocates the memory for timer node and if this is the first 
* time this routine is called then it creates a master timer by using posix
* tiemr_create routine. This master timer is the only timer which runs
* on behalf of all timer nodes.
*
* RETURNS: The tick counter value
*/
ADP_TIMER_ID adpTimerCreate(void)
    {
    ADP_TIMER_NODE *    pNode;
    
    if (masterTimer == (timer_t)-1)
        {
        sigev.sigev_value.sival_int=0;
        sigev.sigev_notify = SIGEV_SIGNAL;
        sigev.sigev_signo = SIGALRM;

        if ( timer_create(CLOCK_REALTIME, &sigev, &masterTimer) == 0)
            {
            bzero (&sigact, sizeof(sigact));
            sigact.sa_handler = adpTimerHandler;
            if (sigaction(SIGALRM, &sigact, NULL) != 0)
                {
                timer_delete(masterTimer);
                masterTimer = (timer_t) NULL;
                return ((ADP_TIMER_ID) NULL);
                }
            }
        else
            {
            masterTimer = (timer_t)NULL;
            return ((ADP_TIMER_ID) NULL);
            }
        adpListInit (&adpTimerList);
        }
    pNode = (ADP_TIMER_NODE *) adpMalloc (sizeof (ADP_TIMER_NODE));

    if (pNode == NULL)
        {
        return ((ADP_TIMER_ID) NULL);
        }
    memset (pNode, 0, sizeof (ADP_TIMER_NODE));
    return (ADP_TIMER_ID)(pNode);
    }
/*******************************************************************************
*
* timespec_greater - check if time value in timespec a is greater than b. 
*
* This routine used to check if time value in timespec a is greater than b.
* 
* RETURNS: TRUE or FALSE
*/
static BOOL timespec_greater 
    (
    struct timespec *   a,      /* time specification */
    struct timespec *   b       /* time specification */
    )
    {
    if (a->tv_sec > b->tv_sec)
        return (TRUE);
    if ( (a->tv_sec == b->tv_sec) && (a->tv_nsec > b->tv_nsec))
        return (TRUE);
    return (FALSE);
    }

/*******************************************************************************
*
* timespec_subtract- subtract timespec structure b from a.
* 
* This routine used to substract timespec structure b from a.
*
* RETURNS: NONE
*/
static void timespec_subtract
    (
    struct timespec *   a,      /* time specification */
    struct timespec *   b       /* time specification */
    )
    {
    a->tv_sec -= b->tv_sec;
    if (b->tv_nsec > a->tv_nsec)
        {
        a->tv_sec -= 1;
        a->tv_nsec += (1000000000 - b->tv_nsec); 
        }
    }
/*******************************************************************************
*
* timespec_add - add timespec structure b to a.
*
* This routine used to add timespec structure b to a.
* 
* RETURNS: NONE
*/
static void timespec_add
    (
    struct timespec *   a,      /* time specification */
    struct timespec *   b       /* time specification */
    )
    {
    a->tv_sec += b->tv_sec;
    if ((a->tv_nsec + b->tv_nsec) >= 1000000000)
        {
        a->tv_sec += 1;
        a->tv_nsec = (a->tv_nsec + b->tv_nsec) - 1000000000;
        }
    else
        {
        a->tv_nsec +=  b->tv_nsec;
        }
    }
         
     
/*******************************************************************************
*
* adpTimerStart - start the specified timer
*
* This routine start the timer with the given value. Upon expiration of this 
* time funcptr will be called with arg as argument.
*
* RETURNS: ST_OK on success.
*/

STATUS adpTimerStart
    (
    ADP_TIMER_ID        timerId,    /* timer id created by adpTimerCreate */
    ADP_TIME_SPEC       ts,         /* time in timespec structure */
    ADP_FUNCPTR         funcptr,    /* handler routine */
    int                 arg         /* argument to handler routine */
    )
    { 
    ADP_TIMER_NODE *    pNode;
    ADP_TIMER_NODE *    pNewNode;
    ADP_TIMER_NODE *    pLastNode;
    struct itimerspec    its;
    int restartTimer = 0;	

    /*
     * Timers are managed in a link list with the smallest timer being
     * the first on the list. All other nodes in the list stores the time
     * interval from earlier nodes.
     */
    
    pNewNode = (ADP_TIMER_NODE *)timerId;

    pNewNode->funcptr  = funcptr;
    pNewNode->arg      = arg;
    pNewNode->ts       = ts;

    if ((pNode = (ADP_TIMER_NODE *)adpListFirst(&adpTimerList)) == NULL)
        {
        /* this is the first node in the list */
        its.it_value = ts;
        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;

        adpListNodeInsert (&adpTimerList, NULL, (ADP_NODE *)pNewNode);

        if (timer_settime (masterTimer, 0, &its, NULL) == 0)
            {
            return (ST_OK);
            }
        else
            {
            adpListNodeRemove(&adpTimerList, (ADP_NODE *)pNewNode);
            return (ST_ERROR);
            }
        }
    else
        {

        /* find the time passed since we started the timer last time */
        if(timer_gettime (masterTimer, &its) != 0)
            {
            return (ST_ERROR);
            }
       /* update the first node time by this value */
        pNode->ts = its.it_value;
        

        /* now find a place for this new node */
        for (pLastNode = NULL; pNode != NULL; pNode = (ADP_TIMER_NODE *)
                                        adpListNext((ADP_NODE *)pLastNode))
            {
            if (timespec_greater(&pNewNode->ts, &pNode->ts))
                {
                timespec_subtract(&pNewNode->ts, &pNode->ts);
                pLastNode = pNode;
                continue;
                }
            else
                {
                /* we have found the place for new node. Update pNode
                 * time value to store the difference from this new node.
                 */
                timespec_subtract(&pNode->ts, &pNewNode->ts);
                
                if (pLastNode == NULL)
                    {
                    /* if this new node is going to be the first in the list,
                     * we need to restart the master timer.
                     */

                    its.it_value = pNewNode->ts;
                    its.it_interval.tv_sec = 0;
                    its.it_interval.tv_nsec = 0;

		            restartTimer = 1;		
                    }
                /* break from for loop */
                break;
                }
            }

        adpListNodeInsert (&adpTimerList, (ADP_NODE *)pLastNode,
              			   (ADP_NODE *)pNewNode);

        /* restart the timer with new value */
		if (restartTimer)
			{
	        if (timer_settime (masterTimer, 0, &its, NULL) != 0)
    	        {
        	    adpListNodeRemove(&adpTimerList, (ADP_NODE *)pNewNode);
            	return (ST_ERROR);
            	}
			}
        }

    return (ST_OK);
    }

/*******************************************************************************
*
* adpTimerCancel - cancels the timer
*
* This routine cancels the timer and removes it from the list of running timer
* nodes.
*
* RETURNS: ST_OK or ST_ERROR
*/

STATUS adpTimerCancel
    (
    ADP_TIMER_ID    timerId     /* timer Id */
    )
    {
    ADP_TIMER_NODE *    pNode;
    ADP_TIMER_NODE *    pTmpNode;
    struct itimerspec    its;
    
    pNode = (ADP_TIMER_NODE *)timerId;

    pTmpNode = (ADP_TIMER_NODE *)adpListFirst(&adpTimerList);

    if (pTmpNode == pNode)
        {
        /* we are canceling the first node in the list so find out the
         * time remaning for this timer to expire and add that value to next
         * node and restart the master timer.
         */
        if(timer_gettime (masterTimer, &its) != 0)
            {
            return (ST_ERROR);
            }
 
        pTmpNode = (ADP_TIMER_NODE *)adpListNext((ADP_NODE *)pTmpNode);

        if (pTmpNode)
            {
            timespec_add(&pTmpNode->ts, &its.it_value);
            its.it_value = pTmpNode->ts;
           }
        else
            {
            /* no more nodes in the list. so stop the timer */
            its.it_value.tv_sec = 0;
            its.it_value.tv_nsec = 0;
            }

        its.it_interval.tv_sec = 0;
        its.it_interval.tv_nsec = 0;

        /* now remove the node from the list */
	    adpListNodeRemove(&adpTimerList, (ADP_NODE *)pNode);
        if (timer_settime (masterTimer, 0, &its, NULL) != 0)
            {
        	adpListNodeInsert (&adpTimerList, NULL, (ADP_NODE *)pNode);
            return (ST_ERROR);
            }

        return (ST_OK);
        }
                
    while ( (pTmpNode = (ADP_TIMER_NODE *) 
                            adpListNext((ADP_NODE *)pTmpNode)) != NULL)
        {
        if (pTmpNode == pNode)
            {
            pTmpNode = (ADP_TIMER_NODE *) adpListNext((ADP_NODE *)pTmpNode);
            timespec_add(&pTmpNode->ts, &pNode->ts);
            adpListNodeRemove(&adpTimerList, (ADP_NODE *)pNode);
            break;
            }
        }
    if (pTmpNode == NULL)
        {
        /* we could not find pNode in the timer list */
        return (ST_ERROR);
        }
    return (ST_OK);
    }
                    
/*******************************************************************************
*
* adpTimerDelete - delete the timer node
*
* This routine cancels the timer (if it was running) and frees the memory 
* allocated to it.
*
* RETURNS: NONE
*/
void adpTimerDelete
    (
    ADP_TIMER_ID    timerId     /* timer Id */
    )
    {
    adpTimerCancel(timerId);
    adpFree (timerId);
    }
/*******************************************************************************
*
* adpTimerListShow - display the timer list
*
* This routine used to display the timer list.
* 
* RETURNS: NONE
*/
void adpTimerListShow (void)
    {
    ADP_TIMER_NODE *    pNode;
    int                 i=0;

    pNode = (ADP_TIMER_NODE *)adpListFirst(&adpTimerList);
    
    while (pNode)
        {
        printf("**Node%d**\n",i);
        printf("timer: %ld secs %ld nanosecs\n",pNode->ts.tv_sec,
                                        pNode->ts.tv_nsec);
        printf("funcptr=%p arg= %x\n", pNode->funcptr, pNode->arg);
        i++;
         pNode = (ADP_TIMER_NODE *)adpListNext((ADP_NODE *)pNode);
        }
    }
    
/*******************************************************************************
*
* adpTimeSet - set the clock to a specified time
*
* This routine sets the clock according to the <year>, <month>, <day>, <hour>,
* <mins> and <sec> arguments passed to this routine.
*
* RETURNS: ST_OK on success.
*/

STATUS adpTimeSet
    (
    int         year,           /* year in format YYYY */
    int         month,          /* month  */
    int         day,            /* day of the month */
    int         hour,           /* hours in 24 hr mode */
    int         mins,           /* minutes */
    int         sec             /* seconds */
    )
    {
    struct tm           currentTime;
    time_t          nowTime;
    struct timespec     newTime;
    char        asctimeBuf[TIME_STR_SIZE];
    size_t      asctimeBufLen;
    
    memset ((void *) &currentTime, 0, sizeof (currentTime));
    currentTime.tm_sec = sec;
    currentTime.tm_min = mins;
    currentTime.tm_hour = hour;
    currentTime.tm_mday = day;
    currentTime.tm_mon = month - 1;
    currentTime.tm_year = year - 1900;

    nowTime = mktime(&currentTime);
    newTime.tv_sec = nowTime;
    newTime.tv_nsec = 0;
    clock_settime (CLOCK_REALTIME, &newTime);
    
    nowTime = time (0);
    localtime_r (&nowTime, &currentTime);
    asctimeBufLen = TIME_STR_SIZE;
    asctime_r (&currentTime, asctimeBuf); 
    printf ("local clock is set to:  %s\n", asctimeBuf);

    return (ST_OK);    
    }
    
/*******************************************************************************
*
* adpTicksPerSec - granularity of the system timer
*
* This routine returns the granularity of the system timer.
*
* RETURNS: sysconf(_SC_CLK_TCK).
*/
int adpTicksPerSec (void)
    {
    return sysconf(_SC_CLK_TCK);
    }
    
/*******************************************************************************
*
* adpTickGet32 - find elapsed tick count from the moment system was booted.
*
* The  function times returns the number of clock ticks that have elapsed since
* an arbitrary point in the past.  For  Linux  this point  is  the moment the 
* system was booted. This return value may overflow the possible range of 
* type clock_t.  On error, (clock_t) -1 is  returned, and errno is set 
* appropriately.
*
* RETURNS:
*   Elapsed tick count.
*   On error, (clock_t) -1 is   returned, and errno is set appropriately.
*/

ULONG adpTickGet32 (void)
    {
    struct tms   tm;
    clock_t      clk;
    
    clk = times (&tm);
    if (clk == ((clock_t) - 1))
        {
        return (ST_ERROR);
        }

    return (clk);
    }
