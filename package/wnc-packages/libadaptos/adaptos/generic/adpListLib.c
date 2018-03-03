/* adpListLib.c - adaptos generic linked list library */

/* Copyright (c) 2004, TeamF1, Inc. */

/*
modification history
--------------------
01d,27jul04,rks  added adpListNext
01c,17jun03,gnm  code cleanups: added function descriptions for all the
                 functions, modified module description.
01b,04jun02,mno  cleanups; added adpListIter, adpListConcat
01a,25may02,vnk  created.
*/

/*
DESCRIPTION

This library provides generic linked list support. The current implementation
provides a doubly-linked list. The nodes are embedded into the data-structures
and can reside only on one list. The double-link list does not allocate nodes
or lists and expect these to be allocated from the caller.

INTERNAL:

The list generic list support will be converted into a template to derive
multiple implementations of link-list libraries to work with embedded link
nodes and free-standing link nodes.

INCLUDES: adaptos.h

SEE ALSO: 

*/

/* includes */

#include "adaptos.h"


/*******************************************************************************
*
* adpListInit - initialize the linked-list support
*
* This routine initializes the linked-list support
*
* RETURNS: ST_OK on success or ERROR otherwise
*/

STATUS  adpListInit
    (
    ADP_LIST *  pList   /* pointer to the list */
    )
    {
    if (pList == NULL)
        return (ADP_LL_ERROR);

    pList->head = NULL;
    pList->tail = NULL;

    return (ADP_LL_OK);
    }

/*******************************************************************************
*
* adpListIter - iterate through a list based on a user defined routine
*
* This routine iterates through the list on the basis of a user defined
* routine. The user-defined routine can be passed via the argument <iterFunc>.
*
* RETURNS: ST_OK success or ERROR otherwise. 
*/

STATUS  adpListIter
    (
    ADP_LIST *      pList,      /* pointer to the list */
    ADP_LIST_ITER_FUNC  iterFunc,   /* iteration function */
    void *      pArg        /* argument to the iteration function */
    )
    {
    ADP_NODE *pNode;
    
    if (pList == NULL)
        return (ADP_LL_ERROR);

    pNode = pList->head;
    while ((pNode) &&
           iterFunc(pArg, pNode))
        {
        pNode = ADP_LIST_NODE_NEXT(pNode);
        }
    
    return (ADP_LL_OK);
    }

/*******************************************************************************
*
* adpListFirst - returns the first node in the list
*
* This routine returns the first node in the list.
*
* RETURNS: pointer to first node or NULL
*/
ADP_NODE * adpListFirst
    (
    ADP_LIST *  pList   /* pointer to the list */
    )
    {
    if (pList == NULL)
        return (NULL);

    return (pList->head);
    }

/*******************************************************************************
*
* adpListNext - returns the next node in the list
*
* This routine returns the next node from current node (passed as an argument).
* If NULL is passed then first node is returned.
*
* RETURNS: pointer to next node or NULL
*/

ADP_NODE *  adpListNext
    (
    ADP_NODE *      pNode   /* pointer to current node */
    )
    {
    if (pNode == NULL)
        return (NULL);

    return (ADP_LIST_NODE_NEXT(pNode));
    }

/*******************************************************************************
*
* adpListNodeCount - count the number of nodes in a list
*
* This routine counts the number of nodes in a linked list.
*
* RETURNS: Number of nodes or ERROR otherwise.
*/ 

int adpListNodeCount
    (
    ADP_LIST *  pList       /* pointer to the list */
    )
    {
    ADP_NODE *  pNode;
    int     ix = 0;
    
    if (pList == NULL)
        return (ADP_LL_ERROR);

    for (pNode = ADP_LIST_NODE_FIRST(pList);
         (pNode != NULL);
         pNode = ADP_LIST_NODE_NEXT(pNode))
        {
        ix++;
        }

    return (ix);
    }

/*******************************************************************************
*
* adpListNodeInsert - insert a node in a list
*
* This routine can be used to insert a node in a linked list. The node pointed
* to by the argument <pNode> is appended after the node <pInsertAfter>. If the
* argument <pInsertAfter> is 0 then <pNode> is added before the current head
* node of the list and defines the new head of the list. If the argument
* <pInsertAfter> is -1 then <pNode> is added after the current tail node of the
* list and defines the new tail node of the list.
*
* RETURNS: ST_OK on success or ERROR otherwise. 
*/

STATUS adpListNodeInsert
    (
    ADP_LIST *  pList,      /* pointer to the list */
    ADP_NODE *  pInsertAfter,   /* node to insert after */
    ADP_NODE *  pNode       /* pointer to the node */
    )
    {

    if (pList == NULL)
        return (ADP_LL_ERROR);

    if ((UINT)pInsertAfter == 0)
        return adpListNodePrepend(pList, pNode);

    if ((UINT)pInsertAfter == -1)
        return adpListNodeAppend(pList, pNode);

    pNode->next = pInsertAfter->next;
    if (pNode->next)
        pNode->next->prev = pNode;
    else
        pList->tail = pNode;

    pInsertAfter->next = pNode;
    pNode->prev = pInsertAfter;

    return (ADP_LL_OK);
    }

/*******************************************************************************
*
* adpListNodeRemove - remove a node from a linked list
*
* This routine removes a node from a linked list.
*
* RETURNS: ST_OK on success or ERROR otherwise.
*/

STATUS adpListNodeRemove
    (
    ADP_LIST *  pList,      /* pointer to the list */
    ADP_NODE *  pNode       /* pointer to the node */
    )
    {

    if ((pList == NULL) ||
        (pList->head == NULL))
        return (ADP_LL_ERROR);

    if (pList->head == pNode)
        pList->head = pNode->next;

    if (pList->tail == pNode)
        pList->tail = pNode->prev;

    if (pNode->next)
        pNode->next->prev = pNode->prev;

    if (pNode->prev)
        pNode->prev->next = pNode->next;

    return (ADP_LL_OK);
    }

/*******************************************************************************
*
* adpListNodeAppend - insert a node to the end of a linked list
*
* This routine appends a node a tail of the linked list. This node is now the
* new tail node of the linked list.
*
* RETURNS: ST_OK on success or ERROR otherwise.
*/

STATUS adpListNodeAppend
    (
    ADP_LIST *  pList,      /* pointer to the list */
    ADP_NODE *  pNode       /* pointer to the node */
    )
    {
    if (pList == NULL)
        return (ADP_LL_ERROR);

    pNode->prev = pList->tail;
    pNode->next = NULL;

    if (pList->tail)
        pList->tail->next = pNode;

    pList->tail = pNode;

    if (pList->head == NULL)
        pList->head = pList->tail;

    return (ADP_LL_OK);
    }

/*******************************************************************************
*
* adpListNodePrepend - insert a node to the head of a linked list
*
* This routine appends a node a head of the linked list. This node is now the
* new head node of the linked list.
*
* RETURNS: ST_OK on sucess or ERROR otherwise.
*/

STATUS adpListNodePrepend
    (
    ADP_LIST *  pList,      /* pointer to the list */
    ADP_NODE *  pNode       /* pointer to the node */
    )
    {
    if (pList == NULL)
        return (ADP_LL_ERROR);

    pNode->prev = NULL;
    pNode->next = pList->head;

    if (pList->head)
        pList->head->prev = pNode;

    pList->head = pNode;

    if (pList->tail == NULL)
        pList->tail = pList->head;

    return (ADP_LL_OK);
    }

/*******************************************************************************
*
* adpListConcat - concatenate two linked-lists
*
* This routine two linked lists as arguments to this routine and appends the
* list pointed to by the second argument, <pSrcList>, to the list pointed to by
* the first argument, <pDestList>.
*
* RETURNS: ST_OK on success or ERROR otherwise.
*/

STATUS adpListConcat
    (
    ADP_LIST *pDestList,         /* list to append to */
    ADP_LIST *pSrcList           /* list to append */
    )
    {
    ADP_NODE *pTail;
    
    if ((pDestList == NULL) ||
        (pSrcList == NULL))
        return (ADP_LL_ERROR);

    if (pDestList->head == NULL)
        {
        pDestList->head = pSrcList->head;
        pDestList->tail = pSrcList->tail;
        return (ADP_LL_OK);
        }

    pTail = pDestList->tail;
    
    pTail->next = pSrcList->head;
    pTail->next->prev = pTail;
    pDestList->tail = pSrcList->tail;

    return (ADP_LL_OK);
    }


