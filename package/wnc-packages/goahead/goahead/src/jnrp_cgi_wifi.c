#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

void initWifi();
static int getWifi(int eid, webs_t wp, int argc, char_t **argv);
static void setWifi(webs_t wp, char_t *path, char_t *query);

static int getWifiRadio(int eid, webs_t wp, int argc, char_t **argv);
static void setWifiRadio(webs_t wp, char_t *path, char_t *query);

static int getWPSInfo(int eid, webs_t wp, int argc, char_t **argv);
static void setWPS1Enable(webs_t wp, char_t *path, char_t *query);	
static void setWPS2Enable(webs_t wp, char_t *path, char_t *query);	
static void setWPSPBC1(webs_t wp, char_t *path, char_t *query);
static void setWPSPBC2(webs_t wp, char_t *path, char_t *query);
static void setWPSPIN1(webs_t wp, char_t *path, char_t *query);
static void setWPSPIN2(webs_t wp, char_t *path, char_t *query);
static int getSSIDoptions(int eid, webs_t wp, int argc, char_t **argv);

//ACL
static int getACLmode(int eid, webs_t wp, int argc, char_t **argv);
static int getWifiACL1(int eid, webs_t wp, int argc, char_t **argv);
static int getWifiACL2(int eid, webs_t wp, int argc, char_t **argv);
static void setACLmode1(webs_t wp, char_t *path, char_t *query);
static void setACLmode2(webs_t wp, char_t *path, char_t *query);
static void setWifiACLDel1(webs_t wp, char_t *path, char_t *query);
static void setWifiACLDel2(webs_t wp, char_t *path, char_t *query);
static void setWifiACL1(webs_t wp, char_t *path, char_t *query);
static void setWifiACL2(webs_t wp, char_t *path, char_t *query);

//Wi-Fi 802.11ac
static void setWifiAC(webs_t wp, char_t *path, char_t *query);
static void setWPS3Enable(webs_t wp, char_t *path, char_t *query);	
static void setWPSPBC3(webs_t wp, char_t *path, char_t *query);
static void setWPSPIN3(webs_t wp, char_t *path, char_t *query);
static int getWifiACL3(int eid, webs_t wp, int argc, char_t **argv);
static void setACLmodeAC(webs_t wp, char_t *path, char_t *query);
static void setWifiACLDel3(webs_t wp, char_t *path, char_t *query);
static void setWifiACL3(webs_t wp, char_t *path, char_t *query);
static int getWifiRadioAC(int eid, webs_t wp, int argc, char_t **argv);
static void setWifiRadioAC(webs_t wp, char_t *path, char_t *query);

//Wi-Fi by P02001 & P02002
static int getWifiLeftSubmenu(int eid, webs_t wp, int argc, char_t **argv);

#if defined(__P02003_Dora)
static int getWiFiAP(int eid, webs_t wp, int argc, char_t **argv);
static void setWiFiAP(webs_t wp, char_t *path, char_t *query);
static int getWiFiEdit(int eid, webs_t wp, int argc, char_t **argv);
static int getwifiActFlag(int eid, webs_t wp, int argc, char_t **argv);
static void setWiFiEdit(webs_t wp, char_t *path, char_t *query);
static int getWiFiSSID(int eid, webs_t wp, int argc, char_t **argv);
static void setWifiSSID(webs_t wp, char_t *path, char_t *query);

int wifi_edit_idx = -1;
int wifi_ssid_edit_idx = -1;

typedef struct NameSSID
{
    char name[128];
}DumpNameSSID;

static int getWiFiSSIDEditInfo(int eid, webs_t wp, int argc, char_t **argv);
static int getWiFiNameIsSel(int eid, webs_t wp, int argc, char_t **argv);
static int getWiFiStatusIsTrue(int eid, webs_t wp, int argc, char_t **argv);
static void setWiFiSSIDEdit(webs_t wp, char_t *path, char_t *query);

static int getWifiACLSetting(int eid, webs_t wp, int argc, char_t **argv);
static void setWifiACLSetting(webs_t wp, char_t *path, char_t *query);
static int getWifiACLList(int eid, webs_t wp, int argc, char_t **argv);
static void setWifiACLList(webs_t wp, char_t *path, char_t *query);
static int getWifiACLConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setWifiACLConfig(webs_t wp, char_t *path, char_t *query);
static int getWifiACLListCount(int eid, webs_t wp, int argc, char_t **argv);
enum
{
	mac_filtering_none=0,
	mac_filtering_same_name=1
};
int P02003_mac_filtering_status = mac_filtering_none;
static int getMacFilteringErrorStatus(int eid, webs_t wp, int argc, char_t **argv);
#endif

void initWifi()
{
	websAspDefine(T("getWifi"), getWifi);
    websFormDefine(T("setWifi"), setWifi);

	websAspDefine(T("getWifiRadio"), getWifiRadio);
    websFormDefine(T("setWifiRadio"), setWifiRadio);

	websAspDefine(T("getWPSInfo"), getWPSInfo);
	websFormDefine(T("setWPS1Enable"), setWPS1Enable);
	websFormDefine(T("setWPS2Enable"), setWPS1Enable);	
	websFormDefine(T("setWPSPBC1"), setWPSPBC1);
	websFormDefine(T("setWPSPBC2"), setWPSPBC2);
	websFormDefine(T("setWPSPIN1"), setWPSPIN1);
	websFormDefine(T("setWPSPIN2"), setWPSPIN2);
	websAspDefine(T("getSSIDoptions"), getSSIDoptions);

	//ACL
	websAspDefine(T("getACLmode"), getACLmode);
	websAspDefine(T("getWifiACL1"), getWifiACL1);
	websAspDefine(T("getWifiACL2"), getWifiACL2);
	websFormDefine(T("setACLmode1"), setACLmode1);
	websFormDefine(T("setACLmode2"), setACLmode2);
	websFormDefine(T("setWifiACLDel1"), setWifiACLDel1);
	websFormDefine(T("setWifiACLDel2"), setWifiACLDel2);
	websFormDefine(T("setWifiACL1"), setWifiACL1);
	websFormDefine(T("setWifiACL2"), setWifiACL2);

	//Wi-Fi 802.11ac
	websFormDefine(T("setWifiAC"), setWifiAC);
	websFormDefine(T("setWPSPIN3"), setWPSPIN3);
	websFormDefine(T("setWPS3Enable"), setWPS3Enable);
	websFormDefine(T("setWPSPBC3"), setWPSPBC3);
	websAspDefine(T("getWifiACL3"), getWifiACL3);
	websFormDefine(T("setACLmodeAC"), setACLmodeAC);
	websFormDefine(T("setWifiACLDel3"), setWifiACLDel3);
	websFormDefine(T("setWifiACL3"), setWifiACL3);
	websAspDefine(T("getWifiRadioAC"), getWifiRadioAC);
	websFormDefine(T("setWifiRadioAC"), setWifiRadioAC);

	//Wi-Fi by P02001 & P02002
	websAspDefine(T("getWifiLeftSubmenu"), getWifiLeftSubmenu);
	
#if defined(__P02003_Dora)
	websAspDefine(T("getWiFiAP"), getWiFiAP);
	websFormDefine(T("setWiFiAP"), setWiFiAP);
	websAspDefine(T("getWiFiEdit"), getWiFiEdit);
	websAspDefine(T("getwifiActFlag"), getwifiActFlag);
	websFormDefine(T("setWiFiEdit"), setWiFiEdit);
	websAspDefine(T("getWiFiSSID"), getWiFiSSID);
	websFormDefine(T("setWifiSSID"), setWifiSSID);
	websAspDefine(T("getWiFiSSIDEditInfo"), getWiFiSSIDEditInfo);
	websAspDefine(T("getWiFiNameIsSel"), getWiFiNameIsSel);
	websAspDefine(T("getWiFiStatusIsTrue"), getWiFiStatusIsTrue);
	websFormDefine(T("setWiFiSSIDEdit"), setWiFiSSIDEdit);

 websAspDefine(T("getWifiACLSetting"), getWifiACLSetting);
 websFormDefine(T("setWifiACLSetting"), setWifiACLSetting);
 websAspDefine(T("getWifiACLList"), getWifiACLList);
 websFormDefine(T("setWifiACLList"), setWifiACLList);
 websAspDefine(T("getWifiACLConfig"), getWifiACLConfig);
 websFormDefine(T("setWifiACLConfig"), setWifiACLConfig);
 websAspDefine(T("getWifiACLListCount"), getWifiACLListCount);
 websAspDefine(T("getMacFilteringErrorStatus"), getMacFilteringErrorStatus); 
#endif

 return ;
}

static int getWifi(int eid, webs_t wp, int argc, char_t **argv)
{
	    char** ppQueryRes = NULL;
	    int nRows=0,nCols=0,status=0;
	    char* pQueryStr = NULL;
	    char* pQueryStr1 = NULL;

	    char* pEnable = NULL;
	    char* pSSID = NULL;
	    char* pBCAST = NULL;
	    char* pSec = NULL;

	    char* pPskPassAscii = NULL;
	    char* pDefWepkeyIdx = NULL;
	    char* pWepKey1 = NULL;
	    char* pWepKey2 = NULL;
	    char* pWepKey3 = NULL;
	    char* pWepKey0 = NULL;

		char* pInterval = NULL;
		char* pThreshold = NULL;

		char* pWPSEnable = NULL;

	    char *pWifi = (char *)malloc(128);
		int ret = 0;

	    if (gstrcmp(argv[0],"0") == 0)
	    {
	    	pQueryStr = "SELECT * FROM wifiProfile where profileName='wlan1'";
	    	pQueryStr1 = "SELECT * FROM wifiap where wlanName='ap1'";
	    } else if (gstrcmp(argv[0],"1") == 0){
	    	pQueryStr = "SELECT * FROM wifiProfile where profileName='wlan2'";
	    	pQueryStr1 = "SELECT * FROM wifiap where wlanName='ap2'";
	    } else if (gstrcmp(argv[0],"2") == 0){
	    	pQueryStr = "SELECT * FROM wifiProfile where profileName='wlan3'";
	    	pQueryStr1 = "SELECT * FROM wifiap where wlanName='ap3'";
	    }

	    gstrcpy(pWifi,"");
	    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	    {
	        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
	        sqlite3_free (pQueryStr);
			free(pWifi);
	        return -1;
	    }

	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ssid",pSSID, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "broadcastSSID",pBCAST, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "security",pSec, getWifiConfigOut);
	    printf("WLAN ssid:%s, BSSID:%s, SEC:%s\n", pSSID, pBCAST, pSec);

	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "pskPassAscii",pPskPassAscii, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey0",pWepKey0, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey1",pWepKey1, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey2",pWepKey2, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey3",pWepKey3, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "defWepkeyIdx",pDefWepkeyIdx, getWifiConfigOut);
	    printf("WLAN, pskascii:%s, wepkey0:%s, defWepkeyIdx:%s\n", pPskPassAscii, pWepKey0, pDefWepkeyIdx);

	    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr1, &nCols, &nRows)) == NULL)
	   	{
	   	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
	   	  sqlite3_free (pQueryStr);
		  free(pWifi);
	   	  return -1;
	   	}
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wlanEnabled",pEnable, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "beaconInterval",pInterval, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "rtsThreshold",pThreshold, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "WPSEnabled",pWPSEnable, getWifiConfigOut);

	    if (ppQueryRes)
	    {
	        sqlite3_free_table (ppQueryRes);
	        gstrcat(pWifi,pEnable);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pSSID);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pBCAST);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pSec);

	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pPskPassAscii);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey0);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey1);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey2);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey3);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pDefWepkeyIdx);
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pInterval);
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pThreshold);
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWPSEnable);
	    }

	getWifiConfigOut:
		ret = websWrite(wp, T("\"%s\""), pWifi);
		free(pWifi);
	    return ret;
}

static int getWifiRadio(int eid, webs_t wp, int argc, char_t **argv)
{
	    char** ppQueryRes = NULL;
	    int nRows=0,nCols=0,status=0;
	    char* pQueryStr = NULL;

	    char* pMode = NULL;
	    char* pChannel = NULL;
	    char* pWidth = NULL;
	    char* pSideBand = NULL;
		char* pTransmit = NULL;

	    char *pWifi = (char *)malloc(128);
		int ret = 0;

		pQueryStr = "SELECT * FROM wifiRadio where radioNo='1'";

	    gstrcpy(pWifi,"");
#ifdef __USED_WEBS_DB__
	    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
	    sqlite3 * pWebDB = NULL;
     if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
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
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "txPower",pTransmit, getWifiConfigOut);

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
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pTransmit);
	    }

	getWifiConfigOut:
		ret = websWrite(wp, T("\"%s\""), pWifi);
		free(pWifi);
	    return ret;
}

static int getWPSInfo(int eid, webs_t wp, int argc, char_t **argv)
{
      char** ppQueryRes = NULL;
      int nRows=0,nCols=0,status=0;
      char* pQueryStr = NULL;
      char* pQueryStr1 = NULL;
  
      char* pWPSEnable = NULL;
	  char* pEnrollPIN = NULL;

      char *pWifi = (char *)malloc(128);
	  int ret = 0;

      if (gstrcmp(argv[0],"0") == 0)
      {
      	pQueryStr = "SELECT WPSEnabled FROM wifiap where wlanName='ap1'";
		pQueryStr1 = "SELECT WPSEnrollPIN FROM wifiWPS where profileName='wlan1'";
      } else {
      	pQueryStr = "SELECT WPSEnabled FROM wifiap where wlanName='ap2'";
		pQueryStr1 = "SELECT WPSEnrollPIN FROM wifiWPS where profileName='wlan1'";
      }

      gstrcpy(pWifi,"");

	  // WiFiAP: WPSEnabled
#ifdef __USED_WEBS_DB__
      if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
      sqlite3 * pWebDB = NULL;
      if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
      {
          printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
          sqlite3_free (pQueryStr);
		  free(pWifi);
          return -1;
      }
      DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "WPSEnabled",pWPSEnable, getWifiConfigOut);
      printf("WPS, pWPSEnable:%s\n", pWPSEnable);
  
      // WiFiWPS: WPSEnrollPIN      
#ifdef __USED_WEBS_DB__
      if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr1, &nCols, &nRows)) == NULL)
#else
      if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr1, &nCols, &nRows)) == NULL)
#endif
      {
     	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
     	  sqlite3_free (pQueryStr);
		  free(pWifi);
     	  return -1;
      }
  	  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "WPSEnrollPIN",pEnrollPIN, getWifiConfigOut);
	  printf("WPS, pEnrollPIN:%s\n", pEnrollPIN);

      if (ppQueryRes)
      {
          sqlite3_free_table (ppQueryRes);
          gstrcat(pWifi,pWPSEnable);
          gstrcat(pWifi,pSep);
          gstrcat(pWifi,pEnrollPIN);
      }
  
  getWifiConfigOut:
	  ret = websWrite(wp, T("\"%s\""), pWifi);
	  free(pWifi);
	  return ret;
}

static int getACLmode(int eid, webs_t wp, int argc, char_t **argv)
{
      char** ppQueryRes = NULL;
      int nRows=0,nCols=0,status=0;
      char* pQueryStr = NULL;
  
      char* pACLPolicy1 = NULL;
	  char* pACLPolicy2 = NULL;
	  char* pACLPolicy3 = NULL;
	  int ret = 0;
      char *pWifi = (char *)malloc(128);
      pQueryStr = "SELECT * FROM wifiACLconfig where profileName='wifiacl'";

	  gstrcpy(pWifi,"");

	  // wifiACLconfig: ACLPolicy1/ACLPolicy2/ACLPolicy3
#ifdef __USED_WEBS_DB__
      if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
      sqlite3 * pWebDB = NULL;
      if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
      {
          printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
          sqlite3_free (pQueryStr);
		  free(pWifi);
          return -1;
      }
      DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ACLPolicy1",pACLPolicy1, getWifiConfigOut);
	  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ACLPolicy2",pACLPolicy2, getWifiConfigOut);
	  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ACLPolicy3",pACLPolicy3, getWifiConfigOut);

      if (ppQueryRes)
      {
          sqlite3_free_table (ppQueryRes);
          gstrcat(pWifi,pACLPolicy1);
          gstrcat(pWifi,pSep);
          gstrcat(pWifi,pACLPolicy2);
		  gstrcat(pWifi,pSep);
          gstrcat(pWifi,pACLPolicy3);
      }
  
  getWifiConfigOut:
	  ret = websWrite(wp, T("\"%s\""), pWifi);
	  free(pWifi);
	  return ret;
}

static int getWifiACL1(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from wifiACLTable1";

 int index = 0;
 char* pACL_MAC = NULL;
 char* pcomment = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ACL_MAC", pACL_MAC, getWifiACL1Out);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "comment", pcomment, getWifiACL1Out);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    }
    websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_one\" id=\"acl_ck_one_%d\" onclick=\"setACLEditRowIdOne('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td><span id=\"acl_mac_one_%d\">%s</span></td>\n"), index+1, pACL_MAC);
    websWrite(wp, T("<td id=\"acl_desc_one_%d\">%s</td>\n"), index+1, pcomment);
#else
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
    }
    websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_one\" id=\"acl_ck_one_%d\" onclick=\"setACLEditRowIdOne('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td><span id=\"acl_mac_one_%d\">%s</span></td>\n"), index+1, pACL_MAC);
    websWrite(wp, T("<td id=\"acl_desc_one_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"), index+1, pcomment);
#endif
  }
 }

getWifiACL1Out:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
	
    int             rc = 0;
    char * sqlCmd = "select * from wifiACLTable1";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    sqlite3_busy_timeout(db, 1500);

    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    
    while( sqlite3_step(stmt) == SQLITE_ROW )
    {
        row_count++;
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_one\" id=\"acl_ck_one_%d\" onclick=\"setACLEditRowIdOne('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td><span id=\"acl_mac_one_%d\">%s</span></td>\n"),row_count,(const char*)sqlite3_column_text(stmt, 0));
        websWrite(wp, T("<td id=\"acl_desc_one_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"),row_count,(const char*)sqlite3_column_text(stmt, 1));
    }

    printf("row_count:%d \n",row_count);

    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
#endif
}

static int getWifiACL2(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from wifiACLTable2";

 int index = 0;
 char* pACL_MAC = NULL;
 char* pcomment = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ACL_MAC", pACL_MAC, getWifiACL2Out);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "comment", pcomment, getWifiACL2Out);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    }
    websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_two\" id=\"acl_ck_two_%d\" onclick=\"setACLEditRowIdTwo('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td><span id=\"acl_mac_two_%d\">%s</span></td>\n"), index+1, pACL_MAC);
    websWrite(wp, T("<td id=\"acl_desc_two_%d\">%s</td>\n"), index+1, pcomment);
#else
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
    }
    websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_two\" id=\"acl_ck_two_%d\" onclick=\"setACLEditRowIdTwo('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td><span id=\"acl_mac_two_%d\">%s</span></td>\n"), index+1, pACL_MAC);
    websWrite(wp, T("<td style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"acl_desc_two_%d\">%s</td>\n"), index+1, pcomment);
#endif
  }
 }

getWifiACL2Out:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
	
    int             rc = 0;
    char * sqlCmd = "select * from wifiACLTable2";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    sqlite3_busy_timeout(db, 1500);

    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    
    while( sqlite3_step(stmt) == SQLITE_ROW )
    {
        row_count++;
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_two\" id=\"acl_ck_two_%d\" onclick=\"setACLEditRowIdTwo('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td><span id=\"acl_mac_two_%d\">%s</span></td>\n"),row_count,(const char*)sqlite3_column_text(stmt, 0));
        websWrite(wp, T("<td id=\"acl_desc_two_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"),row_count,(const char*)sqlite3_column_text(stmt, 1));
    }

    printf("row_count:%d \n",row_count);

    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
#endif
}

static void setWifi(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pwlanEnabled1 = websGetVar(wp, T("wlanEnabled1"), T(""));
    char_t * pssid1 = websGetVar(wp, T("ssid1"), T(""));
    char_t * pbroadcastSSID1 = websGetVar(wp, T("bssid_en1"), T(""));
    char_t * psecurity1 = websGetVar(wp, T("enc_idx1"), T(""));
    char_t * ppskascii1 = websGetVar(wp, T("pskascii1"), T(""));
    char_t * pwepkey01 = websGetVar(wp, T("wepkey01"), T(""));
    char_t * pwepkey11 = websGetVar(wp, T("wepkey11"), T(""));
    char_t * pwepkey21 = websGetVar(wp, T("wepkey21"), T(""));
    char_t * pwepkey31 = websGetVar(wp, T("wepkey31"), T(""));
    char_t * pwepkeyIdx1 = websGetVar(wp, T("wepkeyIdx1"), T(""));

    char_t * pwlanEnabled2 = websGetVar(wp, T("wlanEnabled2"), T(""));
    char_t * pssid2 = websGetVar(wp, T("ssid2"), T(""));
    char_t * pbroadcastSSID2 = websGetVar(wp, T("bssid_en2"), T(""));
    char_t * psecurity2 = websGetVar(wp, T("enc_idx2"), T(""));
    char_t * ppskascii2 = websGetVar(wp, T("pskascii2"), T(""));
    char_t * pwepkey02 = websGetVar(wp, T("wepkey02"), T(""));
    char_t * pwepkey12 = websGetVar(wp, T("wepkey12"), T(""));
    char_t * pwepkey22 = websGetVar(wp, T("wepkey22"), T(""));
    char_t * pwepkey32 = websGetVar(wp, T("wepkey32"), T(""));
    char_t * pwepkeyIdx2 = websGetVar(wp, T("wepkeyIdx2"), T(""));

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	// wifiProfile table
	if (strcmp(websGetVar(wp, T("bssid_en1"), T("")), "on") == 0)
	{
		pbroadcastSSID1 = "1";
	} else {
		pbroadcastSSID1 = "0";
	}

	if (strcmp(websGetVar(wp, T("bssid_en2"), T("")), "on") == 0)
	{
		pbroadcastSSID2 = "1";
	} else {
		pbroadcastSSID2 = "0";
	}

    if (strcmp(websGetVar(wp, T("if_idx"), T("")), "0") == 0)
	{
        	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfile set "
        			"pskPassAscii='%s', "
        			"wepkey0='%s', wepkey1='%s', wepkey2='%s', wepkey3='%s', defWepkeyIdx='%s', "
        			"ssid='%s', "
       			"broadcastSSID='%s', "
       			"security='%s' where profilename='wlan1'",
       			ppskascii1, pwepkey01, pwepkey11, pwepkey21, pwepkey31, pwepkeyIdx1, pssid1, pbroadcastSSID1, psecurity1);
       #ifdef __USED_WEBS_DB__
       		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
       #else
       		status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
       #endif
    } else { 
        	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfile set "
        	 			"pskPassAscii='%s', "
        	 			"wepkey0='%s', wepkey1='%s', wepkey2='%s', wepkey3='%s', defWepkeyIdx='%s', "
        	 			"ssid='%s', "
        				"broadcastSSID='%s', "
        				"security='%s' where profilename='wlan2'",
        				ppskascii2, pwepkey02, pwepkey12, pwepkey22, pwepkey32, pwepkeyIdx2, pssid2, pbroadcastSSID2, psecurity2);
        #ifdef __USED_WEBS_DB__
        			status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
        #else
        			status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
        #endif
    } 

	// wifiAP table
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	if (strcmp(websGetVar(wp, T("ap1en"), T("")), "on") == 0)
	{
		pwlanEnabled1 = "1";
	} else {
		pwlanEnabled1 = "0";
	}

	if (strcmp(websGetVar(wp, T("ap2en"), T("")), "on") == 0)
	{
		pwlanEnabled2 = "1";
	} else {
		pwlanEnabled2 = "0";
	}

    memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set "
	 			"wlanEnabled='%s' where wlanName='ap1'", pwlanEnabled1);
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

    memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set "
		 			"wlanEnabled='%s' where wlanName='ap2'", pwlanEnabled2);
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='2'");
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif	

	if (status>=0)
	{
		//export db to ascii
		//printf("pWifiQry : %s \n", pCWMPQry);
		system(EXPORT_CMD);
	}
    websRedirect(wp, T("wifi/wifi_settings_wait.asp"));
}

static void setWifiRadio(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pMode = websGetVar(wp, T("wifi_mode"), T(""));
    char_t * pWidth1 = websGetVar(wp, T("wifi_width1"), T(""));
	char_t * pWidth2 = websGetVar(wp, T("wifi_width2"), T(""));
    char_t * pBand = websGetVar(wp, T("wifi_band"), T(""));
	char_t * pChannel0 = websGetVar(wp, T("wifi_channel0"), T(""));
	char_t * pChannela = websGetVar(wp, T("wifi_channel1"), T(""));
    char_t * pChannelb = websGetVar(wp, T("wifi_channel2"), T(""));

    char_t * pInterval = websGetVar(wp, T("wifi_interval"), T(""));
    char_t * pThreshold = websGetVar(wp, T("wifi_threshold"), T(""));

	char_t * pTransmit = websGetVar(wp, T("wifi_transmit_power"), T(""));

    char_t * pWidth = NULL;
	char_t * pChannel = NULL;

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	//check Wi-Fi Mode null
	if (strcmp(websGetVar(wp, T("wifi_threshold"), T("")), "") == 0){
		return websRedirect(wp, T("wifi/wifi_advsettings.asp"));
	}

	// wifiRadio table
	if ((strcmp(websGetVar(wp, T("wifi_mode"), T("")), "b") == 0)||
		(strcmp(websGetVar(wp, T("wifi_mode"), T("")), "g") == 0))
	{
	  pWidth = pWidth1;
	  pChannel = pChannel0;
	}else{
	  if (strcmp(websGetVar(wp, T("wifi_width2"), T("")), "20") == 0)
	  {
	    pWidth = pWidth2;
	    pChannel = pChannel0;
	  }else{
	    pWidth = pWidth2;
	    if (strcmp(websGetVar(wp, T("wifi_band"), T("")), "0") == 0)
		  pChannel = pChannela;
        else if (strcmp(websGetVar(wp, T("wifi_band"), T("")), "1") == 0)
		  pChannel = pChannelb;
	  }
	}

	//printf("[setWifiRadio]Mode: %s, Width:%s, Band:%s, Channel:%s\n", pMode, pWidth, pBand, pChannel);

 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiRadio set "
 			"opMode='%s', configuredChannel='%s', chanWidth='%s', sideBand='%s', txPower='%s' where radioNo = '1'"
 			, pMode, pChannel, pWidth, pBand, pTransmit);
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

	// wifiAP table
	//memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	//snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set beaconInterval='%s', rtsThreshold='%s'"
 	//		, pInterval, pThreshold);
//#ifdef __USED_WEBS_DB__
	//status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
//#else
	//status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
//#endif

    memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='3'");
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

	if (status>=0)
	{
		//export db to ascii
		//printf("pWifiQry : %s \n", pCWMPQry);
		system(EXPORT_CMD);
	}
    websRedirect(wp, T("wifi/wifi_advsettings_wait.asp"));
}

static void setWPS1Enable(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
	char_t * pSSID = websGetVar(wp, T("ssid_dummy"), T(""));
	char_t * pWPS = websGetVar(wp, T("ap_en1"), T("off"));
	char_t * pWPS3 = websGetVar(wp, T("ap_en3"), T("off"));

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	//printf("setWPS1Enable==pSSID=%s, pWPS=%s, pWPS3=%s\n", pSSID, pWPS, pWPS3);
    if (strcmp(websGetVar(wp, T("ssid_dummy"), T("")), "0") == 0){   //802.11n
    	if (strcmp(websGetVar(wp, T("ap_en1"), T("")), "on") == 0){
    		pWPS = "1";
    	} else {
    		pWPS = "0";
    	}
		snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set WPSEnabled='%s' where wlanName='ap1'",pWPS);
#ifdef __USED_WEBS_DB__
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
		status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
		snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='4'");
#ifdef __USED_WEBS_DB__
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
		status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif
    }else if (strcmp(websGetVar(wp, T("ssid_dummy"), T("")), "1") == 0){  //802.11ac
    	if (strcmp(websGetVar(wp, T("ap_en3"), T("")), "on") == 0){
    		pWPS = "1";
    	} else {
    		pWPS = "0";
    	}
		snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set WPSEnabled='%s' where wlanName='ap3'",pWPS);
#ifdef __USED_WEBS_DB__
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
		status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
		snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='ac'");
#ifdef __USED_WEBS_DB__
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
		status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif
    }

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}
	
    websRedirect(wp, T("wifi/wifi_wps_wait.asp"));
}

static void setWPS2Enable(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
	char_t * pWPS = websGetVar(wp, T("ap_en2"), T("off"));

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	if (strcmp(websGetVar(wp, T("ap_en2"), T("")), "on") == 0)
	{
		pWPS = "1";
	} else {
		pWPS = "0";
	}

    snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set WPSEnabled='%s' where wlanName='ap2'",pWPS);
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}
	
    websRedirect(wp, T("wifi/wifi_wps_wait.asp"));
}

static void setWPSPBC1(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

    if (strcmp(websGetVar(wp, T("pbc1_dummy"), T("")), "1") == 0){   //802.11n
	  // wifiWPS table
 	  snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='PBC' where profileName='wlan1'");
#ifdef __USED_WEBS_DB__
	  status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	  status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif
    }else if (strcmp(websGetVar(wp, T("pbc1_dummy"), T("")), "3") == 0){	 //802.11ac
 	  snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='PBC' where profileName='wlan3'");
#ifdef __USED_WEBS_DB__
	  status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	  status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif
    }

    websRedirect(wp, T("wifi/wifi_wps_process.asp"));
}

static void setWPSPBC2(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	// wifiWPS table
 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='PBC' where profileName='wlan2'");
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

    websRedirect(wp, T("wifi/wifi_wps_process.asp"));
}

static void setWPSPIN1(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pPIN = websGetVar(wp, T("PIN1"), T(""));
	
 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;
 	if (strcmp(websGetVar(wp, T("pin1_dummy"), T("")), "1") == 0){   		//802.11n
	  // wifiWPS table
 	  snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='EnPIN', WPSEnrollPIN='%s' where profileName='wlan1'", pPIN);
#ifdef __USED_WEBS_DB__
	  status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	  status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif
 	}else if (strcmp(websGetVar(wp, T("pin1_dummy"), T("")), "3") == 0){	 //802.11ac
 	  // wifiWPS table
 	  snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='EnPIN', WPSEnrollPIN='%s' where profileName='wlan3'", pPIN);
#ifdef __USED_WEBS_DB__
	  status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	  status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif
	}

    websRedirect(wp, T("wifi/wifi_wps_process.asp"));
}

static void setWPSPIN2(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pPIN = websGetVar(wp, T("PIN2"), T(""));
	int status = -1;
	
 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));

	// wifiWPS table
 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='EnPIN', WPSEnrollPIN='%s' where profileName='wlan2'", pPIN);
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

    websRedirect(wp, T("wifi/wifi_wps_process.asp"));
}

static int getSSIDoptions(int eid, webs_t wp, int argc, char_t **argv)
{
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 //char * pQueryStr = "select * from wifiProfile";
 char * pQueryStr = "select * from wifiProfile where profileName='wlan1' or profileName='wlan3'";

 int index = 0;
 char* pSSID = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   return 0;
 }

 if (nRows > 0) {
   //DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "ssid", pSSID, getSSIDoptions);
   for (index = 0; index < nRows; index++){
     DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ssid", pSSID, getSSIDoptions);
     websWrite(wp, T("<option value=%s >%s</option>"), pSSID, pSSID);
   }
 }

getSSIDoptions:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 //return websWrite(wp, T("%s"),pSSID);
 return 0;
}

static void setACLmode1(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pACLmode = websGetVar(wp, T("acl_idx1"), T(""));
	int status = -1;
	
 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));

	//wifiACLconfig table
 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiACLconfig set ACLPolicy1='%s' where profileName='wifiacl'", pACLmode);
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}

    websRedirect(wp, T("wifi/wifi_acl.asp"));
}	

static void setACLmode2(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pACLmode = websGetVar(wp, T("acl_idx2"), T(""));
	int status = -1;
	
 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));

	//wifiACLconfig table
 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiACLconfig set ACLPolicy2='%s' where profileName='wifiacl'", pACLmode);
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}

    websRedirect(wp, T("wifi/wifi_acl.asp"));
}	

static void setWifiACLDel1(webs_t wp, char_t *path, char_t *query)
{
    printf("%s\n", __func__);
    char * pId = websGetVar(wp, T("acl_rowid_del1"), T(""));
    //printf("[Wi-Fi:ACL1]Del Id: %s\n",pId);
    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM wifiACLTable1 WHERE _ROWID_='%s'",pId);
        //printf("pPortDelQry: %s \n",pPortDelQry);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

		memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
	    snprintf (pPortDelQry, sizeof(pPortDelQry),"update wifiACLconfig set ACLUpdate='1'");
#ifdef __USED_WEBS_DB__
	    status =  jnrp_sql_dbEXE_Webs(pPortDelQry);
#else
	    status =  jnrp_sql_dbEXE(pDBFile, pPortDelQry);
#endif

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
			printf("pPortDelQry: %s \n",pPortDelQry);
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
        }
    }

    websRedirect(wp, T("wifi/wifi_acl.asp"));
}

static void setWifiACLDel2(webs_t wp, char_t *path, char_t *query)
{
    printf("%s\n", __func__);
    char * pId = websGetVar(wp, T("acl_rowid_del2"), T(""));
    //printf("[Wi-Fi:ACL2] Del Id: %s\n",pId);
    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM wifiACLTable2 WHERE _ROWID_='%s'",pId);
        //printf("pPortDelQry: %s \n",pPortDelQry);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

		memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
	    snprintf (pPortDelQry, sizeof(pPortDelQry),"update wifiACLconfig set ACLUpdate='2'");
#ifdef __USED_WEBS_DB__
	    status =  jnrp_sql_dbEXE_Webs(pPortDelQry);
#else
	    status =  jnrp_sql_dbEXE(pDBFile, pPortDelQry);
#endif

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
			printf("pPortDelQry: %s \n",pPortDelQry);
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
        }
    }

    websRedirect(wp, T("wifi/wifi_acl.asp"));
}

static void setWifiACL1(webs_t wp, char_t *path, char_t *query)
{
    printf("%s\n", __func__);
    char_t * pEditMode = websGetVar(wp, T("editflag1"), T(""));
    char_t * pRowId = websGetVar(wp, T("acl_rowid_edit1"), T(""));
    char * pErrMsg = NULL;
    int status = -1;

    char_t * pIp = websGetVar(wp, T("acl_mac1"), T(""));
    char_t * pDesc = websGetVar(wp, T("acl_desc1"), T(""));

	//printf("setWifiACL1, pEditMode:%s, pRowId:%s, pIp:%s, pDesc:%s\n", pEditMode, pRowId, pIp, pDesc);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setWifiACL add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into wifiACLTable1 values ('%s','%s')", pIp, pDesc);
		//printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    } else if (!gstrcmp(pEditMode,"edit")) {
        //printf("setWifiACL edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update wifiACLTable1 set ACL_MAC='%s', comment='%s' where _ROWID_='%s'", pIp, pDesc, pRowId);
		//printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

	memset (pPortQry, 0x00, sizeof(pPortQry));
	snprintf (pPortQry, sizeof(pPortQry),"update wifiACLconfig set ACLUpdate='3'");
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pPortQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pPortQry);
#endif

    if (pDB)
        sqlite3_close (pDB);

    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }
    
    websRedirect(wp, T("wifi/wifi_acl.asp"));
}

static void setWifiACL2(webs_t wp, char_t *path, char_t *query)
{
    printf("%s\n", __func__);
    char_t * pEditMode = websGetVar(wp, T("editflag2"), T(""));
    char_t * pRowId = websGetVar(wp, T("acl_rowid_edit2"), T(""));
    char * pErrMsg = NULL;
    int status = -1;

    char_t * pIp = websGetVar(wp, T("acl_mac2"), T(""));
    char_t * pDesc = websGetVar(wp, T("acl_desc2"), T(""));

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setWifiACL add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into wifiACLTable2 values ('%s','%s')", pIp, pDesc);
		//printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    } else if (!gstrcmp(pEditMode,"edit")) {
        //printf("setWifiACL edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update wifiACLTable2 set ACL_MAC='%s', comment='%s' where _ROWID_='%s'", pIp, pDesc, pRowId);
		//printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

	memset (pPortQry, 0x00, sizeof(pPortQry));
	snprintf (pPortQry, sizeof(pPortQry),"update wifiACLconfig set ACLUpdate='4'");
#ifdef __USED_WEBS_DB__
	status =  jnrp_sql_dbEXE_Webs(pPortQry);
#else
	status =  jnrp_sql_dbEXE(pDBFile, pPortQry);
#endif

    if (pDB)
        sqlite3_close (pDB);

    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }
    
    websRedirect(wp, T("wifi/wifi_acl.asp"));

}

//Wi-Fi 802.11ac---START
static void setWifiAC(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pwlanEnabled3 = websGetVar(wp, T("wlanEnabled3"), T(""));
	char_t * pssid3 = websGetVar(wp, T("ssid3"), T(""));
    char_t * pbroadcastSSID3 = websGetVar(wp, T("bssid_en3"), T(""));
    char_t * psecurity3 = websGetVar(wp, T("enc_idx3"), T(""));
    char_t * ppskascii3 = websGetVar(wp, T("pskascii3"), T(""));
    char_t * pwepkey03 = websGetVar(wp, T("wepkey03"), T(""));
    char_t * pwepkey13 = websGetVar(wp, T("wepkey13"), T(""));
    char_t * pwepkey23 = websGetVar(wp, T("wepkey23"), T(""));
    char_t * pwepkey33 = websGetVar(wp, T("wepkey33"), T(""));
    char_t * pwepkeyIdx3 = websGetVar(wp, T("wepkeyIdx3"), T(""));

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	// wifiProfile table
	if (strcmp(websGetVar(wp, T("bssid_en3"), T("")), "on") == 0)
	{
		pbroadcastSSID3 = "1";
	} else {
		pbroadcastSSID3 = "0";
	}

    if (strcmp(websGetVar(wp, T("if_idx"), T("")), "0") == 0)
	{
       snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfile set "
        			"pskPassAscii='%s', "
        			"wepkey0='%s', wepkey1='%s', wepkey2='%s', wepkey3='%s', defWepkeyIdx='%s', "
        			"ssid='%s', "
       				"broadcastSSID='%s', "
       				"security='%s' where profilename='wlan3'",
       			ppskascii3, pwepkey03, pwepkey13, pwepkey23, pwepkey33, pwepkeyIdx3, pssid3, pbroadcastSSID3, psecurity3);
       status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
    }

	// wifiAP table
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	if (strcmp(websGetVar(wp, T("ap3en"), T("")), "on") == 0)
	{
		pwlanEnabled3 = "1";
	} else {
		pwlanEnabled3 = "0";
	}

    memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set "
	 			"wlanEnabled='%s' where wlanName='ap3'", pwlanEnabled3);
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='ac'");
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}
    websRedirect(wp, T("wifi/wifi_settings_ac_wait.asp"));
}

static void setWPS3Enable(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
	char_t * pWPS = websGetVar(wp, T("ap_en3"), T("off"));

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	if (strcmp(websGetVar(wp, T("ap_en3"), T("")), "on") == 0){
		pWPS = "1";
	} else {
		pWPS = "0";
	}

    snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set WPSEnabled='%s' where wlanName='ap3'",pWPS);
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='ac'");
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}
	
    websRedirect(wp, T("wifi/wifi_wps_ac_wait.asp"));
}

static void setWPSPBC3(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	// wifiWPS table
 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='PBC' where profileName='wlan3'");
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

    websRedirect(wp, T("wifi/wifi_wps_process_ac.asp"));
}

static void setWPSPIN3(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pPIN = websGetVar(wp, T("PIN3"), T(""));
	
 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	// wifiWPS table
 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiWPS set WPSmethod='EnPIN', WPSEnrollPIN='%s' where profileName='wlan3'", pPIN);
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

    websRedirect(wp, T("wifi/wifi_wps_process_ac.asp"));
}

static int getWifiACL3(int eid, webs_t wp, int argc, char_t **argv)
{
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from wifiACLTable3";

 int index = 0;
 char* pACL_MAC = NULL;
 char* pcomment = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)	{
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ACL_MAC", pACL_MAC, getWifiACL1Out);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "comment", pcomment, getWifiACL1Out);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
      if (index%2)
      {
        websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
      }else{
        websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
      }
      websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_three\" id=\"acl_ck_three_%d\" onclick=\"setACLEditRowIdThree('%d')\"/></td>\n"), index+1, index+1);
      websWrite(wp, T("<td><span id=\"acl_mac_three_%d\">%s</span></td>\n"), index+1, pACL_MAC);
      websWrite(wp, T("<td id=\"acl_desc_three_%d\">%s</td>\n"), index+1, pcomment);
#else
      if (index%2)
      {
        websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
      }else{
        websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
      }
      websWrite(wp, T("<td><input type=\"radio\" name=\"acl_ck_three\" id=\"acl_ck_three_%d\" onclick=\"setACLEditRowIdThree('%d')\"/></td>\n"), index+1, index+1);
      websWrite(wp, T("<td><span id=\"acl_mac_three_%d\">%s</span></td>\n"), index+1, pACL_MAC);
      websWrite(wp, T("<td id=\"acl_desc_three_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"), index+1, pcomment);
#endif
   }
 }

getWifiACL1Out:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
}

static void setACLmodeAC(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pACLmode = websGetVar(wp, T("acl_idx3"), T(""));
	int status = -1;
	
 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));

	//wifiACLconfig table
 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiACLconfig set ACLPolicy3='%s' where profileName='wifiacl'", pACLmode);
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	if (status>=0)
	{
	  //export db to ascii
	  system(EXPORT_CMD);
	}

    websRedirect(wp, T("wifi/wifi_acl_ac.asp"));
}

static void setWifiACLDel3(webs_t wp, char_t *path, char_t *query)
{
    printf("%s\n", __func__);
    char * pId = websGetVar(wp, T("acl_rowid_del3"), T(""));
    //printf("[Wi-Fi:ACL1]Del Id: %s\n",pId);
    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM wifiACLTable3 WHERE _ROWID_='%s'",pId);
        //printf("pPortDelQry: %s \n",pPortDelQry);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

		memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
	    snprintf (pPortDelQry, sizeof(pPortDelQry),"update wifiACLconfig set ACLUpdate='3'");
	    status =  jnrp_sql_dbEXE_Webs(pPortDelQry);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
			printf("pPortDelQry: %s \n",pPortDelQry);
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
        }
    }

    websRedirect(wp, T("wifi/wifi_acl_ac.asp"));
}

static void setWifiACL3(webs_t wp, char_t *path, char_t *query)
{
    printf("%s\n", __func__);
    char_t * pEditMode = websGetVar(wp, T("editflag3"), T(""));
    char_t * pRowId = websGetVar(wp, T("acl_rowid_edit3"), T(""));
    char * pErrMsg = NULL;
    int status = -1;

    char_t * pIp = websGetVar(wp, T("acl_mac3"), T(""));
    char_t * pDesc = websGetVar(wp, T("acl_desc3"), T(""));

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setWifiACL add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into wifiACLTable3 values ('%s','%s')", pIp, pDesc);
		//printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    } else if (!gstrcmp(pEditMode,"edit")) {
        //printf("setWifiACL edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update wifiACLTable3 set ACL_MAC='%s', comment='%s' where _ROWID_='%s'", pIp, pDesc, pRowId);
		//printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

	memset (pPortQry, 0x00, sizeof(pPortQry));
	snprintf (pPortQry, sizeof(pPortQry),"update wifiACLconfig set ACLUpdate='5'");
	status =  jnrp_sql_dbEXE_Webs(pPortQry);

    if (pDB)
        sqlite3_close (pDB);

    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }
    
    websRedirect(wp, T("wifi/wifi_acl_ac.asp"));
}

static int getWifiRadioAC(int eid, webs_t wp, int argc, char_t **argv)
{
	    char** ppQueryRes = NULL;
	    int nRows=0,nCols=0,status=0;
	    char* pQueryStr = NULL;

	    char* pMode = NULL;
	    char* pChannel = NULL;
	    char* pWidth = NULL;
	    char* pSideBand = NULL;
		char* pTransmit = NULL;
		int ret = 0;
	    char *pWifi = (char *)malloc(128);

		pQueryStr = "SELECT * FROM wifiRadio where radioNo='2'";

	    gstrcpy(pWifi,"");
	    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
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
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "txPower",pTransmit, getWifiConfigOut);

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
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pTransmit);
	    }

	getWifiConfigOut:
		ret = websWrite(wp, T("\"%s\""), pWifi);
		free(pWifi);
		return ret;
}

static void setWifiRadioAC(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pMode = websGetVar(wp, T("wifi_mode"), T(""));
    char_t * pWidth1 = websGetVar(wp, T("wifi_width1"), T(""));
	char_t * pWidth2 = websGetVar(wp, T("wifi_width2"), T(""));
	char_t * pWidth3 = websGetVar(wp, T("wifi_width3"), T(""));	
    char_t * pBand = websGetVar(wp, T("wifi_band"), T(""));
	char_t * pChannel0 = websGetVar(wp, T("wifi_channel20"), T(""));
	char_t * pChannela = websGetVar(wp, T("wifi_channel31"), T(""));
    char_t * pChannelb = websGetVar(wp, T("wifi_channel32"), T(""));
	char_t * pChannel2 = websGetVar(wp, T("wifi_channel80"), T(""));

	//char_t * pInterval = websGetVar(wp, T("wifi_interval"), T(""));
    //char_t * pThreshold = websGetVar(wp, T("wifi_threshold"), T(""));

	char_t * pTransmit = websGetVar(wp, T("wifi_transmit_power"), T(""));

    char_t * pWidth = NULL;
	char_t * pChannel = NULL;

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

	// wifiRadio table
	if (strcmp(websGetVar(wp, T("wifi_mode"), T("")), "a") == 0){
	  pWidth = pWidth1;
	  pChannel = pChannel0;
	}else if (strcmp(websGetVar(wp, T("wifi_mode"), T("")), "an") == 0){
	  pWidth = pWidth2;	
	  if (strcmp(websGetVar(wp, T("wifi_width2"), T("")), "20") == 0){
	    pChannel = pChannel0;
	  }else{
	    if (strcmp(websGetVar(wp, T("wifi_band"), T("")), "0") == 0)
		  pChannel = pChannela;
        else if (strcmp(websGetVar(wp, T("wifi_band"), T("")), "1") == 0)
		  pChannel = pChannelb;
	  }
	}else if (strcmp(websGetVar(wp, T("wifi_mode"), T("")), "ac") == 0){
	  pWidth = pWidth3;	
	  if (strcmp(websGetVar(wp, T("wifi_width3"), T("")), "20") == 0){
	    pChannel = pChannel0;
	  }else if (strcmp(websGetVar(wp, T("wifi_width3"), T("")), "80") == 0){
	    pChannel = pChannel2;
	  }else{
	    if (strcmp(websGetVar(wp, T("wifi_band"), T("")), "0") == 0)
		  pChannel = pChannela;
        else if (strcmp(websGetVar(wp, T("wifi_band"), T("")), "1") == 0)
		  pChannel = pChannelb;
	  }
	}

	//printf("[setWifiRadio]Mode: %s, Width:%s, Band:%s, Channel:%s\n", pMode, pWidth, pBand, pChannel);

 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiRadio set "
 			"opMode='%s', configuredChannel='%s', chanWidth='%s', sideBand='%s',  txPower='%s' where radioNo = '2'"
 			, pMode, pChannel, pWidth, pBand, pTransmit);
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	// wifiAP table
	//memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	//snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set beaconInterval='%s', rtsThreshold='%s' where wlanName = 'ap3'"
 	//		, pInterval, pThreshold);
	//status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

    memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='ac'");
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}
    websRedirect(wp, T("wifi/wifi_advsettings_ac_wait.asp"));
}
//Wi-Fi 802.11ac---END

//Wi-Fi by P02001 & P02002 -- START
static int getWifiLeftSubmenu(int eid, webs_t wp, int argc, char_t **argv)
{
	if(argv[0] == NULL) {
		websWrite(wp, T("\"%s\""),"");	
		return 0;
	}else if (!gstrcmp(argv[0],"11")) {
#if defined(__P02001_Dora)
	 	websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings.asp\">802.11n</a></li>\n"));
		websWrite(wp, T("<dl>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbuhover\"><script>document.write(gettext(\"Wi-Fi Settings\"));</script></dd>\n"));
		websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_wps.asp\">WPS</a></dd>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_acl.asp\"><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></a></dd>\n"));
		websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_advsettings.asp\"><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></a></dd>\n"));		
		websWrite(wp, T("</dl>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings_ac.asp\">802.11ac</a></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));	
		websWrite(wp, T("</div>\n"));	
#else	
		websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbuhover\"><a href=\"wifi_settings.asp\"></a><script>document.write(gettext(\"Wi-Fi Settings\"));</script></li>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_wps.asp\">WPS</a></li>\n"));		
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_acl.asp\"><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></a></li>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_advsettings.asp\"><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></a></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));
		websWrite(wp, T("</div>\n"));		
#endif		
	}else if (!gstrcmp(argv[0],"12")) {
#if defined(__P02001_Dora)	
	 	websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings.asp\">802.11n</a></li>\n"));
		websWrite(wp, T("<dl>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_settings.asp\"><script>document.write(gettext(\"Wi-Fi Settings\"));</script></a></dd>\n"));
		websWrite(wp, T("<dd class=\"ddbuhover\">WPS</dd>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_acl.asp\"><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></a></dd>\n"));
		websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_advsettings.asp\"><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></a></dd>\n"));		
		websWrite(wp, T("</dl>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings_ac.asp\">802.11ac</a></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));	
		websWrite(wp, T("</div>\n"));	
#else
		websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings.asp\"><script>document.write(gettext(\"Wi-Fi Settings\"));</script></a></li>\n"));
		websWrite(wp, T("<li class=\"leftbuhover\"><a href=\"wifi_wps.asp\"></a>WPS</li>\n"));		
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_acl.asp\"><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></a></li>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_advsettings.asp\"><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></a></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));
		websWrite(wp, T("</div>\n"));
#endif
	}else if (!gstrcmp(argv[0],"13")) {
#if defined(__P02001_Dora)	
	 	websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings.asp\">802.11n</a></li>\n"));
		websWrite(wp, T("<dl>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_settings.asp\"><script>document.write(gettext(\"Wi-Fi Settings\"));</script></a></dd>\n"));
		websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_wps.asp\">WPS</a></dd>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbuhover\"><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></dd>\n"));
		websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_advsettings.asp\"><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></a></dd>\n"));		
		websWrite(wp, T("</dl>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings_ac.asp\">802.11ac</a></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));	
		websWrite(wp, T("</div>\n"));
#else
		websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings.asp\"><script>document.write(gettext(\"Wi-Fi Settings\"));</script></a></li>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_wps.asp\">WPS</a></li>\n"));		
	 	websWrite(wp, T("<li class=\"leftbuhover\"><a href=\"wifi_acl.asp\"></a><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></li>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_advsettings.asp\"><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></a></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));
		websWrite(wp, T("</div>\n"));
#endif
	}else if (!gstrcmp(argv[0],"14")) {
#if defined(__P02001_Dora)	
	 	websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings.asp\">802.11n</a></li>\n"));
		websWrite(wp, T("<dl>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_settings.asp\"><script>document.write(gettext(\"Wi-Fi Settings\"));</script></a></dd>\n"));
		websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_wps.asp\">WPS</a></dd>\n"));		
	 	websWrite(wp, T("<dd class=\"ddbu\"><a href=\"wifi_acl.asp\"><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></a></dd>\n"));	
		websWrite(wp, T("<dd class=\"ddbuhover\"><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></dd>\n"));		
		websWrite(wp, T("</dl>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings_ac.asp\">802.11ac</a></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));	
		websWrite(wp, T("</div>\n"));
#else
		websWrite(wp, T("<div id=\"left_bg\">\n"));
		websWrite(wp, T("<ul>\n"));
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_settings.asp\"><script>document.write(gettext(\"Wi-Fi Settings\"));</script></a></li>\n"));
		websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_wps.asp\">WPS</a></li>\n"));		
	 	websWrite(wp, T("<li class=\"leftbu\"><a href=\"wifi_acl.asp\"><script>document.write(gettext(\"Wi-Fi Access Control\"));</script></a></li>\n"));
		websWrite(wp, T("<li class=\"leftbuhover\"><a href=\"wifi_advsettings.asp\"></a><script>document.write(gettext(\"Wi-Fi Advanced\"));</script></li>\n"));		
	 	websWrite(wp, T("</ul>\n"));
		websWrite(wp, T("</div>\n"));
#endif
	}
	return 0;
}
//Wi-Fi by P02001 & P02002 -- END

#if defined(__P02003_Dora)
static int getWiFiAP(int eid, webs_t wp, int argc, char_t **argv)
{
  int nRows=0,nCols=0,status=0;
  char** ppQueryRes1 = NULL;
  char** ppQueryRes2 = NULL;
  char** ppQueryRes3 = NULL;
  char** ppQueryRes4 = NULL;
  char** ppQueryRes5 = NULL;
  char** ppQueryRes6 = NULL;
  char* pQueryStr1 = NULL;
  char* pQueryStr2 = NULL;
  char* pQueryStr3 = NULL;
  char* pQueryStr4 = NULL;
  char* pQueryStr5 = NULL;
  char* pQueryStr6 = NULL;

  //Wi-Fi AP1
  char* pEnable1 = NULL;
  char* pSSID1 = NULL;
  char* pBCAST1 = NULL;
  char* pSec1 = NULL;

  //Wi-Fi AP2
  char* pEnable2 = NULL;
  char* pSSID2 = NULL;
  char* pBCAST2 = NULL;
  char* pSec2 = NULL;

  //Wi-Fi AP3
  char* pEnable3 = NULL;
  char* pSSID3 = NULL;
  char* pBCAST3 = NULL;
  char* pSec3 = NULL;

  //Wi-Fi AP4
  char* pEnable4 = NULL;
  char* pSSID4 = NULL;
  char* pBCAST4 = NULL;
  char* pSec4 = NULL;

  //Activate AP1
  char* pAct1 = NULL;
  char* pStartHr1 = NULL;
  char* pStartMin1 = NULL;
  char* pStartMeridian1 = NULL;
  char* pStopHr1 = NULL;
  char* pStopMin1 = NULL;
  char* pStopMeridian1 = NULL;
  
  //Activate AP2
  char* pAct2 = NULL;
  char* pStartHr2 = NULL;
  char* pStartMin2 = NULL;
  char* pStartMeridian2 = NULL;
  char* pStopHr2 = NULL;
  char* pStopMin2 = NULL;
  char* pStopMeridian2 = NULL;

  //Activate AP3
  char* pAct3 = NULL;
  char* pStartHr3 = NULL;
  char* pStartMin3 = NULL;
  char* pStartMeridian3 = NULL;
  char* pStopHr3 = NULL;
  char* pStopMin3 = NULL;
  char* pStopMeridian3 = NULL;

  //Activate AP4
  char* pAct4 = NULL;
  char* pStartHr4 = NULL;
  char* pStartMin4 = NULL;
  char* pStartMeridian4 = NULL;
  char* pStopHr4 = NULL;
  char* pStopMin4 = NULL;
  char* pStopMeridian4 = NULL;	
  
  pQueryStr1 = "SELECT * FROM wifiProfile";
  pQueryStr2 = "SELECT * FROM wifiap";
  pQueryStr3 = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0' AND scheduleStatus='1'";
  pQueryStr4 = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0.0' AND scheduleStatus='1'";
  pQueryStr5 = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath0' AND scheduleStatus='1'";
  pQueryStr6 = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath1' AND scheduleStatus='1'";
  
  if ((ppQueryRes1 = sqlite3QueryResGet_Webs (pQueryStr1, &nCols, &nRows)) == NULL)
  {
      printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr1);
      sqlite3_free (pQueryStr1);
      return -1;
  }
  
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 0, "ssid",pSSID1, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 0, "broadcastSSID",pBCAST1, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 0, "security",pSec1, getWifiConfigOut);
  //printf("WLAN SSID1:%s, BSSID1:%s, SEC1:%s\n", pSSID1, pBCAST1, pSec1);

  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 1, "ssid",pSSID2, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 1, "broadcastSSID",pBCAST2, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 1, "security",pSec2, getWifiConfigOut);
  //printf("WLAN SSID2:%s, BSSID2:%s, SEC2:%s\n", pSSID2, pBCAST2, pSec2);

  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 2, "ssid",pSSID3, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 2, "broadcastSSID",pBCAST3, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 2, "security",pSec3, getWifiConfigOut);
  //printf("WLAN SSID3:%s, BSSID3:%s, SEC3:%s\n", pSSID3, pBCAST3, pSec3);

  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 3, "ssid",pSSID4, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 3, "broadcastSSID",pBCAST4, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes1, nCols, 3, "security",pSec4, getWifiConfigOut);
  //printf("WLAN SSID4:%s, BSSID4:%s, SEC4:%s\n", pSSID4, pBCAST4, pSec4);
  
  if ((ppQueryRes2 = sqlite3QueryResGet_Webs (pQueryStr2, &nCols, &nRows)) == NULL)
  	{
  	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr2);
  	  sqlite3_free (pQueryStr2);
  	  return -1;
  	}
  
  DB_TABLE_VALUE_GET (ppQueryRes2, nCols, 0, "wlanEnabled",pEnable1, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes2, nCols, 1, "wlanEnabled",pEnable2, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes2, nCols, 2, "wlanEnabled",pEnable3, getWifiConfigOut);
  DB_TABLE_VALUE_GET (ppQueryRes2, nCols, 3, "wlanEnabled",pEnable4, getWifiConfigOut);
  //printf("pEnable1:%s, pEnable2:%s, pEnable3:%s, pEnable4:%s\n", pEnable1, pEnable2, pEnable3, pEnable4);

  if ((ppQueryRes3 = sqlite3QueryResGet_Webs (pQueryStr3, &nCols, &nRows)) == NULL) {
  	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr3);
      sqlite3_free (pQueryStr3);
      return -1;
  }else{
	  if (nRows == 0){ 
  	  	pAct1 = "Disabled";
	  	pStartHr1 = "--";
	  	pStartMin1 = "--";
		pStartMeridian1="";
	  	pStopHr1 = "--";
	  	pStopMin1 = "--";
		pStopMeridian1="";
  	  }else{
      	DB_TABLE_VALUE_GET (ppQueryRes3, nCols, 0, "scheduleStatus",pAct1, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes3, nCols, 0, "startHour",pStartHr1, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes3, nCols, 0, "startMinute",pStartMin1, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes3, nCols, 0, "startMeridian",pStartMeridian1, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes3, nCols, 0, "stopHour",pStopHr1, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes3, nCols, 0, "stopMinute",pStopMin1, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes3, nCols, 0, "stopMeridian",pStopMeridian1, getWifiConfigOut);
  	  }
  }

  if (!gstrcmp(pStartMeridian1,"0")){
    pStartMeridian1 = "AM";
  }else if (!gstrcmp(pStartMeridian1,"1")){
    pStartMeridian1 = "PM";
  }	

  if (!gstrcmp(pStopMeridian1,"0")){
    pStopMeridian1 = "AM";
  }else if (!gstrcmp(pStopMeridian1,"1")){
    pStopMeridian1 = "PM";
  }
  //printf("Enter pAct1:%s, pStartHr1:%s,pStopHr1:%s, pStopMeridian1:%s\n", pAct1, pStartHr1, pStopHr1, pStopMeridian1);

  if ((ppQueryRes4 = sqlite3QueryResGet_Webs (pQueryStr4, &nCols, &nRows)) == NULL) {
  	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr4);
      sqlite3_free (pQueryStr4);
      return -1;
  }else{
  	  if (nRows == 0){ 
  	  	pAct2 = "Disabled";
	  	pStartHr2 = "--";
	  	pStartMin2 = "--";
		pStartMeridian2="";
	  	pStopHr2 = "--";
	  	pStopMin2 = "--";
		pStopMeridian2="";
  	  }else{	
      	DB_TABLE_VALUE_GET (ppQueryRes4, nCols, 0, "scheduleStatus",pAct2, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes4, nCols, 0, "startHour",pStartHr2, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes4, nCols, 0, "startMinute",pStartMin2, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes4, nCols, 0, "startMeridian",pStartMeridian2, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes4, nCols, 0, "stopHour",pStopHr2, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes4, nCols, 0, "stopMinute",pStopMin2, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes4, nCols, 0, "stopMeridian",pStopMeridian2, getWifiConfigOut);
  	  }
  }

  if (!gstrcmp(pStartMeridian2,"0")){
    pStartMeridian2 = "AM";
  }else if (!gstrcmp(pStartMeridian2,"1")){
    pStartMeridian2 = "PM";
  }	
  
  if (!gstrcmp(pStopMeridian2,"0")){
    pStopMeridian2 = "AM";
  }else if (!gstrcmp(pStopMeridian2,"1")){
    pStopMeridian2 = "PM";
  }
  //printf("Enter pAct2:%s, pStartHr2:%s,pStopHr2:%s,pStopMeridian2=%s\n", pAct2, pStartHr2, pStopHr2,pStopMeridian2);

  if ((ppQueryRes5 = sqlite3QueryResGet_Webs (pQueryStr5, &nCols, &nRows)) == NULL) {
  	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr5);
      sqlite3_free (pQueryStr5);
      return -1;
  }else{
  	  if (nRows == 0){ 
  	  	pAct3 = "Disabled";
	  	pStartHr3 = "--";
	  	pStartMin3 = "--";
		pStartMeridian3="";
	  	pStopHr3 = "--";
	  	pStopMin3 = "--";
		pStopMeridian3="";
  	  }else{	
      	DB_TABLE_VALUE_GET (ppQueryRes5, nCols, 0, "scheduleStatus",pAct3, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes5, nCols, 0, "startHour",pStartHr3, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes5, nCols, 0, "startMinute",pStartMin3, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes5, nCols, 0, "startMeridian",pStartMeridian3, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes5, nCols, 0, "stopHour",pStopHr3, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes5, nCols, 0, "stopMinute",pStopMin3, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes5, nCols, 0, "stopMeridian",pStopMeridian3, getWifiConfigOut);
  	  }
  }

  if (!gstrcmp(pStartMeridian3,"0")){
    pStartMeridian3 = "AM";
  }else if (!gstrcmp(pStartMeridian3,"1")){
    pStartMeridian3 = "PM";
  }	
  
  if (!gstrcmp(pStopMeridian3,"0")){
    pStopMeridian3 = "AM";
  }else if (!gstrcmp(pStopMeridian3,"1")){
    pStopMeridian3 = "PM";
  }
  //printf("Enter pAct3:%s, pStartHr3:%s,pStopHr3:%s,pStopMeridian3=%s\n", pAct3, pStartHr3, pStopHr3,pStopMeridian3);

  if ((ppQueryRes6 = sqlite3QueryResGet_Webs (pQueryStr6, &nCols, &nRows)) == NULL) {
  	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr6);
      sqlite3_free (pQueryStr6);
      return -1;
  }else{
  	  if (nRows == 0){ 
  	  	pAct4 = "Disabled";
	  	pStartHr4 = "--";
	  	pStartMin4 = "--";
		pStartMeridian4="";
	  	pStopHr4 = "--";
	  	pStopMin4 = "--";
		pStopMeridian4="";
  	  }else{	
      	DB_TABLE_VALUE_GET (ppQueryRes6, nCols, 0, "scheduleStatus",pAct4, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes6, nCols, 0, "startHour",pStartHr4, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes6, nCols, 0, "startMinute",pStartMin4, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes6, nCols, 0, "startMeridian",pStartMeridian4, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes6, nCols, 0, "stopHour",pStopHr4, getWifiConfigOut);
      	DB_TABLE_VALUE_GET (ppQueryRes6, nCols, 0, "stopMinute",pStopMin4, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes6, nCols, 0, "stopMeridian",pStopMeridian4, getWifiConfigOut);
  	  }
  }

  if (!gstrcmp(pStartMeridian4,"0")){
    pStartMeridian4 = "AM";
  }else if (!gstrcmp(pStartMeridian4,"1")){
    pStartMeridian4 = "PM";
  }	
  
  if (!gstrcmp(pStopMeridian4,"0")){
    pStopMeridian4 = "AM";
  }else if (!gstrcmp(pStopMeridian4,"1")){
    pStopMeridian4 = "PM";
  }
  //printf("Enter pAct4:%s, pStartHr4:%s,pStopHr4:%s,pStopMeridian4=%s\n", pAct4, pStartHr4, pStopHr4,pStopMeridian4);
  
  //AP1	
  websWrite(wp, T("<tr>\n"));
  websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"),1);
  if (!gstrcmp(pEnable1,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Disabled\"));</script></span></td>\n"),1);
  else
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Enabled\"));</script></span></td>\n"),1);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_ssid_%d\">%s</td>\n"),1, pSSID1);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_freq_%d\">%s</td>\n"),1, "2.4GHz");
  if (!gstrcmp(pBCAST1,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"off\"));</script></td>\n"),1);
  else
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"on\"));</script></td>\n"),1);
  if (!gstrcmp(pSec1,"WPAWPA2")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),1, "WPA+WPA2");
  }else if(!gstrcmp(pSec1,"OPEN")){
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\"><script>document.write(gettext(\"OPEN\"));</script></td>\n"),1);
  }else{
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),1, pSec1);
  }	
  if (!gstrcmp(pAct1,"Disabled")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),1);
  }else{
    if (!gstrcmp(pAct1,"0"))			
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),1);
    else
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Enabled\"));</script></td>\n"),1);
  }	
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),1, pStartHr1, pStartMin1, pStartMeridian1);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_stop_%d\">%s:%s %s</td>\n"),1, pStopHr1, pStopMin1, pStopMeridian1);
  websWrite(wp, T("</tr>\n"));	

  //AP2	
  websWrite(wp, T("<tr>\n"));
  websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"),2);
  if (!gstrcmp(pEnable2,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Disabled\"));</script></span></td>\n"),2);
  else
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Enabled\"));</script></span></td>\n"),2);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_ssid_%d\">%s</td>\n"),2, pSSID2);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_freq_%d\">%s</td>\n"),2, "2.4GHz");
  if (!gstrcmp(pBCAST2,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"off\"));</script></td>\n"),2);
  else
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"on\"));</script></td>\n"),2);
  if (!gstrcmp(pSec2,"WPAWPA2")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),2, "WPA+WPA2");
  }else if(!gstrcmp(pSec2,"OPEN")){
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\"><script>document.write(gettext(\"OPEN\"));</script></td>\n"),2);
  }else{
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),2, pSec2);
  }
  if (!gstrcmp(pAct2,"Disabled")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),2);
  }else{
    if (!gstrcmp(pAct2,"0"))			
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),2);
    else
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Enabled\"));</script></td>\n"),2);
  }
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),2, pStartHr2, pStartMin2, pStartMeridian2);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_stop_%d\">%s:%s %s</td>\n"),2, pStopHr2, pStopMin2, pStopMeridian2);
  websWrite(wp, T("</tr>\n"));

  //AP3	
  websWrite(wp, T("<tr>\n"));
  websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"),3);
  if (!gstrcmp(pEnable3,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Disabled\"));</script></span></td>\n"),3);
  else
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Enabled\"));</script></span></td>\n"),3);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_ssid_%d\">%s</td>\n"),3, pSSID3);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_freq_%d\">%s</td>\n"),3, "5GHz");
  if (!gstrcmp(pBCAST3,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"off\"));</script></td>\n"),3);
  else
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"on\"));</script></td>\n"),3);
  if (!gstrcmp(pSec3,"WPAWPA2")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),3, "WPA+WPA2");
  }else if(!gstrcmp(pSec3,"OPEN")){
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\"><script>document.write(gettext(\"OPEN\"));</script></td>\n"),3);
  }else{
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),3, pSec3);
  }
  if (!gstrcmp(pAct3,"Disabled")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),3);
  }else{
    if (!gstrcmp(pAct3,"0"))			
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),3);
    else
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Enabled\"));</script></td>\n"),3);
  }
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),3, pStartHr3, pStartMin3, pStartMeridian3);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_stop_%d\">%s:%s %s</td>\n"),3, pStopHr3, pStopMin3, pStopMeridian3);
  websWrite(wp, T("</tr>\n"));

  //AP4
  websWrite(wp, T("<tr>\n"));
  websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"),4);
  if (!gstrcmp(pEnable4,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Disabled\"));</script></span></td>\n"),4);
  else
	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"wifi_status_%d\"><script>document.write(gettext(\"Enabled\"));</script></span></td>\n"),4);
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_ssid_%d\">%s</td>\n"),4, pSSID4);
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_freq_%d\">%s</td>\n"),4, "5GHz");
  if (!gstrcmp(pBCAST4,"0"))			
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"off\"));</script></td>\n"),4);
  else
	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_brod_%d\"><script>document.write(gettext(\"on\"));</script></td>\n"),4);
  if (!gstrcmp(pSec4,"WPAWPA2")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),4, "WPA+WPA2");
  }else if(!gstrcmp(pSec4,"OPEN")){
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\"><script>document.write(gettext(\"OPEN\"));</script></td>\n"),4);
  }else{
    websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_sec_%d\">%s</td>\n"),4, pSec4);
  }
  if (!gstrcmp(pAct4,"Disabled")){
  	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),4);
  }else{
    if (!gstrcmp(pAct4,"0"))			
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Disabled\"));</script></td>\n"),4);
    else
	  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_active_%d\"><script>document.write(gettext(\"Enabled\"));</script></td>\n"),4);
  }
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),4, pStartHr4, pStartMin4, pStartMeridian4);
  websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_stop_%d\">%s:%s %s</td>\n"),4, pStopHr4, pStopMin4, pStopMeridian4);
  websWrite(wp, T("</tr>\n"));

getWifiConfigOut:
	    return 0;  
}

static void setWiFiAP(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
	char_t * pWiFiAction = websGetVar(wp, T("WiFiAction"), T(""));	
	char_t * pSWiFiIdx = websGetVar(wp, T("WiFiIdx"), T(""));
	char_t * pWiFiCount = websGetVar(wp, T("WiFiCount"), T("0"));
	int count=gatoi(pWiFiCount);	
	int index=0;
	wifi_edit_idx=gatoi(pSWiFiIdx);

	if (!gstrcmp(pWiFiAction,"WiFiEdit"))	{
		websRedirect(wp, T("wifi/wifi_setting_edit.asp"));
	} else if (!gstrcmp(pWiFiAction,"WiFiSSID")) {
		websRedirect(wp, T("wifi/wifi_setting_ssid.asp"));
	} else if (!gstrcmp(pWiFiAction,"WiFiAcl"))	{
		websRedirect(wp, T("wifi/wifi_setting_acl.asp"));
	}				
}

static int getWiFiEdit(int eid, webs_t wp, int argc, char_t **argv)
{
	    char** ppQueryRes = NULL;
	    int nRows=0,nCols=0,status=0;
	    char* pQueryStr = NULL;
	    char* pQueryStr1 = NULL;
		char* pQueryStr2 = NULL;

	    char* pEnable = NULL;
	    char* pSSID = NULL;
	    char* pBCAST = NULL;
	    char* pSec = NULL;

	    char* pPskPassAscii = NULL;
	    char* pDefWepkeyIdx = NULL;
	    char* pWepKey1 = NULL;
	    char* pWepKey2 = NULL;
	    char* pWepKey3 = NULL;
	    char* pWepKey0 = NULL;

		char* pInterval = NULL;
		char* pThreshold = NULL;
		char* pFrag = NULL;

		char* pWPSEnable = NULL;

		char* pwlan = NULL;
		int ret = 0;
	    char *pWifi = (char *)malloc(128);

	    if (wifi_edit_idx == 1)
	    {
	    	pQueryStr = "SELECT * FROM wifiProfile where profileName='wlan1'";
	    	pQueryStr1 = "SELECT * FROM wifiap where wlanName='ap1'";
			pwlan = "1";
	    } else if (wifi_edit_idx == 2){
	    	pQueryStr = "SELECT * FROM wifiProfile where profileName='wlan2'";
	    	pQueryStr1 = "SELECT * FROM wifiap where wlanName='ap2'";
			pwlan = "2";
	    } else if (wifi_edit_idx == 3){
	    	pQueryStr = "SELECT * FROM wifiProfile where profileName='wlan3'";
	    	pQueryStr1 = "SELECT * FROM wifiap where wlanName='ap3'";
			pwlan = "3";
	    } else if (wifi_edit_idx == 4){
	    	pQueryStr = "SELECT * FROM wifiProfile where profileName='wlan4'";
	    	pQueryStr1 = "SELECT * FROM wifiap where wlanName='ap4'";
			pwlan = "4";
	    }
		
	    gstrcpy(pWifi,"");
	    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	    {
	        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
	        sqlite3_free (pQueryStr);
			free(pWifi);
	        return -1;
	    }

	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ssid",pSSID, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "broadcastSSID",pBCAST, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "security",pSec, getWifiConfigOut);
	    //printf("WLAN ssid:%s, BSSID:%s, SEC:%s\n", pSSID, pBCAST, pSec);

	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "pskPassAscii",pPskPassAscii, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey0",pWepKey0, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey1",pWepKey1, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey2",pWepKey2, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wepkey3",pWepKey3, getWifiConfigOut);
	    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "defWepkeyIdx",pDefWepkeyIdx, getWifiConfigOut);
	    //printf("WLAN, pskascii:%s, wepkey0:%s, defWepkeyIdx:%s\n", pPskPassAscii, pWepKey0, pDefWepkeyIdx);

	    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr1, &nCols, &nRows)) == NULL)
	   	{
	   	  printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
	   	  sqlite3_free (pQueryStr);
		  free(pWifi);
	   	  return -1;
	   	}
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "wlanEnabled",pEnable, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "beaconInterval",pInterval, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "rtsThreshold",pThreshold, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "fragThreshold",pFrag, getWifiConfigOut);
		DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "WPSEnabled",pWPSEnable, getWifiConfigOut);

	    if (ppQueryRes)
	    {
	        sqlite3_free_table (ppQueryRes);
	        gstrcat(pWifi,pEnable);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pSSID);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pBCAST);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pSec);

	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pPskPassAscii);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey0);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey1);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey2);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWepKey3);
	        gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pDefWepkeyIdx);
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pInterval);
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pThreshold);
			gstrcat(pWifi,pSep);
			gstrcat(pWifi,pFrag);
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pWPSEnable);
			gstrcat(pWifi,pSep);
	        gstrcat(pWifi,pwlan);
	    }

	getWifiConfigOut:
		ret = websWrite(wp, T("\"%s\""), pWifi);
		free(pWifi);
		return ret;
}

static int getwifiActFlag(int eid, webs_t wp, int argc, char_t **argv)
{
 int pScheduleStatus = 0;
 char* pQueryStr = NULL;
 char** ppQueryRes = NULL;
 int nRows=0,nCols=0,status=0,index=0;

 char* pStatus = NULL;

 if (wifi_edit_idx == 1){
   pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0'";
 } else if (wifi_edit_idx == 2){
   pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0.0'";
 } else if (wifi_edit_idx == 3){
   pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath0'";
 } else if (wifi_edit_idx == 4){
   pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath1'";
 } else {
 	 return websWrite(wp, T("\"%s\""), "0");
 }

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
          sqlite3_free (pQueryStr);
          return -1;
 }

 if (nRows > 0){
      for (index = 0; index < nRows; index++) {
          DB_TABLE_VALUE_GET (ppQueryRes, nCols, index, "scheduleStatus",pStatus, getWifiConfigOut);
          if (!gstrcmp(pStatus,"1")){
            pScheduleStatus++;
          }
      }	  
 }

 getWifiConfigOut:
  if (ppQueryRes)
  {
    sqlite3_free_table(ppQueryRes);
  }

 if (pScheduleStatus == 1)
 	return websWrite(wp, T("\"%s\""), "1");
 else
 	return websWrite(wp, T("\"%s\""), "0");
}

static void setWiFiEdit(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
	char_t * penable = websGetVar(wp, T("SSIDenable"), T(""));
    char_t * pssid = websGetVar(wp, T("ssid1"), T(""));
    char_t * pbroadcastSSID = websGetVar(wp, T("bssid_en1"), T(""));
    char_t * psecurity = websGetVar(wp, T("enc_idx1"), T(""));
    char_t * ppskascii = websGetVar(wp, T("pskascii1"), T(""));
    char_t * pwepkey = websGetVar(wp, T("wepkey01"), T(""));
	char_t * pbeancon = websGetVar(wp, T("wifi_interval"), T(""));
    char_t * prts = websGetVar(wp, T("wifi_rts"), T(""));
    char_t * pfragment = websGetVar(wp, T("wifi_frag"), T(""));

	char_t * psecurity3 = websGetVar(wp, T("enc_idx3"), T(""));

	char pCWMPQry [1024] = {0};
	int status = -1;

	// check Wi-Fi ssid input null
	if (strcmp(websGetVar(wp, T("ssid1"), T("")), "") == 0){
		return websRedirect(wp, T("wifi/wifi_setting_edit.asp"));
	} 

	// wifiProfile table
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	if (strcmp(websGetVar(wp, T("SSIDenable"), T("")), "on") == 0){
		penable = "1";
	} else {
		penable = "0";
	}
	
	if (strcmp(websGetVar(wp, T("bssid_en1"), T("")), "on") == 0){
		pbroadcastSSID = "1";
	} else {
		pbroadcastSSID = "0";
	}

    if ((wifi_edit_idx == 1)||(wifi_edit_idx == 2)){
		if (strcmp(websGetVar(wp, T("enc_idx1"), T("")), "WEP") == 0){
			snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfile set "
	        "wepkey0='%s', ssid='%s', broadcastSSID='%s', security='%s' where profilename='wlan%d'",
	        pwepkey, pssid, pbroadcastSSID, psecurity,wifi_edit_idx);
		} else {
			snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfile set "
	        "pskPassAscii='%s', ssid='%s', broadcastSSID='%s', security='%s' where profilename='wlan%d'",
	        ppskascii, pssid, pbroadcastSSID, psecurity,wifi_edit_idx);
		}
        status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
    } else if ((wifi_edit_idx == 3)||(wifi_edit_idx == 4)){
    	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfile set "
        "pskPassAscii='%s', wepkey0='%s', ssid='%s', broadcastSSID='%s', security='%s' where profilename='wlan%d'",
        ppskascii, pwepkey, pssid, pbroadcastSSID, psecurity3,wifi_edit_idx);
        status =  jnrp_sql_dbEXE_Webs(pCWMPQry);    
    }

	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set wlanEnabled='%s' where wlanName='ap%d'", 
    penable, wifi_edit_idx);
	status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

	// wifiAP table
	if ((wifi_edit_idx == 1)||(wifi_edit_idx == 2)){
		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
    	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set "
    	"beaconInterval='%d', rtsThreshold='%d', fragThreshold='%d' where wlanName='ap1'", 
        gatoi(pbeancon), gatoi(prts), gatoi(pfragment));
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
    	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set "
    	"beaconInterval='%d', rtsThreshold='%d', fragThreshold='%d' where wlanName='ap2'", 
        gatoi(pbeancon), gatoi(prts), gatoi(pfragment));
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	    snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='2'");
	    status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
	} else if ((wifi_edit_idx == 3)||(wifi_edit_idx == 4)){
		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
    	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set "
    	"beaconInterval='%d', rtsThreshold='%d', fragThreshold='%d' where wlanName='ap3'", 
        gatoi(pbeancon), gatoi(prts), gatoi(pfragment));
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
    	snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiAP set "
    	"beaconInterval='%d', rtsThreshold='%d', fragThreshold='%d' where wlanName='ap4'", 
        gatoi(pbeancon), gatoi(prts), gatoi(pfragment));
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);

		memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	    snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiProfilecommit set profilecommit ='ac'");
	    status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
	} 

	if (status>=0)
	{
		//export db to ascii
		system(EXPORT_CMD);
	}
    websRedirect(wp, T("wifi/wifi_settings_wait.asp"));
}

static int getWiFiSSID(int eid, webs_t wp, int argc, char_t **argv)
{
  int nRows=0,nCols=0,status=0,index=0;
  char** ppQueryRes1 = NULL;
  char* pQueryStr1 = NULL;
  char** ppQueryRes2 = NULL;
  char* pQueryStr2 = NULL;
  char** ppQueryRes3 = NULL;
  char* pQueryStr3 = NULL;
  char** ppQueryRes4 = NULL;
  char* pQueryStr4 = NULL;

  //Wi-Fi Schedule--AP1
  char* pScheduleName1 = NULL;
  char* pScheduleStatus1 = NULL;

  char* pStartHour1 = NULL;
  char* pStartMinute1 = NULL;
  char* pStartMeridian1 = NULL;
  
  char* pStopHour1 = NULL;
  char* pStopMinute1 = NULL;
  char* pStopMaridian1 = NULL;

  //Wi-Fi Schedule--AP2
  char* pScheduleName2 = NULL;
  char* pScheduleStatus2 = NULL;

  char* pStartHour2 = NULL;
  char* pStartMinute2 = NULL;
  char* pStartMeridian2 = NULL;
  
  char* pStopHour2 = NULL;
  char* pStopMinute2 = NULL;
  char* pStopMaridian2 = NULL;

  //Wi-Fi Schedule--AP3
  char* pScheduleName3 = NULL;
  char* pScheduleStatus3 = NULL;

  char* pStartHour3 = NULL;
  char* pStartMinute3 = NULL;
  char* pStartMeridian3 = NULL;
  
  char* pStopHour3 = NULL;
  char* pStopMinute3 = NULL;
  char* pStopMaridian3 = NULL;

  //Wi-Fi Schedule--AP4
  char* pScheduleName4 = NULL;
  char* pScheduleStatus4 = NULL;

  char* pStartHour4 = NULL;
  char* pStartMinute4 = NULL;
  char* pStartMeridian4 = NULL;
  
  char* pStopHour4 = NULL;
  char* pStopMinute4 = NULL;
  char* pStopMaridian4 = NULL;


  if (wifi_edit_idx == 1)
  {
  	pQueryStr1 = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0'";
	if ((ppQueryRes1 = sqlite3QueryResGet_Webs (pQueryStr1, &nCols, &nRows)) == NULL)
    {
          sqlite3_free (pQueryStr1);
          return -1;
    }
    if (nRows > 0){
      for (index = 0; index < nRows; index++) {
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "scheduleName",pScheduleName1, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "scheduleStatus",pScheduleStatus1, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "startHour",pStartHour1, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "startMinute",pStartMinute1, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "startMeridian",pStartMeridian1, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "stopHour",pStopHour1, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "stopMinute",pStopMinute1, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes1, nCols, index, "stopMeridian",pStopMaridian1, getWifiConfigOut);
          
          if (!gstrcmp(pScheduleStatus1,"0")){
            pScheduleStatus1 = "Not active";
          }else if (!gstrcmp(pScheduleStatus1,"1")){
            pScheduleStatus1 = "Active";
          }
          
          if (!gstrcmp(pStartMeridian1,"0")){
            pStartMeridian1 = "AM";
          }else if (!gstrcmp(pStartMeridian1,"1")){
            pStartMeridian1 = "PM";
          }	
          
          if (!gstrcmp(pStopMaridian1,"0")){
            pStopMaridian1 = "AM";
          }else if (!gstrcmp(pStopMaridian1,"1")){
            pStopMaridian1 = "PM";
          }
          
          websWrite(wp, T("<tr>\n"));
          websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"), index+1);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_name_%d\">%s</td>\n"),index+1, pScheduleName1);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),index+1, pStartHour1, pStartMinute1, pStartMeridian1);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_end_%d\">%s:%s %s</td>\n"),index+1, pStopHour1, pStopMinute1, pStopMaridian1);
	   if (!gstrcmp(pScheduleStatus1,"Not active"))
          	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Not active\"));</script></td>\n"),index+1);
	   else if (!gstrcmp(pScheduleStatus1,"Active"))
	   	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Active\"));</script></td>\n"),index+1);
	   else
	   	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\">%s</td>\n"),index+1, pScheduleStatus1);
          websWrite(wp, T("</tr>\n"));
        }
    }  
  } else if (wifi_edit_idx == 2){
  	pQueryStr2 = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0.0'";
	if ((ppQueryRes2 = sqlite3QueryResGet_Webs (pQueryStr2, &nCols, &nRows)) == NULL)
    {
          sqlite3_free (pQueryStr2);
          return -1;
    }
    if (nRows > 0){
      for (index = 0; index < nRows; index++) {
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "scheduleName",pScheduleName2, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "scheduleStatus",pScheduleStatus2, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "startHour",pStartHour2, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "startMinute",pStartMinute2, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "startMeridian",pStartMeridian2, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "stopHour",pStopHour2, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "stopMinute",pStopMinute2, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes2, nCols, index, "stopMeridian",pStopMaridian2, getWifiConfigOut);
          
          if (!gstrcmp(pScheduleStatus2,"0")){
            pScheduleStatus2 = "Not active";
          }else if (!gstrcmp(pScheduleStatus2,"1")){
            pScheduleStatus2 = "Active";
          }
          
          if (!gstrcmp(pStartMeridian2,"0")){
            pStartMeridian2 = "AM";
          }else if (!gstrcmp(pStartMeridian2,"1")){
            pStartMeridian2 = "PM";
          }	
          
          if (!gstrcmp(pStopMaridian2,"0")){
            pStopMaridian2 = "AM";
          }else if (!gstrcmp(pStopMaridian2,"1")){
            pStopMaridian2 = "PM";
          }
          
          websWrite(wp, T("<tr>\n"));
          websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"), index+1);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_name_%d\">%s</td>\n"),index+1, pScheduleName2);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),index+1, pStartHour2, pStartMinute2, pStartMeridian2);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_end_%d\">%s:%s %s</td>\n"),index+1, pStopHour2, pStopMinute2, pStopMaridian2);
	   if (!gstrcmp(pScheduleStatus2,"Not active"))
          	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Not active\"));</script></td>\n"),index+1);
	   else if (!gstrcmp(pScheduleStatus2,"Active"))
	   	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Active\"));</script></td>\n"),index+1);
	   else
          	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\">%s</td>\n"),index+1, pScheduleStatus2);
          websWrite(wp, T("</tr>\n"));
        }
    }  
  } else if (wifi_edit_idx == 3){
  	pQueryStr3 = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath0'";
	if ((ppQueryRes3 = sqlite3QueryResGet_Webs (pQueryStr3, &nCols, &nRows)) == NULL)
    {
          sqlite3_free (pQueryStr3);
          return -1;
    }
    if (nRows > 0){
      for (index = 0; index < nRows; index++) {
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "scheduleName",pScheduleName3, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "scheduleStatus",pScheduleStatus3, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "startHour",pStartHour3, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "startMinute",pStartMinute3, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "startMeridian",pStartMeridian3, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "stopHour",pStopHour3, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "stopMinute",pStopMinute3, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes3, nCols, index, "stopMeridian",pStopMaridian3, getWifiConfigOut);
          
          if (!gstrcmp(pScheduleStatus3,"0")){
            pScheduleStatus3 = "Not active";
          }else if (!gstrcmp(pScheduleStatus3,"1")){
            pScheduleStatus3 = "Active";
          }
          
          if (!gstrcmp(pStartMeridian3,"0")){
            pStartMeridian3 = "AM";
          }else if (!gstrcmp(pStartMeridian3,"1")){
            pStartMeridian3 = "PM";
          }	
          
          if (!gstrcmp(pStopMaridian3,"0")){
            pStopMaridian3 = "AM";
          }else if (!gstrcmp(pStopMaridian3,"1")){
            pStopMaridian3 = "PM";
          }
          
          websWrite(wp, T("<tr>\n"));
          websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"), index+1);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_name_%d\">%s</td>\n"),index+1, pScheduleName3);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),index+1, pStartHour3, pStartMinute3, pStartMeridian3);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_end_%d\">%s:%s %s</td>\n"),index+1, pStopHour3, pStopMinute3, pStopMaridian3);
	   if (!gstrcmp(pScheduleStatus3,"Not active"))
          	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Not active\"));</script></td>\n"),index+1);
	   else if (!gstrcmp(pScheduleStatus3,"Active"))
	   	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Active\"));</script></td>\n"),index+1);
	   else
          	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\">%s</td>\n"),index+1, pScheduleStatus3);
          websWrite(wp, T("</tr>\n"));
        }
    }  
  } else if (wifi_edit_idx == 4){
  	pQueryStr4 = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath1'";
	if ((ppQueryRes4 = sqlite3QueryResGet_Webs (pQueryStr4, &nCols, &nRows)) == NULL)
    {
          sqlite3_free (pQueryStr4);
          return -1;
    }
    if (nRows > 0){
      for (index = 0; index < nRows; index++) {
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "scheduleName",pScheduleName4, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "scheduleStatus",pScheduleStatus4, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "startHour",pStartHour4, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "startMinute",pStartMinute4, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "startMeridian",pStartMeridian4, getWifiConfigOut);
          
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "stopHour",pStopHour4, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "stopMinute",pStopMinute4, getWifiConfigOut);
          DB_TABLE_VALUE_GET (ppQueryRes4, nCols, index, "stopMeridian",pStopMaridian4, getWifiConfigOut);
          
          if (!gstrcmp(pScheduleStatus4,"0")){
            pScheduleStatus4 = "Not active";
          }else if (!gstrcmp(pScheduleStatus4,"1")){
            pScheduleStatus4 = "Active";
          }
          
          if (!gstrcmp(pStartMeridian4,"0")){
            pStartMeridian4 = "AM";
          }else if (!gstrcmp(pStartMeridian4,"1")){
            pStartMeridian4 = "PM";
          }	
          
          if (!gstrcmp(pStopMaridian4,"0")){
            pStopMaridian4 = "AM";
          }else if (!gstrcmp(pStopMaridian4,"1")){
            pStopMaridian4 = "PM";
          }
          
          websWrite(wp, T("<tr>\n"));
          websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"wifi_ck\" id=\"wifi_ck_%d\" /></td>\n"), index+1);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_name_%d\">%s</td>\n"),index+1, pScheduleName4);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_start_%d\">%s:%s %s</td>\n"),index+1, pStartHour4, pStartMinute4, pStartMeridian4);
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_end_%d\">%s:%s %s</td>\n"),index+1, pStopHour4, pStopMinute4, pStopMaridian4);
	   if (!gstrcmp(pScheduleStatus4,"Not active"))
          	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Not active\"));</script></td>\n"),index+1);
	   else if (!gstrcmp(pScheduleStatus4,"Active"))
	   	websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\"><script>document.write(gettext(\"Active\"));</script></td>\n"),index+1);
	   else
          websWrite(wp, T("<td class=\"tdOdd\" id=\"wifi_status_%d\">%s</td>\n"),index+1, pScheduleStatus4);
          websWrite(wp, T("</tr>\n"));
        }
    }  
  }
  getWifiConfigOut:
  	return 0;  
}

static void setWifiSSID(webs_t wp, char_t *path, char_t *query)
{
	char * pWiFiAction = websGetVar(wp, T("WiFiAction"), T(""));	
	char_t * pWiFiIdx = websGetVar(wp, T("WiFiIdx"), T(""));
	char_t * pWiFiCount = websGetVar(wp, T("WiFiCount"), T("0"));
	char_t * pWiFiName = websGetVar(wp, T("WiFiName"), T(""));
	int count=gatoi(pWiFiCount);	
	int index=0;
	char *str = NULL;
	DumpNameSSID dumpName[count];
	
	//printf("pWiFiAction = %s, pWiFiIdx=%s, pWiFiCount=%s\n",pWiFiAction,pWiFiIdx,pWiFiCount);	
	//printf("pWiFiName = %s\n", pWiFiName);
	if (!gstrcmp(pWiFiAction,"WiFiAdd"))	{
		wifi_ssid_edit_idx=-1;
		websRedirect(wp, T("wifi/wifi_setting_ssid_edit.asp"));
	} else if (!gstrcmp(pWiFiAction,"WiFiEdit")) {
		wifi_ssid_edit_idx=gatoi(pWiFiIdx);
		websRedirect(wp, T("wifi/wifi_setting_ssid_edit.asp"));
	} else if (!gstrcmp(pWiFiAction,"WiFiDel"))	{
		char * pErrMsg = NULL;
		int status = -1;	

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("advanced/wifi_setting_ssid_edit.asp"));
		}
		
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

  char * pProfileName = NULL;
  if (wifi_edit_idx == 1) {
   pProfileName = "wlan0";
  } else if (wifi_edit_idx == 2) {
   pProfileName = "wlan0.0";
  } else if (wifi_edit_idx == 3) {
   pProfileName = "ath0";
  } else if (wifi_edit_idx == 4) {
   pProfileName = "ath1";
  }

  if (wifi_edit_idx == 1 || wifi_edit_idx == 2 || wifi_edit_idx == 3 || wifi_edit_idx == 4) {
    str = strtok(pWiFiName, ",");
    if (str != NULL) strcpy(dumpName[0].name, str);
    snprintf (pSQLCmd, sizeof(pSQLCmd),"DELETE FROM wifiSchedules WHERE associatedSSID='%s' AND scheduleName='%s' AND scheduleStatus='0'", pProfileName, dumpName[0].name);
    status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
    for (index = 1; index < count; index++) {
     memset(dumpName[index].name, 0, sizeof(dumpName[index].name));
     str = strtok(NULL, ",");
     if (str != NULL) strcpy(dumpName[index].name, str);
     snprintf (pSQLCmd, sizeof(pSQLCmd),"DELETE FROM wifiSchedules WHERE associatedSSID='%s' AND scheduleName='%s' AND scheduleStatus='0'", pProfileName, dumpName[index].name);
     status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
    }  	
  }

		status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);
		if (pDB)
			sqlite3_close (pDB);
	
		if (status>=0){
			sqlite3_free (pErrMsg);
			system(EXPORT_CMD);
		}				
		websRedirect(wp, T("wifi/wifi_setting_ssid.asp"));
	}	
}

static int getWiFiSSIDEditInfo(int eid, webs_t wp, int argc, char_t **argv)
{
  char pWiFiSSIDInfo[128];
  int nRows=0,nCols=0, Rows=0;
  char * pQueryStr = NULL;
  char** ppQueryRes = NULL;

  char* pScheduleName = NULL;
  char* pScheduleStatus = NULL;
  char* pStartHour = NULL;
  char* pStartMinute = NULL;
  char* pStartMeridian = NULL;
  char* pStopHour = NULL;
  char* pStopMinute = NULL;
  char* pStopMaridian = NULL;

  if (wifi_edit_idx == 1){
  	  if (wifi_ssid_edit_idx == -1) {           //Add
        return websWrite(wp, T("\"%s\""),"");
  	  }else if (wifi_ssid_edit_idx > 0) {       //Edit
  	    pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0'";
  	  }	
  } else if (wifi_edit_idx == 2){
	  if (wifi_ssid_edit_idx == -1) {           //Add
        return websWrite(wp, T("\"%s\""),"");
  	  }else if (wifi_ssid_edit_idx >0) {        //Edit
        pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0.0'";		
  	  }
  }	else if (wifi_edit_idx == 3){
	  if (wifi_ssid_edit_idx == -1) {           //Add
        return websWrite(wp, T("\"%s\""),"");
  	  }else if (wifi_ssid_edit_idx >0) {        //Edit
        pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath0'";		
  	  }
  } else if (wifi_edit_idx == 4){
	  if (wifi_ssid_edit_idx == -1) {           //Add
        return websWrite(wp, T("\"%s\""),"");
  	  }else if (wifi_ssid_edit_idx >0) {        //Edit
        pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath1'";		
  	  }
  }	    
        
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
    printf("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
    return websWrite(wp, T("\"%s\""),pWiFiSSIDInfo);
  }

  memset(pWiFiSSIDInfo,'\0',sizeof(pWiFiSSIDInfo));
  if (nRows > 0) {
    Rows = wifi_ssid_edit_idx-1;
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "scheduleName", pScheduleName, getSysInfoOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "scheduleStatus", pScheduleStatus, getSysInfoOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "startHour", pStartHour, getSysInfoOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "startMinute", pStartMinute, getSysInfoOut);
  	DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "startMeridian", pStartMeridian, getSysInfoOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "stopHour", pStopHour, getSysInfoOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "stopMinute", pStopMinute, getSysInfoOut);
  	DB_TABLE_VALUE_GET(ppQueryRes, nCols, Rows, "stopMeridian", pStopMaridian, getSysInfoOut);	

    gstrcat(pWiFiSSIDInfo, pScheduleName);
    gstrcat(pWiFiSSIDInfo, pSep);
    gstrcat(pWiFiSSIDInfo, pScheduleStatus);
    gstrcat(pWiFiSSIDInfo, pSep);
    gstrcat(pWiFiSSIDInfo, pStartHour);
    gstrcat(pWiFiSSIDInfo, pSep);
    gstrcat(pWiFiSSIDInfo, pStartMinute);
    gstrcat(pWiFiSSIDInfo, pSep);
    gstrcat(pWiFiSSIDInfo, pStartMeridian);
    gstrcat(pWiFiSSIDInfo, pSep); 
    gstrcat(pWiFiSSIDInfo, pStopHour);
    gstrcat(pWiFiSSIDInfo, pSep);
    gstrcat(pWiFiSSIDInfo, pStopMinute);
    gstrcat(pWiFiSSIDInfo, pSep);
    gstrcat(pWiFiSSIDInfo, pStopMaridian);
    gstrcat(pWiFiSSIDInfo, pSep);
  }	

  getSysInfoOut:
  if (ppQueryRes)
  {
    sqlite3_free_table(ppQueryRes);
  }

    return websWrite(wp, T("\"%s\""),pWiFiSSIDInfo);
}

static int getWiFiNameIsSel(int eid, webs_t wp, int argc, char_t **argv)
{
  int nRows=0,nCols=0,index=0;
  char * pQueryStr = NULL;
  char** ppQueryRes = NULL;

  char* pScheduleName = NULL;
  
  if (wifi_edit_idx == 1){
  	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0'";
  } else if (wifi_edit_idx == 2){
	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0.0'";
  }else if (wifi_edit_idx == 3){
	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath0'";
  }else if (wifi_edit_idx == 4){
	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath1'";
  }

  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL) {
      printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
      return 0;
  }
  int ret = 0;
  char* pWiFiSSIDNameInfo = (char *)malloc(128);
  memset(pWiFiSSIDNameInfo,'\0', 128);
  if (nRows > 0) {
 	for (index = 0; index < nRows; index++){
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "scheduleName", pScheduleName, getWiFiSSIDOut);
	  gstrcat(pWiFiSSIDNameInfo,pScheduleName);
      gstrcat(pWiFiSSIDNameInfo,pSep);
    }
  }
	
  getWiFiSSIDOut:
    if (ppQueryRes)
    {
      sqlite3_free_table(ppQueryRes);
    }

	ret = websWrite(wp, T("\"%s\""), pWiFiSSIDNameInfo);
	free(pWiFiSSIDNameInfo);
    return ret;
}

static int getWiFiStatusIsTrue(int eid, webs_t wp, int argc, char_t **argv)
{
  int nRows=0,nCols=0,index=0;
  char * pQueryStr = NULL;
  char** ppQueryRes = NULL;

  char* pScheduleStatus = NULL;
  
  if (wifi_edit_idx == 1){
  	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0'";
  } else if (wifi_edit_idx == 2){
	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='wlan0.0'";
  } else if (wifi_edit_idx == 3){
	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath0'";
  } else if (wifi_edit_idx == 4){
	pQueryStr = "SELECT * FROM wifiSchedules WHERE associatedSSID='ath1'";
  }

  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL) {
      printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
      return 0;
  }
  int ret = 0;
  char* getWiFiStatusIsTrue = (char *)malloc(128);
  memset(getWiFiStatusIsTrue,'\0', 128);
  if (nRows > 0) {
 	for (index = 0; index < nRows; index++){
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "scheduleStatus", pScheduleStatus, getWiFiSSIDOut);
	  gstrcat(getWiFiStatusIsTrue,pScheduleStatus);
      gstrcat(getWiFiStatusIsTrue,pSep);
    }
  }
	
  getWiFiSSIDOut:
    if (ppQueryRes)
    {
      sqlite3_free_table(ppQueryRes);
    }

	ret = websWrite(wp, T("\"%s\""), getWiFiStatusIsTrue);
	free(getWiFiStatusIsTrue);
    return ret;
}

static void setWiFiSSIDEdit(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
	char_t * porgScheduleStatus = websGetVar(wp, T("org_scheduleStatus"), T(""));
	
	char_t * pscheduleStatus = websGetVar(wp, T("scheduleStatus"), T(""));
    char_t * pscheduleName = websGetVar(wp, T("scheduleName"), T(""));

	char_t * pstartHour = websGetVar(wp, T("startHour"), T(""));
    char_t * pstartMinute = websGetVar(wp, T("startMinute"), T(""));
    char_t * pstartMeridian = websGetVar(wp, T("startMeridian"), T(""));

	char_t * pstopHour = websGetVar(wp, T("stopHour"), T(""));
	char_t * pstopMinute = websGetVar(wp, T("stopMinute"), T(""));
    char_t * pstopMeridian = websGetVar(wp, T("stopMeridian"), T(""));

	char_t * pName = websGetVar(wp, T("WiFiName"), T(""));

	char pCWMPQry [1024] = {0};
	char pSETPQry [1024] = {0};
	int status = -1;

 char * pProfileName = NULL;
 //char * pWlanName = NULL;
 if (wifi_edit_idx == 1) {
  pProfileName = "wlan0";
  //pWlanName = "ap1";
 } else if (wifi_edit_idx == 2) {
  pProfileName = "wlan0.0";
  //pWlanName = "ap2";
 } else if (wifi_edit_idx == 3) {
  pProfileName = "ath0";
  //pWlanName = "ap3";
 } else if (wifi_edit_idx == 4) {
  pProfileName = "ath1";
  //pWlanName = "ap4";
 }

	// wifischedules table
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	memset (pSETPQry, 0x00, sizeof(pSETPQry));
	if (strcmp(websGetVar(wp, T("scheduleStatus"), T("")), "on") == 0){
		pscheduleStatus = "1";
	} else {
		pscheduleStatus = "0";
	}

    //Convert Start Hour to %2d
	if (gatoi(pstartHour)==1) pstartHour="01";
	if (gatoi(pstartHour)==2) pstartHour="02";
	if (gatoi(pstartHour)==3) pstartHour="03";
	if (gatoi(pstartHour)==4) pstartHour="04";
	if (gatoi(pstartHour)==5) pstartHour="05";
	if (gatoi(pstartHour)==6) pstartHour="06";
	if (gatoi(pstartHour)==7) pstartHour="07";
	if (gatoi(pstartHour)==8) pstartHour="08";
	if (gatoi(pstartHour)==9) pstartHour="09";

	//Convert Start Minute to %2d
	if (gatoi(pstartMinute)==0) pstartMinute="00";
	if (gatoi(pstartMinute)==1) pstartMinute="01";
	if (gatoi(pstartMinute)==2) pstartMinute="02";
	if (gatoi(pstartMinute)==3) pstartMinute="03";
	if (gatoi(pstartMinute)==4) pstartMinute="04";
	if (gatoi(pstartMinute)==5) pstartMinute="05";
	if (gatoi(pstartMinute)==6) pstartMinute="06";
	if (gatoi(pstartMinute)==7) pstartMinute="07";
	if (gatoi(pstartMinute)==8) pstartMinute="08";
	if (gatoi(pstartMinute)==9) pstartMinute="09";

	//Convert Stop Hour to %2d
	if (gatoi(pstopHour)==1) pstopHour="01";
	if (gatoi(pstopHour)==2) pstopHour="02";
	if (gatoi(pstopHour)==3) pstopHour="03";
	if (gatoi(pstopHour)==4) pstopHour="04";
	if (gatoi(pstopHour)==5) pstopHour="05";
	if (gatoi(pstopHour)==6) pstopHour="06";
	if (gatoi(pstopHour)==7) pstopHour="07";
	if (gatoi(pstopHour)==8) pstopHour="08";
	if (gatoi(pstopHour)==9) pstopHour="09";

	//Convert Stop Minute to %2d
	if (gatoi(pstopMinute)==0) pstopMinute="00";
	if (gatoi(pstopMinute)==1) pstopMinute="01";
	if (gatoi(pstopMinute)==2) pstopMinute="02";
	if (gatoi(pstopMinute)==3) pstopMinute="03";
	if (gatoi(pstopMinute)==4) pstopMinute="04";
	if (gatoi(pstopMinute)==5) pstopMinute="05";
	if (gatoi(pstopMinute)==6) pstopMinute="06";
	if (gatoi(pstopMinute)==7) pstopMinute="07";
	if (gatoi(pstopMinute)==8) pstopMinute="08";
	if (gatoi(pstopMinute)==9) pstopMinute="09";

	//Get current time.
    char *strHour, *strMinute;
    time_t tm1, tm2;
    struct tm *t1, *t2;
    strHour = malloc(2 * sizeof(char));
    strMinute = malloc(2 * sizeof(char));
    tm1 = time(NULL);
    t2 = gmtime(&tm1);
    tm2 = mktime(t2);
    t1 = localtime(&tm1);
	strftime(strHour, 2, "%H", t1);
	strftime(strMinute, 2, "%M", t1);

	//Check current time include schedule time when activate.
    int Hour = gatoi(strHour);
    int Minute = gatoi(strMinute);
    int total_current= Hour * 60 +Minute;
    int flag = 0;
	
    int startHour = gatoi(pstartHour);
    int stopHour = gatoi(pstopHour);
    int starMin = gatoi(pstartMinute);
    int stopMin = gatoi(pstopMinute);

	free(strHour);
	free(strMinute);

    if (!gstrcmp(pstartMeridian,"1")){
      if (startHour!=12)
        startHour = startHour + 12;
    }
    if (startHour==12)
      startHour=0;
      
    if (!gstrcmp(pstopMeridian,"1")){
      if (stopHour!=12)
        stopHour = stopHour + 12;
    }
    if (stopHour==12)
     stopHour=0;

    int total_start=startHour * 60 + starMin;
	int total_stop=stopHour* 60 + stopMin; 

	if((total_current<total_start)||(total_current>total_stop)){
      flag = 1;
    }else{
      flag = 0; 
    }

    //AP1, AP2, AP3, AP4
    if (wifi_edit_idx == 1 || wifi_edit_idx == 2 || wifi_edit_idx == 3 || wifi_edit_idx == 4) {
      if (pscheduleStatus == "1") {
        snprintf (pSETPQry, sizeof(pSETPQry),"update wifiSchedules set scheduleStatus='0' where associatedSSID='%s'", pProfileName);
        status =  jnrp_sql_dbEXE_Webs(pSETPQry);
        if (flag == 1) {
          //snprintf(pSETPQry, sizeof(pSETPQry),"update wifiap set wlanEnabled='0' where wlanName='%s'", pWlanName);
          //status = jnrp_sql_dbEXE_Webs(pSETPQry);
          snprintf (pSETPQry, sizeof(pSETPQry),"/usr/local/bin/wifiSchedule.sh disable %s", pProfileName);
          system(pSETPQry);
        }
      }
      if (wifi_ssid_edit_idx == -1) { //Add
        snprintf (pCWMPQry, sizeof(pCWMPQry),"insert into wifiSchedules values ('%s','%s','%s','%s','%s','%s','%s','%s','%s')"
        , pscheduleName, pscheduleStatus, pstartHour, pstartMinute, pstartMeridian, pstopHour, pstopMinute, pstopMeridian, pProfileName);
      }	else if (wifi_ssid_edit_idx > 0) { //Edit
        snprintf (pCWMPQry, sizeof(pCWMPQry),"update wifiSchedules set scheduleName='%s', scheduleStatus='%s', startHour='%s', startMinute='%s', startMeridian='%s', stopHour='%s', stopMinute='%s', stopMeridian='%s' where associatedSSID='%s' and scheduleName='%s'"
        , pscheduleName, pscheduleStatus, pstartHour, pstartMinute, pstartMeridian, pstopHour, pstopMinute, pstopMeridian, pProfileName, pName);
   	  }
   	  status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
    }

	if (status>=0)
	{
		//Active WiFi Schedule
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
		if (pscheduleStatus == "1") {
  			int nStartHour = gatoi(pstartHour);
			int nStopHour = gatoi(pstopHour);
			int nStartHourMeridian = gatoi(pstartMeridian);
			int nStopHourMeridian = gatoi(pstopMeridian);
			if (nStartHourMeridian == 1)
			{
				nStartHour += 12;
				if (nStartHour >= 24) {
					nStartHour = 0;
				}
			}
			if (nStopHourMeridian == 1)
			{
				nStopHour += 12;
				if (nStopHour >= 24) {
					nStopHour = 0;
				}
			}
			
   if (wifi_edit_idx == 1 || wifi_edit_idx == 2 || wifi_edit_idx == 3 || wifi_edit_idx == 4) {
    //delete
    snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from crontab where command='/usr/local/bin/wifiSchedule.sh enable %s'", pProfileName);
    status = jnrp_sql_dbEXE_Webs (pSQLCmd);
    snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from crontab where command='/usr/local/bin/wifiSchedule.sh disable %s'", pProfileName);
    status = jnrp_sql_dbEXE_Webs (pSQLCmd);
    //add
    snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into crontab values (2,0,%d,%d,-1,-1,-1,'/usr/local/bin/wifiSchedule.sh enable %s')", gatoi(pstartMinute), nStartHour, pProfileName);
    status = jnrp_sql_dbEXE_Webs (pSQLCmd);
    snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into crontab values (2,0,%d,%d,-1,-1,-1,'/usr/local/bin/wifiSchedule.sh disable %s')", gatoi(pstopMinute), nStopHour, pProfileName);
				status = jnrp_sql_dbEXE_Webs (pSQLCmd);
   }
		} else {
   int num_orgScheduleStatus = gatoi(porgScheduleStatus);
   if (wifi_edit_idx == 1 || wifi_edit_idx == 2 || wifi_edit_idx == 3 || wifi_edit_idx == 4) {
   	if (num_orgScheduleStatus == 1) {
   		//delete
     snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from crontab where command='/usr/local/bin/wifiSchedule.sh enable %s'", pProfileName);
					status = jnrp_sql_dbEXE_Webs (pSQLCmd);
					snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from crontab where command='/usr/local/bin/wifiSchedule.sh disable %s'", pProfileName);
					status = jnrp_sql_dbEXE_Webs (pSQLCmd);
   	}
   }
  }
		//export db to ascii
		system(EXPORT_CMD);
	}
 websRedirect(wp, T("wifi/wifi_setting_ssid.asp"));
}

static int getWifiACLSetting(int eid, webs_t wp, int argc, char_t **argv)
{
 char * pSSID = NULL;
	char * pACLPolicy = NULL;

 if (wifi_edit_idx == 1)
 {
   pSSID = jnrp_sql_getAttrByCondit_Webs("wifiProfile","ssid","profileName='wlan1'");
   pACLPolicy = jnrp_sql_getAttrByCondit_Webs("wifiACLconfig","ACLPolicy1","profileName='wifiacl'");
 } else if (wifi_edit_idx == 2){
   pSSID = jnrp_sql_getAttrByCondit_Webs("wifiProfile","ssid","profileName='wlan2'");
	  pACLPolicy = jnrp_sql_getAttrByCondit_Webs("wifiACLconfig","ACLPolicy2","profileName='wifiacl'");
 } else if (wifi_edit_idx == 3){
   pSSID = jnrp_sql_getAttrByCondit_Webs("wifiProfile","ssid","profileName='wlan3'");
	  pACLPolicy = jnrp_sql_getAttrByCondit_Webs("wifiACLconfig","ACLPolicy3","profileName='wifiacl'");
 } else if (wifi_edit_idx == 4){
   pSSID = jnrp_sql_getAttrByCondit_Webs("wifiProfile","ssid","profileName='wlan4'");
	  pACLPolicy = jnrp_sql_getAttrByCondit_Webs("wifiACLconfig","ACLPolicy4","profileName='wifiacl'");
 } else {
 	 return websWrite(wp, T("\"%s\""), "");
 }

 char pCmdInfo [256] = {0};
 memset (pCmdInfo, 0x00, sizeof(pCmdInfo));

 gstrcat(pCmdInfo, pSSID);
 gstrcat(pCmdInfo, pSep);
 gstrcat(pCmdInfo, pACLPolicy);
 gstrcat(pCmdInfo, pSep);
 return websWrite(wp, T("\"%s\""), pCmdInfo);
}

static void setWifiACLSetting(webs_t wp, char_t *path, char_t *query)
{
 char * pACLPolicy = websGetVar(wp, T("selACLPolicy"), T(""));
 int status = -1;

	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

 if (wifi_edit_idx == 1) {
	  snprintf (pSQLCmd, sizeof(pSQLCmd),"update wifiACLconfig set ACLPolicy1=%d where profileName='wifiacl'", gatoi(pACLPolicy));
	} else if (wifi_edit_idx == 2) {
	  snprintf (pSQLCmd, sizeof(pSQLCmd),"update wifiACLconfig set ACLPolicy2=%d where profileName='wifiacl'", gatoi(pACLPolicy));
	} else if (wifi_edit_idx == 3) {
	  snprintf (pSQLCmd, sizeof(pSQLCmd),"update wifiACLconfig set ACLPolicy3=%d where profileName='wifiacl'", gatoi(pACLPolicy));
	} else if (wifi_edit_idx == 4) {
	  snprintf (pSQLCmd, sizeof(pSQLCmd),"update wifiACLconfig set ACLPolicy4=%d where profileName='wifiacl'", gatoi(pACLPolicy));
	}

	status = jnrp_sql_dbEXE_Webs(pSQLCmd);

 // export db to ascii
 if (status>=0){
   system(EXPORT_CMD);
 }
 websRedirect(wp, T("wifi/wifi_setting_acl.asp"));
}

static int getWifiACLList(int eid, webs_t wp, int argc, char_t **argv)
{
	int index = 1;
	char* pMacAddr = NULL;
	int nRows = 0;

 char * pWiFiACLTable = NULL;
 if (wifi_edit_idx == 1) {
  pWiFiACLTable = "wifiACLTable1";
 } else if (wifi_edit_idx == 2) {
  pWiFiACLTable = "wifiACLTable2";
 } else if (wifi_edit_idx == 3) {
  pWiFiACLTable = "wifiACLTable3";
 } else if (wifi_edit_idx == 4) {
  pWiFiACLTable = "wifiACLTable4";
 }

 //ACL1, ACL2, ACL3, ACL4
 if (wifi_edit_idx == 1 || wifi_edit_idx == 2 || wifi_edit_idx == 3 || wifi_edit_idx == 4) {
 	nRows = jnrp_sql_getRowNum_Webs(pWiFiACLTable);
 	if (nRows > 0)
 	{
 		for (index=1; index<=nRows; index++)
 		{
			  pMacAddr = jnrp_sql_getAttr_Webs(pWiFiACLTable, "ACL_MAC", index);
     websWrite(wp, T("<tr>\n"));
			  websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"AclList.checkbox\" id=\"AclList_id_%d\"></td>\n"), index);
			  websWrite(wp, T("<td id=\"AclList_Mac_%d\" class=\"tdEven\">%s</td>\n"), index, pMacAddr);
			  websWrite(wp, T("</tr>\n")); 			
 		}
 	}
 }
	return 0;
}

static void setWifiACLList(webs_t wp, char_t *path, char_t *query)
{
	char * pAclListAction = websGetVar(wp, T("AclListAction"), T(""));
	char * pRowId = websGetVar(wp, T("AclListIdx"), T(""));
	char * pAclListCount = websGetVar(wp, T("AclListCount"), T("0"));
	int count=gatoi(pAclListCount);
	int index=0;

 //printf("pAclListAction = %s, pRowId=%s, pAclListCount=%s\n",pAclListAction,pRowId,pAclListCount);
	if (!gstrcmp(pAclListAction,"AclListAdd"))
	{
		websRedirect(wp, T("wifi/wifi_setting_acl_editor.asp"));
	}
	else if (!gstrcmp(pAclListAction,"AclListDel"))
	{
		char * pErrMsg = NULL;
		int status = -1;

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("wifi/wifi_setting_acl.asp"));
		}
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

  if (wifi_edit_idx == 1) {
		  snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from wifiACLTable1 where _ROWID_ in (%s)",pRowId);
		} else if (wifi_edit_idx == 2) {
		  snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from wifiACLTable2 where _ROWID_ in (%s)",pRowId);
		} else if (wifi_edit_idx == 3) {
		  snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from wifiACLTable3 where _ROWID_ in (%s)",pRowId);
		} else if (wifi_edit_idx == 4) {
		  snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from wifiACLTable4 where _ROWID_ in (%s)",pRowId);
  }
  //printf("pDeleteCmd=%s\n",pSQLCmd);

  if (wifi_edit_idx == 1 || wifi_edit_idx == 2 || wifi_edit_idx == 3 || wifi_edit_idx == 4) {
		  status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
		  status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);
		}

		if (pDB)
			sqlite3_close (pDB);

		if (status>=0)
		{
			sqlite3_free (pErrMsg);
 	 snprintf (pSQLCmd, sizeof(pSQLCmd),"update wifiACLconfig set ACLUpdate='20032'");
	  status = jnrp_sql_dbEXE_Webs(pSQLCmd);
   if (status>=0){
     system(EXPORT_CMD);
   }
		}
		websRedirect(wp, T("wifi/wifi_setting_acl.asp"));
	}
}

static int getWifiACLConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("\"%d\""), wifi_edit_idx);
}

int checkMacFilteringConfig(char * MacAddress)
{
	char ** ppQryResult = NULL;
 char pCheckQry [256] = {0};
 memset(pCheckQry, 0x00, sizeof(pCheckQry));

 if (wifi_edit_idx == 1) {
   snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from wifiACLTable1 where ACL_MAC='%s'", strupper(MacAddress));
 } else if (wifi_edit_idx == 2) {
   snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from wifiACLTable2 where ACL_MAC='%s'", strupper(MacAddress));
 } else if (wifi_edit_idx == 3) {
   snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from wifiACLTable3 where ACL_MAC='%s'", strupper(MacAddress));
 } else if (wifi_edit_idx == 4) {
   snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from wifiACLTable4 where ACL_MAC='%s'", strupper(MacAddress));
 }

 ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
 if (gatoi(ppQryResult[1]) > 0) //Same Name
 {
   return mac_filtering_same_name;
 }
 ppQryResult = NULL;

 return mac_filtering_none;
}

static void setWifiACLConfig(webs_t wp, char_t *path, char_t *query)
{
 char * pMacAddress = websGetVar(wp, T("aclMacAddress"), T(""));
 int status = -1;

	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	// check the same MAC Address
 P02003_mac_filtering_status = checkMacFilteringConfig(pMacAddress);
 if (P02003_mac_filtering_status == mac_filtering_none)
 {
 } else {
 	 websRedirect(wp, T("wifi/wifi_setting_acl.asp"));
 	 return;
 }

 if (wifi_edit_idx == 1) {
	 //ACL1 add
  snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into wifiACLTable1 values ('%s','')", strupper(pMacAddress));
	 status = jnrp_sql_dbEXE_Webs (pSQLCmd);
 } else if (wifi_edit_idx == 2) {
	 //ACL2 add
  snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into wifiACLTable2 values ('%s','')", strupper(pMacAddress));
	 status = jnrp_sql_dbEXE_Webs (pSQLCmd);
 } else if (wifi_edit_idx == 3) {
	 //ACL3 add
  snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into wifiACLTable3 values ('%s','')", strupper(pMacAddress));
	 status = jnrp_sql_dbEXE_Webs (pSQLCmd);
 } else if (wifi_edit_idx == 4) {
	 //ACL4 add
  snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into wifiACLTable4 values ('%s','')", strupper(pMacAddress));
	 status = jnrp_sql_dbEXE_Webs (pSQLCmd);
 }

 // export db to ascii
 if (status>=0){
 	 snprintf (pSQLCmd, sizeof(pSQLCmd),"update wifiACLconfig set ACLUpdate='20031'");
	  status = jnrp_sql_dbEXE_Webs(pSQLCmd);
   if (status>=0){
     system(EXPORT_CMD);
   }
 }
 websRedirect(wp, T("wifi/wifi_setting_acl.asp"));
}

static int getWifiACLListCount(int eid, webs_t wp, int argc, char_t **argv)
{
	int nRows = 0;

 if (wifi_edit_idx == 1) {
	 nRows = jnrp_sql_getRowNum_Webs ("wifiACLTable1");
	} else if (wifi_edit_idx == 2) {
		nRows = jnrp_sql_getRowNum_Webs ("wifiACLTable2");
	} else if (wifi_edit_idx == 3) {
		nRows = jnrp_sql_getRowNum_Webs ("wifiACLTable3");
	} else if (wifi_edit_idx == 4) {
		nRows = jnrp_sql_getRowNum_Webs ("wifiACLTable4");
	}
	return websWrite(wp, T("\"%d\""), nRows);
}

static int getMacFilteringErrorStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int error_msg = P02003_mac_filtering_status;
	P02003_mac_filtering_status = mac_filtering_none;
 return websWrite(wp, T("\"%d\""), error_msg);
}
#endif
