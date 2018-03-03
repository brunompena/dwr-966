#include "stdsoap2.h"
#include "soapStub.h"
#include "soapH.h"
#include "httpda.h"
#include "httpget.h"
#include "httpform.h"
#include "logging.h"
#include "Common.h"
#include "tr111.h"
#include "RPCmethodImpl.h"
#include "platform.h"
#include "logger.h"
#include "dmutil.h"
#include "ipcutil.h"
#include "tr069lib.h"

struct VCNode
{
	char* name;
	char* value;
	unsigned int ValueSize;
	struct VCNode* next;
};
#ifdef CONFIG_TELENOR_NORWAY
#define PARAM_BASESIZE		11
#else
#define PARAM_BASESIZE		10
#endif
#define RETRY_COUNT			0
#define HTTP_BAD_REQUEST	400
#define HTTP_UNAUTHORIZED	401
#define HTTP_NO_CONTENT		204
#define HTTP_REDIRECT		302
#define HTTP_TEMPORARY_REDIRECT 307

#ifdef _JNR_ES_
char *ConfigFilePath = ".";
char *DataModelPath = ".";
#else
static const char *ConfigFilePath = ".";
static const char *DataModelPath = ".";
#endif
unsigned int ENABLE_TR111;
unsigned int ENABLE_SSL;
unsigned int ENABLE_VERBOSE;

#ifdef _JNR_ES_
unsigned int ENABLE_DBG = 0;
#endif

unsigned int ENABLE_BOOTSTRAP;

char *MGT_USERNAME;
char *MGT_PASSWORD;
char *CONN_REQ_AUTHREALM = "123456";
char DATA_MODEL[MAXSTRINGSIZE];
char MGT_SERVER_URL[MAXURLSIZE];
char STUN_SERVER_ADDR[MAXURLSIZE];
char COMMAND_Key_OF_DOWNLOAD[MAXSTRINGSIZE];
unsigned int CONN_REQ_PORT;
struct DeviceIdStruct Device_ID;
struct TreeNode* ROOT = NULL;
struct TreeNode* FUNCROOT = NULL;
pthread_mutex_t* Inform_mutex = NULL;
static int bServeExit = FALSE;
static int bUpTimeExit = FALSE;
static int bPeriodicInformEnable = FALSE;
static int bPeriodicInformExit = FALSE;
static int PeriodicInformInterval = -1;

#ifdef _JNR_ES_
    #ifdef _JPR_AM_
struct MethodCB Method = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    #else
struct MethodCB Method = {NULL, NULL, NULL};
    #endif
#endif
#ifdef _JPR_AM_
static int BOOTInformCheck = FALSE;
static int BOOTInformOK = FALSE;
static int InformOKLocked = FALSE;
static int isFirstTime = FALSE;
static int isEnablePing = FALSE;
static int isEnableTraceRoute = FALSE;
static int isEnableUploadDiagnostic = FALSE;
static int isEnableDownloadDiagnostic = FALSE;
static int isEnableUDPEchoDiagnostic = FALSE;
static int InformRetryCount= 0;
struct soap			gSoap;

void logCurrentTime(void){
	time_t rawtime;
	struct tm *timeinfo;
	char currenttime[64];
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	memset(currenttime, 0, sizeof(currenttime));
	sprintf(currenttime, "CurrentTime...%d-%d-%dT%d:%d:%dZ", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	LOG(LOG_INFO, "%s\n",currenttime);
}

BOOL isActiveValueChange(struct TreeNode* tree_node)
{
	struct VCNode* new_node;
	struct VCNode* tmp_node;
	if (tree_node == NULL)
		return FALSE;
	else
	{
		if(isActiveValueChange(tree_node->Child) == TRUE)
			return TRUE;
		if(isActiveValueChange(tree_node->Sibling) == TRUE)
			return TRUE;
		if (tree_node->ValueChange == TRUE)
		{
			if (tree_node->NotificationType == ACTIVE_NOTIFICATION)
			{
			    char szTotalName[MAXSTRINGSIZE];
			    DM_GetTotalName(tree_node, szTotalName);
			    LOG(LOG_DEBUG, "active value change:%s\n",szTotalName);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL isBOOTInformOK(void)
{
	return BOOTInformOK;
}
BOOL isInformOKLocked(void)
{
	return InformOKLocked;
}

int InformLockedCheck(int WANIPChanged)
{
    if(InformOKLocked == TRUE){
	    if(gSoap.socket!=-1 && (gSoap.length == 0 || WANIPChanged == 1)){
		  LOG(LOG_INFO, "Close socket...\n");
          gSoap.error = SOAP_TCP_ERROR;
          soap_closesock(&gSoap);
		  return TRUE;
		}
	}
	return FALSE;
}

static void DeleteNode(struct TreeNode* node)
{
	struct TreeNode* Tmp = NULL;
	if (node == NULL)
		return;
	else
	{
		//        0
		//      ??
		//     0-->0-->0-->0
		//   ??
		//  0-->0
		//use post order
		DeleteNode(node->Child);
		DeleteNode(node->Sibling);
		Tmp = node->Parent->Child;
		if (Tmp->Sibling == NULL)
			Tmp->Parent->Child = NULL;
		else
		{
			while(Tmp != NULL)
			{
				if (strcmp(Tmp->Sibling->Name, node->Name) == EQUALITY)
				{
					if (node->Sibling != NULL)
					{
						Tmp->Sibling = node->Sibling;
					}
					else
					{
						Tmp->Sibling = NULL;
					}
					break;
				}
				else
					Tmp = Tmp->Sibling;
			}
		}
		free(node->Name);
		free(node->Value);
		free(node);
		node = NULL;
	}
}

void ExecuteDiagnosticsCompleteInform(void) {
    unsigned int* EVENT_FLAG = NULL;
	EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
	memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
	EVENT_FLAG[DIAGNOSTICS_COMPLETE] = TRUE;
	ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);	
}

static void* PingRoutine(void *vParam)
{
    if(Method.PingCallBackFunc!=NULL){
	    Method.PingCallBackFunc();
        ExecuteDiagnosticsCompleteInform();
	}
}
void StartPingDiagnosticsRoutine()
{
	struct platformThread_t Platform;
	Platform.pfFnStartRoutine = PingRoutine;
	PLATFORM_ThreadCreate(&Platform);
}
void enablePingDiagnostic(int enable){
    isEnablePing = enable;
}

static void* TraceRouteRoutine(void *vParam)
{
    if(Method.TraceRouteCallBackFunc!=NULL){
	    Method.TraceRouteCallBackFunc();
        ExecuteDiagnosticsCompleteInform();
	}
}
void StartTraceRouteDiagnosticsRoutine()
{
	struct platformThread_t Platform;
	Platform.pfFnStartRoutine = TraceRouteRoutine;
	PLATFORM_ThreadCreate(&Platform);
}

void enableTraceRouteDiagnostic(int enable){
    isEnableTraceRoute = enable;
}

static void* UploadDiagnosticRoutine(void *vParam)
{
    if(Method.UploadDiagnosticCallBackFunc!=NULL){
	    Method.UploadDiagnosticCallBackFunc();
        ExecuteDiagnosticsCompleteInform();
	}
}
void StartUploadDiagnosticsRoutine()
{
	struct platformThread_t Platform;
	Platform.pfFnStartRoutine = UploadDiagnosticRoutine;
	PLATFORM_ThreadCreate(&Platform);
}
void enableUploadDiagnostic(int enable){
    isEnableUploadDiagnostic = enable;
}

static void* DownloadDiagnosticRoutine(void *vParam)
{
    if(Method.DownloadDiagnosticCallBackFunc!=NULL){
	    Method.DownloadDiagnosticCallBackFunc();
        ExecuteDiagnosticsCompleteInform();
	}
}

void StartDownloadDiagnosticsRoutine()
{
	struct platformThread_t Platform;
	Platform.pfFnStartRoutine = DownloadDiagnosticRoutine;
	PLATFORM_ThreadCreate(&Platform);
}
void enableDownloadDiagnostic(int enable){
    isEnableDownloadDiagnostic = enable;
}

static void* UDPEchoDiagnosticRoutine(void *vParam)
{
    PLATFORM_SleepMSec(200);//must delay for acs write port
	if(Method.UDPEchoDiagnosticCallBackFunc!=NULL)
	    Method.UDPEchoDiagnosticCallBackFunc();
	return;
}
void StartUDPEchoDiagnosticsRoutine()
{
	struct platformThread_t Platform;
	Platform.pfFnStartRoutine = UDPEchoDiagnosticRoutine;
	PLATFORM_ThreadCreate(&Platform);
}
void enableUDPEchoDiagnostic(int enable){
    if(isEnableUDPEchoDiagnostic == FALSE && enable == TRUE){
#ifdef WITH_TR098
	    DM_WriteDataToNode(DM_GetNode(ROOT,"InternetGatewayDevice.UDPEchoConfig.PacketsReceived"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"InternetGatewayDevice.UDPEchoConfig.PacketsResponded"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"InternetGatewayDevice.UDPEchoConfig.BytesReceived"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"InternetGatewayDevice.UDPEchoConfig.BytesResponded"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"InternetGatewayDevice.UDPEchoConfig.TimeFirstPacketReceived"),"");
		DM_WriteDataToNode(DM_GetNode(ROOT,"InternetGatewayDevice.UDPEchoConfig.TimeLastPacketReceived"),"");
#else
	    DM_WriteDataToNode(DM_GetNode(ROOT,"Device.UDPEchoConfig.PacketsReceived"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"Device.UDPEchoConfig.PacketsResponded"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"Device.UDPEchoConfig.BytesReceived"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"Device.UDPEchoConfig.BytesResponded"),"0");
		DM_WriteDataToNode(DM_GetNode(ROOT,"Device.UDPEchoConfig.TimeFirstPacketReceived"),"");
		DM_WriteDataToNode(DM_GetNode(ROOT,"Device.UDPEchoConfig.TimeLastPacketReceived"),"");
#endif
	    StartUDPEchoDiagnosticsRoutine();
	}
    isEnableUDPEchoDiagnostic = enable;
}
struct TreeNode* TR069_GetNode(char* pszName){
    return DM_GetNode(ROOT, pszName);
}
void TR069_DeleteNode(char* pszName){
    struct TreeNode* node = DM_GetNode(ROOT, pszName);
	if (pszName[strlen(pszName)-1] == '.')
        DeleteNode(node->Child);
	else
	    DeleteNode(node);
}

void TR069_setConnectionRequestPort(unsigned int portNum){
    CONN_REQ_PORT = portNum;
}

unsigned int TR069_getConnectionRequestPort(void){
    return CONN_REQ_PORT;
}
#endif

int isNumber(char* digit) {
   //--- Convert each digit char and add into result.
   while (*digit >= '0' && *digit <='9') {
      digit++;
   }

   //--- Check that there were no non-digits at end.
   if (*digit != 0) {
      return FALSE;
   }
   return TRUE;
}

void strncpy_pack(char* dst, char* src, int dst_size)
{
	memset(dst, 0, dst_size);
	strncpy(dst, src, dst_size - 1);
	dst[dst_size - 1] = '\0';
	if (strlen(dst) < strlen(src))
	{
		LOG(LOG_ERROR, "String copy overflow!\n");
		LOG(LOG_ERROR, "src = %s\n", src);
		LOG(LOG_ERROR, "dst = %s\n", dst);
	}
}

static int InformMutexInit()
{
	int iReturn = _SUCCESS;
	Inform_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if(pthread_mutex_init(Inform_mutex, NULL) != _SUCCESS)
	{
		LOG(LOG_ERROR, "Mutex initialization failed..\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
FUNC_EXIT:
	return iReturn;
}

static int EnvironmentInit()
{
	int i=0, iReturn = _SUCCESS;
	char szName[MAXSTRINGSIZE];
	char szValue[MAXSTRINGSIZE];
	char szPath[MAXSTRINGSIZE];
	FILE * pFile = NULL;

#ifdef _JNR_ES_
	//LOG(LOG_INFO, "JNR ES patch include.\n");
	#ifdef _JPR_AM_
	//LOG(LOG_INFO, "AM patch include.\n");
	#endif
#else
	LOG(LOG_INFO, "Original agent. \n");
#endif

	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s/Config.ini", ConfigFilePath);
	pFile = fopen(szPath,"r");
	if(!pFile){
#ifdef _JNR_ES_
		LOG(LOG_ERROR, "Can't find Config.ini under (%s)\n", ConfigFilePath);
#else
		LOG(LOG_ERROR, "Can't find Config.ini \n");
#endif
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	else
	{
		memset(szName, 0, sizeof(szName));
		memset(szValue, 0, sizeof(szValue));
		while(fscanf(pFile, "%s %s", szName, szValue)!=EOF)
		{
			if (strcmp(szName, "ENABLE_TR111") == EQUALITY)
				ENABLE_TR111 = atoi(szValue);
			else if (strcmp(szName, "ENABLE_SSL") == EQUALITY)
				ENABLE_SSL = atoi(szValue);
			else if (strcmp(szName, "ENABLE_VERBOSE") == EQUALITY)
				ENABLE_VERBOSE = atoi(szValue);
			else if (strcmp(szName, "ENABLE_BOOTSTRAP") == EQUALITY)
				ENABLE_BOOTSTRAP = atoi(szValue);
			else if (strcmp(szName, "DATA_MODEL") == EQUALITY)
			{
				strncpy_pack(DATA_MODEL, szValue, MAXSTRINGSIZE);
			}
			else
				fprintf(stderr, "[Config.ini] Undefined parameter name..%s", szName);
			i++;
			memset(szName, 0, sizeof(szName));
			memset(szValue, 0, sizeof(szValue));
		}
		fclose(pFile);
	}

	LOG(LOG_DEBUG, "ENABLE_TR111   = %d\n", ENABLE_TR111);
	LOG(LOG_DEBUG, "ENABLE_SSL     = %d\n", ENABLE_SSL);
	LOG(LOG_DEBUG, "ENABLE_VERBOSE = %d\n", ENABLE_VERBOSE);
	LOG(LOG_DEBUG, "ENABLE_BOOTSTRAP = %d\n", ENABLE_BOOTSTRAP);
	LOG(LOG_DEBUG, "DATA_MODEL     = %s\n", DATA_MODEL);
FUNC_EXIT:
	return iReturn;
}

static int InitCPEinfo(struct soap* soap, struct TreeNode* Root)
{
	char *pszTmpHead = NULL, *pszTmpTail = NULL;
	char szTmp[8];
	struct TreeNode* node;

	if(!soap)
		return _ERROR;

	/* Get the ACS url */
	int status = _SUCCESS;
	char szValue[256];

//#ifdef _JNR_ES_
//	strcpy(MGT_SERVER_URL, TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.URL"));
//#else
	node = DM_GetNode(Root, "InternetGatewayDevice.ManagementServer.URL");
	strncpy_pack(MGT_SERVER_URL, node->Value, MAXURLSIZE);
//#endif
	//sprintf(MGT_SERVER_URL, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 104, 116, 116, 112, 58, 47, 47, 50, 50, 48, 46, 49, 51, 48, 46, 49, 55, 54, 46, 50, 51, 57, 58, 56, 56, 56, 56, 47, 65, 67, 83, 47, 114, 101, 99, 101, 105, 118, 101, 114);
	//---
	/* Get the device information */
	node = DM_GetNode(Root, "InternetGatewayDevice.DeviceInfo.Manufacturer");
	Device_ID.Manufacturer = (char*)malloc(node->StringSize);
	strncpy_pack(Device_ID.Manufacturer, node->Value, node->StringSize);
	/* Get the device information */
	node = DM_GetNode(Root, "InternetGatewayDevice.DeviceInfo.ManufacturerOUI");
	Device_ID.OUI = (char*)malloc(node->StringSize);
	strncpy_pack(Device_ID.OUI, node->Value, node->StringSize);
	/* Get the device information */
	node = DM_GetNode(Root, "InternetGatewayDevice.DeviceInfo.ProductClass");
	Device_ID.ProductClass = (char*)malloc(node->StringSize);
	strncpy_pack(Device_ID.ProductClass, node->Value, node->StringSize);
	/* Get the device information */
	node = DM_GetNode(Root, "InternetGatewayDevice.DeviceInfo.SerialNumber");
	Device_ID.SerialNumber = (char*)malloc(node->StringSize);
	strncpy_pack(Device_ID.SerialNumber, node->Value, node->StringSize);

	/* Get the device ManagementServer.Username */
	node = DM_GetNode(Root, "InternetGatewayDevice.ManagementServer.Username");
	MGT_USERNAME = (char*)malloc(node->StringSize);
	strncpy_pack(MGT_USERNAME, node->Value, node->StringSize);
	/* Get the device ManagementServer.Password */
	node = DM_GetNode(Root, "InternetGatewayDevice.ManagementServer.Password");
	MGT_PASSWORD = (char*)malloc(node->StringSize);
	strncpy_pack(MGT_PASSWORD, node->Value, node->StringSize);

	pszTmpHead = strrchr((DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ConnectionRequestURL"))->Value, ':') + 1;
#ifdef _JPR_AM_
    if (CONN_REQ_PORT <= 0){
		if ( pszTmpHead != NULL)
		{
			pszTmpTail = strchr(pszTmpHead, '/');
			memset(szTmp, 0, sizeof(szTmp));
			strncpy(szTmp, pszTmpHead, pszTmpTail-pszTmpHead);
			CONN_REQ_PORT = atoi(szTmp);
		}else
		   CONN_REQ_PORT = 9000;
    }
    LOG(LOG_DEBUG, "CPEinfo=%s|%s|%s|%s\n", Device_ID.Manufacturer,Device_ID.OUI,Device_ID.ProductClass,Device_ID.SerialNumber);
    LOG(LOG_DEBUG, "CONN_REQ_PORT=%d\n", CONN_REQ_PORT);
#else
	if (pszTmpHead != NULL)
	{
		pszTmpTail = strchr(pszTmpHead, '/');
		memset(szTmp, 0, sizeof(szTmp));
		strncpy(szTmp, pszTmpHead, pszTmpTail-pszTmpHead);
		CONN_REQ_PORT = atoi(szTmp);
	}
	else
		CONN_REQ_PORT = 80;
#endif
	return _SUCCESS;
}

static int InitSOAPInfo(struct soap* Soap, struct SOAP_ENV__Header* Header)
{
	int iReturn = _SUCCESS;
	if(!Soap)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	soap_init2(Soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE | SOAP_XML_TREE);
	LOG(LOG_DEBUG, "SOAP initialize done.\n");

	//---Set cookie(only for server side)
	//Soap->cookie_domain = "localhost"; 
	//Soap->cookie_path = "/";
	//soap_set_cookie(Soap, "visit", "true", NULL, NULL); 
	//soap_set_cookie_expire(Soap, "visit", 600, NULL, NULL);
	//---
	Soap->send_timeout = 0;
	Soap->recv_timeout = 0;
	Soap->accept_timeout = 10;

	if (Header != NULL)
	{
		/* Prepare SOAP Header */
		soap_default_SOAP_ENV__Header(Soap, Header);
		Soap->header = Header;
#ifdef _JNR_ES_
		Header->cwmp__ID = "1234";
#else
		Header->cwmp__ID = 1234;
#endif
	}

	if (ENABLE_SSL)
	{
		LOG(LOG_DEBUG, "SSL Init..");
		soap_ssl_init();
		LOG(LOG_DEBUG, "SSL Init..OK\n");

		LOG(LOG_DEBUG, "Execute soap_ssl_client_context..");
		if(soap_ssl_client_context(Soap, SOAP_SSL_DEFAULT, NULL/*key file*/, NULL/*"password"*/, "uccert.pem"/*CA file*/, NULL, NULL))
		{
			LOG(LOG_DEBUG, "Execute soap_ssl_client_context..Fail\n");
			soap_print_fault(Soap, stderr);
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}
		LOG(LOG_DEBUG, "Execute soap_ssl_client_context..OK");
	}

FUNC_EXIT:
	return iReturn;
}

static int CreateEventCode(struct soap *Soap, unsigned int* EVENT_FLAG, struct EventArray* Event)
{
	int iReturn = _SUCCESS, iEventCount=0, i,j;

	if(!Soap || !Event || !EVENT_FLAG)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	for(i=0;i<MAXEVENT;i++)
	{
		if (EVENT_FLAG[i] == TRUE)
		{
			iEventCount++;
		}
	}

	Event->__size = iEventCount;
	Event->__ptrEventStruct = (struct EventStruct *) soap_malloc (Soap, sizeof(struct EventStruct) * Event->__size);

	j = 0;
	for(i=0;i<MAXEVENT;i++)
	{
		if (EVENT_FLAG[i] == TRUE)
		{
			switch(i)
			{
			case BOOTSTRAP:
				Event->__ptrEventStruct[j].EventCode="0 BOOTSTRAP";
				Event->__ptrEventStruct[j].CommandKey = "";
				break;
			case BOOT:
				Event->__ptrEventStruct[j].EventCode="1 BOOT";
				Event->__ptrEventStruct[j].CommandKey = "";
				break;
			case PERIODIC:
				Event->__ptrEventStruct[j].EventCode="2 PERIODIC";
				Event->__ptrEventStruct[j].CommandKey = "";
				break;
			case VALUE_CHANGE:
				Event->__ptrEventStruct[j].EventCode="X VALUE CHANGE";
				Event->__ptrEventStruct[j].CommandKey = "";
				break;
			case CONNECTION_REQUEST:
				Event->__ptrEventStruct[j].EventCode="6 CONNECTION REQUEST";
				Event->__ptrEventStruct[j].CommandKey = "";
				break;
			case TRANSFER_COMPLETE:
				Event->__ptrEventStruct[j].EventCode="7 TRANSFER COMPLETE";
				Event->__ptrEventStruct[j].CommandKey = COMMAND_Key_OF_DOWNLOAD;
				break;
			case M_REBOOT:
				Event->__ptrEventStruct[j].EventCode="M Reboot";
				Event->__ptrEventStruct[j].CommandKey = "";
				break;
			case M_DOWNLOAD:
				Event->__ptrEventStruct[j].EventCode="M Download";
				Event->__ptrEventStruct[j].CommandKey = "";
				break;
		#ifdef _JPR_AM_
			case DIAGNOSTICS_COMPLETE:
				Event->__ptrEventStruct[j].EventCode="8 DIAGNOSTICS COMPLETE";
				Event->__ptrEventStruct[j].CommandKey = "";
			break;
		#endif
			default:
				break;
			}
			j++;
		}
	}

FUNC_EXIT:
	return iReturn;
}

static void GetValueChangeList(struct TreeNode* tree_node, struct VCNode* vc_node, unsigned int* NodeCount)
{
	char szTotalName[MAXSTRINGSIZE];
	struct VCNode* new_node;
	struct VCNode* tmp_node;
	if (tree_node == NULL)
		return;
	else
	{
		GetValueChangeList(tree_node->Child, vc_node, NodeCount);
		GetValueChangeList(tree_node->Sibling, vc_node, NodeCount);
		if (tree_node->ValueChange == TRUE)
		{
			if (tree_node->NotificationType == PASSIVE_NOTIFICATION || tree_node->NotificationType == ACTIVE_NOTIFICATION)
			{
				memset(szTotalName, 0, sizeof(szTotalName));
				DM_GetTotalName(tree_node, szTotalName);
				if (vc_node->name == NULL && vc_node->value == NULL && vc_node->next == NULL) //first node
				{
					vc_node->name = (char*)malloc(strlen(szTotalName) + 1);
					vc_node->value = (char*)malloc(tree_node->StringSize);
					vc_node->ValueSize = tree_node->StringSize;
					strncpy_pack(vc_node->name, szTotalName, strlen(szTotalName) + 1);
					strncpy_pack(vc_node->value, tree_node->Value, vc_node->ValueSize);
				}
				else
				{
					tmp_node = vc_node;
					while (tmp_node->next != NULL)
						tmp_node = tmp_node->next;

					new_node = (struct VCNode*)malloc(sizeof(struct VCNode));
					new_node->next = NULL;
					new_node->name = (char*)malloc(strlen(szTotalName) + 1);
					new_node->value = (char*)malloc(tree_node->StringSize);
					new_node->ValueSize = tree_node->StringSize;
					strncpy_pack(new_node->name, szTotalName, strlen(szTotalName) + 1);
					strncpy_pack(new_node->value, tree_node->Value, new_node->ValueSize);
					tmp_node->next = new_node;
				}
				(*NodeCount) ++;
			}
			#ifndef _JPR_AM_
			tree_node->ValueChange = FALSE;
			#endif
		}
	}
}

static int CreateParameterList(struct soap* Soap, unsigned int* EVENT_FLAG, struct ParameterList_Value* ParaList)
{
	unsigned int num_valuechange = 0;  
	int index = 0, count = 0, i = 0;
	struct VCNode *vcnode, *tmp;
	BOOL bNAT = FALSE;
#ifdef _JNR_ES_
	int status = _SUCCESS;
	char szValue[256];
#endif
#ifdef _JPR_AM_
    int param_size = 0;
#endif

	if(!Soap || !ParaList || !EVENT_FLAG)
		return _ERROR;

	ParaList->__size = PARAM_BASESIZE;
	
	if (strcmp((DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.NATDetected"))->Value, "true") == EQUALITY)
	{
		bNAT = TRUE;
		ParaList->__size += 2;
	}

	vcnode = (struct VCNode*)malloc(sizeof(struct VCNode));
	vcnode->name = vcnode->value = NULL;
	vcnode->next = NULL;
	GetValueChangeList(ROOT, vcnode, &num_valuechange);

	if (num_valuechange > 0)
	{
		EVENT_FLAG[VALUE_CHANGE] = TRUE;
		ParaList->__size += num_valuechange;
	}
	else
		free(vcnode);

	ParaList->__ptrParameterValueStruct = (struct ParameterValueStruct *) soap_malloc (Soap, sizeof(struct ParameterValueStruct) * ParaList->__size);


#ifdef _JPR_AM_
	TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.ConnectionRequestURL");
	TR069_GetParameterValue2("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress");
    #ifdef CONFIG_TELENOR_NORWAY
	TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.X_LTEVersion");
	#else
	TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.ModemFirmwareVersion");
	#endif
	ParaList->__ptrParameterValueStruct[0].Name = "InternetGatewayDevice.DeviceSummary";
	ParaList->__ptrParameterValueStruct[0].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceSummary"))->Value;
	ParaList->__ptrParameterValueStruct[1].Name = "InternetGatewayDevice.DeviceInfo.SpecVersion";
	ParaList->__ptrParameterValueStruct[1].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.SpecVersion"))->Value;
	ParaList->__ptrParameterValueStruct[2].Name = "InternetGatewayDevice.DeviceInfo.HardwareVersion";
	ParaList->__ptrParameterValueStruct[2].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.HardwareVersion"))->Value;
	ParaList->__ptrParameterValueStruct[3].Name = "InternetGatewayDevice.DeviceInfo.SoftwareVersion";
	ParaList->__ptrParameterValueStruct[3].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.SoftwareVersion"))->Value;
	ParaList->__ptrParameterValueStruct[4].Name = "InternetGatewayDevice.DeviceInfo.ProvisioningCode";
	ParaList->__ptrParameterValueStruct[4].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.ProvisioningCode"))->Value;
	ParaList->__ptrParameterValueStruct[5].Name = "InternetGatewayDevice.ManagementServer.ConnectionRequestURL";
	ParaList->__ptrParameterValueStruct[5].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ConnectionRequestURL"))->Value;
	ParaList->__ptrParameterValueStruct[6].Name = "InternetGatewayDevice.ManagementServer.ParameterKey";
	ParaList->__ptrParameterValueStruct[6].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ParameterKey"))->Value;
	ParaList->__ptrParameterValueStruct[7].Name = "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress";
	ParaList->__ptrParameterValueStruct[7].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress"))->Value;
    #ifdef CONFIG_TELENOR_NORWAY
	ParaList->__ptrParameterValueStruct[8].Name = "InternetGatewayDevice.DeviceInfo.X_LTEVersion";
	ParaList->__ptrParameterValueStruct[8].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.X_LTEVersion"))->Value;
	#else
	ParaList->__ptrParameterValueStruct[8].Name = "InternetGatewayDevice.DeviceInfo.ModemFirmwareVersion";
	ParaList->__ptrParameterValueStruct[8].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.ModemFirmwareVersion"))->Value;
	#endif
	ParaList->__ptrParameterValueStruct[9].Name = "InternetGatewayDevice.DeviceInfo.X_ModuleIMEI";
	ParaList->__ptrParameterValueStruct[9].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.X_ModuleIMEI"))->Value;
	#ifdef CONFIG_TELENOR_NORWAY
	ParaList->__ptrParameterValueStruct[10].Name = "InternetGatewayDevice.DeviceInfo.X_IMSI";
	ParaList->__ptrParameterValueStruct[10].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.X_IMSI"))->Value;
	param_size = 11;
	#else
	param_size = 10;
	#endif
#else
	ParaList->__ptrParameterValueStruct[0].Name = "InternetGatewayDevice.DeviceSummary";
	ParaList->__ptrParameterValueStruct[0].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceSummary"))->Value;
	ParaList->__ptrParameterValueStruct[1].Name = "InternetGatewayDevice.ManagementServer.ConnectionRequestURL";
	ParaList->__ptrParameterValueStruct[1].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ConnectionRequestURL"))->Value;
	ParaList->__ptrParameterValueStruct[2].Name = "InternetGatewayDevice.ManagementServer.ConnectionRequestUsername";
	ParaList->__ptrParameterValueStruct[2].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ConnectionRequestUsername"))->Value;
	ParaList->__ptrParameterValueStruct[3].Name = "InternetGatewayDevice.ManagementServer.ConnectionRequestPassword";
	ParaList->__ptrParameterValueStruct[3].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ConnectionRequestPassword"))->Value;
	ParaList->__ptrParameterValueStruct[4].Name = "InternetGatewayDevice.ManagementServer.ParameterKey";
	ParaList->__ptrParameterValueStruct[4].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ParameterKey"))->Value;
	ParaList->__ptrParameterValueStruct[5].Name = "InternetGatewayDevice.DeviceInfo.SoftwareVersion";
	ParaList->__ptrParameterValueStruct[5].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.SoftwareVersion"))->Value;
	ParaList->__ptrParameterValueStruct[6].Name = "InternetGatewayDevice.DeviceInfo.HardwareVersion";
	ParaList->__ptrParameterValueStruct[6].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.HardwareVersion"))->Value;
	ParaList->__ptrParameterValueStruct[7].Name = "InternetGatewayDevice.DeviceInfo.ProvisioningCode";
	ParaList->__ptrParameterValueStruct[7].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.ProvisioningCode"))->Value;
	ParaList->__ptrParameterValueStruct[8].Name = "InternetGatewayDevice.DeviceInfo.SpecVersion";
	ParaList->__ptrParameterValueStruct[8].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.SpecVersion"))->Value;
	ParaList->__ptrParameterValueStruct[9].Name = "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress";
	ParaList->__ptrParameterValueStruct[9].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress"))->Value;
#endif
#ifdef _JPR_AM_
	if (bNAT == TRUE)
	{
		ParaList->__ptrParameterValueStruct[param_size].Name = "InternetGatewayDevice.ManagementServer.NATDetected";
		ParaList->__ptrParameterValueStruct[param_size].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.NATDetected"))->Value;
		param_size++;
		ParaList->__ptrParameterValueStruct[param_size].Name = "InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress";
		ParaList->__ptrParameterValueStruct[param_size].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress"))->Value;
		i = param_size +1;
	}
	else
		i = param_size;
#else
	if (bNAT == TRUE)
	{
		ParaList->__ptrParameterValueStruct[10].Name = "InternetGatewayDevice.ManagementServer.NATDetected";
		ParaList->__ptrParameterValueStruct[10].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.NATDetected"))->Value;
		ParaList->__ptrParameterValueStruct[11].Name = "InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress";
		ParaList->__ptrParameterValueStruct[11].Value= (DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress"))->Value;
		i = PARAM_BASESIZE + 2;
	}
	else
		i = PARAM_BASESIZE;
#endif
	if (num_valuechange > 0)
	{
		while(vcnode != NULL)
		{
			ParaList->__ptrParameterValueStruct[i].Name = (char*)soap_malloc(Soap, strlen(vcnode->name) + 1);
			ParaList->__ptrParameterValueStruct[i].Value = (char*)soap_malloc(Soap, vcnode->ValueSize);
			strncpy_pack(ParaList->__ptrParameterValueStruct[i].Name, vcnode->name, strlen(vcnode->name) + 1);
			strncpy_pack(ParaList->__ptrParameterValueStruct[i].Value, vcnode->value, vcnode->ValueSize);
			tmp = vcnode;
			vcnode = vcnode->next;
			free(tmp->name);free(tmp->value);free(tmp);
			i++;
		}
	}

	return _SUCCESS;
}

static void* InformRoutine(void *vParam)
{
	//struct soap			Soap;
	struct EventArray		Eventlist;
	struct http_da_info		Info;
	struct SOAP_ENV__Header		Header;
	struct cwmp__InformResponse	Result;
	struct ParameterList_Value	Parameterlist;
	struct platformThread_t		Platform;
	struct TreeNode*		node;
	struct tm*			timeinfo;
	time_t rawtime;
	
	int i = 0;
	int bSetDigest = FALSE;
	unsigned int retcode;
	unsigned int* EVENT_FLAG = (unsigned int*)vParam;
	char currenttime[MAXTIMESIZE];

	PLATFORM_Mutex_lock(Inform_mutex);
#ifdef _JPR_AM_
    InformOKLocked = TRUE;
#endif
	if (InitSOAPInfo(&gSoap, &Header) != _SUCCESS)
		LOG(LOG_ERROR, "Initial SOAP info fail...\n");

	/* Register Digest Authentication plugin */
	if (soap_register_plugin(&gSoap, http_da))
		soap_print_fault(&gSoap, stderr);

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	memset(currenttime, 0, sizeof(currenttime));
	sprintf(currenttime, "%d-%d-%dT%d:%d:%dZ", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	if (CreateParameterList(&gSoap, EVENT_FLAG, &Parameterlist) != _SUCCESS)
		LOG(LOG_ERROR, "Create parameter list failure.\n");

	if (CreateEventCode(&gSoap, EVENT_FLAG, &Eventlist) != _SUCCESS)
		LOG(LOG_ERROR, "Create event code failure.\n");
#ifdef _JPR_AM_
    for(i=0;i<Eventlist.__size;i++)
	{
		LOG(LOG_DEBUG, "EventCode[%d]=%s\n", i, Eventlist.__ptrEventStruct[i].EventCode);
	}
#endif
	LOG(LOG_DEBUG, "Size of ParameterList is %d\n", Parameterlist.__size);
	for(i=0;i<Parameterlist.__size;i++)
	{
#ifdef _JPR_AM_
		LOG(LOG_DEBUG, "parameterlist[%d]=%s\n", i, Parameterlist.__ptrParameterValueStruct[i].Name);
		LOG(LOG_DEBUG, "Value=%s\n", Parameterlist.__ptrParameterValueStruct[i].Value);
#else
		LOG(LOG_DEBUG, "parameterlist.__ptrParameterValueStruct[%d].Name =%s\n", i, Parameterlist.__ptrParameterValueStruct[i].Name);
		LOG(LOG_DEBUG, "parameterlist.__ptrParameterValueStruct[%d].Value=%s\n", i, Parameterlist.__ptrParameterValueStruct[i].Value);
#endif
	}
	
	//LOG(LOG_DEBUG, "Delay 200ms\n");
	//PLATFORM_SleepMSec(200);//must delay for acs write db!!
	LOG(LOG_INFO, "Execute Inform..<%s>\n", MGT_SERVER_URL);
	/* Send a Inform request */
	retcode = soap_call_cwmp__Inform(&gSoap, MGT_SERVER_URL, "", &Device_ID, &Eventlist, MAXENVELOPES, currenttime, RETRY_COUNT, &Parameterlist, &Result);
#ifdef _JPR_AM_
	logCurrentTime();
#endif	
	if (retcode == HTTP_UNAUTHORIZED)
	{
		LOG(LOG_DEBUG, "Execute Inform..UNAUTHORIZED\n");
		if (strstr(gSoap.buf, "WWW-Authenticate: Digest") != NULL)
		{
			http_da_save(&gSoap, &Info, gSoap.authrealm, MGT_USERNAME, MGT_PASSWORD);
			bSetDigest = TRUE;
		}
		else
		{
			gSoap.userid = MGT_USERNAME;
			gSoap.passwd = MGT_PASSWORD;
			soap_restore_post_parsehdr(&gSoap);
		}
		LOG(LOG_DEBUG, "Execute Inform..again\n");
		retcode = soap_call_cwmp__Inform(&gSoap, MGT_SERVER_URL, "", &Device_ID, &Eventlist, MAXENVELOPES, currenttime, RETRY_COUNT, &Parameterlist, &Result);
	}
	else if (retcode == HTTP_REDIRECT || retcode == HTTP_TEMPORARY_REDIRECT)
	{
		strncpy_pack(MGT_SERVER_URL, gSoap.endpoint, MAXURLSIZE);
#ifdef WITH_TR098
		node = DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.URL");
#elif WITH_TR106
		node = DM_GetNode(ROOT, "Device.ManagementServer.URL");
#endif
		strncpy_pack(node->Value, gSoap.endpoint, node->StringSize);
		strncpy_pack(MGT_SERVER_URL, gSoap.endpoint, MAXURLSIZE);
		Platform.pfFnStartRoutine = InformRoutine;
		Platform.pThreadParm = (void*)EVENT_FLAG;
		PLATFORM_ThreadCreate(&Platform);
		goto FUNC_EXIT;
	}
	else if (retcode == HTTP_BAD_REQUEST)
	{
		Platform.pfFnStartRoutine = InformRoutine;
		PLATFORM_ThreadCreate(&Platform);
	}

	if(retcode == _SUCCESS)
	{
		LOG(LOG_INFO, "Execute Inform..OK\n");
#ifdef _JPR_AM_
		BOOTInformOK = TRUE;
		for(i=0; i < Parameterlist.__size; i++ ){
		    node = DM_GetNode(ROOT, Parameterlist.__ptrParameterValueStruct[i].Name);
			if(node->ValueChange == TRUE 
			    && strcmp(Parameterlist.__ptrParameterValueStruct[i].Value, node->Value) == 0){
			    node->ValueChange = FALSE;
				//LOG(LOG_INFO, "%s->ValueChange = FALSE\n",Parameterlist.__ptrParameterValueStruct[i].Name);
			}
		}
		InformRetryCount = 0;
		if(BOOTInformCheck == FALSE){ //avoid sending value change.
		#ifdef CONFIG_TELENOR_NORWAY
		    TR069_GetParameterValue2("InternetGatewayDevice.X_LteDevice.1.Interface.ExternalIPAddress");
		#else
			TR069_GetParameterValue2("InternetGatewayDevice.X_LteDevice.Interface.1.ExternalIPAddress");
			TR069_GetParameterValue2("InternetGatewayDevice.X_LteDevice.Interface.2.ExternalIPAddress");
			#ifdef WITH_TRIPLE_APN
			TR069_GetParameterValue2("InternetGatewayDevice.X_LteDevice.Interface.3.ExternalIPAddress");
			#endif
		#endif
		}
#endif
		if(bSetDigest == TRUE)
			http_da_restore(&gSoap, &Info);//reuse userid and passwd
		else
		{
			gSoap.userid = MGT_USERNAME;
			gSoap.passwd = MGT_PASSWORD;
			soap_restore_post_parsehdr(&gSoap);
		}
		for(i=0;i<MAXEVENT;i++)
			EVENT_FLAG[i] = FALSE;
	}
	else
	{
#ifdef _JPR_AM_
        InformRetryCount++;
		LOG(LOG_ERROR, "Execute Inform..Fail %d\n",InformRetryCount);
#else
		LOG(LOG_ERROR, "Execute Inform..Fail\n");
#endif
		LOG(LOG_ERROR, "retcode = %d\n", retcode);
		soap_print_fault(&gSoap, stderr);
		goto FUNC_EXIT;
	}

	/* Empty SOAP message */
	gSoap.length = 0;
	gSoap.count = 0;
	if (soap_connect(&gSoap, MGT_SERVER_URL, "") != _SUCCESS)
	{
		LOG(LOG_DEBUG, "Soap connection is failure.\n");
		soap_print_fault(&gSoap, stderr);
		goto FUNC_EXIT;
	}

	if (soap_end_send(&gSoap) != _SUCCESS)
	{
		LOG(LOG_DEBUG, "Soap sending is failure\n");
		soap_print_fault(&gSoap, stderr);
		goto FUNC_EXIT;
	}

	LOG(LOG_DEBUG, "Execute soap serve..\n");
	if (soap_serve(&gSoap)!= _SUCCESS) 
	{
		LOG(LOG_DEBUG, "Execute soap serve..Fail\n");
		soap_print_fault(&gSoap, stderr);
		goto FUNC_EXIT;
	}
	LOG(LOG_DEBUG, "Execute soap serve..OK\n");
	free(EVENT_FLAG);
#ifdef _JPR_AM_
	logCurrentTime();
#endif
FUNC_EXIT:
	
	if(bSetDigest == TRUE)
		http_da_release(&gSoap, &Info);
	soap_destroy(&gSoap);
	soap_end(&gSoap);
	soap_done(&gSoap);
	
	PLATFORM_Mutex_unlock(Inform_mutex);
#ifdef _JPR_AM_
    InformOKLocked = FALSE;
	if(BOOTInformCheck == FALSE){
		if(retcode == _SUCCESS){
			BOOTInformCheck = TRUE;
			DM_CALLBACK_PeriodicInformEnable((
			    DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.PeriodicInformEnable"))->Value,
			    "InternetGatewayDevice.ManagementServer.PeriodicInformEnable");
			if(isFirstTime == TRUE) {
			    TR069_SetParameterValue("InternetGatewayDevice.DeviceInfo.FirstUseDate",currenttime);
			    isFirstTime = FALSE;
			}
		} else if (retcode != HTTP_REDIRECT && retcode != HTTP_TEMPORARY_REDIRECT
		    && retcode != HTTP_BAD_REQUEST){
			if(InformRetryCount < 10) {
	            //LOG(LOG_DEBUG, "sleep 20 second, than retry inform \n");
				if(InformRetryCount < 5)
	                PLATFORM_SleepSec(10);
				else
				    PLATFORM_SleepSec(30);
			} else
			    PLATFORM_SleepSec(600);
		    ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
			return;
		}
	}
	if(retcode != HTTP_REDIRECT && retcode != HTTP_TEMPORARY_REDIRECT
		    && retcode != HTTP_BAD_REQUEST && isActiveValueChange(ROOT) == TRUE)
	{
	    InformOKLocked = TRUE;//avoid sending value change
		if(retcode != _SUCCESS) {
		    if(InformRetryCount < 10)
		        PLATFORM_SleepSec(3);
		    else if(InformRetryCount < 15)
		        PLATFORM_SleepSec(30);
		    else
		        PLATFORM_SleepSec(600);
		} else {
		    EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
		    memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
		    //EVENT_FLAG[VALUE_CHANGE] = TRUE;
		}
		LOG(LOG_DEBUG, "value change inform \n");
		ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
    }
	if(isEnablePing == TRUE){
		isEnablePing = FALSE;
	    StartPingDiagnosticsRoutine();
		return;
	}
	if(isEnableTraceRoute == TRUE){
	    isEnableTraceRoute = FALSE;
	    StartTraceRouteDiagnosticsRoutine();
		return;
	}
	if(isEnableUploadDiagnostic){
	    isEnableUploadDiagnostic = FALSE;
	    StartUploadDiagnosticsRoutine();
		return;
	}
	if(isEnableDownloadDiagnostic){
	    isEnableDownloadDiagnostic = FALSE;
	    StartDownloadDiagnosticsRoutine();
		return;
	}
#endif
}

static void* PeriodicInform(void *vParam)
{
	unsigned int* EVENT_FLAG = NULL;
	while(1)
	{
		LOG(LOG_DEBUG, "PeriodicInform Interval = %d\n", PeriodicInformInterval);
		if (PeriodicInformInterval <= 0)
		{
			PLATFORM_SleepMSec(1000);
		}
		else
		{
			PLATFORM_SleepSec(PeriodicInformInterval);
			if (bPeriodicInformExit == TRUE)
			{
				LOG(LOG_DEBUG, "bPeriodicInformExit = %d\n", bPeriodicInformExit);
				bPeriodicInformExit = FALSE;
				break;
			}
			#ifdef _JPR_AM_
			InformLockedCheck(0);
			#endif
			EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
			memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
			EVENT_FLAG[PERIODIC] = TRUE;
			InformRoutine((void*)EVENT_FLAG);
		}
	}
	return 0;
}

void ExecuteInform(int type, unsigned int* EVENT_FLAG)
{
	struct platformThread_t Platform;
	Platform.pThreadParm = (void*)EVENT_FLAG;
	if (type == INFORM_TYPE_IMMEDIATE)
	{
		Platform.pfFnStartRoutine = InformRoutine;
		PLATFORM_ThreadCreate(&Platform);
	}
	else if (type == INFORM_TYPE_PERIODIC)
	{
		Platform.pfFnStartRoutine = PeriodicInform;
		PLATFORM_ThreadCreate(&Platform);
	}
}

int DM_CALLBACK_PeriodicInformEnable(char* value, const char* ObjectPath)
{
	char* enable = value;
	unsigned int* EVENT_FLAG = NULL;
#ifdef _JPR_AM_
	if (strcmp(enable, "true") == EQUALITY || strcmp(enable, "1") == EQUALITY)
#else
	if (strcmp(enable, "true") == EQUALITY)
#endif
	{
		if (bPeriodicInformEnable == FALSE)
		{
			LOG(LOG_DEBUG, "PeriodicInform Enable..\n");
			bPeriodicInformEnable = TRUE;
			bPeriodicInformExit = FALSE;
			EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
			memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
			ExecuteInform(INFORM_TYPE_PERIODIC, EVENT_FLAG);
		}
	}
#ifdef _JPR_AM_
    else
#else
	else if (strcmp(enable, "false") == EQUALITY)
#endif
	{
		LOG(LOG_DEBUG, "PeriodicInform Disable..\n");
		bPeriodicInformEnable = FALSE;
		bPeriodicInformExit = TRUE;
		PeriodicInformInterval = -1;
	}

	return _SUCCESS;
}

int DM_CALLBACK_PeriodicInformInterval(char* value, const char* ObjectPath)
{
	char* Interval = value;
	PeriodicInformInterval = atoi(Interval);
	return _SUCCESS;
}

int DM_CALLBACK_STUNEnable(char* value, const char* ObjectPath)
{
	int iReturn = _SUCCESS;

#ifdef _JPR_AM_
	if (strcmp(value, "true") == EQUALITY || strcmp(value, "1") == EQUALITY)
#else
	if (strcmp(value, "true") == EQUALITY)
#endif
		iReturn = TR111_Initial();
	else if (strcmp((char*)value, "false") == EQUALITY)
		iReturn = TR111_Release();

	return iReturn;
}

void TransferCompleteRoutine(unsigned int* EVENT_FLAG, char *starttime, char *completetime, unsigned int faultcode, char* CommandKey)
{
	struct soap Soap;
	struct SOAP_ENV__Header			header;
	struct http_da_info			Info;
	struct EventArray			Eventlist;
	struct cwmp__FaultStruct		faultstruct;
	struct ParameterList_Value		Parameterlist;
	struct cwmp__InformResponse 		Result;
	struct cwmp__TransferCompleteResponse 	dummy;
	xsd__unsignedInt retcode;
	time_t rawtime;
	struct tm *timeinfo;
	char currenttime[MAXTIMESIZE];
	int bSetDigest = FALSE;
	int i = 0;

	PLATFORM_Mutex_lock(Inform_mutex);

	strncpy_pack(COMMAND_Key_OF_DOWNLOAD, CommandKey, MAXSTRINGSIZE);

	/* Prepare a SOAP structure to activate a tr-069 session */	
	if (InitSOAPInfo(&Soap, &header) != _SUCCESS)
		LOG(LOG_DEBUG, "Initial SOAP info fail...\n");

	/* Register Digest Authentication plugin */
	if (soap_register_plugin(&Soap, http_da))
		soap_print_fault(&Soap, stderr);

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	sprintf(currenttime, "%d-%d-%dT%d:%d:%dZ", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	if(CreateEventCode(&Soap, EVENT_FLAG, &Eventlist)!=SOAP_OK)
		LOG(LOG_DEBUG, "Create event code fail...\n");
#ifdef _JPR_AM_
    for(i=0;i<Eventlist.__size;i++)
	{
		LOG(LOG_DEBUG, "EventCode[%d]=%s\n", i, Eventlist.__ptrEventStruct[i].EventCode);
	}
#endif
	if(CreateParameterList(&Soap, EVENT_FLAG, &Parameterlist)!=SOAP_OK)
		LOG(LOG_DEBUG, "Create parameter list fail...\n");

	LOG(LOG_DEBUG, "parameterlist.__size=%d\n", Parameterlist.__size);
	for(i=0;i<Parameterlist.__size;i++)
	{
#ifdef _JPR_AM_
		LOG(LOG_DEBUG, "parameterlist[%d] =%s\n", i, Parameterlist.__ptrParameterValueStruct[i].Name);
		LOG(LOG_DEBUG, "Value=%s\n", Parameterlist.__ptrParameterValueStruct[i].Value);
#else
		LOG(LOG_DEBUG, "parameterlist.__ptrParameterValueStruct[%d].Name =%s\n", i, Parameterlist.__ptrParameterValueStruct[i].Name);
		LOG(LOG_DEBUG, "parameterlist.__ptrParameterValueStruct[%d].Value=%s\n", i, Parameterlist.__ptrParameterValueStruct[i].Value);
#endif
	}

	LOG(LOG_INFO, "Execute TransferComplete Inform..\n");
	retcode = soap_call_cwmp__Inform(&Soap, MGT_SERVER_URL, "", &Device_ID, &Eventlist, MAXENVELOPES, currenttime, RETRY_COUNT, &Parameterlist, &Result);
	if (retcode == HTTP_UNAUTHORIZED)
	{
		LOG(LOG_DEBUG, "Execute TransferComplete Inform..UNAUTHORIZED\n");
		if (strstr(Soap.buf, "WWW-Authenticate: Digest") != NULL)
		{
			http_da_save(&Soap, &Info, Soap.authrealm, MGT_USERNAME, MGT_PASSWORD);
			bSetDigest = TRUE;
		}
		else
		{
			Soap.userid = MGT_USERNAME;
			Soap.passwd = MGT_PASSWORD;
			soap_restore_post_parsehdr(&Soap);
		}
		LOG(LOG_DEBUG, "Execute TransferComplete Inform..again\n");
		retcode = soap_call_cwmp__Inform(&Soap, MGT_SERVER_URL, "", &Device_ID, &Eventlist, MAXENVELOPES, currenttime, RETRY_COUNT, &Parameterlist, &Result);
	}
#ifdef _JPR_AM_
	logCurrentTime();
#endif
	if(retcode == _SUCCESS)
	{
		LOG(LOG_INFO, "Execute TransferComplete Inform..OK\n");
		if(bSetDigest == TRUE)
			http_da_restore(&Soap, &Info);//reuse userid and passwd
		else
		{
			Soap.userid = MGT_USERNAME;
			Soap.passwd = MGT_PASSWORD;
			soap_restore_post_parsehdr(&Soap);
		}
		for(i=0;i<MAXEVENT;i++)
			EVENT_FLAG[i] = FALSE;
	}
	else
	{
		LOG(LOG_ERROR, "Execute TransferComplete Inform..Fail\n");
		//LOG(LOG_ERROR, "retcode = %d\n", retcode);
		soap_print_fault(&Soap, stderr);
		goto FUNC_EXIT;
	}

	faultstruct.FaultCode = faultcode;
#ifdef _JPR_AM_
	faultstruct.FaultString = (char *) soap_malloc (&Soap, sizeof(char)*64);
#else
	faultstruct.FaultString = (char *) soap_malloc (&Soap, sizeof(char)*MAXSTRINGSIZE);
#endif
	if (faultcode == INTERNAL_ERROR)
		strcpy(faultstruct.FaultString, "Internal error");
	else if (faultcode == DOWNLOAD_FAILURE)
		strcpy(faultstruct.FaultString, "Download Failure");
#ifdef _JPR_AM_
	else if (faultcode == UPLOAD_FAILURE)
		strcpy(faultstruct.FaultString, "Upload Failure");
#endif
	else if (faultcode == FILE_TRANSFER_SERVER_AUTHENTICATION_FAILURE)
		strcpy(faultstruct.FaultString, "File transfer server authentication failure");
	else
		strcpy(faultstruct.FaultString, "");
#ifdef _JPR_AM_
    LOG(LOG_DEBUG, "faultcode=%d...%s\n",faultcode,faultstruct.FaultString);
#endif
	if (soap_call_cwmp__TransferComplete(&Soap, MGT_SERVER_URL, "", CommandKey, &faultstruct, starttime, completetime, &dummy) == SOAP_OK) 
		soap_dealloc(&Soap, faultstruct.FaultString);
	else 
		soap_print_fault(&Soap, stderr);

FUNC_EXIT:
	free(EVENT_FLAG);
	if(bSetDigest == TRUE)
		http_da_release(&Soap, &Info);
	soap_destroy(&Soap);
	soap_end(&Soap);
	soap_done(&Soap);

	PLATFORM_Mutex_unlock(Inform_mutex);
}

static int CheckAuthentication(struct soap *soap)
{
	int iReturn = _SUCCESS;
	struct TreeNode* UserName;
	struct TreeNode* PassWord;
	if(!soap)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	LOG(LOG_INFO, "Execute CheckAuthentication..\n");

#ifdef WITH_TR098
	UserName = DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ConnectionRequestUsername");
	PassWord = DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ConnectionRequestPassword");
#elif WITH_TR106
	UserName = DM_GetNode(ROOT, "Device.ManagementServer.ConnectionRequestUsername");
	PassWord = DM_GetNode(ROOT, "Device.ManagementServer.ConnectionRequestPassword");
#endif
	if (soap->userid && soap->passwd) // client used basic authentication
	{
	//	LOG(LOG_DEBUG, "soap's userid  = %s\n[DEBUG] Agent's userid = %s\n", soap->userid, UserName->Value);
	//	LOG(LOG_DEBUG, "soap's passwd  = %s\n[DEBUG] Agent's passwd = %s\n", soap->passwd, PassWord->Value);
		if (strcmp(soap->userid, UserName->Value) != EQUALITY || strcmp(soap->passwd, PassWord->Value) != EQUALITY)
		{
			LOG(LOG_ERROR, "Execute CheckAuthentication..Fail\n");
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}
	} 
	else if (soap->authrealm && soap->userid) // Digest authentication 
	{ 
		LOG(LOG_DEBUG, "soap's userid  = %s\n[DEBUG] Agent's userid = %s\n", soap->userid, UserName->Value);
		LOG(LOG_DEBUG, "soap's authrealm  = %s\n[DEBUG] Agent's authrealm = %s\n", soap->authrealm, CONN_REQ_AUTHREALM);
		if (strcmp(soap->authrealm, CONN_REQ_AUTHREALM) != EQUALITY || 
			strcmp(soap->userid, UserName->Value)		!= EQUALITY ||
			http_da_verify_get(soap, PassWord->Value)	!= EQUALITY) 
		{ 
			LOG(LOG_ERROR, "Execute CheckAuthentication..Fail\n");
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}
	}
	else
	{
		LOG(LOG_INFO, "Execute CheckAuthentication..No userid/psword\n");
		soap->authrealm = CONN_REQ_AUTHREALM;
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	LOG(LOG_INFO, "Execute CheckAuthentication..OK\n");
	
FUNC_EXIT:
	return iReturn;
}

static int HttpGet_Callback(struct soap *soap)
{
	int iReturn = _SUCCESS;
	unsigned int* EVENT_FLAG = NULL;
	if(!soap)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	/* Check requestor's authentication: */
	//if (CheckAuthentication(soap) == _ERROR) 
	//{
	//	iReturn = HTTP_UNAUTHORIZED; /* HTTP not authorized */
	//	goto FUNC_EXIT;
	//}

	/* Be triggered to handle the transaction with ACS by another session */
	if (!strncmp(soap->path, "/Inform?", 6)) {
	#ifdef _JPR_AM_
	    if(isInformOKLocked() == TRUE){
		    if(InformLockedCheck(0)==FALSE){
			    // Write a HTTP 503 Error message if we are not ready to serv the request
			    LOG(LOG_DEBUG, "Write a HTTP 503 Error message \n");
			    soap->fresponse( soap, 503, 0 );
			    soap_closesock (soap);
	            return iReturn;
			}
		}
	#endif
 		/* Respond to trigger */
		soap_response(soap, SOAP_HTML);
		soap_send(soap, "CPE is triggered");
		soap_end_send(soap);
		soap->keep_alive = 0;

		//---fire Inform
		EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
		memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
		EVENT_FLAG[CONNECTION_REQUEST] = TRUE;
		ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
		//---
	}
	else
	{
		LOG(LOG_DEBUG, "Trigger format is incorrect..\n");
		iReturn = _ERROR;
	}

FUNC_EXIT:
	return iReturn;
}

static void* UpTimeRoutine(void *vParam)
{
	unsigned int Count = 0;
	char szTime[32];
	while(1)
	{
		PLATFORM_SleepMSec(1000);
		Count++;
		memset(szTime, 0, sizeof(szTime));
		sprintf(szTime, "%d", Count);
		TR069_SetParameterValue("InternetGatewayDevice.DeviceInfo.UpTime", szTime);
		if (bUpTimeExit == TRUE)
		{
			LOG(LOG_DEBUG, "bUpTimeExit = %d\n", bUpTimeExit);
			bUpTimeExit = FALSE;
			break;
		}
	}
}

void StartUpTimeRoutine()
{
	struct platformThread_t Platform;
	Platform.pfFnStartRoutine = UpTimeRoutine;
	PLATFORM_ThreadCreate(&Platform);
}

int TR069_Start(TR069_SetupCallBackFunc SetupCallBackFunc)
{
	struct soap Soap; 
	SOAP_SOCKET master, slave;
	char szPath[MAXSTRINGSIZE];
	unsigned int* EVENT_FLAG = NULL;
#ifdef _JPR_AM_
	SetupCallBackFunc();
#endif
	if (EnvironmentInit() == _ERROR)
		return 0;
	if (InformMutexInit() == _ERROR)
		return 0;

	LOG(LOG_DEBUG, "AgentCode v1.0.\n");
#ifndef _JPR_AM_
	TR069_RegisterSETOptFunc("InternetGatewayDevice.ManagementServer.PeriodicInformEnable", (TR069_SETOptCallBackFunc)DM_CALLBACK_PeriodicInformEnable);
	TR069_RegisterSETOptFunc("InternetGatewayDevice.ManagementServer.PeriodicInformInterval", (TR069_SETOptCallBackFunc)DM_CALLBACK_PeriodicInformInterval);
	TR069_RegisterSETOptFunc("InternetGatewayDevice.ManagementServer.STUNEnable", (TR069_SETOptCallBackFunc)DM_CALLBACK_STUNEnable);
#endif
	memset(szPath, 0, sizeof(szPath));
	sprintf(szPath, "%s/%s", DataModelPath, DATA_MODEL);
	//---Can't modify the order
	ROOT = DM_ImportDataModel(szPath);

	RPCIMPL_SetupCallback();
#ifndef _JPR_AM_
	IPC_StartDaemon();
	//---
	SetupCallBackFunc();
#endif
	if (InitSOAPInfo(&Soap, NULL) != _SUCCESS)
		LOG(LOG_ERROR, "Initial SOAP info. fail!\n");
	
	//---Register HTTP GET plugin
	if (soap_register_plugin_arg(&Soap, http_get, (void*)HttpGet_Callback)) 
		soap_print_fault(&Soap, stderr);

	if (soap_register_plugin(&Soap, http_da)) 
		soap_print_fault(&Soap, stderr);
	//---
#ifdef _JPR_AM_
    //Read from Database
    TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.URL");
    TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.Username");
    TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.Password");
    #ifdef WITH_TR098
	TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.ConnectionRequestUsername");
	TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.ConnectionRequestPassword");
    #elif WITH_TR106
	TR069_GetParameterValue2("Device.ManagementServer.ConnectionRequestUsername");
	TR069_GetParameterValue2("Device.ManagementServer.ConnectionRequestPassword");
    #endif
    TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.ProvisioningCode");
    TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.SerialNumber");
    TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.SoftwareVersion");
    TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.X_ModuleIMEI");
	#ifdef CONFIG_TELENOR_NORWAY
    TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.X_IMSI");
	#endif
	#ifdef UPGRADE_ONLY_LTE
    TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.X_UpgradeOnlyLTE");
	#endif
    TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.PeriodicInformEnable");
	DM_CALLBACK_PeriodicInformInterval(TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.PeriodicInformInterval"),NULL);
	if(strlen(DM_GetNode(ROOT, "InternetGatewayDevice.DeviceInfo.SerialNumber")->Value) == 0){
		char serialNum[64];
		char line[128];
		FILE * fp;		
		memset(serialNum,0,sizeof(serialNum));
		memset(line,0,sizeof(line));
		sprintf(line,"TEST=`cat /tmp/mac | grep SN | cut -d= -f2`;echo -n $TEST");
		fp = popen(line,"r");
		if(fp){
		    fgets(serialNum,sizeof(serialNum),fp);
			pclose(fp);
		}
		if(strlen(serialNum) == 0)
		    sprintf(serialNum,"00000000");
		DM_WriteDataToNode(DM_GetNode(ROOT,"InternetGatewayDevice.DeviceInfo.SerialNumber"),serialNum);
	}
	TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.ManufacturerOUI");
#endif

	if (InitCPEinfo(&Soap, ROOT) != _SUCCESS)
		LOG(LOG_ERROR, "Initial CPE info. fail!\n");

	master = soap_bind(&Soap, NULL, CONN_REQ_PORT, 1);
	if (!soap_valid_socket(master)) {
		soap_print_fault(&Soap, stderr);
		LOG(LOG_DEBUG, "SOAP socket is invalid.\n");
		return _ERROR;
	}
 	LOG(LOG_DEBUG, "SOAP socket is valid.\n");

	if (ENABLE_TR111 == TRUE)
	{
#ifdef _JPR_AM_
        char * stunEnable = TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.STUNEnable");
        if (strcmp(stunEnable, "1") == EQUALITY) {
            TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.STUNServerAddress");
            TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.STUNUsername");
            TR069_GetParameterValue2("InternetGatewayDevice.ManagementServer.STUNPassword");
		    DM_CALLBACK_STUNEnable("1","InternetGatewayDevice.ManagementServer.STUNEnable");
		}
#else
		TR069_SetParameterValue("InternetGatewayDevice.ManagementServer.STUNEnable", "true");
#endif
	}

	EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
	memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
	if (ENABLE_BOOTSTRAP == TRUE)
	{
#ifdef _JPR_AM_
		char * firstUseDate = TR069_GetParameterValue2("InternetGatewayDevice.DeviceInfo.FirstUseDate");
		if (strcmp(firstUseDate, "Null") == EQUALITY)
			isFirstTime = TRUE;
		else {
			isFirstTime = FALSE;
			LOG(LOG_INFO, "first use date: %s\n",firstUseDate);	
		}
		if(isFirstTime == TRUE)
#endif
		EVENT_FLAG[BOOTSTRAP] = TRUE;
	}

	EVENT_FLAG[BOOT] = TRUE;
	ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);

	//TR069_SetParameterValue("InternetGatewayDevice.CopperGate.DeviceInfo.State", "Requested");
	//TR069_SetParameterValue("InternetGatewayDevice.CopperGate.DynamicConf.State", "Execute");
#ifdef _JPR_AM_
    BOOTInformCheck = FALSE;
	BOOTInformOK = FALSE;
	InformOKLocked = FALSE;
	InformRetryCount = 0;
#else
	TR069_SetParameterValue("InternetGatewayDevice.ManagementServer.PeriodicInformEnable", "true");	
	TR069_SetParameterValue("InternetGatewayDevice.ManagementServer.PeriodicInformInterval", "100");
	StartUpTimeRoutine();
#endif
	LOG(LOG_INFO, "START SOAP SERVE\n");	
	while(1)
	{
		slave = soap_accept(&Soap);
		if (soap_valid_socket(slave)) 
		{
			if (soap_serve(&Soap) != _SUCCESS)
				soap_print_fault(&Soap, stderr);
		}
		soap_destroy(&Soap);
		soap_end(&Soap);
		
		if(bServeExit == TRUE)
		{
			LOG(LOG_DEBUG, "g_s_bSoapServeExit = %d\n", bServeExit);
			bServeExit = FALSE;
			break;
		}
	}
	soap_done(&Soap);
	LOG(LOG_INFO, "Soap_serve terminate\n");
	return 0;
}

