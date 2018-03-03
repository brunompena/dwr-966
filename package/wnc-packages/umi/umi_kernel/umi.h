/* umi.h - definitions for the umi library */

/* Copyright (c) 2009 - 2010, TeamF1, Inc. */

/*
modification history
--------------------
02g,19apr11,bng  added UMI_COMP_QOS
02f,03aug10,pnm  added UMI_COMP_HTTP_SERVER
02e,01sep09,sam  added UMI_COMP_SNMP_TRAP
02d,19may09,sam  added UMI_COMP_KLED
02c,20feb09,sam  added UMI_COMP_UMCAST_FILT, UMI_COMP_KMCAST_FILT.
02b,23oct08,rnp  changed umiIdToName() and umiNameToId() from inline to macro,
                 to avoid error on vx 6.3 when this header is included from
                 ieee80211 code, where inline is not available.
02a,19dec07,rnp  added UMI_COMP_IKE_USR
01z,30nov09,sen  added UMI_COMP_WIFIDOG
01y,21nov09,drk  added UMI_COMP_TSUMI
01x,08jul09,kyp  added UMI_COMP_UNBOUND
01w,21jan09,sdk  added UMI_COMP_EAPAUTH, UMI_COMP_WSC_WLAN, UMI_COMP_WSC_UI, 
                       UMI_COMP_WSC_EAP.
01v,17nov08,pnm  added nas components.
01u,11oct08,dev  added UMI_COMP_TR069
01t,18sep08,chd  added UMI_COMP_FIREWALL_SCHED
01s,16aug08,mk   added UMI_COMP_VSIGN2FCTR
01r,14aug08,nrj  added UMI_COMP_QUAGGA, UMI_COMP_RIPNG.
01q,26jun08,vms  added UMI_COMP_PORTMGMT.
01p,26jun08,vpp  added UMI_COMP_BWLIMIT,UMI_COMP_RIP.
01o,14mar08,anp  added UMI_COMP_NIMF.
01n,23feb08,jay  added UMI_COMP_FIREWALL and UMI_COMP_IDS.
01m,08feb08,jkg  added UMI_COMP_DPROXY.
01l,08feb08,ss   added UMI_COMP_PPTP.
01k,03feb08,jkg  added UMI_COMP_UPNP and UMI_COMP_PPPOE.
01j,24Jan08,arf  added UMI_COMP_KNATPT.
01i,19oct07,sdk  added UMI_COMP_KIFDEV.
01h,11sep07,sdk  added UMI_CMD_IFDEV_EVENT.
01g,28jun07,smv  added UMI_CMD_DB_UPDATE and UMI_DBUPDATE_INFO.
01f,10nov05,sam  added bridge, 8021D/W/S.
01e,12aug05,sam  added UMI_COMP_VIPSECK.
01d,05may05,kb   added UMI_CMD_FIND_COMP macro.
01c,27apr05,kb   added UMI_COMP_VIPSEC.
01b,25mar05,uge  added UMI_COMP_RAD.
01a,11feb05,rks  written.
*/

/*
DESCRIPTION
This file has definitions for implementation of the umi library.

INCLUDE FILES:
*/

#ifndef __INCumih
#define __INCumih

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
/* includes */

    
/* defines */

#define UMI_COMP_NAME(x)    { x, #x}

/* umiRegister options */
#define UMI_OPT_ANY_TASK_CTX        0x1
#define UMI_OPT_MY_TASK_CTX         0x2
#define UMI_OPT_RESERVED            0xf0000000

#define UMI_IOCTL_REQUEST           1
#define UMI_IOCTL_REPLY             0

/* umiIoctl options */
#define UMI_IOCTL_OPT_SYNC_CALL     0
#define UMI_IOCTL_OPT_ASYNC_CALL    1
#define UMI_IOCTL_OPT_NO_RESULT     2
        
/* 
 * UMI uses local buffer (on stack) if the ioctl arguments after encoding
 * doesnt exceed UMI_MAX_REQ_SIZE else it allocates the memory dynamically.
 */
#define UMI_MAX_REQ_SIZE            1024   

/* errno codes */
#define UMI_REQ_TOO_BIG             (M_umiLib | 1)
#define UMI_RECV_TIMEOUT            (M_umiLib | 2)
#define UMI_LOCK_FAILED             (M_umiLib | 2)


/* ioctls supported by UMI component */
#define UMI_CMD_LIST_COMP           1   /* return registerd component's ids in 
                                           the output buffer */
#define UMI_CMD_FIND_COMP           2   /* checks whether the given coponent is 
                                           registered or not */

#define UMI_PRIVATE_IOCTL_START UMI_CMD_ADP_GEN_END

/* typedefs */

    
/* generic ioctls handled by UMI on behalf of each component */
typedef enum
    {
    UMI_CMD_ADP_DEBUG = 0,   /* adpDebug ioctls */
    UMI_CMD_STATS = 1,       /* for getting stats */                     
    UMI_CMD_KUMI_DEINIT = 2, /* UMI kernel module unload notification */
    UMI_CMD_CLI,             /* This command provides command line interface to
                               execute any global function/api of the component.
                              */
    UMI_CMD_DB_UPDATE,       /* global command to execute DB update handler */
    UMI_CMD_IFDEV_EVENT,     /* global command to indicate an ifDev event */
    UMI_CMD_ADP_GEN_END = 32 /* If this value is incremented (i.e. if total no
                                of commands becomes >32 then all the components
                                which use umiIoctl or umiRegister need to be 
                                recompiled */ 
    }UMI_GENERIC_CMDS;

typedef enum 
    {
    UMI_COMP_NONE       = 0,
    UMI_COMP_UMI        = 1,
    UMI_COMP_KERNEL     = 2,
    UMI_COMP_UDOT11     = 3,
    UMI_COMP_KDOT11     = 4,
    UMI_COMP_PNAC       = 5,
    UMI_COMP_PNAC_USR   = 6,
    UMI_COMP_RAD        = 7,
    UMI_COMP_EAP_RAD    = 8,
    UMI_COMP_VIPSEC     = 9,
    UMI_COMP_MIBCTL     = 10,
    UMI_COMP_VIPSECK    = 11,
    UMI_COMP_BRIDGE     = 12,
    UMI_COMP_8021D_USR  = 13,
    UMI_COMP_8021W_USR  = 14,
    UMI_COMP_8021S_USR  = 15,
    UMI_COMP_UADSL      = 16,
    UMI_COMP_EVTDSPTCH  = 17,
    UMI_COMP_PLATFORM   = 18,
    UMI_COMP_REBOOT     = 19,
    UMI_COMP_LOGGING    = 20,
    UMI_COMP_CRON       = 21,
    UMI_COMP_KIFDEV     = 22,
    UMI_COMP_NTP        = 23,
    UMI_COMP_VLAN       = 24,
    UMI_COMP_SNMP       = 25,
    UMI_COMP_IFDEV      = 26,
    UMI_COMP_FIRMWARE   = 27,
    UMI_COMP_SFFS       = 28,
    UMI_COMP_TIME       = 29,
    UMI_COMP_USERDB     = 30,
    UMI_COMP_LOGIN      = 31,
    UMI_COMP_SMTPC      = 32,
    UMI_COMP_IAPP       = 33,
    UMI_COMP_WEB        = 34,
    UMI_COMP_SSHD       = 35,
    UMI_COMP_X509       = 36,
    UMI_COMP_HTTPS      = 37,
    UMI_COMP_KNATPT     = 38,
    UMI_COMP_UPNP       = 39,
    UMI_COMP_PPPOE      = 40,
    UMI_COMP_PPTP       = 41,
    UMI_COMP_DPROXY     = 42,
    UMI_COMP_FIREWALL   = 43,
    UMI_COMP_IDS        = 44,
    UMI_COMP_DDNS       = 45,
    UMI_COMP_IGMP       = 46,
    UMI_COMP_L2TP       = 47,
    UMI_COMP_NIMF       = 48,
    UMI_COMP_DHCPC      = 49,
    UMI_COMP_WLAN       = 50,
    UMI_COMP_LANGROUPS  = 51,
    UMI_COMP_WPS        = 52,
    UMI_COMP_VIPSECURE  = 53,
    UMI_COMP_IPS        = 54,
    UMI_COMP_URLFILTER  = 55,
    UMI_COMP_BWLIMIT    = 56,
    UMI_COMP_RIP        = 57,
    UMI_COMP_SSLVPN     = 58,
    UMI_COMP_TM_PROTECTLINK  = 59,
    UMI_COMP_PORTMGMT   = 60,
    UMI_COMP_QUAGGA     = 61,
    UMI_COMP_RIPNG      = 62,
    UMI_COMP_VSIGN2FCTR = 63, 
    UMI_COMP_DHCP_HELPER = 64,
    UMI_COMP_FIREWALL_SCHED = 65,
    UMI_COMP_TR069 = 66,
    UMI_COMP_SMB	= 67,
    UMI_COMP_DISK_MGMT	= 68,
    UMI_COMP_PARTITIONMGMT	= 69,
    UMI_COMP_NAS	= 70,
    UMI_COMP_EAPAUTH     = 71,
    UMI_COMP_WSC_WLAN    = 72,
    UMI_COMP_WSC_UI      = 73,
    UMI_COMP_WSC_EAP     = 74,
    UMI_COMP_EMAILPROXY  = 75,
    UMI_COMP_IMPROXY  = 76,
    UMI_COMP_BONJOUR     = 77,
    UMI_COMP_CPUMEM_USAGE = 78,
    UMI_COMP_UNBOUND      = 79,
    UMI_COMP_CSL          = 80,
    UMI_COMP_TSUMI        = 81,
    UMI_COMP_WIFIDOG      = 82,
    UMI_COMP_IKE_USR    = 83,
    UMI_COMP_UMCAST_FILT = 84,
    UMI_COMP_KMCAST_FILT = 85,
    UMI_COMP_UPSA = 86,
	UMI_COMP_KLED = 87,
    UMI_COMP_VPORT = 88,
    UMI_COMP_WATCHDOG = 89,
    UMI_COMP_SYSTIME = 90,
    UMI_COMP_SNMP_TRAP = 91,
    UMI_COMP_DIMARK_TR69 = 92,
    UMI_COMP_DMC = 93,
	UMI_COMP_NSCAND = 94,
    UMI_COMP_DUM = 95,
    UMI_COMP_PRINTSERVER = 96,
    UMI_COMP_HTTP_SERVER = 97,
	UMI_COMP_OSGI = 98,
	UMI_COMP_QOS = 99,
	UMI_COMP_ATHEROS_SWITCH_CONFIG = 100,
	UMI_COMP_VOIP_WDOG = 101,
    UMI_COMP_ID_MAX     = 1000  /* ids after this is reserved for UMI instances
                                   which is subjected to implementation */
    } UMI_COMP_ID;

typedef struct
    {
    UMI_COMP_ID id;
    char *      name;
    }UMI_ID_NAME_MAP;

#define UMI_ID_NAME_MAP_LIST                                                   \
    UMI_COMP_NAME(UMI_COMP_UMI),                                               \
    UMI_COMP_NAME(UMI_COMP_KERNEL),                                            \
    UMI_COMP_NAME(UMI_COMP_UDOT11),                                            \
    UMI_COMP_NAME(UMI_COMP_KDOT11),                                            \
    UMI_COMP_NAME(UMI_COMP_PNAC),                                              \
    UMI_COMP_NAME(UMI_COMP_PNAC_USR),                                          \
    UMI_COMP_NAME(UMI_COMP_RAD),                                               \
    UMI_COMP_NAME(UMI_COMP_EAP_RAD),                                           \
    UMI_COMP_NAME(UMI_COMP_VIPSEC),                                            \
    UMI_COMP_NAME(UMI_COMP_MIBCTL),                                            \
    UMI_COMP_NAME(UMI_COMP_VIPSECK),                                           \
    UMI_COMP_NAME(UMI_COMP_BRIDGE),                                            \
    UMI_COMP_NAME(UMI_COMP_8021D_USR),                                         \
    UMI_COMP_NAME(UMI_COMP_8021W_USR),                                         \
    UMI_COMP_NAME(UMI_COMP_8021S_USR),                                         \
    UMI_COMP_NAME(UMI_COMP_UADSL),                                             \
    UMI_COMP_NAME(UMI_COMP_EVTDSPTCH),                                         \
    UMI_COMP_NAME(UMI_COMP_PLATFORM),                                          \
    UMI_COMP_NAME(UMI_COMP_REBOOT),                                            \
    UMI_COMP_NAME(UMI_COMP_LOGGING),                                           \
    UMI_COMP_NAME(UMI_COMP_CRON),                                              \
    UMI_COMP_NAME(UMI_COMP_KIFDEV),                                            \
    UMI_COMP_NAME(UMI_COMP_NTP),                                               \
    UMI_COMP_NAME(UMI_COMP_VLAN),                                              \
    UMI_COMP_NAME(UMI_COMP_SNMP),                                              \
    UMI_COMP_NAME(UMI_COMP_IFDEV),                                             \
    UMI_COMP_NAME(UMI_COMP_FIRMWARE),                                          \
    UMI_COMP_NAME(UMI_COMP_SFFS),                                              \
    UMI_COMP_NAME(UMI_COMP_TIME),                                              \
    UMI_COMP_NAME(UMI_COMP_USERDB),                                            \
    UMI_COMP_NAME(UMI_COMP_LOGIN),                                             \
    UMI_COMP_NAME(UMI_COMP_SMTPC),                                             \
    UMI_COMP_NAME(UMI_COMP_IAPP),                                              \
    UMI_COMP_NAME(UMI_COMP_WEB),                                               \
    UMI_COMP_NAME(UMI_COMP_SSHD),                                              \
    UMI_COMP_NAME(UMI_COMP_X509),                                              \
    UMI_COMP_NAME(UMI_COMP_HTTPS),                                             \
    UMI_COMP_NAME(UMI_COMP_KNATPT),                                            \
    UMI_COMP_NAME(UMI_COMP_UPNP),                                              \
    UMI_COMP_NAME(UMI_COMP_PPPOE),                                             \
    UMI_COMP_NAME(UMI_COMP_PPTP),                                              \
    UMI_COMP_NAME(UMI_COMP_DPROXY),                                            \
    UMI_COMP_NAME(UMI_COMP_FIREWALL),                                          \
    UMI_COMP_NAME(UMI_COMP_IDS),                                               \
    UMI_COMP_NAME(UMI_COMP_DDNS),                                              \
    UMI_COMP_NAME(UMI_COMP_L2TP),                                              \
    UMI_COMP_NAME(UMI_COMP_NIMF),                                              \
    UMI_COMP_NAME(UMI_COMP_DHCPC),                                             \
    UMI_COMP_NAME(UMI_COMP_WLAN),                                              \
    UMI_COMP_NAME(UMI_COMP_LANGROUPS),                                         \
    UMI_COMP_NAME(UMI_COMP_WPS),                                               \
    UMI_COMP_NAME(UMI_COMP_VIPSECURE),                                         \
    UMI_COMP_NAME(UMI_COMP_IPS),                                               \
    UMI_COMP_NAME(UMI_COMP_URLFILTER),                                         \
    UMI_COMP_NAME(UMI_COMP_BWLIMIT),                                           \
    UMI_COMP_NAME(UMI_COMP_RIP),                                               \
    UMI_COMP_NAME(UMI_COMP_SSLVPN),                                            \
    UMI_COMP_NAME(UMI_COMP_TM_PROTECTLINK),                                    \
    UMI_COMP_NAME(UMI_COMP_PORTMGMT),                                          \
    UMI_COMP_NAME(UMI_COMP_VSIGN2FCTR),                                        \
    UMI_COMP_NAME(UMI_COMP_DHCP_HELPER),                                       \
    UMI_COMP_NAME(UMI_COMP_FIREWALL_SCHED),                                    \
    UMI_COMP_NAME(UMI_COMP_TR069),                                             \
    UMI_COMP_NAME(UMI_COMP_SMB),                                               \
    UMI_COMP_NAME(UMI_COMP_DISK_MGMT),                                         \
    UMI_COMP_NAME(UMI_COMP_PARTITIONMGMT),                                     \
    UMI_COMP_NAME(UMI_COMP_NAS),                                               \
    UMI_COMP_NAME(UMI_COMP_EAPAUTH),                                           \
    UMI_COMP_NAME(UMI_COMP_WSC_WLAN),                                          \
    UMI_COMP_NAME(UMI_COMP_WSC_UI),                                            \
    UMI_COMP_NAME(UMI_COMP_WSC_EAP),                                           \
    UMI_COMP_NAME(UMI_COMP_EMAILPROXY),                                        \
    UMI_COMP_NAME(UMI_COMP_IMPROXY),                                           \
    UMI_COMP_NAME(UMI_COMP_BONJOUR),                                           \
    UMI_COMP_NAME(UMI_COMP_CPUMEM_USAGE),                                      \
    UMI_COMP_NAME(UMI_COMP_UNBOUND),                                           \
    UMI_COMP_NAME(UMI_COMP_CSL),                                               \
    UMI_COMP_NAME(UMI_COMP_TSUMI),                                             \
    UMI_COMP_NAME(UMI_COMP_IKE_USR),                                           \
    UMI_COMP_NAME(UMI_COMP_UMCAST_FILT),                                       \
    UMI_COMP_NAME(UMI_COMP_KMCAST_FILT),                                       \
    UMI_COMP_NAME(UMI_COMP_UPSA),                                              \
    UMI_COMP_NAME(UMI_COMP_KLED),                                              \
    UMI_COMP_NAME(UMI_COMP_VPORT),                                             \
    UMI_COMP_NAME(UMI_COMP_WATCHDOG),                                          \
    UMI_COMP_NAME(UMI_COMP_SYSTIME),                                           \
    UMI_COMP_NAME(UMI_COMP_SNMP_TRAP),                                         \
    UMI_COMP_NAME(UMI_COMP_DIMARK_TR69),                                       \
    UMI_COMP_NAME(UMI_COMP_DMC),                                               \
    UMI_COMP_NAME(UMI_COMP_NSCAND),                                            \
    UMI_COMP_NAME(UMI_COMP_DUM),                                               \
    UMI_COMP_NAME(UMI_COMP_PRINTSERVER),                                       \
    UMI_COMP_NAME(UMI_COMP_HTTP_SERVER),                                       \
    UMI_COMP_NAME(UMI_COMP_OSGI),                                              \
    UMI_COMP_NAME(UMI_COMP_QOS),                                               \
    UMI_COMP_NAME(UMI_COMP_VOIP_WDOG),                                         \
    UMI_COMP_NAME(UMI_COMP_NONE)

typedef struct
    {
    unsigned int        cmd;    /* ioctl command */
    UMI_COMP_ID         srcId;  /* src component id */
    unsigned int        reqOpt; /* ioctl request options */
    unsigned int        reqId;  /* requested id (used for ASYNC call) */
    int                 req;    /* 1 to indicate request, 0 for reply */
    int                 status; /* to store the result of ioctl handler */
    } UMI_REQ_INFO;


typedef int (* UMI_IOCTL_HANDLER) (void * arg1, UMI_REQ_INFO * pUmiReq,
                                   int cmd, void * pInBuf,
                                   unsigned int inLen, void * pOutBuf,
                                   unsigned int * pOutLen) ;
/* imports */

extern UMI_ID_NAME_MAP  umiIdNameMap[];

/* forward declarations */

int     umiInit (void);

void *  umiRegister (UMI_COMP_ID myId, int umiOpt, UMI_IOCTL_HANDLER handler,
                     void * arg1);

int     umiIoctl (void * pUmiCtx, UMI_COMP_ID destId, int cmd, 
                  void * pInBuf, unsigned int inLen,
                  void * pOutBuf, unsigned int * pOutLen, 
                  int umiOpt, int timeout, int reqId);

void    umiListen (void *pUmiCtx, int timeout);
void    umiUnRegister (void * pUmiCtx);
void    umiDeInit (void);

#define umiNameToId(pName) \
    ({ \
     int i = 0; \
     int id = UMI_COMP_NONE; \
     while (umiIdNameMap[i].id != UMI_COMP_NONE) \
        { \
        if ( strcmp (umiIdNameMap[i].name, pName) == 0) \
            id = umiIdNameMap[i].id; \
        i++; \
        } \
    id; \
    })

#define umiIdToName(ID) \
    ({ \
    int     i = 0; \
    while (umiIdNameMap[i].id != UMI_COMP_NONE) \
        { \
        if (umiIdNameMap[i].id == ID) \
            break; \
        i++; \
        } \
    umiIdNameMap[i].name; \
    })

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* __INCumih */
