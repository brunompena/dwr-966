#include "stdsoap2.h"
#include "soapStub.h"
#include "tr069cpe.nsmap"
#include "RPCmethod.h"
#include "Common.h"
#include "platform.h"
#include "logger.h"

#define MAXCODESIZE 32

struct DownloadInfo
{
	char CommandKey[MAXSTRINGSIZE];
	char URL[MAXURLSIZE];
	char UserName[MAXSTRINGSIZE];
	char PassWord[MAXSTRINGSIZE];
	char TargetFileName[MAXSTRINGSIZE];
	unsigned int FileSize;
#ifdef _JNR_ES_
	char FileType[MAXSTRINGSIZE];
#endif
};

typedef struct 
{
	unsigned short int code;
	char *description;
	/*--------------------------------------------------------
		0:Client(represents the originator of the SOAP request)
		1:Server(represents the SOAP responder) 
	  --------------------------------------------------------*/
	unsigned char type;
} FaultInfo;

FaultInfo FAULTINFO[] = {{9000, "Method not supported", 1},
						 {9001, "Request denied", 1},
						 {9002, "Internal error", 1},
						 {9003, "Invalid arguments", 0},
						 {9004, "Resource exceeded", 1},
						 {9005, "Invalid parameter name", 0},
						 {9006, "Invalid parameter type", 0},
						 {9007, "Invalid parameter value", 0},
						 {9008, "Attempt to set a non-writable parameter", 0},
						 {9009, "Notification requested rejected", 1},
						 {9010, "Download failure", 1},
						 {9011, "Upload failure", 1},
						 {9012, "File transfer server authentication failure", 1},
						 {9013, "Unsupported protocol for file transfer", 1},
					  	 {0, NULL, 0}
};

struct RPCMethodCB* g_RPCMethodCB = NULL;
struct DownloadInfo* g_DownloadInfo = NULL;

void RPC_SetRPCRPCMethodCallback(struct RPCMethodCB* pRPCMethodCB)
{
	g_RPCMethodCB = pRPCMethodCB;
}

static void FillFaultStruct(struct soap *soap, struct cwmp__Fault *fault, int code)
{	
	unsigned int index = 0;
	char szErrorcode[MAXCODESIZE];

	sprintf(szErrorcode,"%d",code);
	fault = (struct cwmp__Fault *) soap_malloc (soap, sizeof(struct cwmp__Fault));
	fault->FaultCode = code;
	while (FAULTINFO[index].code != 0) {
		if (FAULTINFO[index].code == code) {
			fault->FaultString = soap_strdup(soap, FAULTINFO[index].description);
			if (FAULTINFO[index].type == 1)
				soap_receiver_fault_subcode(soap, szErrorcode, FAULTINFO[index].description, NULL);
			else
				soap_sender_fault_subcode(soap, szErrorcode, FAULTINFO[index].description, NULL);
		}
		++index;
	}

	if (soap->version == 2) 
	{
		soap->fault->SOAP_ENV__Detail = (struct SOAP_ENV__Detail*)soap_malloc(soap, sizeof(struct SOAP_ENV__Detail));
		soap->fault->SOAP_ENV__Detail->__any = NULL;
		soap->fault->SOAP_ENV__Detail->__type = SOAP_TYPE_cwmp__Fault;
		soap->fault->SOAP_ENV__Detail->fault = fault;
	} 
	else 
	{
		soap->fault->detail = (struct SOAP_ENV__Detail *) soap_malloc (soap, sizeof(struct SOAP_ENV__Detail));
		soap->fault->detail->__any = NULL;
		soap->fault->detail->__type = SOAP_TYPE_cwmp__Fault;
		soap->fault->detail->fault = fault;
	}
}


int cwmp__GetRPCMethods(struct soap *soap, void *_, struct ArrayOfstring *MethodList)
{
	unsigned int rc = SOAP_OK;
	struct cwmp__Fault *fault=NULL;
	if(!soap||!MethodList)
		return SOAP_ERR;

	LOG(LOG_INFO, "CWMP_GetRPCMethods..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->GetRPCMethods(soap, MethodList);
		if (rc != 0) 
		{
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP_GetRPCMethods..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP_GetRPCMethods..no impl\n");
	
	return SOAP_OK;
}

int cwmp__GetParameterNames(struct soap *soap, xsd__string ParameterPath, xsd__boolean NextLevel, struct ParameterList_Name *ParameterList)
{
	unsigned int rc = SOAP_OK;
	struct cwmp__Fault *fault=NULL;
	if(!soap||!ParameterPath||!ParameterList)
		return SOAP_ERR;

	LOG(LOG_INFO, "CWMP__GetParameterNames..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->GetParameterNames(soap, ParameterPath, NextLevel, ParameterList);
		if (rc != 0) 
		{
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__GetParameterNames..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__GetParameterNames..no impl\n");

	return SOAP_OK;
}

int cwmp__SetParameterValues(struct soap *soap, struct ParameterList_Value *ParameterList, xsd__string ParameterKey, xsd__unsignedInt *Status)
{
	unsigned int rc = 0;
	struct cwmp__Fault *fault=NULL;

	if(!soap||!ParameterList)
		return SOAP_ERR;

	LOG(LOG_INFO, "CWMP__SetParameterValues..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->SetParameterValues(ParameterList, ParameterKey);
		if( rc !=0 )
		{
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__SetParameterValues..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__SetParameterValues..no impl\n");

	return SOAP_OK;
}

int cwmp__GetParameterValues(struct soap *soap, struct ArrayOfstring *ParameterNames, struct ParameterList_Value *ParameterList)
{
	unsigned int rc = 0;
	struct cwmp__Fault *fault=NULL;

	if(!soap||!ParameterNames||!ParameterList)
		return SOAP_ERR;

	LOG(LOG_INFO, "CWMP__GetParameterValues..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->GetParameterValues(soap, ParameterNames, ParameterList);
		if (rc != 0) {
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__GetParameterValues..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__GetParameterValues..no impl\n");
	return SOAP_OK;
}

int cwmp__SetParameterAttributes(struct soap *soap, struct ParameterList_Attributes *ParameterList, struct cwmp__SetParameterAttributesResponse *out)
{
	unsigned int rc = 0;
	struct cwmp__Fault *fault=NULL;
	
	if(!soap||!ParameterList)
		return SOAP_ERR;

	LOG(LOG_INFO, "CWMP__SetParameterAttributes..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->SetParameterAttributes(ParameterList);
		if (rc != 0) {
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__SetParameterAttributes..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__SetParameterAttributes..no impl\n");

	return SOAP_OK;
}

int cwmp__GetParameterAttributes(struct soap *soap, struct ArrayOfstring *ParameterNames, struct ParameterList_GetAttributes *ParameterList)
{
	unsigned int rc = 0;
	struct cwmp__Fault *fault=NULL;

	if(!soap||!ParameterNames||!ParameterList)
		return SOAP_ERR;

	LOG(LOG_INFO, "CWMP__GetParameterAttributes..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->GetParameterAttributes(soap, ParameterNames, ParameterList);
		if (rc != 0) {
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__GetParameterAttributes..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__GetParameterAttributes..no impl\n");

	return SOAP_OK;
}

int cwmp__AddObject(struct soap *soap, xsd__string ObjectName, xsd__string ParameterKey,struct cwmp__AddObjectResponse *result)
{
	unsigned int rc = 0;
	unsigned int number = 0;
	struct cwmp__Fault *fault=NULL;
	LOG(LOG_INFO, "CWMP__AddObject....\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->AddObject(ObjectName, &number, ParameterKey);
		if( rc != 0 )
		{
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		result->InstanceNumber = number;
		result->Status = 0;
		LOG(LOG_INFO, "CWMP__AddObject..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__AddObject..no impl\n");

	return SOAP_OK;//alvin plus for test
}

int cwmp__DeleteObject(struct soap *soap, xsd__string ObjectName, xsd__string ParameterKey, xsd__int *Status)
{
	unsigned int rc = 0;
	struct cwmp__Fault *fault = NULL;
	LOG(LOG_INFO, "CWMP__DeleteObject..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->DeleteObject(ObjectName, ParameterKey);
		if (rc != 0) 
		{
			FillFaultStruct(soap, fault, rc);
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__DeleteObject..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__DeleteObject..no impl\n");
	
	return SOAP_OK;
}

void DownloadRoutine(int sig)
{
	time_t rawtime;
	struct tm *ptr_tm;
	char starttime[MAXTIMESIZE], completetime[MAXTIMESIZE];
	unsigned int rc = 0;
	unsigned int* EVENT_FLAG = NULL;
	LOG(LOG_INFO, "Start download routine..\n");
	time(&rawtime); ptr_tm = localtime(&rawtime);
	sprintf(starttime, "%d-%d-%dT%d:%d:%dZ", ptr_tm->tm_year+1900, ptr_tm->tm_mon+1, ptr_tm->tm_mday, ptr_tm->tm_hour, ptr_tm->tm_min, ptr_tm->tm_sec);

#ifdef _JNR_ES_
	rc = g_RPCMethodCB->Download(g_DownloadInfo->FileType, g_DownloadInfo->URL, g_DownloadInfo->UserName, g_DownloadInfo->PassWord, g_DownloadInfo->FileSize, g_DownloadInfo->TargetFileName);
#else
	rc = g_RPCMethodCB->Download(g_DownloadInfo->URL, g_DownloadInfo->UserName, g_DownloadInfo->PassWord, g_DownloadInfo->FileSize, g_DownloadInfo->TargetFileName);
#endif
	
	time(&rawtime); ptr_tm = localtime(&rawtime);
	sprintf(completetime, "%d-%d-%dT%d:%d:%dZ", ptr_tm->tm_year+1900, ptr_tm->tm_mon+1, ptr_tm->tm_mday, ptr_tm->tm_hour, ptr_tm->tm_min, ptr_tm->tm_sec);
	EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
	memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
	EVENT_FLAG[TRANSFER_COMPLETE] = TRUE;
	TransferCompleteRoutine(EVENT_FLAG, starttime, completetime, rc, g_DownloadInfo->CommandKey);
	free(g_DownloadInfo);
	LOG(LOG_INFO, "Start download routine..done\n");
}
#ifdef _JPR_AM_
static void* DownloadRoutine_delay(void *vParam){
    unsigned int* DelaySeconds = (unsigned int*)vParam;
	LOG(LOG_DEBUG, "DelaySeconds: %d\n",DelaySeconds);
	if(DelaySeconds > 0)
		PLATFORM_SleepSec(DelaySeconds);
	DownloadRoutine(0);
}

void ExecuteTransInform(unsigned int DelaySeconds){
	struct platformThread_t Platform;
	Platform.pThreadParm = (void*)DelaySeconds;
	Platform.pfFnStartRoutine = DownloadRoutine_delay;
	PLATFORM_ThreadCreate(&Platform);
}
#endif

int cwmp__Download(struct soap *soap, xsd__string CommandKey, xsd__string FileType, xsd__string URL, xsd__string UserName, xsd__string PassWord, xsd__unsignedInt FileSize, xsd__string TargetFileName, xsd__unsignedInt DelaySeconds, xsd__string SuccessURL, xsd__string FailureURL, struct cwmp__DownloadResponse *result)
{
	struct cwmp__Fault *fault = NULL;
	unsigned int rc = 0;
	if(!soap)
		return SOAP_ERR;
	LOG(LOG_INFO, "CWMP__DOWNLOAD..\n");

	if (g_RPCMethodCB != NULL)
	{
		g_DownloadInfo = (struct DownloadInfo*)malloc(sizeof(struct DownloadInfo));
		strncpy_pack(g_DownloadInfo->CommandKey, CommandKey, MAXSTRINGSIZE);
		strncpy_pack(g_DownloadInfo->URL, URL, MAXURLSIZE);
		if (UserName!=NULL && PassWord!=NULL)
		{
			strncpy_pack(g_DownloadInfo->UserName, UserName, MAXSTRINGSIZE);
			strncpy_pack(g_DownloadInfo->PassWord, PassWord, MAXSTRINGSIZE);
		}
#ifdef _JPR_AM_
		else {
			memset (g_DownloadInfo->UserName, 0x00, sizeof(MAXSTRINGSIZE));
			memset (g_DownloadInfo->PassWord, 0x00, sizeof(MAXSTRINGSIZE));
		}
#endif
		strncpy_pack(g_DownloadInfo->TargetFileName, TargetFileName, MAXSTRINGSIZE);
		g_DownloadInfo->FileSize = FileSize;
#ifdef _JNR_ES_
		strncpy_pack(g_DownloadInfo->FileType, FileType, MAXSTRINGSIZE);
#endif
#ifdef _JPR_AM_
		ExecuteTransInform(DelaySeconds);
#else
		PLATFORM_SetSignal(SIGALRM, DownloadRoutine);
		if (PLATFORM_CreateTimer(DelaySeconds, 0) != 0) 
			FillFaultStruct(soap, fault, 9002); //internal error

#ifdef _JNR_ES_
		rc = g_RPCMethodCB->Download(FileType, URL, UserName, PassWord, FileSize, TargetFileName);
		if (rc != 0) {
			return SOAP_FAULT;
		}
#endif
#endif
		LOG(LOG_INFO, "CWMP__DOWNLOAD..OK\n");
		//---for new session
		result->Status = 1;
		result->CompleteTime = "0001-01-01T00:00:00Z";
		result->StartTime = "0001-01-01T00:00:00Z";
		//---
	}
	else
		LOG(LOG_INFO, "CWMP__DOWNLOAD..no impl\n");
	return SOAP_OK;
}

int cwmp__Reboot(struct soap *soap, xsd__string CommandKey, struct RebootResponse *out)
{
	struct cwmp__Fault *fault = NULL;
	unsigned int rc = 0;
	if(!soap)
		return SOAP_ERR;

	LOG(LOG_INFO, "CWMP__Reboot..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->Reboot();
		if (rc != 0) {
			FillFaultStruct(soap, fault, rc);	
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__Reboot..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__Reboot..no impl\n");
	return SOAP_OK;
}

int cwmp__FactoryReset(struct soap *soap, void *_, struct FactoryResetResponse *out)
{
	unsigned int rc = 0;
	struct cwmp__Fault *fault = NULL;
	if(!soap)
		return SOAP_ERR;
	LOG(LOG_INFO, "CWMP__FactoryReset..\n");
	if (g_RPCMethodCB != NULL)
	{
		rc = g_RPCMethodCB->FactoryReset();
		if (rc != 0) {
			FillFaultStruct(soap, fault, rc);	
			return SOAP_FAULT;
		}
		LOG(LOG_INFO, "CWMP__FactoryReset..OK\n");
	}
	else
		LOG(LOG_INFO, "CWMP__FactoryReset..no impl\n");
	
	return SOAP_OK;
}

int cwmp__TransferComplete(struct soap *soap, xsd__string CommandKey, struct cwmp__FaultStruct *FaultStruct,xsd__string StartTime, xsd__string CompleteTime,struct cwmp__TransferCompleteResponse *out)
{
	return SOAP_NO_METHOD;
}

int cwmp__Inform(struct soap *soap, struct DeviceIdStruct *DeviceId, struct EventArray *Event, xsd__unsignedInt MaxEnvelopes, xsd__string CurrentTime, xsd__unsignedInt RetryCount, struct ParameterList_Value *ParameterList, struct cwmp__InformResponse *result)
{
	return SOAP_OK;
}

#ifdef _JPR_AM_
static void* UploadRoutine(void *vParam){
    unsigned int* DelaySeconds = (unsigned int*)vParam;
	time_t rawtime;
	struct tm *ptr_tm;
	char starttime[MAXTIMESIZE], completetime[MAXTIMESIZE];
	unsigned int rc = 0;
	unsigned int* EVENT_FLAG = NULL;
	
	LOG(LOG_DEBUG, "DelaySeconds: %d\n",DelaySeconds);
	if(DelaySeconds > 0)
		PLATFORM_SleepSec(DelaySeconds);

	LOG(LOG_INFO, "Start upload routine..\n");
	time(&rawtime); ptr_tm = localtime(&rawtime);
	sprintf(starttime, "%d-%d-%dT%d:%d:%dZ", ptr_tm->tm_year+1900, ptr_tm->tm_mon+1, ptr_tm->tm_mday, ptr_tm->tm_hour, ptr_tm->tm_min, ptr_tm->tm_sec);

	rc = g_RPCMethodCB->Upload(g_DownloadInfo->FileType, g_DownloadInfo->URL, g_DownloadInfo->UserName, g_DownloadInfo->PassWord);
	
	time(&rawtime); ptr_tm = localtime(&rawtime);
	sprintf(completetime, "%d-%d-%dT%d:%d:%dZ", ptr_tm->tm_year+1900, ptr_tm->tm_mon+1, ptr_tm->tm_mday, ptr_tm->tm_hour, ptr_tm->tm_min, ptr_tm->tm_sec);
	EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
	memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
	EVENT_FLAG[TRANSFER_COMPLETE] = TRUE;
	TransferCompleteRoutine(EVENT_FLAG, starttime, completetime, rc, g_DownloadInfo->CommandKey);
	if(g_DownloadInfo != NULL)
	    free(g_DownloadInfo);
	LOG(LOG_INFO, "Start upload routine..done\n");
}

int cwmp__Upload (struct soap *soap,
	      xsd__string CommandKey,
	      xsd__string FileType,
	      xsd__string URL,
	      xsd__string UserName,
	      xsd__string PassWord,
	      xsd__unsignedInt DelaySeconds,
	      cwmp__UploadResponse * response)
{
	struct cwmp__Fault *fault = NULL;
	unsigned int rc = 0;
	if(!soap)
		return SOAP_ERR;
	LOG(LOG_INFO, "CWMP__UPLOAD..\n");

	if (g_RPCMethodCB != NULL)
	{
		g_DownloadInfo = (struct DownloadInfo*)malloc(sizeof(struct DownloadInfo));
		strncpy_pack(g_DownloadInfo->CommandKey, CommandKey, MAXSTRINGSIZE);
		strncpy_pack(g_DownloadInfo->URL, URL, MAXURLSIZE);
		if (UserName!=NULL && PassWord!=NULL)
		{
			strncpy_pack(g_DownloadInfo->UserName, UserName, MAXSTRINGSIZE);
			strncpy_pack(g_DownloadInfo->PassWord, PassWord, MAXSTRINGSIZE);
		}
		else {
			memset (g_DownloadInfo->UserName, 0x00, sizeof(MAXSTRINGSIZE));
			memset (g_DownloadInfo->PassWord, 0x00, sizeof(MAXSTRINGSIZE));
		}
		strncpy_pack(g_DownloadInfo->FileType, FileType, MAXSTRINGSIZE);

	    struct platformThread_t Platform;
	    Platform.pThreadParm = (void*)DelaySeconds;
	    Platform.pfFnStartRoutine = UploadRoutine;
	    PLATFORM_ThreadCreate(&Platform);

		LOG(LOG_INFO, "CWMP__UPLOAD..OK\n");
		//---for new session
		response->Status = 1;
		response->CompleteTime = "0001-01-01T00:00:00Z";
		response->StartTime = "0001-01-01T00:00:00Z";
		//---
	}
	else
		LOG(LOG_INFO, "CWMP__UPLOAD..no impl\n");
	return SOAP_OK;
}
#endif
