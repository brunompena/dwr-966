/* adpFXIO.h - io library for Threadx */

/* Copyright (c) 2006, TeamF1, Inc. */

/*
modification history
--------------------
01a,27may06,pkk  written 
*/

/*
DESCRIPTION

This module consists of the core io functionality.
*/
#ifndef __INCadpFXIOH
#define __INCadpFXIOH
/* defines */
#define MAX_IO_OPENFILES  100



/* typedefs */

typedef enum dev_types
    {
    INVALID_TYPE,    /* this should be first element */
    FX_DEVICE,
    SOCKET_TYPE,
    SERIAL_DEVICE,
    MAX_DEV_TYPES   /* this should be last element */
    } DEV_TYPE;

typedef struct file_handle_tag
    {
    DEV_TYPE      type;
    int           flags;
    void       *  desc;
    } FILE_HANDLE;

/* imports */
extern FILE_HANDLE file_handles[];

void io_init ( void );

extern FX_MEDIA * getMedia(void);

#endif /* __INCadpFXIOH */
