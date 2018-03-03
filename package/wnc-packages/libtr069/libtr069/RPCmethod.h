struct RPCMethodCB
{
	int (*GetRPCMethods)(struct soap *soap, struct ArrayOfstring *MethodList);
	int (*GetParameterNames)(struct soap *soap, char* ParameterPath, xsd__boolean NextLevel, struct ParameterList_Name *ParameterList);
	int (*GetParameterValues)(struct soap *soap, struct ArrayOfstring *ParameterNames, struct ParameterList_Value *ParameterList);
	int (*SetParameterValues)(struct ParameterList_Value *ParameterList, char* ParameterKey);
	int (*SetParameterAttributes)(struct ParameterList_Attributes *ParameterList);
	int (*GetParameterAttributes)(struct soap *soap, struct ArrayOfstring *ParameterNames, struct ParameterList_GetAttributes *ParameterList);
	int (*AddObject)(char* ObjectName, unsigned int* number, char* ParameterKey);
	int (*DeleteObject)(char* ObjectName, char* ParameterKey);
#ifdef _JNR_ES_
	int (*Download)(char *FileType, char* URL, char* UserName, char* PassWord, unsigned int FileSize, char* TargetFileName);
#else
	int (*Download)(char* URL, char* UserName, char* PassWord, unsigned int FileSize, char* TargetFileName);
#endif
#ifdef _JPR_AM_
	int (*Upload)(char *FileType, char* URL, char* UserName, char* PassWord);
#endif
	int (*FactoryReset)();
	int (*Reboot)();
};

void RPC_SetRPCRPCMethodCallback(struct RPCMethodCB* pRPCMethodCB);
