/* adpModDebug.h - header file for module debug library */

/* Copyright (c) 2009, TeamF1, Inc. */

/*
modification history
--------------------
01o,26jan10,sam  moved all logging macros under TEAMF1_MGMT to loggingLib.h
01n,29apr09,chd  added ADP_EVENT_LEVEL_MSG for specific Events Logging.
01m,08mar08,sdk  cast adpModuleDebug __FUNC__ parameter to char * to take care
                 of warnings.
01l,06jan09,rks  merged ADP_DEV_DEBUG change
01k,12aug08,rnp  changed ADP_PRINTF to use INFO level
                 added adpLog() and adpLogHex() which dont need moduleId and can
                 be used without TEAMF1_MGMT
                 added ADP_DEBUG_LEVEL_DEBUG2
                 added ADP_DEBUG_LEVEL_ALL
01j,20jun08,rnp  removed DCC-specific definition for __FUNC__
01i,19oct07,rnp  added ADP_LOG_XXX macros
                 removed ADP_DEBUG_LEVEL_ON/OFF macros
                 added adpLogTypeSet/Get routines
01h,06oct07,san  introduced HAVE_ADP_NO_VAR_NAME_NO_VAR_ARG,
                 HAVE_ADP_NO_VAR_NAME_VAR_ARG macro.          
01g,26jul07,rnp  added ADP_DEBUG_LEVEL_WARN to ADP_DEBUG_LEVEL_DEFAULT
                 changed args of adpModuleDebug and ADP_XXX print macros
                 to compile on vx 5.5 gnu mips
01f,18jul07,rnp  added ADP_EAPAUTH_ID from xcal2.0
                 replaced format,... with ... in the ADP_XXX print macros for
                 diab to compile on vx 6.3(changes from xcal2.0)
01e,09jan06,sam	 added entry for firewall 
01d,10nov05,sam  added entry for stp   
01c,10oct05,sam  used adpPanic in ADP_CRIT
01b,07may05,sam  added modules for ASAP.
01a,17mar01,mvk  created.
*/


#ifndef __INCadpModDebugh
#define __INCadpModDebugh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* defines */

#define ADP_DEBUG_LEVEL_EMERGENCY   0x01
#define ADP_DEBUG_LEVEL_ALERT       0x02
#define ADP_DEBUG_LEVEL_CRIT        0x04
#define ADP_DEBUG_LEVEL_ERROR       0x08
#define ADP_DEBUG_LEVEL_WARN        0x10
#define ADP_DEBUG_LEVEL_NOTICE      0x20
#define ADP_DEBUG_LEVEL_INFO        0x40 
#define ADP_DEBUG_LEVEL_DEBUG       0x80
#define ADP_DEBUG_LEVEL_DEBUG2      0x100
#define ADP_EVENT_LEVEL_MSG         0x200


#define ADP_LOG_NONE                    0x0
#define ADP_LOG_UMILOG                  0x1
#define ADP_LOG_SYSLOG                  0x2
#define ADP_LOG_STDOUT                  0x4
#define ADP_LOG_AUTO                    -1

/* only ADP_DEBUG_LEVEL_ERROR and ADP_DEBUG_LEVEL_WARN are enabled by default */
#define ADP_DEBUG_LEVEL_DEFAULT         \
        (ADP_DEBUG_LEVEL_ERROR | ADP_DEBUG_LEVEL_WARN)

#define ADP_DEBUG_LEVEL_ALL    0xffff

#ifndef DEBUGGING_MODE
#define ADP_DEV_DEBUG(x,...)	
#define ADP_DEV_DEBUG_HEX(x,...)
#else   /* DEBUGGING MODE */
#define ADP_DEV_DEBUG       ADP_DEBUG2
#define ADP_DEV_DEBUG_HEX   ADP_DEBUG2_HEX
#endif /* !DEBUGGING MODE */

#define adpModuleDebugOn(moduleId)
#define adpModuleDebugOff(moduleId)

#ifdef __WNC_OG36XX__

void adp_wnc_print(const char * func, int line, char*, ...); 

#define ADP_WNC_DBG(...) \
		adp_wnc_print( __func__, __LINE__, __VA_ARGS__)
#endif
#ifndef TEAMF1_MGMT
#if defined(HAVE_ADP_NO_VAR_NAME_NO_VAR_ARG)
/* worst case: variable args are not supported in macros at all */
#define ADP_PRINTF printf
#elif defined(HAVE_ADP_NO_VAR_NAME_VAR_ARG)
/* diab case: variable args are supported in macros, but they are unnamed */
#define ADP_MOD_DEBUG(id,level,...) \
        adpLog(level, __func__, __LINE__, __VA_ARGS__)

#define ADP_PRINTF(...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_DEBUG2,__VA_ARGS__)

#define ADP_INFO(...) \
        ADP_MOD_INFO(moduleId,ADP_DEBUG_LEVEL_INFO,__VA_ARGS__)

#define ADP_DEBUG2(...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_DEBUG2,__VA_ARGS__)

#define ADP_DEBUG(...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_DEBUG,__VA_ARGS__)

#define ADP_WARN(...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_WARN,__VA_ARGS__)

#define ADP_ERROR(...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_ERROR,__VA_ARGS__)

#define ADP_CRIT(...) \
    do { \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_CRIT,__VA_ARGS__); \
        adpPanic(); \
    } while(0)
#else /* HAVE_ADP_VAR_NAME_VAR_ARG */

#define ADP_MOD_DEBUG(id,level,args...) \
        adpLog(level, __func__, __LINE__, args)

#define ADP_PRINTF(args...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_DEBUG2, args)

#define ADP_EVENT(format,args...) \
        ADP_MOD_DEBUG(moduleId,ADP_EVENT_LEVEL_MSG, args)

#define ADP_INFO(args...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_INFO,args)

#define ADP_DEBUG2(args...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_DEBUG2,args)

#define ADP_DEBUG(args...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_DEBUG,args)

#define ADP_WARN(args...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_WARN,args)

#define ADP_ERROR(args...) \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_ERROR,args)

#define ADP_CRIT(args...) \
    do { \
        ADP_MOD_DEBUG(moduleId,ADP_DEBUG_LEVEL_CRIT,args); \
        adpPanic(); \
    } while(0)
#endif 

#define ADP_MOD_ID(x)
#define ADP_DEBUG_HEX(text,str, len) \
        adpLogHex(ADP_DEBUG_LEVEL_DEBUG,text,str, len)
#define ADP_DEBUG2_HEX(text, str,len) \
        adpLogHex(ADP_DEBUG_LEVEL_DEBUG2,text,str, len)

#define adpModuleDebugLevelOn(moduleId, level) adpLogLevelSet(level)
#define adpModuleDebugLevelOff(moduleId, level) adpLogLevelSet(0)
#else /* !TEAMF1_MGMT */
#define adpModuleDebugLevelOn(moduleId, level) 
#define adpModuleDebugLevelOff(moduleId, level) 
#endif  /* TEAMF1_MGMT */

void adpLogTypeSet(int);
int  adpLogTypeGet(void);
void adpLog(int level, const char * func, int line, char*, ...);
void adpLogHex(int level, char *, void *, int); 
int adpLogLevelSet(int level); 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCadpModDebugh */
