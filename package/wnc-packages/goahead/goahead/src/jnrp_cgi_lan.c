#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

void initLanWeb();

// LAN Basic Setup
static int getLanBasic(int eid, webs_t wp, int argc, char_t **argv);
static void setLanInfoValue(webs_t wp, char_t *path, char_t *query);

// Dhcp Server
static int getDhcpServBasic (int eid, webs_t wp, int argc, char_t **argv);
static void setDhcpServBasic(webs_t wp, char_t *path, char_t *query);
static int getDhcpClients (int eid, webs_t wp, int argc, char_t **argv);
static int getLanClients (int eid, webs_t wp, int argc, char_t **argv);
static int getDhcpIpRes (int eid, webs_t wp, int argc, char_t **argv);
static void setDhcpIpRes(webs_t wp, char_t *path, char_t *query);
static int getDhcpIpResEnable (int eid, webs_t wp, int argc, char_t **argv);
static void setDhcpIpResEnable(webs_t wp, char_t *path, char_t *query);
static void setDhcpIpResDel(webs_t wp, char_t *path, char_t *query);
static int getLanIpPrefix (int eid,webs_t wp,int argc,char_t * * argv);
static void updateClientList(webs_t wp, char_t *path, char_t *query);
#if defined(__P02003_Dora)
static int getWiFiClients (int eid, webs_t wp, int argc, char_t **argv);
static int getDhcpReservationList(int eid, webs_t wp, int argc, char_t **argv);
static void setDhcpReservation(webs_t wp, char_t *path, char_t *query);
int ip_res_list_idx=-1;
static int getDhcpReservationConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setDhcpReservationConfig(webs_t wp, char_t *path, char_t *query);
enum
{
	reservation_none=0,
	reservation_err_mac=1,
	reservation_err_ip=2
};
int P02003_dhcp_reservation_status = reservation_none;
static int getDhcpReservationErrorStatus(int eid, webs_t wp, int argc, char_t **argv);

//DNS & DynDNS
static int getDNSInfo(int eid, webs_t wp, int argc, char_t **argv);
static int getDDNSInfo(int eid, webs_t wp, int argc, char_t **argv);
static void setDNSInfo(webs_t wp, char_t *path, char_t *query);
static void setDDNSInfo(webs_t wp, char_t *path, char_t *query);
#endif

void initLanWeb()
{
    websAspDefine(T("getLanBasic"), getLanBasic);
    websFormDefine(T("setLanInfoValue"), setLanInfoValue);
    websAspDefine(T("getLanClients"), getLanClients);
    websFormDefine(T("setDhcpServBasic"), setDhcpServBasic);
    websAspDefine(T("getDhcpServBasic"), getDhcpServBasic);
    websFormDefine(T("setDhcpIpRes"), setDhcpIpRes);
    websAspDefine(T("getDhcpIpRes"), getDhcpIpRes);
    websAspDefine(T("getDhcpIpResEnable"), getDhcpIpResEnable);
    websFormDefine(T("setDhcpIpResEnable"), setDhcpIpResEnable);
    websFormDefine(T("setDhcpIpResDel"), setDhcpIpResDel);
    websAspDefine(T("getLanIpPrefix"), getLanIpPrefix);
    websFormDefine(T("updateClientList"), updateClientList);
#if defined(__P02003_Dora)
    websAspDefine(T("getWiFiClients"), getWiFiClients);
    websAspDefine(T("getDhcpReservationList"), getDhcpReservationList);
    websFormDefine(T("setDhcpReservation"), setDhcpReservation);
    websAspDefine(T("getDhcpReservationConfig"), getDhcpReservationConfig);
    websFormDefine(T("setDhcpReservationConfig"), setDhcpReservationConfig);
    websAspDefine(T("getDhcpReservationErrorStatus"), getDhcpReservationErrorStatus);
    websAspDefine(T("getDNSInfo"), getDNSInfo);
    websAspDefine(T("getDDNSInfo"), getDDNSInfo);
    websFormDefine(T("setDNSInfo"), setDNSInfo);
    websFormDefine(T("setDDNSInfo"), setDDNSInfo);
#endif
    return ;
}

static int getLanBasic(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * hostname = jnrp_sql_getAttr_Webs ("system", "name", 1);
#else
    char * hostname = jnrp_sql_getAttr (pDBFile, "system", "name",1);
#endif
	int ret = 0;
    char *pLanInfo = (char *)malloc(128);
    memset (pLanInfo, 0x00, sizeof(pLanInfo));
    char * pQry = "select IpAddress, SubnetMask from Lan";
#ifdef __USED_WEBS_DB__
    char ** ppQryResult = jnrp_sql_getByQry_Webs (pQry);
#else
    char ** ppQryResult = jnrp_sql_getByQry (pDBFile, pQry);
#endif
    if (argv[0])
    {
        if ((gstrcmp(argv[0],"hostname") == 0)&& hostname)
            strcpy(pLanInfo,hostname);
        else if ((gstrcmp(argv[0],"IpAddress") == 0) && ppQryResult[2])
            strcpy(pLanInfo,ppQryResult[2]);
        else if ((gstrcmp(argv[0],"SubnetMask") == 0) && ppQryResult[3])
            strcpy(pLanInfo,ppQryResult[3]);
        else
            strcpy(pLanInfo,"");
    }
	ret = websWrite(wp, T("%s"), pLanInfo);
	free(pLanInfo);
    return ret;
}

static void setLanInfoValue(webs_t wp, char_t *path, char_t *query)
{
    // get values from web gui page
    char_t * pLanIp = websGetVar(wp, T("lan_ip"), T(""));
    char_t * pLanSubmask = websGetVar(wp, T("lan_submask"), T(""));
    char_t * pHost = websGetVar(wp, T("lan_hostname"), T(""));

    // init
    char pLanQry [1024] = {0};
    char pSysQry [1024] = {0};
    char pNetworkQry [1024] = {0};
    char pDhcpServer [1024] = {0};
    //char * pSysQry = "update system set name='%s', domain='%s' where _ROWID_='1'";
    memset (pLanQry, 0x00, sizeof(pLanQry));
    memset (pSysQry, 0x00, sizeof(pSysQry));
    memset (pNetworkQry, 0x00, sizeof(pNetworkQry));
    memset (pDhcpServer, 0x00, sizeof(pDhcpServer));

    int status = -1;

    // change system name
    if (gstrlen(pHost))
    {
        snprintf (pSysQry, sizeof(pSysQry),"update system set name='%s' where _ROWID_='1'",pHost);
        //printf("pSysQry : %s \n",pSysQry);
#ifdef __USED_WEBS_DB__
        status =  jnrp_sql_dbEXE_Webs (pSysQry);
#else
        status =  jnrp_sql_dbEXE (pDBFile, pSysQry);
#endif
    }

    //char * pLanQry = "update Lan set IpAddress='%s', SubnetMask='%s' where _ROWID_='1'";
    if (gstrlen(pLanIp) && gstrlen(pLanSubmask))
    {
        // change lan ip
        snprintf (pLanQry, sizeof(pLanQry),"update Lan set IpAddress='%s', SubnetMask='%s' where _ROWID_='1'",pLanIp,pLanSubmask);
        // change network interface lan ip
        //snprintf (pNetworkQry, sizeof(pNetworkQry),"update networkInterface set ipaddr='%s', subnetmask='%s' where interfaceName='br0'",pLanIp,pLanSubmask);
#ifdef __USED_WEBS_DB__
        status =  jnrp_sql_dbEXE_Webs (pLanQry);
#else
        status =  jnrp_sql_dbEXE (pDBFile, pLanQry);
#endif
        //status =  jnrp_sql_dbEXE(pDBFile, pNetworkQry);
    }

    // change dhcp server ip range domain

    if (gstrlen(pLanIp))
    {
        char * pIpStart;
        char * pIpEnd;
        char * pIpPrefix1 = (char *)malloc(24);
        char * pIpPrefix2 = (char *)malloc(24);
        char * temp1;
        char ** ppLanIp = NULL;
        char *temp2 = (char *)malloc(24);
        strcpy(temp2,pLanIp);
        temp1 = strtok (temp2, ".");
        int count = 1;
        while (temp1 != NULL)
        {
            //printf("count: %d, temp1:%s\n",count,temp1);
            if (count == 1)
                strcpy(pIpPrefix1,temp1);
            else if ((count < 4) && (count > 1))
            {
                strcat(pIpPrefix1,".");
                strcat(pIpPrefix1,temp1);
            }
            temp1 = strtok (NULL, ".");
            count++;
        }
        strcat(pIpPrefix1,".");
#ifdef __USED_WEBS_DB__
        pIpStart = getSplitStr(jnrp_sql_getAttr_Webs ("DhcpServerBasic", "StartIpAddress",1), ".", 4);
        pIpEnd = getSplitStr(jnrp_sql_getAttr_Webs ("DhcpServerBasic", "EndIpAddress",1), ".", 4);
#else
        pIpStart = getSplitStr(jnrp_sql_getAttr (pDBFile, "DhcpServerBasic", "StartIpAddress",1), ".", 4);
        pIpEnd = getSplitStr(jnrp_sql_getAttr (pDBFile, "DhcpServerBasic", "EndIpAddress",1), ".", 4);
#endif
        strcpy(pIpPrefix2,pIpPrefix1);
        strcat(pIpPrefix1,pIpStart);
        strcat(pIpPrefix2,pIpEnd);
        snprintf (pDhcpServer, sizeof(pDhcpServer),"update DhcpServerBasic set StartIpAddress='%s', EndIpAddress='%s' where _ROWID_='1'",pIpPrefix1,pIpPrefix2);
#ifdef __USED_WEBS_DB__
        status =  jnrp_sql_dbEXE_Webs (pDhcpServer);
#else
        status =  jnrp_sql_dbEXE (pDBFile, pDhcpServer);
#endif
		free(pIpStart);
		free(pIpEnd);
        free(pIpPrefix1);
        free(pIpPrefix2);
        free(temp2);
    }

    if (status>=0)
    {
      // export db to ascii
      system(EXPORT_CMD);
    }

    websRedirect(wp, T("lan/lan_settings.asp"));
}

#if defined(__P02003_Dora)
static int getLanClients (int eid, webs_t wp, int argc, char_t **argv)
{
 system("querydev > /dev/null 2>&1");
 FILE *fp = fopen("/tmp/connectdev", "r");
 char line[512];
 memset(line, 0x00, sizeof(line));
 if(fp)
 {
 	int i =0;
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';

   if (!gstrcmp(getSplitStr(line, " ", 3), "LAN"))
   {
     if (i%2)
     {
       websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 5), "(null)") == 0 ? "" : getSplitStr(line, " ", 5));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 1), "(null)") == 0 ? "" : getSplitStr(line, " ", 1));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 2), "(null)") == 0 ? "" : getSplitStr(line, " ", 2));
       websWrite(wp, T("</tr>"));
     }
     else
     {
       websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 5), "(null)") == 0 ? "" : getSplitStr(line, " ", 5));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 1), "(null)") == 0 ? "" : getSplitStr(line, " ", 1));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 2), "(null)") == 0 ? "" : getSplitStr(line, " ", 2));
       websWrite(wp, T("</tr>"));
     }
     i++;
   }
  }
  fclose(fp);
 }

 return 0;
}
#else
static int getLanClients (int eid, webs_t wp, int argc, char_t **argv)
{
#if 1 //New
 FILE *fp = fopen("/tmp/connectdev", "r");
 char line[512];
 memset(line, 0x00, sizeof(line));
 if(fp)
 {
 	int i =0;
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';
     if (i%2)
     {
       websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
     }
     else
     {
       websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
     }
     websWrite(wp, T("<td>%s</td>"),gstrcmp(getSplitStr(line, " ", 5), "(null)") == 0 ? "" : getSplitStr(line, " ", 5));
     websWrite(wp, T("<td>%s</td>"),gstrcmp(getSplitStr(line, " ", 1), "(null)") == 0 ? "" : getSplitStr(line, " ", 1));
     websWrite(wp, T("<td>%s</td>"),gstrcmp(getSplitStr(line, " ", 2), "(null)") == 0 ? "" : getSplitStr(line, " ", 2));
     websWrite(wp, T("</tr>"));
     i++;
  }
  fclose(fp);
 }

 return 0;
#else
#ifdef __USED_WEBS_DB__
    int count = jnrp_sql_getRowNum_Webs ("DhcpLeasedClients");
#else
    int count = jnrp_sql_getRowNum ("DhcpLeasedClients",pDBFile);
#endif
    char * pQry = "select hostName, IPAddr, MacAddr from DhcpLeasedClients";
#ifdef __USED_WEBS_DB__
    char ** ppQryResult = jnrp_sql_getByQry_Webs (pQry);
#else
    char ** ppQryResult = jnrp_sql_getByQry (pDBFile, pQry);
#endif
    int nShift = 3;

        if (ppQryResult)
        {
                int i =0;
                for (i=0;i<count;i++)
                {
                        if (i%2)
                        {
                                websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
                                websWrite(wp, T("<td>%s</td>"),ppQryResult[nShift+nShift*i]);
                                websWrite(wp, T("<td>%s</td>"),ppQryResult[nShift+nShift*i+1]);
                                websWrite(wp, T("<td>%s</td>"),ppQryResult[nShift+nShift*i+2]);
                                websWrite(wp, T("</tr>"));
                        }
                        else
                        {
                                websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
                                websWrite(wp, T("<td>%s</td>"),ppQryResult[nShift+nShift*i]);
                                websWrite(wp, T("<td>%s</td>"),ppQryResult[nShift+nShift*i+1]);
                                websWrite(wp, T("<td>%s</td>"),ppQryResult[nShift+nShift*i+2]);
                                websWrite(wp, T("</tr>"));
                        }
                }
    }

    return 0;
#endif
}
#endif

static int getDhcpServBasic (int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
    char *pDhcpServ = (char *)malloc(128);
    memset (pDhcpServ, 0x00, sizeof(pDhcpServ));
    char pDhcpQry [1024] = {0};
    memset (pDhcpQry, 0x00, sizeof(pDhcpQry));
    if ( (argc == 2 )&& argv[0] && argv[1])
    {
        snprintf (pDhcpQry, sizeof(pDhcpQry),"select %s from %s",argv[0],argv[1]);
#ifdef __USED_WEBS_DB__
        char ** ppQryResult = jnrp_sql_getByQry_Webs (pDhcpQry);
#else
        char ** ppQryResult = jnrp_sql_getByQry (pDBFile, pDhcpQry);
#endif
        if (ppQryResult)
        {
                        if (ppQryResult[1] && (!strcmp(argv[0],"Enable")))
                                strcpy(pDhcpServ,ppQryResult[1]);
                        else if (ppQryResult[1] && (!strcmp(argv[0],"StartIpAddress")))
                                strcpy(pDhcpServ,ppQryResult[1]);
                        else if (ppQryResult[1] && (!strcmp(argv[0],"EndIpAddress")))
                                strcpy(pDhcpServ,ppQryResult[1]);
                        else if (ppQryResult[1] && (!strcmp(argv[0],"LeaseTime")))
                                strcpy(pDhcpServ,ppQryResult[1]);
                        else if (ppQryResult[1] && (!strcmp(argv[0],"domain")))
                                strcpy(pDhcpServ,ppQryResult[1]);
                        else if (ppQryResult[1] && (!strcmp(argv[0],"Range")))
                                strcpy(pDhcpServ,ppQryResult[1]);
                        else
                                strcpy(pDhcpServ,"");
        }else
                        strcpy(pDhcpServ,"");
    }
    else
        strcpy(pDhcpServ,"");

	ret = websWrite(wp, T("%s"), pDhcpServ);
	free(pDhcpServ);
    return ret;
}

#if defined(__P02003_Dora)
static void setDhcpServBasic(webs_t wp, char_t *path, char_t *query)
{
 char_t * pRange = websGetVar(wp, T("num_devices"), T(""));
 char_t * pStartIp = websGetVar(wp, T("dhcp_ip_start"), T(""));
 char pDhcpSerQry [1024] = {0};
 int status = -1;

 //printf("pRange: %s, pStartIp: %s\n", pRange, pStartIp);
 memset (pDhcpSerQry, 0x00, sizeof(pDhcpSerQry));

 if (gstrlen(pRange) && gstrlen(pStartIp))
 {
 	 char * pIP = jnrp_sql_getAttr_Webs ("DhcpServerBasic", "StartIpAddress", 1);
   char * pIpPrefix1 = getSplitStr(pIP, ".", 1);
   char * pIpPrefix2 = getSplitStr(pIP, ".", 2);
   char * pIpPrefix3 = getSplitStr(pIP, ".", 3);
   char pIpPrefixArr[32];
   memset(pIpPrefixArr,'\0',sizeof(pIpPrefixArr));
   snprintf (pIpPrefixArr, sizeof(pIpPrefixArr), "%s.%s.%s.", pIpPrefix1, pIpPrefix2, pIpPrefix3);

   int nIpStart = gatoi(pStartIp);
   int Range = gatoi(pRange);
   int nIpEnd = nIpStart + Range - 1;
   if (nIpEnd > 254)
   {
   		nIpEnd = 254;
   }
   char pIpStartQry [64] = {0};
   memset (pIpStartQry, '\0', sizeof(pIpStartQry));
   snprintf (pIpStartQry, sizeof(pIpStartQry), "%s%s", pIpPrefixArr, pStartIp);

   char pIpEndQry [64] = {0};
   memset (pIpEndQry, '\0', sizeof(pIpEndQry));
   snprintf (pIpEndQry, sizeof(pIpEndQry), "%s%d", pIpPrefixArr, nIpEnd);

   //printf("pStartIp: %s, pIpEnd: %s\n", pIpStartQry, pIpEndQry);
   snprintf (pDhcpSerQry, sizeof(pDhcpSerQry),"update DhcpServerBasic set Enable='%d', StartIpAddress='%s', EndIpAddress='%s', Range='%d'", 1, pIpStartQry, pIpEndQry, Range);
   printf("pDhcpSerQry : %s \n",pDhcpSerQry);   
   status =  jnrp_sql_dbEXE_Webs(pDhcpSerQry);

   free(pIpPrefix1);
   free(pIpPrefix2);
   free(pIpPrefix3);   
 }

 if (status>=0)
 {
   // export db to ascii
   system(EXPORT_CMD);
 }

 websRedirect(wp, T("lan/lan_dhcpserver.asp"));
 sleep(3);
}
#else
static void setDhcpServBasic(webs_t wp, char_t *path, char_t *query)
{
    //printf("setDhcpServBasic test \n");
    char_t * pServerMode = websGetVar(wp, T("dhcp_mode"), T(""));
    //printf("pServerMode: %s \n",pServerMode);
    char_t * pLocalDomain = websGetVar(wp, T("DHCP_localdomain"), T(""));
    char_t * pStartIp = websGetVar(wp, T("DHCP_poolstart"), T(""));
    char_t * pEndIp = websGetVar(wp, T("DHCP_poolend"), T(""));
    char_t * pLeaseTime = websGetVar(wp, T("DHCP_leasetime_value"), T(""));
    int range=0;
    //printf("pLeaseTime: %s \n",pLeaseTime);
    int nEnable = gatoi(pServerMode);
    // calculate lease time
    int nleaseTime = gatoi(pLeaseTime);

    char pDhcpSerQry [1024] = {0};
	char pSysQry [1024] = {0};
    memset (pDhcpSerQry, 0x00, sizeof(pDhcpSerQry));
	memset (pSysQry, 0x00, sizeof(pSysQry));
    int status = -1;
    char * pIpStart;
    char * pIpEnd;

    if (gstrlen(pStartIp) && gstrlen(pEndIp))
    {
       pIpStart = getSplitStr(pStartIp, ".", 4);
       pIpEnd = getSplitStr(pEndIp, ".", 4);
       int nIpStart = gatoi(pIpStart);
       int nIpEnd = gatoi(pIpEnd);
       int nIpRange = 0;
       if (nIpEnd > nIpStart)
               nIpRange = nIpEnd - nIpStart;
       //printf("pIpStart: %s, pIpEnd: %s\n",pIpStart,pIpEnd);
       snprintf (pDhcpSerQry, sizeof(pDhcpSerQry),"update DhcpServerBasic set Enable='%d', StartIpAddress='%s', "
       "EndIpAddress='%s', LeaseTime='%d', Range='%d' ",nEnable,pStartIp,pEndIp,nleaseTime,nIpRange);
       //printf("pDhcpSerQry : %s \n",pDhcpSerQry);
#ifdef __USED_WEBS_DB__
       status =  jnrp_sql_dbEXE_Webs(pDhcpSerQry);
#else
       status =  jnrp_sql_dbEXE(pDBFile, pDhcpSerQry);
#endif
	   snprintf (pSysQry, sizeof(pSysQry),"update system set domain='%s' where _ROWID_='1'", pLocalDomain);
#ifdef __USED_WEBS_DB__
       status =  jnrp_sql_dbEXE_Webs(pSysQry);
#else
       status =  jnrp_sql_dbEXE(pDBFile, pSysQry);
#endif
	   free(pIpStart);
	   free(pIpEnd);
    }

    if (status>=0)
    {
      // export db to ascii
      system(EXPORT_CMD);
    }
    //websRedirect(wp, T("lan/lan_dhcpserver.asp"));
}
#endif

static int getDhcpIpRes (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from DhcpfixedIpAddress";

 int index = 0;
 char* pPoolID = NULL;
 char* pEnable = NULL;
 char* pIpAddr = NULL;
 char* pMacAddr = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "PoolID", pPoolID, getDhcpIpResOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getDhcpIpResOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "IpAddr", pIpAddr, getDhcpIpResOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "MacAddr", pMacAddr, getDhcpIpResOut);

    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\" align=\"center\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\" align=\"center\">"));
    }
    websWrite(wp, T("<td><input type=\"radio\" name=\"ipres_ck\" id=\"ipres_ck_%d\" onclick=\"setIpResEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td id=\"ipres_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td><span id=\"ipres_ip_%d\">%s</span></td>\n"), index+1, pIpAddr);
    websWrite(wp, T("<td id=\"ipres_mac_%d\">%s</td>\n"), index+1, pMacAddr);
  }
 }

getDhcpIpResOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    //char   sqlCmd [1024] = {0};
    //int count = jnrp_sql_getRowNum("DhcpfixedIpAddress",pDBFile);
    char * sqlCmd = "select * from DhcpfixedIpAddress";
    const char      *data = NULL;
	//printf("getDhcpIpRes : %s\n", sqlCmd);
	
    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    //snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);
    sqlite3_busy_timeout(db, 1500);
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
        //char* data = (const char*)sqlite3_column_text( stmt, i );
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\" align=\"center\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\" align=\"center\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"ipres_ck\" id=\"ipres_ck_%d\" onclick=\"setIpResEditRowId('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td id=\"ipres_enable_%d\">%s</td>\n"),row_count,
        gatoi((const char*)sqlite3_column_text( stmt, 1 ))?"Enabled":"Disabled");
        websWrite(wp, T("<td><span id=\"ipres_ip_%d\">%s</span></td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 2));
        websWrite(wp, T("<td id=\"ipres_mac_%d\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 3 ));
    }

    printf("getDhcpIpRes : row_count:%d \n",row_count);

    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
#endif
}

static void setDhcpIpRes(webs_t wp, char_t *path, char_t *query)
{
    //printf("setDhcpIpRes\n");
    char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    //int count = jnrp_sql_getRowNum("DhcpfixedIpAddress",pDBFile) + 1;
    char_t * pRowId = websGetVar(wp, T("ipres_rowid_edit"), T(""));
    //int nPoolID = atoi(pRowId);
    //printf("editflag:%s ,pRowId: %s \n",pEditMode,pRowId);
	//char * pPoolID = jnrp_sql_getAttr(pDBFile,"DhcpfixedIpAddress", "PoolID",1);
	//int nPoolID =1;
	printf("Row number:%d \n",pRowId);
	
		
    char * pErrMsg = NULL;
    int status = -1;

    char_t * pEnable = websGetVar(wp, T("ipres_enable"), T("off"));
    char_t * pIp = websGetVar(wp, T("ipres_ip"), T(""));
    char_t * pMac = websGetVar(wp, T("ipres_mac"), T(""));    
    //char_t * pAction = websGetVar(wp, T("ipres_act"), T(""));    
    //printf("pEnable:%s , pIp:%s, pMac:%s\n",pEnable,pIp,pMac);

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
        printf("setDhcpIpRes add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into DhcpfixedIpAddress values ('%d','%s','%s','%s')",pRowId, gstrcmp(pEnable,"off")?"1":"0",pIp, pMac);
        printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);

    }
    else if (!gstrcmp(pEditMode,"edit"))
    {
        printf("setDhcpIpRes edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update DhcpfixedIpAddress set PoolID='%s', Enable='%d', " 
        "IpAddr='%s', MacAddr='%s' where _ROWID_='%s'" ,pRowId, gstrcmp(pEnable,"off")?1:0, pIp, pMac, pRowId);
        printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        printf("export db to ascii\n");
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }      
	else
		printf("NOT export db to ascii\n");
		
    websRedirect(wp, T("lan/lan_ipreservation.asp"));
}

static int getDhcpIpResEnable (int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("DhcpServerBasic", "IpReserveEnable", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"DhcpServerBasic", "IpReserveEnable", 1);
#endif
    //printf("IP Reservation Enable: %s\n",pQryResult);
    return websWrite(wp, T("%s"),pQryResult);
}

static void setDhcpIpResEnable(webs_t wp, char_t *path, char_t *query)
{
	//printf("setDhcpIpResEnable. \n");
    char_t * pEnable = websGetVar(wp, T("enipres"), T("off"));

#ifdef __USED_WEBS_DB__
    if (!gstrcmp(pEnable,"on"))
    {
        jnrp_sql_setAttr_Webs("DhcpServerBasic", "IpReserveEnable", "1", 1);
    }
    else
    {
        jnrp_sql_setAttr_Webs("DhcpServerBasic", "IpReserveEnable", "0", 1);
    }
#else
	//printf("Set IP Reservation Enable: %s\n",pEnable);
    if (!gstrcmp(pEnable,"on"))
    {
		//printf("Set IP Reservation Enable\n");
        jnrp_sql_setAttr(pDBFile, "DhcpServerBasic", "IpReserveEnable", "1", 1);
	}
    else
    {
		//printf("Set IP Reservation disable\n");
        jnrp_sql_setAttr(pDBFile, "DhcpServerBasic", "IpReserveEnable", "0", 1);
	}
#endif

    // export db to ascii
    system(EXPORT_CMD);

    websRedirect(wp, T("lan/lan_ipreservation.asp"));
}

static void setDhcpIpResDel(webs_t wp, char_t *path, char_t *query)
{
	printf("setDhcpIpResDel \n");    
    char * pId = websGetVar(wp, T("ipres_rowid_del"), T(""));    
    printf("pId: %s\n",pId);	
    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
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
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM DhcpfixedIpAddress WHERE _ROWID_='%s'",pId);
        printf("DELETE FROM DhcpfixedIpAddress WHERE _ROWID_='%s' \n",pId);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
			printf("pPortDelQry: %s \n",pPortDelQry);
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
            system("/usr/local/bin/dhcpdInit");
        }
    }

    websRedirect(wp, T("lan/lan_ipreservation.asp"));
}

static int getLanIpPrefix (int eid, webs_t wp, int argc, char_t **argv)
{
	char pIpPrefixArr[32];
	char * pIP = jnrp_sql_getAttr_Webs ("DhcpServerBasic", "StartIpAddress", 1);
	char * pIpPrefix1 = getSplitStr(pIP, ".", 1);
	char * pIpPrefix2 = getSplitStr(pIP, ".", 2);
	char * pIpPrefix3 = getSplitStr(pIP, ".", 3);

	memset(pIpPrefixArr,'\0',sizeof(pIpPrefixArr));
 	gstrcat(pIpPrefixArr, pIpPrefix1);
	gstrcat(pIpPrefixArr,".");
	gstrcat(pIpPrefixArr, pIpPrefix2);
	gstrcat(pIpPrefixArr,".");
	gstrcat(pIpPrefixArr, pIpPrefix3);
	gstrcat(pIpPrefixArr,".");

	free(pIpPrefix1);
	free(pIpPrefix2);
	free(pIpPrefix3);

	return websWrite(wp, T("%s"), pIpPrefixArr);
}

static void updateClientList(webs_t wp, char_t *path, char_t *query)
{
 //char * pclient_list_value = websGetVar(wp, T("clientlist"), T(""));
 char * purl_clientlist_value = websGetVar(wp, T("url_clientlist"), T(""));
 //if (!gstrcmp(pclient_list_value,"0")) {
   //system("querydev > /dev/null 2>&1");
 //}
 if (!gstrcmp(purl_clientlist_value,"lan")) {
   system("querydev > /dev/null 2>&1");
   websRedirect(wp, T("lan/lan_clientlist.asp"));
 } else if (!gstrcmp(purl_clientlist_value,"wifi")) {
   system("querydev WIFI > /dev/null 2>&1");
   websRedirect(wp, T("wifi/wifi_clientlist.asp"));
 }
 return;
}

#if defined(__P02003_Dora)
static int getWiFiClients (int eid, webs_t wp, int argc, char_t **argv)
{
 system("querydev WIFI > /dev/null 2>&1");
 FILE *fp = fopen("/tmp/wificonnectdev", "r");
 char line[512];
 memset(line, 0x00, sizeof(line));
 if(fp)
 {
 	int i =0;
  while(fgets(line, sizeof(line), fp) != NULL)
  {
   if (line[strlen(line)-1] == '\n')
     line[strlen(line)-1] = '\0';

   if (gstrstr(getSplitStr(line, " ", 3), "WIFI"))
   { 
     if (i%2)
     {
       websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 5), "(null)") == 0 ? "" : getSplitStr(line, " ", 5));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 1), "(null)") == 0 ? "" : getSplitStr(line, " ", 1));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 2), "(null)") == 0 ? "" : getSplitStr(line, " ", 2));
       websWrite(wp, T("</tr>"));
     }
     else
     {
       websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 5), "(null)") == 0 ? "" : getSplitStr(line, " ", 5));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 1), "(null)") == 0 ? "" : getSplitStr(line, " ", 1));
       websWrite(wp, T("<td class=\"tdEven\">%s</td>"),gstrcmp(getSplitStr(line, " ", 2), "(null)") == 0 ? "" : getSplitStr(line, " ", 2));
       websWrite(wp, T("</tr>"));
     }
     i++;
   }
  }
  fclose(fp);
 }

 return 0;
}

static int getDhcpReservationList(int eid, webs_t wp, int argc, char_t **argv)
{
	 int index = 1;
	 //char* pEnable = NULL;
	 char* pIpAddr = NULL;
	 char* pMacAddr = NULL;
	 int nRows = jnrp_sql_getRowNum_Webs ("DhcpfixedIpAddress");
	 if (nRows>0)
	 {
	 	for (index=1; index<=nRows; index++)
	 	{
			  pIpAddr = jnrp_sql_getAttr_Webs ("DhcpfixedIpAddress", "IpAddr", index);
			  pMacAddr = jnrp_sql_getAttr_Webs ("DhcpfixedIpAddress", "MacAddr", index);
     //printf("index=%d, IpAddr=%s, MacAddr=%s\n", index, pIpAddr, pMacAddr);
     websWrite(wp, T("<tr>\n"));
     if (index%2)
     {
			    websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"IpResList.checkbox\" id=\"IpResList_id_%d\"></td>\n"), index);
			    websWrite(wp, T("<td id=\"IpResList_Mac_%d\" class=\"tdEven\">%s</td>\n"), index, pMacAddr);
		 	   websWrite(wp, T("<td id=\"IpResList_ip_%d\" class=\"tdEven\">%s</td>\n"), index, pIpAddr);
		 	 } else {
			    websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"IpResList.checkbox\" id=\"IpResList_id_%d\"></td>\n"), index);
			    websWrite(wp, T("<td id=\"IpResList_Mac_%d\" class=\"tdOdd\">%s</td>\n"), index, pMacAddr);
		 	   websWrite(wp, T("<td id=\"IpResList_ip_%d\" class=\"tdOdd\">%s</td>\n"), index, pIpAddr);
		 	 }
			  websWrite(wp, T("</tr>\n"));
	 	}
	 }
	 return 0;
}

static void setDhcpReservation(webs_t wp, char_t *path, char_t *query)
{
	char * pIPResAction = websGetVar(wp, T("IPResAction"), T(""));	
	char_t * pRowId = websGetVar(wp, T("IPResIdx"), T(""));
	char_t * pIPResCount = websGetVar(wp, T("IPResCount"), T("0"));
	int count=gatoi(pIPResCount);
	int index=0;
	
 	//printf("pIPResAction = %s, pRowId=%s, pIPResCount=%s\n",pIPResAction,pRowId,pIPResCount);	
	if (!gstrcmp(pIPResAction,"IPResAdd"))
	{
		ip_res_list_idx=-1;
		websRedirect(wp, T("lan/lan_ipreservation_editor.asp"));
	}
	else if (!gstrcmp(pIPResAction,"IPResEdit"))
	{
		ip_res_list_idx=gatoi(pRowId);
		websRedirect(wp, T("lan/lan_ipreservation_editor.asp"));
	}
	else if (!gstrcmp(pIPResAction,"IPResDel"))
	{
		char * pErrMsg = NULL;
		int status = -1;

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("lan/lan_ipreservation.asp"));
		}
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [1024] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from DhcpfixedIpAddress where _ROWID_ in (%s)",pRowId);
  		//printf("pDeleteCmd=%s\n",pSQLCmd);
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
		status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

		if (pDB)
			sqlite3_close (pDB);

		if (status>=0)
		{
			sqlite3_free (pErrMsg);
			system(EXPORT_CMD);
			system("/usr/local/bin/dhcpdInit");
		}
		websRedirect(wp, T("lan/lan_ipreservation.asp"));
	}
}

static int getDhcpReservationConfig(int eid, webs_t wp, int argc, char_t **argv)
{
 //printf("ip_res_list_idx=%d\n",ip_res_list_idx);
	if (ip_res_list_idx==-1) //add
	{
		return websWrite(wp, T("\"%s\""),"");
	}
	else if (ip_res_list_idx>0) //edit
	{
		char * pMacAddr = jnrp_sql_getAttr_Webs("DhcpfixedIpAddress","MacAddr",ip_res_list_idx);
		char * pIpAddr = jnrp_sql_getAttr_Webs("DhcpfixedIpAddress","IpAddr",ip_res_list_idx);

		char pCmdInfo [256] = {0};
		memset (pCmdInfo, 0x00, sizeof(pCmdInfo));

		gstrcat(pCmdInfo, pMacAddr);
		gstrcat(pCmdInfo, pSep);
		gstrcat(pCmdInfo, pIpAddr);
		return websWrite(wp, T("\"%s\""), pCmdInfo);
	}
}

int checkDhcpfixedIpAddress (char * MacAddr, char * IpAddr)
{
 char ** ppQryResult = NULL;
 char pMacQry [256] = {0};
 memset(pMacQry, 0x00, sizeof(pMacQry));
 char pIpQry [256] = {0};
 memset(pIpQry, 0x00, sizeof(pIpQry));

 if (ip_res_list_idx==-1) //add
 {
 	snprintf(pMacQry, sizeof(pMacQry), "select COUNT(*) from DhcpfixedIpAddress where UPPER(MacAddr)=UPPER('%s')", MacAddr);
 	snprintf(pIpQry, sizeof(pIpQry), "select COUNT(*) from DhcpfixedIpAddress where IpAddr='%s'", IpAddr);
 }
 else if (ip_res_list_idx>0) //edit
 {
 	snprintf(pMacQry, sizeof(pMacQry), "select COUNT(*) from DhcpfixedIpAddress where UPPER(MacAddr)=UPPER('%s') and _ROWID_!=%d", MacAddr, ip_res_list_idx);
 	snprintf(pIpQry, sizeof(pIpQry), "select COUNT(*) from DhcpfixedIpAddress where IpAddr='%s' and _ROWID_!=%d", IpAddr, ip_res_list_idx);
 }
 ppQryResult = jnrp_sql_getByQry_Webs (pMacQry);
 if (gatoi(ppQryResult[1]) > 0)
 {
 	 return reservation_err_mac;
 }
 ppQryResult = NULL;

 ppQryResult = jnrp_sql_getByQry_Webs (pIpQry);
 if (gatoi(ppQryResult[1]) > 0)
 {
   return reservation_err_ip;
 }

 return reservation_none;
}

static void setDhcpReservationConfig(webs_t wp, char_t *path, char_t *query)
{
 //printf("%s\n", __func__);
	char * pMacAddr = websGetVar(wp, T("mac_address"), T(""));
	char * pIpAddr = websGetVar(wp, T("ip_address"), T(""));
	char * pErrMsg = NULL;
	int status = -1;

 // check the MAC address and IP address already exists.
 P02003_dhcp_reservation_status = checkDhcpfixedIpAddress (pMacAddr, pIpAddr);
 if (P02003_dhcp_reservation_status == reservation_none)
 {
 } else {
 	websRedirect(wp, T("lan/lan_ipreservation.asp"));
 	return;
 }

	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	{
		printf ("unable to open the DB file %s\n", pDBFile);
		websRedirect(wp, T("lan/lan_ipreservation.asp"));
	}
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
	if (ip_res_list_idx==-1) //add
	{
  //int count = jnrp_sql_getRowNum_Webs ("DhcpfixedIpAddress");
  //printf("setDhcpReservationConfig add item\n");
  snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into DhcpfixedIpAddress values (1,1,'%s','%s')", pIpAddr, pMacAddr);
  //printf("pSQLCmd: %s \n",pSQLCmd);
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
	}
	else if (ip_res_list_idx>0) //edit
	{
  //printf("setDhcpReservationConfig edit item=%d\n", ip_res_list_idx);
		snprintf(pSQLCmd,sizeof(pSQLCmd), "update DhcpfixedIpAddress set IpAddr='%s', MacAddr='%s' where _ROWID_=%d", pIpAddr, pMacAddr, ip_res_list_idx);
  //printf("pSQLCmd: %s \n",pSQLCmd);
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
	}

	if (pDB)
		sqlite3_close (pDB);

	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD); 
	}	

	websRedirect(wp, T("lan/lan_ipreservation.asp"));
}

static int getDhcpReservationErrorStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int error_msg = P02003_dhcp_reservation_status;
	P02003_dhcp_reservation_status = reservation_none;
 return websWrite(wp, T("\"%d\""), error_msg);
}

static int getDNSInfo(int eid, webs_t wp, int argc, char_t **argv)
{
 char * pSystemName = jnrp_sql_getAttr_Webs ("system", "name", 1);
 char * pDomainName = jnrp_sql_getAttr_Webs ("system", "domain", 1);
 char * pDescription = jnrp_sql_getAttr_Webs ("system", "description", 1);

 char pDNSInfo [256] = {0};
 memset (pDNSInfo, 0x00, sizeof(pDNSInfo));

 gstrcat(pDNSInfo, pSystemName);
 gstrcat(pDNSInfo, pSep);
 gstrcat(pDNSInfo, pDomainName);
 gstrcat(pDNSInfo, pSep);
 gstrcat(pDNSInfo, pDescription);
 gstrcat(pDNSInfo, pSep);
 
 return websWrite(wp, T("\"%s\""), pDNSInfo);
}

static int getDDNSInfo(int eid, webs_t wp, int argc, char_t **argv)
{
 char * pddnsService = jnrp_sql_getAttr_Webs ("ddns", "ddnsService", 1);
 char * phostname = jnrp_sql_getAttr_Webs ("ddns", "hostname", 1);
 char * pusername = jnrp_sql_getAttr_Webs ("ddns", "username", 1);
 char * ppassword = jnrp_sql_getAttr_Webs ("ddns", "password", 1);
 char * ptimePeriod = jnrp_sql_getAttr_Webs ("ddns", "timePeriod", 1);
 char * pwildflag = jnrp_sql_getAttr_Webs ("ddns", "wildflag", 1);
 //printf("getDDNSInfo: pddnsService:%s\n",pddnsService);
 char pDDNSInfo [512] = {0};
 memset (pDDNSInfo, 0x00, sizeof(pDDNSInfo));

 char time_period[32];
	memset(time_period,'\0',sizeof(time_period));
 if (!gstrcmp(ptimePeriod, "30"))
 {
 	snprintf(time_period, sizeof(time_period), "%d", 1);
 } else {
 	snprintf(time_period, sizeof(time_period), "%d", 0);
 }

 gstrcat(pDDNSInfo, pddnsService);
 gstrcat(pDDNSInfo, pSep);
 gstrcat(pDDNSInfo, phostname);
 gstrcat(pDDNSInfo, pSep);
 gstrcat(pDDNSInfo, pusername);
 gstrcat(pDDNSInfo, pSep);
 gstrcat(pDDNSInfo, ppassword);
 gstrcat(pDDNSInfo, pSep);
 gstrcat(pDDNSInfo, pwildflag);
 gstrcat(pDDNSInfo, pSep);
 gstrcat(pDDNSInfo, time_period);
 gstrcat(pDDNSInfo, pSep);

 return websWrite(wp, T("\"%s\""), pDDNSInfo);
}

static void setDNSInfo(webs_t wp, char_t *path, char_t *query)
{
 char * pSystemName = websGetVar(wp, T("system_name"), T(""));
 char * pDomainName = websGetVar(wp, T("domain_name"), T(""));
 char * pDescription = websGetVar(wp, T("description"), T(""));
 //printf("%s, pSystemName=%s, pDomainName=%s, pDescription=%s\n",__func__,pSystemName,pDomainName,pDescription);
	char * pErrMsg = NULL;
	int status = -1;

 if (gstrlen(pSystemName) && gstrlen(pDomainName))
 {
	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	{
		printf ("unable to open the DB file %s\n", pDBFile);
		websRedirect(wp, T("advanced/advanced_ddns.asp"));
	}
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
 snprintf(pSQLCmd,sizeof(pSQLCmd), "update system set name='%s', domain='%s', description='%s'", pSystemName, pDomainName, pDescription);
 //printf("pSQLCmd: %s \n",pSQLCmd);
 status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	if (pDB)
		sqlite3_close (pDB);

	}

	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD);
		//For P02003
		websRedirect(wp, T("advanced/advanced_ddns.asp"));
		system("/usr/local/bin/dhcpdInit restart");
	}

	//websRedirect(wp, T("advanced/advanced_ddns.asp"));
}

static void setDDNSInfo(webs_t wp, char_t *path, char_t *query)
{
	char * pDDNSService = websGetVar(wp, T("selDDNSService"), T(""));
	char * pDDNSDomainName = websGetVar(wp, T("ddns_domain_name"), T(""));
	char * pDDNSUsername = websGetVar(wp, T("ddns_username"), T(""));
	char * pDDNSPassword = websGetVar(wp, T("ddns_password"), T(""));
	char * pDDNSUseWildcards = websGetVar(wp, T("ddns_use_wildcards"), T("0"));
	char * pDDNSUpdatePeriod = websGetVar(wp, T("ddns_update_period"), T("0"));
 //printf("%s, pDDNSDomainName=%s, pDDNSUsername=%s, pDDNSPassword=%s, pDDNSUseWildcards=%s, pDDNSUpdatePeriod=%s\n",__func__,pDDNSDomainName,pDDNSUsername,pDDNSPassword,pDDNSUseWildcards,pDDNSUpdatePeriod);
	char * pErrMsg = NULL;
	int status = -1;
	int ddns_update_period = 0;

 if (!gstrcmp(pDDNSUpdatePeriod, "1"))
 {
  ddns_update_period = 30;
 } else {
  ddns_update_period = 1;
 }

	sqlite3 * pDB = NULL;
	if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	{
		printf ("unable to open the DB file %s\n", pDBFile);
		websRedirect(wp, T("advanced/advanced_ddns.asp"));
	}
	sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
 snprintf(pSQLCmd,sizeof(pSQLCmd), "update ddns set ddnsService='%s', hostname='%s', username='%s', password='%s', timePeriod=%d, wildflag=%d", pDDNSService, pDDNSDomainName, pDDNSUsername, pDDNSPassword, ddns_update_period, gatoi(pDDNSUseWildcards));
 //printf("pSQLCmd: %s \n",pSQLCmd);
 status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);

	if (pDB)
		sqlite3_close (pDB);

	if (status>=0)
	{
		sqlite3_free (pErrMsg);
		system(EXPORT_CMD); 
	}	

	websRedirect(wp, T("advanced/advanced_ddns.asp"));
}
#endif
