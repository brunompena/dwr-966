/* adpIoLib.c - adaptos wrapper layer for file IO */

/* Copyright (c) 2003 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01j,09jul08,rnp  change prototypes of adpSetBuf(), adpMkTemp() to address
                 warnings on vx 5.5
01i,04oct07,san  added wrapper for IO routines.
01h,03jan06,rnp  static initialization of pAdpIoFuncTbl
01g,20aug03,ksn  fixed lseek prototype
01f,19aug03,gnm  fixed ioctl prototype
01e,18aug03,ksn  fixed compilation warnings
01d,21jun03,ksn  fixed fgets return type.
01c,17jun03,gnm  code cleanup.
01b,20may03,ksn  fixed adpFilePrintf function.
01a,15apr03,ksn  created.
*/

/*
DESCRIPTION

This library provides wrapper layer of file IO abstraction. User specific
routines are installed in to this wrapper layer by calling `adpIoLibInit'. 

.CS
  STATUS adpIoLibInit (ADP_IO_INSTALL_FUNCPTR adpIoFuncTblInstallRtn, 
                       void * arg1);
.CE

This routine is supposed to be called with a function pointer as an argument
which will return pointer to `ADP_IO_FUNCTBL' structure. This structure needs
to be filled with user specific implementation file IO.

.CS

typedef struct
    {
    ADP_FILE_ID     (*adp_creat) (const char * , int , int );
    ADP_FILE_ID     (*adp_open) (const char * , int , int );
    STATUS        (*adp_close) (ADP_FILE_ID );
    ADP_FILE_PTR    (*adp_fopen) (const char * , const char * );
    ADP_FILE_PTR    (*adp_fdopen) (int , const char * );
    int         (*adp_fclose) (ADP_FILE_PTR );
    int         (*adp_read) (ADP_FILE_ID , void * , size_t );
    int         (*adp_write) (ADP_FILE_ID , const void * ,
                                      size_t);   
    int         (*adp_fread) (void *, size_t , size_t , ADP_FILE_PTR);
    int         (*adp_fwrite) (const void * , size_t ,  \
                                       size_t , ADP_FILE_PTR );
    int         (*adp_ioctl) (int , int , void * );
    char *      (*adp_fgets) (char* , int , ADP_FILE_PTR );
    int         (*adp_fputs) (const char * , ADP_FILE_PTR);
    int         (*adp_fprintf) (ADP_FILE_PTR , const char * , ...);
    STATUS      (*adp_stat) (const char * , struct stat * ); 
    STATUS      (*adp_fstat) (int , struct stat * );
    int         (*adp_fseek) (ADP_FILE_PTR , long , int);
    off_t       (*adp_lseek) (ADP_FILE_ID, off_t, int);
    long        (*adp_ftell) (ADP_FILE_PTR);
    STATUS      (*adp_remove) (const char * );
    STATUS      (*adp_unlink) (const char *);
    BOOL        (*adp_isatty) (ADP_FILE_ID );
    int         (*adp_fileno) (ADP_FILE_PTR );
    STATUS      (*adp_chmod) (const char * , UINT );
    STATUS      (*adp_chown) (const char * , long, long); 
    STATUS      (*adp_chdir) (const char * );
    STATUS      (*adp_mkdir) (const char * , UINT);
    STATUS      (*adp_rmdir) (const char * );
    ADP_DIR_ID      (*adp_opendir) (const char * );
    ADP_DIRENT_ID   (*adp_readdir) (ADP_DIR_ID);
    STATUS      (*adp_closedir) (ADP_DIR_ID);    
    STATUS      (*adp_truncate) (const char * , off_t );
    STATUS      (*adp_ftruncate) (ADP_FILE_ID , off_t );
    int         (*adp_feof) (ADP_FILE_PTR);
    } ADP_IO_FUNCTBL;
.CE

Description of each hook routine is given below, it describes all the arguments
passed and the return values.

INCLUDES: adpIoLib.h

SEE ALSO: 

*/

/* includes */

#include "adaptos.h"

/* defines */


/* globals */

LOCAL ADP_IO_FUNCTBL *  pAdpIoFuncTbl = &ADP_IO_API_FUNCTION_TABLE;

/*******************************************************************************
*
* adpIoLibInit - initialize adaptos IO layer to user specific routines  
*
* This routine intializes adaptos file IO layer to user specific IO routines.
* User install his file IO routines in `ADP_IO_FUNCTBL' and returns pointer to
* that structure from his own install routine. This user install routine is
* passed as an argument to this routine.
* 
* RETURNS: ST_OK on success and ST_ERROR otherwise
*/

STATUS adpIoLibInit
    (
    ADP_IO_INSTALL_FUNCPTR adpIoFuncTblInstallRtn,      /* IO FUNCPTR rtn */
    void *  arg1                                        /* arg 1 */
    )
    {
    
    if (adpIoFuncTblInstallRtn != NULL)
        {
        if ((pAdpIoFuncTbl = (ADP_IO_FUNCTBL *)adpIoFuncTblInstallRtn (arg1))
            != NULL)
            return (ST_OK);
        else
            adpErrnoSet (S_adpIoLib_FUNCTBL_INVALID);
        }
    else
        adpErrnoSet (S_adpIoLib_FUNCTBL_INSTALLRTN_INVALID);        
        
    
    return (ST_ERROR);
    }

/*******************************************************************************
*
* adpIoErrSet - set IO specific error level
*
* This routine is called to set adaptos IO layer specific error number.
*
* RETURNS: N/A
*/

void adpIoErrSet
    (
    int err             /* error number */
    )
    {
    adpErrnoSet (M_adpIoLib | err);
    }

/*******************************************************************************
* 
* adpCreat - call user specific creat routine
*
* This routine calls user specific `creat' routine if installed, otherwise set
* error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: File deccriptor on success and -1 on failure 
*/

ADP_FILE_ID adpCreat
    (
    const char *    fileName,   /* path for file to creat */
    int         flag,       /* could be either O_RDWR, O_WRONLY */ 
    int         mode        /* mode to use for new file created */ 
    )
    {
    
    if (pAdpIoFuncTbl->adp_creat != NULL)
        return (pAdpIoFuncTbl->adp_creat (fileName, flag, mode));
        
    adpErrnoSet (S_adpIoLib_CREAT_FUNCPTR_NULL);
    return (ADP_IO_ERROR);        
    }

/*******************************************************************************
* 
* adpOpen - call user specific open routine
*
* This routine calls user specific `open' routine if installed, otherwise set
* error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: File deccriptor on success and -1 on failure 
*/

ADP_FILE_ID adpOpen
    (
    const char *    fileName,   /* path for file to open */
    int         flag,       /* could be either O_RDWR, O_WRONLY */ 
    int         mode        /* mode to use for new file created */ 
    )
    {
    
    if (pAdpIoFuncTbl->adp_open != NULL)
        return (pAdpIoFuncTbl->adp_open (fileName, flag, mode));
        
    adpErrnoSet (S_adpIoLib_OPEN_FUNCPTR_NULL);
    return (ADP_IO_ERROR);        
    }

/*******************************************************************************
* 
* adpClose - call user specific close routine
*
* This routine calls user specific `close' routine if installed, otherwise sets
* error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure 
*/

STATUS adpClose
    (
    ADP_FILE_ID     fileId  /* file identifier */
    )
    {
    if (pAdpIoFuncTbl->adp_close != NULL)
        return (pAdpIoFuncTbl->adp_close (fileId));
        
    adpErrnoSet (S_adpIoLib_CLOSE_FUNCPTR_NULL);
    return (ADP_IO_ERROR);            
    }

/*******************************************************************************
* 
* adpFileOpen - call user specific fopen routine
*
* This routine calls user specific `fopen' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: File pointer on success and NULL pointer on failure 
*/

ADP_FILE_PTR adpFileOpen
    (
    const char *    fileName,  /* path for file to open */
    const char *    mode       /* mode for file to open */
    )
    {
    if (pAdpIoFuncTbl->adp_fopen != NULL)
        return ((ADP_FILE_PTR) pAdpIoFuncTbl->adp_fopen (fileName, mode));
        
    adpErrnoSet (S_adpIoLib_FOPEN_FUNCPTR_NULL);
    return (NULL);            
    }

/*******************************************************************************
* 
* adpFdopen - call user specific fdopen routine
*
* This routine calls user specific `fdopen' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns NULL pointer.
*
* RETURNS: File pointer on success and NULL pointer on failure 
*/

ADP_FILE_PTR adpFdopen
    (
    ADP_FILE_ID     fd,        /* file descriptor */
    const char *    mode       /* mode for file to open */
    )
    {
    if (pAdpIoFuncTbl->adp_fdopen != NULL)
        return ((ADP_FILE_PTR) pAdpIoFuncTbl->adp_fdopen (fd, mode));
        
    adpErrnoSet (S_adpIoLib_FDOPEN_FUNCPTR_NULL);
    return (NULL);            
    }

/*******************************************************************************
* 
* adpFileClose - call user specific fclose routine
*
* This routine calls user specific `fclose' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure 
*/

int adpFileClose
    (
    ADP_FILE_PTR    filePtr   /* poinetr to file to close */
    )
    {
    if (pAdpIoFuncTbl->adp_fclose != NULL)
        return (pAdpIoFuncTbl->adp_fclose (filePtr));
        
    adpErrnoSet (S_adpIoLib_FCLOSE_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                
    }

/*******************************************************************************
* 
* adpRead - call user specific read routine
*
* This routine calls user specific `read' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: Number of bytes read on success and ADP_IO_ERROR on failure 
*/

int adpRead
    (
    ADP_FILE_ID     fd,       /* file descriptor from which to read*/
    char *      buffer,   /* pointer to buffer to receive */ 
    size_t      maxbytes  /* maximum number of bytes to read */
    )
    {
    if (pAdpIoFuncTbl->adp_read != NULL)
        return (pAdpIoFuncTbl->adp_read (fd, buffer, maxbytes));
        
    adpErrnoSet (S_adpIoLib_READ_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                    
    }


/*******************************************************************************
* 
* adpWrite - call user specific write routine
*
* This routine calls user specific `write' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: number of bytes writen on success and ADP_IO_ERROR on failure 
*/

int adpWrite
    (
    ADP_FILE_ID     fd,      /* file descriptor to write to*/
    const char *        buffer,  /* pointer to buffer contating data to */
                                 /* be written to fd */
    unsigned long   nbytes   /* number of bytes to write */
    )
    {
    if (pAdpIoFuncTbl->adp_write != NULL)
        return (pAdpIoFuncTbl->adp_write (fd, buffer, nbytes));
        
    adpErrnoSet (S_adpIoLib_WRITE_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                        
    }

/*******************************************************************************
* 
* adpFileRead - call user specific fread routine
*
* This routine calls user specific `fread' routine if installed, otherwise
* set error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: Number of elements read on success and ADP_IO_ERROR on failure 
*/

int adpFileRead
    (
    void *      buf,   /* buffer to store read bytes */
    size_t      size,  /* size of element */
    size_t      count, /* number of element */
    ADP_FILE_PTR        fp     /* file pointer to read from */
    )
    {
    if (pAdpIoFuncTbl->adp_fread != NULL)
        return (pAdpIoFuncTbl->adp_fread (buf, size, count, fp));
        
    adpErrnoSet (S_adpIoLib_FREAD_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                            
    }

/*******************************************************************************
* 
* adpFileWrite - call user specific fwrite routine
*
* This routine calls user specific `fwrite' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: Number of elements written on success and ADP_IO_ERROR on failure.
*/

int adpFileWrite
    (
    const void *    buf,    /* buffer to copy from*/ 
    size_t      size,   /* element size */
    size_t      count,  /* number of element to write */
    ADP_FILE_PTR        fp      /* file pointer to write to */
    )
    {
    if (pAdpIoFuncTbl->adp_fwrite != NULL)
        return (pAdpIoFuncTbl->adp_fwrite (buf, size, count, fp));
        
    adpErrnoSet (S_adpIoLib_FWRITE_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                            
    }

/*******************************************************************************
* 
* adpIoctl - call user specific ioctl routine
*
* This routine calls user specific `ioctl' routine if installed, otherwise
* sets the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: 0 or return value of fuction on success and ADP_IO_ERROR on
* failure 
*/

int adpIoctl
    (
    int         fd,        /* file or device descriptor */
    int         function,  /* function code */
    void *      arg        /* argument */
    )
    {
    if (pAdpIoFuncTbl->adp_ioctl != NULL)
        return (pAdpIoFuncTbl->adp_ioctl (fd, function, arg));
        
    adpErrnoSet (S_adpIoLib_IOCTL_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                                
    }

/*******************************************************************************
* 
* adpFileGets - call user specific fgets routine
*
* This routine calls user specific `fgets' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns NULL pointer.
*
* RETURNS: Pointer to buffer on success and NULL pointer on failure. 
*/

char * adpFileGets
    (
    char *      buf,    /* buffer where bytes are copied */
    int         size,   /* size of character to read */
    ADP_FILE_PTR    fp      /* stream from which bytes are read */
    )
    {
    if (pAdpIoFuncTbl->adp_fgets != NULL)
        return (pAdpIoFuncTbl->adp_fgets (buf, size, fp));
        
    adpErrnoSet (S_adpIoLib_FGETS_FUNCPTR_NULL);
    return (NULL);                                
    }

/*******************************************************************************
* 
* adpFilePuts - call user specific fputs routine
*
* This routine calls user specific `fputs' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: Non-negative value on success ADP_IO_ERROR on failure. 
*/

int adpFilePuts
    (
    const char *    buf,    /* buffer from which data is read */
    ADP_FILE_PTR    fp      /* stream to which bytes are written */
    )
    {
    if (pAdpIoFuncTbl->adp_fputs != NULL)
        return (pAdpIoFuncTbl->adp_fputs (buf, fp));
        
    adpErrnoSet (S_adpIoLib_FPUTS_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                                
    }

/*******************************************************************************
* 
* adpFilePrintf - call user specific fprintf routine
*
* This routine calls user specific `fprintf' routine if installed, otherwise
* sets the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: Number of bytes written on success and ADP_IO_ERROR on failure.
*/

int adpFilePrintf
    (
    ADP_FILE_PTR    fp,     /* file pointer */
    const char *    fmt,    /* format of the string */
    ...                 /* variable arguments */
    )
    {
    va_list args;
    int     arg[10];
    int     argnum;
    
    va_start(args, fmt);
    for (argnum = 0; argnum < 10; argnum++)
        arg[argnum] = va_arg(args, long int);
    va_end(args);
    
    if (pAdpIoFuncTbl->adp_fprintf != NULL)
        return (pAdpIoFuncTbl->adp_fprintf (fp, fmt, arg[0], arg[1], arg[2],
                                            arg[3], arg[4], arg[5], arg[6],
                                            arg[7], arg[8], arg[9]));

    adpErrnoSet (S_adpIoLib_FPRINTF_FUNCPTR_NULL);
    return (ADP_IO_ERROR);    
    }


/*******************************************************************************
* 
* adpStat - call user specific stat routine
*
* This routine calls user specific `stat' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure.
*/

STATUS adpStat
    (
    const char *        file,  /* name of file */
    struct stat *   pStat  /* pointer to stat structure */
    )
    {
    if (pAdpIoFuncTbl->adp_stat != NULL)
        return (pAdpIoFuncTbl->adp_stat (file, pStat ));    

    adpErrnoSet (S_adpIoLib_STAT_FUNCPTR_NULL);
    return (ADP_IO_ERROR);        
    }

/*******************************************************************************
* 
* adpFileStat - call user specific fstat routine
*
* This routine calls user specific `fstat' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure.
*/

STATUS adpFileStat
    (
    int         fd,    /* file descriptor */
    struct stat *   pStat  /* pointer to stat structure */
    )
    {
    if (pAdpIoFuncTbl->adp_fstat != NULL)
        return (pAdpIoFuncTbl->adp_fstat (fd, pStat ));    

    adpErrnoSet (S_adpIoLib_FSTAT_FUNCPTR_NULL);
    return (ADP_IO_ERROR);        
    }

/*******************************************************************************
* 
* adpFileSeek - call user specific fseek routine
*
* This routine calls user specific `fseek' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpFileSeek
    (
    ADP_FILE_PTR    fp,      /* file descriptor */
    long        offset,  /* offset from whence */
    int         whence   /* position to offset from */
    )
    {
    if (pAdpIoFuncTbl->adp_fseek != NULL)
        return (pAdpIoFuncTbl->adp_fseek (fp, offset, whence));    

    adpErrnoSet (S_adpIoLib_FSEEK_FUNCPTR_NULL);
    return (ADP_IO_ERROR);            
    }

/*******************************************************************************
* 
* adpLseek - call user specific lseek routine
*
* This routine calls user specific `fseek' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: Offset from begining of file in bytes on success and ADP_IO_ERROR
*          on failure. 
*/

off_t adpLseek
    (
    ADP_FILE_ID     fd,      /* file descriptor */
    off_t       offset,  /* offset from whence */
    int         whence   /* position to offset from */
    )
    {
    if (pAdpIoFuncTbl->adp_lseek != NULL)
        return (pAdpIoFuncTbl->adp_lseek (fd, offset, whence));    

    adpErrnoSet (S_adpIoLib_LSEEK_FUNCPTR_NULL);
    return (ADP_IO_ERROR);            
    }

/*******************************************************************************
* 
* adpFileTell - call user specific ftell routine
*
* This routine calls user specific `ftell' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: Current offset in bytes on success and ADP_IO_ERROR on failure. 
*/

long adpFileTell
    (
    ADP_FILE_PTR    fp   /* file descriptor */
    )
    {
    if (pAdpIoFuncTbl->adp_ftell != NULL)
        return (pAdpIoFuncTbl->adp_ftell (fp));    

    adpErrnoSet (S_adpIoLib_FTELL_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                
    }

/*******************************************************************************
* 
* adpRemove - call user specific remove routine
*
* This routine calls user specific `remove' routine if installed, otherwise
* sets the error number to FUNCPTR_NULL and returns -1.
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpRemove
    (
    const char *    name   /* name of file */
    )
    {
    if (pAdpIoFuncTbl->adp_remove != NULL)
        return (pAdpIoFuncTbl->adp_remove (name));    

    adpErrnoSet (S_adpIoLib_REMOVE_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                    
    }

/*******************************************************************************
* 
* adpUnlink - call user specific unlink routine
*
* This routine calls user specific `unlink' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns -1. This routine is same as
* `remove'.
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpUnlink
    (
    const char *    name   /* name of file */
    )
    {
    if (pAdpIoFuncTbl->adp_unlink != NULL)
        return (pAdpIoFuncTbl->adp_unlink (name));    

    adpErrnoSet (S_adpIoLib_UNLINK_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                    
    }

/*******************************************************************************
* 
* adpIsatty - call user specific isatty routine
*
* This routine calls user specific `isatty' routine if installed, otherwise sets
* error number to FUNCPTR_NULL and returns FALSE. `isatty' routine checks
* whether underlying device is tty or not.
*
* RETURNS: TRUE on success and FALSE on failure. 
*/

BOOL adpIsatty
    (
    ADP_FILE_ID     fd   /* file descriptor */
    )
    {
    if (pAdpIoFuncTbl->adp_isatty != NULL)
        return (pAdpIoFuncTbl->adp_isatty (fd));    

    adpErrnoSet (S_adpIoLib_ISATTY_FUNCPTR_NULL);
    return (FALSE);                        
    }

/*******************************************************************************
* 
* adpFileno - call user specific fileno routine
*
* This routine calls user specific `fileno' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: Returns desriptor on success and ADP_IO_ERROR on failure.
*/

int adpFileno
    (
    ADP_FILE_PTR    filePtr /* file pointer */
    )
    {
    if (pAdpIoFuncTbl->adp_fileno != NULL)
        return (pAdpIoFuncTbl->adp_fileno (filePtr));

    adpErrnoSet (S_adpIoLib_FILENO_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                            
    }

/*******************************************************************************
* 
* adpChmod - call user specific chmod routine
*
* This routine calls user specific `chmod' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpChmod
    (
    char *      path,  /* file path*/
    UINT        mode   /* new mode of file */
    )
    {
    if (pAdpIoFuncTbl->adp_chmod != NULL)
        return (pAdpIoFuncTbl->adp_chmod (path, mode));    

    adpErrnoSet (S_adpIoLib_CHMOD_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                                
    }

/*******************************************************************************
* 
* adpChown - call user specific chown routine
*
* This routine calls user specific `chown' routine if installed, otherwise
* set error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure 
*/

STATUS adpChown
    (
    const char *    name,    /* file name*/
    int         owner,   /* owner of file */
    int         group    /* group of th file */
    )
    {
    if (pAdpIoFuncTbl->adp_chown != NULL)
        return (pAdpIoFuncTbl->adp_chown (name, owner, group));    

    adpErrnoSet (S_adpIoLib_CHOWN_FUNCPTR_NULL);
    return (ADP_IO_ERROR);                                
    }

/*******************************************************************************
* 
* adpChdir - call user specific chdir routine
*
* This routine calls user specific `chdir' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpChdir
    (
    char *      path   /* name of new directory path*/
    )
    {
    if (pAdpIoFuncTbl->adp_chdir != NULL)
        return (pAdpIoFuncTbl->adp_chdir (path));    

    adpErrnoSet (S_adpIoLib_CHDIR_FUNCPTR_NULL);
    return (ADP_IO_ERROR); 
    }

/*******************************************************************************
* 
* adpMkdir - call user specific mkdir routine
*
* This routine calls user specific `mkdir' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure 
*/

STATUS adpMkdir
    (
    char*       pathname,  /* name of new directory path */
    UINT        mode       /* mode of new directory */
    )
    {
    if (pAdpIoFuncTbl->adp_mkdir != NULL)
        return (pAdpIoFuncTbl->adp_mkdir (pathname, mode));    

    adpErrnoSet (S_adpIoLib_MKDIR_FUNCPTR_NULL);
    return (ADP_IO_ERROR); 
    }

/*******************************************************************************
* 
* adpRmdir - call user specific rmdir routine
*
* This routine calls user specific `rmdir' routine if installed, otherwise sets
* the error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure 
*/

STATUS adpRmdir
    (
    char*       pathname     /* directory path to remove */
    )
    {
    if (pAdpIoFuncTbl->adp_rmdir != NULL)
        return (pAdpIoFuncTbl->adp_rmdir (pathname));    

    adpErrnoSet (S_adpIoLib_RMDIR_FUNCPTR_NULL);
    return (ADP_IO_ERROR);     
    }

/*******************************************************************************
* 
* adpOpendir - call user specific opendir routine
*
* This routine calls user specific `opendir' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns NULL pointer. 
*
* RETURNS: Pointer to DIR structure on success and NULL pointer on failure. 
*/

ADP_DIR_ID adpOpendir
    (
    const char *        dirName    /* name of directory to open */
    )
    {
    if (pAdpIoFuncTbl->adp_opendir != NULL)
        return ((ADP_DIR_ID)pAdpIoFuncTbl->adp_opendir (dirName));    

    adpErrnoSet (S_adpIoLib_OPENDIR_FUNCPTR_NULL);
    return (NULL);         
    }

/*******************************************************************************
* 
* adpReaddir - call user specific readdir routine
*
* This routine calls user specific `readdir' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns NULL pointer. 
*
* RETURNS: Pointer to dirent structure on success, NULL pointer on failure. 
*/

ADP_DIRENT_ID adpReaddir
    (
    ADP_DIR_ID      pDir       /* pointer to directory to read */
    )
    {
    if (pAdpIoFuncTbl->adp_readdir != NULL)
        return ((ADP_DIRENT_ID)pAdpIoFuncTbl->adp_readdir (pDir));    

    adpErrnoSet (S_adpIoLib_READDIR_FUNCPTR_NULL);
    return (NULL);         
    }

/*******************************************************************************
* 
* adpClosedir - call user specific closedir routine
*
* This routine calls user specific `closedir' routine if installed, otherwise
* sets the error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpClosedir
    (
    ADP_DIR_ID      pDir       /* pointer to directory to close */
    )
    {
    if (pAdpIoFuncTbl->adp_closedir != NULL)
        return (pAdpIoFuncTbl->adp_closedir (pDir));    

    adpErrnoSet (S_adpIoLib_CLOSEDIR_FUNCPTR_NULL);
    return (ADP_IO_ERROR);         
    }

/*******************************************************************************
* 
* adpTruncate - call user specific truncate routine
*
* This routine calls user specific `truncate' routine if installed, otherwise
* sets the error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpTruncate
    (
    char *      name,   /* file name */
    unsigned long   length  /* file size in bytes */
    )
    {
    if (pAdpIoFuncTbl->adp_truncate != NULL)
        return (pAdpIoFuncTbl->adp_truncate (name, length));    

    adpErrnoSet (S_adpIoLib_TRUNCATE_FUNCPTR_NULL);
    return (ADP_IO_ERROR);             
    }

/*******************************************************************************
* 
* adpFileTruncate - call user specific ftruncate routine
*
* This routine calls user specific `ftruncate' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpFileTruncate
    (
    ADP_FILE_ID     fd,     /* file name */
    unsigned long   length  /* file size in bytes */
    )
    {
    if (pAdpIoFuncTbl->adp_ftruncate != NULL)
        return (pAdpIoFuncTbl->adp_ftruncate (fd, length));    

    adpErrnoSet (S_adpIoLib_FTRUNCATE_FUNCPTR_NULL);
    return (ADP_IO_ERROR);             
    }

/*******************************************************************************
* 
* adpFileEOF - call user specific feof routine
*
* This routine calls user specific `feof' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

STATUS adpFileEOF
    (
    ADP_FILE_PTR    filePtr /* file pointer */
    )
    {
    if (pAdpIoFuncTbl->adp_feof != NULL)
        return (pAdpIoFuncTbl->adp_feof (filePtr));    

    adpErrnoSet (S_adpIoLib_FEOF_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpGetc - call user specific getc routine
*
* This routine calls user specific `getc' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpGetc
    (
    ADP_FILE_PTR filePtr /* file pointer */
    )
    {
    if (pAdpIoFuncTbl->adp_getc != NULL)
        return (pAdpIoFuncTbl->adp_getc (filePtr));    

    adpErrnoSet (S_adpIoLib_GETC_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpSetBuf - call user specific setbuf routine
*
* This routine calls user specific `setbuf' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

void adpSetBuf
    (
    ADP_FILE_PTR filePtr,
    char * pBuf
    )
    {
    if (pAdpIoFuncTbl->adp_setbuf != NULL)
        pAdpIoFuncTbl->adp_setbuf (filePtr,pBuf);
    else
        adpErrnoSet (S_adpIoLib_SETBUF_FUNCPTR_NULL);
    }

/*******************************************************************************
* 
* adpMkTemp - call user specific mktemp routine
*
* This routine calls user specific `mktemp' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpMkTemp
    (
    char * pTemplate
    )
    {
    if (pAdpIoFuncTbl->adp_mktemp != NULL)
        return (pAdpIoFuncTbl->adp_mktemp (pTemplate));    

    adpErrnoSet (S_adpIoLib_MKTEMP_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpVarFilePrintf - call user specific vfprintf routine
*
* This routine calls user specific `vfprintf' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpVarFilePrintf
    (
    ADP_FILE_PTR  filePtr, /* file pointer */
    const char    * fmt,   /* format of the string */
    va_list ap             /* variable arguments */
    )
    {
    if (pAdpIoFuncTbl->adp_vfprintf != NULL)
        return (pAdpIoFuncTbl->adp_vfprintf (filePtr,fmt,ap));    

    adpErrnoSet (S_adpIoLib_VFPRINTF_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpFileFlush - call user specific fflush routine
*
* This routine calls user specific `fflush' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpFileFlush
    (
    ADP_FILE_PTR  filePtr  /* file pointer */
    )
    {
    if (pAdpIoFuncTbl->adp_fflush != NULL)
        return (pAdpIoFuncTbl->adp_fflush (filePtr));    

    adpErrnoSet (S_adpIoLib_FFLUSH_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpFilePutc - call user specific fputc routine
*
* This routine calls user specific `fputc' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpFilePutc
    (
    int  c,      
    ADP_FILE_PTR  filePtr /* file pointer */
    )
    {
    if (pAdpIoFuncTbl->adp_fputc != NULL)
        return (pAdpIoFuncTbl->adp_fputc (c,filePtr));    

    adpErrnoSet (S_adpIoLib_FPUTC_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpPutc - call user specific putc routine
*
* This routine calls user specific `putc' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpPutc
    (
    int  c,      
    ADP_FILE_PTR  filePtr /* file pointer */
    )
    {
    if (pAdpIoFuncTbl->adp_putc != NULL)
        return (pAdpIoFuncTbl->adp_putc (c,filePtr));    

    adpErrnoSet (S_adpIoLib_PUTC_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpFileError - call user specific ferror routine
*
* This routine calls user specific 'ferror' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpFileError
    (
    ADP_FILE_PTR  filePtr /* file pointer */
    )
    {
    if (pAdpIoFuncTbl->adp_ferror != NULL)
        return (pAdpIoFuncTbl->adp_ferror (filePtr));    

    adpErrnoSet (S_adpIoLib_FERROR_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpPutChar - call user specific putchar routine
*
* This routine calls user specific 'putchar' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

int adpPutChar
    (
    int c
    )
    {
    if (pAdpIoFuncTbl->adp_putchar != NULL)
        return (pAdpIoFuncTbl->adp_putchar (c));    

    adpErrnoSet (S_adpIoLib_PUTCHAR_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }

/*******************************************************************************
* 
* adpWritev - call user specific writev routine
*
* This routine calls user specific 'writev' routine if installed, otherwise
* sets error number to FUNCPTR_NULL and returns ADP_IO_ERROR. 
*
* RETURNS: ADP_IO_OK on success and ADP_IO_ERROR on failure. 
*/

ssize_t adpWritev
    (
    ADP_SOCKET_ID  sock,
    const struct iovec *iov,
    int iovcnt
    )
    {
    if (pAdpIoFuncTbl->adp_writev != NULL)
        return (pAdpIoFuncTbl->adp_writev (sock,iov,iovcnt));    

    adpErrnoSet (S_adpIoLib_WRITEV_FUNCPTR_NULL);
    return (ADP_IO_ERROR);
    }
