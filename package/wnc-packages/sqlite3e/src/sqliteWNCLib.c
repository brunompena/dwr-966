/* sqliteWNCLib.c -  */
/*
 * Developer : Anita Wu (anita.wu@wnc.com.tw)
 * Date : 20/06/2012
 * All code (c)2012 WNC Wistron NeWeb Corp. all rights reserved
 */

/*
DESCRIPTION

*/

/* includes */


//#define DEBUG


#include "sqliteWNCLib.h"
#include "adaptos.h"

#ifdef UMI
#include "umi.h"
#include "evtDsptch.h"
#endif

/* defines */


//void sqlite3Error(sqlite3*, int, const char*,...);


#ifdef DEBUG
void adp_wnc_print    (
    const char *  function,
    int           line,
    char *        pszFormat,      /* format */
    ...                           /* variable args */
    )
{
	va_list             arg;
	char cmd[512];
	char fmt[256];

	{

		va_start (arg, pszFormat);
		vsprintf(fmt,pszFormat, arg);
		va_end (arg);

		sprintf(cmd,"echo \"<%s:%d> %s\"  >> /var/log/messages",function,line,fmt);
        sprintf(cmd,"echo \"<%s:%d> %s\" ",function,line,fmt);
		system(cmd);
        //sqlCmdExec(cmd);
		//adpMemPartFree(NULL, fmt);

	}



}
#endif

/* typedefs */
typedef struct
    {
    ADP_NODE        node;
    int             opCode;
    const char *    pDbName;
    const char *    pTblName;
    int             rowId;
    char **         ppQueryRes;
    int             nCols;
    } DB_UPDATE_NODE;

/* globals */

static sqlite3 * pSqlDB = NULL;

/* locals */
static ADP_LIST updateList;

/* imports */


/* forward declarations */


void sqliteUpdateFunc ( void * pData, int opCode, char const * pDbName,
                               char const * pTblName, sqlite_int64 rowId,
                              int nCols, char ** ppQueryRes );

int  sqliteCommitFunc (void * pData);
void sqliteRollbackFunc (void *pData);

/*******************************************************************************
*
* sqliteRemoveFunc - uninstall handlers to sqlite hooks
*
* This function removes the handlers installed during sqliteMgmtInit.
*
*/
void sqliteRemoveFunc
    (
    sqlite3 *       pDb
    )
    {
    sqlite3_update_hook (pDb, NULL, NULL);
    sqlite3_commit_hook (pDb, NULL, NULL);
    sqlite3_rollback_hook (pDb, NULL, NULL);
    }

/*******************************************************************************
*
* sqliteRestoreHooks - install backs the handlers to sqlite hooks
*
* This function install backs the handlers to sqlite hooks.
*
*/
void sqliteRestoreFunc
    (
    sqlite3 *       pDb
    )
    {
    sqlite3_update_hook (pDb, sqliteUpdateFunc, (void *)pDb);
    sqlite3_commit_hook (pDb, sqliteCommitFunc, (void *)pDb);
    sqlite3_rollback_hook (pDb, sqliteRollbackFunc, (void *)pDb);
    }



/*******************************************************************************
*
* sqliteUpdateFunc -handler to receive all DB updates
*
* This function receives all the DB updates and saves into a list. This list is
* later used to send notifcation to components when commit is done. In case of
* a rollback, list is reset to empty.
*
* RETURNS: N/A
*
*/

void sqliteUpdateFunc
    (
    void         * pData,
    int            opCode,
    char const   * pDbName,
    char const   * pTblName,
    sqlite3_int64   rowId,
    int            nCols,
    char **        ppQueryRes
    )
    {
    DB_UPDATE_NODE * pNode = sqlite3_malloc(sizeof (DB_UPDATE_NODE));

    //fprintf (stderr, "sqliteUpdateFunc test \n");

    if (pNode == NULL)
        {
        //sqlite3Error((sqlite3 *)pData, SQLITE_NOMEM,
        //            "sqliteTblUpdateHook:malloc failed\n");
        if (ppQueryRes)
            sqlite3_free_table (ppQueryRes);
        }
    else
        {
        pNode->opCode = opCode;
        pNode->pDbName = pDbName;
        pNode->pTblName = pTblName;
        pNode->rowId = (int) rowId;
        pNode->nCols = nCols;
        pNode->ppQueryRes = ppQueryRes;
        adpListNodeAppend (&updateList, (ADP_NODE *)pNode);

        ADP_WNC_DBG ("pTblName:%s,  opCode: %d, rowId: %d, \n", (char *) pNode->pTblName,opCode,pNode->rowId);
        if (ppQueryRes != NULL)
            ADP_WNC_DBG("ppQueryRes != NULL, rowId: %d",opCode,pNode->rowId);
        else
			ADP_WNC_DBG("ppQueryRes ==  NULL");

        }
    }

int sqliteMgmtInit
    (
    const char *    fileName,
    sqlite3 *       pDb
    )
    {
    sqlite3_update_hook (pDb, sqliteUpdateFunc, (void *)pDb);
    sqlite3_commit_hook (pDb, sqliteCommitFunc, (void *)pDb);
    sqlite3_rollback_hook (pDb, sqliteRollbackFunc, (void *)pDb);

#ifdef UMI
    umiInit();
#endif

    adpListInit (&updateList);

    return SQLITE_OK;
    }

/*******************************************************************************
*
* sqliteCommitFunc - handler to process the commit operation
*
* RETURNS: N/A
*
*/
int  sqliteCommitFunc
    (
    void * pData
    )
    {

    // parse table and do something
    sendNotification ();

    //ADP_WNC_DBG("freeUpdateList");
    freeUpdateList ();

    //fprintf(stderr,"send EVTDSPTCH_DB_UPDATE_END \n");
    umiIoctl (NULL, UMI_COMP_EVTDSPTCH, EVTDSPTCH_DB_UPDATE_END,
              NULL, 0, NULL, NULL, UMI_IOCTL_OPT_NO_RESULT, 1, 0);

    return 0;
    }

/*******************************************************************************
*
* sqliteRollbackHookHandler - handler to process the rollback operation
*
* RETURNS: N/A
*
*/
void  sqliteRollbackFunc
    (
    void * pData
    )
    {
    freeUpdateList ();
    }

#if 1
/*******************************************************************************
*
* freeUpdateList - free nodes from DB update list
*
* RETURNS: N/A
*
*/
int  freeUpdateList ( void )
    {
    DB_UPDATE_NODE *            pNode;
    DB_UPDATE_NODE *            pCurNode;

    pNode = (DB_UPDATE_NODE *)adpListFirst (&updateList);

    while (pNode)
        {
        pCurNode = sqlite3_malloc (sizeof(pNode));
        pCurNode = pNode;
        pNode = (DB_UPDATE_NODE *) adpListNext ((ADP_NODE *)pNode);
        adpListNodeRemove (&updateList, (ADP_NODE *)pCurNode);

        sqlite3_free (pCurNode);

        }
    }
#endif

// parse table and do something


void sendNotification (void)
    {
        EVTDSPTCH_DB_UPDATE_REQ     dbEvent;
        DB_UPDATE_NODE *            pNode;
        pNode = (DB_UPDATE_NODE *)adpListFirst (&updateList);
        unsigned int                outLen;
        char *                      pOutBuf;
        int                         i,j;

		memset ((void *)&dbEvent, 0x00, sizeof (dbEvent));

        int status = 0;
        while (pNode)
        {
        ADP_WNC_DBG ("opCode: %d, pDbName:%s, pTblName:%s, rowId:%d\n",pNode->opCode,pNode->pDbName,pNode->pTblName,pNode->rowId);

		dbEvent.opCode    = pNode->opCode;
        dbEvent.rowId     = pNode->rowId;
        strncpy (dbEvent.tblName, pNode->pTblName, (TBLNAME_MAX_LEN - 1));



        outLen = 0;
        pOutBuf = NULL;

#ifdef DEBUG

	if (pNode->ppQueryRes)
            ADP_WNC_DBG ("111 opCode: %d, pDbName:%s, pTblName:%s, rowId:%d\n",pNode->opCode,pNode->pDbName,(char *)pNode->pTblName,pNode->rowId);
	else
		ADP_WNC_DBG ("pNode->ppQueryRes is NULL");

#endif

	/* send UMI IOCTL to desired component */
    #ifdef UMI
        umiIoctl (NULL, UMI_COMP_EVTDSPTCH, EVTDSPTCH_DB_UPDATE_EVENT,
                (void *)&dbEvent, sizeof (dbEvent),
                pOutBuf, &outLen, UMI_IOCTL_OPT_NO_RESULT, 1, 0);
        #endif

        if (pOutBuf)
            sqlite3_free (pOutBuf);
        pNode = (DB_UPDATE_NODE *) adpListNext ((ADP_NODE *)pNode);
        ADP_WNC_DBG("test234");
        }

        return;
    }
/*******************************************************************************
*
* sqliteBusyHandler - handler to handle SQLITE_BUSY errors
*
* RETURNS: N/A
*
*/
int  sqliteBusyHandler
    (
    void *  pData,
    int     nTries
    )
    {
    SQLITE_BUSY_HANDLER_ARG *  pBHArgs = (SQLITE_BUSY_HANDLER_ARG *)pData;

#ifdef DEBUG
    fprintf (stderr, "%s:sqliteBusyHandler: tries=%d\n",pBHArgs->progName, nTries);
#endif
    if ( (pBHArgs->nTries) && (nTries >= pBHArgs->nTries))
        return 0;
    usleep (pBHArgs->sleepTimeInMs * 1000);
    return -1;
    }

/*******************************************************************************
*
* sqlite3QueryResGet -
*
* This routine exeutes the given query and returns the result to caller.
*
* RETURNS: double pointer to results.
*          NULL on failure.
*
* ERRNO: N/A.
*
* SEE_ALSO:
*/

char ** sqlite3QueryResGet
    (
    sqlite3 *       pDB,
    const char *    pDbName,
    const char *    pQueryStr,
    int        *    pnCols,
    int        *    pnRows
    )
    {
    char    **  ppQueryRes = NULL;
    char     *  pErrMsg = NULL;
    int         dbOpened = 0;
    int			i, j;

    SQLITE_BUSY_HANDLER_ARG bhArg = {"", 0, 100}; /* sleep indefinitily at 100
                                                     ms interval */

    if (!(pDbName || pDB) || !pQueryStr || !pnCols || !pnRows)
        return NULL;

    if (pDB == NULL)
        {
        /* open specified database */

        ADP_WNC_DBG("NULL pDB poniter \n");

        if (sqlite3_open (pDbName, &pDB) != SQLITE_OK)
            {
            fprintf(stderr, "Could not open database: %s\n", sqlite3_errmsg(pDB));
            return NULL;
            }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhArg);
        dbOpened = 1;
        }

    /* execute query */
    //fprintf(stderr, "%s --- pQueryStr: %s\n", __func__,  pQueryStr);

    if (sqlite3_get_table (pDB, pQueryStr, &ppQueryRes,
                           pnRows, pnCols, &pErrMsg) != SQLITE_OK)
        {
        ADP_WNC_DBG("sqlite3_get_table failed \n");
        if (ppQueryRes)
            {
            sqlite3_free_table (ppQueryRes);
            ppQueryRes = NULL;
            }
        sqlite3_free (pErrMsg);
        }

//		fprintf(stderr, "%s --- ppQueryRes: %s\n", __func__, ppQueryRes);


    if (dbOpened)
        {
        /* close the database */
        sqlite3_close (pDB);
        }

    return ppQueryRes;
    }


/*******************************************************************************
*
* sqlGetTablePrint - prints the given table
*
* This routine prints the given table
*
* RETURNS: OK on success.
*          ERROR on failure.
*
* ERRNO: N/A.
*
* SEE_ALSO:
*/

void sqlGetTablePrint
    (
    char ** ppQueryRes,
    int     nCols,
    int     nRows
    )
    {
    int i, j;

    fprintf (stderr, "[%s:%d]Cols:%d Rows:%d\n", __FUNCTION__, __LINE__,
            nCols, nRows);

    for (i=0; i < nRows; i++)
        {
        for (j=0; j < nCols; j++)
            fprintf (stderr, "| %-20s ", ppQueryRes[(i+1)*nCols + j]);
        fprintf (stderr, "\n");
        }
    fprintf (stderr, "\n");
    }
