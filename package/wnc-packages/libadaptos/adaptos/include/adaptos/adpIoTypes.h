/* adpIoTypes.h -  adaptos file IO types header */

/* Copyright (c) 2005 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01g,09jul08,rnp  changed prototypes of adp_setbuf, adp_mktemp to address
                 warnings on vxworks 5.5
01f,06oct07,san  added some IO routine in ADP_IO_FUNCTBL.
01e,09may05,sam  added protection for typedefs.
01d,19aug03,gnm  include phile.h for psos2x and fixed ioctl prototype
01c,16aug03,ksn  added ADP_DIR for psos and fixed function prototypes.
01b,21jun03,ksn  fixed retun type of fgets.
01a,15apr03,ksn  created.
*/

#ifndef __INCadpIoTypesH
#define __INCadpIoTypesH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ADP_STDIN       (0)
#define ADP_STDOUT      (1)
#define ADP_STDERR      (2)

/* typedefs */
#ifndef HAVE_ADP_FILE_ID
typedef int                 ADP_FILE_ID;
#define HAVE_ADP_FILE_ID
#endif

#ifndef HAVE_ADP_FILE_PTR
typedef FILE *              ADP_FILE_PTR;
#define HAVE_ADP_FILE_PTR
#endif

#ifndef HAVE_ADP_DIR_ID
typedef DIR *               ADP_DIR_ID;
#define HAVE_ADP_DIR_ID
#endif

#ifndef HAVE_ADP_DIRENT_ID
typedef struct dirent *     ADP_DIRENT_ID;
#define HAVE_ADP_DIRENT_ID
#endif

#define ADP_IO_ERROR                (-1)
#define ADP_IO_OK                   (0)

/* error codes */
#define S_adpIoLib_FUNCTBL_INVALID          (M_adpIoLib | 1)
#define S_adpIoLib_FUNCTBL_INSTALLRTN_INVALID       (M_adpIoLib | 2)

#define S_adpIoLib_CREAT_FUNCPTR_NULL           (M_adpIoLib | 3)
#define S_adpIoLib_OPEN_FUNCPTR_NULL            (M_adpIoLib | 4)
#define S_adpIoLib_CLOSE_FUNCPTR_NULL           (M_adpIoLib | 5)
#define S_adpIoLib_FOPEN_FUNCPTR_NULL           (M_adpIoLib | 6)
#define S_adpIoLib_FDOPEN_FUNCPTR_NULL          (M_adpIoLib | 7)
#define S_adpIoLib_FCLOSE_FUNCPTR_NULL          (M_adpIoLib | 8)
#define S_adpIoLib_READ_FUNCPTR_NULL            (M_adpIoLib | 9)
#define S_adpIoLib_WRITE_FUNCPTR_NULL           (M_adpIoLib | 10)
#define S_adpIoLib_FREAD_FUNCPTR_NULL           (M_adpIoLib | 11)
#define S_adpIoLib_FWRITE_FUNCPTR_NULL          (M_adpIoLib | 12)
#define S_adpIoLib_IOCTL_FUNCPTR_NULL           (M_adpIoLib | 13)
#define S_adpIoLib_FGETS_FUNCPTR_NULL           (M_adpIoLib | 14)
#define S_adpIoLib_FPUTS_FUNCPTR_NULL           (M_adpIoLib | 15)
#define S_adpIoLib_FPRINTF_FUNCPTR_NULL         (M_adpIoLib | 16)
#define S_adpIoLib_STAT_FUNCPTR_NULL            (M_adpIoLib | 17)
#define S_adpIoLib_FSTAT_FUNCPTR_NULL           (M_adpIoLib | 18)
#define S_adpIoLib_FSEEK_FUNCPTR_NULL           (M_adpIoLib | 19)
#define S_adpIoLib_LSEEK_FUNCPTR_NULL           (M_adpIoLib | 20)
#define S_adpIoLib_FTELL_FUNCPTR_NULL           (M_adpIoLib | 21)
#define S_adpIoLib_REMOVE_FUNCPTR_NULL          (M_adpIoLib | 22)
#define S_adpIoLib_UNLINK_FUNCPTR_NULL          (M_adpIoLib | 23)
#define S_adpIoLib_ISATTY_FUNCPTR_NULL          (M_adpIoLib | 24)
#define S_adpIoLib_FILENO_FUNCPTR_NULL          (M_adpIoLib | 25)
#define S_adpIoLib_CHMOD_FUNCPTR_NULL           (M_adpIoLib | 26)
#define S_adpIoLib_CHOWN_FUNCPTR_NULL           (M_adpIoLib | 27)
#define S_adpIoLib_CHDIR_FUNCPTR_NULL           (M_adpIoLib | 28)
#define S_adpIoLib_MKDIR_FUNCPTR_NULL           (M_adpIoLib | 29)
#define S_adpIoLib_RMDIR_FUNCPTR_NULL           (M_adpIoLib | 30)
#define S_adpIoLib_OPENDIR_FUNCPTR_NULL         (M_adpIoLib | 31)
#define S_adpIoLib_READDIR_FUNCPTR_NULL         (M_adpIoLib | 32)
#define S_adpIoLib_CLOSEDIR_FUNCPTR_NULL        (M_adpIoLib | 33)
#define S_adpIoLib_TRUNCATE_FUNCPTR_NULL        (M_adpIoLib | 34)
#define S_adpIoLib_FTRUNCATE_FUNCPTR_NULL       (M_adpIoLib | 35)
#define S_adpIoLib_FEOF_FUNCPTR_NULL            (M_adpIoLib | 36)
#define S_adpIoLib_GETC_FUNCPTR_NULL            (M_adpIoLib | 37)
#define S_adpIoLib_SETBUF_FUNCPTR_NULL          (M_adpIoLib | 38)
#define S_adpIoLib_MKTEMP_FUNCPTR_NULL          (M_adpIoLib | 39)
#define S_adpIoLib_VFPRINTF_FUNCPTR_NULL        (M_adpIoLib | 40)
#define S_adpIoLib_FFLUSH_FUNCPTR_NULL          (M_adpIoLib | 41)
#define S_adpIoLib_FPUTC_FUNCPTR_NULL           (M_adpIoLib | 42)
#define S_adpIoLib_PUTC_FUNCPTR_NULL            (M_adpIoLib | 43)
#define S_adpIoLib_FERROR_FUNCPTR_NULL          (M_adpIoLib | 44)
#define S_adpIoLib_PUTCHAR_FUNCPTR_NULL         (M_adpIoLib | 45)
#define S_adpIoLib_WRITEV_FUNCPTR_NULL          (M_adpIoLib | 46)

#ifndef HAVE_ADP_IO_FUNCTBL
typedef struct
    {
    ADP_FILE_ID     (*adp_creat) (const char * , int , int );
    ADP_FILE_ID     (*adp_open) (const char * , int , int );
    STATUS      (*adp_close) (ADP_FILE_ID );
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
    int         (*adp_getc) (ADP_FILE_PTR);
    void        (*adp_setbuf) (ADP_FILE_PTR, char *);
    int         (*adp_mktemp) (char * ); 
    int         (*adp_vfprintf) (ADP_FILE_PTR,const char *,va_list);
    int         (*adp_fflush) (ADP_FILE_PTR);
    int         (*adp_fputc) (int,ADP_FILE_PTR);
    int         (*adp_putc) (int,ADP_FILE_PTR);
    int         (*adp_ferror) (ADP_FILE_PTR);
    int         (*adp_putchar)(int);
    ssize_t     (*adp_writev)(ADP_FILE_ID,const struct iovec *,int);  
    } ADP_IO_FUNCTBL;
#define HAVE_ADP_IO_FUNCTBL
#endif


#ifdef __cplusplus
}
#endif

#endif /* __INCadpIoTypesH */
