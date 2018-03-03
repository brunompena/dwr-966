/* adpIntRamFs.c - ram-based filesystem for integrity */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,28jun08,rnp  written
*/

/* include files */
#include "adaptos.h"

/* defines */
#define ADP_RAMFS_PATH_LEN  128

/* externs */

/* imports */

/* globals */

/* locals */

/* forward declarations */

/*******************************************************************************
*
* adpInstallFile - install a file in the local file system
*
* RETURNS: ST_OK or ST_ERROR
*/
STATUS adpInstallFile
    (
    const char * pDst,           /* destination */
    const char * pBuffer,
    int    bufSize
    )
    {
    FILE * pFile = NULL;

    pFile = adpFileOpen(pDst, "w+");
    if(pFile == NULL)
        {
        printf("File Open Error : %s\n", pDst);
        return (ST_ERROR);
        }

    if(adpFileWrite(pBuffer, bufSize, 1, pFile) == -1)
        {
        printf("File Write Error : %s\n", pDst);
        fclose(pFile);
        return ST_ERROR;
        }

    fclose(pFile);

    return ST_OK;
    }
