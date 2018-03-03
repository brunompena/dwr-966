/* adpNetLib.c -- generic network library */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01b,28jun08,rnp  re-wrote adp_gethostbyaddr and adp_gethostbyname to address
                 mismatch between hostent and adp_hostent
                 added adpInstallHostsFile()
01a,22apr08,san  written (based on vxWorks/adpNetLib.c). 
*/

/*
DESCRIPTION

This module implements a generic network library as part of the
adaptos framework.

*/

/* includes */

#include "adaptos.h"

/* defines */
#undef gethostbyname
#undef gethostbyaddr
#undef hostent

#define BUF_ALIGN(pbuf,  align)     \
    while (((UINT)pbuf % align) != 0)   \
       {  \
       ++pbuf; \
       }

#define SOCKPAIR_SVC_IP          "127.0.0.1"
#define SOCKPAIR_SVC_BUF_SIZE        1024

/* globals */

static struct adp_servent services[] = {
    { "www",        (char **) NULL,   80, "tcp" },
    { "www",        (char **) NULL,   80, "udp" },
    { "http",       (char **) NULL,   80, "tcp" },
    { "http",       (char **) NULL,   80, "udp" },
    { "telnet",     (char **) NULL,   23, "tcp" },
    { "socks",      (char **) NULL, 1080, "tcp" },
    { "socks",      (char **) NULL, 1080, "udp" },
    { "https",      (char **) NULL,  443, "tcp" },
    { "https",      (char **) NULL,  443, "udp" },
    { "ssl",        (char **) NULL,  443, "tcp" },
    { "ssl",        (char **) NULL,  443, "udp" },
    { "ftp",        (char **) NULL,   21, "tcp" },
    { "gopher",     (char **) NULL,   70, "tcp" },
    { "gopher",     (char **) NULL,   70, "udp" },
    { (char *) NULL,    (char **) NULL,    0, (char *) NULL}
};

/* forward declarations */
#ifdef CONFIG_ADP_HOSTS_FILE
extern const unsigned int hosts_data_size;
extern const char hosts_data[];
STATUS adpInstallFile (char * pDst, const char * pBuffer, int bufSize);
#endif /* CONFIG_ADP_HOSTS_FILE */

#ifdef CONFIG_ADP_HOSTS_FILE
/*******************************************************************************
*
* adpGetIpForHost - returns host IP address for a specified host name.
*
* This routine is extracts host IP address information for a specified host name
* from default host configuration file.
*
* RETURNS: 
*/

static int adpGetIpForHost
    (
    char  * name,
    ULONG * ip
    )
    {
    FILE *fp;
    char  buff[128];
    char *ptr, *ipPtr;

    fp = fopen(ADP_DEFAULT_HOSTS_FILE, "r");
    if (fp == NULL)
        return -1;

    while(!feof(fp))
        {
        if (fgets(buff, 128, fp) == NULL)
            continue;
    
        ptr = strchr(buff, ' ');
        if (ptr == NULL)
            continue;
    
        *ptr = '\0';
        ptr++;
        ipPtr = ptr;
        ptr = strchr(ptr,'\n');
        if (ptr) *ptr = '\0';
        /* found a host with the give name */
        if (!strcmp(buff, name))
            {
            *ip = inet_addr(ipPtr);
            fclose(fp);
            return 0;
            }
        }
    /* no host with the given name found */
    fclose(fp);
    return -1;
    }

/*******************************************************************************
*
* adpGetNameForHostIP - returns host name for a specified IP address.
*
* This routine is extracts host name for a specified IP address
* from default host configuration file.
*
* RETURNS: 
*/

static int adpGetNameForHostIP
    (
    char  * name,
    ULONG * ip
    )
    {
    FILE *fp;
    char  buff[128];
    char *ptr, *ipPtr;

    fp = fopen(ADP_DEFAULT_HOSTS_FILE, "r");
    if (fp == NULL)
        return -1;

    while(!feof(fp))
        {
        if (fgets(buff, 128, fp) == NULL)
            continue;

        ptr = strchr(buff, ' ');
        if (ptr == NULL)
            continue;
    
        *ptr = '\0';
        ptr++;
        ipPtr = ptr;
        ptr = strchr(ptr,'\n');
        if (ptr) *ptr = '\0';
        /* found a host with the give name */
        if (*ip == inet_addr(ipPtr))
            {
            strcpy(name, buff);
            fclose(fp);
            return 0;
            }
        }
    /* no host with the given name found */
    fclose(fp);
    return -1;
    }
#endif /* CONFIG_ADP_HOSTS_FILE */

/*******************************************************************************
*
* adp_getservbyname - returns service port information
*
* This routine searches its internal serivices database, and returns a servent
* struct for the specified service name, and protocol. The protocol is
* specified either as "tcp" or "udp".
*
* Tasks must take a mutex semaphore before calling this routine, and must hold
* the semaphore until the return pointer is processed by the task. 
*
* RETURNS: A valid servent pointer if a match is found or NULL otherwise.
*/

struct adp_servent * adp_getservbyname
    (
    const char *name,              /* service name */
    const char *proto                    /* protocol type */
    )
    {
    struct adp_servent  *pServent;

    for (pServent = services; pServent->s_name != NULL; pServent++)
        {
        if (! strcmp (pServent->s_name, name) &&
            ! strcmp (pServent->s_proto, proto))
            return (pServent);
        }

    return (struct adp_servent *) (NULL);
    }

/*******************************************************************************
*
* adp_getservbyport - returns service port information
*
* This routine searches its internal serivices database, and returns a servent
* struct for the specified service port, and protocol. The protocol is
* specified either as "tcp" or "udp".
*
* Tasks must take a mutex semaphore before calling this routine, and must hold
* the semaphore until the return pointer is processed by the task. 
*
* RETURNS: A valid servent pointer if a match is found or NULL otherwise.
*/

struct adp_servent * adp_getservbyport
    (
    int port,              /* service port */
    char *proto            /* protocol type */
    )
    {
    struct adp_servent  *pServent;

    for (pServent = services; pServent->s_name != NULL; pServent++)
        {
        if ((pServent->s_port == port) &&
            ! strcmp (pServent->s_proto, proto))
            return (pServent);
        }

    return (struct adp_servent *) (NULL);
    }

/*******************************************************************************
*
* adp_gethostbyname - returns host information for a specified hostname
*
* This routine is wrapper around resolvGetHostByName() which creates a
* hostent stucture for a the host specified by `name'. A local static
* buffer is returned after each successful call. Callers must use a mutex
* to make sure that only a single task is processing the hostentBuf.
*
* RETURNS: A valid hostent pointer on success or NULL otherwise.
*/

struct adp_hostent * adp_gethostbyname
    (
    const char *             name,
    ADP_MALLOC_FUNCPTR _mallocRtn
    )
    {
    static char          hostentBuf[256];
    struct adp_hostent * pHostEnt;
    char *               pBuf;
    int                  host;
    struct hostent * h = NULL;

    if (_mallocRtn == NULL)
        pBuf = hostentBuf;
    else
        pBuf = (char *) (*_mallocRtn)(256);

    BUF_ALIGN (pBuf, 4);

    pHostEnt = (struct adp_hostent *) pBuf;

#ifdef CONFIG_ADP_HOSTS_FILE
    if ((host = inet_addr ((char *)name)) != ST_ERROR ||
        adpGetIpForHost((char *)name ,(ULONG *)&host) == 0)
        {
        pBuf += sizeof (struct adp_hostent);

        BUF_ALIGN (pBuf, 4);

        pHostEnt->h_addr_list = (char **) pBuf;

        pBuf += 3 * sizeof (char**);
        BUF_ALIGN (pBuf, 4);

        *(int *) pBuf = host;
        pHostEnt->h_addr_list [0] = pBuf;
        pHostEnt->h_addr_list [1] = NULL;
        pBuf += sizeof (int);

        BUF_ALIGN (pBuf, 4);

        pHostEnt->h_aliases = (char **) pBuf;
        pBuf += 2 * sizeof (char**);

        pHostEnt->h_aliases [0] = NULL;

        BUF_ALIGN (pBuf, 4);

        strcpy (pBuf, name);
        pHostEnt->h_name = pBuf;

        pHostEnt->h_length = sizeof (int);
        pHostEnt->h_addrtype = AF_INET;
        } 
    else
#endif /* CONFIG_ADP_HOSTS_FILE */    
        {
        h = gethostbyname(name);

        if (h)
            {
            /* the adp_hostent and hostent dont match, so we copy the members */
            pHostEnt->h_addr_list = h->h_addr_list;
            pHostEnt->h_aliases = h->h_aliases;
            pHostEnt->h_name = h->h_name;
            pHostEnt->h_length = h->h_length;
            pHostEnt->h_addrtype = h->h_addrtype;
            }
        else
            pHostEnt = NULL;
        }

    return (pHostEnt);
    }


/*******************************************************************************
*
* adp_gethostbyaddr - returns host information for a specified address
*
* This routine is wrapper around resolvGetHostByAddr() which creates a
* hostent stucture for a the host specified by `addr'. A local static
* buffer is returned after each successful call. Callers must use a mutex
* to make sure that only a single task is processing the hostentBuf.
*
* RETURNS: A valid hostent pointer on success or NULL otherwise.
*/
struct adp_hostent* adp_gethostbyaddr
    (
    const char *       addr,
    int                len,
    int                type,
    ADP_MALLOC_FUNCPTR _mallocRtn
    )
    {
    static char          hostentBuf[256];
    struct adp_hostent * pHostEnt;
    char *               pBuf;
    char                 name[128];   
    struct hostent * h = NULL;

    if (_mallocRtn == NULL)
        {
        pBuf = hostentBuf;
        }
    else
        {
        pBuf = (char *) _mallocRtn (256);
        }
    
    BUF_ALIGN (pBuf, 4);

    pHostEnt = (struct adp_hostent *) pBuf;
#ifdef CONFIG_ADP_HOSTS_FILE
    if (adpGetNameForHostIP(name ,(ULONG *) addr) == 0)
        {
        pBuf += sizeof (struct adp_hostent);

        BUF_ALIGN (pBuf,  4);

        strcpy (pBuf, name);
        pHostEnt->h_name = pBuf;

        pBuf += strlen (name) + 1;
        BUF_ALIGN (pBuf, 4);

        pHostEnt->h_addr_list = (char **) pBuf;
        pBuf += 3 * sizeof (char**);

        BUF_ALIGN (pBuf, 4);

        *(int*)pBuf = *(int*)addr;
        pHostEnt->h_addr_list [0] = pBuf;
        pHostEnt->h_addr_list [1] = NULL;
        pBuf += sizeof (int);

        BUF_ALIGN (pBuf, 4);

        pHostEnt->h_aliases = (char **) pBuf;
        pBuf += 2 * sizeof (char**);

        pHostEnt->h_aliases [0] = NULL;

        pHostEnt->h_length = sizeof (int);    
        pHostEnt->h_addrtype = AF_INET;
        }
    else
#endif /* CONFIG_ADP_HOSTS_FILE */    
        {
        h = gethostbyaddr(addr, len, type);

        if (h)
            {
            /* the adp_hostent and hostent dont match, so we copy the members */
            pHostEnt->h_addr_list = h->h_addr_list;
            pHostEnt->h_aliases = h->h_aliases;
            pHostEnt->h_name = h->h_name;
            pHostEnt->h_length = h->h_length;
            pHostEnt->h_addrtype = h->h_addrtype;
            }
        }

    return (pHostEnt);
    }

/*******************************************************************************
*
* adpInstallHostsFile - installs "hosts" file
*
* This routine installs the /teamf1/hosts file from hosts_data[]. This routine
* should be called when setting up the target file-system.
*
* RETURNS: ST_OK/ST_ERROR
*/
STATUS adpInstallHostsFile (void)
    {
    STATUS ret = ST_OK;
#ifdef CONFIG_ADP_HOSTS_FILE
    ret = adpInstallFile("/teamf1/hosts", hosts_data, hosts_data_size);
#endif /* CONFIG_ADP_HOSTS_FILE */
    return ret;
    }
