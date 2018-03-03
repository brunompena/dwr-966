#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

#ifdef USER_MANAGEMENT_SUPPORT
#include    "../um.h"
#endif

static char system_command[COMMAND_MAX];

extern int doSystem(char_t *fmt, ...);
extern int showSystemCommandASP(int eid, webs_t wp, int argc, char_t **argv);

static void SystemCommand(webs_t wp, char_t *path, char_t *query);
static void clearSystemCommand(webs_t wp, char_t *path, char_t *query);
static void repeatLastSystemCommand(webs_t wp, char_t *path, char_t *query);

//time page
static int getCurrTime(int eid, webs_t wp, int argc, char_t **argv);
static int getNTPConfig (int eid, webs_t wp, int argc, char_t **argv);
static int getTimezone (int eid, webs_t wp, int argc, char_t **argv);
static void setCurrTime (webs_t wp, char_t *path, char_t *query);
static void setNTPConfig (webs_t wp, char_t *path, char_t *query);
//user page
static int getUserAccount (int eid, webs_t wp, int argc, char_t **argv);
static void setUserAccount (webs_t wp, char_t *path, char_t *query);
static void setUserAccountDel (webs_t wp, char_t *path, char_t *query);
#if defined(__P02003_Dora)
static void setUserAccountAction (webs_t wp, char_t *path, char_t *query);
static int getUserAccountList (int eid, webs_t wp, int argc, char_t **argv);
int user_account_idx=-1;
static int setRemoteLog(webs_t wp, char_t *path, char_t *query);
static int getUserStatus (int eid, webs_t wp, int argc, char_t **argv);

enum
{
	user_status_none=0,
	user_status_err=1
};
int check_user_status = user_status_none;
#endif

// system log page
static int getSysLog(int eid, webs_t wp, int argc, char_t **argv);
static int getRemoteLog(int eid, webs_t wp, int argc, char_t **argv);
static int getLogFiles(int eid, webs_t wp, int argc, char_t **argv);
static int setSysLog(webs_t wp, char_t *path, char_t *query);

// system reset
static int factoryReset(webs_t wp, char_t *path, char_t *query);
// syste reboot
static int sysReboot(webs_t wp, char_t *path, char_t *query);
static int getLTEDaemonStatus(int eid, webs_t wp, int argc, char_t **argv);

//FW upgrade page
static int get_user_engin_mode(int eid, webs_t wp, int argc, char_t **argv);
static int aspGetFwUpInfo(int eid, webs_t wp, int argc, char_t **argv);

//module firmware upgrade page
static int aspGetFwMDMpercent (int eid, webs_t wp, int argc, char_t **argv);

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
//Remote Management
static int getRemoteManage (int eid, webs_t wp, int argc, char_t **argv);
static void setRemManage (webs_t wp, char_t *path, char_t *query);

#endif

#if defined(__P02003_Dora)
static int getIpRange (int eid, webs_t wp, int argc, char_t **argv);
static int getIpRangeList (int eid, webs_t wp, int argc, char_t **argv);

static void setIpRange (webs_t wp, char_t *path, char_t *query);
static int getEventLog (int eid, webs_t wp, int argc, char_t **argv);
static void setEventLog (webs_t wp, char_t *path, char_t *query);
static void setRemManageAction (webs_t wp, char_t *path, char_t *query);
int rem_man_idx=-1;
enum
{
	iprange_status_none=0,
	iprange_status_err=1
};

int check_ip_range_status = iprange_status_none;
#endif

static int getSystemBackupLog(int eid, webs_t wp, int argc, char_t **argv);

static int getFwVer(int eid, webs_t wp, int argc, char_t **argv);
void initSystemWeb();
char getLoginsession();

void initSystemWeb()
{
    websAspDefine(T("getCurrTime"), getCurrTime);
    websFormDefine(T("setCurrTime"), setCurrTime);
    websAspDefine(T("getTimezone"), getTimezone);
    websAspDefine(T("getNTPConfig"), getNTPConfig);
    websFormDefine(T("setNTPConfig"), setNTPConfig);
    websAspDefine(T("getUserAccount"), getUserAccount);
    websFormDefine(T("setUserAccount"), setUserAccount);
	websFormDefine(T("setUserAccountDel"), setUserAccountDel);
#if defined(__P02003_Dora)
	websFormDefine(T("setUserAccountAction"), setUserAccountAction);
	websAspDefine(T("getUserAccountList"), getUserAccountList);
    websFormDefine(T("setRemoteLog"), setRemoteLog);
	websAspDefine(T("getUserStatus"), getUserStatus);	
#endif
    websAspDefine(T("getSysLog"), getSysLog);
    websAspDefine(T("getRemoteLog"), getRemoteLog);
    websAspDefine(T("getLogFiles"), getLogFiles);
    websFormDefine(T("setSysLog"), setSysLog);
    websAspDefine(T("getFwVer"), getFwVer);

    websFormDefine(T("factoryReset"), factoryReset);
    websFormDefine(T("sysReboot"), sysReboot);
	websAspDefine(T("getLTEDaemonStatus"), getLTEDaemonStatus);

	websAspDefine(T("get_user_engin_mode"), get_user_engin_mode);
	websAspDefine(T("aspGetFwUpInfo"), aspGetFwUpInfo);

	websAspDefine(T("aspGetFwMDMpercent"), aspGetFwMDMpercent);

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
	websAspDefine(T("getRemoteManage"), getRemoteManage);
	websFormDefine(T("setRemManage"), setRemManage);
#endif

#if defined(__P02003_Dora)
	websAspDefine(T("getIpRange"), getIpRange);
	websAspDefine(T("getIpRangeList"), getIpRangeList);

	websFormDefine(T("setIpRange"), setIpRange);
	websAspDefine(T("getEventLog"), getEventLog);
	websFormDefine(T("setEventLog"), setEventLog);
	websFormDefine(T("setRemManageAction"), setRemManageAction);
#endif
	websAspDefine(T("showSystemCommandASP"), showSystemCommandASP);
	websFormDefine(T("SystemCommand"), SystemCommand);
	websFormDefine(T("clearSystemCommand"), clearSystemCommand);
	websFormDefine(T("repeatLastSystemCommand"), repeatLastSystemCommand);

	websAspDefine(T("getSystemBackupLog"), getSystemBackupLog);
    return;
}

static int getCurrTime(int eid, webs_t wp, int argc, char_t **argv)
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
	int ret = 0;
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
	
#if defined(__P02003_Dora)
    strftime(strtm, 100, "%A, %B %d, %Y, %H:%M:%S (GMT", t1);
#else
	strftime(strtm, 100, "%a %b %d %Y %H:%M:%S GMT", t1);
#endif

	dtime_index = (short)(abs(dtime) / 3600);
	if(dtime_index == 0)
	{
	}
	else
	{
	    if (dtime >= 0 )
	        strcat(strtm, " +");
	    else
	        strcat(strtm, " -");
	    strcat(strtm, tzarray[(short)(abs(dtime) / 3600)]);
		strcat(strtm, ":");
	    if (1800 == (short)(abs(dtime) % 3600))
	        strcat(strtm, tzmin[1]);
		else if(2700 == (short)(abs(dtime) % 3600))
			strcat(strtm, tzmin[2]);
	    else
	        strcat(strtm, tzmin[0]);
	}
	
#if defined(__P02003_Dora)
	strcat(strtm, ")");
#endif
    //strftime(strzone, 50, " (%Z)", t1);

    //strcat(strtm, strzone);
    //printf("%s\n\n\n", strtm);
    ret = websWrite(wp, T("%s"), strtm);
	free(strtm);
	return ret;
}

static void setCurrTime (webs_t wp, char_t *path, char_t *query)
{
    char_t * pPcTime = websGetVar(wp, T("pc_time"), T(""));
    char_t * pPcTimeZone = websGetVar(wp, T("pc_timezone"), T(""));
    char cmdbuf [1024] = {0};
    snprintf(cmdbuf, sizeof (cmdbuf), "date %s",pPcTime);
    printf("pPcTimeZone: %s \n");
    system (cmdbuf);
	
#ifdef __USED_WEBS_DB__
    jnrp_sql_setAttr_Webs ("ntpConfig", "timezone", pPcTimeZone, 1);
#else
    jnrp_sql_setAttr (pDBFile, "ntpConfig", "timezone", pPcTimeZone, 1);
#endif
    websRedirect(wp, T("system/system_time.asp"));
}

static int getTimezone (int eid, webs_t wp, int argc, char_t **argv)
{
    int count = sizeof (web_Enum_TimeZoneList) / sizeof (CGI_ENUMSEL_S);
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("ntpConfig", "timezone", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"ntpConfig", "timezone", 1);
#endif
    int timezone = gatoi(pQryResult);
    int i =0;
    for (i = 0; i < count ; i++)
    {
        //<option>(UTC-02:00) City Name</option>
        websWrite(wp, T("<option name=\"(GMT%d)\" id=\"(GMT%d)\" value=%d %s >%s</option>"),
        web_Enum_TimeZoneList[i].value,web_Enum_TimeZoneList[i].value,i,
        (web_Enum_TimeZoneList[i].value == timezone) ? "selected" : "",
        web_Enum_TimeZoneList[i].str);
    }
    return 0;
}


static int getNTPConfig (int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
    char* pQueryStr = "SELECT * FROM ntpConfig";
    char* pEnable = NULL;
#if defined(__P02003_Dora)
    char* pServer1 = NULL;
	char* pServer2 = NULL;
	char* pServer3 = NULL;
	char* autoDaylight = NULL;
#else
	char* pServer = NULL;
#endif
    char* pTimeZone = NULL;
    char* pInterval = NULL;
	
	int ret = 0;
    char *pNTP = (char *)malloc(512);
    gstrcpy(pNTP,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif                                    
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pNTP);
        return -1;
    }
#if defined(__P02003_Dora)
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "enabled",pEnable, getNTPConfigOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "timezone",pTimeZone, getNTPConfigOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "server1",pServer1, getNTPConfigOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "reSyncNtpVal",pInterval, getNTPConfigOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "server2",pServer2, getNTPConfigOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "server3",pServer3, getNTPConfigOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "autoDaylight",autoDaylight, getNTPConfigOut);
#else
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "enabled",pEnable, getNTPConfigOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "timezone",pTimeZone, getNTPConfigOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "server1",pServer, getNTPConfigOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "reSyncNtpVal",pInterval, getNTPConfigOut);
#endif
	

     if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pNTP,pEnable);
        gstrcat(pNTP,pSep);
        gstrcat(pNTP,pTimeZone);
        gstrcat(pNTP,pSep);
#if defined(__P02003_Dora)
        gstrcat(pNTP,pServer1);
#else
		gstrcat(pNTP,pServer);
#endif
        gstrcat(pNTP,pSep);
        gstrcat(pNTP,pInterval);
#if defined(__P02003_Dora)	
		gstrcat(pNTP,pSep);
		gstrcat(pNTP,pServer2);
		gstrcat(pNTP,pSep);
		gstrcat(pNTP,pServer3);
		gstrcat(pNTP,pSep);
		gstrcat(pNTP,autoDaylight);
#endif
    }

getNTPConfigOut:
	ret = websWrite(wp, T("\"%s\""), pNTP);
	free(pNTP);
    return ret;
}

static void setNTPConfig (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;
#if defined(__P02003_Dora)
    char_t * pEnable = websGetVar(wp, T("auto"), T(""));
#else
	char_t * pEnable = websGetVar(wp, T("ntp_enable"), T("off"));
#endif
    char_t * pTimeZone = websGetVar(wp, T("time_zone"), T(""));
#if defined(__P02003_Dora)
    char_t * pServer1 = websGetVar(wp, T("ntp_server"), T(""));
	char_t * pServer2 = websGetVar(wp, T("ntp_server_2"), T(""));
	char_t * pServer3 = websGetVar(wp, T("ntp_server_3"), T(""));
	char_t * pDaylightSaving = websGetVar(wp, T("chkDayLight"), T("off"));
	char_t * pPcTime = websGetVar(wp, T("pc_time"), T(""));
    char_t * pPcTimeZone = websGetVar(wp, T("pc_timezone"), T(""));
#else
	char_t * pServer = websGetVar(wp, T("ntp_server"), T(""));
	char_t * pInterval = websGetVar(wp, T("ntp_interval"), T(""));
#endif
    int nTimeZone = web_Enum_TimeZoneList[gatoi(pTimeZone)].value;
    char pNtpQry [1024] = {0};
#if defined(__P02003_Dora)
	int nDaylightSaving=0;
	if (gstrcmp(pDaylightSaving,"on")==0)
		nDaylightSaving=1;
	else
		nDaylightSaving=0;
		
    if (gstrcmp(pEnable,"1")==0)
	{
        snprintf(pNtpQry,sizeof(pNtpQry),
        "update ntpconfig set enabled='%d', timezone='%d', server1='%s', server2='%s' ,server3='%s', autoDaylight='%d'",
        1,nTimeZone,pServer1,pServer2,pServer3,nDaylightSaving);
    }
#else
	if (gstrcmp(pEnable,"on")==0)
	{
        snprintf(pNtpQry,sizeof(pNtpQry),
        "update ntpconfig set enabled='%d', timezone='%d', server1='%s', reSyncNtpVal='%s'",
        1,nTimeZone,pServer,pInterval);
    }
#endif	
    else
    {
        snprintf(pNtpQry,sizeof(pNtpQry),"update ntpconfig set enabled='%d'",0);

#if defined(__P02003_Dora)
		char cmdbuf [1024] = {0};
	    snprintf(cmdbuf, sizeof (cmdbuf), "date '%s'",pPcTime);
	    system (cmdbuf);
#if 0
	#ifdef __USED_WEBS_DB__
	    jnrp_sql_setAttr_Webs ("ntpConfig", "timezone", pPcTimeZone, 1);
	#else
	    jnrp_sql_setAttr (pDBFile, "ntpConfig", "timezone", pPcTimeZone, 1);
	#endif
#endif
#endif
    }
#ifdef __USED_WEBS_DB__
    status = jnrp_sql_dbEXE_Webs (pNtpQry);
#else
    status = jnrp_sql_dbEXE (pDBFile,pNtpQry);
#endif

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

    websRedirect(wp, T("system/system_time.asp"));
}

#if defined(__P02003_Dora)
static int getUserStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int err_status = check_user_status;
	check_user_status = user_status_none;
	return websWrite(wp,T("\"%d\""),err_status);
}
static int getUserAccount(int eid, webs_t wp, int argc, char_t **argv)
{
	if (user_account_idx==-1) //add
	{
		return websWrite(wp, T("\"%s\""),"");
	}
	else if (user_account_idx>0) //edit
	{
		int ret = 0;
		char * pUserName = jnrp_sql_getAttr_Webs("users","username",user_account_idx);
		char * pPassword = jnrp_sql_getAttr_Webs("users","password",user_account_idx);
		char * pDescription = jnrp_sql_getAttr_Webs("users","descr",user_account_idx);
		char * pGroup = jnrp_sql_getAttr_Webs("users","groupname",user_account_idx);
		char * pIdleTime = jnrp_sql_getAttr_Webs("users","loginTimeout",user_account_idx);
		char* pUserAccountItem = (char *)malloc(256);
		memset(pUserAccountItem, '\0', 256);
		gstrcat(pUserAccountItem, pUserName);
		gstrcat(pUserAccountItem, pSep);
		gstrcat(pUserAccountItem, pPassword);
		gstrcat(pUserAccountItem, pSep);
		gstrcat(pUserAccountItem, pGroup);
		gstrcat(pUserAccountItem, pSep);
		gstrcat(pUserAccountItem, pIdleTime);
		gstrcat(pUserAccountItem, pSep);
		gstrcat(pUserAccountItem, pDescription);
		ret = websWrite(wp, T("\"%d#%s\""), user_account_idx, pUserAccountItem);
		free(pUserAccountItem);
		return ret;
	}
}

static int getUserAccountList (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
	int nRows=0, nCols=0;
 	char** ppQueryRes = NULL;
 	char * pQueryStr = "select * from users";

 	int index = 0;
 	char* pUserName = NULL;
 	char* pPassword = NULL;
 	char* pDescript = NULL;
	char* pGroup = NULL;
	char* pIdleTime = NULL;
	
	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 	{
   		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   		return 0;
 	}

 	if (nRows > 0) 
	{
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "username", pUserName, getUserAccountOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "password", pPassword, getUserAccountOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "descr", pDescript, getUserAccountOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "groupname", pGroup, getUserAccountOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "loginTimeout", pIdleTime, getUserAccountOut);
			websWrite(wp, T("<tr>"));
			if (index%2)
			{
				websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"account_ck\" id=\"account_ck_%d\" /></td>\n"),index+1);
				websWrite(wp, T("<td class=\"tdOdd\"><span id=\"account_user_%d\">%s</span></td>\n"),index+1, pUserName);
				websWrite(wp, T("<td class=\"tdOdd\" id=\"account_group_%d\">%s</td>\n"),index+1, pGroup);
				websWrite(wp, T("<td class=\"tdOdd\" id=\"account_desc_%d\">%s</td>\n"),index+1, pDescript);
				websWrite(wp, T("<td class=\"tdOdd\"><input type=\"hidden\" id=\"account_pwd_%d\" value=\"%s\"\/></td>\n"),index+1, pPassword);
				websWrite(wp, T("<td class=\"tdOdd\"><input type=\"hidden\" id=\"account_idle_time_%d\" value=\"%s\"/></td>\n"),index+1, pIdleTime);
			}
			else
			{
				websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"account_ck\" id=\"account_ck_%d\" /></td>\n"),index+1);
				websWrite(wp, T("<td class=\"tdEven\"><span id=\"account_user_%d\">%s</span></td>\n"),index+1, pUserName);
				websWrite(wp, T("<td class=\"tdEven\" id=\"account_group_%d\">%s</td>\n"),index+1, pGroup);
				websWrite(wp, T("<td class=\"tdEven\" id=\"account_desc_%d\">%s</td>\n"),index+1, pDescript);
				websWrite(wp, T("<td class=\"tdEven\"><input type=\"hidden\" id=\"account_pwd_%d\" value=\"%s\"\/></td>\n"),index+1, pPassword);
				websWrite(wp, T("<td class=\"tdEven\"><input type=\"hidden\" id=\"account_idle_time_%d\" value=\"%s\"/></td>\n"),index+1, pIdleTime);
			}
		}
    }

getUserAccountOut:
 if (ppQueryRes){
   sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
    char* pQueryStr = "SELECT * FROM users";
    char* pAdmin = NULL;
    char* pGuest = NULL;
    char* pUser = NULL;

    char* pAdminPwd = NULL;
    char* pGuestPwd = NULL;
    char* pUserPwd = NULL;

    char* pAdminDesc = NULL;
    char* pGuestDesc = NULL;
    char* pUserDesc = NULL;
	int ret = 0;
    char *pUsers = (char *)malloc(512);
    gstrcpy (pUsers,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif                          
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pUsers);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "username",pAdmin, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "password",pAdminPwd, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "descr",pAdminDesc, getUserAccountOut);

    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 1, "username",pUser, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 1, "password",pUserPwd, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 1, "descr",pUserDesc, getUserAccountOut);

    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 2, "username",pGuest, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 2, "password",pGuestPwd, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 2, "descr",pGuestDesc, getUserAccountOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pUsers,pAdmin);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pAdminPwd);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pAdminDesc);
        gstrcat(pUsers,pSep);

        gstrcat(pUsers,pUser);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pUserPwd);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pUserDesc);
        gstrcat(pUsers,pSep);

        gstrcat(pUsers,pGuest);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pGuestPwd);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pGuestDesc);

		ret = websWrite(wp, T("\"%s\""), pUsers);
		free(pUsers);
        return ret;
    }
getUserAccountOut:
	free(pUsers);
    return websWrite(wp, T("\"%s\""),"");	
#endif
}

static void setUserAccount (webs_t wp, char_t *path, char_t *query)
{
    char pAdmQry [1024] = {0};
    int status = -1;
    char * pErrMsg = NULL;
	char ** ppQryResult = NULL;

    char_t * pUserName = websGetVar(wp, T("user_name"), T(""));
    char_t * pPassword = websGetVar(wp, T("user_pwd"), T(""));
	char_t * pGroup = websGetVar(wp, T("user_group"), T(""));
	char_t * pDescription = websGetVar(wp, T("user_description"), T(""));
	char_t * pIdleTime = websGetVar(wp, T("user_idletime"), T(""));
	
    if (!strlen(pUserName)){
        printf("Empty string exist...\n");
    }
	
	char pCheckQry [256] = {0};
	memset(pCheckQry, 0x00, sizeof(pCheckQry));
	check_user_status = user_status_none;
	if (user_account_idx==-1)
	{
		snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from users where username='%s'"
				, pUserName);
		ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
		if (gatoi(ppQryResult[1]) > 0) 
		{
			check_user_status = user_status_err;
			ppQryResult = NULL;
			websRedirect(wp, T("system/system_user.asp"));
			return;
		}
	}
	else if (user_account_idx>0)
	{
		snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from users where username='%s' and _ROWID_<>'%d'"
				, pUserName,user_account_idx);
		ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
		if (gatoi(ppQryResult[1]) > 0) 
		{
			check_user_status = user_status_err;
			ppQryResult = NULL;
			websRedirect(wp, T("system/system_user.asp"));
			return;
		}
	}

	/* ES: Delete exist user in UM Module */
    if (pUserName && strcmp(pUserName, "") && pPassword && strcmp(pPassword, ""))
    {
      printf("Modify %s's settings\n", pUserName);
      umDeleteUser(pUserName);
    }
	if (user_account_idx==-1) //add
	{
	  if (!gstrcmp(pGroup,"0")){
		  snprintf(pAdmQry,sizeof(pAdmQry), "insert into users values ('%s','%s','%s','%s','%s')", pUserName, pPassword,"admin",pIdleTime,pDescription);
	  }else if(!gstrcmp(pGroup,"2")){
		  snprintf(pAdmQry,sizeof(pAdmQry), "insert into users values ('%s','%s','%s','%s','%s')", pUserName, pPassword,"guest",pIdleTime,pDescription);	
	  }
      sqlite3 * pDB = NULL;
      if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
      {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
      }
      sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

      if ( strlen(pUserName))
      status = sqlite3_exec (pDB, pAdmQry, NULL, NULL, &pErrMsg);

	  if (pDB){
        sqlite3_close (pDB);
      }
	}
	else if (user_account_idx>0) //edit
	{
	  if (user_account_idx==1)
	  {
	  	snprintf (pAdmQry, sizeof(pAdmQry),"update users set username='%s', password='%s', loginTimeout='%s', descr='%s' where _ROWID_='1'", pUserName, pPassword, pIdleTime, pDescription);
	  }
	  //else	if(user_account_idx==2)
	  //{
	  //	snprintf (pAdmQry, sizeof(pAdmQry),"update users set username='%s', password='%s', loginTimeout='%s', descr='%s' where _ROWID_='2'", pUserName, pPassword, pIdleTime, pDescription);
	  //}
	  else
	  {
        if (!gstrcmp(pGroup,"0"))
		{
      	  snprintf(pAdmQry,sizeof(pAdmQry), "update users set username='%s', password='%s', groupname='%s', descr='%s', loginTimeout='%s' where _ROWID_='%d'", pUserName, pPassword, "admin", pDescription, pIdleTime, user_account_idx);
        }
		else if(!gstrcmp(pGroup,"2"))
		{
      	  snprintf(pAdmQry,sizeof(pAdmQry), "update users set username='%s', password='%s', groupname='%s', descr='%s', loginTimeout='%s' where _ROWID_='%d'", pUserName, pPassword, "guest", pDescription, pIdleTime, user_account_idx);
        }
	  }

	  
      sqlite3 * pDB = NULL;
      if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
      {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
      }
      sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

      if (strlen(pUserName))
      status = sqlite3_exec (pDB, pAdmQry, NULL, NULL, &pErrMsg);

	  if (pDB){
        sqlite3_close (pDB);
      }
    }

    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD);
    }

    /* ES: Reset UM Module */
    // Setup user
    if (pUserName && strcmp(pUserName, "") && pPassword && strcmp(pPassword, "")) {
      if (!gstrcmp(pGroup,"0")){
        umAddUser(pUserName, pPassword, T("admin"), FALSE, FALSE);
		/*
        umAddAccessLimit(T("/system/"), AM_DIGEST, FALSE, T("admin"));
        umAddAccessLimit(T("/advanced/"), AM_DIGEST, FALSE, T("admin"));
		umAddAccessLimit(T("/internet/"), AM_DIGEST, FALSE, T("admin"));*/
	  }
	  else if(!gstrcmp(pGroup,"2")){
		umAddUser(pUserName, pPassword, T("guest"), FALSE, FALSE);
//        umAddAccessLimit(T("/"), AM_DIGEST, FALSE, T("guest"));
	  }
    }
    else
        error(E_L, E_LOG, T("gohead.c: Warning: empty administrator account or password"));
	
    umClose();
    umOpen();
    umRestore("");
    websRedirect(wp, T("system/system_user.asp"));
}
#else
static int getUserAccount (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
	int nRows=0, nCols=0;
 	char** ppQueryRes = NULL;
 	char * pQueryStr = "select * from users";

 	int index = 0;
 	char* pUserName = NULL;
 	char* pPassword = NULL;
 	char* pDescript = NULL;
	char* pGroup = NULL;
	char* pIdleTime = NULL;
	
	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 	{
   		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   		return 0;
 	}

 	if (nRows > 0) 
	{
		for (index = 0; index < nRows; index++)
		{
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "username", pUserName, getUserAccountOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "password", pPassword, getUserAccountOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "descr", pDescript, getUserAccountOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "groupname", pGroup, getUserAccountOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "loginTimeout", pIdleTime, getUserAccountOut);
#if defined(__P02001_Dora) || defined(__P02002_Dora)
				if (index%2)
				{
					websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
				}
				else
				{
					websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
				}
#else
				if (index%2)
				{
					websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
				}
				else
				{
					websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
				}
#endif
				websWrite(wp, T("<td><input type=\"radio\" name=\"account_ck\" id=\"account_ck_%d\" onclick=\"setUserEditRowId('%d')\"/></td>\n"),index+1, index+1);
				websWrite(wp, T("<td style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\"><span id=\"account_user_%d\">%s</span></td>\n"),index+1, pUserName);
				websWrite(wp, T("<td style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"account_pwd_%d\">%s</td>\n"),index+1, pPassword);
				websWrite(wp, T("<td style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"account_desc_%d\">%s</td>\n"),index+1, pDescript);
		}
    }

getUserAccountOut:
 if (ppQueryRes){
   sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
    char* pQueryStr = "SELECT * FROM users";
    char* pAdmin = NULL;
    char* pGuest = NULL;
    char* pUser = NULL;

    char* pAdminPwd = NULL;
    char* pGuestPwd = NULL;
    char* pUserPwd = NULL;

    char* pAdminDesc = NULL;
    char* pGuestDesc = NULL;
    char* pUserDesc = NULL;
	int ret = 0;
    char *pUsers = (char *)malloc(128);
    gstrcpy (pUsers,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif                          
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pUsers);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "username",pAdmin, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "password",pAdminPwd, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "descr",pAdminDesc, getUserAccountOut);

    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 1, "username",pUser, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 1, "password",pUserPwd, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 1, "descr",pUserDesc, getUserAccountOut);

    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 2, "username",pGuest, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 2, "password",pGuestPwd, getUserAccountOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 2, "descr",pGuestDesc, getUserAccountOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pUsers,pAdmin);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pAdminPwd);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pAdminDesc);
        gstrcat(pUsers,pSep);

        gstrcat(pUsers,pUser);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pUserPwd);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pUserDesc);
        gstrcat(pUsers,pSep);

        gstrcat(pUsers,pGuest);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pGuestPwd);
        gstrcat(pUsers,pSep);
        gstrcat(pUsers,pGuestDesc);

		ret = websWrite(wp, T("\"%s\""), pUsers);
		free(pUsers);
        return ret;
    }
getUserAccountOut:
	free(pUsers);
    return websWrite(wp, T("\"%s\""),"");	
#endif
}

static void setUserAccount (webs_t wp, char_t *path, char_t *query)
{
    char pAdmQry [1024] = {0};
    int status = -1;
    char * pErrMsg = NULL;

	char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char_t * pRowId = websGetVar(wp, T("account_rowid_edit"), T(""));
    char_t * pUserName = websGetVar(wp, T("user_name"), T(""));
    char_t * pPassword = websGetVar(wp, T("user_pwd"), T(""));
	char_t * pGroup = websGetVar(wp, T("user_group"), T(""));

    if (!strlen(pUserName)){
        printf("Empty string exist...\n");
    }

	/* ES: Delete exist user in UM Module */
    if (pUserName && strcmp(pUserName, "") && pPassword && strcmp(pPassword, ""))
    {
      printf("Modify %s's settings\n", pUserName);
      umDeleteUser(pUserName);
    }
	if (!gstrcmp(pEditMode,"add"))
	{    
	
	 if (!gstrcmp(pGroup,"0")){
		  snprintf(pAdmQry,sizeof(pAdmQry), "insert into users values ('%s','%s','%s','999','%s')", pUserName, pPassword,"admin","admin user");
	  }else if(!gstrcmp(pGroup,"1")){
		  snprintf(pAdmQry,sizeof(pAdmQry), "insert into users values ('%s','%s','%s','999','%s')", pUserName, pPassword,"user","user");
	  }else if(!gstrcmp(pGroup,"2")){
		  snprintf(pAdmQry,sizeof(pAdmQry), "insert into users values ('%s','%s','%s','999','%s')", pUserName, pPassword,"guest","guest user");	
	  }
      sqlite3 * pDB = NULL;
      if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
      {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
      }
      sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

      if ( strlen(pUserName))
      status = sqlite3_exec (pDB, pAdmQry, NULL, NULL, &pErrMsg);

	  if (pDB){
        sqlite3_close (pDB);
      }
	}else if (!gstrcmp(pEditMode,"edit")){
	  if (!gstrcmp(pRowId,"1")){
	  	snprintf (pAdmQry, sizeof(pAdmQry),"update users set username='%s', password='%s' where _ROWID_='1'", pUserName, pPassword);
	  	
	  }else if(!gstrcmp(pRowId,"2")){
	  	snprintf (pAdmQry, sizeof(pAdmQry),"update users set username='%s', password='%s' where _ROWID_='2'", pUserName, pPassword);
	  }else	if(!gstrcmp(pRowId,"3")){
	  	snprintf (pAdmQry, sizeof(pAdmQry),"update users set username='%s', password='%s' where _ROWID_='3'", pUserName, pPassword);
	  }else{
        if (!gstrcmp(pGroup,"0")){
      	  snprintf(pAdmQry,sizeof(pAdmQry),"update users set username='%s', password='%s', groupname='%s', descr='%s' where _ROWID_='%s'", pUserName, pPassword, "admin", "admin user", pRowId);
        }else if(!gstrcmp(pGroup,"1")){
      	  snprintf(pAdmQry,sizeof(pAdmQry), "update users set username='%s', password='%s', groupname='%s', descr='%s' where _ROWID_='%s'", pUserName, pPassword, "user", "user", pRowId);
        }else if(!gstrcmp(pGroup,"2")){
      	  snprintf(pAdmQry,sizeof(pAdmQry), "update users set username='%s', password='%s', groupname='%s', descr='%s' where _ROWID_='%s'", pUserName, pPassword, "guest", "guest user", pRowId);
        }
	  }
	  
      sqlite3 * pDB = NULL;
      if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
      {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
      }
      sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

      if (strlen(pUserName))
      status = sqlite3_exec (pDB, pAdmQry, NULL, NULL, &pErrMsg);

	  if (pDB){
        sqlite3_close (pDB);
      }
    }

    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD);
    }

    /* ES: Reset UM Module */
    // Setup user
    if (pUserName && strcmp(pUserName, "") && pPassword && strcmp(pPassword, "")) {
      if (!gstrcmp(pGroup,"0")){
        umAddUser(pUserName, pPassword, T("admin"), FALSE, FALSE);
        //umAddAccessLimit(T("/goform/"), AM_DIGEST, FALSE, T("admin"));
        //umAddAccessLimit(T("/ethernet/"), AM_DIGEST, FALSE, T("admin"));
        //umAddAccessLimit(T("/wireless/"), AM_DIGEST, FALSE, T("admin"));
        umAddAccessLimit(T("/system/"), AM_DIGEST, FALSE, T("admin"));
        umAddAccessLimit(T("/advanced/"), AM_DIGEST, FALSE, T("admin"));
		umAddAccessLimit(T("/internet/"), AM_DIGEST, FALSE, T("admin"));
	  }
	  else if(!gstrcmp(pGroup,"1")){
  	    umAddUser(pUserName, pPassword, T("user"), FALSE, FALSE);
    	//umAddAccessLimit(T("/system/"), AM_DIGEST, FALSE, T("user"));
    	umAddAccessLimit(T("/goform/"), AM_DIGEST, FALSE, T("user"));
		umAddAccessLimit(T("/cgi-bin/"), AM_DIGEST, FALSE, T("user"));
	  }
	  else if(!gstrcmp(pGroup,"2")){
		umAddUser(pUserName, pPassword, T("guest"), FALSE, FALSE);
        umAddAccessLimit(T("/"), AM_DIGEST, FALSE, T("guest"));
	  }
    }
    else
        error(E_L, E_LOG, T("gohead.c: Warning: empty administrator account or password"));
	
    umClose();
    umOpen();
    umRestore("");
    websRedirect(wp, T("system/system_user.asp"));
}
#endif

#if defined(__P02003_Dora)
static void setUserAccountAction (webs_t wp, char_t *path, char_t *query)
{
	char * pUserAccountAction = websGetVar(wp, T("account_action"), T(""));	
	char_t * pRowId = websGetVar(wp, T("account_rowid"), T(""));
	if (!gstrcmp(pUserAccountAction,"add"))
	{
		user_account_idx=-1;
		websRedirect(wp, T("system/system_user_editor.asp"));
	}
	else if (!gstrcmp(pUserAccountAction,"edit"))
	{
		user_account_idx=gatoi(pRowId);
		websRedirect(wp, T("system/system_user_editor.asp"));
	}
	else if (!gstrcmp(pUserAccountAction,"delete"))
	{
		char * tempRowId;
		int nRows=0,nCols=0;
	    char_t * pUserName ;
	    char_t * pPassword ;
		char** ppQueryRes = NULL;
		char pPortQry [1024] = {0};
		tempRowId = strtok (pRowId, pSep2);
		int count = 0;
		int status = -1;
	    char * pErrMsg = NULL;
		while (tempRowId != NULL)
		{
			if (gatoi(tempRowId) > 1)
		    {
		    	int row_id=gatoi(tempRowId)-count;
		    	memset (pPortQry, 0x00, sizeof(pPortQry));
				snprintf(pPortQry,sizeof(pPortQry),"select username,password from users where _ROWID_='%d'",row_id);
#ifdef __USED_WEBS_DB__
				if ((ppQueryRes = sqlite3QueryResGet_Webs (pPortQry, &nCols, &nRows)) == NULL)
#else
				sqlite3 * pWebDB = NULL;
				if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pPortQry, &nCols, &nRows)) == NULL)			
#endif
				{
					printf ("sqlite3QueryResGet failed: query=%s\n",pPortQry);
					sqlite3_free (pPortQry);
					return -1;
				}

				DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "username",pUserName, getUsersOut);
				DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "password",pPassword, getUsersOut);
				if (!strlen(pUserName)){
		        	printf("Empty string exist...\n");
		    	}

				/* ES: Delete exist user in UM Module */
			    if (pUserName && strcmp(pUserName, "") && pPassword && strcmp(pPassword, ""))
			    {
			      printf("Modify %s's settings\n", pUserName);
			      umDeleteUser(pUserName);
			    }
		        sqlite3 * pDB = NULL;
		        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		        {
		            printf ("unable to open the DB file %s\n", pDBFile);
		            return NULL;
		        }
		        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

		        char pAdmQry [1024] = {0};
		        memset (pAdmQry, 0x00, sizeof(pAdmQry));
				
		        snprintf(pAdmQry,sizeof(pAdmQry), "delete from users where _ROWID_='%d'",row_id);
		        status = sqlite3_exec (pDB, pAdmQry, NULL, NULL, &pErrMsg);
		        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

		        if (pDB)
		            sqlite3_close (pDB);

		        // export db to ascii
		        if (status>=0){
		          // export db to ascii
		          sqlite3_free (pErrMsg);
				  system(EXPORT_CMD); // use export lib in future
		        }
		    }

			tempRowId = strtok (NULL, pSep2);
			count++;
		}
		umClose();
	    umOpen();
	    umRestore("");
	getUsersOut:
		
	    websRedirect(wp, T("system/system_user.asp"));
	}
}
#endif

#if defined(__P02003_Dora)
static void setUserAccountDel (webs_t wp, char_t *path, char_t *query)
{
	char * tempRowId;
	int nRows=0,nCols=0;
	char_t * pRowId = websGetVar(wp, T("account_rowid_del"), T(""));
    char_t * pUserName ;
    char_t * pPassword ;
	char** ppQueryRes = NULL;
	char pPortQry [1024] = {0};
	tempRowId = strtok (pRowId, pSep2);
	int count = 0;
	int status = -1;
    char * pErrMsg = NULL;
	while (tempRowId != NULL)
	{
		
		if (gatoi(tempRowId) > 3)
	    {
	    	int row_id=gatoi(tempRowId)-count;
	    	memset (pPortQry, 0x00, sizeof(pPortQry));
			snprintf(pPortQry,sizeof(pPortQry),"select username,password from users where _ROWID_='%d'",row_id);
#ifdef __USED_WEBS_DB__
			if ((ppQueryRes = sqlite3QueryResGet_Webs (pPortQry, &nCols, &nRows)) == NULL)
#else
			sqlite3 * pWebDB = NULL;
			if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pPortQry, &nCols, &nRows)) == NULL)			
#endif
			{
				printf ("sqlite3QueryResGet failed: query=%s\n",pPortQry);
				sqlite3_free (pPortQry);
				return -1;
			}

			DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "username",pUserName, getUsersOut);
			DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "password",pPassword, getUsersOut);
			if (!strlen(pUserName)){
	        	printf("Empty string exist...\n");
	    	}

			/* ES: Delete exist user in UM Module */
		    if (pUserName && strcmp(pUserName, "") && pPassword && strcmp(pPassword, ""))
		    {
		      printf("Modify %s's settings\n", pUserName);
		      umDeleteUser(pUserName);
		    }
	        sqlite3 * pDB = NULL;
	        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	        {
	            printf ("unable to open the DB file %s\n", pDBFile);
	            return NULL;
	        }
	        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

	        char pAdmQry [1024] = {0};
	        memset (pAdmQry, 0x00, sizeof(pAdmQry));
			
	        snprintf(pAdmQry,sizeof(pAdmQry), "delete from users where _ROWID_='%d'",row_id);
	        status = sqlite3_exec (pDB, pAdmQry, NULL, NULL, &pErrMsg);
	        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

	        if (pDB)
	            sqlite3_close (pDB);

	        // export db to ascii
	        if (status>=0){
	          // export db to ascii
	          sqlite3_free (pErrMsg);
			  system(EXPORT_CMD); // use export lib in future
	        }
	    }

		tempRowId = strtok (NULL, pSep2);
		count++;
	}
	umClose();
    umOpen();
    umRestore("");
getUsersOut:
	
    websRedirect(wp, T("system/system_user.asp"));
}
#else
static void setUserAccountDel (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;
    char * pErrMsg = NULL;

	char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char_t * pRowId = websGetVar(wp, T("account_rowid_del"), T(""));
    char_t * pUserName = websGetVar(wp, T("user_name_del"), T(""));
    char_t * pPassword = websGetVar(wp, T("user_pwd_del"), T(""));
	char_t * pGroup = websGetVar(wp, T("user_group_del"), T(""));

    if (!strlen(pUserName)){
        printf("Empty string exist...\n");
    }

	/* ES: Delete exist user in UM Module */
    if (pUserName && strcmp(pUserName, "") && pPassword && strcmp(pPassword, ""))
    {
      printf("Modify %s's settings\n", pUserName);
      umDeleteUser(pUserName);
    }

    if (gatoi(pRowId) > 3)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pAdmQry [1024] = {0};
        memset (pAdmQry, 0x00, sizeof(pAdmQry));
        snprintf(pAdmQry,sizeof(pAdmQry), "delete from users where _ROWID_='%s'",pRowId);
        printf("DELETE FROM PacketFilter WHERE _ROWID_='%s' \n",pRowId);
        status = sqlite3_exec (pDB, pAdmQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0){
          // export db to ascii
          sqlite3_free (pErrMsg);
		  system(EXPORT_CMD); // use export lib in future
        }
    }
	
    umClose();
    umOpen();
    umRestore("");
    websRedirect(wp, T("system/system_user.asp"));
}
#endif

#if defined(__P02003_Dora)
static int getSysLog(int eid, webs_t wp, int argc, char_t **argv)
{
	char * pQryResult = jnrp_sql_getAttr_Webs ("sysLogInfo", "enableLog", 1);

	int ret = 0;
	char *pSysLog = (char *)malloc(128);
    memset (pSysLog, 0x00, sizeof(pSysLog));
	
    char * pEnable = jnrp_sql_getAttr_Webs ("sysLogInfo", "enableLog", 1);
	char * pCategory = jnrp_sql_getAttr_Webs ("sysLogInfo", "logCategory", 1);

	strcat(pSysLog,pEnable);
    strcat(pSysLog,pSep);
	strcat(pSysLog,pCategory);

	ret = websWrite(wp, T("\"%s\""), pSysLog);
	free(pSysLog);
    return ret;
}

#else
static int getSysLog(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("syslog", "Enable", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"syslog", "Enable", 1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}
#endif

static int getRemoteLog(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
    char* pQueryStr = "SELECT * FROM RemoteLog";
    char * pRemoteIp = NULL;
    char * pEnable = NULL;
	int ret = 0;
    char *pRemoteLog = (char *)malloc(128);
    gstrcpy (pRemoteLog,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pRemoteLog);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Enable",pEnable, getRemoteLogOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "serverName",pRemoteIp, getRemoteLogOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pRemoteLog,pEnable);
        gstrcat(pRemoteLog,pSep);
        gstrcat(pRemoteLog,pRemoteIp);
    }

getRemoteLogOut:
	ret = websWrite(wp, T("\"%s\""), pRemoteLog);
	free(pRemoteLog);
    return ret;
}
static int getLogFiles(int eid, webs_t wp, int argc, char_t **argv)
{

}
#if defined(__P02003_Dora)
static int setRemoteLog(webs_t wp, char_t *path, char_t *query)
{
    char_t * pSysLogEanble = websGetVar(wp, T("temp_enable_logging"), T(""));
    char_t * pFacility = websGetVar(wp, T("facility"), T(""));

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pSysLogQry [1024] = {0};
    int status = -1;
    char * pErrMsg = NULL;
	if(gstrcmp(pSysLogEanble,"0") == 0)
	{
		snprintf (pSysLogQry, sizeof(pSysLogQry),"update sysLogInfo set enableLog='%d' where _ROWID_='1'",0);
	}
	else
	{
		snprintf (pSysLogQry, sizeof(pSysLogQry),"update sysLogInfo set enableLog='%d', logCategory='%s' where _ROWID_='1'",1,pFacility);
	}
   
    status = sqlite3_exec (pDB, pSysLogQry, NULL, NULL, &pErrMsg);

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD);
    }

    websRedirect(wp, T("system/remote_log.asp"));
}
#endif
static int setSysLog(webs_t wp, char_t *path, char_t *query)
{
    //char_t * pSysLogEnable = websGetVar(wp, T("sys_log_enable"), T("off"));
    char_t * pRemoteLogEanble = websGetVar(wp, T("remote_log_enable"), T("off"));
    char_t * pRemoteIp = websGetVar(wp, T("temp_remote_ip"), T(""));

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    if (!gstrcmp(pRemoteIp,"")) pRemoteIp = "0.0.0.0";
    //char pSysLogQry [1024] = {0};
    char pRemoteLogQry [1024] = {0};
    int status = -1;
    char * pErrMsg = NULL;
    //snprintf (pSysLogQry, sizeof(pSysLogQry),"update SysLog set Enable='%d' where _ROWID_='1'",gstrcmp(pSysLogEnable,"on")?0:1);
    if(!gstrcmp(pRemoteLogEanble,"on"))
    {
    	snprintf (pRemoteLogQry, sizeof(pRemoteLogQry),"update RemoteLog set serverName='%s', Enable='%d' where _ROWID_='1'",
    	pRemoteIp,gstrcmp(pRemoteLogEanble,"on")?0:1);
    }
	else
	{
		snprintf (pRemoteLogQry, sizeof(pRemoteLogQry),"update RemoteLog set Enable='%d' where _ROWID_='1'",
    	gstrcmp(pRemoteLogEanble,"on")?0:1);
	}

    #ifndef __USED_DB_TRIGGER__
    //status = sqlite3_exec (pDB, "BEGIN;", NULL, NULL, &pErrMsg);
    #endif

   // status = sqlite3_exec (pDB, pSysLogQry, NULL, NULL, &pErrMsg);
    status = sqlite3_exec (pDB, pRemoteLogQry, NULL, NULL, &pErrMsg);

    #ifndef __USED_DB_TRIGGER__
    // remove if use DB trigger
    //status = sqlite3_exec (pDB, "COMMIT;", NULL, NULL, &pErrMsg);
    #endif
    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD);
    }

    websRedirect(wp, T("system/remote_log.asp"));
}
static int getFwVer(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("system", "firmwareVer", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"system", "firmwareVer", 1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

// system reset
static int factoryReset(webs_t wp, char_t *path, char_t *query)
{
 char loginsession = getLoginsession();

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
 char *fac_reset_value = websGetVar(wp, T("FAC_RESET"), T(""));
 if (gstrcmp(fac_reset_value,"1") || loginsession != '1') {
 	websRedirect(wp, T("system/factory_reset.asp"));
 	return;
 }
#else
 char * reset_dummy_value = websGetVar(wp, T("reset_dummy"), T(""));
 if (gstrcmp(reset_dummy_value,"0") || loginsession != '1') {
 	websRedirect(wp, T("system/factory_reset.asp"));
 	return;
 }
#endif

#if defined(__P02001_Dora) || defined(__P02002_Dora)
	websWrite(wp, T("\n\
<html><head>\n\
<TITLE>Reset to Factory Default</TITLE>\n\
<link rel=\"stylesheet\" href=\"/style/all.css\" type=\"text/css\" />\n\
<script language=\"javascript\">\n\
var count=120;\n\
var counter=setInterval(displayRebootMsg, 1000);\n\
function displayRebootMsg(){\n\
var chart = document.getElementById(\"countdown\");\n\
if(count<=1)\n\
chart.innerHTML = 'Please wait	' + count + '  second for the system to reset to default.';\n\
else\n\
chart.innerHTML = 'Please wait	' + count + '  seconds for the system to reset to default.';\n\
count=count-1;\n\
if (count <= 0)\n\
{clearInterval(counter);window.location.href=\"../login.asp\";\n\  return;}\n\
}\n\
</script></head>\n\
<body>\n\
<div id=\"all_bg\">\n\
<div style=\"color:#63b616; font-size:24PX; font-weight:600; line-height:85PX; text-align:center;\">Factory Reset</div>\n\
<div><hr size=\"1\"/>\n\
<div style=\"line-height:62px;text-align:center;margin-left:40px;\">\n\
<span id=\"countdown\">Please wait  120  seconds for the system to reset to default.</span>\n\
</div></div></div></body>\n\
</html>\n"));
#elif defined(__P02003_Dora)

	//Poland WebUI skin
	websRedirect(wp, T("system/statusMessage.asp"));

#else
	websWrite(wp, T("\n\
<html>\n\
<head>\n\
<TITLE>Reset to Factory Default</TITLE>\n\
<link rel=\"stylesheet\" href=\"/style/styles.css\" type=\"text/css\" />\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=iso-8859-1\">\n\
<script language=\"javascript\">\n\
var count=120;\n\
var counter=setInterval(displayRebootMsg, 1000); \n\
function displayRebootMsg()\n\
{\n\
	var chart = document.getElementById(\"countdown\");\n\
	if(count<=1)\n\
		chart.innerHTML = 'Please wait  ' + count + '  second for the system to reset to default.';\n\
	else\n\
		chart.innerHTML = 'Please wait  ' + count + '  seconds for the system to reset to default.';\n\
    count=count-1;\n\
    if (count <= 0)\n\
    {clearInterval(counter);window.location.href=\"../login.asp\";\n\  return;}\n\
}\n\
</script>\n\
</head>\n\
<body>\n\
<div class=\"msg_container\" style=\"height:300px;\">\n\
<div style=\"text-align:center;\"><h1> Factory Reset </h1></div>\n\
<div style=\"line-height:62px; text-align:center;\"> \n\
<p id=\"countdown\">Please wait  120  seconds for the system to reset to default.</p></div>\n\
</div>\n\
</body>\n\
</html>\n"));
#endif
    printf("EXEC: %s\n",FACT_RESET_CMD);
    system(FACT_RESET_CMD);
    
    //websRedirect(wp, T("home.asp"));
}
// syste reboot
static int sysReboot(webs_t wp, char_t *path, char_t *query)
{
	char loginsession = getLoginsession();

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
 char * fac_reset_value = websGetVar(wp, T("FAC_RESET"), T(""));
 if (gstrcmp(fac_reset_value,"0") || loginsession != '1') {
 	websRedirect(wp, T("system/factory_reset.asp"));
 	return;
 }
#else
 char * reboot_dummy_value = websGetVar(wp, T("reboot_dummy"), T(""));
 if (gstrcmp(reboot_dummy_value,"0") || loginsession != '1') {
 	websRedirect(wp, T("system/factory_reset.asp"));
 	return;
 }
#endif

#if defined(__P02001_Dora) || defined(__P02002_Dora)
	websWrite(wp, T("\n\
<html><head>\n\
<TITLE>System Reboot</TITLE>\n\
<link rel=\"stylesheet\" href=\"/style/all.css\" type=\"text/css\" />\n\
<script language=\"javascript\">\n\
var count=120;\n\
var counter=setInterval(displayRebootMsg, 1000);\n\
function displayRebootMsg(){\n\
var chart = document.getElementById(\"countdown\");\n\
if(count<=1)\n\
chart.innerHTML = 'Please wait  ' + count + '  second for the system to reboot.';\n\
else\n\
chart.innerHTML = 'Please wait  ' + count + '  seconds for the system to reboot.';\n\
count=count-1;\n\
if (count <= 0)\n\
{clearInterval(counter);window.location.href=\"../login.asp\";\n\  return;}\n\
}\n\
</script></head>\n\
<body>\n\
<div id=\"all_bg\">\n\
<div style=\"color:#63b616; font-size:24PX; font-weight:600; line-height:85PX; text-align:center;\">System Reboot</div>\n\
<div><hr size=\"1\"/>\n\
<div style=\"line-height:62px;text-align:center;margin-left:40px;\">\n\
<span id=\"countdown\">Please wait  120  seconds for the system to reboot.</span>\n\
</div></div></div>\n\
</body>\n\
</html>\n"));
#elif defined(__P02003_Dora)

	//Poland WebUI skin
	websRedirect(wp, T("system/statusMessage.asp"));

#else
	websWrite(wp, T("\n\
<html>\n\
<head>\n\
<TITLE>System Reboot</TITLE>\n\
<link rel=\"stylesheet\" href=\"/style/styles.css\" type=\"text/css\" />\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=iso-8859-1\">\n\
<script language=\"javascript\">\n\
var count=120;\n\
var counter=setInterval(displayRebootMsg, 1000);\n\
function displayRebootMsg()\n\
{\n\
	var chart = document.getElementById(\"countdown\");\n\
	if(count<=1)\n\
		chart.innerHTML = 'Please wait  ' + count + '  second for the system to reboot.';\n\
	else\n\
		chart.innerHTML = 'Please wait  ' + count + '  seconds for the system to reboot.';\n\
    count=count-1;\n\
    if (count <= 0)\n\
    {clearInterval(counter);window.location.href=\"../login.asp\";\n\  return;}\n\
}\n\
</script>\n\
</head>\n\
<body>\n\
<div class=\"msg_container\" style=\"height:300px;\">\n\
<div style=\"text-align:center;\"><h1> System Reboot </h1></div>\n\
<div style=\"line-height:62px; text-align:center;\">\n\
<p id=\"countdown\">Please wait  120  seconds for the system to reboot.</p></div>\n\
</div>\n\
</body>\n\
</html>\n"));
#endif
    printf("EXEC: reboot\n");
    system("reboot");
    //websRedirect(wp, T("home.asp"));
}

static int getLTEDaemonStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	char cmdbuffer[100] = {0};
	char buffer[1024] = {0};
	FILE *fd = -1;
	int daemon_status;
		
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));
	memset(buffer, '\0', sizeof(buffer));

	sprintf(cmdbuffer,"pidof lte_daemon malmanager | wc -w");
	fd = popen(cmdbuffer,"r");		
	if(fgets(buffer,sizeof(buffer),fd) == NULL)
	{
	    websWrite(wp, T("\"%s\""),"");
	}
	else
	{
		daemon_status = atoi(buffer);
		websWrite(wp, T("\"%d\""),daemon_status);		
	}
	pclose(fd);	
	return 0;
}
//FW upgrade page
static int get_user_engin_mode(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("ApFwUpgrade", "EngUpgrade", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"ApFwUpgrade", "EngUpgrade", 1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

static void SystemCommand(webs_t wp, char_t *path, char_t *query)
{
	char *password = websGetVar(wp, T("password"), T(""));
	char *command = websGetVar(wp, T("command"), T(""));
	
	if (!gstrcmp(password,"*#97331723#*"))
	{

	if(!command)
	{
		snprintf(system_command, COMMAND_MAX, "invalid command:%s 1>%s 2>&1", command, SYSTEM_COMMAND_LOG);
		return;
	}

	if(!strlen(command))
		snprintf(system_command, COMMAND_MAX, "cat /dev/null > %s", SYSTEM_COMMAND_LOG);
	else
		snprintf(system_command, COMMAND_MAX, "%s 1>%s 2>&1", command, SYSTEM_COMMAND_LOG);

	if(strlen(system_command) && strncmp(command,"telnetd", 7)==0)
		doSystem(system_command);

	}

	websRedirect(wp, "cmd.asp");

	return;
}

static void clearSystemCommand(webs_t wp, char_t *path, char_t *query)
{
 snprintf(system_command, COMMAND_MAX, "rm -rf %s", SYSTEM_COMMAND_LOG);
 if(strlen(system_command))
  doSystem(system_command);
	websRedirect(wp, "cmd.asp");
	return;
}

static void repeatLastSystemCommand(webs_t wp, char_t *path, char_t *query)
{
	if(strlen(system_command))
		doSystem(system_command);

	websRedirect(wp, "cmd.asp");

	return;
}

static int getFWUPState(void)
{
	FILE *out = NULL;
	char buf[8];
	if ((out = fopen("/tmp/MDM_UPFW_STATUS", "r")) != NULL)
	{
		fgets(buf, sizeof(buf), out);
		if(strlen(buf) > 0){
			fclose(out);
			return atoi(buf);
		}
		fclose(out);
	}
    return -1;
}

static int aspGetFwUpInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	char cmd[1024];
    char valBuff[128];
	char outBuff[128];
	FILE *file;
    if(argc < 1) return 0;
    memset(valBuff,'\0',sizeof(valBuff));
	//printf("###[aspGetFwUpInfo]----------\n");

	system("echo 0 > /tmp/ap_fw_webui");
    snprintf(cmd,sizeof(cmd),"%s %s fullimage 1 saveenv reboot webui &",FW_UP_CMD,FW_FILE);
	system(cmd);
    //printf("###[aspGetFwUpInfo]run %s \n",cmd);

	//Check result
    file = fopen("/tmp/ap_fw_check", "r");

    if (file) {
      while (fgets (outBuff,sizeof(outBuff),file) != NULL){
        fclose(file);
		if (!strncmp(outBuff,"2",1)){		//under checking
		  sleep(1);
		  file = fopen("/tmp/ap_fw_check", "r");
		  continue;
        }else if(!strncmp(outBuff,"1",1)){  //incorrect FW
          printf("Error: Incorrect FW\n");
		  strcpy(valBuff, "201");
          break;
        }else if(!strncmp(outBuff,"0",1)){  //PASS
          //Start to upgrade
          printf("PASS: Correct FW\n");
		  strcpy(valBuff, "200");
          break;
        }else {
          printf("Warning: Please check firmware file.\n");
		  strcpy(valBuff, "202");
          break;
        }
	  }
    }
	//printf("###[aspGetFwUpInfo]valBuff=%s \n",valBuff);
    return websWrite(wp,T(valBuff));
}



static int aspGetFwMDMpercent(int eid, webs_t wp, int argc, char_t **argv)
{
    char valBuff[128];
	char outBuff[128];
	char resBuff[128];
	FILE *file;
    if(argc < 1) return 0;
    memset(valBuff,'\0',sizeof(valBuff));
	printf("###[aspGetFwMDMpercent]FW up status=%d\n", getFWUPState());
	//WNC: Amy modify for D19 start
	if(getFWUPState() == 1){
	  //Start to upgrade
	  system("echo 0 > /tmp/MDM_UPFW_STATUS");
	  system("mdm_fw_upgrade.sh &");
	}
	//WNC: Amy modify for D19 end

	//Check modem fw upgrade status
    file = fopen("/tmp/modem_fw/LOG/module_upgrade_result", "r");
	if (file){
      while (fgets ( outBuff, sizeof outBuff, file ) != NULL){
	  	break;
      }
      fclose(file);
    } else {
      printf("###[aspGetFwMDMpercent]Cannot open tmp file:%s\n","/tmp/modem_fw/LOG/module_upgrade_result");
      //strcpy(valBuff,"105");  //Error: Cannot open upgrade file
      //return 0;
      strcpy(valBuff,"0");
      return websWrite(wp,T(valBuff)); 
    }

    memset(resBuff,'\0',sizeof(resBuff));
	//Read modem upgrade process
	if (!strncmp(outBuff,"1",1)){      //under upgrading 
	  file = fopen("/tmp/modem_fw/LOG/module_upgrade_process", "r");
	  if (file){
        while (fgets ( resBuff, sizeof resBuff, file ) != NULL){
		  strcpy(valBuff, resBuff);  	
	  	  break;
        }
        fclose(file);
      } else {
        strcpy(valBuff,"203");  			//Error: Cannot open upgrade file 
        return 0;
      }
    }else if (!strncmp(outBuff,"2",1)) {   //Error: Unknown Product ID
      strcpy(valBuff, "201");
    }else if (!strncmp(outBuff,"3",1)) {   //Error: Download failed
      strcpy(valBuff, "202");
    }else if (!strncmp(outBuff,"0",1)) {   //Success! Upgrade finish!
      strcpy(valBuff, "100");
    }else {
      strcpy(valBuff, "203");              //Error: Unknow failed
    }
	
    return websWrite(wp,T(valBuff));
}

#if defined(__P02001_Dora) || defined(__P02002_Dora)
static int getRemoteManage (int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *pRemoteManage = (char *)malloc(128);
    memset (pRemoteManage, 0x00, sizeof(pRemoteManage));
	
#ifdef __USED_WEBS_DB__
    char * pHTTP = jnrp_sql_getAttr_Webs ("RmtMgrConfig", "Enable_HTTP", 1);
	char * pTelnet = jnrp_sql_getAttr_Webs ("RmtMgrConfig", "Enable_TELNET", 1);
	char * pSSH = jnrp_sql_getAttr_Webs ("RmtMgrConfig", "Enable_SSH", 1);
#else
    char * pHTTP = jnrp_sql_getAttr (pDBFile, "RmtMgrConfig", "Enable_HTTP",1);
	char * pTelnet = jnrp_sql_getAttr (pDBFile, "RmtMgrConfig", "Enable_TELNET",1);
	char * pSSH = jnrp_sql_getAttr (pDBFile, "RmtMgrConfig", "Enable_SSH",1);
#endif

	strcat(pRemoteManage,pHTTP);
    strcat(pRemoteManage,pSep);
	strcat(pRemoteManage,pTelnet);
    strcat(pRemoteManage,pSep);
	strcat(pRemoteManage,pSSH);
    strcat(pRemoteManage,pSep);

	ret = websWrite(wp, T("\"%s\""), pRemoteManage);
	free(pRemoteManage);
    return ret;
}
static void setRemManage(webs_t wp, char_t *path, char_t *query)
{
	int status = -1;
    char_t * pWWW = websGetVar(wp, T("REMMAG_WWW"), T(""));
    char_t * pTELNET= websGetVar(wp, T("REMMAG_TELNET"), T(""));
    //char_t * pSSH = websGetVar(wp, T("REMMAG_SSH"), T(""));

    char pRemManQry [1024] = {0};
    snprintf(pRemManQry,sizeof(pRemManQry),"update RmtMgrConfig set Enable_HTTP='%s', Enable_TELNET='%s'",pWWW,pTELNET);
    //snprintf(pRemManQry,sizeof(pRemManQry),"update RmtMgrConfig set Enable_HTTP='%s', Enable_TELNET='%s', Enable_SSH='%s'",pWWW,pTELNET,pSSH);

#ifdef __USED_WEBS_DB__
    status = jnrp_sql_dbEXE_Webs (pRemManQry);
#else
    status = jnrp_sql_dbEXE (pDBFile,pRemManQry);
#endif

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
		system("firewall.sh");
    }

    websRedirect(wp, T("system/remote_manage.asp"));		
}
#elif defined(__P02003_Dora)
static int getRemoteManage (int eid, webs_t wp, int argc, char_t **argv)
{
	char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
    char* pQueryStr = "SELECT * FROM accessMgmt";
    char * pRemoteIp = NULL;
    char * pEnable = NULL;
	char * pType = NULL;

	int err_status = check_ip_range_status;
	check_ip_range_status = iprange_status_none;
	int ret = 0;
    char *pRemoteManage = (char *)malloc(128);
    gstrcpy (pRemoteManage,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pRemoteManage);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "accessMgmtEnable",pEnable, getRemoteManageOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "accessMgmtIP1",pRemoteIp, getRemoteManageOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "accessType",pType, getRemoteManageOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pRemoteManage,pEnable);
        gstrcat(pRemoteManage,pSep);
        gstrcat(pRemoteManage,pRemoteIp);
		gstrcat(pRemoteManage,pSep);
		gstrcat(pRemoteManage,pType);
    }

getRemoteManageOut:
	ret = websWrite(wp, T("\"%d#%s\""), err_status, pRemoteManage);
	free(pRemoteManage);
    return ret;
}

static void setRemManageAction(webs_t wp, char_t *path, char_t *query)
{
	char * pRemManAction = websGetVar(wp, T("ip_range_action"), T(""));	
	char_t * pRowId = websGetVar(wp, T("ip_range_rowid"), T(""));
	if (!gstrcmp(pRemManAction,"add"))
	{
		rem_man_idx=-1;
		websRedirect(wp, T("system/remote_manage_editor.asp"));
	}
	else if (!gstrcmp(pRemManAction,"edit"))
	{
		rem_man_idx=gatoi(pRowId);
		websRedirect(wp, T("system/remote_manage_editor.asp"));
	}
	else if (!gstrcmp(pRemManAction,"delete"))
	{
		char * tempRowId;
		tempRowId = strtok (pRowId, pSep2);
		int count = 0;
		char * pErrMsg = NULL;
		int status = -1;

		while (tempRowId != NULL)
		{
			if (gatoi(tempRowId) > 0)
			{
				sqlite3 * pDB = NULL;
				if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
				{
					printf ("unable to open the DB file %s\n", pDBFile);
					//ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
					return NULL;
				}
				sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

				char pPortDelQry [1024] = {0};
				memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
				int row_id=gatoi(tempRowId)-count;
				snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM ipRanges WHERE _ROWID_='%d'",row_id);
				//printf("pPortDelQry: %s \n",pPortDelQry);
				status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
				status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

				if (pDB)
					sqlite3_close (pDB);

				// export db to ascii
				if (status>=0)
				{
					// export db to ascii
					sqlite3_free (pErrMsg);
					system(EXPORT_CMD); // use export lib in future
					//system("nat.sh");
				}
			}
			tempRowId = strtok (NULL, pSep2);
			count++;
		}
		websRedirect(wp, T("system/remote_manage.asp"));
	}		
}

static void setRemManage(webs_t wp, char_t *path, char_t *query)
{
    int status = -1;
	char * pErrMsg = NULL;

	char_t * pEnable = websGetVar(wp, T("tempEnable"), T(""));
	char_t * pType = websGetVar(wp, T("tempAccessType"), T(""));
	char_t * pIp = websGetVar(wp, T("tempIp"), T(""));
	
	sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	char pRemManQry [1024] = {0};
	memset (pRemManQry, 0x00, sizeof(pRemManQry));
	int nEnable=0;
	if (gstrcmp(pEnable,"0")==0)
		nEnable=0;
	else
		nEnable=1;

	if(nEnable == 0)
	{
		snprintf(pRemManQry,sizeof(pRemManQry),"update accessMgmt set accessMgmtEnable='%d'",0);
	}
	else
	{
		snprintf(pRemManQry,sizeof(pRemManQry),"update accessMgmt set accessMgmtEnable='%d', accessMgmtIP1='%s', accessType='%s' where _ROWID_='1'",
			nEnable,pIp,pType);
	}
	status = sqlite3_exec (pDB, pRemManQry, NULL, NULL, &pErrMsg);

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD);
    }
    websRedirect(wp, T("system/remote_manage.asp"));
	
}
#endif

#if defined(__P02003_Dora)
static int getIpRange (int eid, webs_t wp, int argc, char_t **argv)
{
	if (rem_man_idx==-1) //add
	{
		return websWrite(wp, T("\"%s\""),"");
	}
	else if (rem_man_idx>0) //edit
	{
		int ret = 0;
		char * pStartIp = jnrp_sql_getAttr_Webs("ipRanges","startIP",rem_man_idx);
		char * pEndIp = jnrp_sql_getAttr_Webs("ipRanges","endIP",rem_man_idx);		
		char* pIpRangeItem = (char *)malloc(128);
		memset(pIpRangeItem, '\0', 128);
		gstrcat(pIpRangeItem, pStartIp);
		gstrcat(pIpRangeItem, pSep);
		gstrcat(pIpRangeItem, pEndIp);
		ret = websWrite(wp, T("\"%s\""), pIpRangeItem);
		free(pIpRangeItem);
		return ret;
	}
}

static int getIpRangeList (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
	 int nRows=0, nCols=0;
	 char** ppQueryRes = NULL;
	 char * pQueryStr = "select * from ipRanges";

	 int index = 0;
	 char* pStartIp = NULL;
	 char* pEndIp = NULL;

	 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	 {
	   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	   //sqlite3_free(pQueryStr);
	   return 0;
	 }

	 if (nRows > 0) {
	 	for (index = 0; index < nRows; index++)
	 	{
	    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "startIP", pStartIp, getIpRangeOut);
	    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "endIP", pEndIp, getIpRangeOut);
	    websWrite(wp, T("<tr>"));
		if (index%2)
		{
			websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"ip_range_ck\" id=\"ip_range_ck_%d\" /></td>\n"),index+1);
			websWrite(wp, T("<td class=\"tdOdd\" id=\"ip_range_start_ip_%d\">%s</td>\n"),index+1, pStartIp);
			websWrite(wp, T("<td class=\"tdOdd\" id=\"ip_range_end_ip_%d\">%s</td>\n"),index+1, pEndIp);
		}
		else
		{
			websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"ip_range_ck\" id=\"ip_range_ck_%d\" /></td>\n"),index+1);
			websWrite(wp, T("<td class=\"tdEven\" id=\"ip_range_start_ip_%d\">%s</td>\n"),index+1, pStartIp);
			websWrite(wp, T("<td class=\"tdEven\" id=\"ip_range_end_ip_%d\">%s</td>\n"),index+1, pEndIp);
		}

	  }
	 }

	getIpRangeOut:
	 if (ppQueryRes)
	 {
	  sqlite3_free_table(ppQueryRes);
	 }

	 return 0;
#else
	    return 0;
#endif
}

static void setIpRange(webs_t wp, char_t *path, char_t *query)
{	
    char * pErrMsg = NULL;
    int status = -1;
	char ** ppQryResult = NULL;
    char_t * pStartIp = websGetVar(wp, T("startIP"), T(""));
    char_t * pEndIp = websGetVar(wp, T("endIP"), T(""));
    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }

	char * pAccessMgmtIP = jnrp_sql_getAttr_Webs ("accessMgmt", "accessMgmtIP1", 1);
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

	char pRemManQry [1024] = {0};
	memset (pRemManQry, 0x00, sizeof(pRemManQry));
	snprintf(pRemManQry,sizeof(pRemManQry),"update accessMgmt set accessMgmtEnable='1', accessMgmtIP1='%s', accessType='1' where _ROWID_='1'",
		pAccessMgmtIP);
	status = sqlite3_exec (pDB, pRemManQry, NULL, NULL, &pErrMsg);  

 	char pCheckQry [256] = {0};
	memset(pCheckQry, 0x00, sizeof(pCheckQry));
	check_ip_range_status = iprange_status_none;
	if(rem_man_idx == -1)
	{
		snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from ipRanges where startIP='%s' and endIP='%s'", pStartIp,pEndIp);
	    ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
	    if (gatoi(ppQryResult[1]) > 0) //Same Name
	    {
	    	check_ip_range_status = iprange_status_err;
			ppQryResult = NULL;
	 		websRedirect(wp, T("system/remote_manage.asp"));
			return;
	    }
	}
	else if(rem_man_idx > 0)
	{
		snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from ipRanges where startIP='%s' and endIP='%s' and _ROWID_<>'%d'", pStartIp,pEndIp,rem_man_idx);
	    ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
	    if (gatoi(ppQryResult[1]) > 0) //Same Name
	    {
	    	check_ip_range_status = iprange_status_err;
			ppQryResult = NULL;
	 		websRedirect(wp, T("system/remote_manage.asp"));
			return;
	    }
	}

	char pIpRangeQry [512] = {0};
    memset (pIpRangeQry, 0x00, sizeof(pIpRangeQry));
    if (rem_man_idx ==-1) //add
    {
        snprintf(pIpRangeQry,sizeof(pIpRangeQry), "insert into ipRanges values ('%s','%s')"
        ,pStartIp,pEndIp);
        
        status = sqlite3_exec (pDB, pIpRangeQry, NULL, NULL, &pErrMsg);
    }
    else if (rem_man_idx>0) //edit
    {
        snprintf(pIpRangeQry,sizeof(pIpRangeQry), "update ipRanges set startIP='%s', endIP='%s' where _ROWID_='%d'",
        pStartIp,pEndIp,rem_man_idx);
        status = sqlite3_exec (pDB, pIpRangeQry, NULL, NULL, &pErrMsg);
    }


    if (pDB)
    {
        sqlite3_close (pDB);
    }
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD);
    }
    websRedirect(wp, T("system/remote_manage.asp"));
}

static int getEventLog (int eid, webs_t wp, int argc, char_t **argv)
{
 char * pCategory = jnrp_sql_getAttr_Webs ("sysLogInfo", "logCategory", 1);
 int nCategory = gatoi(pCategory);

 if (nCategory == 1) {
   system("tail -n 250 /var/log/messages_webui_all.log > /tmp/getEventLog");
 } else if (nCategory == 2) {
   system("tail -n 250 /var/log/internet.log > /tmp/getEventLog");
 } else if (nCategory == 3) {
   system("tail -n 250 /var/log/wifi.log > /tmp/getEventLog");
 } else if (nCategory == 4) {
   system("tail -n 250 /var/log/firewall.log > /tmp/getEventLog");
 } else if (nCategory == 5) {
   system("tail -n 250 /var/log/app.log > /tmp/getEventLog");
 } else if (nCategory == 6) {
   system("tail -n 250 /var/log/lan.log > /tmp/getEventLog");
 }

 FILE *fp = fopen("/tmp/getEventLog", "r");
 char line[512];
 memset(line, '\0', sizeof(line));
 if(fp)
 {
   while(fgets(line, sizeof(line), fp) != NULL)
   {
     websWrite(wp, T("%s"), line);
   }
   fclose(fp);
 }
 system("rm -f /tmp/getEventLog");

 return 0;
}

static void setEventLog(webs_t wp, char_t *path, char_t *query)
{
	
}
#endif

static int getSystemBackupLog (int eid, webs_t wp, int argc, char_t **argv)
{
	system("ls -a1 /mnt/data/backup_log | grep '^[a-zA-Z0-9]' | wc -l > /tmp/backup_log_file");
	system("ls -a1 /mnt/data/backup_log | grep '^[a-zA-Z0-9]' >> /tmp/backup_log_file");

	char line[128];
	memset(line, '\0', sizeof(line));

	FILE *fp = fopen("/tmp/backup_log_file", "r");
	if (fp)
	{
		int count = 0;
		while(fgets(line, sizeof(line), fp) != NULL)
		{
			if (line[strlen(line)-1] == '\n')
				line[strlen(line)-1] = '\0';
			count++;
			if (count == 1) {
				int listNum = gatoi(line);
				if (listNum == 0) {
					websWrite(wp, T("<option disabled><script>document.write(gettext(\"Backup log files not found\"));</script></option>"));
					break;
				}
			} else {
				websWrite(wp, T("<option value=%s >%s</option>"), line, line);
			}
		}
		fclose(fp);
	} else {
		websWrite(wp, T("<option disabled><script>document.write(gettext(\"Backup log files not found\"));</script></option>"));
	}	
	system("rm -f /tmp/backup_log_file");
	return 0;
}

char getLoginsession()
{
 FILE *file;
 char loginsession = '0';
	file = fopen("/tmp/loginsession", "r");
	if (file)
	{
		loginsession = fgetc(file);
		fclose(file);
	}
	return loginsession;
}
