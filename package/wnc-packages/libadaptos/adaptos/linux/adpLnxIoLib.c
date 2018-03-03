/* adpLnxIoLib.c - linux I/O hooks for adaptos I/O library  */

/* Copyright (c) 2005 - 2007, TeamF1, Inc. */

/*
modification history
--------------------
01d,01oct07,san  added some wrapper for Linux IO routines 
                 and define __INCadpBuffIoTypesH.   
01c,03jan06,rnp  statically initialized adpLnxIoFuncTbl
01b,01oct05,rnp  warning fixes
01a,09mar05,uge  written
*/

/*
DESCRIPTION

This library provides Linux file I/O hook for adaptos I/O library.

INCLUDES: adaptos.h, adpIoTypes.h

SEE ALSO: 
*/

/* includes */
#define __INCadpIoLibH
#define __INCadpBuffIoTypesH
#include "adaptos.h"

/* defines */

/* forward declarations */

LOCAL ADP_FILE_ID   adpLnxIoCreat (const char * , int , int);
LOCAL STATUS        adpLnxOpen (const char *, int, int);
LOCAL int           adpLnxFread (void *,size_t, size_t, ADP_FILE_PTR);
LOCAL int           adpLnxFwrite(const void *,size_t, size_t, ADP_FILE_PTR);
LOCAL STATUS        adpLnxStat (const char *, struct stat *);
LOCAL BOOL          adpLnxIsatty (ADP_FILE_ID);
LOCAL STATUS        adpLnxChown (const char *, long, long);
LOCAL int           adpLnxIogetc (ADP_FILE_PTR);
LOCAL int           adpLnxIoputc (int,ADP_FILE_PTR);
LOCAL int           adpLnxIoferror (ADP_FILE_PTR);
LOCAL int           adpLnxIoputchar(int);

    
/* globals */

ADP_IO_FUNCTBL    adpLnxIoFuncTbl =
    {
    .adp_creat     = adpLnxIoCreat,
    .adp_open      = adpLnxOpen,
    .adp_close     = close,
    .adp_fopen     = fopen,
    .adp_fdopen    = fdopen,
    .adp_fclose    = fclose,
    .adp_read      = (int (*)(ADP_FILE_ID, void *, size_t))read,
    .adp_write     = (int (*)(ADP_FILE_ID, const void *, size_t))write,
    .adp_fread     = adpLnxFread,
    .adp_fwrite    = adpLnxFwrite,
    .adp_ioctl     = (int (*)(int, int, void *))ioctl,
    .adp_fgets     = (char * (*)(char *, int, ADP_FILE_PTR))fgets,
    .adp_fputs     = fputs,
    .adp_fprintf   = fprintf,
    .adp_stat      = adpLnxStat,
    .adp_fstat     = fstat,
    .adp_fseek     = fseek,
    .adp_lseek     = (off_t (*)(ADP_FILE_ID, off_t, int))lseek,
    .adp_ftell     = ftell,
    .adp_remove    = remove,
    .adp_unlink    = (STATUS (*)(const char *))unlink,
    .adp_isatty    = adpLnxIsatty,
    .adp_fileno    = fileno,
    .adp_chmod     = chmod,
    .adp_chown     = adpLnxChown,
    .adp_chdir     = (STATUS (*)(const char *))chdir,
    .adp_mkdir     = mkdir,
    .adp_rmdir     = (STATUS (*)(const char *))rmdir,
    .adp_opendir   = (ADP_DIR_ID (*)(const char *))opendir,
    .adp_readdir   = readdir,
    .adp_closedir  = closedir,
    .adp_truncate  = truncate,
    .adp_ftruncate = ftruncate,
    .adp_feof      = feof,
    .adp_getc      = adpLnxIogetc,
    .adp_setbuf    = setbuf,
    .adp_mktemp    = mkstemp,
    .adp_vfprintf  = vfprintf,
    .adp_fflush    = fflush,
    .adp_fputc     = fputc,
    .adp_putc      = adpLnxIoputc,
    .adp_ferror    = adpLnxIoferror,
    .adp_putchar   = adpLnxIoputchar,
    .adp_writev   =  writev  
    };

/*******************************************************************************
*
* adpLnxIoLibInstall - intialize adaptos IO hook structure 
*
* This routine initilizes adaptos file IO hook structure of Linux.
*
* RETURNS: pointer ADP_IO_FUNCTBL
*/

ADP_IO_FUNCTBL * adpLnxIoLibInstall
    (
    void * pArg     /* currently unused */
    )
    {

    adpLnxIoFuncTbl.adp_creat   = adpLnxIoCreat;
    adpLnxIoFuncTbl.adp_open    = adpLnxOpen;
    adpLnxIoFuncTbl.adp_close   = close;
    adpLnxIoFuncTbl.adp_fopen   = fopen;
    adpLnxIoFuncTbl.adp_fdopen  = fdopen;
    adpLnxIoFuncTbl.adp_fclose  = fclose;
    adpLnxIoFuncTbl.adp_read    = (int (*)(ADP_FILE_ID, void *,
                                           size_t))read;
    adpLnxIoFuncTbl.adp_write   = (int (*)(ADP_FILE_ID, const void *,
                                           size_t))write;
    adpLnxIoFuncTbl.adp_fread   = adpLnxFread;
    adpLnxIoFuncTbl.adp_fwrite  = adpLnxFwrite;
    adpLnxIoFuncTbl.adp_ioctl   = (int (*)(int, int, void *))ioctl;    
    adpLnxIoFuncTbl.adp_fgets   = (char * (*)(char *, int, ADP_FILE_PTR))fgets;
    adpLnxIoFuncTbl.adp_fputs   = fputs;
    adpLnxIoFuncTbl.adp_fprintf = fprintf;
    adpLnxIoFuncTbl.adp_stat    = adpLnxStat;    
    adpLnxIoFuncTbl.adp_fstat   = fstat;
    adpLnxIoFuncTbl.adp_fseek   = fseek;
    adpLnxIoFuncTbl.adp_lseek   = (off_t (*)(ADP_FILE_ID, off_t, int))lseek;
    adpLnxIoFuncTbl.adp_ftell   = ftell;
    adpLnxIoFuncTbl.adp_remove  = remove;
    adpLnxIoFuncTbl.adp_unlink  = (STATUS (*)(const char *))unlink;
    adpLnxIoFuncTbl.adp_isatty  = adpLnxIsatty;
    adpLnxIoFuncTbl.adp_fileno  = fileno;
    adpLnxIoFuncTbl.adp_chmod   = chmod;    
    adpLnxIoFuncTbl.adp_chown   = adpLnxChown;
    adpLnxIoFuncTbl.adp_chdir   = (STATUS (*)(const char *))chdir;
    adpLnxIoFuncTbl.adp_mkdir   = mkdir;
    adpLnxIoFuncTbl.adp_rmdir   = (STATUS (*)(const char *))rmdir;
    adpLnxIoFuncTbl.adp_opendir = (ADP_DIR_ID (*)(const char *))opendir;
    adpLnxIoFuncTbl.adp_readdir = readdir;
    adpLnxIoFuncTbl.adp_closedir = closedir;
    adpLnxIoFuncTbl.adp_truncate = truncate;
    adpLnxIoFuncTbl.adp_ftruncate = ftruncate;
    adpLnxIoFuncTbl.adp_feof = feof;
    
    return (&adpLnxIoFuncTbl);
    }

/*******************************************************************************
*
* adpLnxIoCreat - wrapper for Linux creat routine 
*
* This routine is a wrapper routine for Linux creat routine.
*
* RETURNS: Pointer ADP_IO_FUNCTBL
*/

LOCAL ADP_FILE_ID adpLnxIoCreat
    (
    const char * pName,   /* file path name */
    int          flags,  /* flags for the file to open */
    int          mode    /* mode for the file to create  */
    )
    {
    return (creat (pName, mode));
    }

/*******************************************************************************
*
* adpLnxOpen - wrapper for Linux open routine 
*
* This routine is a wrapper routine for Linux open routine.
*
* RETURNS: 1 or 0
*/

LOCAL STATUS adpLnxOpen
    (
    const char * path,    /* pointer to path */
    int          flags,   /* flags */
    int          mode     /* mode */
    )
    {
    return (open  (path, flags, (mode_t) mode));
    }

/*******************************************************************************
*
* adpLnxFread - wrapper for Linux fread routine 
*
* This routine is a wrapper routine for Linux fread routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpLnxFread
    (
    void *         ptr,     /* ptr to store */
    size_t         size,    /* size of data */
    size_t         nmemb,   /* no. of elements */
    ADP_FILE_PTR   stream   /* the stream to use */
    )
    {
    return (fread (ptr, size, nmemb, stream));
    }

/*******************************************************************************
*
* adpLnxFwrite - wrapper for Linux fwrite routine 
*
* This routine is a wrapper routine for Linux fwrite routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpLnxFwrite
    (
    const void *   ptr,     /* ptr to store */
    size_t         size,    /* size of data */
    size_t         nmemb,   /* no. of elements */
    ADP_FILE_PTR   stream   /* the stream to use */
    )
    {
    return (fwrite (ptr, size, nmemb, stream));
    }


/*******************************************************************************
*
* adpLnxStat - wrapper for Linux stat routine 
*
* This routine is a wrapper routine for Linux stat routine.
*
* RETURNS: 0 or -1
*/

LOCAL STATUS adpLnxStat
    (
    const char *        pName,   /* file name */
    struct stat * pBuf     /* pointer to stat */
    )
    {
    return (stat (pName, pBuf));
    }

/*******************************************************************************
*
* adpLnxIsatty - wrapper for Linux isatty routine 
*
* This routine is a wrapper routine for Linux isatty routine.
*
* RETURNS: 1 or 0
*/

LOCAL BOOL adpLnxIsatty
    (
    ADP_FILE_ID    desc       /* descriptor */
    )
    {
    return (isatty ((int) desc));
    }

/*******************************************************************************
*
* adpLnxChown - wrapper for Linux chown routine 
*
* This routine is a wrapper routine for Linux chown routine.
*
* RETURNS: 1 or 0
*/

LOCAL STATUS adpLnxChown
    (
    const char * path,    /* pointer to path */
    long         owner,   /* owner */
    long         group    /* group */
    )
    {
    return (chown (path,(uid_t) owner,(gid_t) group));
    }

/*******************************************************************************
*
* adpLnxIogetc  - wrapper for Linux getc routine 
*
* This routine is a wrapper routine for Linux getc routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpLnxIogetc 
    (
    ADP_FILE_PTR  fp     /* file pointer to read from */
    )
    {
    return (getc(fp));
    }

/*******************************************************************************
*
* adpLnxIoputc   - wrapper for Linux putc routine 
*
* This routine is a wrapper routine for Linux putc routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpLnxIoputc 
    (
    int c,
    ADP_FILE_PTR fp
    )
    {
    return (putc(c,fp));
    }

/*******************************************************************************
*
* adpLnxIoferror  - wrapper for Linux ferror routine 
*
* This routine is a wrapper routine for Linux ferror routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpLnxIoferror
    (
    ADP_FILE_PTR fp
    )
    {
    return (ferror(fp));
    }
    
/*******************************************************************************
*
* adpLnxIoputchar  - wrapper for Linux putchar routine 
*
* This routine is a wrapper routine for Linux putchar routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpLnxIoputchar
    (
    int c
    )
    {
    return (putchar(c));
    }


