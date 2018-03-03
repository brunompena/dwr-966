/* adpTxUnMap.h - unmap list for ThreadX */

/* Copyright (c) 2006 - 2007, TeamF1, Inc. */

/*
modification history
--------------------

01b,16oct07,san  added CONFIG_USNET for fixing
                 compilation error.    
01a,17may06,pkk  written (based on LynxOs/adpXXX.*)
*/

#ifndef __INCadpTxUnMapH
#define __INCadpTxUnMapH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define adpMalloc(x)                        malloc(x)
#define adpCalloc(a,b)                      calloc(a,b)
#define adpFree(x)                          free(x)
#define adpMemPartCreate(a,b,c)             ((ADP_PART_ID)1)
#define adpMemPartDestroy(a)                ((ADP_PART_ID)1)
#define adpMemPartDelete(a)                 ((ADP_PART_ID)1)
#define adpMemPartAlloc(a,b)                malloc(b)
#define adpMemPartFree(a,b)                 (free(b),0)
#define adpMemPartRealloc(a,b,c)            realloc(b,c)
#define adpMemPartAllocWithTimeout(a,b,c,d) malloc (b)   

#if defined(CONFIG_USNET)
/* 
 * USNET defines IP in ussw/INCLUDE/NET.H which create compilation error 
 * in afsstring2key.c.
 * so it is undef'ed here
 */ 
#ifdef IP
#undef IP
#endif /* IP */
#endif /* CONFIG_USNET */

#ifdef __cplusplus
}
#endif

#endif /* __INCadpTxUnMapH */
