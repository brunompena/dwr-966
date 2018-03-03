/* adpLogLib.c - logging library  */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,07aug08,rnp  written
*/

/* includes */

#include <adaptos.h>

/* defines */

/* typedefs */

/* externs */

/* locals */
LOCAL int  adpLogType = ADP_LOG_AUTO;
LOCAL int  adpLogLevel = ADP_DEBUG_LEVEL_DEFAULT;
/* forward declarations */

/* globals */

LOCAL int adpLogCanPrintToStdout (void)
    {
    int pid = adpGetPid();
    char buf[20];
    char file[20] = {0};

    snprintf(buf, sizeof(buf), "/proc/%d/fd/1", pid);
    memset(file, 0, sizeof(file));
    /* read the stdout link to see where it points */
    adpReadLink(buf, file, sizeof(file)-1);
    /* 
     * if it doesnt point to null, then we're in a foreground process and so we
     * can print to stdout
     */ 
    if (strcmp(file, "/dev/null"))
        return 1;
    return 0;
    }
    
#ifdef __WNC_OG36XX__

void adp_wnc_print    ( 
    const char *  function,
    int           line,
    char *        pszFormat,      /* format */
    ...                           /* variable args */
    )
{
	va_list             arg;
	char cmd[512];
	char fmt[256];

	{

		va_start (arg, pszFormat);
		vsprintf(fmt,pszFormat, arg);
		va_end (arg);
		
		sprintf(cmd,"echo \"<%s:%d> %s\"  >> /var/log/messages",function,line,fmt);
		system(cmd);
		//adpMemPartFree(NULL, fmt);
		
	}
	

	
}   

#endif

/*******************************************************************************
* adpLogTypeGet - get adpLogType
*
* This routine gets adpLogType
*
* RETURNS: logType
*/
int adpLogTypeGet (void)
    {
    return adpLogType;
    }

/*******************************************************************************
* adpLogTypeSet - set adpLogType
*
* This routine sets adpLogType
*
* RETURNS: N/A
*/
void adpLogTypeSet
    ( 
    int logType
    )
    {
    adpLogType = logType;
    }

/*******************************************************************************
*
* adpLog - logs the debugging information
*
* This routine logs the debugging information
*
* RETURNS: N/A
*/
void adpLog
    ( 
    int           debugLevel,     /* debugging level */
    const char *  function,
    int           line,
    char *        pszFormat,      /* format */
    ...                           /* variable args */
    )
    {
    va_list             arg;

    /* if logging is disabled, return right away */
    if (adpLogType == ADP_LOG_NONE)
        return;

    /* is this level on */       
    if (adpLogLevel & debugLevel)
        {
        char * fmt = pszFormat;
        /* avoid function:line prefix for ADP_PRINTF */
        if (function && debugLevel != ADP_DEBUG_LEVEL_INFO)
            {
            int len = strlen(function)+strlen(pszFormat)+15;
            fmt = adpMemPartAlloc(NULL, len);
            if (fmt == NULL)
                /* fall-back to user-supplied format string */
                fmt = pszFormat;
            else
                snprintf(fmt, len, "<%s:%d> %s", 
                         function, line, pszFormat);
            }
        if (adpLogType == ADP_LOG_AUTO)
            {
            va_start (arg, pszFormat);
            if (adpLogCanPrintToStdout())
                vprintf (fmt, arg);
#ifdef HAVE_ADP_VSYSLOG
            else
                vsyslog (LOG_INFO, fmt, arg);
#endif /* HAVE_ADP_VSYSLOG */
            va_end (arg);
            }
        else
            {
#ifdef HAVE_ADP_VSYSLOG
            if (adpLogType & ADP_LOG_SYSLOG)
                {
                va_start (arg, pszFormat);
                /* note: vsyslog doesnt work on vxworks */
                vsyslog (LOG_INFO, fmt, arg);
                va_end (arg);
                }
#endif /* HAVE_ADP_VSYSLOG */
            if (adpLogType & ADP_LOG_STDOUT)
                {
                va_start (arg, pszFormat);
                vprintf (fmt, arg);
                va_end (arg);
                }
            }
        if (fmt != pszFormat)
            adpMemPartFree(NULL, fmt);
        }
    return;
    }

/*******************************************************************************
*
* adpLogHex - prints the string in hex format
*
* This routine prints the string in hex format.
*
* RETURNS: N/A
*/

void adpLogHex 
    (
    int     debugLevel, /* debugging level */
    char *  pText,      /* text to display before printing the str*/
    void *  pStr,       /* binary string */
    int     len         /* string length */
    )
    {
    int   i = 0;
    char  buf[100];
    int   offset = 0;

    if (pText)
        {
        adpLog(debugLevel, NULL, 0, "%s (hexdump bytes: %d)\n",pText,len);
        }
    for (i = 0; i < len; i++)
        {
        offset += snprintf(buf+offset, sizeof(buf)-offset, 
                           "%02x ",*((unsigned char *)pStr+i));
        if ((i + 1) % 16 == 0)
            {
            adpLog(debugLevel,NULL,0,"%s\n", buf);
            offset = 0;
            }
        }
    if (i % 16 != 0)
        adpLog(debugLevel,NULL,0,"%s\n", buf);
    }

/*******************************************************************************
*
* adpLogLevelSet - set current log level
* 
* This routine sets the current log level
*
* RETURNS: ST_OK or ERROR
*/

int adpLogLevelSet
    (
    int  level  /* module ID */
    )
    {
    adpLogLevel = level;
    return (ST_OK);
    }

/*******************************************************************************
*
* adpLogLevelGet - get current log level
*
* This routine gets current log level
*
* RETURNS: ST_OK or ERROR
*/

int adpLogLevelGet (void)
    {
    return adpLogLevel;
    }
