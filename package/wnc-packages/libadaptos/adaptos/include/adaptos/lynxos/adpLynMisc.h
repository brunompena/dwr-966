/* adpLynMisc.h - misc defines for LynxOS */

/* Copyright (c) 2005 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01b,21jun07,rnp  added HAVE_ADP_STRNCASECMP guard
                 added ADP_DEFINE_MAIN_WRAPPER and ADP_DEFINE_DUMMY_ROUTINE
01a,31aug05,rnp  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpLynMiscH
#define __INCadpLynMiscH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define mktemp(X)           mkstemp(X)

#define mkstemp(X)          adp_mkstemp(X)
#define ADP_MEM_PART_ID_OS  (0)
#define ADP_TICKS_PER_SEC   adpTicksPerSec()
#undef  inet_aton
#define inet_aton(X,Y)      adp_inet_aton(X,Y)
#ifndef HAVE_ADP_STRNCASECMP
#define strncasecmp         adp_strncasecmp
#define HAVE_ADP_STRNCASECMP
#endif /* HAVE_ADP_STRNCASECMP */
#define strcasecmp          adp_strcasecmp
#define gettimeofday        adp_gettimeofday

#define gethostbyname(name)             gethostbyname((char *)name)
#define gethostbyaddr(addr, len, type)  gethostbyaddr((char *)addr, len, type)
#define getservbyname(name, proto)      getservbyname((char *)name, proto)

#define min(X,Y)        ((X)<(Y)?(X):(Y))
#define max(X,Y)        ((X)>(Y)?(X):(Y))

/* order of arguments is different on lynxOS */
#define getpwuid_r(UID,REC,BUF,BUFSIZE) getpwuid_r((REC),(UID),(BUF),(BUFSIZE))

#define adpGetPid()     getpid()
#define adpAbort()      abort()

IMPORT int opterr;
IMPORT int optind;
IMPORT char * optarg;

#ifndef ADP_DEFINE_MAIN_WRAPPER
#define ADP_DEFINE_MAIN_WRAPPER(function)
#endif /* ADP_DEFINE_MAIN_WRAPPER */

#ifndef ADP_DEFINE_DUMMY_ROUTINE
#define ADP_DEFINE_DUMMY_ROUTINE(funcName) \
STATUS funcName (void) { return ST_OK; }
#endif /* ADP_DEFINE_DUMMY_ROUTINE */

#ifdef __cplusplus
}
#endif

#endif /* __INCadpLynMiscH */
