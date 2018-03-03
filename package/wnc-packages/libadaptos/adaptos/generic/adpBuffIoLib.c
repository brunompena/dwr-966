/* adpBuffIoLib.c - Buffer based file I/O routines */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------

01a,12sep07,san  created.
*/

/*
DESCRIPTION

This library provides Buffer based  file I/O hook for adaptos I/O library.

INCLUDES: adaptos.h.

SEE ALSO: 
*/

/* includes */
#define __INCadpIoLibH

#include "adaptos.h"

/* globals */
static int file_descriptor;
FILE_STRUCT * pFileBuffer[MAX_IO_OPENFILES];

/* forward declarations */
LOCAL int _buffIofread(char *,int,int,FILE_STRUCT *);
LOCAL int _buffIofwrite(const char *,int,int,FILE_STRUCT *);
LOCAL FILE_STRUCT * _buffIofopen(char *,char *);
LOCAL int _buffIofclose(FILE_STRUCT *);
LOCAL int _buffIofseek(FILE_STRUCT *, int, int);
LOCAL char * _buffIofgets(char *, int, FILE_STRUCT *);
LOCAL int _buffIofileno(FILE_STRUCT *);
LOCAL int _buffIofeof(FILE_STRUCT *);
LOCAL int _buffIoftell(FILE_STRUCT *);
LOCAL int _buffIofputs(const char *, FILE_STRUCT *);
LOCAL int _buffIofprintf(FILE_STRUCT *, const char *, ...);
LOCAL STATUS _buffIoclose (ADP_FILE_ID);
LOCAL int _buffIoread(ADP_FILE_ID, void *, size_t);
LOCAL int _buffIowrite(ADP_FILE_ID, const void *, size_t);
LOCAL int _buffIogetc(FILE_STRUCT *);
LOCAL int _buffIosetbuf(FILE_STRUCT *, char *);
LOCAL int _buffIomktemp(const char * ); 
LOCAL int _buffIovfprintf(FILE_STRUCT *,const char *,va_list);
LOCAL int _buffIofflush(FILE_STRUCT *);
LOCAL int _buffIofputc(int,FILE_STRUCT *);
LOCAL int _buffIoputc(int,FILE_STRUCT *);
LOCAL int _buffIoferror(FILE_STRUCT *);
LOCAL int _buffIoputchar(int);
LOCAL ssize_t _buffIowritev(ADP_SOCKET_ID,const struct iovec *,int);

/* extern */
extern int adpBufferFileTableInit(void);

ADP_IO_FUNCTBL    adpBuffIoFuncTbl =
    {
    NULL,
    NULL,
    _buffIoclose,
    (ADP_FILE_PTR (*)(const char *,const char *))_buffIofopen,
    NULL,
    (int (*)(ADP_FILE_PTR ))_buffIofclose,
    _buffIoread,
    _buffIowrite,
    (int (*)(void *,size_t,size_t,ADP_FILE_PTR))_buffIofread,
    (int (*)(const void *,size_t,size_t,ADP_FILE_PTR))_buffIofwrite,
    NULL,
    (char * (*)(char *,int,ADP_FILE_PTR))_buffIofgets,
    (int (*)(const char *,ADP_FILE_PTR))_buffIofputs,
    (int (*)(ADP_FILE_PTR,const char *,...))_buffIofprintf,
    NULL,
    NULL,
    (int (*)(ADP_FILE_PTR,long,int))_buffIofseek,
    NULL,
    (long (*)(ADP_FILE_PTR))_buffIoftell,
    NULL,
    NULL,
    NULL,
    (int (*)(ADP_FILE_PTR))_buffIofileno,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    _buffIofeof,
    _buffIogetc,
    _buffIosetbuf,
    _buffIomktemp,
    _buffIovfprintf,
    _buffIofflush,
    _buffIofputc,
    _buffIoputc,
    _buffIoferror,
    _buffIoputchar
    };

/*******************************************************************************
*
* adpBuffIoLibInstall - intialize adaptos IO hook structure 
*
* This routine initilizes adaptos file IO hook structure .
*
* RETURNS: pointer ADP_IO_FUNCTBL
*/

ADP_IO_FUNCTBL * adpBuffIoLibInstall
    (
    void * pArg
    )
    {
    adpBufferFileTableInit();    
    return (&adpBuffIoFuncTbl);
    }

/*******************************************************************************
*
* _buffIoread - 
*
* This routine read data from file buffer of given file descriptor.
*
* RETURNS: No. of data elements read successfully
*/
     
LOCAL int _buffIoread
    (
    ADP_FILE_ID     fd,       /* file descriptor from which to read*/
    void *      pBuffer,   /* pointer to buffer to receive */ 
    size_t      maxbytes  /* maximum number of bytes to read */
    )
    {
#if defined(HAVE_NEED_ADP_HANDLER_FOR_SOCKET_AND_FILE_DESC)    
    return (readsocket (fd, (char *)pBuffer, maxbytes));  
#else
    return (read (fd, (char *)pBuffer, maxbytes));   
#endif 
    }

/*******************************************************************************
*
* _buffIowrite - 
*
* This routine write data to file buffer of given file descriptor.
*
* RETURNS: No. of data elements write successfully
*/

LOCAL int _buffIowrite
    (
    ADP_FILE_ID     fd,      /* file descriptor to write to*/
    const void *          pBuffer,  /* pointer to buffer contating data to */
                                 /* be written to fd */
    size_t   nbytes   /* number of bytes to write */
    )
    {
#if defined(HAVE_NEED_ADP_HANDLER_FOR_SOCKET_AND_FILE_DESC)    
    return (writesocket (fd, (char *)pBuffer, nbytes));
#else
    return (write (fd, (char *)pBuffer, nbytes));
#endif 
    }

/*******************************************************************************
*
* _buffIoclose  - 
*
* This routine closes a file descriptor
*
* RETURNS: 0 on Success  OR -1 on Failure
*/

LOCAL STATUS _buffIoclose
    (
    ADP_FILE_ID fd     /* file descriptor to write to */
    )
    {
#if defined(HAVE_NEED_ADP_HANDLER_FOR_SOCKET_AND_FILE_DESC)    
    return (closesocket (fd));
#else
    return (close (fd)); 
#endif 
    }

/*******************************************************************************
*
*  _buffIofprintf - 
*       
* This routine produce output according to a format
*       
* RETURNS: return the number of characters printed. 
*/

LOCAL int _buffIofprintf
    (
    FILE_STRUCT * pFileStruct,
    const char * pFmt,...
    )
    {
    char * arg[10];        
    va_list args;
    int i;
    va_start(args, pFmt);
    for (i=0 ; i<10 ; i++)
        arg[i] = (char *)va_arg(args,char *);
    if (pFileStruct == stderr || pFileStruct == stdout)   
    return (printf(pFmt,arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],
           arg[7],arg[8],arg[9]));
    else
        printf("Not yet Implemented\n");
    }

/*******************************************************************************
*
* _buffIofputs - 
*
* This routine writes the string  pBuff to stdout.
*
* RETURNS: 0
*/

LOCAL int _buffIofputs
    (
    const char  * pBuff,
    FILE_STRUCT * pFileStruct
    )
    {
    if (pFileStruct == stderr || pFileStruct == stdout) 
        printf("%s",pBuff); 
    else
        printf("Not yet Implemented\n");    
    return (0);
    }

/*******************************************************************************
*
* _buffIofread - 
*
* This routine read data from file buffer.
*
* RETURNS: No. of data elements read successfully
*/

LOCAL int _buffIofread
    (
    char * pBuff,
    int size,
    int nmemb,
    FILE_STRUCT * pFileStruct
    )
    {
    int count=0;
    if((pFileStruct->mode & (_READ | _RW))==0)
        return 0;
     count=(size*nmemb);
    if(count > pFileStruct->wptr - pFileStruct->rptr)  
        return 0;
    while(count)
        {
        if(pFileStruct->rptr==(pFileStruct->base + BUFFERSIZE))
            {
            *pBuff = *pFileStruct->rptr;    
            count--; 
            break;  
            }   
        *pBuff++ = *pFileStruct->rptr++;
        count--; 	
        }  
    return ((size*nmemb-count)/size);  
    }

/*******************************************************************************
*
* _buffIofwrite - 
*
* This routine write data to file buffer. 
*
* RETURNS: No. of data elements write successfully
*/

LOCAL int _buffIofwrite
    (
    const char * pBuff, 
    int size,
    int nmemb,
    FILE_STRUCT * pFileStruct
    )
    {
    int count;    
    if((pFileStruct->mode & (_WRITE | _RW ))==0)
        return -1;   
    count=(size*nmemb);
    if(count > (pFileStruct->base + BUFFERSIZE) - pFileStruct->wptr)
        return -1;
    while(count) 
        {
        if(pFileStruct->wptr==(pFileStruct->base + BUFFERSIZE - 1))
            {
            *pFileStruct->wptr = _EOF;	 
            break;
            }	 
        *pFileStruct->wptr++ = *pBuff++;
        count--; 	
        }  
    return ((size*nmemb-count)/size);  
    } 

/*******************************************************************************
*
* _buffIofopen - 
*
* This routine associates a stream for the file whose name is the string pointed
* to by name.
*
* RETURNS: pointer a stream upon successful completion or NULL on failure
*/

LOCAL FILE_STRUCT * _buffIofopen
    (
    char * pFileName,
    char * pMode
    )
    {
    unsigned int index=0,file_found=0;  

    /* check file-mode */
    if(strcmp(pMode,"r")!=0 && strcmp(pMode,"r+")!=0 && strcmp(pMode,"w")!=0 )
        return NULL;
    if(*pMode == 'r')
        {
        for(index=0;index<MAX_IO_OPENFILES;index++) 
            if((pFileBuffer[index]->mode & (_READ | _RW)) != 0)
                { 
                /* check whetherfile is present or not in list of exist files */
                if(strcmp(pFileBuffer[index]->fname,pFileName)==0) 
                    {
                    file_found = 1;
                    break; 
                    }
                }
        if((index >= MAX_IO_OPENFILES) && strcmp(pMode,"r") == 0)
            return NULL;  
        }     
    if(file_found == 1 && *pMode == 'r')
        {
        pFileBuffer[index]->file_count++; /* update no of instance for file */
        pFileBuffer[index]->rptr=pFileBuffer[index]->base;
        return pFileBuffer[index];
        }        
    for(index=0;index<MAX_IO_OPENFILES;index++) 
        if((pFileBuffer[index]->mode & _UNBUF) != 0)
            break;        
    if(index >= MAX_IO_OPENFILES)
        return NULL;	  	
    if((pFileBuffer[index]->base = (char *)malloc(BUFFERSIZE))==NULL) 
        return NULL; 
    memset(pFileBuffer[index]->base,0,BUFFERSIZE);
    strcpy(pFileBuffer[index]->fname , pFileName); 
    pFileBuffer[index]->rptr=pFileBuffer[index]->base;
    pFileBuffer[index]->wptr=pFileBuffer[index]->base;
    pFileBuffer[index]->fd  = file_descriptor++; 	
    pFileBuffer[index]->file_count = 1;
    pFileBuffer[index]->mode=(strcmp(pMode,"r+")==0) ? _RW : _WRITE;
    return pFileBuffer[index];  
    }   

/*******************************************************************************
*
* _buffIofclose - 
*
* This routine flush the stream pointed to by fp.
*
* RETURNS: 0 
*/

LOCAL int _buffIofclose
    (
    FILE_STRUCT * pFileStruct
    )
    {
    if (pFileStruct != NULL)
        { 
        pFileStruct->file_count--; 
        pFileStruct->flag = _UNBUF; 
        return 0;
        }
    return -1;  
    }


/*******************************************************************************
*
* _buffIofseek - 
*
* This routine set the position indicator for the stream pointed to by stream
*
* RETURNS: 0 or -1
*/

LOCAL int _buffIofseek
    (
    FILE_STRUCT * pFileStruct, 
    int offset,
    int whence
    )
    {
    if(whence == SEEK_CUR)   
        pFileStruct->rptr = pFileStruct->rptr + offset ;
    else if (whence == SEEK_SET)
        pFileStruct->rptr = pFileStruct->base + offset;
    return 0;
    }

/*******************************************************************************
*
* _buffIofgets - 
*
* This routine reads in at most one less than size characters from stream
*
* RETURNS: buff on success,and NULL on error
*/

LOCAL char * _buffIofgets
    (
    char * pBuff,
    int size,
    FILE_STRUCT * pFileStruct
    )  
    {
    char * start = pBuff;   
    int count=0;  
    if((pFileStruct->mode & (_READ | _RW))==0)
        return NULL;
    count=size-1;
    while(pFileStruct->rptr != pFileStruct->wptr && *pFileStruct->rptr != '\n'
          && count != 0)
        {
        *pBuff++ = *pFileStruct->rptr++;
        count--;
        }	 
    if(*pFileStruct->rptr == '\n')
        {
        pFileStruct->rptr++;	   
        if(*pFileStruct->rptr == '\0')   
            pFileStruct->rptr++;
        }	 
    if(pFileStruct->rptr == pFileStruct->wptr)
        pFileStruct->flag = _EOF; 
    *pBuff = '\0';
    return start;   
    }
   
/*******************************************************************************
*
* _buffIofileno - 
*
* This routine examines the  argument  stream  and  returns  its
* integer descriptor.
*
* RETURNS: value of integer descriptor of stream
*/

LOCAL int _buffIofileno
    (
    FILE_STRUCT * pFileStruct
    )
    {
    return (pFileStruct->fd);
    }	


/*******************************************************************************
*
* _buffIofeof - 
*
* This routine test the end-of-file  indicator  for  the  stream
* pointed to by stream
*
* RETURNS: 1 or 0
*/

LOCAL int _buffIofeof
    (
    FILE_STRUCT * pFileStruct
    )
    {
    if ((pFileStruct->flag & _EOF) != 0 )
        return 1;	 
    return 0;
    }

/*******************************************************************************
*
* _buffIoftell - 
*
* This routine value of position pointer of the  stream
* pointed to by stream
*
* RETURNS: value of position pointer from start
*/

LOCAL int _buffIoftell
    (
    FILE_STRUCT * pFileStruct
    )
    {
    if (pFileStruct == NULL)
        return 0;   
    return ((int)(pFileStruct->rptr-pFileStruct->base)); 
    }

/*******************************************************************************
*
* _buffIogetc - reads the next character from stream and returns it as an
*               unsigned char cast to an int
*
* RETURNS: next character from stream , EOF on end of file or error
*/

LOCAL int _buffIogetc
    (
    FILE_STRUCT * pFileStruct
    )
    {
    return ( pFileStruct->rptr < pFileStruct->wptr ? * pFileStruct->rptr++:020);
    }

/*******************************************************************************
*
* _buffIosetbuf - 
*
* This routine value of position pointer of the  stream
* pointed to by stream
*
* RETURNS: value of position pointer from start
*/

LOCAL int _buffIosetbuf
    (
    FILE_STRUCT * pFileStruct, 
    char * pBuffer
    )
    {
    return 0;
    }

/*******************************************************************************
*
* _buffIomktemp - 
*
* This routine value of position pointer of the  stream
* pointed to by stream
*
* RETURNS: value of position pointer from start
*/

LOCAL int _buffIomktemp
    (
    const char * pTemplate
    )
    {
    return 0;
    }

/*******************************************************************************
*       
* _buffIovfprintf - equivalent to  fprintf except that they are called with 
*                   va_list instead of a variable number of arguments
*
* RETURNS: 0
*/

LOCAL int _buffIovfprintf
    (
    FILE_STRUCT * pFileStruct,       /* file pointer */
    const char  * fmt,               /* format of the string */  
    va_list       ap                 /* variable arguments */  
    ) 
    {
    char * arg[10]; 
    int i;
    for(i=0;i<10;i++)
        arg[i] = (char *)va_arg(ap,char *);
    printf(fmt,arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],arg[7],arg[8],
           arg[9]);  
    return(0);
    }

LOCAL int _buffIofflush
    (
    FILE_STRUCT * pFileStruct
    )
    {
    return 0;
    }

LOCAL int _buffIofputc
    (
    int c,
    FILE_STRUCT * pFileStruct
    )
    {
    if (pFileStruct == stderr || pFileStruct == stdout) 
        printf("%c",c);
    else
        printf("Not Yet Implemented\n");  
    return 0;
    }

LOCAL int _buffIoputc
    (
    int c,
    FILE_STRUCT * pFileStruct
    )
    {
    if (pFileStruct == stderr || pFileStruct == stdout) 
        printf("%c",c);
    else
        printf("Not Yet Implemented\n");  
    return 0;
    }

LOCAL int _buffIoferror
    (
    FILE_STRUCT * pFileStruct
    )
    {
    return 0;
    }

LOCAL int _buffIoputchar
    (
    int c
    )
    {
    printf("%c",c);
    return 0;
    }

/*******************************************************************************
*
* adpSockWritev - writes at most iovcnt blocks described by vector to the socket
*                 associated with the socket descriptor sock.
*
* RETURNS: On success returns the number of bytes written.  On error, -1 
*/

ssize_t _buffIowritev
    (
    ADP_SOCKET_ID  sock,
    const struct iovec *iov,
    int iovcnt
    )
    {
    int retVal = -1 , i=0 ;
    if ( iovcnt > 0 &&  iovcnt <= IOV_MAX )
        {
        for ( i=0 ; i < iovcnt ; i++ )
            { 
            if (iov != NULL)
                {
#if defined(HAVE_NEED_ADP_HANDLER_FOR_SOCKET_AND_FILE_DESC)                
                retVal += writesocket(sock,iov->iov_base,iov->iov_len);
#else
                retVal += write(sock,iov->iov_base,iov->iov_len);
#endif /* HAVE_NEED_ADP_HANDLER_FOR_SOCKET_AND_FILE_DESC */                
                iov++;
                }
            }
        }
    return retVal;
    }

