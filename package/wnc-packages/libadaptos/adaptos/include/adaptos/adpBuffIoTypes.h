/* adpBuffIoTypes.h -  adaptos Buffer based file types header */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01a,06mar07,san created.
*/

#ifndef __INCadpBuffIoTypesH
#define __INCadpBuffIoTypesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define BUFFERSIZE 2048
#define MAX_IO_OPENFILES  100

/* typedefs */	
#ifndef HAVE_FILE_STRUCT
typedef struct
    {
       char   fname[100];                    /* name of buffer */
       char * base;                          /* location of buffer */
       char * rptr;  	                     /* read pointer of buffer */
       char * wptr;                          /* write poiter of buffer */
       int    mode;                          /* mode of file access */
       int    flag;                          /* flag of file */ 
       int    fd;                            /* file descriptor  */ 
       int    file_count;                    /* no of open file instance */  
    } FILE_STRUCT;
#define HAVE_FILE_STRUCT
#endif


enum _flags {
     _READ  = 01,   /* file open for reading */    
     _WRITE = 02,   /* file open for writing */   
     _RW    = 04,   /* file for reading and writing */ 
     _UNBUF = 010,  /* file if unbuffered */ 
     _EOF   = 020,  
     _ERR   = 040    
}; 

#define FILE FILE_STRUCT

#undef  stderr
#undef  stdout
#undef  stdin
#define stdin   (void *)-1
#define stdout  (void *)1
#define stderr  (void *)2


/* Resolving Linking Error */
#define IOV_MAX 1024 /* used in adpWritev */

/* defines */
#ifndef ADP_IO_API_FUNCTION_TABLE
#define ADP_IO_API_FUNCTION_TABLE   adpBuffIoFuncTbl
#endif /* ADP_IO_API_FUNCTION_TABLE */

#ifndef ADP_IO_API_LIB_INIT_RTN
#define ADP_IO_API_LIB_INIT_RTN     adpBuffIoLibInstall
#endif /* ADP_IO_API_LIB_INIT_RTN */

#ifdef __cplusplus
}
#endif

#endif /* __INCadpBuffIoTypesH */

