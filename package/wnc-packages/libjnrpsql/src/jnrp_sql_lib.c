#include "jnrp_sql_lib.h"

void jnrpInitDb (sqlite3 * pDB, char * pDBName);
void jnrpCloseDb (sqlite3 * pDB);
char * jnrp_sql_getAttr(char * pDBName, char * pTblName, char * pAttrName, int rowId);
char * jnrp_sql_getAttrByCondit(char * pDBName, char * pTblName, char * pAttrName, char * pCondit);
int jnrp_sql_setAttr (char * pDBName, char * pTblName, char * pAttrName, char * value, int rowId);
char ** jnrp_sql_getRow (char * pDBName, char * pTblName, int rowId);
int jnrp_sql_dbEXE (char * pDBName, char * sqlQry);
char ** jnrp_sql_getByQry (char * pDBName, char * sqlQry);
int jnrp_sql_getRowNum (char * pTblName, char * pDBName);
int jnrp_sql_wr_dbEXE (char * pDBName, char * sqlQry);

void jnrpInitDb (sqlite3 * pDB, char * pDBName)
{

    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return;
    }

    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);
    return;

}

void jnrpCloseDb (sqlite3 * pDB)
{
    if (pDB)
        sqlite3_close (pDB);

    return;
}

char * jnrp_sql_getAttr(char * pDBName, char * pTblName, char * pAttrName, int rowId)
{

    char     *  pErrMsg = NULL;

    char  * sqlQry = "SELECT %s FROM %s WHERE _ROWID_='%d'";
    sqlQry = sqlite3_mprintf (sqlQry, pAttrName, pTblName,rowId);
    //printf("\nsqlQry:%s \n",sqlQry);

    char **ppQueryRes = NULL;
    int nCols = 0;
    int nRows = 0;

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);

    if (sqlite3_get_table (pDB, sqlQry, &ppQueryRes,
                           &nRows, &nCols, &pErrMsg) != SQLITE_OK)
    {
        printf("sqlite3_get_table failed \n");
        if (ppQueryRes)
        {
            sqlite3_free_table (ppQueryRes);
            ppQueryRes = NULL;
        }
        sqlite3_free (pErrMsg);
    }

    //sqlGetTablePrint(ppQueryRes,nCols,nRows);

// ppQueryRes[0]: column name ; ppQueryRes[1]: column value
    if (pDB)
        sqlite3_close (pDB);

    //If get data is NULL, then will string change to "". For avoid Web CGI happen crash.
    if (ppQueryRes[1] == NULL)
     ppQueryRes[1] = "";

    return ppQueryRes[1];
}

char * jnrp_sql_getAttrByCondit(char * pDBName, char * pTblName, char * pAttrName, char * pCondit)
{

    char     *  pErrMsg = NULL;

    char  * sqlQry = "SELECT %s FROM %s WHERE %s";
    sqlQry = sqlite3_mprintf (sqlQry, pAttrName, pTblName,pCondit);
    //printf("\nsqlQry:%s \n",sqlQry);

    char **ppQueryRes = NULL;
    int nCols = 0;
    int nRows = 0;

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);

    if (sqlite3_get_table (pDB, sqlQry, &ppQueryRes,
                           &nRows, &nCols, &pErrMsg) != SQLITE_OK)
    {
        printf("sqlite3_get_table failed \n");
        if (ppQueryRes)
        {
            sqlite3_free_table (ppQueryRes);
            ppQueryRes = NULL;
        }
        sqlite3_free (pErrMsg);
    }

    //sqlGetTablePrint(ppQueryRes,nCols,nRows);

// ppQueryRes[0]: column name ; ppQueryRes[1]: column value
    if (pDB)
        sqlite3_close (pDB);

    //If get data is NULL, then will string change to "". For avoid Web CGI happen crash.
    if (ppQueryRes[1] == NULL)
     ppQueryRes[1] = "";

    return ppQueryRes[1];
}
/*
 *      Name        | Age
 *      -----------------------
 *      Alice       | 43
 *      Bob         | 28
 *      Cindy       | 21
 *
 *  azResult[0] = "Name";       <column name 1>
 *      azResult[1] = "Age";    <column name 2>
 *      azResult[2] = "Alice";  <name value 1>
 *      azResult[3] = "43";             <age value 1>
 *      azResult[4] = "Bob";    <name value 2>
 *      azResult[5] = "28";             <age value 2>
 *      azResult[6] = "Cindy";  <name value 3>
 *      azResult[7] = "21";             <age value 3>
 */

char ** jnrp_sql_getRow (char * pDBName, char * pTblName, int rowId)
{
    char  * pErrMsg = NULL;
    char  * sqlQry = "SELECT * FROM %s WHERE _ROWID_='%d'";
    sqlQry = sqlite3_mprintf (sqlQry, pTblName,rowId);
    //printf("\nsqlQry:%s \n",sqlQry);

    char **ppQueryRes = NULL;
    int nCols = 0;
    int nRows = 0;

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);

    if (sqlite3_get_table (pDB, sqlQry, &ppQueryRes,
                           &nRows, &nCols, &pErrMsg) != SQLITE_OK)
    {
        printf("sqlite3_get_table failed \n");
        if (ppQueryRes)
        {
        sqlite3_free_table (ppQueryRes);
        ppQueryRes = NULL;
        }
        sqlite3_free (pErrMsg);
    }

    //sqlGetTablePrint(ppQueryRes,nCols,nRows);
    if (pDB)
        sqlite3_close (pDB);

    return ppQueryRes;
}

char ** jnrp_sql_getByQry (char * pDBName, char * sqlQry)
{
    char  * pErrMsg = NULL;
    //printf("\nsqlQry:%s \n",sqlQry);

    char **ppQueryRes = NULL;
    int nCols = 0;
    int nRows = 0;

    sqlite3 * pDB = NULL;
    //printf ("jnrp_sql_getByQry pDBName: %s \n",pDBName);
    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    if (!pDB)
        printf("pDB == NULL");

    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);

    if (sqlite3_get_table (pDB, sqlQry, &ppQueryRes,
                           &nRows, &nCols, &pErrMsg) != SQLITE_OK)
    {
        printf("sqlite3_get_table failed \n");
        if (ppQueryRes)
        {
            sqlite3_free_table (ppQueryRes);
            ppQueryRes = NULL;
        }
        sqlite3_free (pErrMsg);
    }

    //sqlGetTablePrint(ppQueryRes,nCols,nRows);
    if (pDB)
        sqlite3_close (pDB);

    return ppQueryRes;
}


int jnrp_sql_setAttr (char * pDBName, char * pTblName, char * pAttrName, char * value, int rowId)
{

    char     *  pErrMsg = NULL;
    char  * sqlQry = "UPDATE %s set %s='%s' WHERE _ROWID_='%d'";
    sqlQry = sqlite3_mprintf (sqlQry, pTblName, pAttrName,value,rowId);

    //printf ("sqlQry: %s \n",sqlQry);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);

    if (sqlite3_exec (pDB, sqlQry, NULL, NULL, &pErrMsg) != SQLITE_OK)
    {
        printf ("Query %s failed\n", sqlQry);
        sqlite3_free (pErrMsg);
        sqlite3_free (sqlQry);
        return -1;
    }

    //sqlite3_free (sqlQry);
    if (pDB)
        sqlite3_close (pDB);

    return 0;
}

int jnrp_sql_wr_dbEXE(char * pDBName, char * sqlQry)
{
    char     *  pErrMsg = NULL;

    printf ("sqlQry: %s \n",sqlQry);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);

    if (sqlQry)
    {
        if (sqlite3_exec (pDB, sqlQry, NULL, NULL, &pErrMsg) != SQLITE_OK)
        {
            printf ("Query %s failed\n", sqlQry);
            sqlite3_free (pErrMsg);
            sqlite3_free (sqlQry);
            return -1;
        }
        //sqlite3_free (sqlQry);
    }

    if (pDB)
        sqlite3_close (pDB);

    return 0;
}

// for update, insert, delete, create
int jnrp_sql_dbEXE (char * pDBName, char * sqlQry)
{
#if 0	
    char     *  pErrMsg = NULL;

    printf ("sqlQry: %s \n",sqlQry);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBName, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBName);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhLibArg);

    if (sqlQry)
    {
        if (sqlite3_exec (pDB, sqlQry, NULL, NULL, &pErrMsg) != SQLITE_OK)
        {
            printf ("Query %s failed\n", sqlQry);
            sqlite3_free (pErrMsg);
            sqlite3_free (sqlQry);
            return -1;
        }
        //sqlite3_free (sqlQry);
    }

    if (pDB)
        sqlite3_close (pDB);

    return 0;
#else
	int status = -1;
	int Retrycount = 0;
	do {
		status = jnrp_sql_wr_dbEXE(pDBName, sqlQry);
		if(status >= 0)
			return 0;
			
		sleep(1);
		Retrycount++;
	}while(status!=0 || Retrycount > 2);
	
	return -1;
#endif    
}

int jnrp_sql_getRowNum (char * pTblName, char * pDBName)
{
    int nRet = 0;

    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    char   sqlCmd [1024] = {0};
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBName, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);

    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
    }
    nRet = row_count;

    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();


    return nRet;
}
