/* adpRandLib.h -  adaptos random library header file */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01b,28aug05,rnp  added new adpRand API(related types)
01a,22apr03,ksn  created.
*/

#ifndef __INCadpRandLibH
#define __INCadpRandLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include <adaptos.h>

#define S_adpRandLib_NOT_SUPPORTED          (M_adpRandLib | 1)
#define S_adpRandLib_INIT_FAILED            (M_adpRandLib | 2)
#define S_adpRandLib_NOT_YET_SEEDED         (M_adpRandLib | 3)

/* prng function pointer table */
#ifndef HAVE_ADP_RAND_FUNC_TBL
typedef struct adpRandFuncTbl
    {
    STATUS (* randExit) (void);
    STATUS (* randGen) (char *, int);
    STATUS (* randSeed) (char *, int);
    } ADP_RAND_FUNC_TBL;
#define HAVE_ADP_RAND_FUNC_TBL
#endif

/* prng init function pointer type */
#ifndef HAVE_ADP_RAND_PROV_INIT_FUNC
typedef ADP_RAND_FUNC_TBL * (* ADP_RAND_PROV_INIT_FUNC)(int);
#define HAVE_ADP_RAND_PROV_INIT_FUNC
#endif

/* deprecated, user the newer APIs instead */    
IMPORT STATUS   adpRand (char * , int);

IMPORT STATUS   adpRandLibInit (ADP_RAND_PROV_INIT_FUNC , int);
IMPORT STATUS   adpRandLibExit (void);
IMPORT STATUS   adpRandGen (char * , int);
IMPORT STATUS   adpRandSeed (char * , int);

#ifdef __cplusplus
}
#endif

#endif /* __INCadpRandLibH */
