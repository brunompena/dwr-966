/* adpNetdb.h - interface header  */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01c,09sep05,rnp  removed vxworks references.
01b,20jun02,mno  ported to adaptos framework.
01a,11jan02,ksn  ported to vxWorks.
*/

/*-
 * Copyright (c) 1980, 1983, 1988, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

/*
 *      @(#)netdb.h 8.1 (Berkeley) 6/2/93
 *      From: Id: netdb.h,v 8.9 1996/11/19 08:39:29 vixie Exp $
 * $FreeBSD: src/include/netdb.h,v 1.22 2001/10/04 10:34:30 bde Exp $
 */

#ifndef __INCadpNetdbh
#define __INCadpNetdbh


#ifndef HAVE_ADP_H_ERRNO
extern int h_errno;
#define HAVE_ADP_H_ERRNO
#endif

/*
 * Structures returned by network data base library.  All addresses are
 * supplied in host order, and returned in network order (suitable for
 * use in system calls).
 */
#ifndef HAVE_ADP_HOSTENT
struct adp_hostent {
    char    * h_name;           /* official name of host */
    char    **h_aliases;        /* alias list */
    int       h_addrtype;       /* host address type */
    int       h_length;         /* length of address */
    char    **h_addr_list;      /* list of addresses from name server */
#define h_addr  h_addr_list[0]  /* address, for backward compatibility */
    unsigned int h_ttl;         /* Time to Live in Seconds */
};
#define HAVE_ADP_HOSTENT
#endif

/*
 * Assumption here is that a network number
 * fits in an unsigned long -- probably a poor one.
 */
#ifndef HAVE_ADP_NETENT
struct adp_netent {
    char         *n_name;       /* official name of net */
    char        **n_aliases;    /* alias list */
    int           n_addrtype;   /* net address type */
    unsigned long n_net;        /* network # */
};
#define HAVE_ADP_NETENT
#endif

#ifndef HAVE_ADP_SERVENT
struct adp_servent {
    char     *s_name;       /* official service name */
    char    **s_aliases;    /* alias list */
    int       s_port;       /* port # */
    char     *s_proto;      /* protocol to use */
};
#define HAVE_ADP_SERVENT
#endif

#ifndef HAVE_ADP_PROTOENT
struct adp_protoent {
    char     *p_name;       /* official protocol name */
    char    **p_aliases;    /* alias list */
    int       p_proto;      /* protocol # */
};
#define HAVE_ADP_PROTOENT
#endif

/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

#define NETDB_INTERNAL  -1  /* see errno */
#define NETDB_SUCCESS   0   /* no problem */
#define HOST_NOT_FOUND  1 /* Authoritative Answer Host not found */
#define TRY_AGAIN   2 /* Non-Authoritative Host not found, or SERVERFAIL */
#define NO_RECOVERY 3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define NO_DATA     4 /* Valid name, no data record of requested type */
#define NO_ADDRESS  NO_DATA     /* no address, look for MX record */

#endif /* __INCadpNetdbh */
