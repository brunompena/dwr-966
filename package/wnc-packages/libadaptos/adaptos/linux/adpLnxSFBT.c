/* adpLnxSFBT.c - seg-fault backtrace */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01a,12oct07,rnp  written
*/

/*
DESCRIPTION
This file implements a routine that can be invoked on seg-fault to provide a
backtrace
*/

#define _GNU_SOURCE
#include "adaptos.h"
#include "dlfcn.h"
#include "execinfo.h"

/* define if using ldl */
#undef CONFIG_LDL
#ifndef HAVE_ADP_LNX_SFBT_OPT
#define ADP_LNX_SFBT_DUMP_TO_STDOUT   1
#define ADP_LNX_SFBT_DUMP_TO_STDERR   2
#define ADP_LNX_SFBT_DUMP_TO_SYSLOG   3

typedef struct adpLnxSFBTOpt
    {
    /* should the routine invoke exit after dumping */
    BOOL sfbtExit;
    /* where to dump the backtrace */
    int  sfbtDumpDest;
    } ADP_LNX_SFBT_OPT;

IMPORT ADP_LNX_SFBT_OPT adpLnxSFBTOpt;
#define HAVE_ADP_LNX_SFBT_OPT
#endif /* HAVE_ADP_LNX_SFBT_OPT */

ADP_LNX_SFBT_OPT adpLnxSFBTOpt =
    {
    TRUE, 
    ADP_LNX_SFBT_DUMP_TO_STDOUT,
    };

/*******************************************************************************
*
* adpLnxSFBT - dump a function backtrace
* 
* This routine dumps a function backtrace. The behavior is controlled by the
* options specified in adpLnxSFBTOpt.
*/
void adpLnxSFBT(void)
    {
    void * btrace[50];
    int size, i;
#ifndef CONFIG_LDL
    char ** str;
#endif /* CONFIG_LDL */
    size = backtrace(btrace, sizeof(btrace)/sizeof(void *));
#ifndef CONFIG_LDL
    str = backtrace_symbols(btrace,size);
#endif /* CONFIG_LDL */
    for (i = 0; i < size; i++)
        {
        Dl_info dli;
        memset(&dli, 0, sizeof(dli));
#ifdef CONFIG_LDL
        dladdr(btrace[i], &dli);
        if (adpLnxSFBTOpt.sfbtDumpDest == ADP_LNX_SFBT_DUMP_TO_STDOUT)
            fprintf(stdout, "[0x%08x]0x%08x+%08x: %s(%s)\n", btrace[i],
                    dli.dli_fbase, btrace[i] - (int)dli.dli_fbase, 
                    dli.dli_fname, dli.dli_sname);
        else if (adpLnxSFBTOpt.sfbtDumpDest == ADP_LNX_SFBT_DUMP_TO_STDERR)
            fprintf(stderr, "[0x%08x]0x%08x+%08x: %s(%s)\n", btrace[i],
                    dli.dli_fbase, btrace[i] - (int)dli.dli_fbase, 
                    dli.dli_fname, dli.dli_sname);
        else if (adpLnxSFBTOpt.sfbtDumpDest == ADP_LNX_SFBT_DUMP_TO_SYSLOG)
            syslog(LOG_DEBUG, "[0x%08x]0x%08x+%08x: %s(%s)\n", (UINT)btrace[i],
                   (UINT)dli.dli_fbase, (UINT)btrace[i] - (UINT)dli.dli_fbase, 
                   dli.dli_fname, dli.dli_sname);
#else /* !CONFIG_LDL */
        if (adpLnxSFBTOpt.sfbtDumpDest == ADP_LNX_SFBT_DUMP_TO_STDOUT)
            fprintf(stdout, "%s\n", str[i]);
        else if (adpLnxSFBTOpt.sfbtDumpDest == ADP_LNX_SFBT_DUMP_TO_STDERR)
            fprintf(stderr, "%s\n", str[i]);
        else if (adpLnxSFBTOpt.sfbtDumpDest == ADP_LNX_SFBT_DUMP_TO_SYSLOG)
            syslog(LOG_DEBUG, "%s\n", str[i]);
#endif /* CONFIG_NO_LDL */
        }

    if (adpLnxSFBTOpt.sfbtExit)
        exit(1);
    }
