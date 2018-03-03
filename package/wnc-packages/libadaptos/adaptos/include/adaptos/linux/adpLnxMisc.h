/* adpLnxMisc.h - misc defines for Linux OS */

/* Copyright (c) 2005 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01g,04sep08,rnp  added adpReadLink define
01e,26nov08,sdk  added HAVE_ADP_MINMAX macro
01f,28jun08,rnp  added HAVE_ADP_VSYSLOG
01e,07dec07,rnp  added HAVE_ADP_PASSWD macro
01d,24jul07,rnp  added HAVE_ADP_DAEMON_API macro
01c,21jun07,rnp  added HAVE_ADP_STRNCASECMP guard
                 added ADP_DEFINE_MAIN_WRAPPER and ADP_DEFINE_DUMMY_ROUTINE
01b,05sep05,rnp  fixed some warnings
                 added defines for adpGetPid and apdAbort
01a,07may05,sam  misc types for linux.
*/

#ifndef __INCadpLnxMiscH
#define __INCadpLnxMiscH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define mktemp(X)           mkstemp(X)

#define ADP_MEM_PART_ID_OS  (0)
#define ADP_TICKS_PER_SEC   adpTicksPerSec()
#undef  inet_aton
#define inet_aton           adp_inet_aton
#ifndef HAVE_ADP_STRNCASECMP
#define strncasecmp         adp_strncasecmp
#define HAVE_ADP_STRNCASECMP
#endif /* HAVE_ADP_STRNCASECMP */
#define strcasecmp          adp_strcasecmp
#define gettimeofday        adp_gettimeofday
#define gethostbyname(name) gethostbyname(name)
#ifndef HAVE_ADP_MINMAX
#define min(X,Y)    ((X)<(Y)?(X):(Y))
#define max(X,Y)    ((X)>(Y)?(X):(Y))
#define HAVE_ADP_MINMAX
#endif /*HAVE_ADP_MINMAX*/

#define adpGetPid()     getpid()
#define adpAbort()      abort()
#define adpReadLink(l,f,s)   readlink(l,f,s)

#ifndef ADP_DEFINE_MAIN_WRAPPER
#define ADP_DEFINE_MAIN_WRAPPER(function)
#endif /* ADP_DEFINE_MAIN_WRAPPER */

#ifndef ADP_DEFINE_DUMMY_ROUTINE
#define ADP_DEFINE_DUMMY_ROUTINE(funcName) \
STATUS funcName (void) { return ST_OK; }
#endif /* ADP_DEFINE_DUMMY_ROUTINE */

#ifndef HAVE_ADP_DAEMON_API
#define HAVE_ADP_DAEMON_API
#endif /* HAVE_ADP_DAEMON_API */

#ifndef HAVE_ADP_PASSWD
#define HAVE_ADP_PASSWD
#endif /* HAVE_ADP_PASSWD */

#ifndef HAVE_ADP_VSYSLOG
#define HAVE_ADP_VSYSLOG
#endif /* HAVE_ADP_VSYSLOG */

#ifdef __cplusplus
}
#endif

#endif /* __INCadpLnxMiscH */
