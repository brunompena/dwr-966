#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

#define GETLTEFROMFILE
#if defined(GETLTEFROMFILE)
 #define WANINFO "/tmp/getWanInfo"
 #define NETWORKINFO "/tmp/getNetworkInfo"
 #define APNSTATUS "/tmp/getIPStatus"
#endif
#if !defined(__P02001_Dora) && !defined(__P02002_Dora) && !defined(__P02003_Dora)
#define FINETUNEHOMEPAGE
#endif
void initHomeWeb();
// Home page information
static int getSysTime (int eid, webs_t wp, int argc, char_t **argv);
static int getSysInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getWanInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getNetworkInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getWiFiInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getModuleVer(int eid, webs_t wp, int argc, char_t **argv);

#if defined(__P02001_Dora) || defined(__P02002_Dora)
static int getWifiRadio(int eid, webs_t wp, int argc, char_t **argv);
static int getWiFiClient(int eid, webs_t wp, int argc, char_t **argv);
static int getHomeAPNStatus(int eid, webs_t wp, int argc, char_t **argv);
#endif
static int getLanInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getVoipInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getNtpTimezone (int eid, webs_t wp, int argc, char_t **argv);
static int getCurrentTime (int eid, webs_t wp, int argc, char_t **argv);
static int getConnectionTime (int eid, webs_t wp, int argc, char_t **argv);
static int getStatusBar (int eid, webs_t wp, int argc, char_t **argv);
static int getNetwork(int eid,webs_t wp,int argc,char_t * * argv);
#if defined(__P02003_Dora)
static int getHomeInternetInfo1(int eid,webs_t wp,int argc,char_t * * argv);
static int getHomeInternetInfo2(int eid,webs_t wp,int argc,char_t * * argv);
static int getHomeInternetInfo3(int eid,webs_t wp,int argc,char_t * * argv);
static int getHomeWiFiRadio(int eid, webs_t wp, int argc, char_t **argv);
static int getHomeTotalWiFiClient(int eid,webs_t wp,int argc,char_t * * argv);
static int getHomeWiFiStatus(int eid,webs_t wp,int argc,char_t * * argv);
//Landing Page
int setResetDefaultReady = 0;
static int aspSetResetDefaultFinish(int eid, webs_t wp, int argc, char_t **argv);
int setupWizardReadyDisplay = 0;
static int setSetupWizardReady(int eid,webs_t wp,int argc,char_t * * argv);
static int getSetupWizardFinish(int eid,webs_t wp,int argc,char_t * * argv);
static void setSetupWizardFinish(webs_t wp, char_t *path, char_t *query);
static int getSetupWizardWiFiInfo(int eid,webs_t wp,int argc,char_t * * argv);
static int getSetupWizardAdminInfo(int eid,webs_t wp,int argc,char_t * * argv);
static int getSetupWizardCurPINCode(int eid,webs_t wp,int argc,char_t * * argv);
static void setSetupWizardVerifyPIN(webs_t wp, char_t *path, char_t *query);
static void setSetupWizardUnblockPUK(webs_t wp, char_t *path, char_t *query);
static void setSetupWizardChangePin(webs_t wp, char_t *path, char_t *query);
#endif

#if !defined(__P02001_Dora) && !defined(__P02002_Dora)
static int getHomePage (int eid, webs_t wp, int argc, char_t **argv);
static int getWiFiAPInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getFirewallInfo (int eid, webs_t wp, int argc, char_t **argv);

#if defined(FINETUNEHOMEPAGE)
static char pSysInfo[128];
static char pWiFiInfo[128];
static char pWiFiAPInfo[128];
static char pLanInfo[128];
static char pCurrentTime[128];
static char pVoipInfo[128];
static char pFirewallInfo[128];
static int getHomePage (int eid, webs_t wp, int argc, char_t **argv)
{
 sqlite3         *db = NULL;
 sqlite3_stmt    *stmt = NULL;
 int             rc = 0;
 const char      *data = NULL;
 char * sqlCmd = NULL;

 sqlite3_initialize();
 rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
 if (rc != SQLITE_OK) {
  sqlite3_close( db );
  printf("open sqlite not ok \n");
  return 0;
 }
 sqlite3_busy_timeout(db, 1500);

 //sqlite3 query, getSysInfo()
 sqlCmd = "select name, firmwareVer, hardwareVer, moduleVer from system";
 rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
 if (rc != SQLITE_OK)
 {
   printf("prepare sqlite not ok \n");
   return 0;
 }
 sqlite3_step( stmt );
 memset(pSysInfo,'\0',sizeof(pSysInfo));
 gstrcat(pSysInfo,sqlite3_column_text(stmt, 0));
 gstrcat(pSysInfo,pSep);
 gstrcat(pSysInfo,sqlite3_column_text(stmt, 1));
 gstrcat(pSysInfo,pSep);
 gstrcat(pSysInfo,sqlite3_column_text(stmt, 2));
 gstrcat(pSysInfo,pSep);
 gstrcat(pSysInfo,sqlite3_column_text(stmt, 3));
 sqlite3_finalize( stmt );

 //sqlite3 query, getWiFiInfo()
 sqlCmd = "SELECT ssid, security FROM wifiProfile";
 rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
 if (rc != SQLITE_OK)
 {
   printf("prepare sqlite not ok \n");
   return 0;
 }

 memset(pWiFiInfo,'\0',sizeof(pWiFiInfo));
 while( sqlite3_step( stmt ) == SQLITE_ROW )
 {
  gstrcat(pWiFiInfo,sqlite3_column_text(stmt, 0));
  gstrcat(pWiFiInfo,pSep);
  gstrcat(pWiFiInfo,sqlite3_column_text(stmt, 1));
  gstrcat(pWiFiInfo,pSep);
 }
 sqlite3_finalize( stmt );

 //sqlite3 query, getWiFiAPInfo()
 sqlCmd = "SELECT wlanEnabled FROM wifiAP";
 rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
 if (rc != SQLITE_OK)
 {
   printf("prepare sqlite not ok \n");
   return 0;
 }

 memset(pWiFiAPInfo,'\0',sizeof(pWiFiAPInfo));
 while( sqlite3_step( stmt ) == SQLITE_ROW )
 {
  gstrcat(pWiFiAPInfo,sqlite3_column_text(stmt, 0));
  gstrcat(pWiFiAPInfo,pSep);
 }
 sqlite3_finalize( stmt );

 //sqlite3 query, getLanInfo()
 system ("lanportstatus");
 sqlCmd = "SELECT speed, linkstatus FROM LANStatus";
 rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
 if (rc != SQLITE_OK)
 {
   printf("prepare sqlite not ok \n");
   return 0;
 }

 memset(pLanInfo,'\0',sizeof(pLanInfo));
 while( sqlite3_step( stmt ) == SQLITE_ROW )
 {
  gstrcat(pLanInfo,sqlite3_column_text(stmt, 0));
  gstrcat(pLanInfo,pSep);
  if(strcmp(sqlite3_column_text(stmt, 1),"1")==0)
    gstrcat(pLanInfo,"connected");
  else
    gstrcat(pLanInfo,"not connected");
  gstrcat(pLanInfo,pSep);
 }
 sqlite3_finalize( stmt );

 //sqlite3 query, getCurrentTime(), getNtpTimezone()
 sqlCmd = "SELECT timezone FROM ntpConfig where ROWID=1";
 rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
 if (rc != SQLITE_OK)
 {
   printf("prepare sqlite not ok \n");
   return 0;
 }
 sqlite3_step( stmt );
 memset(pCurrentTime,'\0',sizeof(pCurrentTime));
 gstrcat(pCurrentTime,sqlite3_column_text(stmt, 0));
 sqlite3_finalize( stmt );

 //sqlite3 query, getVoipInfo()
 sqlCmd = "SELECT State, LineStatus FROM VoiceLine";
 rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
 if (rc != SQLITE_OK)
 {
   printf("prepare sqlite not ok \n");
   return 0;
 }

 memset(pVoipInfo,'\0',sizeof(pVoipInfo));
 while( sqlite3_step( stmt ) == SQLITE_ROW )
 {
  gstrcat(pVoipInfo,sqlite3_column_text(stmt, 0));
  gstrcat(pVoipInfo,pSep);
  gstrcat(pVoipInfo,sqlite3_column_text(stmt, 1));
  gstrcat(pVoipInfo,pSep);
 }
 sqlite3_finalize( stmt );

 //sqlite3 query, getFirewallInfo()
 sqlCmd = "SELECT PacketFilterEnable, MacFilterEnable, UrlFilterEnable FROM FirewallConfig";
 rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
 if (rc != SQLITE_OK)
 {
   printf("prepare sqlite not ok \n");
   return 0;
 }

 sqlite3_step( stmt );
 memset(pFirewallInfo,'\0',sizeof(pFirewallInfo));
 gstrcat(pFirewallInfo,sqlite3_column_text(stmt, 0));
 gstrcat(pFirewallInfo,pSep);
 gstrcat(pFirewallInfo,sqlite3_column_text(stmt, 1));
 gstrcat(pFirewallInfo,pSep);
 gstrcat(pFirewallInfo,sqlite3_column_text(stmt, 2));
 gstrcat(pFirewallInfo,pSep);
 sqlite3_finalize( stmt );

 sqlite3_close(db);
 sqlite3_shutdown();

 return 0;
}

static int getWiFiAPInfo (int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getWiFiAPInfo, %s\n", pWiFiAPInfo);
  return websWrite(wp, T("\"%s\""), pWiFiAPInfo);
}

static int getFirewallInfo (int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getFirewallInfo, %s\n", pFirewallInfo);
  return websWrite(wp, T("\"%s\""), pFirewallInfo);
}
#else
static int getHomePage (int eid, webs_t wp, int argc, char_t **argv)
{
 return 0;
}

static int getWiFiAPInfo (int eid, webs_t wp, int argc, char_t **argv)
{
  char pWiFiAPInfo[128];
  memset(pWiFiAPInfo,'\0',sizeof(pWiFiAPInfo));

#ifdef __USED_WEBS_DB__
 char * wlan1Enabled = jnrp_sql_getAttrByCondit_Webs("wifiAP","wlanEnabled","wlanName='ap1'");
 char * wlan2Enabled = jnrp_sql_getAttrByCondit_Webs("wifiAP","wlanEnabled","wlanName='ap2'");
#else
 char * wlan1Enabled = jnrp_sql_getAttrByCondit(pDBFile,"wifiAP","wlanEnabled","wlanName='ap1'");
 char * wlan2Enabled = jnrp_sql_getAttrByCondit(pDBFile,"wifiAP","wlanEnabled","wlanName='ap2'");
#endif

  gstrcat(pWiFiAPInfo,wlan1Enabled);
  gstrcat(pWiFiAPInfo,pSep);
  gstrcat(pWiFiAPInfo,wlan2Enabled);

  return websWrite(wp, T("\"%s\""), pWiFiAPInfo);
}

static int getFirewallInfo (int eid, webs_t wp, int argc, char_t **argv)
{
  char pFirewallInfo[256];

  int nRows=0,nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "SELECT PacketFilterEnable, MacFilterEnable, UrlFilterEnable FROM FirewallConfig";

  char* pPacketFilterEnable = NULL;
  char* pMacFilterEnable = NULL;
  char* pUrlFilterEnable = NULL;

  memset(pFirewallInfo,'\0',sizeof(pFirewallInfo));
#ifdef __USED_WEBS_DB__
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
  sqlite3 * pWebDB = NULL;
  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
    //sqlite3_free(pQueryStr);
    return websWrite(wp, T("\"%s\""),pFirewallInfo);
  }
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "PacketFilterEnable", pPacketFilterEnable, getFirewallInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "MacFilterEnable", pMacFilterEnable, getFirewallInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UrlFilterEnable", pUrlFilterEnable, getFirewallInfoOut);

  gstrcat(pFirewallInfo,pPacketFilterEnable);
  gstrcat(pFirewallInfo,pSep);
  gstrcat(pFirewallInfo,pMacFilterEnable);
  gstrcat(pFirewallInfo,pSep);
  gstrcat(pFirewallInfo,pUrlFilterEnable);

getFirewallInfoOut:
  if (ppQueryRes)
  {
    sqlite3_free_table(ppQueryRes);
  }

  return websWrite(wp, T("\"%s\""), pFirewallInfo);
}
#endif
#endif
static int getSysTime (int eid, webs_t wp, int argc, char_t **argv)
{
    // get time
    char sUptime [128] = {0};
    memset (sUptime, 0x00, sizeof(sUptime));
    long upSec = get_uptime();
    int sec = upSec%60;
    int min = upSec/60;
    int hour = min/60;
    //int day = hour/60;
    int day = hour/24;
    min = min%60;
	hour = hour%24;
    snprintf(sUptime,sizeof(sUptime),"%d:%d:%d:%d",day,hour,min,sec);
    return websWrite(wp, T("%s"),sUptime);
}

static int getSysInfo (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef FINETUNEHOMEPAGE
  //printf("getSysInfo, %s\n", pSysInfo);
  return websWrite(wp, T("\"%s\""), pSysInfo);
#else

 char * pName = jnrp_sql_getAttr_Webs ("system", "name", 1);
 char * pFirmwareVer = jnrp_sql_getAttr_Webs ("system", "firmwareVer", 1);
 char * pHardwareVer = jnrp_sql_getAttr_Webs ("system", "hardwareVer", 1);

 char pSysInfo [256] = {0};
 memset (pSysInfo, 0x00, sizeof(pSysInfo));

 gstrcat(pSysInfo, pName);
 gstrcat(pSysInfo, pSep);
 gstrcat(pSysInfo, pFirmwareVer);
 gstrcat(pSysInfo, pSep);
 gstrcat(pSysInfo, pHardwareVer);
 gstrcat(pSysInfo, pSep);
 
 return websWrite(wp, T("\"%s\""), pSysInfo);
#endif
}

static int getWanInfo (int eid, webs_t wp, int argc, char_t **argv)
{
/*#ifdef GETLTEFROMFILE

 char pWanInfo[256];

 memset(pWanInfo,'\0',sizeof(pWanInfo));
 system("lteStatus.sh getWanInfo");

 FILE *fp = fopen(WANINFO, "r");
 char line[128];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   gstrcat(pWanInfo,line);
   gstrcat(pWanInfo,pSep);
  }
  fclose(fp);
 } else {
  return websWrite(wp, T("\"%s\""),"##");
 }

 //remove(WANINFO);

 return websWrite(wp, T("\"%s\""), pWanInfo);

#else*/

    char pWanInfo[128];

    int nRows=0,nCols=0;
    char** ppQueryRes = NULL;
    char * pQueryStr = "SELECT * FROM lteIPStatus WHERE qid=1";

    char* pIpAddr = NULL;
    char* pGateway = NULL;
    char* pDNS1 = NULL;
	char* pNetmask = NULL;

    memset(pWanInfo,'\0',sizeof(pWanInfo));
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        return websWrite(wp, T("\"%s\""),pWanInfo);
    }

    if (nRows > 0) {
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "ip", pIpAddr, getWanInfoOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "gateway", pGateway, getWanInfoOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "primary_dns", pDNS1, getWanInfoOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "netmask", pNetmask, getWanInfoOut);

      gstrcat(pWanInfo,pIpAddr);
      gstrcat(pWanInfo,pSep);
      gstrcat(pWanInfo,pGateway);
      gstrcat(pWanInfo,pSep);
      gstrcat(pWanInfo,pDNS1);
	  gstrcat(pWanInfo,pSep);
      gstrcat(pWanInfo,pNetmask);
    }

getWanInfoOut:
    if (ppQueryRes)
    {
	sqlite3_free_table(ppQueryRes);
    }

    return websWrite(wp, T("\"%s\""),pWanInfo);

//#endif
}

static int getNetworkInfo (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GETLTEFROMFILE

 char pNwInfo[128];

 memset(pNwInfo,'\0',sizeof(pNwInfo));
 system("lteStatus.sh getNetworkInfo");

 FILE *fp = fopen(NETWORKINFO, "r");
 char line[64];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   gstrcat(pNwInfo,line);
   gstrcat(pNwInfo,pSep);
  }
  fclose(fp);
 } else {
  return websWrite(wp, T("\"%s\""),"#");
 }

 //remove(NETWORKINFO);

 return websWrite(wp, T("\"%s\""),pNwInfo);

#else

 char pNwInfo[128];

 memset(pNwInfo,'\0',sizeof(pNwInfo));

 system ("lte_get -n -q 0 --timeout 5000 --update-db > /dev/NULL");
 //system ("lte_get --signal-strength -q 0 --timeout 5000 --update-db > /dev/NULL");
 system ("sleep 1");

#ifdef __USED_WEBS_DB__
 char * pNetworkName = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","networkName","qid=1");
 char * pInternet = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","radioInterference","qid=1");
#if defined(__P02001_Dora) || defined(__P02002_Dora)
 char * pRegistStatus = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","registrationState","qid=1");
#endif
#else
 char * pNetworkName = jnrp_sql_getAttrByCondit(pDBFile,"lteNwStatus","networkName","qid=1");
 char * pInternet = jnrp_sql_getAttrByCondit(pDBFile,"lteNwStatus","radioInterference","qid=1");
#if defined(__P02001_Dora) || defined(__P02002_Dora)
 char * pRegistStatus = jnrp_sql_getAttrByCondit(pDBFile,"lteNwStatus","registrationState","qid=1");
#endif
#endif

 gstrcat(pNwInfo,pNetworkName);
 gstrcat(pNwInfo,pSep);
 gstrcat(pNwInfo,pInternet);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
 gstrcat(pNwInfo,pSep);
 gstrcat(pNwInfo,pRegistStatus);
#endif
 gstrcat(pNwInfo,pSep);

 return websWrite(wp, T("\"%s\""),pNwInfo);

#endif
}

static int getWiFiInfo (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef FINETUNEHOMEPAGE
    //printf("getWiFiInfo, %s\n", pWiFiInfo);
    return websWrite(wp, T("\"%s\""),pWiFiInfo);
#else
    char pWiFiInfo[256];

    int nRows=0,nCols=0;
    char** ppQueryRes = NULL;
    char * pQueryStr = "SELECT ssid, security FROM wifiProfile";

    int index = 0;
    char* pSSID = NULL;
    char* pSSID_encry = NULL;

    memset(pWiFiInfo,'\0',sizeof(pWiFiInfo));
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        //sqlite3_free(pQueryStr);
        return websWrite(wp, T("\"%s\""),pWiFiInfo);
    }

    if (nRows > 0) {
    	for (index = 0; index < nRows; index++) {
      //free(pSSID);
      //free(pSSID_encry);

      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ssid", pSSID, getWiFiInfoOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "security", pSSID_encry, getWiFiInfoOut);

      gstrcat(pWiFiInfo,pSSID);
      gstrcat(pWiFiInfo,pSep);
      gstrcat(pWiFiInfo,pSSID_encry);
      gstrcat(pWiFiInfo,pSep);
     }
    }

getWiFiInfoOut:

    if (ppQueryRes)
    {
      sqlite3_free_table(ppQueryRes);
    }

    return websWrite(wp, T("\"%s\""),pWiFiInfo);
#endif
}

static int getModuleVer(int eid, webs_t wp, int argc, char_t **argv)
{
	char * pErrMsg = NULL;
	int status = -1;
	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	char sModuleVer [128] = {0};
    memset (sModuleVer, 0x00, sizeof(sModuleVer));

    system("lte_get -D1 | awk -F_ '{print $2}' | awk -F- '{print $1}' | awk -F- '{print $1}' | cut -d' ' -f 1 > /tmp/lteModuleVer");
 	FILE *fp = fopen("/tmp/lteModuleVer", "r");
 	if(fp) {
	  	while(fgets(sModuleVer, sizeof(sModuleVer), fp) != NULL){
	   		if (sModuleVer[strlen(sModuleVer)-1] == '\n')
	     	sModuleVer[strlen(sModuleVer)-1] = '\0';
	  	}
  		fclose(fp);
	} else {
			return websWrite(wp, T("%s"), "v01.00");
	}

	//update DB value
	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK) {
		printf ("unable to open the DB file %s\n", pDBFile);
	}
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update system set moduleVer='%s'", sModuleVer);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
	
	if (pDB)
		sqlite3_close (pDB);

	if (status>=0){
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD);
	}
	
	//return module ver string to webui
	return websWrite(wp, T("\"%s\""), sModuleVer);
}

#if defined(__P02001_Dora) || defined(__P02002_Dora)
static int getWifiRadio(int eid, webs_t wp, int argc, char_t **argv)
{
	    char** ppQueryRes = NULL;
	    int nRows=0,nCols=0,status=0;
	    char* pQueryStr = NULL;

	    char* pMode = NULL;
	    char* pChannel = NULL;
	    char* pWidth = NULL;
	    char* pSideBand = NULL;

	    sqlite3 * pWebDB = NULL;
		int ret = 0;
	    char *pWifi = (char *)malloc(128);

		pQueryStr = "SELECT * FROM wifiRadio where radioNo='1'";

	    gstrcpy(pWifi,"");
	    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
	    {
	        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
	        sqlite3_free (pQueryStr);
			free(pWifi);
	        return -1;
	    }

	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "opMode",pMode, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "configuredChannel",pChannel, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "chanWidth",pWidth, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "sideBand",pSideBand, getWifiConfigOut);

	    if (ppQueryRes)
	    {
	        sqlite3_free_table (ppQueryRes);
	        gstrcat(pWifi,pMode);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pChannel);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWidth);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pSideBand);
	    }

	getWifiConfigOut:
		ret = websWrite(wp, T("\"%s\""), pWifi);
		free(pWifi);
	    return ret;
}

//Get Wi-Fi total client
static int getWiFiClient(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
    char* pQueryStr = NULL;
    
    char* pPeers1 = NULL;
    char* pPeers2 = NULL;
	int totalPeer = 0;
    
    sqlite3 * pWebDB = NULL;
    char pCWMPQry [1024] = {0};
    pQueryStr = "SELECT * FROM wifiStatus";

	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
    snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiClient set triger ='1'");
    #ifdef __USED_WEBS_DB__
    	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
    #else
    	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
    #endif	
    
    if (status>=0)
    {
    	system(EXPORT_CMD);
    }
	
    sleep(1);
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
    {
    printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
    sqlite3_free (pQueryStr);
    return -1;
    }
    
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "peers1",pPeers1, getWifiConfigOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "peers2",pPeers2, getWifiConfigOut);

	totalPeer = gatoi(pPeers1) + gatoi(pPeers2);
	//printf ("[getWiFiClient]peers1: %s, peers2: %s, totalPeer: %d\n", pPeers1, pPeers2, totalPeer);
    
    getWifiConfigOut:
    return websWrite(wp, T("\"%d\""),totalPeer);
}

static int getHomeAPNStatus (int eid, webs_t wp, int argc, char_t **argv)
{
    char pAPNStatus[128];
    memset(pAPNStatus, '\0', sizeof(pAPNStatus));
    system("lteStatus.sh getAPNStatus");
    
    FILE *fp = fopen(APNSTATUS, "r");
    char line[64];
    memset(line, '\0', sizeof(line));
    if(fp){
    	while(fgets(line, sizeof(line), fp) != NULL){
    		if (line[strlen(line)-1] == '\n')
     			line[strlen(line)-1] = '\0';
    		gstrcat(pAPNStatus,line);
    		gstrcat(pAPNStatus,pSep);
    	}
    	fclose(fp);
    } else {
       	return websWrite(wp, T("%s"), "#");
 	}

	return websWrite(wp, T("%s"), pAPNStatus);
}
#endif

#if 0
static int getLanInfo (int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
    char *pLanInfo = (char *)malloc(128);
    char pLanQry [1024] = {0};
    memset (pLanQry, 0x00, sizeof(pLanQry));

    if ( (argc == 1 )&& argv[0] && (strcmp(argv[0],"preload")==0))
        system ("lanportstatus");

    if ( (argc == 2 )&& argv[0] && argv[1])
    {
        snprintf (pLanQry, sizeof(pLanQry),"select %s from LANStatus where portNumber='%s'",argv[1],argv[0]);
#ifdef __USED_WEBS_DB__
        char ** ppQryResult = jnrp_sql_getByQry_Webs (pLanQry);
#else
        char ** ppQryResult = jnrp_sql_getByQry (pDBFile, pLanQry);
#endif
        if (ppQryResult[1] && (strcmp(argv[1],"linkstatus")==0))
        {
            if (strcmp(ppQryResult[1],"1")==0)
                strcpy(pLanInfo,"connected");
            else
                strcpy(pLanInfo,"not connected");
        }
        else if (ppQryResult[1] && (strcmp(argv[1],"speed")==0))
            strcpy(pLanInfo,ppQryResult[1]);
        else
           strcpy(pLanInfo,"");
    }
    else
        strcpy(pLanInfo,"");

	ret = websWrite(wp, T("%s"), pLanInfo);
	free(pLanInfo);
    return ret;
}
#else
static int getLanInfo (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef FINETUNEHOMEPAGE
  //printf("getLanInfo, %s\n", pLanInfo);
  return websWrite(wp, T("\"%s\""), pLanInfo);
#else
    char pLanInfo[128];

    int nRows=0,nCols=0;
    char** ppQueryRes = NULL;
    char * pQueryStr = "SELECT speed, linkstatus FROM LANStatus";
    sqlite3 * pWebDB = NULL;

    int index = 0;
    char* pSpeed = NULL;
    char* pLinkStatus = NULL;

    system ("lanportstatus");

    memset(pLanInfo,'\0',sizeof(pLanInfo));
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        //sqlite3_free(pQueryStr);
        return websWrite(wp, T("\"%s\""),pLanInfo);
    }

    if (nRows > 0) {
    	for (index = 0; index < nRows; index++) {
      //free(pSpeed);
      //free(pLinkStatus);

      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "speed", pSpeed, getLanInfoOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "linkstatus", pLinkStatus, getLanInfoOut);

      gstrcat(pLanInfo,pSpeed);
      gstrcat(pLanInfo,pSep);
      if (strcmp(pLinkStatus,"1")==0)
        gstrcat(pLanInfo,"connected");
      else
        gstrcat(pLanInfo,"not connected");
      gstrcat(pLanInfo,pSep);
      }
    }

getLanInfoOut:
    if (ppQueryRes)
    {
      sqlite3_free_table(ppQueryRes);
    }

    return websWrite(wp, T("\"%s\""),pLanInfo);
#endif
}
#endif

static int getVoipInfo (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef FINETUNEHOMEPAGE
  //printf("getVoipInfo, %s\n", pVoipInfo);
  return websWrite(wp, T("\"%s\""), pVoipInfo);
#else
    char pVoipInfo[128];

    int nRows=0,nCols=0;
    char** ppQueryRes = NULL;
    char * pQueryStr = "SELECT State, LineStatus FROM VoiceLine";

    int index = 0;
    char* pState = NULL;
    char* pLineStatus = NULL;

    memset(pVoipInfo,'\0',sizeof(pVoipInfo));
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        //sqlite3_free(pQueryStr);
        return websWrite(wp, T("\"%s\""),pVoipInfo);
    }

    if (nRows > 0) {
    	for (index = 0; index < nRows; index++) {
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "State", pState, getVoiopInfoOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "LineStatus", pLineStatus, getVoiopInfoOut);

      gstrcat(pVoipInfo,pState);
      gstrcat(pVoipInfo,pSep);
      gstrcat(pVoipInfo,pLineStatus);
      gstrcat(pVoipInfo,pSep);
     }
    }

getVoiopInfoOut:
    if (ppQueryRes)
    {
      sqlite3_free_table(ppQueryRes);
    }

    return websWrite(wp, T("\"%s\""),pVoipInfo);
#endif
}

static int getNtpTimezone (int eid, webs_t wp, int argc, char_t **argv)
{
	char pTimeZone[128];
	int count = sizeof (web_Enum_TimeZoneList) / sizeof (CGI_ENUMSEL_S);
#ifdef FINETUNEHOMEPAGE
 char * pQryResult = NULL;
 pQryResult = &pCurrentTime[0];
#else
#ifdef __USED_WEBS_DB__
	char * pQryResult = jnrp_sql_getAttr_Webs("ntpConfig", "timezone",1);
#else
	char * pQryResult = jnrp_sql_getAttr(pDBFile,"ntpConfig", "timezone",1);
#endif
#endif
	int timezone = gatoi(pQryResult);
	int i =0;
	memset(pTimeZone,'\0',sizeof(pTimeZone));

	switch (timezone)
	{
	 case -720:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-12:00)");
		break;
	 case -660:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-11:00)");
		break;
	 case -600:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-10:00)");
		break;
	 case -540:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-09:00)");
		break;
	 case -480:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-08:00)");
		break;
	 case -420:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-07:00)");
		break;
	 case -360:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-06:00)");
		break;
	 case -300:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-05:00)");
		break;
	 case -270:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-04:30)");
		break;
	 case -240:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-04:00)");
		break;
	 case -210:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-03:30)");
		break;
	 case -180:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-03:00)");
		break;
	 case -120:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-02:00)");
		break;
	 case -60:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT-01:00)");
		break;
	 case 0:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT)");
		break;
	 case 60:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+01:00)");
		break;
	 case 120:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+02:00)");
		break;
	 case 180:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+03:00)");
		break;
	 case 210:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+03:30)");
		break;
	 case 240:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+04:00)");
		break;
	 case 270:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+04:30)");
		break;
	 case 300:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+05:00)");
		break;
	 case 330:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+05:30)");
		break;
	 case 345:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+05:45)");
		break;
	 case 360:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+06:00)");
		break;
	 case 390:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+06:30)");
		break;
	 case 420:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+07:00)");
		break;
	 case 480:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+08:00)");
		break;
	 case 540:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+09:00)");
		break;
	 case 570:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+09:30)");
		break;
	 case 600:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+10:00)");
		break;
	 case 660:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+11:00)");
		break;
	 case 720:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+12:00)");
		break;
	 case 780:
	 	snprintf(pTimeZone,sizeof(pTimeZone),"%s","(GMT+13:00)");
		break;
	}

    return websWrite(wp, T("\"%s\""),pTimeZone);
}

static int getCurrentTime (int eid, webs_t wp, int argc, char_t **argv)
{
	char tzarray[13][3] = {"00", "01", "02", "03", "04", "05", "06", \
"07", "08", "09", "10", "11", "12"};
    char tzmin[][3] = {"00", "30"};
    long dtime = 0;
    short diffhour = 0;
    char diffmin[3] = {"00"};
    char *strtm, *str;
    time_t tm1, tm2;
    struct tm *t1, *t2;
	char *date = NULL;
	char *month = NULL;
	char *day = NULL;
	char *year = NULL;
	char *tm = NULL;
    strtm = malloc(120 * sizeof(char));
	str = malloc(128 * sizeof(char));
    tm1 = time(NULL);
    t2 = gmtime(&tm1);
    tm2 = mktime(t2);
    t1 = localtime(&tm1);
#ifdef FINETUNEHOMEPAGE
    char * pQryResult = NULL;
    pQryResult = &pCurrentTime[0];
#else
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs("ntpConfig", "timezone",1);
#else
    char * pQryResult = jnrp_sql_getAttr(pDBFile,"ntpConfig", "timezone",1);
#endif
#endif
    int timezone = gatoi(pQryResult);
    dtime = timezone*60;
/* Print local time as a string */
    strftime(strtm, 100, "%a %b %d %Y %H:%M:%S GMT", t1);
	date = strtok(strtm, " ");
	month = strtok(NULL, " ");
	day = strtok(NULL, " ");
	year = strtok(NULL, " ");
	tm = strtok(NULL, " ");
	gstrcpy(str,"");
	if (gstrcmp(month,"Jan")==0)
		gstrcat(str,"01");
	else if (gstrcmp(month,"Feb")==0)
		gstrcat(str,"02");
	else if (gstrcmp(month,"Mar")==0)
		gstrcat(str,"03");
	else if (gstrcmp(month,"Apr")==0)
		gstrcat(str,"04");
	else if (gstrcmp(month,"May")==0)
		gstrcat(str,"05");
	else if (gstrcmp(month,"Jun")==0)
		gstrcat(str,"06");
	else if (gstrcmp(month,"Jul")==0)
		gstrcat(str,"07");
	else if (gstrcmp(month,"Aug")==0)
		gstrcat(str,"08");
	else if (gstrcmp(month,"Sep")==0)
		gstrcat(str,"09");
	else if (gstrcmp(month,"Oct")==0)
		gstrcat(str,"10");
	else if (gstrcmp(month,"Nov")==0)
		gstrcat(str,"11");
	else if (gstrcmp(month,"Dec")==0)
		gstrcat(str,"12");
	gstrcat(str,"/");
	gstrcat(str,day);
	gstrcat(str," ");
	gstrcat(str,tm);
	free(strtm);
	websWrite(wp, T("%s"),str);
	free(str);
}

#define PROC_IF_STATISTIC	"/proc/net/dev"
#define PROC_MEM_STATISTIC	"/proc/meminfo"

#define TXBYTE		0
#define TXPACKET	1
#define RXBYTE		2
#define RXPACKET	3


static inline char *strip_space(char *str)
{
	while( *str == ' ')
		str++;
	return str;
}

char* getField(char *a_line, char *delim, int count)
{
	int i=0;
	char *tok;
	tok = strtok(a_line, delim);
	while(tok){
		if(i == count)
			break;
        i++;
		tok = strtok(NULL, delim);
    }
    if(tok && isdigit(*tok))
		return tok;

	return NULL;
}

/*
 * description: return WAN interface name
 */
char* getWanIfName(void)
{
	return"usb0";
}

/*
 * description: return LAN interface name
 */
char* getLanIfName(void)
{
	return "br0";
}

/*
 *   C version. (ASP version is below)
 */
static long long getIfStatistic(char *interface, int type)
{
	int found_flag = 0;
	int skip_line = 2;
	char buf[1024], *field, *semiColon = NULL;
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		printf("no proc?\n");
		return -1;
	}

	while(fgets(buf, 1024, fp)){
		char *ifname;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	switch(type){
	case TXBYTE:
		if(  (field = getField(semiColon, " ", 8))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case TXPACKET:
		if(  (field = getField(semiColon, " ", 9))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXBYTE:
		if(  (field = getField(semiColon, " ", 0))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	case RXPACKET:
		if(  (field = getField(semiColon, " ", 1))  ){
			return strtoll(field, NULL, 10);
		}
		break;
	}
	return -1;
}

/*
 *     getIfStatistic()   ASP version
 */
int getIfStatisticASP(int eid, webs_t wp, int argc, char_t **argv)
{
	int found_flag = 0;
	int skip_line = 2;
	char *interface, *type, *field, *semiColon = NULL;
	char buf[1024], result[32];
	FILE *fp = fopen(PROC_IF_STATISTIC, "r");
	if(!fp){
		websWrite(wp, T("no proc?\n"));
		return -1;
	}

    if(ejArgs(argc, argv, T("%s %s"), &interface, &type) != 2){
		websWrite(wp, T("Wrong argument.\n"));
        return -1;
    }

	while(fgets(buf, 1024, fp)){
		char *ifname;
		if(skip_line != 0){
			skip_line--;
			continue;
		}
		if(! (semiColon = strchr(buf, ':'))  )
			continue;
		*semiColon = '\0';
		ifname = buf;
		ifname = strip_space(ifname);

		if(!strcmp(ifname, interface)){
			found_flag = 1;
			break;
		}
	}
	fclose(fp);

	semiColon++;

	if(!strcmp(type, T("TXBYTE")  )){
		if(  (field = getField(semiColon, " ", 8))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
	}else if(!strcmp(type, T("TXPACKET")  )){
		if(  (field = getField(semiColon, " ", 9))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXBYTE")  )){
		if(  (field = getField(semiColon, " ", 0))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else if(!strcmp(type, T("RXPACKET")  )){
		if(  (field = getField(semiColon, " ", 1))  ){
			snprintf(result, 32,"%lld",   strtoll(field, NULL, 10));
			ejSetResult(eid, result);
		}
    }else{
		websWrite(wp, T("unknown type.") );
		return -1;
	}
	return -1;
}

int getWANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data/1024);
	websWrite(wp, T("\"%s KB\""), buf);
	return 0;
}

int getWANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getWANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data/1024);
	websWrite(wp, T("\"%s KB\""), buf);
	return 0;
}

int getWANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getWanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), RXBYTE);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANRxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), RXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxByteASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), TXBYTE);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"), buf);
	return 0;
}

int getLANTxPacketASP(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t buf[32];
	long long data = getIfStatistic( getLanIfName(), TXPACKET);
	snprintf(buf, 32, "%lld", data);
	websWrite(wp, T("%s"),buf);
	return 0;
}

static int getConnectionTime (int eid, webs_t wp, int argc, char_t **argv)
{
 char connectionTime[128];
 memset(connectionTime, '\0', sizeof(connectionTime));

 //system("lte_get --ip-status -q 0 --timeout 5000 | grep 'CLIENT:connection time:' | cut -d: -f 3-6 > /tmp/lteConnectionTime");

 FILE *fp = fopen("/tmp/lteConnectionTime", "r");
 if(fp)
 {
  while(fgets(connectionTime, sizeof(connectionTime), fp) != NULL)
  {
   if (connectionTime[strlen(connectionTime)-1] == '\n')
     connectionTime[strlen(connectionTime)-1] = '\0';
  }
  fclose(fp);
 } else {
 	return websWrite(wp, T("%s"), "0:0:0:0");
 }

 //remove("/tmp/lteConnectionTime");

 return websWrite(wp, T("%s"), connectionTime);
}

static int getNetwork(int eid,webs_t wp,int argc,char_t * * argv)
{
#ifdef __P02002_Dora

#else
	websWrite(wp, T("<td width=\"20\"><img id=\"wan_network_type_3g\" src=\"\" /></td>") );
	websWrite(wp, T("<td width=\"26\">3G</td>") );

#endif
	return 0;
	
}

static int getStatusBar (int eid, webs_t wp, int argc, char_t **argv)
{
   char** ppQueryRes = NULL;
   int nRows=0,nCols=0,status=0;
   char* pQueryStr = NULL;

   int ret = 0;
   char* pSignal = (char *)malloc(128);
   char* pType = (char *)malloc(128);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
   char* pRegist = (char *)malloc(128);
#endif

   char *pStatusBar = (char *)malloc(128);

   pQueryStr = "select * from lteNwStatus where qid='1'";
   gstrcpy(pStatusBar,"");

#if defined(__P02001_Dora) || defined(__P02002_Dora)
   //system ("lte_get --ip-status -q 0 --update-db > /dev/NULL");
#endif
#ifndef __P02002_Dora
   system ("lte_get -n -q 0 --timeout 5000 --update-db > /dev/NULL");
//   system ("lte_get --signal-strength -q 0 --timeout 5000 --update-db > /dev/NULL");
   system ("sleep 1");  
#endif   
#ifdef __USED_WEBS_DB__
   if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
#else
   sqlite3 * pWebDB = NULL;
   if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL){
#endif
     printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
	 sqlite3_free (pQueryStr);
	 free(pSignal);
	 free(pType);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
	 free(pRegist);
#endif
	 free(pStatusBar);
	 return -1;
   }
#ifdef __USED_WEBS_DB__
//   char * pSignalTemp = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","signalStrength","qid=1");
   char * pTypeTemp = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","radioInterference","qid=1");
   char * pRegistStatus = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","registrationState","qid=1");
#if defined(__P02001_Dora) || defined(__P02002_Dora)
   char * pNetworkName = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","networkName","qid=1");
   char * pIP = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","ip","qid=1");
   char * pDNS = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","primary_dns","qid=1");
   char * pGateway = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","gateway","qid=1");
   char * pSubMask = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","netmask","qid=1");
   char * pStatus = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
#endif
#else
//   char * pSignalTemp = jnrp_sql_getAttrByCondit("lteNwStatus","signalStrength","qid=1");
   char * pTypeTemp = jnrp_sql_getAttrByCondit("lteNwStatus","radioInterference","qid=1");
   char * pRegistStatus = jnrp_sql_getAttrByCondit(pDBFile,"lteNwStatus","registrationState","qid=1");
#if defined(__P02001_Dora) || defined(__P02002_Dora)
   char * pNetworkName = jnrp_sql_getAttrByCondit("lteNwStatus","lteNwStatus","networkName","qid=1");
   char * pIP = jnrp_sql_getAttrByCondit("lteIPStatus","ip","qid=1");
   char * pDNS = jnrp_sql_getAttrByCondit("lteIPStatus","primary_dns","qid=1");
   char * pGateway = jnrp_sql_getAttrByCondit("lteIPStatus","gateway","qid=1");   
   char * pSubMask = jnrp_sql_getAttrByCondit("lteIPStatus","netmask","qid=1");
   char * pStatus = jnrp_sql_getAttrByCondit("lteIPStatus","status","qid=1");
#endif   
#endif   
//#ifndef __P02002_Dora
//   int pSignalVal = atoi(pSignalTemp); 
 
   //Signal
//   int pos = (int)((double)(pSignalVal + 106)*100/(double)(-80 + 106));
//   if(pos < 0) pos = 0;
//   else if(pos > 100) pos = 100;
//   int sPos = pos%20 > 15 ? pos/20+1:pos/20; /*0 no signal, 100 full*/
//#else
   int sPos = 0;
   FILE *fp = fopen("/tmp/signal_level", "r");
   char line[8];
   if(fp){
    if(fgets(line, sizeof(line), fp)!=NULL)
       sPos = atoi(line);
	fclose(fp);
   }
//#endif
   if(!strcmp(pRegistStatus, "Registered")){	
#if defined(__P02001_Dora) || defined(__P02002_Dora)
   	 //printf("Enter getStatusBar::pRegistStatus=Registered");  
     strcpy(pRegist, "1");
#endif
     if (sPos == 0){
       strcpy(pSignal, "0");
     }else if (sPos == 1){
       strcpy(pSignal, "1");
     }else if (sPos == 2){
       strcpy(pSignal, "2");
     }else if (sPos == 3){
       strcpy(pSignal, "3");
     }else if (sPos == 4){
       strcpy(pSignal, "4");
     }else if (sPos == 5){
       strcpy(pSignal, "5");
     }else{
       strcpy(pSignal, "6");
     }
   }else{
#if defined(__P02001_Dora) || defined(__P02002_Dora)
     //printf("Enter getStatusBar::pRegistStatus=Un-Registered");  
	 strcpy(pRegist, "0");
#endif
	 strcpy(pSignal, "6");
   }
   //Type
   if(!strcmp(pRegistStatus, "Registered")){
   	if(!strcmp(pTypeTemp, "UMTS")){  //UMTS: 3G ,LTE: 4G
    	strcpy(pType,"0");
   	}else if(!strcmp(pTypeTemp, "LTE")){
   		strcpy(pType,"1");
   	}else{
     	strcpy(pType,"0");
   	}
   }else{
   		strcpy(pType,"2");           
   }
   if (ppQueryRes)
   {
	sqlite3_free_table (ppQueryRes);
	gstrcat(pStatusBar,pSignal);
	gstrcat(pStatusBar,pSep);
	gstrcat(pStatusBar,pType);
	gstrcat(pStatusBar,pSep);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
	gstrcat(pStatusBar,pRegist);  //2
	gstrcat(pStatusBar,pSep);
	gstrcat(pStatusBar,pNetworkName);  //3
	gstrcat(pStatusBar,pSep);
	gstrcat(pStatusBar,pIP);  //4
	gstrcat(pStatusBar,pSep);
	gstrcat(pStatusBar,pDNS);  //5
	gstrcat(pStatusBar,pSep);
	gstrcat(pStatusBar,pGateway);  //6
	gstrcat(pStatusBar,pSep);
	gstrcat(pStatusBar,pSubMask);  //7
	gstrcat(pStatusBar,pSep);
	gstrcat(pStatusBar,pStatus);  //8
	gstrcat(pStatusBar,pSep);
#endif
   }
   //printf("Enter getStatusBar::pStatusBarInfo=%s\n", pStatusBar);  
getConfigOut:

	ret = websWrite(wp, T("%s"), pStatusBar);
	free(pSignal);
	free(pType);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
	free(pRegist);
#endif
	free(pStatusBar);
	return ret;
}

#if defined(__P02003_Dora)
static char * getSignalLevel ()
{  
 FILE *fp = fopen("/tmp/signal_level", "r");
 int sPos = 0;
 char pSignalLevel[32];
 char line[8];
 
 memset(pSignalLevel,'\0',sizeof(pSignalLevel));
 if(fp){
  if(fgets(line, sizeof(line), fp)!=NULL)
    sPos = atoi(line);
  fclose(fp);
 }
 if(sPos < 0 || sPos > 5) sPos = 6;

 snprintf(pSignalLevel, sizeof(pSignalLevel), "%d", sPos);
 return pSignalLevel;
}

static int getHomeInternetInfo1(int eid,webs_t wp,int argc,char_t * * argv)
{
 char pInternetInfo[128];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from lteNwStatus";

 char* pRegistrationState = NULL;
 char* pRadioInterference = NULL;
 char* pSignal = NULL;
 char* pNetworkName = NULL;
 char* pRoaming = NULL;

 int index = 0;
 system ("lte_get -n -q 0 --timeout 5000 --update-db > /dev/NULL");
 system ("lte_get --signal-strength -q 0 --timeout 5000 --update-db > /dev/NULL");
	sleep(1);

 memset(pInternetInfo,'\0',sizeof(pInternetInfo));

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pInternetInfo);
 }

 if (nRows > 0) {
  nRows = 1;

  for (index = 0; index < nRows; index++) {
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "registrationState", pRegistrationState, getHomeInternetInfoOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "radioInterference", pRadioInterference, getHomeInternetInfoOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "signalStrength", pSignal, getHomeInternetInfoOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "networkName", pNetworkName, getHomeInternetInfoOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Roaming", pRoaming, getHomeInternetInfoOut);

   gstrcat(pInternetInfo,pRegistrationState);
   gstrcat(pInternetInfo,pSep);
   gstrcat(pInternetInfo,pRoaming);
   gstrcat(pInternetInfo,pSep);
   gstrcat(pInternetInfo,pNetworkName);
   gstrcat(pInternetInfo,pSep);
   gstrcat(pInternetInfo,getSignalLevel());
   gstrcat(pInternetInfo,pSep);
   gstrcat(pInternetInfo,pRadioInterference);
   gstrcat(pInternetInfo,pSep);
   gstrcat(pInternetInfo,pSignal);
   gstrcat(pInternetInfo,pSep);
  }
 }

getHomeInternetInfoOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return websWrite(wp, T("\"%s\""),pInternetInfo);
}

static int getHomeInternetInfo2(int eid,webs_t wp,int argc,char_t * * argv)
{
	char pInternetInfo[256];
	FILE *fileCA, *fileCAstate;
	char ca, ca_status;
	memset(pInternetInfo,'\0',sizeof(pInternetInfo));

 	system("lte_get --ip-status -q 0 --timeout 5000 > /tmp/getHomeInternetInfo2");

	char * pWanStatus = jnrp_sql_getAttr_Webs ("ethWanStatus", "eStatus", 1);

	system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep pcell_pci | sed 's/^.*state\": //g' | sed 's/ .*$//g' > /tmp/getCA");

	FILE *fp = fopen("/tmp/getHomeInternetInfo2", "r");
	char line[128];
	int len = 0;
 	memset(line, '\0', sizeof(line));
	if(fp){
		while(fgets(line, sizeof(line), fp) != NULL){
			if (line[strlen(line)-1] == '\n')
	 			line[strlen(line)-1] = '\0';

			if (gstrstr(line,"CLIENT:status:")
		 		|| gstrstr(line,"CLIENT:ip address:") || gstrstr(line,"CLIENT:gateway:")
		 		|| gstrstr(line,"CLIENT:netmask:") || gstrstr(line,"CLIENT:primary DNS:"))
			{
	 			gstrcat(pInternetInfo,getSplitStr(line, ":", 3));
	 			gstrcat(pInternetInfo,pSep);
			} else if(gstrstr(line,"CLIENT:IPv6 status:"))	{
				len = strlen("CLIENT:IPv6 status:");
				gstrcat(pInternetInfo,(line+len));
				gstrcat(pInternetInfo,pSep);
			} else if(gstrstr(line,"CLIENT:IPv6 address:")) {
				len = strlen("CLIENT:IPv6 address:");
				gstrcat(pInternetInfo,(line+len));
				gstrcat(pInternetInfo,pSep);
			} else if(gstrstr(line,"CLIENT:IPv6 primary DNS:")) {
				len = strlen("CLIENT:IPv6 primary DNS:");
				gstrcat(pInternetInfo,(line+len));
				gstrcat(pInternetInfo,pSep);
			}
		}
		fclose(fp);
	} else {
 		return websWrite(wp, T("\"%s\""), "#####");
 	}
	gstrcat(pInternetInfo,pWanStatus);
	gstrcat(pInternetInfo,pSep);

	//Get CA Info
	fileCA = fopen("/tmp/getCA", "r");
	if (fileCA){
		ca = fgetc(fileCA);
		if ((ca == '0')||(ca == '1')||(ca == '2')){
			fclose(fileCA);
			system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep errno | sed 's/^.*state\": //g' | sed 's/,.*$//g' > /tmp/getCAstate");
			fileCAstate = fopen("/tmp/getCAstate", "r");
			if (fileCAstate){
				ca_status = fgetc(fileCAstate);
				if (ca_status == '0'){
					gstrcat(pInternetInfo,"0");
	 				gstrcat(pInternetInfo,pSep);
				}else if (ca_status == '1'){
					gstrcat(pInternetInfo,"1");
	 				gstrcat(pInternetInfo,pSep);
				}else if (ca_status == '2'){
					gstrcat(pInternetInfo,"2");
	 				gstrcat(pInternetInfo,pSep);
				}else{
				    gstrcat(pInternetInfo,"0");
	 				gstrcat(pInternetInfo,pSep);
				}
				fclose(fileCAstate);
			}else{
				gstrcat(pInternetInfo,"0");
	 			gstrcat(pInternetInfo,pSep);
			}
		} else {
			fclose(fileCA);
			system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep errno | sed 's/^.*state\": //g' | sed 's/ .*$//g' > /tmp/getCAstate");
			fileCAstate = fopen("/tmp/getCAstate", "r");
			if (fileCAstate){
				ca_status = fgetc(fileCAstate);
				if (ca_status == '0'){
					gstrcat(pInternetInfo,"0");
	 				gstrcat(pInternetInfo,pSep);
				}else if (ca_status == '1'){
					gstrcat(pInternetInfo,"1");
	 				gstrcat(pInternetInfo,pSep);
				}else if (ca_status == '2'){
					gstrcat(pInternetInfo,"2");
	 				gstrcat(pInternetInfo,pSep);
				}else{
				    gstrcat(pInternetInfo,"0");
	 				gstrcat(pInternetInfo,pSep);
				}
				fclose(fileCAstate);
			}else{
				gstrcat(pInternetInfo,"0");
	 			gstrcat(pInternetInfo,pSep);
			}
		}
	} else{
		gstrcat(pInternetInfo,"0");
	 	gstrcat(pInternetInfo,pSep);
	}
 	return websWrite(wp, T("\"%s\""), pInternetInfo);
}

static int getHomeInternetInfo3(int eid,webs_t wp,int argc,char_t * * argv)
{
	char pInternetInfo[128];
	memset(pInternetInfo,'\0',sizeof(pInternetInfo));

 FILE *fp = fopen("/tmp/getHomeInternetInfo2", "r");
 char line[128];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';

   if (gstrstr(line,"CLIENT:connection time:")) {
     gstrcat(pInternetInfo,getSplitStr(line, ":", 3));
     gstrcat(pInternetInfo,":");
     gstrcat(pInternetInfo,getSplitStr(line, ":", 4));
     gstrcat(pInternetInfo,":");
     gstrcat(pInternetInfo,getSplitStr(line, ":", 5));
     gstrcat(pInternetInfo,":");
     gstrcat(pInternetInfo,getSplitStr(line, ":", 6));
   }
  }
  fclose(fp);
 } else {
 	return websWrite(wp, T("%s"), "0:0:0:0");
 }
 //remove("/tmp/getHomeInternetInfo2");
 return websWrite(wp, T("%s"), pInternetInfo);
}

static int getHomeWiFiRadio(int eid, webs_t wp, int argc, char_t **argv)
{
 char * pChannel_11n = jnrp_sql_getAttrByCondit_Webs ("wifiRadio", "configuredChannel", "radioNo=1");
 char * pChannel_11ac = jnrp_sql_getAttrByCondit_Webs ("wifiRadio", "configuredChannel", "radioNo=2");

 char pWiFiInfo [256] = {0};
 memset (pWiFiInfo, 0x00, sizeof(pWiFiInfo));

 gstrcat(pWiFiInfo, pChannel_11n);
 gstrcat(pWiFiInfo, pSep);
 gstrcat(pWiFiInfo, pChannel_11ac);
 gstrcat(pWiFiInfo, pSep);

 return websWrite(wp, T("\"%s\""), pWiFiInfo);
}

static int getHomeTotalWiFiClient(int eid,webs_t wp,int argc,char_t * * argv)
{
#if 1 //New
 int count = 0;
 system("querydev WIFICOUNT > /dev/null 2>&1");

 char buffer[128] = {0};
 FILE *fd = -1;

 memset(buffer, '\0', sizeof(buffer));

 fd = popen("tail -n 1 /tmp/countwifidev","r");
 if(fgets(buffer,sizeof(buffer),fd) == NULL)
 {
   websWrite(wp, T("\"%s\""), "0");
 }
 else
 {
   if (buffer[strlen(buffer)-1] == '\n')
     buffer[strlen(buffer)-1] = '\0';
   websWrite(wp, T("\"%s\""),buffer);
 }
 pclose(fd);
 return 0;
#else
 int count = 0;
 char * pQryCount = "select count(*) from DhcpLeasedClients where clientIf='wlan0' or clientIf='wlan0.0'";
 char ** ppQryCountResult = jnrp_sql_getByQry_Webs (pQryCount);
 if (ppQryCountResult)
 {
   count = gatoi(ppQryCountResult[1]);
 }

 return websWrite(wp, T("%d"), count);
#endif
}

static int getHomeWiFiStatus(int eid,webs_t wp,int argc,char_t * * argv)
{
	//printf("Enter getHomeWiFiStatus----------\n");
	char * pStatus1 = jnrp_sql_getAttr_Webs("wifiap", "wlanEnabled", 1);
 	char * pStatus2 = jnrp_sql_getAttr_Webs("wifiap", "wlanEnabled", 2);
	char * pStatus3 = jnrp_sql_getAttr_Webs("wifiap", "wlanEnabled", 3);
	char * pStatus4 = jnrp_sql_getAttr_Webs("wifiap", "wlanEnabled", 4);
	

	//printf("pStatus1:%s, pStatus2:%s, pStatus3:%s, pStatus4:%s\n", pStatus1, pStatus2, pStatus3, pStatus4);

 	char pWiFiStatus [256] = {0};
 	memset (pWiFiStatus, 0x00, sizeof(pWiFiStatus));

 	gstrcat(pWiFiStatus, pStatus1);
 	gstrcat(pWiFiStatus, pSep);
 	gstrcat(pWiFiStatus, pStatus2);
 	gstrcat(pWiFiStatus, pSep);
	gstrcat(pWiFiStatus, pStatus3);
 	gstrcat(pWiFiStatus, pSep);
	gstrcat(pWiFiStatus, pStatus4);
 	gstrcat(pWiFiStatus, pSep);

 	return websWrite(wp, T("\"%s\""), pWiFiStatus);
}

static int aspSetResetDefaultFinish(int eid,webs_t wp,int argc,char_t * * argv)
{
	char * pConfirmSSID = jnrp_sql_getAttrByCondit_Webs ("wifiProfile", "ssid", "profileName='wlan1'");
	char * pConfirmPSK = jnrp_sql_getAttrByCondit_Webs ("wifiProfile", "pskPassAscii", "profileName='wlan1'");
	char * pPWD = jnrp_sql_getAttrByCondit_Webs ("users", "password", "username='admin'");
	char pConfirmSSID_ac [128] = {0};
	memset (pConfirmSSID_ac, 0x00, sizeof(pConfirmSSID_ac));
	snprintf(pConfirmSSID_ac, sizeof(pConfirmSSID_ac), "%s_5GHz", pConfirmSSID);
	char pConfirmAdminPWD [128] = {0};
	memset (pConfirmAdminPWD, 0x00, sizeof(pConfirmAdminPWD));

	if (!gstrcmp(pPWD,"")){
		//gstrcpy(pConfirmAdminPWD,pConfirmPSK);
		gstrcpy(pConfirmAdminPWD,"admin");
	}	

 	//printf("%s, pConfirmSSID=%s, pConfirmPSK=%s, pConfirmAdminPWD=%s\n",__func__,pConfirmSSID,pConfirmPSK,pConfirmAdminPWD);
	char * pErrMsg = NULL;
	int status = -1;
	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	 if (gstrlen(pConfirmSSID) && gstrlen(pConfirmPSK) && gstrlen(pConfirmAdminPWD)) {
	 } else {
	   setResetDefaultReady = 0;
	 }

	setResetDefaultReady = 1;

	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK) {
		printf ("unable to open the DB file %s\n", pDBFile);
		setResetDefaultReady = 0;
	}
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update wifiProfile set ssid='%s', pskPassAscii='%s' where profileName='wlan1'", pConfirmSSID, pConfirmPSK);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update wifiProfile set ssid='%s', pskPassAscii='%s' where profileName='wlan3'", pConfirmSSID_ac, pConfirmPSK);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update wifiProfilecommit set profilecommit ='both'");
	//status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg); //remove to avoid wifi disable after reset to default.

	// not set WEB UI password.
	//snprintf(pSQLCmd,sizeof(pSQLCmd), "update users set password='%s' where username='admin'", pConfirmAdminPWD);
	//status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
 
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update ResetToDefault set resetToDefault=%d", 0);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	if (pDB)
		sqlite3_close (pDB);

	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD);
	}
	//not necessary to set 'admin' group password
	//umSetUserPassword("admin", pConfirmAdminPWD);
 	//umRestore("");

	char * pStaticIp = jnrp_sql_getAttrByCondit_Webs ("Lan", "IpAddress", "LogicalIfName='LAN1'");
	snprintf(pSQLCmd,sizeof(pSQLCmd), "iptables -t nat -D PREROUTING -i br0 -p tcp --dport 80 -j DNAT --to-destination %s", pStaticIp);
	system(pSQLCmd);
	system("killall -9 dnsmasq");
	system("/usr/sbin/dnsmasq -u root --resolv-file=/ramdisk/etc/resolv.conf");

	system("lte_set -q 0 -c -1 > /tmp/landingPageInfo");
	FILE *fp = fopen("/tmp/landingPageInfo", "r");
	char line[128];
	memset(line, '\0', sizeof(line));

	if(fp) {
		while(fgets(line, sizeof(line), fp) != NULL){
			if (line[strlen(line)-1] == '\n')
				line[strlen(line)-1] = '\0';
			 
			if (gstrstr(line,"Error:")) {
				system("echo 1, 1, 0 > /mnt/data/lte_connection_settings");
				system("lte_service.sh restart > /dev/null 2>&1");
			}
		}
		fclose(fp);
	}

	return websWrite(wp, T("%d"), setResetDefaultReady);
}

static int setSetupWizardReady(int eid,webs_t wp,int argc,char_t * * argv)
{
	//printf("setSetupWizardReady\n");
	setupWizardReadyDisplay = 0;
	return websWrite(wp, T("%d"), setupWizardReadyDisplay);
}

static int getSetupWizardFinish(int eid,webs_t wp,int argc,char_t * * argv)
{
 return websWrite(wp, T("%d"), setupWizardReadyDisplay);
}

static void setSetupWizardFinish(webs_t wp, char_t *path, char_t *query)
{
	char * pConfirmSSID = websGetVar(wp, T("confirm_ssid"), T(""));
	char * pConfirmPSK = websGetVar(wp, T("confirm_psk"), T(""));
	char * pConfirmAdminPWD = websGetVar(wp, T("confirm_pwd"), T(""));
	char pConfirmSSID_ac [128] = {0};
	memset (pConfirmSSID_ac, 0x00, sizeof(pConfirmSSID_ac));
	snprintf(pConfirmSSID_ac, sizeof(pConfirmSSID_ac), "%s_5GHz", pConfirmSSID);

 	//printf("%s, pConfirmSSID=%s, pConfirmPSK=%s, pConfirmAdminPWD=%s\n",__func__,pConfirmSSID,pConfirmPSK,pConfirmAdminPWD);
	char * pErrMsg = NULL;
	int status = -1;
	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	 if (gstrlen(pConfirmSSID) && gstrlen(pConfirmPSK) && gstrlen(pConfirmAdminPWD)) {
	 } else {
	   setupWizardReadyDisplay = 0;
	   websRedirect(wp, T("setupWizard.asp"));
	   return;
	 }

	setupWizardReadyDisplay = 1;

	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK) {
		printf ("unable to open the DB file %s\n", pDBFile);
		websRedirect(wp, T("setupWizard.asp"));
	}
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update wifiProfile set ssid='%s', pskPassAscii='%s' where profileName='wlan1'", pConfirmSSID, pConfirmPSK);
	//printf("pSQLCmd: %s \n",pSQLCmd);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update wifiProfile set ssid='%s', pskPassAscii='%s' where profileName='wlan3'", pConfirmSSID_ac, pConfirmPSK);
	//printf("pSQLCmd: %s \n",pSQLCmd);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update wifiProfilecommit set profilecommit ='both'");
	//printf("pSQLCmd: %s \n",pSQLCmd);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	snprintf(pSQLCmd,sizeof(pSQLCmd), "update users set password='%s' where username='admin'", pConfirmAdminPWD);
	//printf("pSQLCmd: %s \n",pSQLCmd);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
 
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update ResetToDefault set resetToDefault=%d", 0);
	//printf("pSQLCmd: %s \n",pSQLCmd);
	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	if (pDB)
		sqlite3_close (pDB);

	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		//jnrp_sql_setAttr_Webs("ResetToDefault", "resetToDefault", "0", 1);
		system(EXPORT_CMD);
	}

	//sleep(1);
	umSetUserPassword("admin", pConfirmAdminPWD);
 	umRestore("");

	char * pStaticIp = jnrp_sql_getAttrByCondit_Webs ("Lan", "IpAddress", "LogicalIfName='LAN1'");
	snprintf(pSQLCmd,sizeof(pSQLCmd), "iptables -t nat -D PREROUTING -i br0 -p tcp --dport 80 -j DNAT --to-destination %s", pStaticIp);
	//printf("pSQLCmd: %s \n",pSQLCmd);
	system(pSQLCmd);
	//system("/usr/local/bin/firewall.sh > /dev/null");
	//killall dnsmasq, run dnsmasq
	system("killall -9 dnsmasq");
	system("/usr/sbin/dnsmasq -u root --resolv-file=/ramdisk/etc/resolv.conf");

	system("lte_set -q 0 -c -1 > /tmp/landingPageInfo");
	FILE *fp = fopen("/tmp/landingPageInfo", "r");
	char line[128];
	memset(line, '\0', sizeof(line));

	if(fp) {
		while(fgets(line, sizeof(line), fp) != NULL){
			if (line[strlen(line)-1] == '\n')
				line[strlen(line)-1] = '\0';
			 
			if (gstrstr(line,"Error:")) {
				system("echo 1, 1, 0 > /mnt/data/lte_connection_settings");
				system("lte_service.sh restart > /dev/null 2>&1");
			}
		}
		fclose(fp);
	}

	websRedirect(wp, T("setupWizard.asp"));
	return;
}

static int getSetupWizardWiFiInfo(int eid,webs_t wp,int argc,char_t * * argv)
{
 char * pSSID = jnrp_sql_getAttrByCondit_Webs ("wifiProfile", "ssid", "profileName='wlan1'");
 char * pPskPassAscii = jnrp_sql_getAttrByCondit_Webs ("wifiProfile", "pskPassAscii", "profileName='wlan1'");

 char pWiFiInfo [256] = {0};
 memset (pWiFiInfo, 0x00, sizeof(pWiFiInfo));

 gstrcat(pWiFiInfo, pSSID);
 gstrcat(pWiFiInfo, pSep);
 gstrcat(pWiFiInfo, pPskPassAscii);
 gstrcat(pWiFiInfo, pSep);

 return websWrite(wp, T("\"%s\""), pWiFiInfo);
}

static int getSetupWizardAdminInfo(int eid,webs_t wp,int argc,char_t * * argv)
{
 char * pPassword = jnrp_sql_getAttrByCondit_Webs ("users", "password", "username='admin'");

 char pAdminInfo [128] = {0};
 memset (pAdminInfo, 0x00, sizeof(pAdminInfo));

 gstrcat(pAdminInfo, pPassword);
 gstrcat(pAdminInfo, pSep);

 return websWrite(wp, T("\"%s\""), pAdminInfo);
}

static int getSetupWizardCurPINCode(int eid,webs_t wp,int argc,char_t * * argv)
{
 char pCurPinCode[32];
 FILE *fp;
 char line[32];
 char cmdbuffer[100] = {0};
 memset(line, '\0', sizeof(line));

 memset(pCurPinCode, '\0', sizeof(pCurPinCode));
 if(access("/sysconfig/malmanager.cfg", F_OK) >=0 ){
   //for malmanger
   sprintf(cmdbuffer,"grep WWAN_PIN /sysconfig/malmanager.cfg | cut -d'\"' -f2");
   fp = popen(cmdbuffer,"r");
   if(fgets(line,sizeof(line),fp) == NULL){
     return websWrite(wp, T("\"%s\""), "#");
   }else{
	   if (line[strlen(line)-1] == '\n')
		 line[strlen(line)-1] = '\0';
	   gstrcat(pCurPinCode,line);
	   gstrcat(pCurPinCode,pSep);
   }
   pclose(fp);
 }else{
   //for lte daemon
	 fp = fopen("/mnt/data/lte_pin_code", "r");
	 if(fp)
	 {
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {
	   if (line[strlen(line)-1] == '\n')
		 line[strlen(line)-1] = '\0';
	   gstrcat(pCurPinCode,line);
	   gstrcat(pCurPinCode,pSep);
	  }
	  fclose(fp);
	 } else {
		return websWrite(wp, T("\"%s\""), "#");
	 }
 }
 return websWrite(wp, T("\"%s\""), pCurPinCode);
}

static void setSetupWizardVerifyPIN(webs_t wp, char_t *path, char_t *query)
{
	char * pPinCode = websGetVar(wp, T("lte_pin"), T(""));
 char pPinInfo [128] = {0};
 memset (pPinInfo, 0x00, sizeof(pPinInfo));

 snprintf(pPinInfo,sizeof(pPinInfo), "lte_set -n verify 1 %s > /dev/NULL", pPinCode);
 system(pPinInfo);
 sleep(1);
 websRedirect(wp, T("setupWizard.asp"));
}

static void setSetupWizardUnblockPUK(webs_t wp, char_t *path, char_t *query)
{
	char * pPinCode = websGetVar(wp, T("lte_pin"), T(""));
	char * pPukCode = websGetVar(wp, T("lte_puk"), T(""));
 char pPinInfo [128] = {0};
 memset (pPinInfo, 0x00, sizeof(pPinInfo));

 snprintf(pPinInfo,sizeof(pPinInfo), "lte_set -n unblock 1 %s %s > /dev/NULL", pPukCode, pPinCode);
 system(pPinInfo);
 sleep(1);
 websRedirect(wp, T("setupWizard.asp"));
}

static void setSetupWizardChangePin(webs_t wp, char_t *path, char_t *query)
{
	char * pOldPin = websGetVar(wp, T("lte_OldPin"), T(""));
	char * pNewPin = websGetVar(wp, T("lte_NewPin"), T(""));
 char pPinInfo [128] = {0};
 memset (pPinInfo, 0x00, sizeof(pPinInfo));

 snprintf(pPinInfo,sizeof(pPinInfo), "lte_set -n change 1 %s %s > /dev/NULL", pOldPin, pNewPin);
 system(pPinInfo);
 sleep(1);
 websRedirect(wp, T("setupWizard.asp"));
}
#endif

void initHomeWeb()
{
	websAspDefine(T("getSysTime"), getSysTime);
	websAspDefine(T("getSysInfo"), getSysInfo);
	websAspDefine(T("getWanInfo"), getWanInfo);
	websAspDefine(T("getNetworkInfo"), getNetworkInfo);
	websAspDefine(T("getWiFiInfo"), getWiFiInfo);
	websAspDefine(T("getModuleVer"), getModuleVer);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
	websAspDefine(T("getWifiRadio"), getWifiRadio);
	websAspDefine(T("getWiFiClient"), getWiFiClient);
	websAspDefine(T("getHomeAPNStatus"), getHomeAPNStatus);
#endif
	websAspDefine(T("getLanInfo"), getLanInfo);
	websAspDefine(T("getVoipInfo"), getVoipInfo);
	websAspDefine(T("getNtpTimezone"), getNtpTimezone);
	websAspDefine(T("getCurrentTime"), getCurrentTime);

	websAspDefine(T("getWANRxByteASP"), getWANRxByteASP);
	websAspDefine(T("getWANTxByteASP"), getWANTxByteASP);
	websAspDefine(T("getLANRxByteASP"), getLANRxByteASP);
	websAspDefine(T("getLANTxByteASP"), getLANTxByteASP);
	websAspDefine(T("getWANRxPacketASP"), getWANRxPacketASP);
	websAspDefine(T("getWANTxPacketASP"), getWANTxPacketASP);
	websAspDefine(T("getLANRxPacketASP"), getLANRxPacketASP);
	websAspDefine(T("getLANTxPacketASP"), getLANTxPacketASP);

	websAspDefine(T("getConnectionTime"), getConnectionTime);
	websAspDefine(T("getStatusBar"), getStatusBar);
	websAspDefine(T("getNetwork"), getNetwork);
#if defined(__P02003_Dora)
	websAspDefine(T("getHomeInternetInfo1"), getHomeInternetInfo1);
 websAspDefine(T("getHomeInternetInfo2"), getHomeInternetInfo2);
 websAspDefine(T("getHomeInternetInfo3"), getHomeInternetInfo3);
 websAspDefine(T("getHomeWiFiRadio"), getHomeWiFiRadio);
 websAspDefine(T("getHomeTotalWiFiClient"), getHomeTotalWiFiClient);
 websAspDefine(T("getHomeWiFiStatus"),getHomeWiFiStatus);
 websAspDefine(T("aspSetResetDefaultFinish"), aspSetResetDefaultFinish);
 websAspDefine(T("setSetupWizardReady"), setSetupWizardReady);
 websAspDefine(T("getSetupWizardFinish"), getSetupWizardFinish);
 websFormDefine(T("setSetupWizardFinish"), setSetupWizardFinish);
 websAspDefine(T("getSetupWizardWiFiInfo"), getSetupWizardWiFiInfo);
 websAspDefine(T("getSetupWizardAdminInfo"), getSetupWizardAdminInfo);
 websAspDefine(T("getSetupWizardCurPINCode"), getSetupWizardCurPINCode); 
 websFormDefine(T("setSetupWizardVerifyPIN"), setSetupWizardVerifyPIN);
 websFormDefine(T("setSetupWizardUnblockPUK"), setSetupWizardUnblockPUK);
 websFormDefine(T("setSetupWizardChangePin"), setSetupWizardChangePin);
#endif

#if !defined(__P02001_Dora) && !defined(__P02002_Dora)
	websAspDefine(T("getHomePage"), getHomePage);
	websAspDefine(T("getWiFiAPInfo"), getWiFiAPInfo);
	websAspDefine(T("getFirewallInfo"), getFirewallInfo);
#endif
	return ;
}
