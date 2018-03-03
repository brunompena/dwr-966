/* sqliteMgmtLib.h -   */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01b,26jan10,sam removed call to ADP_ERROR in DB_TABLE_VALUE_GET().
01a,28jun07,smv written.
*/

/*
DESCRIPTION

*/

#ifndef  SQLITEWNCLIB_H
#define  SQLITEWNCTLIB_H

/* includes */

#include "sqlite3.h"
#include "adaptos.h"
#include "adpListLib.h"
#include "adpModDebug.h"
/* defines */

#define PROGRAM_NAME_MAX_LEN 32
#define SQL_MAX_CMD_SIZE          1024

//If get data is NULL, then will string change to "". For avoid Web CGI happen crash.
#define DB_TABLE_VALUE_GET(ppQueryRes,nCols,rowIdx,colName,pData,errLbl) \
    do { \
     pData = dbRecordValueGet (ppQueryRes, nCols, rowIdx, colName); \
     if (pData == NULL) \
        pData = ""; \
     if (strcmp(pData,"noCols") == 0) \
        goto errLbl; \
    } while(0);

#define DB_TABLE_INT_VALUE_GET(ppQueryRes,nCols,rowIdx,colName,val, errLbl) \
    do { \
        char * pTmp; \
        DB_TABLE_VALUE_GET(ppQueryRes,nCols,rowIdx,colName,pTmp,errLbl) \
        val = atoi(pTmp); \
    } while (0);


//#define sqlCmdExec(x)            adpCmdExec(x, "/dev/null", 0)
#ifdef DEBUG
#define ADP_WNC_DBG(...) adp_wnc_print( __func__, __LINE__, __VA_ARGS__)
#else
#define ADP_WNC_DBG(...)
#endif
/* typedefs */

typedef struct
     {
     char   * pTblName;
     int   (* pCallBack)(void *, int, long long, char **, int, int, char **);
     } SQLITE_TBL_UPDATE_HANDLER;

typedef struct
    {
    char            progName[PROGRAM_NAME_MAX_LEN];
    int             nTries;
    unsigned int    sleepTimeInMs;
    } SQLITE_BUSY_HANDLER_ARG;
/*******************************************************************************
*
* dbRecordValueGet - gets the value of specified cell.
*
* This routine gets the value of specified cell
*
* RETURNS: pointer to cell data.
*          NULL on failure.
*
* ERRNO: N/A.
*
* SEE_ALSO:
*/

static inline char * dbRecordValueGet
    (
    char ** ppQueryRes,
    int     nCols,
    int     rowIdx,
    char  * pColName
    )
    {
    int idx;
    for (idx = 0; idx < nCols; ++idx)
         if (strcmp (ppQueryRes[idx], pColName) == 0)
             break;
    if (idx == nCols)
        return "noCols";

    return ppQueryRes[(rowIdx+1)*nCols + idx];
    }


/* imports */

extern char ** sqlite3QueryResGet (sqlite3 * pDB, const char * pDbName,
                                   const char * pQueryStr, int * pnCols,
                                   int * pnRows);

extern void sqlGetTablePrint (char ** ppQueryRes, int nCols, int nRows);
extern int  sqliteBusyHandler ( void * pData, int nTries);
extern void sqliteRemoveFunc (sqlite3 * pDB);
extern void sqliteRestoreFunc (sqlite3 * pDB);

#endif   /* !SQLITEWNCLib_H */
