/* adpGetProtoByName.c - get the protocol name */

/* Copyright (c) 2003, TeamF1, Inc. */

/*
modification history
--------------------
01c,18aug03,ksn  added prototype of adp_strcasecmp.
01b,01may02,mno  ported to adaptos framework.
01a,18nov00,mno  created.
*/

#include "adaptos.h"

extern int adp_strcasecmp(const char *s1, const char *s2);

static struct adp_protoent protocols[] = {
    {"ip",      (char **) NULL,   0},
    {"icmp",        (char **) NULL,   1},
    {"igmp",        (char **) NULL,   2},
    {"ggp",     (char **) NULL,   3},
    {"ipencap",     (char **) NULL,   4},
    {"st",      (char **) NULL,   5},
    {"tcp",     (char **) NULL,   6},
    {"egp",     (char **) NULL,   8},
    {"pup",     (char **) NULL,   12},
    {"udp",     (char **) NULL,   17},
    {"hmp",     (char **) NULL,   20},
    {"xns-idp",     (char **) NULL,   22},
    {"rdp",     (char **) NULL,   27},
    {"iso-tp4",     (char **) NULL,   29},
    {"xtp",     (char **) NULL,   36},
    {"ddp",     (char **) NULL,   37},
    {"idpr-cmtp",   (char **) NULL,   39},
    {"ipv6",        (char **) NULL,   41},
    {"ipv6-route",  (char **) NULL,   43},
    {"ipv6-frag",   (char **) NULL,   44},
    {"ipv6-crypt",  (char **) NULL,   50},
    {"ipv6-auth",   (char **) NULL,   51},
    {"ipv6-icmp",   (char **) NULL,   58},
    {"ipv6-nonxt",  (char **) NULL,   59},
    {"ipv6-opts",   (char **) NULL,   60},
    {"rspf",        (char **) NULL,   73},
    {"vmtp",        (char **) NULL,   81},
    {"ospf",        (char **) NULL,   89},
    {"ipip",        (char **) NULL,   94},
    {"encap",       (char **) NULL,   98},

    { (char *) NULL,    (char **) NULL,   0}
}; 

/*******************************************************************************
*
* getprotobyname - returns protocol information
*
* This routine searches its internal protocols database, and returns a protent
* struct for the specified protocol. 
*
* RETURNS: a valid protent pointer if a match is found and NULL otherwise.
*/

struct adp_protoent* adp_getprotobyname
    (
    char *pName                              /* protocol name */
    )
    {
    struct adp_protoent *pProtent;
    extern int      strcasecmp();

    for (pProtent=protocols; pProtent->p_name != NULL; pProtent++)
        {
        if (! strcasecmp (pProtent->p_name, pName))
            return (pProtent);
        }

    return (struct adp_protoent *) NULL;
    }

