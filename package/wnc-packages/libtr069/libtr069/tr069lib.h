#ifndef __TR069LIB_H__
#define __TR069LIB_H__

typedef int (*TR069_SetupCallBackFunc)(void);
typedef int (*TR069_SETOptCallBackFunc)(char* value, const char* ObjectPath);
typedef char* (*TR069_GETOptCallBackFunc)(const char* Path);

typedef int (*TR069_DOWNLOADOptCallBackFunc)(char *FileType, char* URL, char* UserName, char* PassWord, unsigned int FileSize, char* TargetFileName);
typedef int (*TR069_RESETOptCallBackFunc)(void);
typedef int (*TR069_REBOOTOptCallBackFunc)(void);
/*
=======================================================================
TR069_Start()

Description:
	This method is used to start tr069 daemon

Prototype:
	int TR069_Start(TR069_SetupCallBackFunc);

Parameters:
	TR069_SetupCallBackFunc : [in] Pointer to the setup function

Return Value:
	0: if successful.
	-1: if unsuccessful.
=======================================================================
*/
int TR069_Start(TR069_SetupCallBackFunc SetupCallBackFunc);
/*
=======================================================================
TR069_RegisterSETOptFunc()

Description:
        This method is used to register a callback function while tr069 daemon receive SetParamterValues

Prototype:
        int TR069_RegisterSETOptFunc(char *, TR069_SETOptCallBackFunc);

Parameters:
	char* : [in] the path of node in data model
        TR069_SETOptCallBackFunc : [in] Pointer to the callback function

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069_RegisterSETOptFunc(char *path, TR069_SETOptCallBackFunc SETOptCallBackFunc);
/*
=======================================================================
TR069_RegisterGETOptFunc()

Description:
        This method is used to register a callback function while tr069 daemon receive GetParamterValues

Prototype:
        int TR069_RegisterGETOptFunc(char *, TR069_GETOptCallBackFunc);

Parameters:
	char* : [in] the path of node in data model
        TR069_SETOptCallBackFunc : [in] Pointer to the callback function

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069_RegisterGETOptFunc(char *path, TR069_GETOptCallBackFunc GETOptCallBackFunc);
/*
=======================================================================
TR069_SetParameterValue()

Description:
        This method is used to set a value to a node in data model

Prototype:
        int TR069_SetParameterValue(char*, char*);

Parameters:
        path : [in] the path of node in data model
	value : [in] the value setted to the node

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069_SetParameterValue(char* path, char* value);
/*
=======================================================================
TR069_GetParameterValue()

Description:
        This method is used to get a value from a node in data model

Prototype:
        int TR069_GetParameterValue(char* path, char* value, size_t iLen);

Parameters:
        path : [in] the path of node in data model
		value : [Out] the value of node in data model
		iLen : [in] the length of value

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069_GetParameterValue(char* path, char* value, size_t iLen);
/*
=======================================================================
TR069DB_SetParameterService()

Description:
        This method is used to set a value to a node in data model

Prototype:
        int TR069_SetParameterValue(char*, char*);

Parameters:
        path : [in] the path of node in data model
	value : [in] the value setted to the node

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069DB_SetParameterService(char* path, char* value);
/*
=======================================================================
TR069DB_GetParameterService()

Description:
        This method is used to get a value from a node in data model

Prototype:
        int TR069_GetParameterValue(char* path, char* value, size_t iLen);

Parameters:
        path : [in] the path of node in data model
		value : [Out] the value of node in data model
		iLen : [in] the length of value

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069DB_GetParameterService(char* path, char* value, size_t iLen);

/*
=======================================================================
TR069_RegisterDOWNLOADOptFunc()

Description:
        This method is used to register a callback function while tr069 daemon receive Download method

Prototype:
        int TR069_RegisterDOWNLOADOptFunc( TR069_DOWNLOADOptCallBackFunc );

Parameters:
        TR069_DOWNLOADOptCallBackFunc : [in] Pointer to the callback function

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069_RegisterDOWNLOADOptFunc( TR069_DOWNLOADOptCallBackFunc CallBackFunc );

/*
=======================================================================
TR069_RegisterRESETOptFunc()

Description:
        This method is used to register a callback function while tr069 daemon receive FactoryReset method

Prototype:
        int TR069_RegisterRESETOptFunc( TR069_RESETOptCallBackFunc );

Parameters:
        TR069_RegisterRESETOptFunc : [in] Pointer to the callback function

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069_RegisterRESETOptFunc( TR069_RESETOptCallBackFunc CallBackFunc );

/*
=======================================================================
TR069_RegisterREBOOTOptFunc()

Description:
        This method is used to register a callback function while tr069 daemon receive Reboot method

Prototype:
        int TR069_RegisterREBOOTOptFunc( TR069_REBOOTOptCallBackFunc );

Parameters:
	char* : [in] the path of node in data model
        TR069_RegisterREBOOTOptFunc : [in] Pointer to the callback function

Return Value:
        0: if successful.
        -1: if unsuccessful.
=======================================================================
*/
int TR069_RegisterREBOOTOptFunc( TR069_REBOOTOptCallBackFunc CallBackFunc);
//#ifdef _JPR_AM_
typedef int (*TR069_SETDoneCallBackFunc)(void);
typedef int (*TR069_SaveLogCallBackFunc)(unsigned int level, char* log);
typedef int (*TR069_UPLOADOptCallBackFunc)(char *FileType, char* URL, char* UserName, char* PassWord);
int TR069_RegisterSETDoneFunc( TR069_SETDoneCallBackFunc CallBackFunc );
int TR069_RegisterSaveLogFunc( TR069_SaveLogCallBackFunc CallBackFunc );
int TR069_RegisterUPLOADOptFunc( TR069_UPLOADOptCallBackFunc CallBackFunc );
int TR069_RegisterPingDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc );
int TR069_RegisterTraceRouteDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc );
int TR069_RegisterUploadDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc );
int TR069_RegisterDownloadDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc );

struct TreeNode* TR069_GetNode(char* pszName);
int DM_WriteDataToNode(struct TreeNode* Node, char* value);
//#endif
#endif
