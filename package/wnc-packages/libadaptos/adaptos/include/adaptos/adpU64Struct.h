/* adpU64Struct.h - header file to perform 64-bit ALU ops using 32-bit words */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01b,29dec07,rnp  removed adp_qdivrem prototype
01a,12jul07,rnp  ported from FreeBSD 4.5
*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 * $FreeBSD: src/sys/libkern/adddi3.c,v 1.5 1999/08/28 00:46:30 peter Exp $
 */

#ifndef __INCadpU64Structh
#define __INCadpU64Structh
#include "adaptos.h"

#ifndef HAVE_ADP_UINT64_STRUCT
typedef struct 
    {
#if BYTE_ORDER == BIG_ENDIAN
    UINT adpu64H; /* higher 32 bits - MSB first */
    UINT adpu64L; /* lower 32 bits */
#elif BYTE_ORDER == LITTLE_ENDIAN
    UINT adpu64L; /* lower 32 bits - LSB first*/
    UINT adpu64H; /* higher 32 bits */
#endif
    } UINT64_STRUCT;
#define HAVE_ADP_UINT64_STRUCT
#endif /* HAVE_ADP_UINT64_STRUCT */

typedef unsigned int qshift_t;

UINT64_STRUCT adp_castitoq(int hi, int lo);
int adp_castqtoi(UINT64_STRUCT a);
UINT64_STRUCT adp_adddi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_anddi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_ashldi3(UINT64_STRUCT a, qshift_t shift);
UINT64_STRUCT adp_ashrdi3(UINT64_STRUCT a, qshift_t shift);
int adp_cmpdi2(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_divdi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_iordi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_lshldi3(UINT64_STRUCT a, qshift_t shift);
UINT64_STRUCT adp_lshrdi3(UINT64_STRUCT a, qshift_t shift);
UINT64_STRUCT adp_moddi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_muldi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_negdi2(UINT64_STRUCT a);
UINT64_STRUCT adp_one_cmpldi2(UINT64_STRUCT a);
UINT64_STRUCT adp_subdi3(UINT64_STRUCT a, UINT64_STRUCT b);
int adp_ucmpdi2(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_udivdi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_umoddi3(UINT64_STRUCT a, UINT64_STRUCT b);
UINT64_STRUCT adp_xordi3(UINT64_STRUCT a, UINT64_STRUCT b);

#endif /* __INCadpU64Structh */
