#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

void initVoipWeb();
//Service-> SIP server
static int getVoIPServiceStatus(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPServiceProxy(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPServiceProxyAddr(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPServiceOutAddr(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPServiceRegAddr(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPServiceSecAddr(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPServiceDomainAddr(int eid, webs_t wp, int argc, char_t **argv);
static void setServiceSIPserver (webs_t wp, char_t *path, char_t *query);

//Line1-> SIP account
static int getVoIPLine1DisplayName(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPLine1UserName(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPLine1Auth(int eid, webs_t wp, int argc, char_t **argv);
static void setLine1SIPaccount (webs_t wp, char_t *path, char_t *query);

//Line1-> Terminals
static int getVoIPLine1Terminals(int eid, webs_t wp, int argc, char_t **argv);
static void setLine1Terminals (webs_t wp, char_t *path, char_t *query);

//Line1-> Settings
static void setLine1 (webs_t wp, char_t *path, char_t *query);

//Line2-> SIP account
static int getVoIPLine2DisplayName(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPLine2UserName(int eid, webs_t wp, int argc, char_t **argv);
static int getVoIPLine2Auth(int eid, webs_t wp, int argc, char_t **argv);
static void setLine2SIPaccount (webs_t wp, char_t *path, char_t *query);

//Line2-> Terminals
static int getVoIPLine2Terminals(int eid, webs_t wp, int argc, char_t **argv);
static void setLine2Terminals (webs_t wp, char_t *path, char_t *query);

//Line2-> Settings
static void setLine2 (webs_t wp, char_t *path, char_t *query);

//System-> Debug
static int getVoIPSystemDebug(int eid, webs_t wp, int argc, char_t **argv);
static void setSystemDebug (webs_t wp, char_t *path, char_t *query);

void initVoipWeb()
{
    websAspDefine(T("getVoIPServiceStatus"), getVoIPServiceStatus);
	websAspDefine(T("getVoIPServiceProxy"), getVoIPServiceProxy);
	websAspDefine(T("getVoIPServiceProxyAddr"), getVoIPServiceProxyAddr);
	websAspDefine(T("getVoIPServiceOutAddr"), getVoIPServiceOutAddr);
	websAspDefine(T("getVoIPServiceRegAddr"), getVoIPServiceRegAddr);
	websAspDefine(T("getVoIPServiceSecAddr"), getVoIPServiceSecAddr);
	websAspDefine(T("getVoIPServiceDomainAddr"), getVoIPServiceDomainAddr);
	websFormDefine(T("setServiceSIPserver"), setServiceSIPserver);

	websAspDefine(T("getVoIPLine1DisplayName"), getVoIPLine1DisplayName);
	websAspDefine(T("getVoIPLine1UserName"), getVoIPLine1UserName);
	websAspDefine(T("getVoIPLine1Auth"), getVoIPLine1Auth);
	websFormDefine(T("setLine1SIPaccount"), setLine1SIPaccount);

	websAspDefine(T("getVoIPLine1Terminals"), getVoIPLine1Terminals);
	websFormDefine(T("setLine1Terminals"), setLine1Terminals);

	websFormDefine(T("setLine1"), setLine1);

	websAspDefine(T("getVoIPLine2DisplayName"), getVoIPLine2DisplayName);
	websAspDefine(T("getVoIPLine2UserName"), getVoIPLine2UserName);
	websAspDefine(T("getVoIPLine2Auth"), getVoIPLine2Auth);
	websFormDefine(T("setLine2SIPaccount"), setLine2SIPaccount);

	websAspDefine(T("getVoIPLine2Terminals"), getVoIPLine2Terminals);
	websFormDefine(T("setLine2Terminals"), setLine2Terminals);	

	websFormDefine(T("setLine2"), setLine2);

	websAspDefine(T("getVoIPSystemDebug"), getVoIPSystemDebug);
	websFormDefine(T("setSystemDebug"), setSystemDebug);

    return;
}

static int getVoIPServiceStatus(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
	int ret = 0;
    char* pQueryStr = "select State from VoiceProfile";
    char* pState = NULL;
    char *pVoipProfile = (char *)malloc(128);
    gstrcpy(pVoipProfile,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipProfile);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "State", pState, getVoIPServiceStatusOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipProfile,pState);
    }

getVoIPServiceStatusOut:
    ret = websWrite(wp, T("\"%s\""), pVoipProfile);
	free(pVoipProfile);
	return ret;
}

static int getVoIPServiceProxy(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select * from VoiceProfileSignaling";
	//Proxy Info
    //char* pProxyAddr = NULL;
	char* pProxyPort = NULL;             
	char* pProxyProtocol = NULL; 
    //Outbount Proxy Info
    //char* pOutboundProxyAddr = NULL;
	char* pOutboundProxyPort = NULL;
	//Register Info
	char* pRegExpirationTime = NULL;
	char* pRegRetryTime = NULL;
	//char* pRegistrarAddr = NULL;
	char* pRegistrarPort = NULL;
	char* pRegistrarProtocol = NULL;
	//char* pBackupRegAddr = NULL;
	char* pBackupRegPort = NULL;
	char* pBackupRegProtocol = NULL;
	//Domain Info
	//char* pUADomain = NULL;
	char* pUAPort = NULL;
	char* pUAProtocol = NULL;
    char *pVoipProfileSignal = (char *)malloc(128);
	int ret = 0;
    gstrcpy(pVoipProfileSignal,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipProfileSignal);
        return -1;
    }
	/*Proxy Info*/
    //DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ProxyAddr", pProxyAddr, getVoIPServiceSignalOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ProxyPort", pProxyPort, getVoIPServiceSignalOut);	
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ProxyProtocol", pProxyProtocol, getVoIPServiceSignalOut);	
	
	/*Outbount Proxy Info*/
    //DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "OutboundProxyAddr", pOutboundProxyAddr, getVoIPServiceSignalOut);	
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "OutboundProxyPort", pOutboundProxyPort, getVoIPServiceSignalOut);

	/*Register Info*/
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RegExpirationTime", pRegExpirationTime, getVoIPServiceSignalOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RegRetryTime", pRegRetryTime, getVoIPServiceSignalOut);
	//DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RegistrarAddr", pRegistrarAddr, getVoIPServiceSignalOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RegistrarPort", pRegistrarPort, getVoIPServiceSignalOut);	
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RegistrarProtocol", pRegistrarProtocol, getVoIPServiceSignalOut);	
	//DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "BackupRegAddr", pBackupRegAddr, getVoIPServiceSignalOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "BackupRegPort", pBackupRegPort, getVoIPServiceSignalOut);	
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "BackupRegProtocol", pBackupRegProtocol, getVoIPServiceSignalOut);	
	
    /*Domain Info*/
	//DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "UADomain", pUADomain, getVoIPServiceSignalOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "UAPort", pUAPort, getVoIPServiceSignalOut);	
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "UAProtocol", pUAProtocol, getVoIPServiceSignalOut);	
	
    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
		/*Proxy Info*/
		gstrcat(pVoipProfileSignal,pProxyPort);         //0
		gstrcat(pVoipProfileSignal,pSep);
		gstrcat(pVoipProfileSignal,pProxyProtocol);     //1
		/*Outbount Proxy Info*/
		gstrcat(pVoipProfileSignal,pSep);
        gstrcat(pVoipProfileSignal,pOutboundProxyPort); //2
		gstrcat(pVoipProfileSignal,pSep);
		/*Register Info*/
		gstrcat(pVoipProfileSignal,pRegExpirationTime); //3
		gstrcat(pVoipProfileSignal,pSep);
        gstrcat(pVoipProfileSignal,pRegRetryTime);      //4
		gstrcat(pVoipProfileSignal,pSep);
		gstrcat(pVoipProfileSignal,pRegistrarPort);     //5
		gstrcat(pVoipProfileSignal,pSep);
		gstrcat(pVoipProfileSignal,pRegistrarProtocol); //6
		gstrcat(pVoipProfileSignal,pSep);
		gstrcat(pVoipProfileSignal,pBackupRegPort);     //7
		gstrcat(pVoipProfileSignal,pSep);
		gstrcat(pVoipProfileSignal,pBackupRegProtocol); //8
		gstrcat(pVoipProfileSignal,pSep);
		/*Domain Info*/
		gstrcat(pVoipProfileSignal,pUAPort);            //9
		gstrcat(pVoipProfileSignal,pSep);
		gstrcat(pVoipProfileSignal,pUAProtocol);        //10
    }

getVoIPServiceSignalOut:
	ret = websWrite(wp, T("\"%s\""), pVoipProfileSignal);
	free(pVoipProfileSignal);
    return ret;
}

static int getVoIPServiceProxyAddr(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;
	char* pQueryStr = "select ProxyAddr from VoiceProfileSignaling";
    char* pProxyAddr = NULL;
	int ret = 0;
    char *pVoipProfile = (char *)malloc(128);
    gstrcpy(pVoipProfile,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
	   sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipProfile);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "ProxyAddr", pProxyAddr, getVoIPServiceStatusOut);
    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipProfile,pProxyAddr);
    }

getVoIPServiceStatusOut:
	ret = websWrite(wp, T("\"%s\""), pVoipProfile);
	free(pVoipProfile);
	return ret;
}

static int getVoIPServiceOutAddr(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select OutboundProxyAddr from VoiceProfileSignaling";
    char* pRegistrarAddr = NULL;
	int ret = 0;
    char *pVoipProfile = (char *)malloc(128);
    gstrcpy(pVoipProfile,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipProfile);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "OutboundProxyAddr", pRegistrarAddr, getVoIPServiceStatusOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipProfile,pRegistrarAddr);
    }

getVoIPServiceStatusOut:
    ret = websWrite(wp, T("\"%s\""), pVoipProfile);
	free(pVoipProfile);
	return ret;
}

static int getVoIPServiceRegAddr(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select RegistrarAddr from VoiceProfileSignaling";
    char* pBackupRegAddr = NULL;
	int ret = 0;
    char *pVoipProfile = (char *)malloc(128);
    gstrcpy(pVoipProfile,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipProfile);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RegistrarAddr", pBackupRegAddr, getVoIPServiceStatusOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipProfile,pBackupRegAddr);
    }

getVoIPServiceStatusOut:
    ret = websWrite(wp, T("\"%s\""), pVoipProfile);
	free(pVoipProfile);
	return ret;
}

static int getVoIPServiceSecAddr(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select BackupRegAddr from VoiceProfileSignaling";
    char* pOutboundProxyAddr = NULL;
	int ret = 0;
    char *pVoipProfile = (char *)malloc(128);
    gstrcpy(pVoipProfile,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipProfile);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "BackupRegAddr", pOutboundProxyAddr, getVoIPServiceStatusOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipProfile,pOutboundProxyAddr);
    }

getVoIPServiceStatusOut:
    ret = websWrite(wp, T("\"%s\""), pVoipProfile);
	free(pVoipProfile);
	return ret;
}

static int getVoIPServiceDomainAddr(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select UADomain from VoiceProfileSignaling";
    char* pUADomain = NULL;
	int ret = 0;
    char *pVoipProfile = (char *)malloc(128);
    gstrcpy(pVoipProfile,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipProfile);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "UADomain", pUADomain, getVoIPServiceStatusOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipProfile,pUADomain);
    }

getVoIPServiceStatusOut:
    ret = websWrite(wp, T("\"%s\""), pVoipProfile);
	free(pVoipProfile);
	return ret;
}

static int getVoIPLine1DisplayName(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select SipDispName from VoiceLineSignaling where LineId = 1";
    char* pSipDispName = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipDispName", pSipDispName, getVoIPLine1NameOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pSipDispName);
    }

getVoIPLine1NameOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPLine1UserName(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select SipUserName from VoiceLineSignaling where LineId = 1";
    char* pSipUserName = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipUserName", pSipUserName, getVoIPLine1NameOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pSipUserName);
    }

getVoIPLine1NameOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPLine1Auth(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select * from AuthCfg where LineId = 1";
    char* pSipAuthUsrName = NULL;
	char* pSipAuthPasswd = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipAuthUsrName", pSipAuthUsrName, getVoIPLine1AuthOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipAuthPasswd", pSipAuthPasswd, getVoIPLine1AuthOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pSipAuthUsrName);  //0
        gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pSipAuthPasswd);   //1
    }

getVoIPLine1AuthOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPLine1Terminals(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select * from VoiceLine where LineId = 1";
    char* pState = NULL;
	char* pName = NULL;
	char* pLineStatus = NULL;
	char* pLineMode = NULL;
	char* pIntrusion = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "State", pState, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Name", pName, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "LineStatus", pLineStatus, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "LineMode", pLineMode, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Intrusion", pIntrusion, getVoIPLine1TerminalsOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pState); 
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pName);
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pLineStatus);
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pLineMode);
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pIntrusion);
    }

getVoIPLine1TerminalsOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPLine2DisplayName(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select SipDispName from VoiceLineSignaling where LineId = 2";
    char* pSipDispName = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipDispName", pSipDispName, getVoIPLine1NameOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pSipDispName);
    }

getVoIPLine1NameOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPLine2UserName(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select SipUserName from VoiceLineSignaling where LineId = 2";
    char* pSipUserName = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipUserName", pSipUserName, getVoIPLine1NameOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pSipUserName);
    }

getVoIPLine1NameOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPLine2Auth(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select * from AuthCfg where LineId = 2";
    char* pSipAuthUsrName = NULL;
	char* pSipAuthPasswd = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipAuthUsrName", pSipAuthUsrName, getVoIPLine1AuthOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipAuthPasswd", pSipAuthPasswd, getVoIPLine1AuthOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pSipAuthUsrName);  //0
        gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pSipAuthPasswd);   //1
    }

getVoIPLine1AuthOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPLine2Terminals(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select * from VoiceLine where LineId = 2";
    char* pState = NULL;
	char* pName = NULL;
	char* pLineStatus = NULL;
	char* pLineMode = NULL;
	char* pIntrusion = NULL;
	int ret = 0;
    char *pVoipLine1 = (char *)malloc(128);
    gstrcpy(pVoipLine1,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipLine1);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "State", pState, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Name", pName, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "LineStatus", pLineStatus, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "LineMode", pLineMode, getVoIPLine1TerminalsOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Intrusion", pIntrusion, getVoIPLine1TerminalsOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipLine1,pState); 
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pName);
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pLineStatus);
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pLineMode);
		gstrcat(pVoipLine1,pSep);
		gstrcat(pVoipLine1,pIntrusion);
    }

getVoIPLine1TerminalsOut:
    ret = websWrite(wp, T("\"%s\""), pVoipLine1);
	free(pVoipLine1);
	return ret;
}

static int getVoIPSystemDebug(int eid, webs_t wp, int argc, char_t **argv)
{
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0,status=0;

	char* pQueryStr = "select * from DbgSetting";
    char* pCmDbg_DbgType = NULL;
	char* pCmDbg_DbgLvl = NULL;
	char* pMmDbg_DbgType = NULL;
	char* pMmDbg_DbgLvl = NULL;
	char* pAgentsDbg_DbgType = NULL;
	char* pAgentsDbg_DbgLvl = NULL;
	char* pVmapiDbg_DbgType = NULL;
    char* pVmapiDbg_DbgLvl = NULL;
	char* pRtpDbg_DbgType = NULL;
	char* pRtpDbg_DbgLvl = NULL;
	char* pSipDbg_DbgType = NULL;
	char* pSipDbg_DbgLvl = NULL;
	char* pFaxDbg_DbgType = NULL;
	char* pFaxDbg_DbgLvl = NULL;	
	int ret = 0;
    char *pVoipSystem = (char *)malloc(128);
    gstrcpy(pVoipSystem,"");
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
		free(pVoipSystem);
        return -1;
    }
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "CmDbg_DbgType", pCmDbg_DbgType, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "CmDbg_DbgLvl", pCmDbg_DbgLvl, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "MmDbg_DbgType", pMmDbg_DbgType, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "MmDbg_DbgLvl", pMmDbg_DbgLvl, getVoIPSystemDebugOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "AgentsDbg_DbgType", pAgentsDbg_DbgType, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "AgentsDbg_DbgLvl", pAgentsDbg_DbgLvl, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "VmapiDbg_DbgType", pVmapiDbg_DbgType, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "VmapiDbg_DbgLvl", pVmapiDbg_DbgLvl, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RtpDbg_DbgType", pRtpDbg_DbgType, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "RtpDbg_DbgLvl", pRtpDbg_DbgLvl, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipDbg_DbgType", pSipDbg_DbgType, getVoIPSystemDebugOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "SipDbg_DbgLvl", pSipDbg_DbgLvl, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "FaxDbg_DbgType", pFaxDbg_DbgType, getVoIPSystemDebugOut);
	DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "FaxDbg_DbgLvl", pFaxDbg_DbgLvl, getVoIPSystemDebugOut);

    if (ppQueryRes)
    {
        sqlite3_free_table (ppQueryRes);
        gstrcat(pVoipSystem,pCmDbg_DbgType);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pCmDbg_DbgLvl);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pMmDbg_DbgType);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pMmDbg_DbgLvl);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pAgentsDbg_DbgType);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pAgentsDbg_DbgLvl);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pVmapiDbg_DbgType);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pVmapiDbg_DbgLvl);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pRtpDbg_DbgType);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pRtpDbg_DbgLvl);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pSipDbg_DbgType);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pSipDbg_DbgLvl);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pFaxDbg_DbgType);
		gstrcat(pVoipSystem,pSep);
		gstrcat(pVoipSystem,pFaxDbg_DbgLvl);
    }

getVoIPSystemDebugOut:
    ret = websWrite(wp, T("\"%s\""), pVoipSystem);
	free(pVoipSystem);
	return ret;
}

static void setServiceSIPserver (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;
    char_t * pEnable = websGetVar(wp, T("sip_enable"), T("off"));
	char_t * proxyaddr = websGetVar(wp, T("sip_proxyaddr"), T(""));
    char_t * proxyport = websGetVar(wp, T("sip_proxyport"), T(""));
    char_t * proxyprotocol = websGetVar(wp, T("sip_proxyprotocol"), T(""));
	char_t * outaddr = websGetVar(wp, T("sip_outaddr"), T(""));
    char_t * outport = websGetVar(wp, T("sip_outport"), T(""));
    //char_t * outprotocol = websGetVar(wp, T("sip_outprotocol"), T(""));
	//char_t * regextime = websGetVar(wp, T("sip_regextime"), T(""));
    //char_t * regretime = websGetVar(wp, T("sip_regretime"), T(""));
	char_t * regaddr = websGetVar(wp, T("sip_regaddr"), T(""));
    char_t * regport = websGetVar(wp, T("sip_regport"), T(""));
    char_t * regprotocol = websGetVar(wp, T("sip_regprotocol"), T(""));
	char_t * secaddr = websGetVar(wp, T("sip_secaddr"), T(""));
    char_t * secport = websGetVar(wp, T("sip_secport"), T(""));
    char_t * secprotocol = websGetVar(wp, T("sip_secprotocol"), T(""));
	char_t * dmaddr = websGetVar(wp, T("sip_dmaddr"), T(""));
    char_t * dmport = websGetVar(wp, T("sip_dmport"), T(""));
    char_t * dmprotocol = websGetVar(wp, T("sip_dmprotocol"), T(""));
	int iproxyport = gatoi(proxyport);
	int iproxyprotocol = gatoi(proxyprotocol);
	int ioutport = gatoi(outport);
	//int iregextime = gatoi(regextime);
	//int iregretime = gatoi(regretime);
	int iregport = gatoi(regport);
	int iregprotocol = gatoi(regprotocol);
	int isecport = gatoi(secport);
	int isecprotocol = gatoi(secprotocol);
	int idmport = gatoi(dmport);
	int idmprotocol = gatoi(dmprotocol);
 
    char pVoiceProfileQry [1024] = {0};
	char pVoiceProfileSignalingQry [1024] = {0};
	memset (pVoiceProfileQry, 0x00, sizeof(pVoiceProfileQry));
    memset (pVoiceProfileSignalingQry, 0x00, sizeof(pVoiceProfileSignalingQry));
    if (gstrcmp(pEnable,"on")==0)
    {
        snprintf(pVoiceProfileQry,sizeof(pVoiceProfileQry),"update VoiceProfile set State='%d'", 1);

		snprintf (pVoiceProfileSignalingQry, sizeof(pVoiceProfileSignalingQry),
		"update VoiceProfileSignaling set " 
		"ProxyAddr='%s', ProxyPort='%d', ProxyProtocol='%d',"
		"OutboundProxyAddr='%s', OutboundProxyPort='%d',"
		//"RegExpirationTime='%d', RegRetryTime='%d',"
		"RegistrarAddr='%s', RegistrarPort='%d', RegistrarProtocol='%d',"
		"BackupRegAddr='%s', BackupRegPort='%d', BackupRegProtocol='%d',"
		"UADomain='%s', UAPort='%d', UAProtocol='%d'"
		, proxyaddr, iproxyport, iproxyprotocol
		, outaddr, ioutport
		//, iregextime, iregretime
		, regaddr, iregport, iregprotocol
		, secaddr, isecport, isecprotocol
		, dmaddr, idmport, idmprotocol);
		status = jnrp_sql_dbEXE (pDBFile,pVoiceProfileSignalingQry);
    } else {
        snprintf(pVoiceProfileQry,sizeof(pVoiceProfileQry),"update VoiceProfile set State='%d'",0);
    }
#ifdef __USED_WEBS_DB__
    status = jnrp_sql_dbEXE_Webs (pVoiceProfileQry);
#else
    status = jnrp_sql_dbEXE (pDBFile,pVoiceProfileQry);
#endif
    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");
	
    websRedirect(wp, T("voip/voip_service_sipserver.asp"));
}

static void setLine1SIPaccount (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;

	char_t * displayName = websGetVar(wp, T("line1_displayName"), T(""));
    char_t * userName = websGetVar(wp, T("line1_userName"), T(""));
    char_t * authName = websGetVar(wp, T("line1_authName"), T(""));
	char_t * pwd = websGetVar(wp, T("line1_pwd"), T(""));

    char pVoiceLineSignalingQry [1024] = {0};
	char pVoiceLineQry [1024] = {0};
	char pAuthCfgQry [1024] = {0};
	memset (pVoiceLineSignalingQry, 0x00, sizeof(pVoiceLineSignalingQry));
	memset (pVoiceLineQry, 0x00, sizeof(pVoiceLineQry));
	memset (pAuthCfgQry, 0x00, sizeof(pAuthCfgQry));

	snprintf (pVoiceLineSignalingQry, sizeof(pVoiceLineSignalingQry),
		"update VoiceLineSignaling set SipDispName='%s', SipUserName='%s' where LineId = 1", displayName, userName);
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineSignalingQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineSignalingQry);
#endif

	snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set DirName='%s' where LineId = 1", displayName);
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif

	snprintf (pAuthCfgQry, sizeof(pAuthCfgQry),
		"update AuthCfg set SipAuthUsrName='%s', SipAuthPasswd='%s' where LineId = 1", authName, pwd);
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pAuthCfgQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pAuthCfgQry);
#endif

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");

    websRedirect(wp, T("voip/voip_line_line1_sipaccount.asp"));
}

static void setLine1Terminals (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;

	char_t * enable = websGetVar(wp, T("terminals_enable"), T("off"));
    char_t * linename = websGetVar(wp, T("terminals_linename"), T(""));
    char_t * callmode = websGetVar(wp, T("terminals_callmode"), T(""));
	char_t * cllint = websGetVar(wp, T("terminals_callint"), T(""));
	int ienable = gatoi(enable);
	int icallmode = gatoi(callmode);
	int icllint = gatoi(cllint);

    char pVoiceLineQry [1024] = {0};
	memset (pVoiceLineQry, 0x00, sizeof(pVoiceLineQry));

    if (gstrcmp(enable,"on")==0)
    {
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d', Name='%s', LineMode='%d', Intrusion='%d' where LineId = 1", 1, linename, icallmode, icllint);
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	} else {
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d' where LineId = 1", 0);
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	}

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");

    websRedirect(wp, T("voip/voip_line_line1_terminals.asp"));
}

static void setLine1(webs_t wp, char_t *path, char_t *query)
{
    int status = -1;
    //Terminals
	char_t * enable = websGetVar(wp, T("line1_terminals_enable"), T("off"));
    char_t * linename = websGetVar(wp, T("line1_terminals_linename"), T(""));
    char_t * callmode = websGetVar(wp, T("line1_terminals_callmode"), T(""));
	char_t * cllint = websGetVar(wp, T("line1_terminals_callint"), T(""));
	//SIP account
	char_t * displayName = websGetVar(wp, T("line1_displayName"), T(""));
    char_t * userName = websGetVar(wp, T("line1_userName"), T(""));
    char_t * authName = websGetVar(wp, T("line1_authName"), T(""));
	char_t * pwd = websGetVar(wp, T("line1_pwd"), T(""));
	
	int ienable = gatoi(enable);
	int icallmode = gatoi(callmode);
	int icllint = gatoi(cllint);

	//Terminals, SIP account DB
	char pVoiceLineSignalingQry [1024] = {0};
	char pVoiceLineQry [1024] = {0};
	char pAuthCfgQry [1024] = {0};
	memset (pVoiceLineSignalingQry, 0x00, sizeof(pVoiceLineSignalingQry));
	memset (pVoiceLineQry, 0x00, sizeof(pVoiceLineQry));
	memset (pAuthCfgQry, 0x00, sizeof(pAuthCfgQry));
	
    //Set Terminals DB
    if (gstrcmp(enable,"on")==0)
    {
#if defined(__P02001_Dora) || defined(__P02002_Dora)    
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d', Name='%s', LineMode='%d', Intrusion='%d' where LineId = 2", 1, linename, icallmode, icllint);
#else
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d', Name='%s', LineMode='%d', Intrusion='%d' where LineId = 1", 1, linename, icallmode, icllint);
#endif
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	} else {
#if defined(__P02001_Dora) || defined(__P02002_Dora)    	
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d' where LineId = 2", 0);
#else
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d' where LineId = 1", 0);
#endif
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	}

	//Set SIP account DB
#if defined(__P02001_Dora) || defined(__P02002_Dora)   	
	snprintf (pVoiceLineSignalingQry, sizeof(pVoiceLineSignalingQry),
		"update VoiceLineSignaling set SipDispName='%s', SipUserName='%s' where LineId = 2", displayName, userName);
#else
	snprintf (pVoiceLineSignalingQry, sizeof(pVoiceLineSignalingQry),
		"update VoiceLineSignaling set SipDispName='%s', SipUserName='%s' where LineId = 1", displayName, userName);
#endif
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineSignalingQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineSignalingQry);
#endif

#if defined(__P02001_Dora) || defined(__P02002_Dora)
	snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set DirName='%s' where LineId = 2", displayName);
#else
	snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set DirName='%s' where LineId = 1", userName);
#endif

#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif

#if defined(__P02001_Dora) || defined(__P02002_Dora)
	snprintf (pAuthCfgQry, sizeof(pAuthCfgQry),
		"update AuthCfg set SipAuthUsrName='%s', SipAuthPasswd='%s' where LineId = 2", authName, pwd);
#else
	snprintf (pAuthCfgQry, sizeof(pAuthCfgQry),
		"update AuthCfg set SipAuthUsrName='%s', SipAuthPasswd='%s' where LineId = 1", authName, pwd);
#endif
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pAuthCfgQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pAuthCfgQry);
#endif

	if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");

    websRedirect(wp, T("voip/voip_linesettings.asp"));
}

static void setLine2SIPaccount (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;

	char_t * displayName = websGetVar(wp, T("line2_displayName"), T(""));
    char_t * userName = websGetVar(wp, T("line2_userName"), T(""));
    char_t * authName = websGetVar(wp, T("line2_authName"), T(""));
	char_t * pwd = websGetVar(wp, T("line2_pwd"), T(""));

    char pVoiceLineSignalingQry [1024] = {0};
	char pVoiceLineQry [1024] = {0};
	char pAuthCfgQry [1024] = {0};
	memset (pVoiceLineSignalingQry, 0x00, sizeof(pVoiceLineSignalingQry));
	memset (pVoiceLineQry, 0x00, sizeof(pVoiceLineQry));
	memset (pAuthCfgQry, 0x00, sizeof(pAuthCfgQry));

	snprintf (pVoiceLineSignalingQry, sizeof(pVoiceLineSignalingQry),
		"update VoiceLineSignaling set SipDispName='%s', SipUserName='%s' where LineId = 2", displayName, userName);
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineSignalingQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineSignalingQry);
#endif
#if defined(__P02001_Dora) || defined(__P02002_Dora)
	snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set DirName='%s' where LineId = 2", displayName);
#else
	snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set DirName='%s' where LineId = 2", userName);
#endif

#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif

	snprintf (pAuthCfgQry, sizeof(pAuthCfgQry),
		"update AuthCfg set SipAuthUsrName='%s', SipAuthPasswd='%s' where LineId = 2", authName, pwd);
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pAuthCfgQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pAuthCfgQry);
#endif

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");

    websRedirect(wp, T("voip/voip_line_line2_sipaccount.asp"));
}

static void setLine2Terminals (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;

	char_t * enable = websGetVar(wp, T("terminals_enable"), T("off"));
    char_t * linename = websGetVar(wp, T("terminals_linename"), T(""));
    char_t * callmode = websGetVar(wp, T("terminals_callmode"), T(""));
	char_t * cllint = websGetVar(wp, T("terminals_callint"), T(""));
	int ienable = gatoi(enable);
	int icallmode = gatoi(callmode);
	int icllint = gatoi(cllint);

    char pVoiceLineQry [1024] = {0};
	memset (pVoiceLineQry, 0x00, sizeof(pVoiceLineQry));

    if (gstrcmp(enable,"on")==0)
    {
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d', Name='%s', LineMode='%d', Intrusion='%d' where LineId = 2", 1, linename, icallmode, icllint);
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	} else {
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d' where LineId = 2", 0);
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	}

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");

    websRedirect(wp, T("voip/voip_line_line2_terminals.asp"));
}

static void setLine2(webs_t wp, char_t *path, char_t *query)
{
    int status = -1;
    //Terminals
	char_t * enable = websGetVar(wp, T("line2_terminals_enable"), T("off"));
    char_t * linename = websGetVar(wp, T("line2_terminals_linename"), T(""));
    char_t * callmode = websGetVar(wp, T("line2_terminals_callmode"), T(""));
	char_t * cllint = websGetVar(wp, T("line2_terminals_callint"), T(""));
	//SIP account
	char_t * displayName = websGetVar(wp, T("line2_displayName"), T(""));
    char_t * userName = websGetVar(wp, T("line2_userName"), T(""));
    char_t * authName = websGetVar(wp, T("line2_authName"), T(""));
	char_t * pwd = websGetVar(wp, T("line2_pwd"), T(""));
	
	int ienable = gatoi(enable);
	int icallmode = gatoi(callmode);
	int icllint = gatoi(cllint);

	//Terminals, SIP account DB
	char pVoiceLineSignalingQry [1024] = {0};
	char pVoiceLineQry [1024] = {0};
	char pAuthCfgQry [1024] = {0};
	memset (pVoiceLineSignalingQry, 0x00, sizeof(pVoiceLineSignalingQry));
	memset (pVoiceLineQry, 0x00, sizeof(pVoiceLineQry));
	memset (pAuthCfgQry, 0x00, sizeof(pAuthCfgQry));
	
    //Set Terminals DB
    if (gstrcmp(enable,"on")==0)
    {
#if defined(__P02001_Dora) || defined(__P02002_Dora)    
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d', Name='%s', LineMode='%d', Intrusion='%d' where LineId = 1", 1, linename, icallmode, icllint);
#else
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d', Name='%s', LineMode='%d', Intrusion='%d' where LineId = 2", 1, linename, icallmode, icllint);
#endif
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	} else {
#if defined(__P02001_Dora) || defined(__P02002_Dora)   	
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d' where LineId = 1", 0);
#else
	  snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set State='%d' where LineId = 2", 0);
#endif
#ifdef __USED_WEBS_DB__
	  status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	  status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif
	}

	//Set SIP account DB
#if defined(__P02001_Dora) || defined(__P02002_Dora)  	
	snprintf (pVoiceLineSignalingQry, sizeof(pVoiceLineSignalingQry),
		"update VoiceLineSignaling set SipDispName='%s', SipUserName='%s' where LineId = 1", displayName, userName);
#else
	snprintf (pVoiceLineSignalingQry, sizeof(pVoiceLineSignalingQry),
		"update VoiceLineSignaling set SipDispName='%s', SipUserName='%s' where LineId = 2", displayName, userName);
#endif
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineSignalingQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineSignalingQry);
#endif
#if defined(__P02001_Dora) || defined(__P02002_Dora)
	snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set DirName='%s' where LineId = 1", displayName);
#else
	snprintf (pVoiceLineQry, sizeof(pVoiceLineQry),
		"update VoiceLine set DirName='%s' where LineId = 2", userName);
#endif

#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pVoiceLineQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pVoiceLineQry);
#endif

#if defined(__P02001_Dora) || defined(__P02002_Dora)
	snprintf (pAuthCfgQry, sizeof(pAuthCfgQry),
		"update AuthCfg set SipAuthUsrName='%s', SipAuthPasswd='%s' where LineId = 1", authName, pwd);
#else
	snprintf (pAuthCfgQry, sizeof(pAuthCfgQry),
		"update AuthCfg set SipAuthUsrName='%s', SipAuthPasswd='%s' where LineId = 2", authName, pwd);
#endif
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pAuthCfgQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pAuthCfgQry);
#endif

	if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");

    websRedirect(wp, T("voip/voip_linesettings.asp"));
}

static void setSystemDebug (webs_t wp, char_t *path, char_t *query)
{
    int status = -1;

	char_t * callmanager_type = websGetVar(wp, T("callmanager_type"), T(""));
    char_t * callmanager_level = websGetVar(wp, T("callmanager_level"), T(""));
    char_t * mediamanager_type = websGetVar(wp, T("mediamanager_type"), T(""));
	char_t * mediamanager_level = websGetVar(wp, T("mediamanager_level"), T(""));
	char_t * agents_type = websGetVar(wp, T("agents_type"), T(""));
    char_t * agents_level = websGetVar(wp, T("agents_level"), T(""));
    char_t * vmapi_type = websGetVar(wp, T("vmapi_type"), T(""));
	char_t * vmapi_level = websGetVar(wp, T("vmapi_level"), T(""));
	char_t * rtp_type = websGetVar(wp, T("rtp_type"), T(""));
    char_t * rtp_level = websGetVar(wp, T("rtp_level"), T(""));
    char_t * sip_type = websGetVar(wp, T("sip_type"), T(""));
	char_t * sip_level = websGetVar(wp, T("sip_level"), T(""));
	char_t * fax_type = websGetVar(wp, T("fax_type"), T(""));
	char_t * fax_level = websGetVar(wp, T("fax_level"), T(""));
	int icallmanager_type = gatoi(callmanager_type);
    int icallmanager_level = gatoi(callmanager_level);
    int imediamanager_type = gatoi(mediamanager_type);
	int imediamanager_level = gatoi(mediamanager_level);
	int iagents_type = gatoi(agents_type);
    int iagents_level = gatoi(agents_level);
    int ivmapi_type = gatoi(vmapi_type);
	int ivmapi_level = gatoi(vmapi_level);
	int irtp_type = gatoi(rtp_type);
    int irtp_level = gatoi(rtp_level);
    int isip_type = gatoi(sip_type);
	int isip_level = gatoi(sip_level);
	int ifax_type = gatoi(fax_type);
	int ifax_level = gatoi(fax_level);

    char pDbgSettingQry [1024] = {0};
	memset (pDbgSettingQry, 0x00, sizeof(pDbgSettingQry));

	snprintf (pDbgSettingQry, sizeof(pDbgSettingQry),
		"update DbgSetting set "
		"CmDbg_DbgType='%d', CmDbg_DbgLvl='%d', MmDbg_DbgType='%d', MmDbg_DbgLvl='%d',"
		"AgentsDbg_DbgType='%d', AgentsDbg_DbgLvl='%d', VmapiDbg_DbgType='%d', VmapiDbg_DbgLvl='%d',"
		"RtpDbg_DbgType='%d', RtpDbg_DbgLvl='%d', SipDbg_DbgType='%d', SipDbg_DbgLvl='%d',"
		"FaxDbg_DbgType='%d', FaxDbg_DbgLvl='%d'"
		, icallmanager_type, icallmanager_level, imediamanager_type, imediamanager_level
		, iagents_type, iagents_level, ivmapi_type, ivmapi_level
	    , irtp_type, irtp_level, isip_type, isip_level
	    , ifax_type, ifax_level);
#ifdef __USED_WEBS_DB__
	status = jnrp_sql_dbEXE_Webs (pDbgSettingQry);
#else
	status = jnrp_sql_dbEXE (pDBFile,pDbgSettingQry);
#endif

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
    }

	system("sh -x /etc/rc.d/rc.bringup_voip_start 0 ip");

    websRedirect(wp, T("voip/voip_system_debug.asp"));
}
