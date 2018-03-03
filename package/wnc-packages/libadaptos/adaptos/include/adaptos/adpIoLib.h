/* adpIoLib.h -  adaptos file IO library header */

/* Copyright (c) 2005 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01k,09jul08,rnp  changed prototypes of adpSetBuf() and adpMkTemp()
01j,06oct07,san  change adpFileSeek prototype for 
                 fixing warning & added prototype of 
                 wrapper added in IO table.  
01i,05sep05,rnp  added support for lynxOS
01h,07may05,sam  restructured adaptos.
01g,20aug03,ksn  fixed lseek prototype
01f,19aug03,gnm  fixed ioctl prototype
01e,16aug03,ksn  fixed function prototypes
01d,08jul03,ksn  fixed macro fprintf.
01c,21jun03,ksn  fixed return type of fgets.
01b,20may03,ksn  fixed fprintf definition.
01a,15apr03,ksn  created.
*/

#ifndef __INCadpIoLibH
#define __INCadpIoLibH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "adaptos.h"
    
typedef ADP_IO_FUNCTBL * (*ADP_IO_INSTALL_FUNCPTR)(void *);

ADP_FILE_ID     adpCreat (const char * , int , int );
ADP_FILE_ID     adpOpen (const char * , int , int );
STATUS          adpClose (ADP_FILE_ID );
ADP_FILE_PTR    adpFileOpen (const char * , const char * );
int             adpFileClose (ADP_FILE_PTR );
ADP_FILE_PTR    adpFdopen (int , const char * );
int             adpRead (ADP_FILE_ID , char * , size_t );
int             adpWrite (ADP_FILE_ID , const char * , unsigned long );
int             adpFileRead (void * , size_t , size_t , ADP_FILE_PTR);
int             adpFileWrite (const void * , size_t , size_t , ADP_FILE_PTR );
int             adpIoctl (int , int , void *);
char *          adpFileGets (char * , int , ADP_FILE_PTR );
int             adpFilePuts (const char * , ADP_FILE_PTR );
int             adpFilePrintf (ADP_FILE_PTR , const char * , ...); 
STATUS          adpFileStat (int , struct stat * );
STATUS          adpFileSeek (ADP_FILE_PTR ,long , int );
off_t           adpLseek (ADP_FILE_ID ,off_t , int );
STATUS          adpStat  (const char *, struct stat *);
long            adpFileTell (ADP_FILE_PTR );
STATUS          adpRemove (const char * );
STATUS          adpUnlink (const char * );
BOOL            adpIsatty (ADP_FILE_ID );
int             adpFileno (ADP_FILE_PTR );
STATUS          adpChmod (char * , UINT );
STATUS          adpChown (const char * , int , int );
STATUS          adpChdir (char * );
STATUS          adpMkdir (char* , UINT );
STATUS          adpRmdir (char* );
ADP_DIR_ID      adpOpendir (const char * );
ADP_DIRENT_ID   adpReaddir (ADP_DIR_ID );
STATUS          adpClosedir (ADP_DIR_ID );
STATUS          adpTruncate (char * , unsigned long );
STATUS          adpFileTruncate (ADP_FILE_ID , unsigned long );
STATUS          adpFileEOF (ADP_FILE_PTR);
int             adpGetc(ADP_FILE_PTR);
void            adpSetBuf(ADP_FILE_PTR, char *);
int             adpMkTemp(char * );
int             adpVarFilePrintf(ADP_FILE_PTR,const char *,va_list);
int             adpFileFlush(ADP_FILE_PTR);
int             adpFilePutc(int,ADP_FILE_PTR);
int             adpPutc(int,ADP_FILE_PTR);
int             adpFileError(ADP_FILE_PTR);
int             adpPutChar(int);
ssize_t         adpWritev(ADP_FILE_ID,const struct iovec *,int);

/* IO routine redirection */
#ifndef HAVE_ADP_CREATE
#undef  creat
#define creat(X, Y, Z)          adpCreat (X, Y, Z)
#define HAVE_ADP_CREATE
#endif

#ifndef HAVE_ADP_OPEN
#undef  open
#define open(X, Y, Z)           adpOpen (X, Y, Z)
#define HAVE_ADP_OPEN
#endif

#ifndef HAVE_ADP_FDOPEN
#undef  fdopen
#define fdopen(X, Y)            adpFdopen (X, Y)
#define HAVE_ADP_FDOPEN
#endif

#ifndef HAVE_ADP_CLOSE
#undef  close
#define close(X)                adpClose (X)
#define HAVE_ADP_CLOSE
#endif

#ifndef HAVE_ADP_FOPEN
#undef  fopen
#define fopen(X, Y)             adpFileOpen (X, Y)
#define HAVE_ADP_FOPEN
#endif

#ifndef HAVE_ADP_FCLOSE
#undef  fclose
#define fclose(X)               adpFileClose (X)
#define HAVE_ADP_FCLOSE
#endif

#ifndef HAVE_ADP_READ
#undef  read
#define read(X, Y, Z)           adpRead (X, Y, Z)
#define HAVE_ADP_READ
#endif

#ifndef HAVE_ADP_WRITE
#undef  write
#define write(X, Y, Z)          adpWrite (X, Y, Z)
#define HAVE_ADP_WRITE
#endif

#ifndef HAVE_ADP_FREAD
#undef  fread
#define fread(W, X, Y, Z)       adpFileRead (W, X, Y, Z)
#define HAVE_ADP_FREAD
#endif

#ifndef HAVE_ADP_FWRITE
#undef  fwrite
#define fwrite(W, X, Y, Z)      adpFileWrite (W, X, Y, Z)
#define HAVE_ADP_FWRITE
#endif

#ifndef HAVE_ADP_IOCTL
#undef  ioctl
#define ioctl(X, Y, Z)          adpIoctl (X, Y, Z)
#define HAVE_ADP_IOCTL
#endif

#ifndef HAVE_ADP_FGETS
#undef  fgets
#define fgets(X, Y, Z)          adpFileGets (X, Y, Z)
#define HAVE_ADP_FGETS
#endif

#ifndef HAVE_ADP_FPUTS
#undef  fputs
#define fputs(X, Z)             adpFilePuts (X, Z)
#define HAVE_ADP_FPUTS
#endif

#ifndef HAVE_ADP_FPRINTF
#undef  fprintf
#define fprintf                 adpFilePrintf
#define HAVE_ADP_FPRINTF
#endif

#ifndef HAVE_ADP_STAT
#define stat(X, Y)              adpStat (X, Y)
#define HAVE_ADP_STAT
#endif

#ifndef HAVE_ADP_LSTAT
#undef  lstat
#define lstat(X, Y)             adpStat (X, Y) 
#define HAVE_ADP_LSTAT
#endif

#ifndef HAVE_ADP_FSTAT
#undef  fstat
#define fstat(X, Y)             adpFileStat (X, Y)
#define HAVE_ADP_FSTAT
#endif

#ifndef HAVE_ADP_FSEEK
#undef  fseek
#define fseek(X, Y, Z)          adpFileSeek (X, Y, Z)
#define HAVE_ADP_FSEEK
#endif

#ifndef HAVE_ADP_LSEEK
#undef  lseek
#define lseek(X, Y, Z)          adpLseek (X, Y, Z)
#define HAVE_ADP_LSEEK
#endif

#ifndef HAVE_ADP_FTELL
#undef  ftell
#define ftell(X)                adpFileTell (X)
#define HAVE_ADP_FTELL
#endif

#ifndef HAVE_ADP_REMOVE
#undef  remove
#define remove(X)               adpRemove (X)
#define HAVE_ADP_REMOVE
#endif

#ifndef HAVE_ADP_UNLINK
#undef  unlink
#define unlink(X)               adpUnlink (X)
#define HAVE_ADP_UNLINK
#endif

#ifndef HAVE_ADP_ISATTY
#undef  isatty
#define isatty(X)               adpIsatty (X)
#define HAVE_ADP_ISATTY
#endif

#ifndef HAVE_ADP_FILENO
#undef  fileno
#define fileno(X)               adpFileno (X)
#define HAVE_ADP_FILENO
#endif

#ifndef HAVE_ADP_CHMOD
#undef  chmod
#define chmod(X, Y)             adpChmod (X, Y) 
#define HAVE_ADP_CHMOD
#endif

#ifndef HAVE_ADP_CHOWN
#undef  chown
#define chown(X, Y, Z)          adpChown (X, Y, Z)  
#define HAVE_ADP_CHOWN
#endif

#ifndef HAVE_ADP_CHDIR
#undef  chdir
#define chdir(X)                adpChdir (X)
#define HAVE_ADP_CHDIR
#endif

#ifndef HAVE_ADP_RMDIR
#undef  rmdir
#define rmdir(X)                adpRmdir (X)
#define HAVE_ADP_RMDIR
#endif

#ifndef HAVE_ADP_OPENDIR
#undef  opendir
#define opendir(X)              adpOpendir (X) 
#define HAVE_ADP_OPENDIR
#endif

#ifndef HAVE_ADP_READDIR
#undef  readdir
#define readdir(X)              adpReaddir (X)
#define HAVE_ADP_READDIR
#endif

#ifndef HAVE_ADP_CLOSEDIR
#undef  closedir
#define closedir(X)             adpClosedir (X)
#define HAVE_ADP_CLOSEDIR
#endif

#ifndef HAVE_ADP_TRUNCATE
#undef  trunctae
#define truncate(X, Y)          adpTruncate (X, Y)
#define HAVE_ADP_TRUNCATE
#endif

#ifndef HAVE_ADP_FILE_TRUNC
#undef  ftrunctae
#define ftruncate(X, Y)         adpFileTruncate (X, Y)
#define HAVE_ADP_FILE_TRUNC
#endif

#ifndef HAVE_ADP_FEOF
#undef  feof
#define feof(X)                 adpFileEOF (X)
#define HAVE_ADP_FEOF
#endif

#ifndef HAVE_ADP_GETC
#undef  getc
#define getc(X)                 adpGetc (X) 
#define HAVE_ADP_GETC
#endif

#ifndef HAVE_ADP_SETBUF
#undef  setbuf
#define setbuf(X, Y)            adpSetBuf (X, Y) 
#define HAVE_ADP_SETBUF
#endif

#ifndef HAVE_ADP_MKTEMP
#undef  mktemp
#define mktemp(X)               adpMkTemp (X) 
#define HAVE_ADP_MKTEMP
#endif

#ifndef HAVE_ADP_VFPRINTF
#undef  vfprintf
#define vfprintf                adpVarFilePrintf
#define HAVE_ADP_VFPRINTF
#endif 

#ifndef HAVE_ADP_FFLUSH
#undef  fflush
#define fflush                  adpFileFlush
#define HAVE_ADP_FFLUSH
#endif 

#ifndef HAVE_ADP_FPUTC
#undef  fputc
#define fputc                   adpFilePutc
#define HAVE_ADP_FPUTC  
#endif 

#ifndef HAVE_ADP_PUTC
#undef  putc
#define putc                    adpPutc
#define HAVE_ADP_PUTC
#endif 

#ifndef HAVE_ADP_FERROR
#undef  ferror
#define ferror                  adpFileError
#define HAVE_ADP_FERROR
#endif

#ifndef HAVE_ADP_PUTCHAR
#undef  putchar
#define putchar                 adpPutChar
#define HAVE_ADP_PUTCHAR
#endif 

#ifndef HAVE_ADP_WRITEV
#undef  writev
#define writev                  adpWritev              
#define HAVE_ADP_WRITEV
#endif 

/* externs */
IMPORT STATUS adpIoLibInit (ADP_IO_INSTALL_FUNCPTR adpIoFuncTblInstallRtn,
                            void *  arg1);

/* imports */
IMPORT ADP_IO_FUNCTBL * adpBuffIoLibInstall (void *);
IMPORT ADP_IO_FUNCTBL   adpBuffIoFuncTbl;

#ifdef __cplusplus
}
#endif

#endif /* __INCadpIoLibH */
