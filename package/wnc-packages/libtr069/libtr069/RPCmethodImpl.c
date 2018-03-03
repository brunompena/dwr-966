#include "soapStub.h"
#include "Common.h"
#include "RPCmethod.h"
#include "RPCmethodImpl.h"
#include "dmutil.h"
#include "logger.h"

struct BackupData
{
	char* value;
};
#ifdef _JPR_AM_
void getParameter_pre(struct ArrayOfstring *ParameterNames){
    struct TreeNode* node = NULL;
	int getHost = FALSE;
	int getWLAN1 = FALSE;
	int getWLAN2 = FALSE;
	int getWLAN3 = FALSE;
	int getWLAN4 = FALSE;
	int getWANPortMapping = FALSE;
	int getDHCPStaticAddress = FALSE;
	int i=0;
	#ifdef WITH_TR098
	for (i = 0 ; i < (unsigned int)ParameterNames->__size ; i++) {
	    if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.") == 0){
			getHost = TRUE;
		    getWLAN1 = TRUE;
		    getWLAN2 = TRUE;
		    getWLAN3 = TRUE;
		    getWLAN4 = TRUE;
			getWANPortMapping = TRUE;
			getDHCPStaticAddress = TRUE;
			break;		
	    } else if(strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice") != NULL){
		    if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.") == 0
			  || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.") == 0){
			    getHost = TRUE;
		        getWLAN1 = TRUE;
		        getWLAN2 = TRUE;
		        getWLAN3 = TRUE;
		        getWLAN4 = TRUE;
				getDHCPStaticAddress = TRUE;
			} else{
		        if(strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.Hosts") != NULL){
					getHost = TRUE;
				}
				else if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.") == 0){
		            getWLAN1 = TRUE;
		            getWLAN2 = TRUE;
		            getWLAN3 = TRUE;
		            getWLAN4 = TRUE;
				} else if(strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.DHCPStaticAddress") != NULL){
		            getDHCPStaticAddress = TRUE;
				} else { 
				    if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.TotalAssociations") == 0
				      || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.") == 0
				      || strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.AssociatedDevice") != NULL){
		                getWLAN1 = TRUE;
				    }
		            else if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.2.TotalAssociations") == 0
				      || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.2.") == 0
				      || strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.2.AssociatedDevice") != NULL){
		                getWLAN2 = TRUE;
				    }
				#ifdef WITH_WIFI_11AC
		            else if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.3.TotalAssociations") == 0
				      || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.3.") == 0
				      || strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.3.AssociatedDevice") != NULL){
		                getWLAN3 = TRUE;
				    }
		            else if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.4.TotalAssociations") == 0
				      || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.4.") == 0
				      || strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.LANDevice.1.WLANConfiguration.4.AssociatedDevice") != NULL){
		                getWLAN4 = TRUE;
				    }
				#endif
				}
			}	
		} else if(strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice") != NULL){
		    if(strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.") == 0
			  || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.1.") == 0
			  || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.") == 0
			  || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.") == 0
			  || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.") == 0
			  || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.") == 0
			  || strcmp(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMappingNumberOfEntries") == 0){
		        getWANPortMapping = TRUE;
			} else if(strstr(ParameterNames->__ptrstring[i],"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping") != NULL){
			    getWANPortMapping = TRUE;
			}	
		}
	}
	
	if(getHost == TRUE){
        node = DM_GetNode(ROOT, "InternetGatewayDevice.LANDevice.1.Hosts.HostNumberOfEntries");
	    if (node!=NULL && node->GETOptCallBackFunc != NULL){
		    char * result = node->GETOptCallBackFunc("InternetGatewayDevice.LANDevice.1.Hosts.HostNumberOfEntries");
		    if (result != NULL)
		        DM_WriteDataToNode(node, result);
		}
	}
	if(getWLAN1 == TRUE){
        node = DM_GetNode(ROOT, "InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.TotalAssociations");
	    if (node!=NULL && node->GETOptCallBackFunc != NULL){
		    char * result = node->GETOptCallBackFunc("InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.TotalAssociations");
		    if (result != NULL)
		        DM_WriteDataToNode(node, result);
		}
	}
	if(getWLAN2 == TRUE){
        node = DM_GetNode(ROOT, "InternetGatewayDevice.LANDevice.1.WLANConfiguration.2.TotalAssociations");
	    if (node!=NULL && node->GETOptCallBackFunc != NULL){
		    char * result = node->GETOptCallBackFunc("InternetGatewayDevice.LANDevice.1.WLANConfiguration.2.TotalAssociations");
		    if (result != NULL)
		        DM_WriteDataToNode(node, result);
		}
	}
	#ifdef WITH_WIFI_11AC
	if(getWLAN3 == TRUE){
        node = DM_GetNode(ROOT, "InternetGatewayDevice.LANDevice.1.WLANConfiguration.3.TotalAssociations");
	    if (node!=NULL && node->GETOptCallBackFunc != NULL){
		    char * result = node->GETOptCallBackFunc("InternetGatewayDevice.LANDevice.1.WLANConfiguration.3.TotalAssociations");
		    if (result != NULL)
		        DM_WriteDataToNode(node, result);
		}
	}
	if(getWLAN4 == TRUE){
        node = DM_GetNode(ROOT, "InternetGatewayDevice.LANDevice.1.WLANConfiguration.4.TotalAssociations");
	    if (node!=NULL && node->GETOptCallBackFunc != NULL){
		    char * result = node->GETOptCallBackFunc("InternetGatewayDevice.LANDevice.1.WLANConfiguration.4.TotalAssociations");
		    if (result != NULL)
		        DM_WriteDataToNode(node, result);
		}
	}
	#endif
	if(getWANPortMapping == TRUE){
        node = DM_GetNode(ROOT, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMappingNumberOfEntries");
	    if (node!=NULL && node->GETOptCallBackFunc != NULL){
		    char * result = node->GETOptCallBackFunc("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMappingNumberOfEntries");
		    if (result != NULL)
		        DM_WriteDataToNode(node, result);
		}
	}
	if(getDHCPStaticAddress == TRUE){
        node = DM_GetNode(ROOT, "InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.DHCPStaticAddressNumberOfEntries");
	    if (node!=NULL && node->GETOptCallBackFunc != NULL){
		    char * result = node->GETOptCallBackFunc("InternetGatewayDevice.LANDevice.1.LANHostConfigManagement.DHCPStaticAddressNumberOfEntries");
		    if (result != NULL)
		        DM_WriteDataToNode(node, result);
		}
	}
	#endif
}
#endif
static void FillParameterNameList(struct soap *soap, struct TreeNode* node, struct ParameterList_Name *ParameterNameList, unsigned int* index)
{
	char szName[MAXSTRINGSIZE];
	if (node == NULL)
		return;
	else
	{
		memset(szName, 0, sizeof(szName));
		DM_GetTotalName(node, szName);
		ParameterNameList->__ptrParameterInfoStruct[*index].Writable = node->Writable;
		ParameterNameList->__ptrParameterInfoStruct[*index].Name	 = (char*) soap_malloc(soap, strlen(szName) + 1);
		strncpy_pack(ParameterNameList->__ptrParameterInfoStruct[*index].Name, szName, strlen(szName) + 1);
		(*index)++;
		FillParameterNameList(soap, node->Child, ParameterNameList, index);
		FillParameterNameList(soap, node->Sibling, ParameterNameList, index);
	}
}

static void FillParameterValueList(struct soap *soap, struct TreeNode* node, struct ParameterList_Value *ParameterValueList, unsigned int* index)
{
	char szName[MAXSTRINGSIZE];
	if (node == NULL)
		return;
	else
	{
		memset(szName, 0, sizeof(szName));
		DM_GetTotalName(node, szName);
	#ifdef _JPR_AM_
		if (node->GETOptCallBackFunc != NULL)
		{
			//LOG(LOG_DEBUG, "get %s\n", szName);
			char *result = NULL;
			result = node->GETOptCallBackFunc(szName);
			if (result == NULL || DM_WriteDataToNode(node, result) == _ERROR)
			{
				LOG(LOG_DEBUG, "failed to get %s\n", szName);
			}
		}
	#endif
		ParameterValueList->__ptrParameterValueStruct[*index].Name	 = (char*) soap_malloc(soap, strlen(szName) + 1);
		ParameterValueList->__ptrParameterValueStruct[*index].Value	 = (char*) soap_malloc(soap, node->StringSize);
		strncpy_pack(ParameterValueList->__ptrParameterValueStruct[*index].Name, szName, strlen(szName) + 1);
		strncpy_pack(ParameterValueList->__ptrParameterValueStruct[*index].Value, node->Value, node->StringSize);
#ifdef _JPR_AM_
		LOG(LOG_DEBUG, "Name : \"%s\"\n", ParameterValueList->__ptrParameterValueStruct[*index].Name);
		LOG(LOG_DEBUG, "Value: \"%s\"\n", ParameterValueList->__ptrParameterValueStruct[*index].Value);
		if((*index) >= ParameterValueList->__size -1)
		    return;
#endif
		(*index)++;
		FillParameterValueList(soap, node->Child, ParameterValueList, index);
		FillParameterValueList(soap, node->Sibling, ParameterValueList, index);
	}
}

static void FillParameterAttrList(struct soap *soap, struct TreeNode* node, struct ParameterList_GetAttributes *ParameterAttrList, unsigned int* index)
{
	char szName[MAXSTRINGSIZE];
	if (node == NULL)
		return;
	else
	{
		memset(szName, 0, sizeof(szName));
		DM_GetTotalName(node, szName);
		//ParameterAttrList->__ptrParameterAttributeStruct[*index].AccessList.__size		= 0;
		//ParameterAttrList->__ptrParameterAttributeStruct[*index].AccessList.__ptrstring	= NULL;
		ParameterAttrList->__ptrParameterAttributeStruct[*index].AccessList.__size			= 1;
		ParameterAttrList->__ptrParameterAttributeStruct[*index].AccessList.__ptrstring			= (char**)soap_malloc(soap, 1);
		ParameterAttrList->__ptrParameterAttributeStruct[*index].AccessList.__ptrstring[0]		= (char*) soap_malloc(soap, 11);
		strncpy_pack(ParameterAttrList->__ptrParameterAttributeStruct[*index].AccessList.__ptrstring[0], "Subscriber", 11);		
		ParameterAttrList->__ptrParameterAttributeStruct[*index].Notification				= node->NotificationType;
		ParameterAttrList->__ptrParameterAttributeStruct[*index].Name					= (char*) soap_malloc(soap, strlen(szName) + 1);
		strncpy_pack(ParameterAttrList->__ptrParameterAttributeStruct[*index].Name, szName, strlen(szName) + 1);
		(*index)++;
		FillParameterAttrList(soap, node->Child, ParameterAttrList, index);
		FillParameterAttrList(soap, node->Sibling, ParameterAttrList, index);
	}
}

static void SetSubParameterAttr(struct TreeNode* node, enum NOTIFICATION_TYPE type)
{
	if (node == NULL)
		return;
	else
	{
		SetSubParameterAttr(node->Child, type);
		SetSubParameterAttr(node->Sibling, type);
		node->NotificationType = type;
	}
}

static void DeleteNode(struct TreeNode* node)
{
	struct TreeNode* Tmp = NULL;
	if (node == NULL)
		return;
	else
	{
		//        0
		//      ��
		//     0-->0-->0-->0
		//   ��
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

int GetRPCMethod_Impl(struct soap *soap, struct ArrayOfstring *MethodList)
{
	LOG(LOG_DEBUG, "Execute GetRPCMethod Callback function!!\n");
#ifdef _JPR_AM_
	MethodList->__size = 12;
#else
	MethodList->__size = 11;
#endif
	MethodList->__ptrstring = (xsd__string *)soap_malloc(soap, sizeof(xsd__string)*MethodList->__size);
	MethodList->__ptrstring[0]	= "GetRPCMethods";
	MethodList->__ptrstring[1]	= "GetParameterNames";
	MethodList->__ptrstring[2]	= "GetParameterValues";
	MethodList->__ptrstring[3]	= "GetParameterAttributes";
	MethodList->__ptrstring[4]	= "SetParameterValues";
	MethodList->__ptrstring[5]	= "SetParameterAttributes";
	MethodList->__ptrstring[6]	= "Reboot";
	MethodList->__ptrstring[7]	= "Download";	
	MethodList->__ptrstring[8]	= "FactoryReset";	
	MethodList->__ptrstring[9]	= "AddObject";
	MethodList->__ptrstring[10]	= "DeleteObject";
#ifdef _JPR_AM_
	MethodList->__ptrstring[11]	= "Upload";
#endif
	return _SUCCESS;
}

int GetParameterNames_Impl(struct soap *soap, char* ParameterPath, xsd__boolean NextLevel, struct ParameterList_Name *ParameterNameList)
{
	unsigned int rc = _SUCCESS, index = 0;
	char *pch = NULL;
	char **allparameters = NULL;
	char szName[MAXSTRINGSIZE];
	struct TreeNode* TargetNode = NULL;
	struct TreeNode* Tmp = NULL;
	
	LOG(LOG_DEBUG, "Execute GetParameterNames Callback function!!\n");

	TargetNode = DM_GetNode(ROOT, ParameterPath);
	if (TargetNode == NULL)
	{
		rc = INVALID_PARAMETER_NAME;
		goto FUNC_EXIT;
	}
	memset(szName, 0, sizeof(szName));
	DM_GetTotalName(TargetNode, szName);
	if (strlen(ParameterPath) == 0 && NextLevel == TRUE)//for fit empty path and nextlevel true must return root node
	{
		ParameterNameList->__size									= 1;
		ParameterNameList->__ptrParameterInfoStruct					= (struct ParameterInfoStruct*)soap_malloc(soap, sizeof(struct ParameterInfoStruct) * ParameterNameList->__size);
		ParameterNameList->__ptrParameterInfoStruct[index].Writable = FALSE;
		ParameterNameList->__ptrParameterInfoStruct[index].Name		= (char*) soap_malloc(soap, strlen(szName) + 1);
		strncpy_pack(ParameterNameList->__ptrParameterInfoStruct[index].Name, szName, strlen(szName) + 1);
		return rc;
	}

	if (TargetNode->DataType == XSD_OBJECT)
	{
		ParameterNameList->__size									= DM_GetElementNum(TargetNode, NextLevel)-1;//the getparametername need only the sub node except parent node, so sub the parent node
		ParameterNameList->__ptrParameterInfoStruct					= (struct ParameterInfoStruct*)soap_malloc(soap, sizeof(struct ParameterInfoStruct) * ParameterNameList->__size);
		if (TargetNode->Child != NULL)
		{
			Tmp = TargetNode->Child;
			if (NextLevel == TRUE)
			{
				while(Tmp != NULL)
				{
					memset(szName, 0, sizeof(szName));
					DM_GetTotalName(Tmp, szName);
					ParameterNameList->__ptrParameterInfoStruct[index].Writable = Tmp->Writable;
					ParameterNameList->__ptrParameterInfoStruct[index].Name		= (char*) soap_malloc(soap, strlen(szName) + 1);
					strncpy_pack(ParameterNameList->__ptrParameterInfoStruct[index].Name, szName, strlen(szName) + 1);
					Tmp = Tmp->Sibling;
					index++;
				}
			}
			else
				FillParameterNameList(soap, Tmp, ParameterNameList, &index);
		}
	}
	else
	{
		ParameterNameList->__size									= 1;
		ParameterNameList->__ptrParameterInfoStruct					= (struct ParameterInfoStruct*)soap_malloc(soap, sizeof(struct ParameterInfoStruct) * ParameterNameList->__size);
		ParameterNameList->__ptrParameterInfoStruct[index].Writable = TargetNode->Writable;
		ParameterNameList->__ptrParameterInfoStruct[index].Name		= (char*) soap_malloc(soap, strlen(szName) + 1);
		strncpy_pack(ParameterNameList->__ptrParameterInfoStruct[index].Name, szName, strlen(szName) + 1);
	}

FUNC_EXIT:
	return rc;
}


int GetParameterValues_Impl(struct soap *soap, struct ArrayOfstring *ParameterNames, struct ParameterList_Value *ParameterValueList)
{
	unsigned int rc = _SUCCESS, i, index = 0, ElementNum = 0;
	char *result = NULL;
	char *pch = NULL;
	char **allparameters = NULL;
	char szName[MAXSTRINGSIZE];
	char szObjectPath[MAXSTRINGSIZE];
	struct TreeNode* TargetNode = NULL;
	struct TreeNode* Tmp = NULL;
	struct TreeNode* ObjectNode = NULL;
	
	LOG(LOG_DEBUG, "Execute GetParameterValues Callback function, size=%d\n", (unsigned int)ParameterNames->__size);
#ifdef _JPR_AM_
	getParameter_pre(ParameterNames);
#endif
	// Calculate the number of name-value pairs
	for (i = 0 ; i < (unsigned int)ParameterNames->__size ; i++) {
		Tmp = NULL;
		Tmp = DM_GetNode(ROOT, ParameterNames->__ptrstring[i]);
		if (Tmp == NULL)
		{
#ifdef _JPR_AM_
            LOG(LOG_DEBUG, "Invalid parameter: %s\n",ParameterNames->__ptrstring[i]);
#endif
			rc = INVALID_PARAMETER_NAME;
			goto FUNC_EXIT;
		}
		ElementNum += DM_GetElementNum(Tmp, FALSE);
	}

	ParameterValueList->__size						= ElementNum;
	ParameterValueList->__ptrParameterValueStruct	= (struct ParameterValueStruct*)soap_malloc(soap, sizeof(struct ParameterValueStruct) * ParameterValueList->__size);
	for (i = 0 ; i < (unsigned int)ParameterNames->__size ; i++) {
		TargetNode = DM_GetNode(ROOT, ParameterNames->__ptrstring[i]);
		if (TargetNode->GETOptCallBackFunc != NULL)
		{
			ObjectNode = DM_GetObjectNode(TargetNode);
			DM_GetTotalName(ObjectNode, szObjectPath);
			//LOG(LOG_DEBUG, "Execute getCB %s\n", szObjectPath);
#ifdef _JPR_AM_
            result = TargetNode->GETOptCallBackFunc(ParameterNames->__ptrstring[i]);
            if(result == NULL || (TargetNode->DataType != XSD_OBJECT && DM_WriteDataToNode(TargetNode, result) == _ERROR))
			{
				rc = INTERNAL_ERROR;
				goto FUNC_EXIT;
			}
#else
#ifdef _JNR_ES_
			result = TargetNode->GETOptCallBackFunc(ParameterNames->__ptrstring[i]);
#else
			result = TargetNode->GETOptCallBackFunc(szObjectPath);
#endif
			if (result == NULL || DM_WriteDataToNode(TargetNode, result) == _ERROR)
			{
				rc = INTERNAL_ERROR;
				goto FUNC_EXIT;
			}
#endif //_JPR_AM_
		}

		memset(szName, 0, sizeof(szName));
		DM_GetTotalName(TargetNode, szName);
		ParameterValueList->__ptrParameterValueStruct[index].Name	= (char*) soap_malloc(soap, strlen(szName) + 1);
		ParameterValueList->__ptrParameterValueStruct[index].Value	= (char*) soap_malloc(soap, TargetNode->StringSize);
		strncpy_pack(ParameterValueList->__ptrParameterValueStruct[index].Name, szName, strlen(szName) + 1);
		strncpy_pack(ParameterValueList->__ptrParameterValueStruct[index].Value, TargetNode->Value, TargetNode->StringSize);
		LOG(LOG_DEBUG, "Name : \"%s\"\n", ParameterValueList->__ptrParameterValueStruct[index].Name);
		LOG(LOG_DEBUG, "Value: \"%s\"\n", ParameterValueList->__ptrParameterValueStruct[index].Value);
		index++;
		if (TargetNode->Child != NULL)
		{
			Tmp = TargetNode->Child;
			FillParameterValueList(soap, Tmp, ParameterValueList, &index);
		}
	}
#ifdef _JNR_ES_
	return rc;
#endif

FUNC_EXIT:
	return rc;
}

int GetParameterAttributes_Impl(struct soap *soap, struct ArrayOfstring *ParameterNames, struct ParameterList_GetAttributes *ParameterAttrList)
{
	unsigned int rc = _SUCCESS, i, index = 0, ElementNum = 0;
	char *pch = NULL;
	char **allparameters = NULL;
	char szName[MAXSTRINGSIZE];
	struct TreeNode* TargetNode = NULL;
	struct TreeNode* Tmp = NULL;

	LOG(LOG_DEBUG, "Execute GetParameterAttributes Callback function!!\n");
	// Calculate the number of name-value pairs
	for (i = 0 ; i < (unsigned int)ParameterNames->__size ; i++) {
		Tmp = NULL;
		Tmp = DM_GetNode(ROOT, ParameterNames->__ptrstring[i]);
		if (Tmp == NULL)
		{
			rc = INVALID_PARAMETER_NAME;
			goto FUNC_EXIT;
		}
		ElementNum += DM_GetElementNum(Tmp, FALSE);
	}

	ParameterAttrList->__size						 = ElementNum;
	ParameterAttrList->__ptrParameterAttributeStruct = (struct ParameterAttributeStruct*)soap_malloc(soap, sizeof(struct ParameterAttributeStruct) * ParameterAttrList->__size);
	for (i = 0 ; i < (unsigned int)ParameterNames->__size ; i++) {
		TargetNode = DM_GetNode(ROOT, ParameterNames->__ptrstring[i]);
		memset(szName, 0, sizeof(szName));
		DM_GetTotalName(TargetNode, szName);
		//ParameterAttrList->__ptrParameterAttributeStruct[index].AccessList.__size			= 0;
		//ParameterAttrList->__ptrParameterAttributeStruct[index].AccessList.__ptrstring		= NULL;
		ParameterAttrList->__ptrParameterAttributeStruct[index].AccessList.__size			= 1;
		ParameterAttrList->__ptrParameterAttributeStruct[index].AccessList.__ptrstring			= (char**)soap_malloc(soap, 1);
		ParameterAttrList->__ptrParameterAttributeStruct[index].AccessList.__ptrstring[0]		= (char*) soap_malloc(soap, 11);
		strncpy_pack(ParameterAttrList->__ptrParameterAttributeStruct[index].AccessList.__ptrstring[0], "Subscriber", 11);
		ParameterAttrList->__ptrParameterAttributeStruct[index].Notification				= TargetNode->NotificationType;
		ParameterAttrList->__ptrParameterAttributeStruct[index].Name					= (char*) soap_malloc(soap, strlen(szName) + 1);
		strncpy_pack(ParameterAttrList->__ptrParameterAttributeStruct[index].Name, szName, strlen(szName) + 1);
		index++;
		if (TargetNode->Child != NULL)
		{
			Tmp = TargetNode->Child;
			FillParameterAttrList(soap, Tmp, ParameterAttrList, &index);
		}
	}

FUNC_EXIT:
	return rc;
}

int SetParameterValues_Impl(struct ParameterList_Value *ParameterValueList, char* ParameterKey)
{
	unsigned int rc = _SUCCESS, i, j;
	unsigned int* EVENT_FLAG = NULL;
	BOOL bInformImmediate = FALSE;
	char szObjectPath[MAXSTRINGSIZE];
	struct TreeNode* TargetNode = NULL;
	struct TreeNode* ObjectNode = NULL;
	struct BackupData* backup = NULL;
 	LOG(LOG_DEBUG, "Execute SetParameterValues Callback function!!\n");

	//---check parameter valid
	for (i = 0 ; i < (unsigned int)ParameterValueList->__size ; i++) 
	{
		TargetNode = NULL;
		TargetNode = DM_GetNode(ROOT, ParameterValueList->__ptrParameterValueStruct[i].Name);
		if (TargetNode != NULL)
		{
			if (TargetNode->Writable != TRUE)
			{
#ifdef _JPR_AM_
            LOG(LOG_DEBUG, "Read only parameter: %s\n",ParameterValueList->__ptrParameterValueStruct[i].Name);
#endif
				//rc = ATTEMPT_TO_SET_A_NON_WRITABLE_PARAMETER;
				rc = INVALID_ARGUMENTS;
				goto FUNC_EXIT;
			}
		}
		else
		{
#ifdef _JPR_AM_
            LOG(LOG_DEBUG, "Invalid parameter: %s\n",ParameterValueList->__ptrParameterValueStruct[i].Name);
#endif
			//rc = INVALID_PARAMETER_NAME;
			rc = INVALID_ARGUMENTS;
			goto FUNC_EXIT;
		}
		//---check if the same parameter
		for (j = i+1 ; j < (unsigned int)ParameterValueList->__size ; j++) 
		{
			if (strcmp(ParameterValueList->__ptrParameterValueStruct[i].Name, ParameterValueList->__ptrParameterValueStruct[j].Name) == EQUALITY)
			{
				rc = INVALID_ARGUMENTS;
				goto FUNC_EXIT;
			}
		}
	}
	//---

	//---create backup array
	backup = (struct BackupData*)malloc(sizeof(struct BackupData)*ParameterValueList->__size);
	//---set parameter value
	for (i = 0 ; i < (unsigned int)ParameterValueList->__size ; i++) 
	{
		TargetNode = DM_GetNode(ROOT, ParameterValueList->__ptrParameterValueStruct[i].Name);
		//---set backup data
		backup[i].value = (char*)malloc(TargetNode->StringSize);
		strncpy_pack(backup[i].value, TargetNode->Value, TargetNode->StringSize);
		//---
		DM_WriteDataToNode(TargetNode, ParameterValueList->__ptrParameterValueStruct[i].Value);
		LOG(LOG_DEBUG, "Name : \"%s\"\n", ParameterValueList->__ptrParameterValueStruct[i].Name);
		LOG(LOG_DEBUG, "Value: \"%s\"\n", ParameterValueList->__ptrParameterValueStruct[i].Value);
	}

	//---execute plugin function
	for (i = 0 ; i < (unsigned int)ParameterValueList->__size ; i++) 
	{
		TargetNode = DM_GetNode(ROOT, ParameterValueList->__ptrParameterValueStruct[i].Name);
		if (TargetNode->SETOptCallBackFunc != NULL)//---have plugfunc
		{
			ObjectNode = DM_GetObjectNode(TargetNode);
			DM_GetTotalName(ObjectNode, szObjectPath);
#ifdef _JNR_ES_
			if (TargetNode->SETOptCallBackFunc(ParameterValueList->__ptrParameterValueStruct[i].Value, ParameterValueList->__ptrParameterValueStruct[i].Name) == _ERROR)
#else
			if (TargetNode->SETOptCallBackFunc(ParameterValueList->__ptrParameterValueStruct[i].Value, szObjectPath) == _ERROR)
#endif
			{
				rc = INTERNAL_ERROR;
				//---rollback data
				for (j = 0 ; j < (unsigned int)ParameterValueList->__size ; j++) 
				{
					TargetNode = DM_GetNode(ROOT, ParameterValueList->__ptrParameterValueStruct[j].Name);
					DM_WriteDataToNode(TargetNode, backup[j].value);
				}
				//---
				goto FUNC_EXIT;
			}
		}
	}

	//---set value change flag
	for (i = 0 ; i < (unsigned int)ParameterValueList->__size ; i++) 
	{
		TargetNode = DM_GetNode(ROOT, ParameterValueList->__ptrParameterValueStruct[i].Name);
#ifdef _JPR_AM_
        if(strcmp(TargetNode->Value,ParameterValueList->__ptrParameterValueStruct[i].Value) != EQUALITY){
          TargetNode->ValueChange = TRUE;
          if (TargetNode->NotificationType == ACTIVE_NOTIFICATION)
	        bInformImmediate = TRUE;
        }
#else
		TargetNode->ValueChange = TRUE;
		if (TargetNode->NotificationType == ACTIVE_NOTIFICATION)
			bInformImmediate = TRUE;
#endif
	}
	//---
#ifdef _JPR_AM_
		if (Method.SETDoneCallBackFunc != NULL)//---have plugfunc
		{
			Method.SETDoneCallBackFunc();
		}
#endif

#ifdef WITH_TR098
	TargetNode = DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ParameterKey");
#elif WITH_TR104
	TargetNode = DM_GetNode(ROOT, "Device.ManagementServer.ParameterKey");
#endif
	DM_WriteDataToNode(TargetNode, ParameterKey);
#ifndef _JPR_AM_
	if (bInformImmediate == TRUE)
	{
		EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
		memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
		ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
	}
#endif
FUNC_EXIT:
	//---clear backup
	if (backup != NULL)
	{
		for (j = 0 ; j < (unsigned int)ParameterValueList->__size ; j++) 
			free(backup[j].value);
		free(backup);
	}
	//---
	return rc;
}

int SetParameterAttributes_Impl(struct ParameterList_Attributes *ParameterAttrList)
{
	unsigned int rc = _SUCCESS, i;
	struct TreeNode* TargetNode = NULL;
	LOG(LOG_DEBUG, "Execute SetParameterAttributes Callback function!!\n");

	//---check parameter valid
	for (i = 0 ; i < (unsigned int)ParameterAttrList->__size ; i++) {
		if (ParameterAttrList->__ptrSetParameterAttributesStruct[i].NotificationChange == TRUE) 
		{
			TargetNode = NULL;
			TargetNode = DM_GetNode(ROOT, ParameterAttrList->__ptrSetParameterAttributesStruct[i].Name);
			if (TargetNode == NULL)
			{
				rc = INVALID_PARAMETER_NAME;
				goto FUNC_EXIT;
			}
		}
		else
		{
			rc = NOTIFICATION_REQUESTED_REJECTED;
			goto FUNC_EXIT;
		}
	}

	//---set parameter Attributes
	for (i = 0 ; i < (unsigned int)ParameterAttrList->__size ; i++) {
		TargetNode = DM_GetNode(ROOT, ParameterAttrList->__ptrSetParameterAttributesStruct[i].Name);
		TargetNode->NotificationType = ParameterAttrList->__ptrSetParameterAttributesStruct[i].Notification;
		if (TargetNode->Child != NULL)
			SetSubParameterAttr(TargetNode->Child, TargetNode->NotificationType);
	}

FUNC_EXIT:
	return rc;
}

int AddObject_Impl(char* ObjectName, unsigned int* number, char* ParameterKey)
{
	unsigned int rc = _SUCCESS, InstanceNum = 1;
	struct TreeNode* TargetNode = NULL;
	struct TreeNode* Instance = NULL;
	struct TreeNode* NewInstance = NULL;
	struct TreeNode* Tmp = NULL;
	LOG(LOG_DEBUG, "Execute AddObject Callback function!!\n");

	Instance = DM_GetInstanceFromText("InstanceList.dat", ObjectName);//got an instance from InstanceList.dat
	if (Instance == NULL)
	{
		rc = INVALID_PARAMETER_NAME;
		goto FUNC_EXIT;
	}

	TargetNode = DM_GetNode(ROOT, ObjectName);//check objectname if in data tree
	if (TargetNode == NULL)
	{
		rc = INVALID_PARAMETER_NAME;
		goto FUNC_EXIT;
	}

	//---calculate number of instance   //no use
	/*Tmp = TargetNode->Child;
	while(Tmp != NULL)
	{
		if (isNumber(Tmp->Name) == TRUE && InstanceNum == atoi(Tmp->Name))
		{
			Tmp = TargetNode->Child;
			InstanceNum ++;
			continue;
		}
		Tmp = Tmp->Sibling;
	}*/
	
	//---decide the number of instance 
	while(1)
	{
		Tmp = TargetNode->Child;
		while(Tmp != NULL)
		{
			if (isNumber(Tmp->Name) == TRUE && InstanceNum == atoi(Tmp->Name))
				break;
			Tmp = Tmp->Sibling;
		}
		if (Tmp == NULL)
			break;
		InstanceNum ++;
	}
	sprintf(Instance->Name, "%d", InstanceNum);
	//---

	if (TargetNode->Child != NULL)
	{
		Tmp = TargetNode->Child;
		while (Tmp->Sibling != NULL)
			Tmp = Tmp->Sibling;
		Tmp->Sibling = Instance;
	}
	else
		TargetNode->Child = Instance;
	Instance->Parent = TargetNode;
	*number = InstanceNum;

	//---Setup plugin function
	DM_SetupTreePluginFunc(Instance);
#ifdef WITH_TR098
	TargetNode = DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ParameterKey");
#elif WITH_TR104
	TargetNode = DM_GetNode(ROOT, "Device.ManagementServer.ParameterKey");
#endif
	DM_WriteDataToNode(TargetNode, ParameterKey);

FUNC_EXIT:
	return rc;
}

int DeleteObject_Impl(char* ObjectName, char* ParameterKey)
{
	unsigned int rc = _SUCCESS;
	struct TreeNode* TargetNode = NULL;
	struct TreeNode* Tmp = NULL;
	struct TreeNode* TmpPrev = NULL;

	LOG(LOG_DEBUG, "Execute DeleteObject Callback function!!\n");
	TargetNode = DM_GetNode(ROOT, ObjectName);
	if (TargetNode == NULL)
	{
		rc = INVALID_PARAMETER_NAME;
		goto FUNC_EXIT;
	}
	DeleteNode(TargetNode->Child);
	Tmp = TargetNode->Parent->Child;
	TmpPrev = TargetNode->Parent;
	while(Tmp != NULL)
	{
		if (strcmp(Tmp->Name, TargetNode->Name) == EQUALITY)
		{
			if (TmpPrev == TargetNode->Parent)
			{
				if (TargetNode->Sibling != NULL)
				{
					TmpPrev->Child = TargetNode->Sibling;
				}
				else
					TmpPrev->Child = NULL;
			}
			else
			{
				TmpPrev->Sibling = TargetNode->Sibling;
			}
			break;
		}
		else
		{
			TmpPrev = Tmp;
			Tmp = Tmp->Sibling;
		}
	}	
	free(TargetNode->Name);
	free(TargetNode->Value);
	free(TargetNode);
#ifdef WITH_TR098
	TargetNode = DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.ParameterKey");
#elif WITH_TR104
	TargetNode = DM_GetNode(ROOT, "Device.ManagementServer.ParameterKey");
#endif
	DM_WriteDataToNode(TargetNode, ParameterKey);

FUNC_EXIT:
	return rc;
}

int Reboot_Impl()
{
#ifdef _JNR_ES_
	if (Method.REBOOTOptCallBackFunc != NULL)//---have plugfunc
	{
		return Method.REBOOTOptCallBackFunc();
	}
#endif
	LOG(LOG_DEBUG, "Doesn't implemented Reboot method...\n");
	return _SUCCESS;
}

int FactoryReset_Impl()
{
#ifdef _JNR_ES_
	if (Method.RESETOptCallBackFunc != NULL)//---have plugfunc
	{
		return Method.RESETOptCallBackFunc();
	}
#endif
	LOG(LOG_DEBUG, "Doesn't implemented FactortReset method...\n");
	return _SUCCESS;
}

#ifdef _JNR_ES_
int Download_Impl(char *FileType, char* URL, char* UserName, char* PassWord, unsigned int FileSize, char* TargetFileName)
#else
int Download_Impl(char* URL, char* UserName, char* PassWord, unsigned int FileSize, char* TargetFileName)
#endif
{
	unsigned int rc = _SUCCESS;
	unsigned int iFilesize = 0;
	FILE *pFile;
	char CheckPath[MAXSTRINGSIZE];
	char FileName[MAXSTRINGSIZE];
	char* pszTmp;
	DWORD size = 0;
	int status = -1;
	char szCommand[MAXSTRINGSIZE];

#ifdef _JNR_ES_
	if (Method.DOWNLOADOptCallBackFunc != NULL)//---have plugfunc
	{
		return Method.DOWNLOADOptCallBackFunc( FileType, URL, UserName, PassWord, FileSize, TargetFileName);
	}
#endif

	LOG(LOG_DEBUG, "[Download] %s^%s^%s^%d^%s\n", URL, UserName, PassWord, FileSize, TargetFileName);
//---check TargetFileName 
    if (strlen(TargetFileName) == 0)
    {
		pszTmp = strrchr(URL, '/');
		if (pszTmp != NULL)
		{
			strncpy_pack(FileName, pszTmp + 1, MAXSTRINGSIZE);
			TargetFileName = FileName;
		}
		else
		{
			LOG(LOG_DEBUG, "Can't find filename");
			rc = DOWNLOAD_FAILURE;
			goto FUNC_EXIT;
		}
	}
	LOG(LOG_DEBUG, "TargetFileName is %s\n", TargetFileName);
//---

	if (UserName == NULL)
		sprintf(szCommand, "wget --connect-timeout=30 --tries=3 -q %s -O /tmp/download.dwn", URL);
	else
		sprintf(szCommand, "wget -q --user=%s --password=%s %s -O /tmp/download.dwn", UserName, PassWord, URL);

	LOG(LOG_DEBUG, "Download Command is \"%s\"\n", szCommand);
	LOG(LOG_DEBUG, "Start Download..\n");
	//system(szCommand);

	//---check filesize
	if (FileSize != 0)
	{
		sprintf(CheckPath, "%s/%s", DOWNLOAD_PATH, TargetFileName);
		LOG(LOG_DEBUG, "Check %s..\n", CheckPath);
		pFile = fopen(CheckPath, "r");
		if (pFile != NULL) 
		{
			//---Obtain file size
			fseek(pFile, 0, SEEK_END);
			iFilesize = ftell(pFile);
			fclose(pFile);

			if (FileSize != iFilesize)
			{
				LOG(LOG_DEBUG, "Check %s..Fail\n", CheckPath);
				LOG(LOG_DEBUG, "%s is %d bytes.\n", TargetFileName, iFilesize);
				rc = DOWNLOAD_FAILURE;
				goto FUNC_EXIT;
			}
			LOG(LOG_DEBUG, "Check %s..OK\n", CheckPath);
		} 
		else
		{
			LOG(LOG_DEBUG, "Check %s..Fail\n", CheckPath);
			rc = DOWNLOAD_FAILURE;
			goto FUNC_EXIT;
		}
	}

FUNC_EXIT:
	return rc;
}
#ifdef _JPR_AM_
int Upload_Impl(char *FileType, char* URL, char* UserName, char* PassWord)
{
	unsigned int rc = _SUCCESS;
	unsigned int iFilesize = 0;
	FILE *pFile;
	char CheckPath[MAXSTRINGSIZE];
	char FileName[MAXSTRINGSIZE];
	char* pszTmp;
	DWORD size = 0;
	int status = -1;
	char szCommand[MAXSTRINGSIZE];

	if (Method.UPLOADOptCallBackFunc != NULL)//---have plugfunc
	{
		return Method.UPLOADOptCallBackFunc( FileType, URL, UserName, PassWord);
	}

FUNC_EXIT:
	return rc;
}
#endif
void RPCIMPL_SetupCallback()
{
	struct RPCMethodCB* RPCMethodCB = NULL;
	//--
	//unsigned int number;
	//AddObject_Impl("InternetGatewayDevice.WANDevice.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.", &number, "111");//----for test
	//DeleteObject_Impl("InternetGatewayDevice.WANDevice.2.", "111");
	//AddObject_Impl("InternetGatewayDevice.WANDevice.", &number, "111");//----for test
	//DeleteObject_Impl("InternetGatewayDevice.WANDevice.3.", "111");
	//AddObject_Impl("InternetGatewayDevice.WANDevice.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.2.WANConnectionDevice.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.2.WANConnectionDevice.1.WANIPConnection.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.PortMapping.", &number, "111");//----for test
	//AddObject_Impl("InternetGatewayDevice.WANDevice.2.WANConnectionDevice.1.WANIPConnection.1.PortMapping.", &number, "111");//----for test
	//DM_Browse(ROOT);
	//DM_ExportDataModel("111.dat", ROOT);
	//--
	LOG(LOG_INFO, "Setup RPC CB...\n");
	RPCMethodCB = (struct RPCMethodCB*)malloc(sizeof(struct RPCMethodCB));
	RPCMethodCB->GetRPCMethods = GetRPCMethod_Impl;
	RPCMethodCB->GetParameterNames = GetParameterNames_Impl;
	RPCMethodCB->GetParameterValues = GetParameterValues_Impl;
	RPCMethodCB->SetParameterValues = SetParameterValues_Impl;
	RPCMethodCB->GetParameterAttributes = GetParameterAttributes_Impl;
	RPCMethodCB->SetParameterAttributes = SetParameterAttributes_Impl;
	RPCMethodCB->AddObject = AddObject_Impl;
	RPCMethodCB->DeleteObject = DeleteObject_Impl;
	RPCMethodCB->Download = Download_Impl;
	RPCMethodCB->FactoryReset = FactoryReset_Impl;
	RPCMethodCB->Reboot = Reboot_Impl;
#ifdef _JPR_AM_
	RPCMethodCB->Upload = Upload_Impl;
#endif	
	RPC_SetRPCRPCMethodCallback(RPCMethodCB);
}
