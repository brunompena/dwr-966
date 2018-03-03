#ifndef __COMMON_H__
#define __COMMON_H__

#include <pthread.h>
typedef unsigned int BOOL;
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef int SOCKET;

#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define _SUCCESS 0
#define _ERROR -1
#define EQUALITY 0

#define MAXRECVSIZE 1024
#define MAXURLSIZE 128
#define MAXSTRINGSIZE 256
#define MAXLOGSIZE 256
#define MAXVALUESIZE 16
#define MAXTIMESIZE 64
#define MAXENVELOPES 1

#define DOWNLOAD_PATH "."

#define INFORM_TYPE_IMMEDIATE	1
#define INFORM_TYPE_PERIODIC	2

#define DM_GET 1
#define DM_SET 2
#define DM_SERVICE_NAME			"/tmp/service"

#define METHOD_NOT_SUPPORTED						9000
#define REQUEST_DENIED								9001
#define INTERNAL_ERROR								9002
#define INVALID_ARGUMENTS							9003
#define RESOURCE_EXCEEDED							9004
#define INVALID_PARAMETER_NAME						9005
#define INVALID_PARAMETER_TYPE						9006
#define INVALID_PARAMETER_VALUE						9007
#define ATTEMPT_TO_SET_A_NON_WRITABLE_PARAMETER		9008
#define NOTIFICATION_REQUESTED_REJECTED				9009
#define DOWNLOAD_FAILURE							9010
#define UPLOAD_FAILURE								9011
#define FILE_TRANSFER_SERVER_AUTHENTICATION_FAILURE	9012
#define UNSUPPORTED_PROTOCOL_FOR_FILE_TRANSFER		9013

enum DATA_TYPE
{
	XSD_OBJECT,	XSD_STRING,	XSD_INT, XSD_UNSIGNEDINT, XSD_BOOLEAN, XSD_DATETIME, XSD_BASE64,
};

enum NOTIFICATION_TYPE 
{
	NOTIFICATION_OFF, PASSIVE_NOTIFICATION,	ACTIVE_NOTIFICATION
};
#ifdef _JPR_AM_
#define MAXEVENT 9
#else
#define MAXEVENT 8
#endif
enum EVENT_TYPE
{
	BOOTSTRAP, BOOT, PERIODIC, VALUE_CHANGE, CONNECTION_REQUEST, TRANSFER_COMPLETE, M_REBOOT, M_DOWNLOAD
	#ifdef _JPR_AM_
	, DIAGNOSTICS_COMPLETE
	#endif
};

#include "tr069lib.h"

struct TreeNode
{
	char *Name;
	char *Value;
	enum DATA_TYPE DataType;
	unsigned int StringSize;
	enum NOTIFICATION_TYPE NotificationType;
	unsigned int Writable;
	unsigned int ValueChange;
	struct TreeNode* Parent;
	struct TreeNode* Sibling;
	struct TreeNode* Child;
	TR069_SETOptCallBackFunc SETOptCallBackFunc; 
	TR069_GETOptCallBackFunc GETOptCallBackFunc;
};

#ifdef _JNR_ES_
struct MethodCB
{
	TR069_DOWNLOADOptCallBackFunc DOWNLOADOptCallBackFunc;	//Required in TR069 admendent 1
	TR069_RESETOptCallBackFunc RESETOptCallBackFunc;	    //Optional in TR069 admendent 1
	TR069_REBOOTOptCallBackFunc REBOOTOptCallBackFunc;	    //Required in TR069 admendent 1
	#ifdef _JPR_AM_
	TR069_SETDoneCallBackFunc SETDoneCallBackFunc;
	TR069_SaveLogCallBackFunc SaveLogCallBackFunc;
	TR069_UPLOADOptCallBackFunc UPLOADOptCallBackFunc;
	TR069_SetupCallBackFunc PingCallBackFunc;
	TR069_SetupCallBackFunc TraceRouteCallBackFunc;
	TR069_SetupCallBackFunc UploadDiagnosticCallBackFunc;  //TR143
	TR069_SetupCallBackFunc DownloadDiagnosticCallBackFunc;  //TR143
	TR069_SetupCallBackFunc UDPEchoDiagnosticCallBackFunc;  //TR143
	#endif
};

extern struct MethodCB Method;

#endif

extern unsigned int ENABLE_TR111;
extern unsigned int ENABLE_SSL;
extern struct TreeNode*	ROOT;
extern struct TreeNode*	FUNCROOT;
extern pthread_mutex_t* Inform_mutex;
int isNumber(char* digit);
void strncpy_pack(char* dst, char* src, int dst_size);
void ExecuteInform(int type, unsigned int* EVENT_FLAG);
void TransferCompleteRoutine(unsigned int* EVENT_FLAG, char *starttime, char *completetime, unsigned int faultcode, char* CommandKey);

#endif //__COMMON_H__
