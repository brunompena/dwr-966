#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

#define APP_LOG_DEBUG
#ifdef APP_LOG_DEBUG
#ifndef DEBUG_MSG
#define DEBUG_MSG(fmt, args...)   printf("(%s)" fmt, __func__, ##args)
#endif
#endif

//sms page
static int getData(int eid, webs_t wp, int argc, char_t **argv);

//dlna page
static int getDLNA(int eid, webs_t wp, int argc, char_t **argv);
static int setDLNA(webs_t wp, char_t *path, char_t *query);
#define MODEM_UPGRADE_DIR "/ramdisk/tmp/mdm_mass_storage"

//usb page
static int getUsbInfo(int eid, webs_t wp, int argc, char_t ** argv);
static int getSambaInfo(int eid, webs_t wp, int argc, char_t **argv);
static int getFileShare(int eid, webs_t wp, int argc, char_t **argv);
static int setFileShare(webs_t wp, char_t *path, char_t *query);

#if defined(__P02003_Dora) 
int treeview_id=1;
static int getSambaAccount(int eid, webs_t wp, int argc, char_t **argv);
static int getSambaSetting(int eid, webs_t wp, int argc, char_t **argv);
static int setSambaSetting(webs_t wp, char_t *path, char_t *query);
static int getSambaShare(int eid, webs_t wp, int argc, char_t **argv);
static int setSambaShare(webs_t wp, char_t *path, char_t *query);
static int getSambaShareConfig(int eid, webs_t wp, int argc, char_t **argv);
static int setSambaShareConfig(webs_t wp, char_t *path, char_t *query);
static int getUsbTreeView(int eid, webs_t wp, int argc, char_t **argv);
#endif

void initApplicationWeb();

void initApplicationWeb()
{
	websAspDefine(T("getData"), getData); //for test

	websAspDefine(T("getDLNA"), getDLNA); 
	websFormDefine(T("setDLNA"), setDLNA);

	websAspDefine(T("getUsbInfo"), getUsbInfo);
	websAspDefine(T("getSambaInfo"), getSambaInfo);
	websAspDefine(T("getFileShare"), getFileShare);	
	websFormDefine(T("setFileShare"), setFileShare);

#if defined(__P02003_Dora) 
	websAspDefine(T("getSambaAccount"), getSambaAccount);
	websAspDefine(T("getSambaSetting"), getSambaSetting);
	websFormDefine(T("setSambaSetting"), setSambaSetting);
	websAspDefine(T("getSambaShare"), getSambaShare);
	websFormDefine(T("setSambaShare"), setSambaShare);
	websAspDefine(T("getSambaShareConfig"), getSambaShareConfig);
	websFormDefine(T("setSambaShareConfig"), setSambaShareConfig);	
	websAspDefine(T("getUsbTreeView"), getUsbTreeView);
#endif	
	return;
}

static int getData(int eid, webs_t wp, int argc, char_t **argv)
{
	//printf("test getData\n");
	int ret = 0;
    char *pData = (char *)malloc(128);
    gstrcpy (pData,"");
	//gstrcat(pData,"test test 1111");
	ret = websWrite(wp, T("%s"), pData);
	free(pData);
    return ret;
}

int get_dlna_status()
{
    int dlna_enable = 0;
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("dlnaConfig", "enable", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"dlnaConfig", "enable", 1);
#endif
    dlna_enable = gatoi(pQryResult);
    printf("dlna_enable: %d\n", dlna_enable);	
    return dlna_enable;
}

static int getDLNA(int eid, webs_t wp, int argc, char_t **argv)
{
	//need to check dlna is enabled or disabled	
   	int dlna_status = get_dlna_status();
	printf("getDLNA: %d\n", dlna_status);	
	return websWrite(wp, T("%d"), dlna_status);
}	

static int setDLNA(webs_t wp, char_t *path, char_t *query)
{
	int doAction = 0;
	int status = -1;
	char_t * pDLNAEnable = websGetVar(wp, T("dlna_enable"), T("off"));	
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));	

	int modemExist=0;
	struct stat sb;
	if (stat(MODEM_UPGRADE_DIR, &sb) == 0) //check if directory does exist
	{
		//check if modem upgrade file exists
		struct dirent **namelist;
		int n;	
		n = scandir(MODEM_UPGRADE_DIR, &namelist, 0, alphasort);
		if (n>2) //include folder . & ..
		{
			modemExist=1;
		}		   
		free(namelist); 	
	}
	printf("checkModemExist %d\n", modemExist);	
	
	if (modemExist)
	{
		doAction = 0;
		#ifdef __P02003_Dora
		setResultCode("Cannot set dlna now. Modem is upgrading.");
		#endif		
	}
	else
	{
		if (get_dlna_status()==0 && gstrcmp(pDLNAEnable, "on")==0)
		{
	        snprintf(pSQLCmd,sizeof(pSQLCmd),"update dlnaConfig set enable='%d'",1);		
			doAction = 1;		
		}
		else if (get_dlna_status()==1 && gstrcmp(pDLNAEnable,"off")==0)
		{
			snprintf(pSQLCmd,sizeof(pSQLCmd),"update dlnaConfig set enable='%d'",0);
			doAction = 1;
		}
	}

	if (doAction==1)
	{
		#ifdef __USED_WEBS_DB__
			status = jnrp_sql_dbEXE_Webs (pSQLCmd);
		#else
			status = jnrp_sql_dbEXE (pDBFile,pSQLCmd);
		#endif	
	    if (status>=0)
	    {						
	        // export db to ascii
	        system(EXPORT_CMD);
			#ifdef __P02003_Dora
			system("dlna.sh");
			setResultCode("Setting completed");
			#endif
	    }
	}

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
    websRedirect(wp, T("advanced/dlna.asp"));
#else
    websRedirect(wp, T("applications/dlna.asp"));
#endif 

}

static int getUsbInfo(int eid, webs_t wp, int argc, char_t ** argv)
{
	FILE *fp;
	char diskcount_str[20] = "";
	char diskname_str[20] = "";
	char diskname[100] = "";
	char diskserialnum[100] = "";
	char diskpartcount_str[20] = "";
	char diskinfo_str[20] = "";
	char diskinfopath[100] = "";
	char diskinfofs[20] = "";
	char diskinfosize[20] = "";
	char diskinfofresp[20] = "";
	char diskinfousedsp[20] = "";
	char diskinfouse[20] = "";
	unsigned int i = 0, j = 0;
	unsigned int diskcount = 0;
	unsigned int diskpartcount = 0;
	int ret = 0;
	char *pUsbStatus = (char *)malloc(1024); 
	gstrcpy(pUsbStatus,""); 	

	system("/etc/init.d/usb_automount_status.sh status");
	fp = fopen("/tmp/usb_info.txt", "r");
	if (fp == NULL) {
#ifdef APP_LOG_DEBUG
		DEBUG_MSG("[%s:%d] failed to read /tmp/usb_info.txt",
			__FUNCTION__, __LINE__);
#endif
		ret = -1;
		goto APP_OUT;
	}
	if (fscanf(fp, "%20s %u", diskcount_str, &diskcount) != 2) {
#ifdef APP_LOG_DEBUG
		DEBUG_MSG
		    ("[%s:%d] failed to read diskcount from /tmp/usb_info.txt",
		     __FUNCTION__, __LINE__);
#endif
		ret = -1;
		goto APP_OUT;
	}
#ifdef APP_LOG_DEBUG
	DEBUG_MSG("[%s:%d] Disk count -> %u\n", __FUNCTION__, __LINE__,
		diskcount);
#endif
	if (diskcount == 0) {
#ifdef APP_LOG_DEBUG
		DEBUG_MSG("[%s:%d] Diskcount is 0\n", __FUNCTION__, __LINE__);
#endif
		//websWrite(wp, T("No USB/SATA Storage Devices mounted."));					
		ret = -1;
		goto APP_OUT;
	}
	for (i = 0; i < diskcount; i++) {
		if (fscanf
		    (fp, "%20s %100s %[^=\n]", diskname_str, diskserialnum,
		     diskname) == 0) {
#ifdef APP_LOG_DEBUG
			DEBUG_MSG
			    ("[%s:%d] failed to read diskname from /tmp/usb_info.txt",
			     __FUNCTION__, __LINE__);
#endif
			ret = -1;
			goto APP_OUT;			
		}
#ifdef APP_LOG_DEBUG
		DEBUG_MSG("[%s:%d] Disk %u : \n diskname -> %s\n", __FUNCTION__,
			__LINE__, i, diskname);
#endif
		if (fscanf(fp, "%20s %u", diskpartcount_str, &diskpartcount) != 2) {
#ifdef APP_LOG_DEBUG
			DEBUG_MSG
			    ("[%s:%d] failed to read diskpartcount from /tmp/usb_info.txt",
			     __FUNCTION__, __LINE__);
#endif
			ret = -1;
			goto APP_OUT;
		}
#ifdef APP_LOG_DEBUG
		DEBUG_MSG("[%s:%d] Disk %u : \n diskpartcount -> %u\n",
			__FUNCTION__, __LINE__, i, diskpartcount);
#endif
		//Table field names
		for (j = 0; j < diskpartcount; j++) {
			if (fscanf
			    (fp, "%20s %100s %20s %20s %20s %20s %20s", diskinfo_str,
			     diskinfopath, diskinfofs, diskinfosize,
			     diskinfofresp, diskinfousedsp, diskinfouse) != 7) {
#ifdef APP_LOG_DEBUG
				DEBUG_MSG
				    ("[%s:%d] failed to read diskpartvalue from /tmp/usb_info.txt",
				     __FUNCTION__, __LINE__);
#endif
				ret = -1;
				goto APP_OUT;
			}
#ifdef APP_LOG_DEBUG
			DEBUG_MSG
			    ("[%s:%d] diskpath -> %s\n diskinfofs -> %s\n diskinfosize -> %s\n diskinfofresp -> %s\n diskinfousee -> %s \n",
			     __FUNCTION__, __LINE__, diskinfopath, diskinfofs,
			     diskinfosize, diskinfofresp, diskinfouse);
#endif


			gstrcat(pUsbStatus, diskinfopath);		
			gstrcat(pUsbStatus, pSep);
			gstrcat(pUsbStatus, diskinfofs);		
			gstrcat(pUsbStatus, pSep);			
			gstrcat(pUsbStatus, diskinfosize);		
			gstrcat(pUsbStatus, pSep);			
			gstrcat(pUsbStatus, diskinfofresp);		
			gstrcat(pUsbStatus, pSep);			
			gstrcat(pUsbStatus, diskinfousedsp);		
			gstrcat(pUsbStatus, pSep);
			gstrcat(pUsbStatus, diskinfouse);
			gstrcat(pUsbStatus, "&");	
		}
#ifdef APP_LOG_DEBUG
		DEBUG_MSG("[%s:%d] diskcount -> %u\n", __FUNCTION__, __LINE__,
			diskcount);
#endif
	}

APP_OUT:
	if (fp)
	{
		fclose(fp);
		ret = websWrite(wp, T("\"%s\""), pUsbStatus);
		free(pUsbStatus);
		return ret;
	}
	else
	{
		printf("No USB/SATA Storage Devices mounted.\n");
		free(pUsbStatus);
		return websWrite(wp, T("\"%s\""),"");
	}

}

static int getSambaInfo(int eid, webs_t wp, int args, char_t **argv)
{
	//printf("getSambaInfo \n");
	
	FILE *fp;
	char line[128];
	memset(line, '\0', sizeof(line));
	int ret = 0;
	char *pSambaSettingInfo = (char *)malloc(1024);
	gstrcpy(pSambaSettingInfo,"");
				
	fp = fopen("/sysconfig/samba-cfg", "r");
	if(fp)	
	{
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {	  	
   	  	if (line[strlen(line)-1] == '\n')
 	 		line[strlen(line)-1] = '\0';
 	 	
 	   	//printf("line=%s\n", line);	   
 	   	
		if (!gstrcmp(getSplitStr(line,"=",1),"smb_ena"))
		{
			gstrcat(pSambaSettingInfo,getSplitStr(line,"=",2));
			gstrcat(pSambaSettingInfo,pSep);			
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"smb_name"))
		{
			gstrcat(pSambaSettingInfo,getSplitStr(line,"=",2));
			gstrcat(pSambaSettingInfo,pSep);			
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"smb_descr"))
		{
			gstrcat(pSambaSettingInfo,getSplitStr(line,"=",2));
			gstrcat(pSambaSettingInfo,pSep);			
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"smb_workgroup"))
		{
			gstrcat(pSambaSettingInfo,getSplitStr(line,"=",2));
			gstrcat(pSambaSettingInfo,pSep);			
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"file_share_Count"))
		{
			gstrcat(pSambaSettingInfo,getSplitStr(line,"=",2));
			gstrcat(pSambaSettingInfo,pSep);			
		}		
	  }
	  fclose(fp);	  
	} else {	  
	  free(pSambaSettingInfo);
	  return websWrite(wp, T("\"%s\""),"");
	}
	ret = websWrite(wp, T("\"%s\""), pSambaSettingInfo);
	free(pSambaSettingInfo);
	return ret;
}

static int getFileShare(int eid, webs_t wp, int argc, char_t **argv)
{
	//printf("getFileShare \n");
	if (argv[0]==NULL)
	{
		websWrite(wp, T("\"%s\""),"");
		return 0;	
	}

	char cmd_buffer[100];
	char cmd_result[128];
	FILE *fd = -1;
	char pQueryStr[128];
	memset(pQueryStr, '\0', sizeof(pQueryStr));
	int file_share_Count = 0;
	char pFileShareData[1024];
 	memset(pFileShareData,'\0', sizeof(pFileShareData));
	
    if (!gstrcmp(argv[0],"FILE_SHARE_NAME"))
    {
    	gstrcat(pQueryStr, "name");
    }
	else if (!gstrcmp(argv[0],"FILE_SHARE_PATH"))
	{
		gstrcat(pQueryStr, "path");
	}
	else if (!gstrcmp(argv[0],"FILE_SHARE_LEVEL"))
	{
		gstrcat(pQueryStr, "rwLvl");
	}	
	else if (!gstrcmp(argv[0],"FILE_SHARE_USERS"))
	{
		gstrcat(pQueryStr, "users");
	}
	/*
	else if (!gstrcmp(argv[0],"FILE_SHARE_PASSWORD"))
	{
		gstrcat(pQueryStr, "users_password");
	}
	*/

	/****************************************************
	 * Get File Share Count
	 ****************************************************/			 
	memset(cmd_buffer, '\0', sizeof(cmd_buffer));
	memset(cmd_result, '\0', sizeof(cmd_result));
	sprintf(cmd_buffer,"cat /sysconfig/samba-cfg | grep file_share_Count");
	fd = popen(cmd_buffer,"r");		
	if(fgets(cmd_result,sizeof(cmd_result),fd) == NULL)
	{
		file_share_Count = 0;
	}
	else
	{
		if (cmd_result[strlen(cmd_result)-1] == '\n')
		 cmd_result[strlen(cmd_result)-1] = '\0';		
		file_share_Count = atoi(getSplitStr(cmd_result,"=",2));
	}	
	pclose(fd);	
	if (file_share_Count == 0)
	{
		websWrite(wp, T("\"%s\""),"");
		return 0;
	}

	int idx=0;
	for (idx=0; idx<file_share_Count; idx++)
	{	
		FILE *fp = -1;
		memset(cmd_buffer, '\0', sizeof(cmd_buffer));
		memset(cmd_result, '\0', sizeof(cmd_result));
		sprintf(cmd_buffer,"cat /sysconfig/samba-cfg | grep fs_%d_%s", idx, pQueryStr);
		fp = popen(cmd_buffer,"r");		
		if(fgets(cmd_result,sizeof(cmd_result),fp) == NULL)
		{				
			gstrcat(pFileShareData, pSep);
		}		
		else
		{
			if (cmd_result[strlen(cmd_result)-1] == '\n')				
			 cmd_result[strlen(cmd_result)-1] = '\0';						
			
			if (!gstrcmp(pQueryStr,"path"))
			{
				char *pSrc = (char *)malloc(128);
				char pDest[128]={0};
				strcpy(pSrc, getSplitStr(cmd_result,"=",2));			
				//filter /mnt/usb prefix
				strncpy(pDest,pSrc+8,strlen(pSrc)-8);
				gstrcat(pFileShareData, pDest);
				free(pSrc);
			}
			else
			{
				gstrcat(pFileShareData, getSplitStr(cmd_result,"=",2));
			}
			
			gstrcat(pFileShareData, pSep);
		}	
		pclose(fp); 
	}	
	
	websWrite(wp, T("\"%s\""),pFileShareData);
	return 0;
}	


static int setFileShare(webs_t wp, char_t *path, char_t *query)
{
    //printf("setFileShare \n");	
	
	char_t *SAMBA_ENABLE,
		   *SAMBA_NAME,
		   *SAMBA_DESCR,
		   *SAMBA_WORKGROUP,
		   *FILE_SHARE_COUNT,
           *FILE_SHARE_NAME, 
	       *FILE_SHARE_PATH,
	       *FILE_SHARE_LEVEL,
	       *FILE_SHARE_USERS,
		   *FILE_SHARE_PASSWORD;
	int file_share_count=0;
	int idx=0;

    SAMBA_ENABLE = websGetVar(wp, T("SAMBA_ENABLE"), T("0"));
	SAMBA_NAME = websGetVar(wp, T("SAMBA_NAME"), T("ServerName"));		
	SAMBA_DESCR = websGetVar(wp, T("SAMBA_DESCR"), T("ServerDes"));
	SAMBA_WORKGROUP = websGetVar(wp, T("SAMBA_WORKGROUP"), T("WorkGrp"));
	FILE_SHARE_COUNT = websGetVar(wp, T("FILE_SHARE_COUNT"), T("0"));
	FILE_SHARE_NAME = websGetVar(wp, T("FILE_SHARE_NAME"), T(""));
	FILE_SHARE_PATH = websGetVar(wp, T("FILE_SHARE_PATH"), T(""));
	FILE_SHARE_LEVEL = websGetVar(wp, T("FILE_SHARE_LEVEL"), T(""));
	FILE_SHARE_USERS = websGetVar(wp, T("FILE_SHARE_USERS"), T(""));
	FILE_SHARE_PASSWORD = websGetVar(wp, T("FILE_SHARE_PASSWORD"), T(""));
	file_share_count = atoi(FILE_SHARE_COUNT);	
	//printf("file_share_count=%d\n",file_share_count);

	/***************************************************
	 * write current setting to /sysconfig/samba-cfg
	 ****************************************************/				
	char line[128];
	memset(line, '\0', sizeof(line));	
	FILE *fp = fopen("/sysconfig/samba-cfg", "r");
	FILE *out = fopen("/sysconfig/.samba-cfg", "w+");
	if(fp && out)	
	{
      /***********************************************
       * write default data
       ***********************************************/      
      memset(line, '\0', sizeof(line));	
      sprintf(line,"user_obj_Count=1\n");		
      fwrite(line, strlen(line), 1, out);	
	  
	  memset(line, '\0', sizeof(line));   
	  sprintf(line,"user_0_username=admin\n");			  
	  fwrite(line, strlen(line), 1, out);
	  
	  memset(line, '\0', sizeof(line));   
	  sprintf(line,"user_0_password=admin\n");			  
	  fwrite(line, strlen(line), 1, out);	  	  	  
	
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {	  	
   	  	if (line[strlen(line)-1] == '\n')
 	 		line[strlen(line)-1] = '\0';
 	 	
 	   	//printf("line=%s\n", line);	   
		if (!gstrcmp(getSplitStr(line,"=",1),"smb_ena"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"smb_ena=%s\n",SAMBA_ENABLE);
			fwrite(line, strlen(line), 1, out);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"smb_name"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"smb_name=%s\n",SAMBA_NAME);	
			fwrite(line, strlen(line), 1, out);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"smb_descr"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"smb_descr=%s\n",SAMBA_DESCR);	
			fwrite(line, strlen(line), 1, out);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"smb_workgroup"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"smb_workgroup=%s\n",SAMBA_WORKGROUP);		
			fwrite(line, strlen(line), 1, out);		
		}	
		
		if (!gstrcmp(getSplitStr(line,"=",1),"file_share_Count"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"file_share_Count=%s\n",FILE_SHARE_COUNT);	
			fwrite(line, strlen(line), 1, out);		
			
			//save file share info according to file share count			
			for (idx=0; idx<file_share_count; idx++)
			{
				//name
				memset(line, '\0', sizeof(line));
				sprintf(line, "fs_%d_name=%s\n",idx,getSplitStr2(FILE_SHARE_NAME,"#",idx+1));				
				fwrite(line, strlen(line), 1, out); 	

				//path
				memset(line, '\0', sizeof(line));
				sprintf(line, "fs_%d_path=/mnt/usb%s\n",idx,getSplitStr2(FILE_SHARE_PATH,"#",idx+1));
				fwrite(line, strlen(line), 1, out); 
					
				//rwLvl
				memset(line, '\0', sizeof(line));
				sprintf(line, "fs_%d_rwLvl=%s\n",idx,getSplitStr2(FILE_SHARE_LEVEL,"#",idx+1));
				fwrite(line, strlen(line), 1, out); 			
				
				//users
				memset(line, '\0', sizeof(line));
				sprintf(line, "fs_%d_users=%s\n",idx,getSplitStr2(FILE_SHARE_USERS,"#",idx+1));
				fwrite(line, strlen(line), 1, out); 

				/*	
				//password				
				memset(line, '\0', sizeof(line));
				sprintf(line, "fs_%d_users_password=%s\n",idx,getSplitStr2(FILE_SHARE_PASSWORD,"#",idx+1));
				fwrite(line, strlen(line), 1, out); 									
				*/
			}					
		}	
	  }
	  fclose(fp);
	  fclose(out);

	  system("mv /sysconfig/.samba-cfg /sysconfig/samba-cfg");
	 
	} else {
	 
	}	
	
	if (!gstrcmp(SAMBA_ENABLE,"0")) //disable samba
	{
		system("/etc/init.d/samba stop");
	}
	else //enable samba
	{
		system("/etc/init.d/samba stop");
		system("/etc/init.d/samba start");		
	}	

	//system("sleep 2");
#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
    websRedirect(wp, T("advanced/usb_storage.asp"));
#else
    websRedirect(wp, T("applications/usb_storage.asp"));
#endif 

}

#if defined(__P02003_Dora) 
void generateSambaCfg()
{
	/***************************************************
	 * write current setting to /sysconfig/samba-cfg
	 * generate /sysconf/samba-cfg	
	 ****************************************************/				
	char line[128];
	memset(line, '\0', sizeof(line));	
	FILE *out = fopen("/sysconfig/.samba-cfg", "w+");
    int index = 0;
    char* pName = NULL;
    char* pPath = NULL;
    char* pRwLvl = NULL;
    char* pUsers = NULL;
    int shareCount = jnrp_sql_getRowNum_Webs("SambaShare");	
	char_t * SAMBA_ADMIN_PWD = jnrp_sql_getAttr_Webs("SambaAccount", "Password", 1);		

	char * SAMBA_ENA = jnrp_sql_getAttr_Webs("SambaSetting", "Ena", 1);
	char * SAMBA_NAME = jnrp_sql_getAttr_Webs("SambaSetting", "Name", 1);
	char * SAMBA_DESCR = jnrp_sql_getAttr_Webs("SambaSetting", "Descr", 1);
	char * SAMBA_WORKGROUP = jnrp_sql_getAttr_Webs("SambaSetting", "Workgroup", 1);
	if (out)
	{
      memset(line, '\0', sizeof(line));	
      sprintf(line,"user_obj_Count=1\n");		
      fwrite(line, strlen(line), 1, out);	
	  
	  memset(line, '\0', sizeof(line));   
	  sprintf(line,"user_0_username=admin\n");			  
	  fwrite(line, strlen(line), 1, out);
	  
	  memset(line, '\0', sizeof(line));   
	  sprintf(line,"user_0_password=%s\n",SAMBA_ADMIN_PWD);			  
	  fwrite(line, strlen(line), 1, out);	  	  	  

      memset(line, '\0', sizeof(line));	
      sprintf(line,"smb_ena=%s\n",SAMBA_ENA);
      fwrite(line, strlen(line), 1, out);		

      memset(line, '\0', sizeof(line));	
      sprintf(line,"smb_name=%s\n",SAMBA_NAME);	
      fwrite(line, strlen(line), 1, out);		
      
      memset(line, '\0', sizeof(line));	
      sprintf(line,"smb_descr=%s\n",SAMBA_DESCR);	
      fwrite(line, strlen(line), 1, out);		
      
      memset(line, '\0', sizeof(line));	
      sprintf(line,"smb_workgroup=%s\n",SAMBA_WORKGROUP);		
      fwrite(line, strlen(line), 1, out);		
	  
	  memset(line, '\0', sizeof(line));   
	  sprintf(line,"file_share_Count=%d\n",shareCount); 
	  fwrite(line, strlen(line), 1, out);	  
	  
      if (shareCount>0)
      {
       	for (index=1; index<=shareCount; index++)
       	{
      		  pName = jnrp_sql_getAttr_Webs("SambaShare", "Name", index);
      		  pPath = jnrp_sql_getAttr_Webs("SambaShare", "Path", index);
      		  pRwLvl = jnrp_sql_getAttr_Webs("SambaShare", "RwLvl", index);
      		  pUsers = jnrp_sql_getAttr_Webs("SambaShare", "Users", index);				  

              //name
              memset(line, '\0', sizeof(line));
              sprintf(line, "fs_%d_name=%s\n",(index-1),pName);				
              fwrite(line, strlen(line), 1, out); 	
              
              //path
              memset(line, '\0', sizeof(line));
              sprintf(line, "fs_%d_path=%s\n",(index-1),pPath);
              fwrite(line, strlen(line), 1, out); 
              	
              //rwLvl
              memset(line, '\0', sizeof(line));
              sprintf(line, "fs_%d_rwLvl=%s\n",(index-1),pRwLvl);
              fwrite(line, strlen(line), 1, out); 			
              
              //users
              memset(line, '\0', sizeof(line));
              sprintf(line, "fs_%d_users=%s\n",(index-1),pUsers);
              fwrite(line, strlen(line), 1, out); 				  
       	}
      }			
	  fclose(out);
	  system("mv /sysconfig/.samba-cfg /sysconfig/samba-cfg");		 
	}

	if (gatoi(SAMBA_ENA)==1)
	{
		system("/etc/init.d/samba stop");
		system("/etc/init.d/samba start");				  
	}
	else
	{
		system("/etc/init.d/samba stop");
	}
}

static int getSambaAccount(int eid, webs_t wp, int args, char_t **argv)
{
	printf("%s\n", __func__);	
	//current: only 2 user accounts: admin/all
	int index=0;
	for (index=1; index<=2; index++)
	{
		char * pUsername = jnrp_sql_getAttr_Webs("SambaAccount", "Username", index);
		if (!gstrcmp(pUsername,"all"))
		websWrite(wp, T("<option value=\"%s\"><script>document.write(gettext(\"all\"));</script></option>\n"), pUsername);	
		else
		websWrite(wp, T("<option value=\"%s\">%s</option>\n"), pUsername, pUsername);		
	}
}

static int getSambaSetting(int eid, webs_t wp, int args, char_t **argv)
{
	printf("%s\n", __func__);	
	char * pDisplayUI = jnrp_sql_getAttr_Webs("SambaSetting", "DisplayUI", 1);
	char * pEna = jnrp_sql_getAttr_Webs("SambaSetting", "Ena", 1);
	char * pName = jnrp_sql_getAttr_Webs("SambaSetting", "Name", 1);
	char * pDescr = jnrp_sql_getAttr_Webs("SambaSetting", "Descr", 1);
	char * pWorkgroup = jnrp_sql_getAttr_Webs("SambaSetting", "Workgroup", 1);
	char * pActiveUser = jnrp_sql_getAttr_Webs("SambaSetting", "ActiveUser", 1);
	char condition[256]={0};
	snprintf(condition,sizeof(condition), "Username='%s'", pActiveUser); 
	char * pPassword = jnrp_sql_getAttrByCondit_Webs("SambaAccount","Password",condition);	
	
	printf("pDisplayUI=%s,pEna=%s,pName=%s,pDescr=%s,pWorkgroup=%s\n", pDisplayUI,pEna,pName,pDescr,pWorkgroup);
	printf("pActiveUser=%s\n",pActiveUser);
	int ret = 0;	
	char* pSambaSetting = (char *)malloc(128);
	memset(pSambaSetting,'\0', 128);
	gstrcat(pSambaSetting, pDisplayUI);
	gstrcat(pSambaSetting, pSep);	
	gstrcat(pSambaSetting, pEna);
	gstrcat(pSambaSetting, pSep);
	gstrcat(pSambaSetting, pName);
	gstrcat(pSambaSetting, pSep);
	gstrcat(pSambaSetting, pDescr);
	gstrcat(pSambaSetting, pSep);
	gstrcat(pSambaSetting, pWorkgroup);
	gstrcat(pSambaSetting, pSep);
	gstrcat(pSambaSetting, pActiveUser);
	gstrcat(pSambaSetting, pSep);
	gstrcat(pSambaSetting, pPassword);
	ret = websWrite(wp, T("\"%s\""), pSambaSetting);
	free(pSambaSetting);
	return ret;
}

static int setSambaSetting(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);	
    char_t * SAMBA_ENABLE = websGetVar(wp, T("SAMBA_ENABLE"), T("0"));
	char_t * SAMBA_NAME = websGetVar(wp, T("SAMBA_NAME"), T("ServerName"));		
	char_t * SAMBA_DESCR = websGetVar(wp, T("SAMBA_DESCR"), T("ServerDes"));
	char_t * SAMBA_WORKGROUP = websGetVar(wp, T("SAMBA_WORKGROUP"), T("WorkGrp"));
	char_t * SAMBA_USERS = websGetVar(wp, T("SAMBA_USERS"), T("all"));
	char_t * SAMBA_PASSWORD = websGetVar(wp, T("SAMBA_PASSWORD"), T(""));
	int samba_ena=gatoi(SAMBA_ENABLE);

	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update SambaSetting set Ena=%d, Name='%s', Descr='%s', Workgroup='%s', ActiveUser='%s' where _ROWID_='1'", 
		samba_ena,SAMBA_NAME,SAMBA_DESCR,SAMBA_WORKGROUP,SAMBA_USERS);			
	execSQLCmd(pSQLCmd);

	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update SambaAccount set Password='%s' where Username='%s'", 
		SAMBA_PASSWORD,SAMBA_USERS);			
	execSQLCmd(pSQLCmd);	
	generateSambaCfg();
	websRedirect(wp, T("advanced/usb_storage.asp"));
}

static int getSambaShare(int eid, webs_t wp, int args, char_t **argv)
{
	printf("%s\n", __func__);	
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from SambaShare";
	int index = 0;
	char* pName = NULL;
	char* pPath = NULL;
	char* pRwLvl = NULL;
	char* pUsers = NULL;
	char* pSimplePath[128]={0};

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Name", pName, getSambaShareOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Path", pPath, getSambaShareOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "RwLvl", pRwLvl, getSambaShareOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Users", pUsers, getSambaShareOut);

			memset(pSimplePath, 0x00, sizeof(pSimplePath));
			strncpy(pSimplePath,pPath+8,strlen(pPath)-8);

			printf("index=%d, Name=%s, Path=%s, RwLvl=%s, Users=%s, SimplePath=%s\n", index+1, pName, pPath, pRwLvl, pUsers, pSimplePath);
			websWrite(wp, T("<tr>\n"));
			websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"share.checkbox\" id=\"shareTbl_id_%d\"></td>\n"), index+1, index+1);
			websWrite(wp, T("<td class=\"tdEven\" id=\"shareTbl_name_%d\">%s</td>\n"), index+1, pName);
			websWrite(wp, T("<td class=\"tdEven\" id=\"shareTbl_path_%d\">%s</td>\n"), index+1, pSimplePath);
			if (gatoi(pRwLvl)==0)
				websWrite(wp, T("<td class=\"tdEven\" id=\"shareTbl_rwLvl_%d\">%s</td>\n"), index+1, "Read-Only");
			else
				websWrite(wp, T("<td class=\"tdEven\" id=\"shareTbl_rwLvl_%d\">%s</td>\n"), index+1, "Read-Write");
			if (!gstrcmp(pUsers,"all"))
				websWrite(wp, T("<td class=\"tdEven\" id=\"shareTbl_users_%d\"><script>document.write(gettext(\"all\"));</script></td>\n"), index+1);
			else
				websWrite(wp, T("<td class=\"tdEven\" id=\"shareTbl_users_%d\">%s</td>\n"), index+1, pUsers);
			websWrite(wp, T("</tr>\n"));
		}
	}	
getSambaShareOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;
}

static int setSambaShare(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);	
	char_t * pShareTblAction = websGetVar(wp, T("ShareTblAction"), T(""));	
	char_t * pShareName = websGetVar(wp, T("ShareName"), T(""));
	char_t * pShareDelList = websGetVar(wp, T("ShareDelList"), T(""));	
	
	printf("pShareTblAction = %s\n",pShareTblAction);	
	printf("pShareName = %s\n", pShareName);
	printf("pShareDelList= %s\n", pShareDelList);		
	if (!gstrcmp(pShareTblAction,"ShareTblAdd"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),"");						
		websRedirect(wp, T("advanced/usb_storage_editor.asp"));
	}
	else if (!gstrcmp(pShareTblAction,"ShareTblEdit"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),pShareName);				
		websRedirect(wp, T("advanced/usb_storage_editor.asp"));
	}
	else if (!gstrcmp(pShareTblAction,"ShareTblDel"))
	{
		char *token;
		int len = strlen("delete from SambaShare where Name in ()")+strlen(pShareDelList)+4;
		char* pSQLCmd = (char *)malloc(len);
		memset(pSQLCmd,'\0', len);
		token = gstrtok(pShareDelList, ",");
		while (token != NULL)
		{
			sprintf(pSQLCmd, "delete from SambaShare where Name in ('%s')", token);
			execSQLCmd(pSQLCmd);
			token = gstrtok(NULL, ",");			
		}
		free(pSQLCmd);
		generateSambaCfg();
		websRedirect(wp, T("advanced/usb_storage.asp"));
	}	
}

static int getSambaShareConfig(int eid, webs_t wp, int args, char_t **argv)
{
	printf("%s\n", __func__);	
	if (!gstrcmp(edit_name,"")) //add
	{		
		return websWrite(wp, T("\"%s\""),"");
	}
	else //edit
	{
		printf("pName=%s\n", edit_name);
		char condition[256]={0};
		snprintf(condition,sizeof(condition), "Name='%s'", edit_name);	
		char * pPath = jnrp_sql_getAttrByCondit_Webs("SambaShare","Path",condition);
		char * pRwLvl = jnrp_sql_getAttrByCondit_Webs("SambaShare","RwLvl",condition);
		char * pUsers = jnrp_sql_getAttrByCondit_Webs("SambaShare","Users",condition);
		char * pSimplePath[128] = {0};
		memset (pSimplePath, 0x00, sizeof(pSimplePath));	
		strncpy(pSimplePath,pPath+8,strlen(pPath)-8);
		int ret = 0;
		char* pSambaShare = (char *)malloc(128);
		memset(pSambaShare,'\0', 128);
		gstrcat(pSambaShare, edit_name);
		gstrcat(pSambaShare, pSep);
		gstrcat(pSambaShare, pSimplePath);
		gstrcat(pSambaShare, pSep);
		gstrcat(pSambaShare, pRwLvl);
		gstrcat(pSambaShare, pSep);
		gstrcat(pSambaShare, pUsers);
		ret = websWrite(wp, T("\"%s\""), pSambaShare);
		free(pSambaShare);
		return ret;
	}		
}

static int setSambaShareConfig(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);	
	char * pName = websGetVar(wp, T("file_share_name"), T(""));
	char * pPath = websGetVar(wp, T("samba_path"), T(""));
	char * pRwLvl = websGetVar(wp, T("file_share_access_level"), T(""));
	char * pUsers = websGetVar(wp, T("file_share_users"), T(""));
	int ppRwLvl = gatoi(pRwLvl);
	char pSQLCmd [256] = {0};
	memset(pSQLCmd, 0x00, sizeof(pSQLCmd));

	char ** ppQryResult = NULL;
	//check if name is duplicate
	snprintf(pSQLCmd, sizeof(pSQLCmd), "select COUNT(*) from SambaShare where Name='%s'", pName);
	ppQryResult = jnrp_sql_getByQry_Webs(pSQLCmd);
	if ((!gstrcmp(edit_name,"") && gatoi(ppQryResult[1])>0) ||
		(gstrcmp(edit_name,pName) && gatoi(ppQryResult[1])>0))
	{
		setResultCode("Samba Share with the same name already exists");	
		websRedirect(wp, T("advanced/usb_storage.asp"));	
	}
	else
	{
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
		if (!gstrcmp(edit_name,"")) //add
		{
			snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into SambaShare values ('%s','%s',%d,'%s')", pName, pPath, ppRwLvl, pUsers);					
		}
		else //edit
		{
			snprintf(pSQLCmd,sizeof(pSQLCmd), "update SambaShare set Name='%s',Path='%s',RwLvl=%d,Users='%s' where Name='%s'", pName, pPath, ppRwLvl, pUsers, edit_name);						
		}
		execSQLCmd(pSQLCmd);		
		generateSambaCfg();
		websRedirect(wp, T("advanced/usb_storage.asp"));	
	}
}

void scan_dir(webs_t wp,const char *dir,int pid)
{
	//printf("%s\n", __func__);
	struct dirent **namelist;
	int n;
	int i=0;

	n = scandir(dir, &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else 
	{
		for (i = 0; i < n; i++)
		{					
			struct stat st;
			char *fullpath = NULL;
			if (namelist[i]->d_name[0] == '.')
			{ 
				free (namelist[i]);
				continue;
			}
			fullpath = (char *)malloc (strlen (dir) + strlen (namelist[i]->d_name) + 2);
			sprintf (fullpath, "%s/%s", dir, namelist[i]->d_name);
			//printf("%s\n", fullpath);

			if (stat (fullpath, &st) < 0)
			{
				free (namelist[i]);
				free (fullpath);
				continue;
			}
			
			if (S_ISDIR (st.st_mode))
			{ //dir							
				treeview_id++;
				websWrite(wp, T("d.add(%d,%d,'%s','%s','');\n"),treeview_id,pid,namelist[i]->d_name,fullpath);									
				//printf("treeview_id=%d,pid=%d,name=%s\n",treeview_id,pid,namelist[i]->d_name);
				//printf("dir=%s\n",fullpath);					
				scan_dir(wp,fullpath,treeview_id);
			}
			else 
			{ //file           

			}

			free (namelist[i]);
			free (fullpath);
		}
		free(namelist);    
	}
}

static int getUsbTreeView(int eid, webs_t wp, int args, char_t **argv)
{
	printf("%s\n", __func__);	
	/* sample
	websWrite(wp, T("d = new dTree('d');\n"));	
	websWrite(wp, T("d.add(0,-1,'My example tree');\n"));		
	websWrite(wp, T("d.add(1,0,'Node 1','usb_storage_editor.asp');\n"));		
	websWrite(wp, T("d.add(2,0,'Node 2','usb_storage_editor.asp');\n"));	
	websWrite(wp, T("d.add(3,1,'Node 1.1','usb_storage_editor.asp');\n"));	
	websWrite(wp, T("d.add(4,0,'Node 3','usb_storage_editor.asp');\n"));		
	websWrite(wp, T("document.write(d);\n"));	
	*/
	treeview_id=1;
	websWrite(wp, T("d = new dTree('d');\n"));	
	websWrite(wp, T("d.add(0,-1,'Usb Storage Content');\n"));
	websWrite(wp, T("d.add(1,0,'/','/mnt/usb/','');\n")); //default									
	scan_dir(wp,"/mnt/usb",1);
	websWrite(wp, T("document.write(d);\n"));	
	
	return 0;
}
#endif

