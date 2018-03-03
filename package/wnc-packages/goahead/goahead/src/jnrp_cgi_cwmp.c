#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

/*
 * Schema of TR069
 *
 * CREATE TABLE tr69Config
(
    URL			text,
    Username	text,
    Password	text,
    STUNEnable	text,
    STUNServerAddress	text,
    STUNUsername	text,
    STUNPassword	text,
    SerialNumber	text,
    ConnectionRequestUsername	text,
    ConnectionRequestPassword	text,
    CACertPath		text,
    ClientCertPath	text,
    Passphrase		text,
    ProvisioningCode text,
    PRIMARY KEY (URL)
);
 *
 */

void initCWMPWeb();
static int getCWMPSettings(int eid, webs_t wp, int argc, char_t **argv);
static void setCWMPSettings(webs_t wp, char_t *path, char_t *query);
static int getTR69Enable(int eid, webs_t wp, int argc, char_t **argv);
static int getTR69Data(int eid, webs_t wp, int argc, char_t **argv);
static int getPort(int eid, webs_t wp, int argc, char_t **argv);
static void setTR69Reboot(webs_t wp, char_t *path, char_t *query);

void initCWMPWeb()
{
	websAspDefine(T("getCWMPSettings"), getCWMPSettings);
    websFormDefine(T("setCWMPSettings"), setCWMPSettings);
	websAspDefine(T("getTR69Enable"), getTR69Enable);
	websAspDefine(T("getTR69Data"), getTR69Data);
	websAspDefine(T("getPort"), getPort);
	websFormDefine(T("setTR69Reboot"), setTR69Reboot);
    return ;
}

static int getCWMPSettings(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char *pCWMPInfo = (char *)malloc(128);
	memset (pCWMPInfo, 0x00, sizeof(pCWMPInfo));
	char * pQry = "select ProvisioningCode, URL, Username, password, ConnectionRequestUsername, ConnectionRequestPassword, Passphrase, STUNServerAddress, STUNUsername, STUNPassword  from tr69Config";
#ifdef __USED_WEBS_DB__
    char ** ppQryResult = jnrp_sql_getByQry_Webs (pQry);
#else
    char ** ppQryResult = jnrp_sql_getByQry (pDBFile, pQry);
#endif
    int i;

    if (argv[0])
    {
        if ((gstrcmp(argv[0],"provisioningCode") == 0) && ppQryResult[10])
        	strcpy(pCWMPInfo,ppQryResult[10]);
        else if ((gstrcmp(argv[0],"acsURL") == 0) && ppQryResult[11])
        	strcpy(pCWMPInfo,ppQryResult[11]);
        else if ((gstrcmp(argv[0],"username") == 0) && ppQryResult[12])
        	strcpy(pCWMPInfo,ppQryResult[12]);
        else if ((gstrcmp(argv[0],"password") == 0) && ppQryResult[13])
        	strcpy(pCWMPInfo,ppQryResult[13]);
        else if ((gstrcmp(argv[0],"cruUsername") == 0) && ppQryResult[14])
        	strcpy(pCWMPInfo,ppQryResult[14]);
        else if ((gstrcmp(argv[0],"cruPassword") == 0) && ppQryResult[15])
        	strcpy(pCWMPInfo,ppQryResult[15]);
        else if ((gstrcmp(argv[0],"certPassword") == 0) && ppQryResult[16])
        	strcpy(pCWMPInfo,ppQryResult[16]);
        else if ((gstrcmp(argv[0],"stunURL") == 0) && ppQryResult[17])
        	strcpy(pCWMPInfo,ppQryResult[17]);
        else if ((gstrcmp(argv[0],"stunUsername") == 0) && ppQryResult[18])
        	strcpy(pCWMPInfo,ppQryResult[18]);
        else if ((gstrcmp(argv[0],"stunPassword") == 0) && ppQryResult[19])
        	strcpy(pCWMPInfo,ppQryResult[19]);
        else
            strcpy(pCWMPInfo,"");
    }

	ret = websWrite(wp, T("%s"), pCWMPInfo);
	free(pCWMPInfo);
    return ret;
}

static int getTR69Enable(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs("tr69Config", "Enable",1);
#else
    char * pQryResult = jnrp_sql_getAttr(pDBFile,"tr69Config", "Enable",1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

static int getTR69Data(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs("tr69Config", "InterfaceName",1);
#else
    char * pQryResult = jnrp_sql_getAttr(pDBFile,"tr69Config", "InterfaceName",1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

static int getPort(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs("tr69Config", "ConnectionRequestPort",1);
#else
    char * pQryResult = jnrp_sql_getAttr(pDBFile,"tr69Config", "ConnectionRequestPort",1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

static void setCWMPSettings(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);
    char_t * pprovisioningCode = websGetVar(wp, T("provisioningCode"), T(""));
    char_t * pacsURL = websGetVar(wp, T("acsURL"), T(""));
    char_t * pusername = websGetVar(wp, T("username"), T(""));
    char_t * ppassword = websGetVar(wp, T("password"), T(""));
    char_t * pcruUsername = websGetVar(wp, T("cruUsername"), T(""));

    char_t * pcruPassword = websGetVar(wp, T("cruPassword"), T(""));
    char_t * pcertPassword = websGetVar(wp, T("certPassword"), T(""));
    char_t * pstunURL = websGetVar(wp, T("stunURL"), T(""));
    char_t * pstunUsername = websGetVar(wp, T("stunUsername"), T(""));
    char_t * pstunPassword = websGetVar(wp, T("stunPassword"), T(""));
	char_t * pEnable = websGetVar(wp, T("tr69_en"), T("off"));

	char_t * pInterface = websGetVar(wp, T("interface_set"), T(""));
	char_t * pPort = websGetVar(wp, T("port"), T(""));

	if (strcmp(websGetVar(wp, T("tr69_en"), T("")), "on") == 0)
	{
		pEnable = "1";
	} else {
		pEnable = "0";
	}

 	char pCWMPQry [1024] = {0};
	memset (pCWMPQry, 0x00, sizeof(pCWMPQry));
	int status = -1;

 	snprintf (pCWMPQry, sizeof(pCWMPQry),"update tr69Config set "
 			"ProvisioningCode='%s', "
 			"Enable='%s', "
			"URL='%s', "
			"Username='%s', "
			"Password='%s', "
			"ConnectionRequestUsername='%s', "
			"ConnectionRequestPassword='%s', "
			"Passphrase='%s', "
			"STUNServerAddress='%s', "
			"STUNUsername='%s', "
			"STUNPassword='%s', "
			"InterfaceName='%s', "
			"ConnectionRequestPort='%s' ",
			pprovisioningCode,pEnable,pacsURL,pusername,ppassword,pcruUsername,pcruPassword,
			pcertPassword,pstunURL,pstunUsername,pstunPassword,pInterface,pPort);
		//printf("pDhcpSerQry : %s \n",pDhcpSerQry);
#ifdef __USED_WEBS_DB__
		status =  jnrp_sql_dbEXE_Webs(pCWMPQry);
#else
		status =  jnrp_sql_dbEXE(pDBFile, pCWMPQry);
#endif

		if (status>=0)
	{
		// export db to ascii
		printf("pCWMPQry : %s \n", pCWMPQry);
		system(EXPORT_CMD);
/*		
		system("tr69Init");
		if (strcmp(websGetVar(wp, T("interface_set"), T("")), "usb0") == 0){
			system("tr69_resolv.sh usb0 &");
		} else if (strcmp(websGetVar(wp, T("interface_set"), T("")), "usb1") == 0) {
			system("tr69_resolv.sh usb1 &");
		} 
#ifdef __P01003_UNE
        else if (strcmp(websGetVar(wp, T("interface_set"), T("")), "usb2") == 0){
			system("tr69_resolv.sh usb2 &");
        }
#endif
*/
	}	
    websRedirect(wp, T("system/tr69_reboot.asp"));
}

// system reboot
static void setTR69Reboot(webs_t wp, char_t *path, char_t *query)
{
	websWrite(wp, T("\n\
<html>\n\
<head>\n\
<TITLE>System Reboot</TITLE>\n\
<link rel=stylesheet href=/style/normal_ws.css type=text/css>\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=iso-8859-1\">\n\
<script language=\"javascript\">\n\
var count=120;\n\
var counter=setInterval(displayRebootMsg, 1000); \n\
function displayRebootMsg()\n\
{	var chart = document.getElementById(\"countdown\");\n\
	chart.innerHTML = 'Please wait &nbsp;&nbsp;' + count + '&nbsp;&nbsp;seconds for the system reboot.';\n\
    count=count-1;\n\
    if (count <= 0)\n\
    {clearInterval(counter);window.location.href=\"../login.asp\";\n\  return;}\n\
}\n\
</script>\n\
</head>\n\
<body><h1> System Reboot </h1>\n\
<div id=\"countdown\" class=\"msg_container\" style=\"line-height:62px; text-align:center;\"> Please wait &nbsp;&nbsp;120&nbsp;&nbsp;seconds for the system reboot.</div>\n\
</body>\n\
</html>\n"));
    printf("EXEC: reboot\n");
    system("reboot");
}	
