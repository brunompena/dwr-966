/* adpListLib.h -  adaptos linked list library header */

/* Copyright (c) 2005 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01f,17aug07,rnp  added ADP_SLIST(derived from bsd queue.h).
01e,07may05,sam  restructured adaptos.
01d,17jun03,gnm  code cleanups
01c,19apr03,ksn  modified error macros.
01b,10may02,mno  cleanup.
01a,08may02,vnk  created.
*/

/*
 * Copyright (c) 1991, 1993
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
 *  @(#)queue.h 8.5 (Berkeley) 8/20/94
 */

#ifndef __INCadpListLibH
#define __INCadpListLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */
#include "adaptos.h"

/* error codes */

#define ADP_LL_OK           (0)
#define ADP_LL_ERROR        (-1)
#define S_adpListLib_NO_MEM (M_adpListLib | 0x1)

/* typedefs */

#ifndef HAVE_ADP_NODE
typedef struct adp_node     /* node of a linked list. */
    {
    struct adp_node *next;  /* points at the next node in the list */
    struct adp_node *prev;  /* points at the previous node in the list */
    } ADP_NODE;
#define HAVE_ADP_NODE
#endif

#ifndef HAVE_ADP_LIST_ITER_FUNC_TYPE
typedef BOOL (*ADP_LIST_ITER_FUNC)(void *pArg, ADP_NODE *pNode);
#define HAVE_ADP_LIST_ITER_FUNC_TYPE
#endif

#ifndef HAVE_ADP_LIST
typedef struct          /* linked list. */
    {
    ADP_NODE *head;     /* header of list */
    ADP_NODE *tail;     /* tail of list */
    } ADP_LIST;
#define HAVE_ADP_LIST
#endif

#ifndef ADP_LIST_NODE_FIRST
#define ADP_LIST_NODE_FIRST(pList)  \
    (                               \
    (((ADP_LIST *)(pList))->head)   \
    )
#endif

#ifndef ADP_LIST_NODE_LAST
#define ADP_LIST_NODE_LAST(pList)   \
    (                               \
    (((ADP_LIST *)(pList))->tail)   \
    )
#endif

#ifndef ADP_LIST_NODE_NEXT
#define ADP_LIST_NODE_NEXT(pNode)   \
    (                               \
    (((ADP_NODE *)(pNode))->next)   \
    )
#endif

#ifndef ADP_LIST_NODE_PREVIOUS
#define ADP_LIST_NODE_PREVIOUS(pNode)   \
    (                                   \
    (((ADP_NODE *)(pNode))->prev)       \
    )
#endif

#ifndef ADP_LIST_IS_EMPTY
#define ADP_LIST_IS_EMPTY(pList)        \
    (                                   \
    (((ADP_LIST *)pList)->head == NULL) \
    )
#endif

#define adpListNodeAdd  adpListNodeAppend

/*
 * Singly-linked List declarations.
 */
#define ADP_SLIST_HEAD(name, type)              \
struct name {                                   \
    struct type *slh_first; /* first element */ \
}

#define ADP_SLIST_HEAD_INITIALIZER(head)        \
    { NULL }

#define ADP_SLIST_ENTRY(type)                   \
struct {                                        \
    struct type *sle_next;  /* next element */  \
}
 
/*
 * Singly-linked List functions.
 */
#define ADP_SLIST_EMPTY(head)   ((head)->slh_first == NULL)

#define ADP_SLIST_FIRST(head)   ((head)->slh_first)

#define ADP_SLIST_FOREACH(var, head, field)                                   \
    for ((var) = ADP_SLIST_FIRST((head));                                     \
        (var);                                                                \
        (var) = ADP_SLIST_NEXT((var), field))

#define ADP_SLIST_FOREACH_PREVPTR(var, varp, head, field)                     \
    for ((varp) = &ADP_SLIST_FIRST((head));                                   \
        ((var) = *(varp)) != NULL;                                            \
        (varp) = &ADP_SLIST_NEXT((var), field))

#define ADP_SLIST_INIT(head) do {                                             \
    ADP_SLIST_FIRST((head)) = NULL;                                           \
} while (0)

#define ADP_SLIST_INSERT_AFTER(slistelm, elm, field) do {                     \
    ADP_SLIST_NEXT((elm), field) = ADP_SLIST_NEXT((slistelm), field);         \
    ADP_SLIST_NEXT((slistelm), field) = (elm);                                \
} while (0)

#define ADP_SLIST_INSERT_HEAD(head, elm, field) do {                          \
    ADP_SLIST_NEXT((elm), field) = ADP_SLIST_FIRST((head));                   \
    ADP_SLIST_FIRST((head)) = (elm);                                          \
} while (0)

#define ADP_SLIST_NEXT(elm, field)  ((elm)->field.sle_next)

#define ADP_SLIST_REMOVE(head, elm, type, field) do {                         \
    if (ADP_SLIST_FIRST((head)) == (elm)) {                                   \
        ADP_SLIST_REMOVE_HEAD((head), field);                                 \
    }                                                                         \
    else {                                                                    \
        struct type *curelm = ADP_SLIST_FIRST((head));                        \
        while (ADP_SLIST_NEXT(curelm, field) != (elm))                        \
            curelm = ADP_SLIST_NEXT(curelm, field);                           \
        ADP_SLIST_NEXT(curelm, field) =                                       \
            ADP_SLIST_NEXT(ADP_SLIST_NEXT(curelm, field), field);             \
    }                                                                         \
} while (0)

#define ADP_SLIST_REMOVE_HEAD(head, field) do {                               \
    ADP_SLIST_FIRST((head)) = ADP_SLIST_NEXT(ADP_SLIST_FIRST((head)), field); \
} while (0)

/* prototypes */

IMPORT STATUS   adpListInit (ADP_LIST *pList);
IMPORT int      adpListNodeCount (ADP_LIST *pList);
IMPORT STATUS   adpListNodeInsert (ADP_LIST *pList, ADP_NODE *pInsertAfter,
                                   ADP_NODE *pNode);
IMPORT STATUS   adpListNodeRemove (ADP_LIST *pList, ADP_NODE *pNode);
IMPORT STATUS   adpListNodeAppend (ADP_LIST *pList, ADP_NODE *pNode);
IMPORT STATUS   adpListNodePrepend (ADP_LIST *pList, ADP_NODE *pNode);
IMPORT STATUS   adpListConcat (ADP_LIST *pDestList, ADP_LIST *pSrcList);
IMPORT STATUS   adpListIter (ADP_LIST *pList, ADP_LIST_ITER_FUNC iterFunc,
                             void *pArg);
IMPORT ADP_NODE *  adpListFirst (ADP_LIST *pList);
IMPORT ADP_NODE *  adpListNext (ADP_NODE *pNode);

#ifdef __cplusplus
}
#endif

#endif /* __INCadpMutexLibh */
