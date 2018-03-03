/* adpPsosBase.h - include files for PSOS OS */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,07may05,sam  moved all PSOS defn. from adaptos.h.
*/

#ifndef __INCadpPsosBaseH
#define __INCadpPsosBaseH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdlib.h"
#include "stdio.h"
#include <string.h>
#include "errno.h"
#include "ctype.h"
#include "sys/stat.h"
#include <sys/times.h>
#include <sys/types.h>
#include "psos.h"
#include "pna.h"
#include <sys/param.h>

#ifndef ADAPTOS_PSOS_2X
#include <netinet/in.h>
#include <netinet/tcp.h>
#ifndef (ADAPTOS_PSOS_2X_ADV) 
#include <arpa/inet.h>
#endif /* ADAPTOS_PSOS_2X_ADV */
#endif /* ADAPTOS_PSOS_2X */

#ifndef ADAPTOS_PSOS_2X
#ifndef ADAPTOS_PSOS_2X_ADV
#include "strings.h"
#endif /* ADAPTOS_PSOS_2X_ADV */
#include "sys/socket.h"
#endif /* ADAPTOS_PSOS_2X */
#ifdef ADAPTOS_PSOS_3X
#include "rescfg.h"
#endif /* ADAPTOS_PSOS_3X */

#ifdef ADAPTOS_PSOS_2X
#include "time.h"
#endif /* ADAPTOS_PSOS_2X */

#ifdef ADAPTOS_PSOS_2X 
#include <phile.h>
#endif

#ifdef ADAPTOS_PSOS_3X
#include "sys/socket.h"
#endif /* ADAPTOS_PSOS_3X */

#ifdef ADAPTOS_PSOS_2X
#include "stddef.h"
#include "configs.h"
#undef  BREAK
#define O_CREAT     0x0200
#define O_TRUNC     0x0800
#define O_APPEND    0x0400
#endif /* ADAPTOS_PSOS_2X */

typedef struct ADP_DIR
    {
    XDIR        _xdir;
    struct dirent   _dirent;
    } ADP_DIR;

#define DIR     ADP_DIR

typedef struct timeval
    {
    long  tv_sec;      
    long  tv_usec;     
    }ADP_TIMEVAL;

#define min(X,Y)    ((X)<(Y)?(X):(Y))
#define max(X,Y)    ((X)>(Y)?(X):(Y))

#include "adpPsosMisc.h"

#ifdef __cplusplus
}
#endif

#endif /* __INCadapPsosBaseH */
