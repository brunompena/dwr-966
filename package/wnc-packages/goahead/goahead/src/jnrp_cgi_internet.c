#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

#define GETLTEFROMFILE
#if defined(GETLTEFROMFILE)
 #define APNPROFILE "/tmp/getAPNProfile"
 #define APNSTATUS "/tmp/getIPStatus"
 #define PINCONFIG "/tmp/getPinConfig"
 #define LTERADIO "/tmp/getLTERadio"
#endif

void initInternetWeb();

static int getAPNProfiles (int eid, webs_t wp, int argc, char_t **argv);
static int getAPNStatus (int eid, webs_t wp, int argc, char_t **argv);
static void setWan1Connect (webs_t wp, char_t *path, char_t *query);
static void setWan2Connect (webs_t wp, char_t *path, char_t *query);
#ifdef __P01003_UNE
static void setWan3Connect (webs_t wp, char_t *path, char_t *query);
#endif
static int getLTERadio (int eid, webs_t wp, int argc, char_t **argv);
static void setLTERadio (webs_t wp, char_t *path, char_t *query);
static int getWanStatistics (int eid, webs_t wp, int argc, char_t **argv);
static int getRadioInfo (int eid, webs_t wp, int argc, char_t **argv);
static int getPinConfig (int eid, webs_t wp, int argc, char_t **argv);
static void setPinConfig (webs_t wp, char_t *path, char_t *query);
static void setChangePin (webs_t wp, char_t *path, char_t *query);
static int getCAInfo(int eid, webs_t wp, int argc, char_t **argv);
//Network Provider Selection
static int getNetworkSelMode (int eid, webs_t wp, int argc, char_t **argv);
static void setNetworkSelMode (webs_t wp, char_t *path, char_t *query);
static int scanNetworkProvider (int eid, webs_t wp, int argc, char_t **argv);
static int getRadioMode(int eid,webs_t wp,int argc,char_t * * argv);
static int getRadioFrequency(int eid,webs_t wp,int argc,char_t * * argv);
static int showNetworkProvider (int eid, webs_t wp, int argc, char_t **argv);
static void setNetworkProvider (webs_t wp, char_t *path, char_t *query);

//PIN verify by Home page
static void setVerifyPIN (webs_t wp, char_t *path, char_t *query);
static void setPUK (webs_t wp, char_t *path, char_t *query);

#if defined(__P02003_Dora)
int wan_profile_list_idx=-1;
static int getWanProfileListData1(int eid,webs_t wp,int argc,char_t * * argv);
static int getWanProfileListData2(int eid,webs_t wp,int argc,char_t * * argv);
static int getWanProfileListConnectIndex(int eid,webs_t wp,int argc,char_t * * argv);
static void setWanProfile(webs_t wp, char_t *path, char_t *query);
static int getWanProfileConfig(int eid,webs_t wp,int argc,char_t * * argv);
static void setWanProfileConfig(webs_t wp, char_t *path, char_t *query);
static int getAntennaMode(int eid, webs_t wp, int argc, char_t **argv);
static int getInternetPriority(int eid, webs_t wp, int argc, char_t **argv);
static int getEthernetWANSetting(int eid, webs_t wp, int argc, char_t **argv);
static void setInternetPriority(webs_t wp, char_t *path, char_t *query);
static void setEthernetWanSetting(webs_t wp, char_t *path, char_t *query);
static int getNetworkType(int eid, webs_t wp, int argc, char_t **argv);
static int getRoamingPDPType(int eid, webs_t wp, int argc, char_t **argv);
#endif

void initInternetWeb()
{
 websAspDefine(T("getAPNProfiles"), getAPNProfiles);
 websAspDefine(T("getAPNStatus"), getAPNStatus);
 websFormDefine(T("setWan1Connect"), setWan1Connect);
 websFormDefine(T("setWan2Connect"), setWan2Connect);
#ifdef __P01003_UNE
 websFormDefine(T("setWan3Connect"), setWan3Connect);
#endif
 websAspDefine(T("getLTERadio"), getLTERadio);
 websFormDefine(T("setLTERadio"), setLTERadio);
 websAspDefine(T("getWanStatistics"), getWanStatistics);
 websAspDefine(T("getRadioInfo"), getRadioInfo);
 websAspDefine(T("getPinConfig"), getPinConfig);
 websFormDefine(T("setPinConfig"), setPinConfig);
 websFormDefine(T("setChangePin"), setChangePin);
 websAspDefine(T("getCAInfo"), getCAInfo);
 //Network Provider Selection --start
 websAspDefine(T("getNetworkSelMode"), getNetworkSelMode);
 websFormDefine(T("setNetworkSelMode"), setNetworkSelMode);
 websAspDefine(T("scanNetworkProvider"), scanNetworkProvider);
 websAspDefine(T("getRadioFrequency"), getRadioFrequency);
 websAspDefine(T("getRadioMode"), getRadioMode);
 	
 websAspDefine(T("showNetworkProvider"), showNetworkProvider);
 websFormDefine(T("setNetworkProvider"), setNetworkProvider);
 //Network Provider Selection --end
 //PIN verify by Home page
 websFormDefine(T("setVerifyPIN"), setVerifyPIN);
 websFormDefine(T("setPUK"), setPUK);

#if defined(__P02003_Dora)
 websAspDefine(T("getWanProfileListData1"), getWanProfileListData1);
 websAspDefine(T("getWanProfileListData2"), getWanProfileListData2);
 websAspDefine(T("getWanProfileListConnectIndex"), getWanProfileListConnectIndex);
 websFormDefine(T("setWanProfile"), setWanProfile);
 websAspDefine(T("getWanProfileConfig"), getWanProfileConfig);
 websFormDefine(T("setWanProfileConfig"), setWanProfileConfig);
 websAspDefine(T("getAntennaMode"), getAntennaMode); 
 websAspDefine(T("getInternetPriority"), getInternetPriority);
 websAspDefine(T("getEthernetWANSetting"), getEthernetWANSetting);
 websFormDefine(T("setInternetPriority"), setInternetPriority);
 websFormDefine(T("setEthernetWanSetting"), setEthernetWanSetting);
 websAspDefine(T("getNetworkType"), getNetworkType);
 websAspDefine(T("getRoamingPDPType"), getRoamingPDPType);
#endif

 return ;
}

static int getAPNProfiles (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GETLTEFROMFILE

 char pAPNProfile[512];

 memset(pAPNProfile, '\0', sizeof(pAPNProfile));
 system("lteStatus.sh getAPNProfiles");

 FILE *fp = fopen(APNPROFILE, "r");
 char line[128];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   gstrcat(pAPNProfile,line);
   gstrcat(pAPNProfile,pSep);
  }
  fclose(fp);
 } else {
#ifdef __P01003_UNE
 	return websWrite(wp, T("\"%s\""), "#####");
#else
 	return websWrite(wp, T("\"%s\""), "###");
#endif

 }

 //remove(APNPROFILE);

 return websWrite(wp, T("\"%s\""), pAPNProfile);

#else

 char pAPNInfo[256];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select name, APNName from lteNwProfile";

 int index = 0;
 char* pProfileName = NULL;
 char* pAPNName = NULL;

 system ("lte_get -i 0 1 --timeout 5000 --update-db > /dev/NULL");
 system ("lte_get -i 0 2 --timeout 5000 --update-db > /dev/NULL");
#ifdef __P01003_UNE
 system ("lte_get -i 0 3 --timeout 5000 --update-db > /dev/NULL");
#endif

 sleep(1);

 memset(pAPNInfo,'\0',sizeof(pAPNInfo));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pAPNInfo);
 }

 if (nRows > 0) {
  for (index = 0; index < nRows; index++) {
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "name", pProfileName, getAPNOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "APNName", pAPNName, getAPNOut);

   gstrcat(pAPNInfo,pProfileName);
   gstrcat(pAPNInfo,pSep);
   gstrcat(pAPNInfo,pAPNName);
   gstrcat(pAPNInfo,pSep);
  }
 }

getAPNOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return websWrite(wp, T("\"%s\""),pAPNInfo);

#endif
}

static int getAPNStatus (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GETLTEFROMFILE

 char pAPNStatus[128];

 memset(pAPNStatus, '\0', sizeof(pAPNStatus));
 system("lteStatus.sh getAPNStatus");

 FILE *fp = fopen(APNSTATUS, "r");
 char line[64];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   gstrcat(pAPNStatus,line);
   gstrcat(pAPNStatus,pSep);
  }
  fclose(fp);
 } else {
#ifdef __P01003_UNE
 	return websWrite(wp, T("\"%s\""), "##");
#else
 	return websWrite(wp, T("\"%s\""), "#");
#endif
 }

 //remove(APNSTATUS);

 return websWrite(wp, T("\"%s\""), pAPNStatus);

#else

 char pIPStatus[128];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select status from lteIPStatus where qid=1";

 int index = 0;
 char* pStatus = NULL;


 memset(pIPStatus,'\0',sizeof(pIPStatus));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pIPStatus);
 }

 if (nRows > 0) {
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "status", pStatus, getAPNStatusOut);

   gstrcat(pIPStatus,pStatus);
   gstrcat(pIPStatus,pSep);
 }

 char * pQueryStr2 = "select status from lteIPStatus where qid=2";

#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr2, &nCols, &nRows)) == NULL)
#else
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr2, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr2);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pIPStatus);
 }

 if (nRows > 0) {
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "status", pStatus, getAPNStatusOut);

   gstrcat(pIPStatus,pStatus);
   gstrcat(pIPStatus,pSep);
 }

getAPNStatusOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return websWrite(wp, T("\"%s\""),pIPStatus);

#endif
}

static void setWan1Connect (webs_t wp, char_t *path, char_t *query)
{
 char_t * pAPN1Bt = websGetVar(wp, T("apn1_button"), T(""));
 char_t * pAPN1Name = websGetVar(wp, T("apn1_name"), T(""));

 char cmdbuf [256] = {0};
 memset (cmdbuf, 0x00, sizeof(cmdbuf));

 if (gstrcmp(pAPN1Bt,"Connect") == 0) {
 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -M 0 1 -a '%s' > /dev/NULL", pAPN1Name);
 	system (cmdbuf);

  system ("lte_get -i 0 1 --timeout 5000 --update-db > /dev/NULL");
  sleep(1);

/* mantis id: 896
 * usb0 cannot get ip after disconnect and connect.
 * tempory fix: down usb0 after disconnect and up usb0 before connect.
 */
	//system("/sbin/ifconfig usb0 up");
 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -q 0 -c -1 > /dev/NULL");
 	system(cmdbuf);
 } else if (gstrcmp(pAPN1Bt,"Disconnect") == 0) {
 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -d -q 0 > /dev/NULL");
 	system(cmdbuf);
 	system("/sbin/ifconfig usb0 0.0.0.0");
 }

#ifdef __USED_WEBS_DB__
 char * pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
#else
 char * pQryResult = jnrp_sql_getAttrByCondit(pDBFile,"lteIPStatus","status","qid=1");
#endif
 int index = 0;
 while(((gstrcmp(pQryResult,"connecting") == 0) || (gstrcmp(pQryResult,"disconnecting") == 0)) && index < 30)
 {
 	index++;
#ifdef __USED_WEBS_DB__
  pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
#else
  pQryResult = jnrp_sql_getAttrByCondit(pDBFile,"lteIPStatus","status","qid=1");
#endif
 }

 // export db to ascii
 system(EXPORT_CMD);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
 websRedirect(wp, T("internet/internet_profile.asp"));
#else
 websRedirect(wp, T("internet/internet_general.asp"));
#endif

}

static void setWan2Connect (webs_t wp, char_t *path, char_t *query)
{
 char_t * pAPN2Bt = websGetVar(wp, T("apn2_button"), T(""));
 char_t * pAPN2Name = websGetVar(wp, T("apn2_name"), T(""));

 char cmdbuf [256] = {0};
 memset (cmdbuf, 0x00, sizeof(cmdbuf));

 if (gstrcmp(pAPN2Bt,"Connect") == 0) {
 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -M 0 2 -a '%s' > /dev/NULL", pAPN2Name);
 	system (cmdbuf);

  system ("lte_get -i 0 2 --timeout 5000 --update-db > /dev/NULL");
  sleep(1);

 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -q 1 -c -2 > /dev/NULL");
 	system (cmdbuf);

 } else if (gstrcmp(pAPN2Bt,"Disconnect") == 0) {
 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -d -q 1 > /dev/NULL");
 	system (cmdbuf);
 }


#ifdef __USED_WEBS_DB__
 char * pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=2");
#else
 char * pQryResult = jnrp_sql_getAttrByCondit(pDBFile,"lteIPStatus","status","qid=2");
#endif
 int index = 0;

 while(((gstrcmp(pQryResult,"connecting") == 0) || (gstrcmp(pQryResult,"disconnecting") == 0)) && index < 30)
 {
 	index++;
#ifdef __USED_WEBS_DB__
  pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=2");
#else
  pQryResult = jnrp_sql_getAttrByCondit(pDBFile,"lteIPStatus","status","qid=2");
#endif
 }

 // export db to ascii
 system(EXPORT_CMD);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
 websRedirect(wp, T("internet/internet_profile.asp"));
#else
 websRedirect(wp, T("internet/internet_general.asp"));
#endif

}

#ifdef __P01003_UNE
static void setWan3Connect (webs_t wp, char_t *path, char_t *query)
{
 char_t * pAPN3Bt = websGetVar(wp, T("apn3_button"), T(""));
 char_t * pAPN3Name = websGetVar(wp, T("apn3_name"), T(""));

 char cmdbuf [128] = {0};
 memset (cmdbuf, 0x00, sizeof(cmdbuf));

 if (gstrcmp(pAPN3Bt,"Connect") == 0) {
 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -M 0 3 -a '%s' > /dev/NULL", pAPN3Name);
 	system (cmdbuf);
  	system ("lte_get -i 0 3 --timeout 5000 --update-db > /dev/NULL");
  	sleep(1);

 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -q 2 -c -3 > /dev/NULL");
 	system (cmdbuf);

 } else if (gstrcmp(pAPN3Bt,"Disconnect") == 0) {
 	snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -d -q 2 > /dev/NULL");
 	system (cmdbuf);
 }

 //system ("lte_get --ip-status -q 2 --update-db > /dev/NULL");
 //sleep(1);

#ifdef __USED_WEBS_DB__
 char * pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=3");
#else
 char * pQryResult = jnrp_sql_getAttrByCondit(pDBFile,"lteIPStatus","status","qid=3");
#endif
 int index = 0;

 while(((gstrcmp(pQryResult,"connecting") == 0) || (gstrcmp(pQryResult,"disconnecting") == 0)) && index < 30)
 {
 	index++;
  	//system ("lte_get --ip-status -q 2 --update-db > /dev/NULL");
  	//sleep(1);
#ifdef __USED_WEBS_DB__
  	pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=3");
#else
  	pQryResult = jnrp_sql_getAttrByCondit(pDBFile,"lteIPStatus","status","qid=3");
#endif
 }

 // export db to ascii
 system(EXPORT_CMD);

 websRedirect(wp, T("internet/internet_general.asp"));
}
#endif

static int getLTERadio (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GETLTEFROMFILE

 char pRadioMode[128];

 memset(pRadioMode, '\0', sizeof(pRadioMode));
 system("lteStatus.sh getLTERadio");

 FILE *fp = fopen(LTERADIO, "r");
 char line[64];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
 	int i = 0;
  while(fgets(line, sizeof(line), fp) != NULL)
  {
  	i = i + 1;
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   if (i == 1) {
    if (gstrcmp(line,"0x001C") == 0) {
 	    gstrcat(pRadioMode, "auto");
    } else if (gstrcmp(line,"0x0010") == 0) {
 	    gstrcat(pRadioMode, "4G");
    } else if (gstrcmp(line,"0x0008") == 0) {
 	    gstrcat(pRadioMode, "3G");
    } else if (gstrcmp(line,"0x0004") == 0) {
 	    gstrcat(pRadioMode, "2G");
    } else {
      gstrcat(pRadioMode, "auto");
    }
    gstrcat(pRadioMode,pSep);
   }
   if (i == 2) {
    if (gstrcmp(line,"0x00000000000800C4") == 0 || gstrcmp(line,"0x800C4") == 0) {
      gstrcat(pRadioMode, "auto");
    } else if (gstrcmp(line,"0x0000000000000004") == 0 || gstrcmp(line,"0x4") == 0) {
      gstrcat(pRadioMode, "1800");
    } else if (gstrcmp(line,"0x0000000000000040") == 0 || gstrcmp(line,"0x40") == 0) {
      gstrcat(pRadioMode, "2600");
    } else if (gstrcmp(line,"0x0000000000080000") == 0 || gstrcmp(line,"0x80000") == 0) {
      gstrcat(pRadioMode, "800");
    } else if (gstrcmp(line,"0x0000000000000080") == 0 || gstrcmp(line,"0x80") == 0) {
      gstrcat(pRadioMode, "900");
    } else {
      gstrcat(pRadioMode, "auto");
    }
    gstrcat(pRadioMode,pSep);
   }
  }
  fclose(fp);
 } else {
 	return websWrite(wp, T("\"%s\""), "#");
 }
 
#ifdef __P02002_Dora
	gstrcat(pRadioMode, "1");
	gstrcat(pRadioMode,pSep);
#endif
  system("lte_get --allow-roaming --timeout 5000 > /tmp/getRoaming");
  FILE *fp_roaming = fopen("/tmp/getRoaming","r");
  char line_roaming[64];
  memset(line_roaming, '\0', sizeof(line_roaming));
  if(fp_roaming)
  {
    while(fgets(line_roaming, sizeof(line_roaming), fp_roaming) != NULL)
    {
		if(line_roaming[strlen(line_roaming)-1] == '\n')
		   line_roaming[strlen(line_roaming)-1] = '\0';
		gstrcat(pRadioMode, line_roaming);
		gstrcat(pRadioMode, pSep); 
    }
    fclose(fp_roaming);
  }



 //remove(LTERADIO);

 return websWrite(wp, T("\"%s\""), pRadioMode);

#else

 char pRadioMode[128];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from lteRadio";

 char* pMode = NULL;
 char* pBand = NULL;

 //system ("lte_get -S --timeout 5000 --update-db > /dev/NULL");
 //sleep(1);

 memset(pRadioMode,'\0',sizeof(pRadioMode));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pRadioMode);
 }

 if (nRows > 0) {
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "lte_mode", pMode, getLTEModeOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "lte_band", pBand, getLTEModeOut);

  gstrcat(pRadioMode,pMode);
  gstrcat(pRadioMode,pSep);
  gstrcat(pRadioMode,pBand);
 }

getLTEModeOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }
 
#ifdef __P02002_Dora
	 gstrcat(pRadioMode, "1");
	 gstrcat(pRadioMode,pSep);
#endif

 return websWrite(wp, T("\"%s\""),pRadioMode);

#endif
}

#if defined(__P02003_Dora)
static void setLTERadio (webs_t wp, char_t *path, char_t *query)
{
 char * porg_radio_mode = websGetVar(wp, T("org_radio_mode"), T(""));
 char * porg_radio_frequency = websGetVar(wp, T("org_radio_frequency"), T(""));
 char * porg_antenna = websGetVar(wp, T("org_antenna"), T(""));
 char * pLTEMode = websGetVar(wp, T("lte_radioMode"), T(""));
 char * pLTEBand = websGetVar(wp, T("lte_radioFrequency"), T(""));
 char * pAntennaMode = websGetVar(wp, T("antenna_mode"), T(""));
 char_t * pRoaming = websGetVar(wp, T("lte_roaming"), T("off"));

 int diff_flag = 0;
 int index = 0;
 int connect_flag = 0;
 char * pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");

 if (gstrcmp(porg_radio_mode, pLTEMode) != 0) {
   diff_flag = 1;
   if (gstrcmp(porg_radio_mode, "3G") == 0 || gstrcmp(porg_radio_mode, "2G") == 0) {
     if (gstrcmp(pLTEMode, "auto") == 0 && gstrcmp(porg_radio_frequency, pLTEBand) == 0) {
       if (gstrcmp(pQryResult, "connected") == 0) {
         connect_flag = 1;
         system ("lte_set -d -q 0 > /dev/NULL");
         index = 0;
         pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
         while(gstrcmp(pQryResult, "connected") == 0 && index < 20)
         {
           index++;
           sleep(1);
           pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
         }
       }
     }
   }
   if (gstrcmp(pLTEMode,"auto") == 0) {
     system ("lte_set -S 0x1C > /dev/NULL");
   } else if (gstrcmp(pLTEMode,"4G") == 0) {
     system ("lte_set -S 0x10 > /dev/NULL");
   } else if (gstrcmp(pLTEMode,"3G") == 0) {
     system ("lte_set -S 0x08 > /dev/NULL");
   } else if (gstrcmp(pLTEMode,"2G") == 0) {
     system ("lte_set -S 0x04 > /dev/NULL");
   }
   if (connect_flag == 1) {
     index = 0;
     pQryResult = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","radioInterference","qid=1");
     while(gstrcmp(pQryResult, "LTE") != 0 && index < 30)
     {
       index++;
       sleep(1);
       pQryResult = jnrp_sql_getAttrByCondit_Webs("lteNwStatus","radioInterference","qid=1");
     }
     system ("lte_set -q 0 -c -1 > /dev/NULL");
   }
 }

 if (gstrcmp(porg_radio_frequency, pLTEBand) != 0 && gstrlen(pLTEBand)) {
  diff_flag = 1;
  if (gstrcmp(pLTEBand,"auto") == 0) {
   system ("lte_set --LTE-band 0x800C4 > /dev/NULL");
  } else if (gstrcmp(pLTEBand,"1800") == 0) {
  	system ("lte_set --LTE-band 0x04 > /dev/NULL");
  } else if (gstrcmp(pLTEBand,"2600") == 0) {
  	system ("lte_set --LTE-band 0x40 > /dev/NULL");
  } else if (gstrcmp(pLTEBand,"800") == 0) {
 	 system ("lte_set --LTE-band 0x80000 > /dev/NULL");
  } else if (gstrcmp(pLTEBand,"900") == 0) {
 	 system ("lte_set --LTE-band 0x80 > /dev/NULL");
  }
 }

 if (gstrcmp(porg_antenna, pAntennaMode) != 0) {
   if (gstrcmp(pAntennaMode,"0") == 0) {
     system("/mnt/data/ant_sw_int");
   } else if (gstrcmp(pAntennaMode,"1") == 0) {
     system("/mnt/data/ant_sw_ext");
   }
 }
 if(gstrcmp(pRoaming, "on")==0)
 {
 	system("lte_set --allow-roaming 1 > /dev/NULL");
 }
 else
 {
 	system("lte_set --allow-roaming 0 > /dev/NULL");
 }

 if (diff_flag == 1)
 {
 	system ("lte_get -S --timeout 5000 --update-db > /dev/NULL");
 	sleep(2);

  //export db to ascii
  system(EXPORT_CMD);
 }else
  system("sync");//to sync roaming setting

 websRedirect(wp, T("internet/internet_radio.asp"));
 return;
}
#else
static void setLTERadio (webs_t wp, char_t *path, char_t *query)
{
 char_t * pLTEMode = websGetVar(wp, T("lte_radioMode"), T(""));
 char_t * pLTEBand = websGetVar(wp, T("lte_radioFrequency"), T(""));

 if (gstrlen(pLTEMode)) {
   if (gstrcmp(pLTEMode,"auto") == 0) {
 	 system ("lte_set -S 0x1C > /dev/NULL");
   } else if (gstrcmp(pLTEMode,"4G") == 0) {
 	 system ("lte_set -S 0x10 > /dev/NULL");
   } else if (gstrcmp(pLTEMode,"3G") == 0) {
 	 system ("lte_set -S 0x08 > /dev/NULL");
   } else if (gstrcmp(pLTEMode,"2G") == 0) {
   system ("lte_set -S 0x04 > /dev/NULL");
   }
 }

 if (gstrlen(pLTEBand)) {
   if (gstrcmp(pLTEBand,"auto") == 0) {
#ifdef __P02002_Dora
 	 system ("lte_set --LTE-band 0xFFFFFF > /dev/NULL");
#else
	system ("lte_set --LTE-band 0x800C4 > /dev/NULL");
#endif
   } else if (gstrcmp(pLTEBand,"1800") == 0) {
  	system ("lte_set --LTE-band 0x04 > /dev/NULL");
   } else if (gstrcmp(pLTEBand,"2600") == 0) {
  	system ("lte_set --LTE-band 0x40 > /dev/NULL");
   } else if (gstrcmp(pLTEBand,"800") == 0) {
 	 system ("lte_set --LTE-band 0x80000 > /dev/NULL");
   } else if (gstrcmp(pLTEBand,"900") == 0) {
 	 system ("lte_set --LTE-band 0x80 > /dev/NULL");
   }
 }

 if (gstrlen(pLTEMode) || gstrlen(pLTEBand))
 {
 	system ("lte_get -S --timeout 5000 --update-db > /dev/NULL");
 	sleep(2);

  //export db to ascii
  system(EXPORT_CMD);
 }

#if defined(__P02001_Dora) || defined(__P02002_Dora)
 websRedirect(wp, T("internet/internet_radio.asp"));
#else
 websRedirect(wp, T("internet/internet_general.asp"));
#endif
}
#endif

static int getWanStatistics (int eid, webs_t wp, int argc, char_t **argv)
{
 char pWanInfo[512];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from lteNwStatus";

 int index = 0;
 char* pTxPackets = NULL;
 char* pTxBytes = NULL;
 char* pTxErrors = NULL;
 char* pTxOverflows = NULL;
 char* pRxPackets = NULL;
 char* pRxBytes = NULL;
 char* pRxErrors = NULL;
 char* pRxOverflows = NULL;
 char* pStatus  = NULL;
 char* pSignal  = NULL;
 char* pNetworkName  = NULL;
 char* pMCC  = NULL;
 char* pMNC  = NULL;
 char* pCellID  = NULL;
 char* pRSSI	= NULL;		//WNC_SY_20130906:for mantis 78909
 char* pRSCP	= NULL;		
 char* pRSRP	= NULL;		
 char* pSINR	= NULL;		
 char* pRADIO	= NULL;

 system ("lte_get --pkt-statistics -q 0 --timeout 5000 --update-db > /dev/NULL");
 system ("lte_get -n -q 0 --timeout 5000 --update-db > /dev/NULL");
 system ("lte_get --signal-strength -q 0 --timeout 5000 --update-db > /dev/NULL");

#if defined ( __P02003_Dora)
#else
 system ("lte_get --pkt-statistics -q 1 --timeout 5000 --update-db > /dev/NULL");
#ifdef __P01003_UNE
 system ("lte_get --pkt-statistics -q 2 --timeout 5000 --update-db > /dev/NULL");
#endif
#endif

 sleep(1);

 memset(pWanInfo,'\0',sizeof(pWanInfo));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pWanInfo);
 }

 if (nRows > 0) {
#ifdef __P01003_UNE
  nRows = 3; //temp
#elif defined (__P02003_Dora)
  nRows = 1; //temp
#else
  nRows = 2; //temp
#endif

  for (index = 0; index < nRows; index++) {
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "tx_packets_ok", pTxPackets, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "tx_bytes_ok", pTxBytes, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "tx_errors", pTxErrors, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "tx_overflows", pTxOverflows, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rx_packets_ok", pRxPackets, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rx_bytes_ok", pRxBytes, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rx_errors", pRxErrors, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rx_overflows", pRxOverflows, getStatisticsOut);

   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "registrationState", pStatus, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "signalStrength", pSignal, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "networkName", pNetworkName, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "MCC", pMCC, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "MNC", pMNC, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "CellID", pCellID, getStatisticsOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rssi", pRSSI, getStatisticsOut);	//WNC_SY_20130906:for mantis 78909
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rscp", pRSCP, getStatisticsOut);	
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rsrp", pRSRP, getStatisticsOut);	
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "sinr", pSINR, getStatisticsOut);	
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "radioInterference", pRADIO, getStatisticsOut);

   gstrcat(pWanInfo,pTxPackets);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pTxBytes);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pTxErrors);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pTxOverflows);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRxPackets);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRxBytes);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRxErrors);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRxOverflows);
   gstrcat(pWanInfo,pSep);

   gstrcat(pWanInfo,pStatus);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pSignal);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pNetworkName);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pMCC);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pMNC);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pCellID);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRSSI);	//WNC_SY_20130906:for mantis 78909
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRADIO);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRSCP);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pRSRP);
   gstrcat(pWanInfo,pSep);
   gstrcat(pWanInfo,pSINR);
   gstrcat(pWanInfo,pSep);
  }
 }

getStatisticsOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

  //export db to ascii
  //system(EXPORT_CMD);

 return websWrite(wp, T("\"%s\""),pWanInfo);
}

static int getRadioInfo (int eid, webs_t wp, int argc, char_t **argv)
{
  char pRadioInfo[256];

  int nRows=0,nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "select * from lteNwStatus";

  char* pStatus = NULL;
  char* pRSRQ = NULL;
  char* pRSRP = NULL;
  char* pSNR = NULL;

  //system ("lte_get --signal-strength -q 0 --timeout 5000 --update-db > /dev/NULL");
  system ("lte_get --sig-info -q 0 --timeout 5000 --update-db > /dev/NULL");
  sleep(1);

  memset(pRadioInfo,'\0',sizeof(pRadioInfo));
#ifdef __USED_WEBS_DB__
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
  sqlite3 * pWebDB = NULL;
  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
    //sqlite3_free(pQueryStr);
    return websWrite(wp, T("\"%s\""),pRadioInfo);
  }
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "radioInterference", pStatus, getRadioInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "rsrq", pRSRQ, getRadioInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "rsrp", pRSRP, getRadioInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "snr", pSNR, getRadioInfoOut);

  gstrcat(pRadioInfo,pStatus);
  gstrcat(pRadioInfo,pSep);
  gstrcat(pRadioInfo,pRSRQ);
  gstrcat(pRadioInfo,pSep);
  gstrcat(pRadioInfo,pRSRP);
  gstrcat(pRadioInfo,pSep);
  gstrcat(pRadioInfo,pSNR);

  getRadioInfoOut:
    if (ppQueryRes)
    {
      sqlite3_free_table(ppQueryRes);
    }

    //export db to ascii
    //system(EXPORT_CMD);

  return websWrite(wp, T("\"%s\""),pRadioInfo);
}

static int getCAInfo (int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	int count = sizeof (web_Enum_BandValueList) / sizeof (CGI_EBAND_P);
	
	char pCAInfo[512];
	memset(pCAInfo,'\0',sizeof(pCAInfo));

	char *pCA = (char *)malloc(128);
	char *pBand1 = (char *)malloc(128);
	char *pBand2 = (char *)malloc(128);

	gstrcpy(pBand1,"-1");
	gstrcpy(pBand2,"-1");

	FILE *fileCA, *fileCAstate, *fileCABand1, *fileCABand2;
	char ca, ca_status;
	char *ca_band1, *ca_band2;
	char line[2014], line2[2048];

	system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep pcell_pci | sed 's/^.*state\": //g' | sed 's/ .*$//g' > /tmp/getCA");
	//Get CA Info
	fileCA = fopen("/tmp/getCA", "r");
	if (fileCA){
		ca = fgetc(fileCA);
		if ((ca == '0')||(ca == '1')||(ca == '2')){		//Have addition Info (Band...etc)
			fclose(fileCA);
			system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep errno | sed 's/^.*state\": //g' | sed 's/,.*$//g' > /tmp/getCAstate");
			fileCAstate = fopen("/tmp/getCAstate", "r");	//Get CA State
			if (fileCAstate){
				ca_status = fgetc(fileCAstate);
				fclose(fileCAstate);
				if (ca_status == '0'){
					gstrcpy(pCA,"0");
				}else if (ca_status == '1'){
					gstrcpy(pCA,"1");
				}else if (ca_status == '2'){
					gstrcpy(pCA,"2");
				}else{
					gstrcpy(pCA,"0");
				}
			}else{
				gstrcpy(pCA,"0");
			}

			system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep pcell_band | sed 's/^.*pcell_band\": //g' | sed 's/,.*$//g' > /tmp/getCAband1");
			fileCABand1 = fopen("/tmp/getCAband1", "r");	//Get Primary Band
			if (fileCABand1){
				while (fgets ( line, sizeof line, fileCABand1 ) != NULL){
        			ca_band1=line;
        		}
				fclose(fileCABand1);
				int CAband1 = gatoi(ca_band1);
				int i =0;
    			for (i = 0; i < count ; i++) {
					if (web_Enum_BandValueList[i].value == CAband1){
						gstrcpy(pBand1,web_Enum_BandValueList[i].str);
					}
    			}
			}	
			system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep scell_band | sed 's/^.*scell_band\": //g' | sed 's/ .*$//g' > /tmp/getCAband2");
			fileCABand2 = fopen("/tmp/getCAband2", "r");	//Get Second Band
			if (fileCABand2){
				while (fgets ( line2, sizeof line2, fileCABand2 ) != NULL){
        			ca_band2=line2;
        		}
				fclose(fileCABand2);
				int CAband2 = gatoi(ca_band2);
				int i =0;
    			for (i = 0; i < count ; i++) {
					if (web_Enum_BandValueList[i].value == CAband2){
						gstrcpy(pBand2,web_Enum_BandValueList[i].str);
					}
    			}
			}
		} else {							              //No addion Info(Band...etc)
			fclose(fileCA);
			system ("JsonClient /tmp/cgi-2-sys get_wwan_lte_ca_info | grep errno | sed 's/^.*state\": //g' | sed 's/ .*$//g' > /tmp/getCAstate");
			fileCAstate = fopen("/tmp/getCAstate", "r");
			if (fileCAstate){
				ca_status = fgetc(fileCAstate);
				fclose(fileCAstate);
				if (ca_status == '0'){
					gstrcpy(pCA,"0");
				}else if (ca_status == '1'){
					gstrcpy(pCA,"1");
				}else if (ca_status == '2'){
					gstrcpy(pCA,"2");
				}else{
					gstrcpy(pCA,"0");
				}
			}else{
				gstrcpy(pCA,"0");
			}
			gstrcpy(pBand1,"-1");
			gstrcpy(pBand2,"-1");
		}
	} else{
		gstrcpy(pCA,"0");
		gstrcpy(pBand1,"-1");
		gstrcpy(pBand2,"-1");
	}

	gstrcat(pCAInfo,pCA);
	gstrcat(pCAInfo,pSep);
	gstrcat(pCAInfo,pBand1);
	gstrcat(pCAInfo,pSep);
	gstrcat(pCAInfo,pBand2);
	gstrcat(pCAInfo,pSep);

	ret = websWrite(wp, T("\"%s\""), pCAInfo);
	free(pCA);
	free(pBand1);
	free(pBand2);
	return ret;
}


static int getPinConfig (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GETLTEFROMFILE

 char pPinConfig[128];

 memset(pPinConfig, '\0', sizeof(pPinConfig));
 system("lteStatus.sh getPinConfig");

 FILE *fp = fopen(PINCONFIG, "r");
 char line[64];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   gstrcat(pPinConfig,line);
   gstrcat(pPinConfig,pSep);
  }
  fclose(fp);
 } else {
 	return websWrite(wp, T("\"%s\""), "##");
 }

 //remove(PINCONFIG);

 return websWrite(wp, T("\"%s\""), pPinConfig);

#else

 char pPinConfig[256];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from ltePinStatus where pinId=1";

 char* pPinStatus = NULL;
 char* pPinVerify = NULL;
 char* pPinUnblock = NULL;

 system("lteStatus.sh getPinConfig");

 system ("lte_get -p --timeout 5000 --update-db > /dev/NULL");
 sleep(1);

 memset(pPinConfig,'\0',sizeof(pPinConfig));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pPinConfig);
 }

 if (nRows > 0) {
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "pinStatus", pPinStatus, getPinConfigOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "pinVerifyRetryLeft", pPinVerify, getPinConfigOut);
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "pinUnblockRetryLeft", pPinUnblock, getPinConfigOut);

   gstrcat(pPinConfig,pPinStatus);
   gstrcat(pPinConfig,pSep);
   gstrcat(pPinConfig,pPinVerify);
   gstrcat(pPinConfig,pSep);
   gstrcat(pPinConfig,pPinUnblock);

   //printf("pPinStatus = %s\n",pPinStatus);
   //printf("pPinVerify = %s\n",pPinVerify);
 }

getPinConfigOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return websWrite(wp, T("\"%s\""),pPinConfig);

#endif
}

static void setPinConfig (webs_t wp, char_t *path, char_t *query)
{
 char_t * pPinStatus = websGetVar(wp, T("PinStatus"), T(""));
 char_t * pPinEnable = websGetVar(wp, T("pin_enable"), T(""));
 char_t * pPinCode = websGetVar(wp, T("lte_pin"), T(""));
 char_t * pPukCode = websGetVar(wp, T("lte_puk"), T(""));

 char pPinPukQry [128] = {0};
 char pPinQry [128] = {0};
 char pVerifyQry [128] = {0};
 memset (pPinPukQry, 0x00, sizeof(pPinPukQry));
 memset (pPinQry, 0x00, sizeof(pPinQry));
 memset (pVerifyQry, 0x00, sizeof(pVerifyQry));

 if (gstrcmp(pPinStatus,"1") == 0) {
 	if (pPinCode) {
 	  snprintf(pPinQry, sizeof(pPinQry), "lte_set -n verify 1 '%s' > /dev/NULL", pPinCode);
 	  system (pPinQry);
 	}
 } else	if (gstrcmp(pPinStatus,"4") == 0) {
 	if (pPinCode && pPukCode) {
 	  snprintf(pPinPukQry, sizeof(pPinPukQry), "lte_set -n unblock 1 '%s' '%s' > /dev/NULL", pPukCode, pPinCode);
 	  system (pPinPukQry);
 	  sleep(1);
 	}
 } else {
   if (pPinCode) {
 	 if (pPinEnable) {
 	   if (gstrcmp(pPinEnable,"on") == 0) {
 	      snprintf(pPinQry, sizeof(pPinQry), "lte_set -n set 1 1 '%s' > /dev/NULL", pPinCode);
 	   } else {
 	      snprintf(pPinQry, sizeof(pPinQry), "lte_set -n set 1 0 '%s' > /dev/NULL", pPinCode);
 	   }
       system (pPinQry);
 	   
     }
   }
 }

 if (pPinCode || pPukCode) {
   system ("lte_get -p --timeout 5000 --update-db > /dev/NULL");

   //Poland WebUI skin
#if defined(__P02003_Dora)
	updateIMSIToDB();
#endif

   sleep(3);

   //export db to ascii
   system(EXPORT_CMD);
 }
#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
 websRedirect(wp, T("internet/internet_pin.asp"));
#else
 websRedirect(wp, T("internet/internet_simsetting.asp"));
#endif

}

static void setChangePin (webs_t wp, char_t *path, char_t *query)
{
 char_t * pOldPin = websGetVar(wp, T("lte_OldPin"), T(""));
 char_t * pNewPin = websGetVar(wp, T("lte_NewPin"), T(""));
 char_t * pConfirmPin = websGetVar(wp, T("lte_ConfirmPin"), T(""));

 //printf("lte_OldPin = '%s'\n",pOldPin);
 //printf("lte_NewPin = '%s'\n",pNewPin);
 //printf("lte_ConfirmPin = '%s'\n",pConfirmPin);

 char pChangePinQry [128] = {0};
 memset (pChangePinQry, 0x00, sizeof(pChangePinQry));

 snprintf(pChangePinQry, sizeof(pChangePinQry), "lte_set -n change 1 '%s' '%s' > /dev/NULL", pOldPin, pNewPin);
 system (pChangePinQry);

 system ("lte_get -p --timeout 5000 --update-db > /dev/NULL");
 sleep(3);

 //export db to ascii
 system(EXPORT_CMD);
#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
 websRedirect(wp, T("internet/internet_pin.asp"));
#else
 websRedirect(wp, T("internet/internet_simsetting.asp"));
#endif
}

//PIN verify by Home page
static void setVerifyPIN (webs_t wp, char_t *path, char_t *query)
{
	char_t * pPinCode = websGetVar(wp, T("wwan_pin_popup"), T(""));
	char pVerifyQry [128] = {0};
	memset (pVerifyQry, 0x00, sizeof(pVerifyQry));

	if (pPinCode) {
 	  snprintf(pVerifyQry, sizeof(pVerifyQry), "lte_set -n verify 1 '%s' > /dev/NULL", pPinCode);
 	  system (pVerifyQry);
 	}

	if (pPinCode) {
   		system ("lte_get -p --timeout 5000 --update-db > /dev/NULL");
   		sleep(3);
   		system(EXPORT_CMD); //export db to ascii
 	}

	websRedirect(wp, T("../home.asp"));
}

static void setPUK (webs_t wp, char_t *path, char_t *query)
{
	char_t * pPinCode = websGetVar(wp, T("new_pin"), T(""));
	char_t * pPukCode = websGetVar(wp, T("puk_verify"), T(""));

	char pPinPukQry [128] = {0};
	memset (pPinPukQry, 0x00, sizeof(pPinPukQry));

	if (pPinCode && pPukCode) 
	{
		snprintf(pPinPukQry, sizeof(pPinPukQry), "lte_set -n unblock 1 '%s' '%s' > /dev/NULL", pPukCode, pPinCode);
		system (pPinPukQry);
		sleep(1);
	}

	if (pPinCode || pPukCode) 
	{
		system ("lte_get -p --timeout 5000 --update-db > /dev/NULL");
		sleep(3);

		//export db to ascii
		system(EXPORT_CMD);
	}

	websRedirect(wp, T("../home.asp"));
}


//Network Provider Selection --start
static int getNetworkSelMode (int eid, webs_t wp, int argc, char_t **argv)
{
 char pRegMethod[32];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select lteRegMethod from lteNwStatus";

 char* pSelMode = NULL;

 memset(pRegMethod,'\0',sizeof(pRegMethod));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""), pRegMethod);
 }

 if (nRows > 0) {
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "lteRegMethod", pSelMode, getRegMethodOut);

  gstrcat(pRegMethod,pSelMode);
 }

getRegMethodOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return websWrite(wp, T("\"%s\""), pRegMethod);
}

static void setNetworkSelMode (webs_t wp, char_t *path, char_t *query)
{
 char_t * pRegMethod = websGetVar(wp, T("providerRegMethod"), T(""));
 //char_t * pSelType = websGetVar(wp, T("applySelType"), T(""));

 //printf("pRegMethod = %s\n",pRegMethod);
 //printf("pSelType = %s\n",pSelType);

 if (gstrcmp(pRegMethod,"auto") == 0) {
 	system ("lte_set -r auto > /dev/NULL");

 	//unnecessary
  //export db to ascii
  //system(EXPORT_CMD);
 } else if (gstrcmp(pRegMethod,"manual") == 0) {
  //To Do: Scan Networks
 }
 sleep(5);
#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
 websRedirect(wp, T("internet/internet_scan.asp"));
#else
 websRedirect(wp, T("internet/internet_general.asp"));
#endif
}

//only for P0200x --- start
static int getRadioMode (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __P02002_Dora
	websWrite(wp, T("<option value=\"4G\"><script>document.write(gettext(\"LTE network only\"));</script></option>"));
#else
	websWrite(wp, T("<option value=\"auto\"><script>document.write(gettext(\"Auto\"));</script></option>"));
	websWrite(wp, T("<option value=\"4G\"><script>document.write(gettext(\"LTE network only\"));</script></option>"));
	websWrite(wp, T("<option value=\"3G\"><script>document.write(gettext(\"3G network only\"));</script></option>"));
#endif

	return 0;
}

static int getRadioFrequency (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __P02002_Dora
	websWrite(wp, T("<option value=\"auto\"><script>document.write(gettext(\"Auto\"));</script></option>"));
	websWrite(wp, T("<option value=\"1800\">1800</option>"));
	websWrite(wp, T("<option value=\"2600\">2600</option>"));
	websWrite(wp, T("<option value=\"800\">800</option>"));
#else
	websWrite(wp, T("<option value=\"auto\"><script>document.write(gettext(\"Auto\"));</script></option>"));
	websWrite(wp, T("<option value=\"1800\">1800</option>"));
	websWrite(wp, T("<option value=\"2600\">2600</option>"));
	websWrite(wp, T("<option value=\"800\">800</option>"));
	websWrite(wp, T("<option value=\"900\">900</option>"));
#endif
	return 0;
}

//only for P0200x --- end

static int scanNetworkProvider (int eid, webs_t wp, int argc, char_t **argv)
{
 system ("lte_get -s --timeout 0 --update-db > /dev/NULL");
 sleep(5);
 return 0;
}

#if defined(__P02003_Dora)
static int showNetworkProvider (int eid, webs_t wp, int argc, char_t **argv)
{
 char pLteProvider[256];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from lteOperatorList";

 int index = 0;
 char* pProviderName = NULL;
 char* pMcc = NULL;
 char* pMnc = NULL;
 char* pRat = NULL;
 char* ptr = NULL;

 char pRadioMode[128];
 char pProviderName1[256];

 memset(pRadioMode, '\0', sizeof(pRadioMode));

 system("lteStatus.sh getLTERadio");

 FILE *fp = fopen(LTERADIO, "r");
 char line[64];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
 	int i = 0;
  while(fgets(line, sizeof(line), fp) != NULL)
  {
  	i = i + 1;
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   if (i == 1) {
    if (gstrcmp(line,"0x001C") == 0) {
 	    gstrcat(pRadioMode, "auto");
    } else if (gstrcmp(line,"0x0010") == 0) {
 	    gstrcat(pRadioMode, "4G");
    } else if (gstrcmp(line,"0x0008") == 0) {
 	    gstrcat(pRadioMode, "3G");
    } else if (gstrcmp(line,"0x0004") == 0) {
 	    gstrcat(pRadioMode, "2G");
    } else {
      gstrcat(pRadioMode, "auto");
    }
    //gstrcat(pRadioMode,pSep);
   }
  }
  fclose(fp);
 } else {
 	gstrcat(pRadioMode, "auto");
 }


 
 memset(pLteProvider,'\0',sizeof(pLteProvider));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pLteProvider);
 }

 snprintf(pLteProvider, sizeof(pLteProvider),"%d",nRows);
 gstrcat(pLteProvider,pSep);

 websWrite(wp, T("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"));
 websWrite(wp, T("<tr>"));
 websWrite(wp, T("<td class=\"tdH\" width=\"10%\">&nbsp;</td>"));
 websWrite(wp, T("<td class=\"tdH\"><script>document.write(gettext(\"List of Network Providers\"));</script></td>"));
 websWrite(wp, T("</tr>"));
 if (nRows > 0) {
  //printf("showNetworkProvider nRows=%d\n",nRows);
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "providerName", pProviderName, getLTEProviderOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "mcc", pMcc, getLTEProviderOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "mnc", pMnc, getLTEProviderOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rat", pRat, getLTEProviderOut);
    //printf("showNetworkProvider index=%d\n",index);
    //printf("pMcc = %s\n",pMcc);
    //printf("pMnc = %s\n",pMnc);
    //printf("pRat = %s\n",pRat);
    memset(pProviderName1, '\0', sizeof(pProviderName1));
    memcpy(pProviderName1,pProviderName,strlen(pProviderName));
    if ((strstr(pProviderName,pRadioMode)!=NULL) ||(!gstrcmp(pRadioMode,"auto"))) {
	    websWrite(wp, T("<tr>"));
	    websWrite(wp, T("<td class=\"tdOdd\">"));
	    websWrite(wp, T("<input type=\"radio\" name=\"providerNameIndex\" value=\"%d\" />"), index+1);
	    websWrite(wp, T("</td>"));
	    ptr = strstr(pProviderName1,"4G");
	    if(ptr!=NULL) {
		memcpy(ptr,"LTE",3);
	    }
	    if(!gstrcmp(pProviderName1,"Cyfrowy Polsa, LTE"))
		websWrite(wp, T("<td class=\"tdOdd\"><script>document.write(gettext(\"Cyfrowy Polsat, LTE\"));</script></td>"));
	    else if(!gstrcmp(pProviderName1,"Plus, LTE"))
		websWrite(wp, T("<td class=\"tdOdd\"><script>document.write(gettext(\"Cyfrowy Polsat, LTE\"));</script></td>"));
	    else if(!gstrcmp(pProviderName1,"Plus, 3G"))
		websWrite(wp, T("<td class=\"tdOdd\"><script>document.write(gettext(\"Cyfrowy Polsat, 3G\"));</script></td>"));
	    else if(!gstrcmp(pProviderName1,"Plus, 2G"))
		websWrite(wp, T("<td class=\"tdOdd\"><script>document.write(gettext(\"Cyfrowy Polsat, 2G\"));</script></td>"));
	    else
	       websWrite(wp, T("<td class=\"tdOdd\">%s</td>"),pProviderName1);
	    websWrite(wp, T("</tr>"));
    }
  }
 } else {
  websWrite(wp, T("<tr>"));
  websWrite(wp, T("<td class=\"tdOdd\" colspan=\"2\" id=\"no_network_provider\"><script>document.write(gettext(\"There is no any network provider!\"));</script></td>"));
  websWrite(wp, T("</tr>"));
 }
 websWrite(wp, T("</table>"));

getLTEProviderOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
}
#else
static int showNetworkProvider (int eid, webs_t wp, int argc, char_t **argv)
{
 char pLteProvider[256];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from lteOperatorList";

 int index = 0;
 char* pProviderName = NULL;
 char* pMcc = NULL;
 char* pMnc = NULL;
 char* pRat = NULL;

 scanNetworkProvider(eid, wp, argc, argv);

 memset(pLteProvider,'\0',sizeof(pLteProvider));
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return websWrite(wp, T("\"%s\""),pLteProvider);
 }

 snprintf(pLteProvider, sizeof(pLteProvider),"%d",nRows);
 gstrcat(pLteProvider,pSep);

 websWrite(wp, T("<table class=\"listable\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"margin:0 0 20px 24px;\">"));
 websWrite(wp, T("<tr>"));
 websWrite(wp, T("<th>&nbsp;</th>"));
 websWrite(wp, T("<th><script>document.write(gettext(\"List of Network Providers\"));</script></th>"));
 websWrite(wp, T("</tr>"));
 websWrite(wp, T("<tbody>"));
 if (nRows > 0) {
  //printf("showNetworkProvider nRows=%d\n",nRows);
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "providerName", pProviderName, getLTEProviderOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "mcc", pMcc, getLTEProviderOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "mnc", pMnc, getLTEProviderOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "rat", pRat, getLTEProviderOut);
    //printf("showNetworkProvider index=%d\n",index);
    //printf("pMcc = %s\n",pMcc);
    //printf("pMnc = %s\n",pMnc);
    //printf("pRat = %s\n",pRat);
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
    }
    websWrite(wp, T("<td>"));
    websWrite(wp, T("<input type=\"radio\" name=\"providerNameIndex\" value=\"%d\" />"), index+1);
    websWrite(wp, T("</td>"));
    websWrite(wp, T("<td>%s</td>"),pProviderName);
    websWrite(wp, T("</tr>"));
  }
 } else {
  websWrite(wp, T("<tr>"));
  websWrite(wp, T("<td>&nbsp;</td>"));
  websWrite(wp, T("<td id=\"no_network_provider\"><script>document.write(gettext(\"There is no any network provider!\"));</script></td>"));
  websWrite(wp, T("</tr>"));
 }
 websWrite(wp, T("</tbody>"));
 websWrite(wp, T("</table>"));

getLTEProviderOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
}
#endif

static void setNetworkProvider (webs_t wp, char_t *path, char_t *query)
{
 char_t * pProviderIndex = websGetVar(wp, T("providerNameIndex"), T(""));
 int rowId = gatoi(pProviderIndex);

 char pProviderQry [128] = {0};
 memset (pProviderQry, 0x00, sizeof(pProviderQry));

#ifdef __USED_WEBS_DB__
 char * pMcc = jnrp_sql_getAttr_Webs("lteOperatorList","mcc",rowId);
 char * pMnc = jnrp_sql_getAttr_Webs("lteOperatorList","mnc",rowId);
 char * pRat = jnrp_sql_getAttr_Webs("lteOperatorList","rat",rowId);
#else
 char * pMcc = jnrp_sql_getAttr(pDBFile,"lteOperatorList","mcc",rowId);
 char * pMnc = jnrp_sql_getAttr(pDBFile,"lteOperatorList","mnc",rowId);
 char * pRat = jnrp_sql_getAttr(pDBFile,"lteOperatorList","rat",rowId);
#endif

 snprintf(pProviderQry, sizeof(pProviderQry), "lte_set -r manual %s %s %s > /dev/NULL", pMcc, pMnc, pRat);
 system (pProviderQry);
 //printf("pProviderQry = %s\n",pProviderQry);

 websHeader(wp);
 websWrite(wp, "<head>\n");
 websWrite(wp, "<link rel=\"stylesheet\" href=\"../style/all.css\" type=\"text/css\" />\n");
 websWrite(wp, "<script language=\"Javascript\" src=\"../js/mgmt.js\" type=\"text/javascript\"></script>\n");
 websWrite(wp, "</head>\n");
 websWrite(wp, "<body style=\"background-color:transparent\">\n");
#if defined(__P02003_Dora)
	websWrite(wp, "<br><br><p align=\"left\"><span class=\"statusMsg\"><script>document.write(gettext(\"Setting completed.\"));</script></span></p>\n");
#else
	websWrite(wp, "<br><br><p align=\"left\"><font size=\"3\" color=\"blue\" face=\"Arial\"><script>document.write(gettext(\"Setting completed.\"));</script></font></p>\n");
#endif
 websWrite(wp, "</body>\n");
	websFooter(wp);
	websDone(wp, 200);

	//unnecessary
 //export db to ascii
 //system(EXPORT_CMD);

 return;
}
//Network Provider Selection --end

#if defined(__P02003_Dora)
static int getWanProfileListData1(int eid, webs_t wp, int argc, char_t **argv)
{
 char pWANProfile[256];
 memset(pWANProfile, '\0', sizeof(pWANProfile));
 snprintf(pWANProfile,sizeof(pWANProfile), "");

	int index = 1;
	char* pIP = NULL;
	int nRows = jnrp_sql_getRowNum_Webs ("lteIPStatus");
	//int nRows = 1;
	char * pQryResult = jnrp_sql_getAttr_Webs ("lteNwStatus", "Roaming", 1);
	
	if (nRows>0)
	{
		for (index=1; index<=nRows; index++)
	 {
			 pIP = jnrp_sql_getAttr_Webs ("lteIPStatus", "ip", index);
    //printf("index=%d, pIP=%s\n", index, pIP);
    gstrcat(pWANProfile,pIP);
    gstrcat(pWANProfile,pSep);
	    pIP = jnrp_sql_getAttr_Webs ("lteIPv6Status", "ip", index);
        gstrcat(pWANProfile,pIP);
        gstrcat(pWANProfile,pSep);
	 }
	}

	return websWrite(wp, T("\"%s###%s\""), pWANProfile,pQryResult);
}

static int getWanProfileListData2(int eid, webs_t wp, int argc, char_t **argv)
{
 char pWANProfile[256];
 memset(pWANProfile, '\0', sizeof(pWANProfile));
 snprintf(pWANProfile,sizeof(pWANProfile), "");

	int index = 1;
	char* pProfileName = NULL;
	char* pProfileStatus = NULL;
	int nRows = jnrp_sql_getRowNum_Webs ("lteNwProfile");
	system("lte_get -i 0 1 --timeout 5000 --update-db > /dev/NULL");
	system("lte_get -i 0 2 --timeout 5000 --update-db > /dev/NULL");
	//int nRows = 1;
	if (nRows>0)
	{
		for (index=1; index<=nRows; index++)
	 {
			 pProfileName = jnrp_sql_getAttr_Webs ("lteNwProfile", "name", index);
    //printf("index=%d, pProfileName=%s\n", index, pProfileName);
    gstrcat(pWANProfile,pProfileName);
    gstrcat(pWANProfile,pSep);
	 }
	}

	return websWrite(wp, T("\"%s\""), pWANProfile);
}

static int getWanProfileListConnectIndex(int eid, webs_t wp, int argc, char_t **argv)
{
	char pProfileInfo[256];
	FILE *fileCA, *fileCAstate;
	memset(pProfileInfo,'\0',sizeof(pProfileInfo));

 	system("lte_get -l --timeout 5000 > /tmp/getWanProfileInfo");

	FILE *fp = fopen("/tmp/getWanProfileInfo", "r");
	char line[128];
	int len = 0;
 	memset(line, '\0', sizeof(line));
	if(fp){
		while(fgets(line, sizeof(line), fp) != NULL){
			if (line[strlen(line)-1] == '\n')
	 			line[strlen(line)-1] = '\0';

			if (gstrstr(line,"index:"))
			{
	 			gstrcat(pProfileInfo,getSplitStr(line, ":", 2));
	 			gstrcat(pProfileInfo,pSep);
			} 
		}
		fclose(fp);
	}else {
 		return websWrite(wp, T("\"%s\""), "#####");
 	}

	return websWrite(wp, T("\"%s\""), pProfileInfo);
}

static void setWanProfile(webs_t wp, char_t *path, char_t *query)
{
	char * pProfileAction = websGetVar(wp, T("profileAction"), T(""));
	char * pProfileIndex = websGetVar(wp, T("profileIndex"), T(""));
	char * pProfile1Status = websGetVar(wp, T("profile1Status"), T(""));
	char * pProfile2Status = websGetVar(wp, T("profile2Status"), T(""));

 //printf("%s, pProfileAction=%s, pProfileIndex=%s, pProfile1Status=%s\n", __func__, pProfileAction, pProfileIndex, pProfile1Status);
 int nIndex = gatoi(pProfileIndex);
 int nStatus1 = gatoi(pProfile1Status);
 int nStatus2 = gatoi(pProfile2Status);

	if (!gstrcmp(pProfileAction,"Profile_Edit"))
	{
		wan_profile_list_idx=nIndex;
		websRedirect(wp, T("internet/internet_profile_editor.asp"));
	}
	else if (!gstrcmp(pProfileAction,"Profile_Connect"))
	{
   int index = 0;
   char * pQryResult = NULL;
   char cmdbuf [256] = {0};
   memset (cmdbuf, 0x00, sizeof(cmdbuf));
   char cntStatus[32] = {0};
   memset (cntStatus, 0x00, sizeof(cntStatus));
   //printf("%s, nIndex=%d, nStatus1=%d\n", __func__, nIndex, nStatus1);
   if (nIndex == 1) {
   	if (nStatus1 == 1) {
      snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -q 0 -c -1 > /dev/NULL");
      system(cmdbuf);
	  system("sync");//to sync malmanager.cfg
      snprintf(cntStatus, sizeof (cntStatus), "no connection");
   	} else if (nStatus1 == 0) {
      snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -d -q 0 > /dev/NULL");
      system(cmdbuf);
	  system("sync");//to sync malmanager.cfg
      //system("/sbin/ifconfig usb0 0.0.0.0");
      snprintf(cntStatus, sizeof (cntStatus), "connected");
   	}
   	index = 0;
   	pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
   	//printf("%s, pQryResult=%s\n", __func__, pQryResult);
    while(!gstrcmp(pQryResult, cntStatus) && index < 20)
    {
      index++;
      sleep(1);
      pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
      //printf("%s, pQryResult=%s, index=%d\n", __func__, pQryResult, index);
    }
   }else if (nIndex == 2) {
   	if (nStatus2 == 1) {
      snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -q 0 -c -2 > /dev/NULL");
      system(cmdbuf);
	  system("sync");//to sync malmanager.cfg
      snprintf(cntStatus, sizeof (cntStatus), "no connection");
   	} else if (nStatus2 == 0) {
      snprintf(cmdbuf, sizeof (cmdbuf), "lte_set -d -q 0 > /dev/NULL");
      system(cmdbuf);
	  system("sync");//to sync malmanager.cfg
      //system("/sbin/ifconfig usb0 0.0.0.0");
      snprintf(cntStatus, sizeof (cntStatus), "connected");
   	}
   	index = 0;
   	pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
   	//printf("%s, pQryResult=%s\n", __func__, pQryResult);
    while(!gstrcmp(pQryResult, cntStatus) && index < 20)
    {
      index++;
      sleep(1);
      pQryResult = jnrp_sql_getAttrByCondit_Webs("lteIPStatus","status","qid=1");
      //printf("%s, pQryResult=%s, index=%d\n", __func__, pQryResult, index);
    }
   }
	  websRedirect(wp, T("internet/internet_profile.asp"));
	}

 return;
}

static int getWanProfileConfig(int eid, webs_t wp, int argc, char_t **argv)
{
 //printf("wan_profile_list_idx=%d\n",wan_profile_list_idx);
	if (wan_profile_list_idx == -1) //none
	{
		wan_profile_list_idx = 1;
	}

	if (wan_profile_list_idx > 0) //edit
	{
   char pWANProfile[256];
   memset(pWANProfile, '\0', sizeof(pWANProfile));

   char cmdbuf[256];
   memset(cmdbuf, '\0', sizeof(cmdbuf));
   snprintf(cmdbuf,sizeof(cmdbuf), "lte_get -i 0 %d --timeout 5000 --output-file /tmp/wan_profile1 > /dev/NULL", wan_profile_list_idx);
   //printf("%s, cmdbuf=%s\n", __func__, cmdbuf);
   system(cmdbuf);

   FILE *fp = fopen("/tmp/wan_profile1", "r");
   char line[256];
   memset(line, '\0', sizeof(line));
   if(fp)
   {
     while(fgets(line, sizeof(line), fp) != NULL)
     {
       if (line[strlen(line)-1] == '\n')
         line[strlen(line)-1] = '\0';

       if (gstrstr(line,"Profile Name=")
   	     || gstrstr(line,"PDP Type=") || gstrstr(line,"APN Name=")
         || gstrstr(line,"Username=") || gstrstr(line,"Password=")
         || gstrstr(line,"Authentication Preference="))
       {
          gstrcat(pWANProfile,getSplitStr(line, "=", 2));
          gstrcat(pWANProfile,pSep);
       }
     }
     fclose(fp);
   } else {
 	   return websWrite(wp, T("\"%s\""), "");
   }
   return websWrite(wp, T("\"%s\""), pWANProfile);
	}
	return websWrite(wp, T("\"%s\""), "");
}

static void setWanProfileConfig(webs_t wp, char_t *path, char_t *query)
{
	char * pErrMsg = NULL;
	int status = -1;
	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
	
	char * pProfileName = websGetVar(wp, T("wan_ProfileName"), T(""));
	char * pUserName = websGetVar(wp, T("wan_UserName"), T(""));
	char * pPassword = websGetVar(wp, T("wan_Password"), T(""));
	char * pAuthMethod = websGetVar(wp, T("wan_AuthMethod"), T(""));
	char * pAPN = websGetVar(wp, T("wan_APN"), T(""));
	char * pPDPType = websGetVar(wp, T("wan_PDPType"), T(""));
	char * pRoamingPDPType = websGetVar(wp, T("roaming_PDPType"), T(""));

	char cmdbuf [512] = {0};
	memset (cmdbuf, '\0', sizeof(cmdbuf));
	snprintf(cmdbuf,sizeof(cmdbuf), "lte_set -M 0 %d -t %s -m %s -P '%s' -a '%s' -u '%s' -p '%s' > /dev/NULL", wan_profile_list_idx, pPDPType, pAuthMethod, pProfileName, pAPN, pUserName, pPassword);
 	//printf("%s, cmdbuf=%s\n", __func__, cmdbuf);
	system(cmdbuf);

	//unnecessary, export db to ascii
	system ("lte_get -i 0 1 --timeout 5000 --update-db > /dev/NULL");
	sleep(1);
 	system(EXPORT_CMD);

 	wan_profile_list_idx=-1;

	//Set roaming pdp type into DB
	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	{
		printf ("unable to open the DB file %s\n", pDBFile);
	}
	
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update RoamingPDPType set pdptype2='%s', pdptype='%s'", pPDPType, pRoamingPDPType);
    status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	if (pDB) sqlite3_close (pDB);

	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD);
	}
	
 	websRedirect(wp, T("internet/internet_profile.asp"));
 	return;
}

/*
internal antenna:
 /mnt/data/ant_sw_int
external antenna:
 /mnt/data/ant_sw_ext
*/
static int getAntennaMode(int eid, webs_t wp, int argc, char_t **argv)
{
 char pAntennaMode[32];
 memset(pAntennaMode, 0x00, sizeof(pAntennaMode));
 //system("cat /sys/class/leds/ANT_SW/brightness > /tmp/antenna.webui");
 system("cat /sys/class/leds/LTE_ANT_IND/brightness > /tmp/antenna.webui");

 FILE *fp = fopen("/tmp/antenna.webui", "r");
 char line[32];
 memset(line, 0x00, sizeof(line));
 if(fp)
 {
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
   gstrcat(pAntennaMode,line);
  }
  fclose(fp);
 }
 system("rm -f /tmp/antenna.webui");
 if (!gstrcmp(pAntennaMode, "255")) {
 	return websWrite(wp, T("\"%s\""), "1");
 } else {
 	return websWrite(wp, T("\"%s\""), "0");
 }
}

/* Internal wan priority: 0 (3G/LTE), 1(Ethernet WAN)*/
static int getInternetPriority(int eid, webs_t wp, int argc, char_t **argv)
{
  char pInternetPriority[256];

  int nRows=0,nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "Select etype From lteWanPriority";

  char* pPriorityStatus = NULL;

  memset(pInternetPriority,'\0',sizeof(pInternetPriority));
#ifdef __USED_WEBS_DB__
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
  sqlite3 * pWebDB = NULL;
  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	//sqlite3_free(pQueryStr);
    return websWrite(wp, T("\"%s\""),pInternetPriority);
  }
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "etype", pPriorityStatus, getInternetPriorityOut);
  
  //printf ("###jnrp_cgi_internet.c: getInternetPriority: pPriorityStatus = %s\n", pPriorityStatus);

  gstrcat(pInternetPriority,pPriorityStatus);

getInternetPriorityOut:
  if (ppQueryRes)
  {
    sqlite3_free_table(ppQueryRes);
  }

  return websWrite(wp, T("\"%s\""), pInternetPriority);
}

static void setInternetPriority(webs_t wp, char_t *path, char_t *query)
{
	char * pPriorityStatus = websGetVar(wp, T("internet_priority_status"), T(""));

	char * pErrMsg = NULL;
	int status = -1;
	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	//printf ("###jnrp_cgi_internet.c: setInternetPriority: pPriorityStatus = %s\n", pPriorityStatus);

	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	{
		printf ("unable to open the DB file %s\n", pDBFile);
		websRedirect(wp, T("internet/lteWanPriority.asp"));
	}
	
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update lteWanPriority set etype='%s'",pPriorityStatus);
    status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	if (pDB) sqlite3_close (pDB);

	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD);
	}
 	websRedirect(wp, T("internet/internet_priority.asp"));
 	return;
}

static int getEthernetWANSetting(int eid, webs_t wp, int argc, char_t **argv)
{
	char** ppQueryRes = NULL;
	int nRows=0,nCols=0;
	
	char* pType = NULL;
	char* pServiceName = NULL;
	char* pUserName = NULL;
	char* pPassword = NULL;
	char* pIP = NULL;
	char* pMacAddr = NULL;
	char* pSubnetMask = NULL;
	char* pGateway = NULL;
	char* pMtu = NULL;
	char* pHostName = NULL;
	char* pDNS1 = NULL;
	char* pDNS2 = NULL;

  	char pWan[512];
	char * pQueryStr = "Select * From ethWanSetting";

	memset(pWan,'\0',sizeof(pWan));
#ifdef __USED_WEBS_DB__
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
  sqlite3 * pWebDB = NULL;
  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	//sqlite3_free(pQueryStr);
    return websWrite(wp, T("\"%s\""),pWan);
  }

  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "etype",pType, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "service_name",pServiceName, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "username",pUserName, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "password",pPassword, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ip",pIP, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "mac",pMacAddr, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "subnet",pSubnetMask, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "gateway",pGateway, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "mtu",pMtu, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "hostname",pHostName, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "dns1",pDNS1, getEthernetWanSettingOut);
  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "dns2",pDNS2, getEthernetWanSettingOut);
	
  gstrcat(pWan,pType);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pServiceName);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pUserName);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pPassword);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pIP);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pMacAddr);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pSubnetMask);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pGateway);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pMtu);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pHostName);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pDNS1);
  gstrcat(pWan,pSep);
  gstrcat(pWan,pDNS2);
  gstrcat(pWan, pSep);

getEthernetWanSettingOut:
if (ppQueryRes)
{
  sqlite3_free_table (ppQueryRes);
}
  return websWrite(wp, T("\"%s\""),pWan);
}

static void setEthernetWanSetting(webs_t wp, char_t *path, char_t *query)
{
	char_t * pType = websGetVar(wp, T("ethernet_wan_type"), T(""));
	char_t * pServiceName = websGetVar(wp, T("wan_servicename"), T(""));
	char_t * pUserName = websGetVar(wp, T("wan_username"), T(""));
	char_t * pPassword = websGetVar(wp, T("wan_password"), T(""));
	char_t * pIP = websGetVar(wp, T("wan_ip"), T(""));
	char_t * pMacAddr = websGetVar(wp, T("wan_mac"), T(""));
	char_t * pSubnetMask = websGetVar(wp, T("wan_subnet"), T(""));
	char_t * pGateway = websGetVar(wp, T("wan_gw"), T(""));
	char_t * pMtu = websGetVar(wp, T("wan_mtu"), T(""));
	char_t * pHostName = websGetVar(wp, T("wan_hostname"), T(""));
	char_t * pDNS1 = websGetVar(wp, T("wan_dns1"), T(""));
	char_t * pDNS2 = websGetVar(wp, T("wan_dns2"), T(""));

	char * pErrMsg = NULL;
	int status = -1;
	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	{
		printf ("unable to open the DB file %s\n", pDBFile);
		websRedirect(wp, T("internet/internet_ethernet_wan.asp"));
	}
	
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update ethWanConnType set etype='%s'",pType);
	
	if (!gstrcmp(pType,"0")){
		snprintf(pSQLCmd,sizeof(pSQLCmd), "update ethWanSetting set etype='%s', service_name='%s'," 
			"username='%s', password='%s', ip='%s', mac='%s', dns1='%s', dns2='%s'", 
			pType, pServiceName, pUserName, pPassword, pIP, pMacAddr, pDNS1, pDNS2);
	} else if (!gstrcmp(pType,"1")) {
		snprintf(pSQLCmd,sizeof(pSQLCmd), "update ethWanSetting set etype='%s'," 
			"hostname='%s', dns1='%s', dns2='%s'", 
			pType, pHostName, pDNS1, pDNS2);
	} else if (!gstrcmp(pType,"2")) {
		snprintf(pSQLCmd,sizeof(pSQLCmd), "update ethWanSetting set etype='%s'," 
			"ip='%s', subnet='%s', gateway='%s', mtu='%s', dns1='%s', dns2='%s'", 
			pType, pIP, pSubnetMask, pGateway, pMtu, pDNS1, pDNS2);
	}

	status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
	if (pDB) sqlite3_close (pDB);
	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD);
	}
	websRedirect(wp, T("internet/internet_ethernet_wan.asp"));
	return;
}

/* Internal wan priority: 0 (2G/3G/LTE), 1(Ethernet WAN)*/
static int getNetworkType(int eid, webs_t wp, int argc, char_t **argv)
{
  char pNetworkType[256];

  int nRows=0,nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "Select eStatus From ethWanStatus";

  char* pStatus = NULL;

  memset(pNetworkType,'\0',sizeof(pNetworkType));
#ifdef __USED_WEBS_DB__
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
  sqlite3 * pWebDB = NULL;
  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	//sqlite3_free(pQueryStr);
    return websWrite(wp, T("\"%s\""),pNetworkType);
  }
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "eStatus", pStatus, getNetworkTypeOut);
  
  //printf ("###jnrp_cgi_internet.c: getNetworkType: eStatus = %s\n", pStatus);

  gstrcat(pNetworkType,pStatus);

getNetworkTypeOut:
  if (ppQueryRes)
  {
    sqlite3_free_table(ppQueryRes);
  }

  return websWrite(wp, T("\"%s\""), pNetworkType);
}

static int getRoamingPDPType(int eid, webs_t wp, int argc, char_t **argv)
{
  char pRoamingPDPType[256];

  int nRows=0,nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "Select * From RoamingPDPType";

  char* pType = NULL;
  char* pRoamingType = NULL;

  memset(pRoamingPDPType,'\0',sizeof(pRoamingPDPType));
#ifdef __USED_WEBS_DB__
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
  sqlite3 * pWebDB = NULL;
  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
    return websWrite(wp, T("\"%s\""),pRoamingPDPType);
  }
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "pdptype2", pType, getRoamingPDPTypeOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "pdptype", pRoamingType, getRoamingPDPTypeOut);
  
  //printf ("###jnrp_cgi_internet.c: getNetworkType: eStatus = %s\n", pStatus);

  gstrcat(pRoamingPDPType,pType);
  gstrcat(pRoamingPDPType,pSep);
  gstrcat(pRoamingPDPType,pRoamingType);

getRoamingPDPTypeOut:
  if (ppQueryRes)
  {
    sqlite3_free_table(ppQueryRes);
  }

  return websWrite(wp, T("\"%s\""), pRoamingPDPType);
}
#endif
