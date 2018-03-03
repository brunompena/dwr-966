/* adpIntIoLib.c - Integrity I/O hooks for adaptos I/O library  */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,08apr08,san  written
*/

/*
DESCRIPTION

This library provides Integrity file I/O hook for adaptos I/O library.

INCLUDES: adaptos.h, adpIoTypes.h

SEE ALSO: 
*/

/* includes */
#define __INCadpIoLibH
#define __INCadpBuffIoTypesH
#include "adaptos.h"

/* defines */

/* forward declarations */

LOCAL ADP_FILE_ID   adpIntIoCreat (const char * , int , int);
LOCAL STATUS        adpIntOpen (const char *, int, int);
LOCAL int           adpIntFread (void *,size_t, size_t, ADP_FILE_PTR);
LOCAL int           adpIntFwrite(const void *,size_t, size_t, ADP_FILE_PTR);
LOCAL STATUS        adpIntStat (const char *, struct stat *);
LOCAL BOOL          adpIntIsatty (ADP_FILE_ID);
LOCAL STATUS        adpIntChown (const char *, long, long);
LOCAL STATUS        adpIntFileStat(int, struct stat *);

/* add funtion prototype */ 
int mktemp(const char * a);
    
/* globals */

ADP_IO_FUNCTBL    adpIntIoFuncTbl =
    {
     adpIntIoCreat,
     adpIntOpen,
     close,
     fopen,
     fdopen,
     fclose,
     (int (*)(ADP_FILE_ID, void *, size_t))read,
     (int (*)(ADP_FILE_ID, const void *, size_t))write,
     adpIntFread,
     adpIntFwrite,
     (int (*)(int, int, void *))ioctl,
     (char * (*)(char *, int, ADP_FILE_PTR))fgets,
     fputs,
     fprintf,
     adpIntStat,
     adpIntFileStat,
     fseek,
     (off_t (*)(ADP_FILE_ID, off_t, int))lseek,
     ftell,
     remove,
     (STATUS (*)(const char *))unlink,
     adpIntIsatty,
     fileno,
     chmod,
     adpIntChown,
     (STATUS (*)(const char *))chdir,
     mkdir,
     (STATUS (*)(const char *))rmdir,
     (ADP_DIR_ID (*)(const char *))opendir,
     readdir,
     closedir,
     truncate,
     ftruncate,
     feof
    };

/*******************************************************************************
*
* adpIntIoLibInstall - intialize adaptos IO hook structure 
*
* This routine initilizes adaptos file IO hook structure of Integrity.
*
* RETURNS: pointer ADP_IO_FUNCTBL
*/

ADP_IO_FUNCTBL * adpIntIoLibInstall
    (
    void * pArg     /* currently unused */
    )
    {

    adpIntIoFuncTbl.adp_creat   = adpIntIoCreat;
    adpIntIoFuncTbl.adp_open    = adpIntOpen;
    adpIntIoFuncTbl.adp_close   = close;
    adpIntIoFuncTbl.adp_fopen   = fopen;
    adpIntIoFuncTbl.adp_fdopen  = fdopen;
    adpIntIoFuncTbl.adp_fclose  = fclose;
    adpIntIoFuncTbl.adp_read    = (int (*)(ADP_FILE_ID, void *,
                                           size_t))read;
    adpIntIoFuncTbl.adp_write   = (int (*)(ADP_FILE_ID, const void *,
                                           size_t))write;
    adpIntIoFuncTbl.adp_fread   = adpIntFread;
    adpIntIoFuncTbl.adp_fwrite  = adpIntFwrite;
    adpIntIoFuncTbl.adp_ioctl   = (int (*)(int, int, void *))ioctl;    
    adpIntIoFuncTbl.adp_fgets   = (char * (*)(char *, int, ADP_FILE_PTR))fgets;
    adpIntIoFuncTbl.adp_fputs   = fputs;
    adpIntIoFuncTbl.adp_fprintf = fprintf;
    adpIntIoFuncTbl.adp_stat    = adpIntStat;    
    adpIntIoFuncTbl.adp_fstat   = adpIntFileStat;
    adpIntIoFuncTbl.adp_fseek   = fseek;
    adpIntIoFuncTbl.adp_lseek   = (off_t (*)(ADP_FILE_ID, off_t, int))lseek;
    adpIntIoFuncTbl.adp_ftell   = ftell;
    adpIntIoFuncTbl.adp_remove  = remove;
    adpIntIoFuncTbl.adp_unlink  = (STATUS (*)(const char *))unlink;
    adpIntIoFuncTbl.adp_isatty  = adpIntIsatty;
    adpIntIoFuncTbl.adp_fileno  = fileno;
    adpIntIoFuncTbl.adp_chmod   = chmod;    
    adpIntIoFuncTbl.adp_chown   = adpIntChown;
    adpIntIoFuncTbl.adp_chdir   = (STATUS (*)(const char *))chdir;
    adpIntIoFuncTbl.adp_mkdir   = mkdir;
    adpIntIoFuncTbl.adp_rmdir   = (STATUS (*)(const char *))rmdir;
    adpIntIoFuncTbl.adp_opendir = (ADP_DIR_ID (*)(const char *))opendir;
    adpIntIoFuncTbl.adp_readdir = readdir;
    adpIntIoFuncTbl.adp_closedir = closedir;
    adpIntIoFuncTbl.adp_truncate = truncate;
    adpIntIoFuncTbl.adp_ftruncate = ftruncate;
    adpIntIoFuncTbl.adp_feof = feof;
    
    return (&adpIntIoFuncTbl);
    }

/*******************************************************************************
*
* adpIntIoCreat - wrapper for Integrity creat routine 
*
* This routine is a wrapper routine for Integrity creat routine.
*
* RETURNS: Pointer ADP_IO_FUNCTBL
*/

LOCAL ADP_FILE_ID adpIntIoCreat
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
* adpIntOpen - wrapper for Integrity open routine 
*
* This routine is a wrapper routine for Integrity open routine.
*
* RETURNS: 1 or 0
*/

LOCAL STATUS adpIntOpen
    (
    const char * path,    /* pointer to path */
    int          flags,   /* flags */
    int          mode     /* mode */
    )
    {
    int ret;
    ret = open  (path, flags, (mode_t) mode);
    
    /* set file open error number in case if some env does not set 
     * appropriate error no. */
#ifdef CONFIG_SIM_USE_WINDOWS_HOST_IO
    if (ret == -1)
        errno = ENOENT;
#endif /* CONFIG_SIM_USE_WINDOWS_HOST_IO */    
    return (ret);
    }

/*******************************************************************************
*
* adpIntFread - wrapper for Integrity fread routine 
*
* This routine is a wrapper routine for Integrity fread routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpIntFread
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
* adpIntFwrite - wrapper for Integrity fwrite routine 
*
* This routine is a wrapper routine for Integrity fwrite routine.
*
* RETURNS: 1 or 0
*/

LOCAL int adpIntFwrite
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
* adpIntStat - wrapper for Integrity stat routine 
*
* This routine is a wrapper routine for Integrity stat routine.
*
* RETURNS: 0 or -1
*/

LOCAL STATUS adpIntStat
    (
    const char *        pName,   /* file name */
    struct stat * pBuf     /* pointer to stat */
    )
    {
    return (stat (pName, pBuf));
    }

/*******************************************************************************
*
* adpIntIsatty - wrapper for Integrity isatty routine 
*
* This routine is a wrapper routine for Integrity isatty routine.
*
* RETURNS: 1 or 0
*/

LOCAL BOOL adpIntIsatty
    (
    ADP_FILE_ID    desc       /* descriptor */
    )
    {
    printf("isatty is not supported\n");
    return (B_TRUE); 
    }

/*******************************************************************************
*
* adpIntChown - wrapper for Integrity chown routine 
*
* This routine is a wrapper routine for Integrity chown routine.
*
* RETURNS: 1 or 0
*/

LOCAL STATUS adpIntChown
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
* adpIntFileStat - wrapper for Integrity fstat routine 
*
* This routine is a wrapper routine for Integrity fstat routine.
*
* RETURNS: 1 or 0
*/

LOCAL STATUS adpIntFileStat
    (
    int             fd,    /* file descriptor */
    struct stat *   pStat  /* pointer to stat structure */
    )
    {
    int ret=ST_OK;
#ifdef HAVE_FSTAT_PARTIAL
    int offset;
    
     if((offset = lseek(fd, 0, SEEK_CUR)) == -1)
         return (ST_ERROR);

     if((pStat->st_size=lseek(fd, 0,SEEK_END)) == -1)
          return (ST_ERROR);

     if((offset = lseek(fd, offset, SEEK_SET)) == -1)
         return (ST_ERROR); 
#else
    ret =  fstat(fd,pStat);
#endif /* HAVE_FSTAT_PARTIAL */
     return (ret);
    }
