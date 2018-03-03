/* adpGetOpt.h - interface header for adpGetOpt.c */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01d,10oct05,rnp  fixed some bugs in getopt(SPR #13)
01c,09may05,sam  moved function mapping to adp<os>Misc.h 
01b,23jul03,ws   defined opterr, optopt and optreset
01a,02jul03,ksn  ported from openbsd, and merged old relevant code from
                 getopt file 
*/



#ifndef _ADPGETOPT_H
#define _ADPGETOPT_H

/* includes */
#include "adaptos.h"

#ifndef HAVE_ADP_GETOPT_OBJ
typedef struct getoptObj
    {
    int    adpGetOptValid;  /* indicates validity of other variables inside
                             * BSDgetopt() 
                             */
    char * getOptProgName;  /* program name(internal getopt variable) */
    char * getOptPlace;     /* option letter processing(internal getopt var.) */
    int    BSD_opterr;      /* if error message should be printed */
    int    BSD_optind;      /* index into parent argv vector */
    int    BSD_optopt;      /* character checked for validity */
    int    BSD_optreset;    /* reset getopt */
    char * BSD_optarg;      /* argument associated with option */    
    } GETOPT_OBJ;
#define HAVE_ADP_GETOPT_OBJ
#endif


IMPORT int          BSDgetoptWrap(int argc, char * const *argv, 
                                  const char *opts);
IMPORT void         getoptTableCleanup (UINT    taskId);
IMPORT GETOPT_OBJ * getoptObjGet (void);

#endif /* _ADPGETOPT_H */
