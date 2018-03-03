/* jnrplib.c -  */
/*
 * TODO: need to add c str parser tool
 * Developer : Anita Wu
 * Date : 20/06/2012
 * All code (c)2012 wnc inc. all rights reserved
 */
#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */
#ifdef _DEBUG
static int      debugSecurity = 1;
#else
static int      debugSecurity = 0;
#endif
int login_flag = LOGIN_DEFAULT;
#if defined(__P02003_Dora)
char *now_date;
#endif

//add Multi-language
char g_multi_lang[32];
void initSetLanguage(void);

//enhanced security issues
char g_login_user_ip[32];

int getRandomStr(char *result);
void genToken(int num);

#ifdef __USED_WEBS_DB__
void jnrpInitDb_Webs (char * pDBName);
void jnrpCloseDb_Webs (void);

char * jnrp_sql_getAttr_Webs (char * pTblName, char * pAttrName, int rowId);
char * jnrp_sql_getAttrByCondit_Webs (char * pTblName, char * pAttrName, char * pCondit);
char ** jnrp_sql_getRow_Webs (char * pTblName, int rowId);
char ** jnrp_sql_getByQry_Webs (char * sqlQry);
int jnrp_sql_getRowNum_Webs (char * pTblName);

int jnrp_sql_setAttr_Webs (char * pTblName, char * pAttrName, char * value, int rowId);
int jnrp_sql_dbEXE_Webs (char * sqlQry);

char ** sqlite3QueryResGet_Webs(const char * pQueryStr, int * pnCols, int * pnRows);

void jnrpInitDb_Webs (char * pDBName)
{
	 ADP_WNC_DBG ("jnrpInitDb_Webs(), pWebDb=%s\n", pWebDb);

  if (sqlite3_open (pDBName, &pWebDb) != SQLITE_OK)
  {
    printf ("jnrpInitDb_Webs(), unable to open the DB file %s\n", pDBName);
    return;
  }

  ADP_WNC_DBG ("jnrpInitDb_Webs(), pWebDb=%s\n", pWebDb);

  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
  return;
}

void jnrpCloseDb_Webs ()
{
  ADP_WNC_DBG ("jnrpCloseDb_Webs(), pWebDb=%s\n", pWebDb);
  if (pWebDb)
    sqlite3_close (pWebDb);
  return;
}

char * jnrp_sql_getAttr_Webs (char * pTblName, char * pAttrName, int rowId)
{
 char *pErrMsg = NULL;
 char *sqlQry = "SELECT %s FROM %s WHERE _ROWID_='%d'";

 sqlQry = sqlite3_mprintf (sqlQry, pAttrName, pTblName, rowId);

 //ADP_WNC_DBG ("jnrp_sql_getAttr_Webs(), sqlQry : %s\n", sqlQry);

 char **ppQueryRes = NULL;
 int nCols = 0;
 int nRows = 0;

 if (!pWebDb) {
 	ADP_WNC_DBG ("jnrp_sql_getAttr_Webs(), pWebDb == NULL\n");
  if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
  {
    printf ("unable to open the DB file %s\n", pDBFile);
    return NULL;
  }
  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
 }

 if (sqlite3_get_table (pWebDb, sqlQry, &ppQueryRes, &nRows, &nCols, &pErrMsg) != SQLITE_OK)
 {
   printf ("jnrp_sql_getAttr_Webs(), sqlite3_get_table failed\n");
   if (ppQueryRes)
   {
     sqlite3_free_table (ppQueryRes);
     ppQueryRes = NULL;
   }
   sqlite3_free (pErrMsg);
 }
 //sqlGetTablePrint(ppQueryRes,nCols,nRows);

 // ppQueryRes[0]: column name ; ppQueryRes[1]: column value

 //If get data is NULL, then will string change to "". For avoid Web CGI happen crash.
 if (ppQueryRes[1] == NULL)
  ppQueryRes[1] = "";

 return ppQueryRes[1];
}

char * jnrp_sql_getAttrByCondit_Webs(char * pTblName, char * pAttrName, char * pCondit)
{
 char *pErrMsg = NULL;
 char *sqlQry = "SELECT %s FROM %s WHERE %s";

 sqlQry = sqlite3_mprintf (sqlQry, pAttrName, pTblName,pCondit);
 
 //ADP_WNC_DBG ("jnrp_sql_getAttrByCondit_Webs(), sqlQry : %s\n", sqlQry);

 char **ppQueryRes = NULL;
 int nCols = 0;
 int nRows = 0;

 if (!pWebDb) {
 	ADP_WNC_DBG ("jnrp_sql_getAttrByCondit_Webs(), pWebDb == NULL\n");
  if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
  {
    printf ("unable to open the DB file %s\n", pDBFile);
    return NULL;
  }
  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
 }

 if (sqlite3_get_table (pWebDb, sqlQry, &ppQueryRes, &nRows, &nCols, &pErrMsg) != SQLITE_OK)
 {
   printf ("jnrp_sql_getAttrByCondit_Webs(), sqlite3_get_table failed\n");
   if (ppQueryRes)
   {
     sqlite3_free_table (ppQueryRes);
     ppQueryRes = NULL;
   }
   sqlite3_free (pErrMsg);
 }
 //sqlGetTablePrint(ppQueryRes,nCols,nRows);
 if (nRows==0)
	return ""; 	

 //ppQueryRes[0]: column name ; ppQueryRes[1]: column value

 //If get data is NULL, then will string change to "". For avoid Web CGI happen crash.
 if (ppQueryRes[1] == NULL)
   ppQueryRes[1] = "";

 return ppQueryRes[1];
}

/*
 *      Name        | Age
 *      -----------------------
 *      Alice       | 43
 *
 *      azResult[0] = "Name";       <column name 1>
 *      azResult[1] = "Age";    <column name 2>
 *      azResult[2] = "Alice";  <name value 1>
 *      azResult[3] = "43";             <age value 1>
 */
char ** jnrp_sql_getRow_Webs (char * pTblName, int rowId)
{
 char  *pErrMsg = NULL;
 char  *sqlQry = "SELECT * FROM %s WHERE _ROWID_='%d'";

 sqlQry = sqlite3_mprintf (sqlQry, pTblName, rowId);
 
 //ADP_WNC_DBG ("jnrp_sql_getRow_Webs(), sqlQry : %s\n", sqlQry);

 char **ppQueryRes = NULL;
 int nCols = 0;
 int nRows = 0;

 if (!pWebDb) {
 	ADP_WNC_DBG ("jnrp_sql_getRow_Webs(), pWebDb == NULL\n");
  if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
  {
    printf ("unable to open the DB file %s\n", pDBFile);
    return NULL;
  }
  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
 }

 if (sqlite3_get_table (pWebDb, sqlQry, &ppQueryRes, &nRows, &nCols, &pErrMsg) != SQLITE_OK)
 {
   printf ("jnrp_sql_getRow_Webs(), sqlite3_get_table failed\n");
   if (ppQueryRes)
   {
     sqlite3_free_table (ppQueryRes);
     ppQueryRes = NULL;
   }
   sqlite3_free (pErrMsg);
 }
 //sqlGetTablePrint(ppQueryRes,nCols,nRows);
 return ppQueryRes;
}

char ** jnrp_sql_getByQry_Webs (char * sqlQry)
{
 char  *pErrMsg = NULL;

 //ADP_WNC_DBG ("jnrp_sql_getByQry_Webs(), sqlQry : %s\n", sqlQry);

 char **ppQueryRes = NULL;
 int nCols = 0;
 int nRows = 0;

 if (!pWebDb) {
 	ADP_WNC_DBG ("jnrp_sql_getByQry_Webs(), pWebDb == NULL\n");
  if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
  {
    printf ("unable to open the DB file %s\n", pDBFile);
    return NULL;
  }
  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
 }

 if (sqlite3_get_table (pWebDb, sqlQry, &ppQueryRes, &nRows, &nCols, &pErrMsg) != SQLITE_OK)
 {
   printf ("jnrp_sql_getByQry_Webs(), sqlite3_get_table failed\n");
   if (ppQueryRes)
   {
     sqlite3_free_table (ppQueryRes);
     ppQueryRes = NULL;
   }
   sqlite3_free (pErrMsg);
 }
 //sqlGetTablePrint(ppQueryRes,nCols,nRows);
 return ppQueryRes;
}

int jnrp_sql_getRowNum_Webs (char * pTblName)
{
 char *pErrMsg = NULL;
 char *sqlQry = "SELECT COUNT(*) FROM %s";

 sqlQry = sqlite3_mprintf (sqlQry, pTblName);

 //ADP_WNC_DBG ("jnrp_sql_getRowNum_Webs(), sqlQry : %s\n", sqlQry);

 char **ppQueryRes = NULL;
 int nCols = 0;
 int nRows = 0;

 if (!pWebDb) {
 	ADP_WNC_DBG ("jnrp_sql_getRowNum_Webs(), pWebDb == NULL\n");
  if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
  {
    printf ("unable to open the DB file %s\n", pDBFile);
    return NULL;
  }
  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
 }

 if (sqlite3_get_table (pWebDb, sqlQry, &ppQueryRes, &nRows, &nCols, &pErrMsg) != SQLITE_OK)
 {
   printf ("jnrp_sql_getRowNum_Webs(), sqlite3_get_table failed\n");
   if (ppQueryRes)
   {
     sqlite3_free_table (ppQueryRes);
     ppQueryRes = NULL;
   }
   sqlite3_free (pErrMsg);
 }
 //sqlGetTablePrint(ppQueryRes,nCols,nRows);

 if (ppQueryRes[1] == NULL)
   ppQueryRes[1] = "";

 return gatoi(ppQueryRes[1]);
}

int jnrp_sql_setAttr_Webs (char * pTblName, char * pAttrName, char * value, int rowId)
{
 char *pErrMsg = NULL;
 char  *sqlQry = "UPDATE %s set %s='%s' WHERE _ROWID_='%d'";
 sqlQry = sqlite3_mprintf (sqlQry, pTblName, pAttrName, value, rowId);

 ADP_WNC_DBG ("jnrp_sql_setAttr_Webs(), sqlQry : %s\n",sqlQry);

 if (!pWebDb) {
 	printf("jnrp_sql_setAttr_Webs(), pWebDb == NULL\n");
  if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
  {
    printf ("jnrp_sql_setAttr_Webs(), unable to open the DB file %s\n", pDBFile);
    return NULL;
  }
  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
 }

 if (sqlite3_exec (pWebDb, sqlQry, NULL, NULL, &pErrMsg) != SQLITE_OK)
 {
   printf ("jnrp_sql_setAttr_Webs(), Query %s failed\n", sqlQry);
   sqlite3_free (pErrMsg);
   sqlite3_free (sqlQry);
   return -1;
 }
 //sqlite3_free (sqlQry);

 return 0;
}

// for update, insert, delete, create
int jnrp_sql_dbEXE_Webs (char * sqlQry)
{
 char *pErrMsg = NULL;

 ADP_WNC_DBG ("jnrp_sql_dbEXE_Webs(), sqlQry : %s\n",sqlQry);

 if (!pWebDb) {
 	ADP_WNC_DBG ("jnrp_sql_dbEXE_Webs(), pWebDb == NULL\n");
  if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
  {
    printf ("jnrp_sql_dbEXE_Webs(), unable to open the DB file %s\n", pDBFile);
    return NULL;
  }
  sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
 }

 if (sqlQry)
 {
   if (sqlite3_exec (pWebDb, sqlQry, NULL, NULL, &pErrMsg) != SQLITE_OK)
   {
     printf ("jnrp_sql_dbEXE_Webs(), Query %s failed\n", sqlQry);
     sqlite3_free (pErrMsg);
     sqlite3_free (sqlQry);
     return -1;
   }
   //sqlite3_free (sqlQry);
 }

 return 0;
}

char ** sqlite3QueryResGet_Webs(const char * pQueryStr, int * pnCols, int * pnRows)
{
    char    **  ppQueryRes = NULL;
    char     *  pErrMsg = NULL;
    int         dbOpened = 0;
    int			i, j;

    if (!pQueryStr || !pnCols || !pnRows)
        return NULL;

    if (pWebDb == NULL)
    {
        /* open specified database */
        ADP_WNC_DBG("sqlite3QueryResGet_Webs(), pWebDb == NULL\n");
        if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
            {
            printf ("sqlite3QueryResGet_Webs(), unable to open the DB file %s\n", pDBFile);
            return NULL;
            }
        sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
        dbOpened = 1;
    }

    /* execute query */
    //fprintf(stderr, "%s --- pQueryStr: %s\n", __func__,  pQueryStr);
    //ADP_WNC_DBG("sqlite3QueryResGet_Webs(), pQueryStr : %s\n",pQueryStr);

    if (sqlite3_get_table (pWebDb, pQueryStr, &ppQueryRes,
                           pnRows, pnCols, &pErrMsg) != SQLITE_OK)
    {
        printf("sqlite3QueryResGet_Webs(), sqlite3_get_table failed\n");
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
        sqlite3_close (pWebDb);
    }
    return ppQueryRes;
}
#endif //__USED_WEBS_DB__

#if defined(__P02003_Dora)
int execSQLCmd (char * sqlCmd);
void setResultCode(char * resultMsg);
char result_msg[256] = {0};
static int getActionResult(int eid, webs_t wp, int argc, char_t **argv);
static void loginUrlFilter(webs_t wp, char_t *path, char_t *query);
static void logoutUrlFilter(webs_t wp, char_t *path, char_t *query);

int execSQLCmd (char * sqlCmd)
{
	char * pErrMsg = NULL;
	int status = -1;	

	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	{
		printf ("unable to open the DB file %s\n", pDBFile);
		return -1;
	}
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	printf("sqlCmd=%s\n",sqlCmd);
	status = sqlite3_exec (pDB, sqlCmd, NULL, NULL, &pErrMsg);
	printf("pErrMsg=%s\n",pErrMsg);
	if (pErrMsg != NULL)
	{
		if (strstr(pErrMsg,"not unique")!=NULL)
		{
			if (strstr(sqlCmd,"insert into hostTbl")!=NULL)
			{
				setResultCode("Computer Name with the same name already exists");
			}
			else if (strstr(sqlCmd,"insert into ContentFltr")!=NULL)
			{
				setResultCode("Filter with the same name already exists");
			}
			else if (strstr(sqlCmd,"insert into UrlProfile")!=NULL)
			{
				setResultCode("Profile with the same name already exists");
			}
			else if (strstr(sqlCmd,"insert into Schedules")!=NULL)
			{
				setResultCode("Schedule with the same name already exists");
			}			
		}
		//setResultCode(pErrMsg);
		sqlite3_free(pErrMsg);
		return -1;
	}
	
	status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);	
	//printf("pErrMsg=%s\n",pErrMsg);

	if (pDB)
		sqlite3_close (pDB);
	
	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD);		
		setResultCode("Setting completed");
	}		
	return 0;
}

void setResultCode(char * resultMsg)
{
	memset(result_msg, 0x00, sizeof(result_msg));
	strcpy(result_msg, resultMsg);
}
#endif

void jnrpWebInit (char * dbfile);

//add Multi-language
void initSetLanguage(void)
{
	char *pLanguage = jnrp_sql_getAttr_Webs("MultiLanguage", "language", 1);
	memset(g_multi_lang, 0, sizeof(g_multi_lang));
	if (pLanguage == NULL || !gstrcmp(pLanguage,""))
		memcpy(g_multi_lang, "en", sizeof(g_multi_lang));
	else
		memcpy(g_multi_lang, pLanguage, sizeof(g_multi_lang));
	return;
}

//Poland WebUI skin --start
#if defined(__P02003_Dora)
void updateIMSIToDB()
{
	system("lte_get -D5 --timeout 5000 --update-db > /dev/NULL");
	return;
}

static void getIMSI_db(int eid, webs_t wp, int argc, char_t **argv)
{
	char *pIMSI = NULL;
	char str[64];
	char buf[64];
	FILE *fp = NULL;

	updateIMSIToDB();
	memset(str, 0, sizeof(str));
	memset(buf, 0, sizeof(buf));
	fp = fopen("/tmp/sim.imsi", "r");
	if (fp != NULL)
	{
		fread(str, sizeof(str), 1, fp);
        fclose(fp);
		memcpy(buf, str, 5);
	} else {
		pIMSI = jnrp_sql_getAttr_Webs("lteHwInfo", "IMSI", 1);
		if (pIMSI == NULL || !gstrcmp(pIMSI,""))
			memcpy(buf, "00000", 5);
		else
			memcpy(buf, pIMSI, 5);
	}
	memset(g_sim_imsi, 0, sizeof(g_sim_imsi));
	memcpy(g_sim_imsi, buf, sizeof(buf));

	websWrite(wp, T("%s"), buf);
	return;
}

static void getIMSI_db1(int eid, webs_t wp, int argc, char_t **argv)
{
	char *pIMSI = NULL;
	char str[64];
	char buf[64];
	FILE *fp = NULL;

	updateIMSIToDB();
	memset(str, 0, sizeof(str));
	memset(buf, 0, sizeof(buf));
	fp = fopen("/tmp/sim.imsi", "r");
	if (fp != NULL)
	{
		fread(str, sizeof(str), 1, fp);
        fclose(fp);
		memcpy(buf, str, 15);
	} else {
		pIMSI = jnrp_sql_getAttr_Webs("lteHwInfo", "IMSI", 1);
		if (pIMSI == NULL || !gstrcmp(pIMSI,""))
			memcpy(buf, "000000000000000", 15);
		else
			memcpy(buf, pIMSI, 15);
	}

	websWrite(wp, T("%s"), buf);
	return;
}

void updateIMEIToDB()
{
	system("lte_get -D2 --timeout 5000 --update-db > /dev/NULL");
	return;
}

static void getIMEI_db(int eid, webs_t wp, int argc, char_t **argv)
{
	char *pIMEI = NULL;
	char str[64];
	char buf[64];
	FILE *fp = NULL;

	updateIMEIToDB();
	memset(str, 0, sizeof(str));
	memset(buf, 0, sizeof(buf));
	pIMEI = jnrp_sql_getAttr_Webs("lteHwInfo", "IMEI", 1);
	if (pIMEI == NULL || !gstrcmp(pIMEI,""))
		memcpy(buf, "000000000000000", 15);
	else
		memcpy(buf, pIMEI, 15);

	websWrite(wp, T("%s"), buf);
	return;
}


static void getIMSI_mem(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("%s"), g_sim_imsi);
	return;
}
#endif
//Poland WebUI skin --end

char * getIpAddress(char * pDBFile);
#if defined(__P02003_Dora)
static void getCurrentTime();
#endif
// sqlite3 test codes
//enhanced security issues
static int getUniName(int eid, webs_t wp, int argc, char_t **argv);
static int getUniId(int eid, webs_t wp, int argc, char_t **argv);
//add Multi-language
static void getLanguage(int eid, webs_t wp, int argc, char_t **argv);
static void setLanguage(webs_t wp, char_t *path, char_t *query);
static int getVoipValue(int eid, webs_t wp, int argc, char_t **argv);
static void setVoipValue(webs_t wp, char_t *path, char_t *query);
static int writeMainMenu (int eid, webs_t wp, int argc, char_t **argv);
static int writeTopMenu (int eid, webs_t wp, int argc, char_t **argv);
static int writeLeftMenu (int eid, webs_t wp, int argc, char_t **argv);
static void user_logout(webs_t wp, char_t *path, char_t *query);
static void user_login(webs_t wp, char_t *path, char_t *query);
static int getConf(int eid, webs_t wp, int argc, char_t **argv);
static void getuser_login(int eid, webs_t wp, int argc, char_t **argv);
static void getintologin(int eid,webs_t wp,int argc,char_t * * argv);
#if defined(__P02003_Dora)
int P02003_login_pin_status = 0;
static int getLoginPinStatus(int eid, webs_t wp, int argc, char_t **argv);
static int getLoginResetToDefault(int eid, webs_t wp, int argc, char_t **argv);

static int getLoginPinStatus(int eid, webs_t wp, int argc, char_t **argv)
{
 if (P02003_login_pin_status == 1) {
   P02003_login_pin_status = 0;
   return websWrite(wp, T("\"%s\""), "1");
 } else if (P02003_login_pin_status == -1) {
   P02003_login_pin_status = 0;
   return websWrite(wp, T("\"%s\""), "-1");
 } else {
   return websWrite(wp, T("\"%s\""), "0");
 }
}

static int getLoginResetToDefault(int eid, webs_t wp, int argc, char_t **argv)
{
 char * pResetToDefault = jnrp_sql_getAttr_Webs ("ResetToDefault", "resetToDefault", 1);
 return websWrite(wp, T("\"%d\""), gatoi(pResetToDefault));
}
#endif

char * getIpAddress(char * pDBFile)
{
#ifdef __USED_WEBS_DB__
    return jnrp_sql_getAttr_Webs ("Lan", "IpAddress", 1);
#else
    return jnrp_sql_getAttr (pDBFile, "Lan", "IpAddress", 1);
#endif
}

#if defined(__P02003_Dora)
static void getCurrentTime()
{
	char tzarray[15][3] = {"00", "01", "02", "03", "04", "05", "06", \
"07", "08", "09", "10", "11", "12", "13", "14"};
	char tzmin[][3] = {"00", "30", "45"};
	long dtime = 0;
	short diffhour = 0;
	char diffmin[3] = {"00"};
	char *strtm;
	int dtime_index=0;
	time_t tm1, tm2;
	struct tm *t1, *t2;
	strtm = malloc(120 * sizeof(char));
	tm1 = time(NULL);
	t2 = gmtime(&tm1);
	tm2 = mktime(t2);
	t1 = localtime(&tm1);
#ifdef __USED_WEBS_DB__
	char * pQryResult = jnrp_sql_getAttr_Webs ("ntpConfig", "timezone", 1);
#else
	char * pQryResult = jnrp_sql_getAttr (pDBFile,"ntpConfig", "timezone", 1);	  
#endif
	int timezone = gatoi(pQryResult);
	//dtime = (long)(tm1 - tm2);
	dtime = timezone*60;
	//printf("dtime: %d \n",dtime);
/* Print local time as a string */
	
	strftime(strtm, 100, "%a %b %d %Y %H:%M:%S GMT", t1);

	dtime_index = (short)(abs(dtime) / 3600);
	if(dtime_index == 0)
	{
	}
	else
	{
		if (dtime >= 0 )
			strcat(strtm, "+");
		else
			strcat(strtm, "-");
		strcat(strtm, tzarray[(short)(abs(dtime) / 3600)]);
		//strcat(strtm, ":");
		if (1800 == (short)(abs(dtime) % 3600))
			strcat(strtm, tzmin[1]);
		else if(2700 == (short)(abs(dtime) % 3600))
			strcat(strtm, tzmin[2]);
		else
			strcat(strtm, tzmin[0]);
	}
	free(now_date);
	now_date = malloc(64 * sizeof(char));
	gstrcpy(now_date,"");
	gstrcat(now_date,strtm);
	free(strtm);
}
#endif
//Firewall

void jnrpWebInit (char * dbfile)
{
	//add Multi-language
	initSetLanguage();

	//Poland WebUI skin --start
#if defined(__P02003_Dora)
	websAspDefine(T("getIMSI_db"), getIMSI_db);
	websAspDefine(T("getIMSI_db1"), getIMSI_db1);
	websAspDefine(T("getIMSI_mem"), getIMSI_mem);
	websAspDefine(T("getIMEI_db"), getIMEI_db);
#endif
	//Poland WebUI skin --end

    //printf ("jnrpWebInit test\n");
    // goahead asp function init
    // sqlite3 test
    //enhanced security issues
    websAspDefine(T("getUniName"), getUniName);
    websAspDefine(T("getUniId"), getUniId);
	//add Multi-language
	websAspDefine(T("getLanguage"), getLanguage);
	websFormDefine(T("setLanguage"), setLanguage);
    websAspDefine(T("writeTopMenu"), writeTopMenu);
    websAspDefine(T("writeMainMenu"), writeMainMenu);
    websAspDefine(T("writeLeftMenu"), writeLeftMenu);
    websAspDefine(T("getVoipValue"), getVoipValue);
    websFormDefine(T("setVoipValue"), setVoipValue);
    websFormDefine(T("user_logout"), user_logout);
	websFormDefine(T("user_login"), user_login);
	websAspDefine(T("getuser_login"), getuser_login);
	websAspDefine(T("getintologin"), getintologin);
	
	websAspDefine(T("getConf"), getConf);
#ifdef __P02003_Dora
	websAspDefine(T("getLoginPinStatus"), getLoginPinStatus);
	websAspDefine(T("getLoginResetToDefault"), getLoginResetToDefault);
	websAspDefine(T("getActionResult"), getActionResult);
	websFormDefine(T("loginUrlFilter"), loginUrlFilter);
	websFormDefine(T("logoutUrlFilter"), logoutUrlFilter);
#endif

    // home information
    initHomeWeb();
    
    // lan page
    initLanWeb();

    // advanced pages
    initFirewallWeb();

    initCWMPWeb();
    // system pages
    initSystemWeb();

    initWifi();
    initInternetWeb();

	// application pages
	initApplicationWeb();

    //voip pages
	initVoipWeb();
	
#ifndef __USED_WEBS_DB__
    // init sqlite3 db
    if (dbfile)
    {
        //printf("db init \n");
        //jnrpInitDb (pWebDb,dbfile);
        //if (!pWebDb)
          //  printf("init pWebDb failed\n");

        pDBFile = dbfile;
        //printf("pDBFile: %s \n",pDBFile);
    }
    else
    {
        //jnrpInitDb(IMPORT_DB_FILE);
        pDBFile = IMPORT_DB_FILE;
        //printf ("use default db path \n");
    }
#endif

#if defined(__P02003_Dora)
	jnrp_sql_setAttr_Webs("VpnConfig", "vpn_mode", "disable",1);
	jnrp_sql_setAttr_Webs("VpnConfig", "vpn_ipsec_mode", "",1);
#endif

    return;
}

//enhanced security issues
int getRandomStr(char *result)
{
	char str[16];
	int *random;

	memset(str, 0, sizeof(str));
	memset(result, 0, 32);
	FILE *fp = fopen("/dev/urandom", "r");
	if (fp != NULL)
	{
		fread(str, sizeof(str), 1, fp);
        fclose(fp);
	} else {
		printf("%s(%d)--: failed to open /dev/urandom\n", __FILE__, __LINE__);
		return 0;
	}

	random = (int *)str;
	snprintf (result, 32 - 1, "%08x%08x", random[0], random[1]);
	
	return 1;
}

void genToken(int num)
{
	char cmd[128];
	char random1[32];
	char random2[32];
	memset(cmd, 0, sizeof(cmd));

	if (num == 1 || num == 3)
	{
		memset(random1, 0, sizeof(random1));
		getRandomStr(random1);
		sprintf(cmd, "echo uniId_%s > /tmp/csrf.uniName", random1);
		system(cmd);
	}

	if (num == 2 || num == 3)
	{
		memset(random2, 0, sizeof(random2));
		getRandomStr(random2);
		sprintf(cmd, "echo %s > /tmp/csrf.uniId", random2);
		system(cmd);
	}
	return;
}

static int getUniName(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char line[64];
	memset(line, 0, sizeof(line));

	fp = fopen("/tmp/csrf.uniName", "r");
	if (fp)
	{
		if (fgets(line, sizeof(line), fp) != NULL)
		{
			line[strlen(line)-1] = 0;
		}
		fclose(fp);
	} else {
		printf("%s(%d)--: failed to open /tmp/csrf.uniName\n", __FILE__, __LINE__);
	}

	return websWrite(wp, T("%s"), line);
}

static int getUniId(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char line[64];
	memset(line, 0, sizeof(line));

	fp = fopen("/tmp/csrf.uniId", "r");
	if (fp)
	{
		if (fgets(line, sizeof(line), fp) != NULL)
		{
			line[strlen(line)-1] = 0;
		}
		fclose(fp);
	} else {
		printf("%s(%d)--: failed to open /tmp/csrf.uniId\n", __FILE__, __LINE__);
	}

	return websWrite(wp, T("%s"), line);
}

//add Multi-language
static void getLanguage(int eid, webs_t wp, int argc, char_t **argv)
{
	websWrite(wp, T("%s"), g_multi_lang);
	return;
}

static void setLanguage(webs_t wp, char_t *path, char_t *query)
{
	char_t * pLanguage = websGetVar(wp, T("multi_language"), T("en"));

	jnrp_sql_setAttr_Webs("MultiLanguage", "language", pLanguage, 1);

	memset(g_multi_lang, 0, sizeof(g_multi_lang));
	if (pLanguage == NULL || !gstrcmp(pLanguage,""))
		memcpy(g_multi_lang, "en", sizeof(g_multi_lang));
	else
		memcpy(g_multi_lang, pLanguage, sizeof(g_multi_lang));

	system(EXPORT_CMD);
	websRedirect(wp, T("system/language.asp"));
	return;
}

static int getVoipValue(int eid, webs_t wp, int argc, char_t **argv)
{
    //char * context = jnrp_sql_getAttr(pWebDb,"voipwatchdog", "context",1);
    //char * id = jnrp_sql_getAttr(pWebDb,"voipwatchdog", "id",1);
    //printf("context: %s, id: %s \n",context,id);
    //return websWrite(wp, T("%s"),context);
    return 0;
}

static void setVoipValue(webs_t wp, char_t *path, char_t *query)
{
    char_t *voip;
    voip = websGetVar(wp, T("voipTest"), T(""));
    char_t *voip_enable = websGetVar(wp, T("voip_enable"), T("off"));
    printf("voip_enable: %s \n",voip_enable);

    //jnrp_sql_setAttr (pWebDb,"voipwatchdog", "context", voip, 1);

    //websHeader(wp);
    //websWrite(wp, T("<body><h2>voipTest: %s</h2>\n"), voip);
    //websFooter(wp);
    //websDone(wp, 200);
    websRedirect(wp, T("advanced/firewall_general.asp"));

}

static int writeMainMenu (int eid, webs_t wp, int argc, char_t **argv)
{
    char line[2048];
    FILE *file;
    //static const char filename[] = "/usr/local/www/mainmenu.asp";
    
#if defined(__P02003_Dora)
	#ifdef __USED_WEBS_DB__
		char * pTmp = jnrp_sql_getAttr_Webs ("FirewallNatConfig", "Enable", 1);
	#else
		char * pTmp = jnrp_sql_getAttr (pDBFile, "FirewallNatConfig", "Enable",1);
	#endif
		if(!gstrcmp(pTmp,"1")) //nat mode
		{
			//Poland WebUI skin --start
			if (!gstrcmp(g_sim_imsi, "26012"))
				file = fopen(NAT_MAINMENU_CP_FILE, "r");
			else if (!gstrcmp(g_sim_imsi, "26001"))
				file = fopen(NAT_MAINMENU_PLK_FILE, "r");
			else
				file = fopen(NAT_MAINMENU_FILE, "r");
			//Poland WebUI skin --end
		}
		else
		{
			//Poland WebUI skin --start
			if (!gstrcmp(g_sim_imsi, "26012"))
				file = fopen(MAINMENU_CP_FILE, "r");
			else if (!gstrcmp(g_sim_imsi, "26001"))
				file = fopen(MAINMENU_PLK_FILE, "r");
			else
				file = fopen(MAINMENU_FILE, "r");
			//Poland WebUI skin --end
		}
#else
		file = fopen(MAINMENU_FILE, "r");
#endif
    
    if (file)
    {
        while (fgets ( line, sizeof line, file ) != NULL)
        {
            websWrite(wp, T("%s"), line);
        }
        fclose(file);
    }
    else
        perror ( MAINMENU_FILE );

    return 0;
}

static int writeTopMenu (int eid, webs_t wp, int argc, char_t **argv)
{
    char line[2048];
    FILE *file;
    //static const char filename[] = "/usr/local/www/topmenu.asp";
    file = fopen(TOPMENU_FILE, "r");
    if (file)
    {
        while (fgets ( line, sizeof line, file ) != NULL)
        {
            websWrite(wp, T("%s"), line);
        }
        fclose(file);
    }
    else
        perror ( TOPMENU_FILE );

    return 0;
}

static int writeLeftMenu (int eid, webs_t wp, int argc, char_t **argv)
{
    char line[2048];
    FILE *file;
#if defined(__P02003_Dora)
	#ifdef __USED_WEBS_DB__
			char * pTmp = jnrp_sql_getAttr_Webs ("FirewallNatConfig", "Enable", 1);
	#else
			char * pTmp = jnrp_sql_getAttr (pDBFile, "FirewallNatConfig", "Enable",1);
	#endif
#endif
    if (!gstrcmp(argv[0],"lte"))
        file = fopen(LEFT_LTE_MENU_FILE, "r");
    else if (!gstrcmp(argv[0],"wifi"))
	{
#if defined(__P02003_Dora)
	     //file = fopen(LEFT_WIFI_AC_MENU_FILE, "r");
	     if(!gstrcmp(pTmp,"1")){ //nat mode
			file = fopen(LEFT_WIFI_AC_MENU_FILE, "r");
		 }else{
			file = fopen(LEFT_WIFI_AC_BRIDGE_MENU_FILE, "r");
		 }
#else
         file = fopen(LEFT_WIFI_MENU_FILE, "r");
#endif
    }	
    else if (!gstrcmp(argv[0],"lan"))
    {
#if defined(__P02003_Dora)
		if(!gstrcmp(pTmp,"1")) //nat mode
		{
			file = fopen(LEFT_LAN_NAT_MENU_FILE, "r");
		}
		else
		{
			file = fopen(LEFT_LAN_MENU_FILE, "r");
		}
#else
        file = fopen(LEFT_LAN_MENU_FILE, "r");
#endif
    }
    else if (!gstrcmp(argv[0],"voip"))
        file = fopen(LEFT_VOIP_MENU_FILE, "r");
    else if (!gstrcmp(argv[0],"app"))
        file = fopen(LEFT_APP_MENU_FILE, "r");
    else if (!gstrcmp(argv[0],"adv"))
    {
#if defined(__P02003_Dora)
		if(!gstrcmp(pTmp,"1")) //nat mode
		{
			file = fopen(LEFT_ADV_NAT_MENU_FILE, "r");
		}
		else
		{
			file = fopen(LEFT_ADV_MENU_FILE, "r");
		}
    
#else
		file = fopen(LEFT_ADV_MENU_FILE, "r");
#endif
	}
    else if (!gstrcmp(argv[0],"sys"))
        file = fopen(LEFT_SYS_MENU_FILE, "r");
    else
        return 0;

    //file = fopen(filename, "r");
    if (file)
    {
        while (fgets ( line, sizeof line, file ) != NULL)
        {
            websWrite(wp, T("%s"), line);
        }
        fclose(file);
    }
    //else
      //  perror ( filename );

    return 0;
}

static void user_logout(webs_t wp, char_t *path, char_t *query)
{
#if defined(__P02003_Dora)
syslog(LOG_LOCAL5, "[Web GUI]: Logout IP Address: %s", wp->ipaddr);
#endif
    printf("user_logout start \n");
    //printf("path: %s \n",path);
    //printf("%s(%d)--: wp.userName:%s \n",__FILE__,__LINE__,wp->userName);
    //printf("wp.userName2:%s \n",wp->userName);
    //websError(wp, 401, T("Logout User ID"));
    //websSetRequestFlags(wp, wp->flags | WEBS_AUTH_DIGEST);
    //jnrp_sql_setAttr (pDBFile, "loginSession", "login", "0",1);
    // TODO: workaround here
    system ("echo 0 > /tmp/loginsession");
	system ("echo 1qaz2wsx%^wer > /tmp/loginsessionuserid");
	system ("echo rew^%xsw2zaq1 > /tmp/loginsessionpassword");

	//enhanced security issues
	memset(g_login_user_ip, 0, sizeof(g_login_user_ip));

	login_flag = LOGIN_DEFAULT;
    websRedirect(wp, T("login.asp"));
}

static void getintologin(int eid, webs_t wp, int argc, char_t **argv)
{
	//enhanced security issues
	if (strlen(g_login_user_ip) > 0) {
		if (gstrcmp(g_login_user_ip, wp->ipaddr))
		{
			login_flag == LOGIN_DEFAULT;
			websWrite(wp, T("\"%s\""), "1");
			return;
		}
	}
	memset(g_login_user_ip, 0, sizeof(g_login_user_ip));

	system ("echo 0 > /tmp/loginsession");
	system ("echo 1qaz2wsx%^wer > /tmp/loginsessionuserid");
	system ("echo rew^%xsw2zaq1 > /tmp/loginsessionpassword");

/*#if defined(__P02003_Dora)
	return websWrite(wp, T("\"%s\""), "1");
#else*/
#if defined(__P02003_Dora)
	getCurrentTime();
#endif
	if(login_flag == LOGIN_OK || login_flag == LOGIN_DEFAULT)
	{
		login_flag == LOGIN_DEFAULT;
		websWrite(wp, T("\"%s\""), "1");
		return;
	}
	else
	{
		login_flag == LOGIN_DEFAULT;
		websWrite(wp, T("\"%s\""), "0");
		return;
	}
//#endif
}

static void getuser_login(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *password, *accessLimit,*path,*value,*usergroup;
	char_t *userid;
	int    flags, nRet;
#if defined(__P02003_Dora)
	char* pIdleTime;
	char *pLoginResult = (char *)malloc(128);
	gstrcpy (pLoginResult,"");
#endif
#ifdef WEBS_LOGOUT_SUPPORT
	FILE *file,*file1,*file2;
	file = fopen("/tmp/loginsession", "r");
	file1 = fopen("/tmp/loginsessionuserid", "r");
    file2 = fopen("/tmp/loginsessionpassword", "r");
    char c;	
	char line[64],line2[64];
	if (file)
	{
		c = fgetc(file);
		if (c != '1')
		{
			userid = "1qaz2wsx";
			websWrite(wp, T("\"%s\""), "1");
			#if defined(__P02003_Dora)
			free(pLoginResult);
			#endif
			return;
		}
		else
		{
		}

		fclose(file);
	}
	else
		printf("%s(%d)--: failed to open /tmp/loginsession \n",__FILE__,__LINE__);

	if (file1)
    {
        while (fgets ( line, sizeof line, file1) != NULL)
        {
        	value=line;
        }
		int length = gstrlen(value)-1;
		char temp[128];
		gstrncpy(temp,value,length);
		temp[length]=0;
		userid = (char *)malloc(length);
    	gstrcpy(userid,temp);
		bfree(B_L, temp);

        fclose(file1);
    }
    else
        printf("%s(%d)--: failed to open /tmp/loginsessionuserid \n",__FILE__,__LINE__);

	if (file2)
    {
        while (fgets ( line2, sizeof line2, file2 ) != NULL)
        {
        	password=line2;
        }
        fclose(file2);
    }
    else
        printf("%s(%d)--: failed to open /tmp/loginsessionpassword \n",__FILE__,__LINE__);
#else
	password = websGetRequestPassword(wp);
    userid = websGetRequestUserName(wp);
#endif

	//enhanced security issues
	if (strlen(g_login_user_ip) > 0)
	{
		if (gstrcmp(g_login_user_ip, wp->ipaddr) != 0)
		{
			websWrite(wp, T("\"%s\""), "1");
			#if defined(__P02003_Dora)
			free(pLoginResult);
			#endif
			if (userid) free(userid);
			return;
		}
	}

	path = websGetRequestPath(wp);
#if !defined(__P02003_Dora)
	accessLimit = umGetAccessLimit(path);
	if (accessLimit == NULL) {
		websWrite(wp, T("\"%s\""), "1");
		#if defined(__P02003_Dora)
		free(pLoginResult);
		#endif
		if (userid) free(userid);
		return;
	}
#endif		
	usergroup = umGetUserGroup(userid);

	if (userid && *userid) 
	{	
        if (!umUserExists(userid)) 
		{
			websWrite(wp, T("\"%s\""), "1");
			#if defined(__P02003_Dora)
			free(pLoginResult);
			#endif
			free(userid);
			return;
        }		
#if !defined(__P02003_Dora)
		else if (!umUserCanAccessURL(userid, accessLimit)) 
		{
			websWrite(wp, T("\"%s\""), "1");
			#if defined(__P02003_Dora)
			free(pLoginResult);
			#endif
			free(userid);
			return;
        }
#endif
    }
	
#if defined(__P02003_Dora)
	char ** ppQryResult = NULL;
	char pGetIdleTimeQry [1024] = {0};
	memset(pGetIdleTimeQry, 0x00, sizeof(pGetIdleTimeQry));	
	snprintf(pGetIdleTimeQry, sizeof(pGetIdleTimeQry), "select _ROWID_ from users where username='%s'", userid);
	ppQryResult = jnrp_sql_getByQry_Webs (pGetIdleTimeQry);

	if (userid) free(userid);	   
	pIdleTime = jnrp_sql_getAttr_Webs("users","loginTimeout",gatoi(ppQryResult[1]));
	if(!gstrcmp(usergroup,"guest"))
	{
		gstrcat(pLoginResult,"2");
	}
	else
	{
		gstrcat(pLoginResult,"0");
	}
	gstrcat(pLoginResult,pSep);
	gstrcat(pLoginResult,pIdleTime);
	
	gstrcat(pLoginResult,pSep);
	gstrcat(pLoginResult,now_date);

	{
			char tzarray[15][3] = {"00", "01", "02", "03", "04", "05", "06", \
		"07", "08", "09", "10", "11", "12", "13", "14"};
			char tzmin[][3] = {"00", "30", "45"};
			long dtime = 0;
			short diffhour = 0;
			char diffmin[3] = {"00"};
			char *strtm;
			int dtime_index=0;
			time_t tm1, tm2;
			struct tm *t1, *t2;
			strtm = malloc(120 * sizeof(char));
			tm1 = time(NULL);
			t2 = gmtime(&tm1);
			tm2 = mktime(t2);
			t1 = localtime(&tm1);
#ifdef __USED_WEBS_DB__
			char * pQryResult = jnrp_sql_getAttr_Webs ("ntpConfig", "timezone", 1);
#else
			char * pQryResult = jnrp_sql_getAttr (pDBFile,"ntpConfig", "timezone", 1);	  
#endif
			int timezone = gatoi(pQryResult);
			//dtime = (long)(tm1 - tm2);
			dtime = timezone*60;
			//printf("dtime: %d \n",dtime);
		/* Print local time as a string */
			
			strftime(strtm, 100, "%a %b %d %Y %H:%M:%S GMT", t1);
		
			dtime_index = (short)(abs(dtime) / 3600);
			if(dtime_index == 0)
			{
			}
			else
			{
				if (dtime >= 0 )
					strcat(strtm, "+");
				else
					strcat(strtm, "-");
				strcat(strtm, tzarray[(short)(abs(dtime) / 3600)]);
				//strcat(strtm, ":");
				if (1800 == (short)(abs(dtime) % 3600))
					strcat(strtm, tzmin[1]);
				else if(2700 == (short)(abs(dtime) % 3600))
					strcat(strtm, tzmin[2]);
				else
					strcat(strtm, tzmin[0]);
			}
			gstrcat(pLoginResult,pSep);
			gstrcat(pLoginResult,strtm);
			free(now_date);
			now_date = malloc(64 * sizeof(char));
			gstrcpy(now_date,"");
			gstrcat(now_date,strtm);
			free(strtm);
		}
	websWrite(wp, T("\"%s\""), pLoginResult);
	free(pLoginResult);
	return;
#else
	if (userid) free(userid);
	if(!gstrcmp(usergroup,"guest"))
	{
		websWrite(wp, T("\"%s\""), "2");
		return;
	}
	websWrite(wp, T("\"%s\""), "0");
	return;
#endif
}

static void user_login(webs_t wp, char_t *path, char_t *query)
{
#if defined(__P02003_Dora)
 char pPin1Status[64];
 memset(pPin1Status, '\0', sizeof(pPin1Status));

 system("lteStatus.sh getPinConfig");

 FILE *fp = fopen("/tmp/getPinConfig", "r");
 if(fp)
 {
  fgets(pPin1Status, sizeof(pPin1Status), fp);
  if (pPin1Status[strlen(pPin1Status)-1] == '\n')
   pPin1Status[strlen(pPin1Status)-1] = '\0';
  fclose(fp);
 } else {
 	gstrcpy(pPin1Status,"-1");
 }

 //system ("lte_get -p --timeout 5000 --update-db > /dev/NULL");
 //sleep(1);

 //char * pPin1Status = jnrp_sql_getAttrByCondit_Webs("ltePinStatus","pinStatus","pinId='1'");
 char * pPin2PinValue = jnrp_sql_getAttrByCondit_Webs("ltePinStatus","pinValue","pinId='2'");

 P02003_login_pin_status = 0;
 if (!gstrcmp(pPin1Status, "2") || !gstrcmp(pPin1Status, "4") || !gstrcmp(pPin1Status, "1") || !gstrcmp(pPin1Status, "3"))
 {
 } else {
 	if (!gstrcmp(pPin2PinValue, "1234"))
 	{
#if 0 	
 		if (!gstrcmp(pPin1Status, "-1")) {
			P02003_login_pin_status = -1;
		} else {
			P02003_login_pin_status = 1;
		}
		websRedirect(wp, T("login.asp"));
		return;
#else
		if (!gstrcmp(pPin1Status, "-1")) {
			P02003_login_pin_status = -1;
			websRedirect(wp, T("login.asp"));
			return;
		}
	
#endif
	}
	
 }
#endif
	
    char_t          *type, *userid, *password, *accessLimit;
    int             flags, nRet;
    accessMeth_t    am;
	char_t * pUserid = websGetVar(wp, T("login_username"), T(""));
	char_t * pPassword = websGetVar(wp, T("login_password"), T(""));
	char p[512];

    a_assert(websValid(wp));
    a_assert(path && *path);
	
	sprintf(p,"echo %s > /tmp/loginsessionuserid",pUserid);	
	system(p);
	sprintf(p,"echo %s > /tmp/loginsessionpassword",pPassword);	
	system(p);

	//enhanced security issues
	memset(g_login_user_ip, 0, sizeof(g_login_user_ip));
	memcpy(g_login_user_ip, wp->ipaddr, sizeof(wp->ipaddr));
	
	char_t *value;
	char_t   *user_id;

/*
 *  Get the critical request details
 */
    password = pPassword;
    userid = pUserid;
	
#if defined(__P02003_Dora)
	getCurrentTime();
#endif
	if(umUserExists(userid))
	{
		char_t * userpass = umGetUserPassword(userid);
        if (userpass) 
		{
            if (gstrcmp(password, userpass) != 0) 
			{
				login_flag = LOGIN_PASSWORD_ERROR;
				websRedirect(wp, T("login.asp"));
            } 
			else
			{
				login_flag = LOGIN_OK;
				#ifdef WEBS_LOGOUT_SUPPORT
	    			system("echo 1 > /tmp/loginsession");
	    		#endif

#if defined(__P02003_Dora)
syslog(LOG_LOCAL5, "[Web GUI]: Login Username: %s, IP Address: %s", pUserid, wp->ipaddr);
#endif

				websRedirect(wp, T("home.asp"));
            }

            bfree (B_L, userpass);
        }
		else
		{
			login_flag = LOGIN_NO_PASSWORD;
			websRedirect(wp, T("login.asp"));
		}
		
	}
	else 
	{
		login_flag = LOGIN_NO_USERNAME;
		websRedirect(wp, T("login.asp"));
	}
	
	return;
}


static int getConf(int eid, webs_t wp, int argc, char_t **argv)
{	
	if(argv[0] == NULL) 
	{
		websWrite(wp, T("\"%s\""),"");	
		return 0;
	}
		
	//command : source /sysconfig/xxx-cfg
	//command : echo $argv[0]	
	char cmdbuffer[100] = {0};
	char buffer[1024] = {0};
	FILE *fd = -1;
	
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));
	memset(buffer, '\0', sizeof(buffer));

	sprintf(cmdbuffer,"cfg.sh %s",argv[0]);
	fd = popen(cmdbuffer,"r");		
	if(fgets(buffer,sizeof(buffer),fd) == NULL)
	{
	    websWrite(wp, T("\"%s\""),"");
	}
	else
	{
		if (buffer[strlen(buffer)-1] == '\n')
		 buffer[strlen(buffer)-1] = '\0';			
		websWrite(wp, T("\"%s\""),buffer);		
	}
	pclose(fd);	
	return 0;
}

#if defined(__P02003_Dora)
static int getActionResult(int eid, webs_t wp, int argc, char_t **argv)
{
	printf("%s result_msg=%s\n", __func__,result_msg);  	
	websWrite(wp, T("<script>document.write(gettext(\"%s\"));</script>"),result_msg);		
	setResultCode("");	
	return 0;
}	

int checkUrlProfile(char *groupName, char *wp_ipaddr)
{
	char pBuffCmd [128] = {0};
	memset (pBuffCmd, 0x00, sizeof(pBuffCmd));

	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from UrlProfile";
	int index = 0;
	char* pUserName = NULL;
	char* pProfileName = NULL;
	int result=0;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "UserName", pUserName, checkUrlProfileOut);		
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ProfileName", pProfileName, checkUrlProfileOut);
			printf("index=%d, pUserName=%s, pProfileName=%s\n", index+1, pUserName, pProfileName);					
			if (gstrcmp(groupName,pUserName)==0)
			{
				result=1;
				snprintf(pBuffCmd,sizeof(pBuffCmd), "/usr/local/bin/schedule_rule.sh access_network %s PC%s", wp_ipaddr, pProfileName);
				system(pBuffCmd);
			}			
		}
	}
	
checkUrlProfileOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	if (result == 1) {
		return 1;
	} else {
		return 0;
	}
}

char * checkLoginAccount(char *loginUsername, char *loginPassword)
{
	//printf("%s loginUsername=%s,loginPassword=%s\n", __func__,loginUsername,loginPassword);	
	int nRows=jnrp_sql_getRowNum_Webs("users");
	int index=0;
	char *pGroupName=NULL;	
	
	for(index=1;index<=nRows;index++)
	{
		char *pUserName = jnrp_sql_getAttr_Webs("users", "username", index);
		char *pPassword = jnrp_sql_getAttr_Webs("users", "password", index);		
		//printf("pUserName=%s,pPassword=%s\n",pUserName,pPassword);
		if (!gstrcmp(loginUsername,pUserName) && !gstrcmp(loginPassword,pPassword))
		{   //user account match
			pGroupName = jnrp_sql_getAttr_Webs("users", "groupname", index);
			//printf("pGroupName=%s\n",pGroupName);
		}
	}
	if (pGroupName==NULL)
		pGroupName = "";
		
	return pGroupName;	
}

static void loginUrlFilter(webs_t wp, char_t *path, char_t *query)
{
	//printf("%s\n", __func__);
	char_t * pLoginUsername = websGetVar(wp, T("users.username"), T(""));
	char_t * pLoginPassword = websGetVar(wp, T("users.password"), T(""));

	//check parental control status
	char * pchkParCtrlEnable = jnrp_sql_getAttr_Webs("UrlFltrConfig","EnableProfiles",1);	
	int pParCtrlEnable=gatoi(pchkParCtrlEnable);
	if (pParCtrlEnable)
	{	
		char * pGroupName=checkLoginAccount(pLoginUsername,pLoginPassword);
		if (gstrcmp(pGroupName,""))
		{	//check if login group exist in url profiles
			if (checkUrlProfile(pGroupName, wp->ipaddr)) 
			{
				websRedirect(wp, T("urlFilterLogout.asp"));
			}
			else
			{
				setResultCode("The user account cannot access Internet.");
				websRedirect(wp, T("urlFilterLogin.asp"));				
			}
		}
		else
		{	//cannot find login data	
			setResultCode("Please enter valid Username and Password.");
			websRedirect(wp, T("urlFilterLogin.asp"));
		}
	}
	else
	{
			websRedirect(wp, T("urlFilterLogin.asp"));
	}
	return;
}

static void logoutUrlFilter(webs_t wp, char_t *path, char_t *query)
{
 char pBuffCmd [128] = {0};
 memset (pBuffCmd, 0x00, sizeof(pBuffCmd));
 snprintf(pBuffCmd,sizeof(pBuffCmd), "/usr/local/bin/schedule_rule.sh session_timeout %s", wp->ipaddr);
 //printf("logoutUrlFilter, pBuffCmd=%s\n", pBuffCmd);
 system(pBuffCmd);
 websRedirect(wp, T("urlFilterLogin.asp"));
 return;
}
#endif
