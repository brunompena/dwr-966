#ifndef  JNRP_SQL_LIB_H
#define  JNRP_SQL_LIB_H

/* includes */

#include <sqlite3.h>
#include <sqliteWNCLib.h>
//#include <adaptos.h>
//#include <adpListLib.h>
//#include <adpModDebug.h>

#define DB_MAX_RETRIES                  100
#define NUM_MILLIS_TO_SLEEP             100

//static sqlite3 *                pLibDB = NULL;
static SQLITE_BUSY_HANDLER_ARG   bhLibArg = {"", DB_MAX_RETRIES, NUM_MILLIS_TO_SLEEP};

extern void jnrpCloseDb (sqlite3 * pDB);
extern void jnrpInitDb (sqlite3 * pDB, char * pDbFile);

extern char * jnrp_sql_getAttr(char * pDBName, char * pTblName, char * pAttrName, int rowId);
extern char * jnrp_sql_getAttrByCondit(char * pDBName, char * pTblName, char * pAttrName, char * pCondit);
extern int jnrp_sql_setAttr (char * pDBName, char * pTblName, char * pAttrName, char * value, int rowId);
extern char ** jnrp_sql_getRow (char * pDBName, char * pTblName, int rowId);
extern int jnrp_sql_dbEXE (char * pDBName, char * sqlQry);
extern char ** jnrp_sql_getByQry (char * pDBName, char * sqlQry);
extern int jnrp_sql_getRowNum (char * pTblName, char * pDBName);

#endif
