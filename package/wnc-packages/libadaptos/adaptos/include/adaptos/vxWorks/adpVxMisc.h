/* adpVxMisc.h - generic utility routines and definitions */

/* Copyright (c) 2003 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01r,04sep08,rnp  added adpReadLink define
01q,07dec07,rnp  added defines for getpwnam() and crypt()
01p,27oct07,rnp  added adpVSysLog prototype
01o,24jul07,rnp  added HAVE_ADP_DAEMON_TASK macro
                 added ADP_MEM_TRACK protection to adpMalloc, adpFree and
                 adpCalloc mappings
01n,21jun07,rnp  added ADP_DEFINE_MAIN_WRAPPER
                 added HAVE_ADP_STRNCASECMP guard
                 added prototypes for adpInetNtop(), adpInetPton(), adpStrDup()
                 removed prototypes of adp_getservbyport(), adp_gethostbyname(),
                 adp_gethostbyaddr(), adp_getservbyname(), since they are in
                 adpNetLib.h
                 moved defines for hostent and servent under ifndef
                 ADAPTOS_VXWORKS_6X
                 moved mappings for getaddrinfo and getnameinfo under ifndef
                 HAVE_ADP_GETADDRINFO
                 moved adpNameInfoGet() and adpAddrInfoGet() under
                 HAVE_ADP_GETADDRINFO instead of ifndef ADAPTOS_VXWORKS_6X
01m,24may07,san  added strerror_r()
01l,02oct06,pkk  redefined isXXX routines to adpIsXXX 
01k,24aug06,chd  fixed compilation warnings.
01j,14jul06,rks  added adp_strtok_r
01i,20jun06,chd  updated geteuid mapping
01h,07jun06,chd  removed some mappings for vxWorks^x version.
01g,15nov05,chd  added adpStrDup, adpGlob, adpGlobFree, adpEUidGet, adpVSysLog,
                 adpStrToUQ, adpStrToULL, adpIfNameIndex, adpIfNameIndexFree,
                 adpIfAddrsGet, adpIfAddrsFree.
01f,09nov05,chd  added adpNameInfoGet, adpAddrInfoGet, adpProtoEntGet,
                 adpProtoEntSet, adpProtoEntEnd, adpProtoByNumberGet, 
                 adpInetNtop and adpInetPton.
01e,05sep05,rnp  added adp_getservbyport
                 added ADP_TIME_SPEC and ADP_TIMER_ID
                 added defines for adpGetPid and adpAbort
01d,08jul03,ksn  removed macro inet_aton
01c,02may03,ksn  removed sockpair_duplex and strncasecmp
01b,02may03,ws   added define for getpid
01a,12jan03,ksn  created from vxcompat.h
*/

#ifndef __INCadpVxMiscH
#define __INCadpVxMiscH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
/* includes */
#include "adaptos.h"
#include <adpTypes.h>

/* definitions */
#define MAIN_EXEC_PARAMS_DEF                                            \
        char *arg1,                                                     \
        char *arg2,                                                     \
        char *arg3,                                                     \
        char *arg4,                                                     \
        char *arg5,                                                     \
        char *arg6,                                                     \
        char *arg7,                                                     \
        char *arg8,                                                     \
        char *arg9,                                                     \
        char *arg10,                                                    \
        char *arg11,                                                    \
        char *arg12,                                                    \
        char *arg13,                                                    \
        char *arg14,                                                    \
        char *arg15,                                                    \
        char *arg16,                                                    \
        char *arg17,                                                    \
        char *arg18,                                                    \
        char *arg19

#define CALL_DO_MAIN_EXEC(rtn, pollCount, arg0)                         \
    doMainExec (rtn, pollCount,                                         \
                    arg0,  arg1,  arg2,  arg3,  arg4,                   \
                    arg5,  arg6,  arg7,  arg8,  arg9,                   \
                    arg10, arg11, arg12, arg13, arg14,                  \
                    arg15, arg16, arg17, arg18, arg19)


/* function mappings */
#ifndef ADAPTOS_VXWORKS_6X
#define sleep(secs)         taskDelay(sysClkRateGet() * (secs))
#endif /* ADAPTOS_VXWORKS_6X */
/* vxworks has a 2 argument strerror_r() */
#define strerror_r(errCode,buf,bufsize)   strerror_r(errCode,buf)
#define getpid()            adpTaskIdSelf()
#define getuid()            (int) taskTcb(taskIdSelf())
#define utimes              adp_utimes
#define mkstemps            adp_mkstemps
#define mkstemp             adp_mkstemp
#define mkdtemp             adp_mkdtemp
#define mktemp              adp_mkstemp
#define _gettemp            adp_gettemp
#ifndef HAVE_ADP_STRNCASECMP
#define strncasecmp         adp_strncasecmp
#define HAVE_ADP_STRNCASECMP
#endif /* HAVE_ADP_STRNCASECMP */
#define strcasecmp          adp_strcasecmp
#define strtok_r            adp_strtok_r
#define gettimeofday        adp_gettimeofday
#define netent              adp_netnet
#define protoent            adp_protoent
#define alarm(X)            adpAlarm(X)
#define getprotobyname(name)        adp_getprotobyname(name)
#ifndef ADAPTOS_VXWORKS_6X
#define hostent             adp_hostent
#define servent             adp_servent 
#define gethostbyname(name) adp_gethostbyname(name, NULL)
#define getservbyname(name, proto)  adp_getservbyname(name, proto)
#define getservbyport(port, proto)  adp_getservbyport(port, proto)
#define gethostbyaddr(addr, len, type)  \
             adp_gethostbyaddr(addr, len, type, (ADP_MALLOC_FUNCPTR) NULL)
#define strdup(str)  adpStrDup(str)
#endif /* !ADAPTOS_VXWORKS_6X */
#ifndef HAVE_ADP_GETADDRINFO
#define getaddrinfo(hostname, servname, hints, res) \
            adpAddrInfoGet(hostname, servname, hints, res)
#define getnameinfo(sa, salen, host, hostlen, serv, servlen, flags) \
            adpNameInfoGet(sa, salen, host, hostlen, serv, servlen, flags)
#endif /* HAVE_ADP_GETADDRINFO */
#define getprotoent adpProtoEntGet
#define setprotoent(f) adpProtoEntSet(f)
#define endprotoent adpProtoEntEnd
#define getprotobynumber(proto) adpProtoByNumberGet(proto)
#define inet_ntop(af, src, dst, size) adpInetNtop(af, src, dst, size)
#define inet_pton(af, src, dst) adpInetPton(af, src, dst)
#define glob(pattern, flags, func, pglob) \
            adpGlob(pattern, flags, func, pglob)
#define globfree(pglob)  adpGlobFree(pglob)
#define geteuid adpEUidGet
#define vsyslog(pri, fmt, ap)  adpVSysLog(pri, fmt, ap)
#define strtouq(nptr, endptr, base) adpStrToUQ(nptr, endptr, base)
#define strtoull(nptr, endptr, base) adpStrToULL(nptr, endptr, base)    
#ifndef ADAPTOS_VXWORKS_6X
#define if_nameindex  adpIfNameIndex
#define if_freenameindex(ptr) adpIfNameIndexFree(ptr)
#define getifaddrs(pif) adpIfAddrsGet(pif)
#define freeifaddrs(ifp) adpIfAddrsFree(ifp)
#endif /* !ADAPTOS_VXWORKS_6X */
#define adpGetPid()     taskIdSelf()
#define adpAbort()      taskSuspend(taskIdSelf())
#define adpReadLink(l,f,s)   

#ifndef HAVE_ADP_GETOPT
#define getopt(X,Y,Z)       BSDgetoptWrap(X,Y,Z)
#define HAVE_ADP_GETOPT
#endif

#ifndef HAVE_ADP_OPTERR
#define opterr      (getoptObjGet()->BSD_opterr)
#define HAVE_ADP_OPTERR
#endif

#ifndef HAVE_ADP_OPTIND
#define optind      (getoptObjGet()->BSD_optind)
#define HAVE_ADP_OPTIND
#endif

#ifndef HAVE_ADP_OPTOPT
#define optopt      (getoptObjGet()->BSD_optopt)
#define HAVE_ADP_OPTOPT
#endif

#ifndef HAVE_ADP_OPTRESET
#define optreset    (getoptObjGet()->BSD_optreset)
#define HAVE_ADP_OPTRESET
#endif

#ifndef HAVE_ADP_OPTARG
#define optarg      (getoptObjGet()->BSD_optarg)
#define HAVE_ADP_OPTARG
#endif

#ifndef HAVE_ADP_TIME_SPEC
#define HAVE_ADP_TIME_SPEC
typedef int ADP_TIME_SPEC;
#endif

#ifndef HAVE_ADP_TIMER_ID
#define HAVE_ADP_TIMER_ID
typedef ULONG ADP_TIMER_ID;
#endif

#define ADP_MEM_PART_ID_OS  (memSysPartId)
#ifndef ADP_MEM_TRACK
#define adpMalloc(x)        adpMemPartAlloc(NULL, x)
#define adpFree(x)          adpMemPartFree(NULL, x)
#define adpCalloc(a,b)      adpMemPartAlloc(NULL, ((a)*(b)))
#endif /* ADP_MEM_TRACK */
             
#ifdef MAXHOSTNAMELEN
#define ADP_MAXHOSTNAMELEN  MAXHOSTNAMELEN
#else
#define ADP_MAXHOSTNAMELEN  64
#endif
#define ADP_TICKS_PER_SEC   CLOCKS_PER_SEC

#define syslog              adpSysLog
#define openlog             adpOpenLog
#define closelog            adpCloseLog

#ifndef HAVE_CTYPE

#ifdef isalpha             
#undef isalpha             
#endif 

#ifdef isupper            
#undef isupper            
#endif 

#ifdef islower           
#undef islower           
#endif 

#ifdef isdigit          
#undef isdigit          
#endif 

#ifdef isxdigit        
#undef isxdigit        
#endif 

#ifdef isspace        
#undef isspace        
#endif 

#ifdef ispunct       
#undef ispunct       
#endif 

#ifdef isalnum      
#undef isalnum      
#endif 

#ifdef isprint     
#undef isprint     
#endif 

#ifdef isgraph    
#undef isgraph    
#endif 

#ifdef iscntrl   
#undef iscntrl   
#endif 

#ifdef tolower  
#undef tolower  
#endif 

#ifdef toupper 
#undef toupper 
#endif 


#define isalpha             adpIsAlpha
#define isupper             adpIsUpper
#define islower             adpIsLower
#define isdigit             adpIsDigit
#define isxdigit            adpIsXDigit
#define isspace             adpIsSpace
#define ispunct             adpIsPunct
#define isalnum             adpIsAlnum
#define isprint             adpIsPrint
#define isgraph             adpIsGraph
#define iscntrl             adpIsCntrl
#define tolower             adpToLower
#define toupper             adpToUpper


int adpIsAlpha(int);
int adpIsUpper(int);
int adpIsLower(int);
int adpIsDigit(int);
int adpIsXDigit(int);
int adpIsSpace(int);
int adpIsPunct(int);
int adpIsAlnum(int);
int adpIsPrint(int);
int adpIsGraph(int);
int adpIsCntrl(int);
int adpToLower(int);
int adpToUpper(int);
#endif 

#ifndef ADP_DEFINE_MAIN_WRAPPER
#define ADP_DEFINE_MAIN_WRAPPER(funcName) \
int funcName (char * pArgString) \
    { \
    return adpMainWrapper(#funcName, pArgString, funcName##Main); \
    }
#endif /* ADP_DEFINE_MAIN_WRAPPER */

#ifndef ADP_DEFINE_DUMMY_ROUTINE
#define ADP_DEFINE_DUMMY_ROUTINE(funcName) \
STATUS funcName (void) { return ST_OK; }
#endif /* ADP_DEFINE_DUMMY_ROUTINE */

#ifndef HAVE_ADP_DAEMON_TASK
#define HAVE_ADP_DAEMON_TASK
#endif /* HAVE_ADP_DAEMON_TASK */

typedef int (* ADP_MAIN)(int, char **);

IMPORT  int  openlog (char *ident, int option, int facility);
IMPORT  void closelog (void);
IMPORT  void syslog (int pri, char *fmt, ...);
IMPORT void adpVSysLog (int pri, const char *fmt, va_list ap);
IMPORT  clock_t clock(void);
IMPORT  STATUS  doMainExec (FUNCPTR mainFunc, UINT pollCount,
                            char *arg0, MAIN_EXEC_PARAMS_DEF);
IMPORT  ADP_VOIDFUNCPTR adpSignal(int sig, void (*sigHandler)(int));
IMPORT  unsigned int    adpAlarm (UINT secs);
IMPORT struct protoent * adpProtoByNumberGet (int proto);
IMPORT void adpProtoEntEnd();
IMPORT void warnx(const char *fmt, ...);
IMPORT int adpEUidGet(void);
IMPORT u_quad_t adpStrToUQ (const char *nptr, char **endptr, int base);
IMPORT void adpProtoEntSet (int);
#ifdef ADAPTOS_VXWORKS_6X
IMPORT int adpSnPrintf(char *str,size_t count,const char *fmt,...);
IMPORT int vsnprintf(char *str, size_t count, const char *fmt, va_list args);
#endif /* ADAPTOS_VXWORKS_6X */
#ifndef HAVE_ADP_GETADDRINFO
struct addrinfo;
int adpNameInfoGet (const struct sockaddr *, size_t, char *, size_t,
                     char *, size_t, int);
int adpAddrInfoGet (const char *, const char *,const struct addrinfo *,
                    struct addrinfo **);
#endif /* HAVE_ADP_GETADDRINFO */

IMPORT int adpMainWrapper (char * pArgv0, char * pArgString, ADP_MAIN realMain);

IMPORT int adpInetPton (int af, const char * cp, void * buf);
IMPORT const char * adpInetNtop (int af, const void * cp, 
                                 char * buf, size_t len);
IMPORT char * adpStrDup (const char * str);
#ifndef HAVE_ADP_GETPWNAM
#define getpwnam(name)  adpPasswdGetPwNam(name)
#endif /* HAVE_ADP_GETPWNAM */
#ifndef HAVE_ADP_CRYPT
#define crypt(key,salt) adpPasswdCrypt(key, salt)
#endif /* HAVE_ADP_CRYPT */

#ifdef __cplusplus
}
#endif
#endif /*__INCadpVxMiscH */
