/* umiProbe.c - functions to get information from UMI library */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01b,10may05,rks  change to send UMI_CMD_STATS to destination comp 
                 instead of UMI
01a,29apr05,sam  written
*/

/*
DESCRIPTION
This file provides a binary to probe the UMI library to get its status.
*/

/* includes */

#include "adaptos.h"
#include "adpModDebug.h"
#include "umi.h"
#include "umiP.h"

/* defines */


/* typedefs */


/* externs */


/* globals */

struct cmd
    {
    char *  cmdName;
    int     cmdVal;
    } cmds [] = { {"stats", UMI_CMD_STATS} };

/* locals */


/* imports */


/* forward declarations */


/*******************************************************************************
*
* umiStatsPrint - print the UMI stats
* 
* DESCRIPTION:
* This function prints the UMI stats.
* 
* RETURNS: 0 for success , -1 for error
*/

void umiStatsPrint 
    (
    UMI_STATS * pStats,   /* pointer to umi stats struct */
    int         compId    /* UMI component ID */
    )
    {
    ADP_PRINTF("UMI STATS FOR COMPONENT: %d \n", compId);
    ADP_PRINTF("=======================================\n");
    ADP_PRINTF("Locks Taken                         :    %d \n",
               pStats->lockTake);
    ADP_PRINTF("Failed to take Locks                :    %d \n",
               pStats->lockTakeFail);
    ADP_PRINTF("Failed to convert args->buf         :    %d \n",
               pStats->argsToBufFail);
    ADP_PRINTF("Locks Given                         :    %d \n",
               pStats->lockGive);
    ADP_PRINTF("Ioctl sendto failed                 :    %d \n",
               pStats->ioctlSendToFail);
    ADP_PRINTF("Async or No-result succedded        :    %d \n",
               pStats->asyncOrNoResultIoctlSuccess);
    ADP_PRINTF("Given up on a sync call             :    %d \n",
               pStats->syncWaitExited);
    ADP_PRINTF("Sync call succeeded                 :    %d \n",
               pStats->syncIoctlSuccess);
    ADP_PRINTF("Failed in umiListen                 :    %d \n",
               pStats->recvFromFailed);
    ADP_PRINTF("Recvd request is too small          :    %d \n",
               pStats->reqTooShort);
    ADP_PRINTF("Recvd a Reply                       :    %d \n",
               pStats->recvReply);
    ADP_PRINTF("Recv request with no reply option   :    %d \n",
               pStats->lockTake);
    ADP_PRINTF("Reply Sent                          :    %d \n",
               pStats->replySent);
    return;
    }

/*******************************************************************************
*
* umiProbe - query the UMI library
* 
* DESCRIPTION:
* This function queries the UMI library.
* 
* RETURNS: 0 for success , -1 for error
*/

static int umiProbe
    (
    int     compId,     /* component ID */
    int     command     /* UMI command */
    )
    {
    int   status = ST_OK;
    UMI_STATS  stats;
    unsigned int outLen=sizeof(UMI_STATS);
    
    umiInit ();

    switch (command)
        {
        case  UMI_CMD_STATS:
            {
            status = umiIoctl (NULL, compId, command, 
                               NULL, 0, &stats, &outLen ,
                               0, 0, 0);
            if (status < 0)
                return (ST_ERROR);

            umiStatsPrint(&stats, compId);
            }
            break;
            
        default:
            break;

        }

    return (status);
    }

/*******************************************************************************
*
* printUsage - print the usage
* 
* DESCRIPTION:
* This function prints the usage.
* 
* RETURNS: N/A
*/

static void printUsage 
    (
    char * progName   /* programe name */
    )
    {
    printf ("Usage:%s %s <compId>\n", progName, cmds[0].cmdName);
    }

/*******************************************************************************
*
* main - main function for umiProbe utility
* 
* DESCRIPTION:
* This is the main function for umiProbe. This utility is used to query the
* umi library.
* 
* RETURNS: 0 for success or -1 for failure.
*/

int main 
    (
    int argc,     /* number of arguments */
    char ** argv  /* argument list */
    )
    {
    int i;

    if (argc < 3 )
        {
        printUsage (argv[0]);
        return ST_ERROR;
        }

    for (i = 0; i < sizeof (cmds)/sizeof (struct cmd); i++)
        {
        if (strcmp (argv[1], cmds[i].cmdName) == 0)
            {
            return umiProbe (atoi(argv[2]), cmds[i].cmdVal);
            }
        }
    
    return  ST_OK;
    }
                            

