/* adpTxMisc.h - misc defines for ThreadX */

/* Copyright (c) 2006 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01c,27mar07,san  modified USNET specific socket api mapping
01b,17may06,pkk  adopted from LynxOs and modified for threadX 
01a,31aug05,rnp  written (based on linux/adpXXX.*)
*/

#ifndef __INCadpTxMiscH
#define __INCadpTxMiscH

#ifdef __cplusplus
#   define __BEGIN_DECLS extern "C" {
#   define __END_DECLS  }
#else
#   define __BEGIN_DECLS
#   define __END_DECLS
#endif 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* file IO flags */
#define O_RDONLY      0x00000000
#define O_WRONLY      0x00000001
#define O_RDWR	      0x00000002 
#define O_ACCMODE     0x00000003
#define O_CREAT	      0x00000004
#define O_EXCL	      0x00000008
#define O_TRUNC	      0x00000010
#define O_APPEND      0x00000020

#define	__S_IFDIR     0x00000001
#define	__S_IFREG     0x00000002
#define	__S_IFCHR     0x00000004
#define __S_IFSOCK    0x00000008

#define SIOCGIFCONF   0x00000001
#define SIOCGIFFLAGS  0x00000002

#define S_ISDIR(mode) ((mode)&__S_IFDIR)

/* TASK LIB macros specific to THREADX */

#define THX_AUTOSTART_NUM_BITS      1
/* preemt threshold for threadx will be between 0 - 31*/
#define THX_PREEMT_NUM_BITS         5   

#define THX_AUTOSTART_MASK          THX_AUTOSTART_NUM_BITS
#define THX_PREEMT_MASK             ((1<<(THX_PREEMT_NUM_BITS)) - 1) 

#define THX_AUTOSTART_SHIFT         0
#define THX_PREEMT_SHIFT            THX_AUTOSTART_NUM_BITS
#define THX_TIMESLICE_SHIFT         THX_AUTOSTART_NUM_BITS + \
	                            THX_PREEMT_NUM_BITS

#define OPTIONS_GET_AUTOSTART(x)    ((x)&THX_AUTOSTART_MASK)
#define OPTIONS_GET_PREEMT_THRES(x) ((x) & ((THX_PREEMT_MASK)<<\
			            (THX_PREEMT_SHIFT))) >> (THX_PREEMT_SHIFT)
#define OPTIONS_GET_TIMESLICE(x)    ((x)>>(THX_TIMESLICE_SHIFT)) 

#define OPTIONS_PACK(as,pt,ts)      (((as) & THX_AUTOSTART_MASK) | \
	                            (((pt) & (THX_PREEMT_MASK)) << \
				     (THX_PREEMT_SHIFT)) |\
				    ((ts) << (THX_TIMESLICE_SHIFT)))

#define exit(x)             tx_thread_terminate(tx_thread_identify())


#define mktemp(X)           mkstemp(X)
#define mkstemp(X)          adp_mkstemp(X)
#define ADP_MEM_PART_ID_OS  (0)
#define ADP_TICKS_PER_SEC   adpTicksPerSec()
#undef  inet_aton
#define inet_aton(X,Y)      adp_inet_aton(X,Y)
#define strncasecmp         adp_strncasecmp
#define strcasecmp          adp_strcasecmp
#define gettimeofday        adp_gettimeofday
#define hostent             adp_hostent
#define gethostbyname(name) \
    adp_gethostbyname((name), (ADP_MALLOC_FUNCPTR) NULL)

#define gethostbyaddr(addr, len, type)  \
             adp_gethostbyaddr(addr, len, type, (ADP_MALLOC_FUNCPTR) NULL)

#ifndef min   
#define min(X,Y)        ((X)<(Y)?(X):(Y))
#endif 
#ifndef max   
#define max(X,Y)        ((X)>(Y)?(X):(Y))
#endif 


/* define the socket api mapping for ThreadX */
#if defined(CONFIG_USNET)
#define select                           selectsocket
#else
#define select                           bsd_select
#endif /* CONFIG_USNET */    
#define ioctl                            adpSocketIoctl
#define bind                             adpSocketBind
#define listen                           adpSocketListen
#define accept                           adpSocketAccept
#define socket(fam,type,proto)           adpSocketCreate((fam),(type),(proto))
#define connect(soc,addr,len)            adpSocketConnect((soc),(addr),(len))
#define getsockname(soc,addr,len)        \
                adpSocketGetSockName((soc),(addr),(len))
#define getpeername(soc,addr,len)        \
                adpSocketGetPeerName((soc),(addr),(len))
#define sendto(soc,buf,len,flgs,addr,tolen)    \
                adpSocketSendTo((soc),(buf),(len),(flgs),(addr),(tolen))
#define recvfrom(soc,buf,len,flgs,addr,flen)   \
                adpSocketRecvFrom((soc),(buf),(len),(flgs),(addr),(flen))
#define setsockopt(soc,level,name,arg,len)     \
                adpSocketSetSockOpt((soc),(level),(name),(arg),(len))
#define send(soc,buf,len,flgs)            \
                adpSocketSend((soc),(buf),(len),(flgs))
#define shutdown(soc, how)                 adpSocketShutDown((soc), (how))
#define getsockopt(soc,level,name,arg,arglen)  \
                adpSocketGetSockOpt((soc),(level),(name),(arg),(arglen))
#define recv(soc,buf,len,flgs)            \
                adpSocketRecv((soc),(buf),(len),(flgs))


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

#define adpGetPid()     getpid()
#define adpAbort()      abort()

#ifndef ADP_DEFINE_MAIN_WRAPPER
#define ADP_DEFINE_MAIN_WRAPPER(function)
#endif /* ADP_DEFINE_MAIN_WRAPPER */

#ifndef ADP_DEFINE_DUMMY_ROUTINE
#define ADP_DEFINE_DUMMY_ROUTINE(funcName) \
STATUS funcName (void) { return ST_OK; }
#endif /* ADP_DEFINE_DUMMY_ROUTINE */

#define servent                     adp_servent 
#define getservbyname(name, proto)  adp_getservbyname(name, proto)
#define getservbyport(port, proto)  adp_getservbyport(port, proto)
#define gethostname(name,size)      adp_gethostname((name), (size))


/* prototypes which are not available */
int      creat(const char * , int , int );
int      open (const char * , int , int );
STATUS   close(int);
int      read (int , char * , size_t );
int      write(int , const char * , unsigned long);
STATUS   stat (const char * , struct stat * ); 
STATUS   fstat(int , struct stat * );
off_t    lseek(int, off_t, int);
STATUS   unlink(const char *);
STATUS   chdir(char * );
STATUS   mkdir(char * , UINT);
STATUS   rmdir(char * );
STATUS   truncate (const char * , off_t );
STATUS   ftruncate (int , off_t );

#if defined (CONFIG_USNET) 

/* ilo specific */
#define CONFIG_ARCH_V850E_ILO

/* need to investigate */
#define cticks _tx_timer_system_clock 	/* timer related change */

/*    */
#define perror(x) printf("%s : errno::%d",x,errno)

/* Resolving Compilation error */
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

/* define if you don't have DNS support */
#define HAVE_NOT_DNS_SUPPORT

/* when socket & file descriptor are not handle in similar way */
#define HAVE_NEED_ADP_HANDLER_FOR_SOCKET_AND_FILE_DESC 

/* added for resolving compilation error */
#define HAVE_ADP_NO_VAR_NAME_NO_VAR_ARG

/* add for resolving compilation error */
#define HAVE_NOT_SUPPORT__ATTRIBUTE__

/* added for resolving compilation error */
#define __FUNCTION__ __FILE__

/* need to investigate this */
#define ENOTSUP 10

#endif /* CONFIG_USNET */

#ifdef CONFIG_ARCH_V850E_ILO
#define time(X) rtc_get()
#endif 

#ifndef HAVE_ADP_STRUCT_DIRENT
#define HAVE_ADP_STRUCT_DIRENT
/* Directory entry */
#define NAME_MAX 50
struct dirent		/* dirent */
    {
    char	d_name [NAME_MAX + 1];	/* file name, null-terminated */
    int     d_type; 
    };
#endif /* HAVE_ADP_STRUCT_DIRENT */

#ifdef __cplusplus
}
#endif

#endif /* __INCadpTxMiscH */
