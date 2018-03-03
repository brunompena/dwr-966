/* adpLnxBase.h - include files for Linux OS */

/* Copyright (c) 2006, TeamF1, Inc. */

/*
modification history
--------------------
01e,19mar10,rks  including linux/tcp.h from here sometimes causes compilation
                 issues when compiling 3rdparty software. I believe
                 netinet/tcp.h is more portable.
01d,19aug06,chd  added adpShmLib.h
01c,19oct05,rnp  added err.h
01b,02aug05,k_p  added includes <sys/ipc.h> and <sys/msg.h>   
01a,07may05,sam  moved all Linux defn. from adaptos.h.
*/

#ifndef __INCadpLnxBaseH
#define __INCadpLnxBaseH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <err.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <stdarg.h>
#include <netdb.h>
#include <semaphore.h>
#include <pthread.h> 
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <shadow.h>
#include <signal.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/ipc.h>
#include <sys/msg.h>      
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <glob.h>
#include <sys/wait.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/in_systm.h>
#include <netinet/ether.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "adpLnxTypes.h"
#include "adpLnxMisc.h"
#include "adpShmLib.h"    
#ifdef __cplusplus
}
#endif

#endif /* __INCadpLnxBaseH */
