/* adpIntMisc.h - misc defines for Integrity OS */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01b,25jun08,rnp  added HAVE_ADP_PASSWD macro
                 removed protection around mapping of gethostbyname,
                 gethostbyaddr, etc. Changed path of ADP_DEFAULT_HOSTS_FILE
                 defined CONFIG_ADP_HOSTS_FILE
01a,08apr08,san  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpIntMiscH
#define __INCadpIntMiscH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
#define min(X,Y)    ((X)<(Y)?(X):(Y))
#define max(X,Y)    ((X)>(Y)?(X):(Y))

#define adpGetPid()     getpid()
#define adpAbort()      abort()

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

#define hostent             adp_hostent
#define servent             adp_servent 
#define gethostbyname(name) adp_gethostbyname(name, NULL)
#define getservbyname(name, proto)  adp_getservbyname(name, proto)
#define getservbyport(port, proto)  adp_getservbyport(port, proto)
#define gethostbyaddr(addr, len, type)  \
             adp_gethostbyaddr(addr, len, type, (ADP_MALLOC_FUNCPTR) NULL)
#define ADP_DEFAULT_HOSTS_FILE  "/teamf1/hosts" 

#ifdef CONFIG_SIM_USE_WINDOWS_HOST_IO
#define HAVE_FSTAT_PARTIAL
#define USE_ADP_SOCKET_READ_TIME_OUT    1000
#endif /* CONFIG_SIM_USE_WINDOWS_HOST_IO */    

#define time(X) adp_time()

#ifndef HAVE_ADP_PASSWD
#define HAVE_ADP_PASSWD
#endif /* HAVE_ADP_PASSWD */

/* TODO: move to adpConf.h */
#define CONFIG_ADP_HOSTS_FILE

#ifdef __cplusplus
}
#endif

#endif /* __INCadpIntMiscH */
