#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "platform.h"
#include "logger.h"
#include "dmutil.h"

struct StringNode{
	char* Name;
	unsigned int isObject;
	struct StringNode* next;
};

static struct TreeNode* SiblingSearch(struct TreeNode* node, char* pszName)
{
	if (node == NULL)
		return NULL;
	while(node->Sibling != NULL)
	{
		if (strcmp(node->Name, pszName) == EQUALITY)
			return node;
		node = node->Sibling;
	}
	if (strcmp(node->Name, pszName) == EQUALITY)
		return node;
	else
		return NULL;
	
}

static void StoreData(FILE *fp, struct TreeNode* node)
{
	char szName[MAXSTRINGSIZE];
	char szParam[MAXSTRINGSIZE];
	if (node == NULL)
		return;
	else
	{
		//---inoder
		memset(szParam, 0, sizeof(szParam));
		if (node->DataType == XSD_STRING)
			sprintf(szParam, "%d %d \"%s\" %d %d %d", node->DataType, node->StringSize-1, node->Value, node->Writable, node->NotificationType, node->ValueChange);
		else
			sprintf(szParam, "%d 0 \"%s\" %d %d %d", node->DataType, node->Value, node->Writable, node->NotificationType, node->ValueChange);

		memset(szName, 0, sizeof(szName));
		DM_GetTotalName(node, szName);
		fprintf(fp,"%s", szName);
		fprintf(fp," %s", szParam);
		fprintf(fp,"\n");
		StoreData(fp, node->Child);
		StoreData(fp, node->Sibling);
	}
}

static void GetSubElementNum(struct TreeNode* node, unsigned int* NodeCount)
{
	if (node == NULL)
		return;
	else
	{
		(*NodeCount) ++;
		GetSubElementNum(node->Child, NodeCount);
		GetSubElementNum(node->Sibling, NodeCount);
	}
}

static void Traversal(struct TreeNode* node)
{
	unsigned int count = 0;
	struct TreeNode* Tmp;
	if (node == NULL)
		return;
	else
	{
		//---inoder
		Tmp = node;
		while (Tmp->Parent != NULL)
		{
			count = count + strlen(Tmp->Parent->Name) + 1;
			Tmp = Tmp->Parent;
		}
		while (count != 0)
		{
			printf(" ");
			count--;
		}
		printf("%s", node->Name);
		
		if (node->Child != NULL)
			printf(".");

		if (node->GETOptCallBackFunc != NULL || node->SETOptCallBackFunc != NULL)
			printf("    GETFunc = %p, SETFunc = %p", node->GETOptCallBackFunc, node->SETOptCallBackFunc);

		printf("\n");
		Traversal(node->Child);
		Traversal(node->Sibling);
	}
}

int TR069_SetParameterValue(char* path, char* value)
{
	int iReturn = _SUCCESS;
	struct TreeNode* node = NULL;
	struct TreeNode* ObjectNode = NULL;
	char szObjectPath[MAXSTRINGSIZE];
	unsigned int* EVENT_FLAG = NULL;
	node = DM_GetNode(ROOT, path);
	if (node == NULL)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	//---Alvin add 2011/12/18
	if (node->SETOptCallBackFunc != NULL)//---have plugfunc
	{
		ObjectNode = DM_GetObjectNode(node);
		DM_GetTotalName(ObjectNode, szObjectPath);
#ifdef _JNR_ES_
		if (node->SETOptCallBackFunc(value, path) == _ERROR)
#else
		if (node->SETOptCallBackFunc(value, szObjectPath) == _ERROR)
#endif
		{
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}
	}
	//---

	if (strcmp(node->Value, value) != EQUALITY)
	{
		DM_WriteDataToNode(node, value);
#ifdef __JPR_AM_
		if(isBOOTInformOK()==TRUE) {
			node->ValueChange = TRUE;
			if (node->NotificationType == ACTIVE_NOTIFICATION)
			{
				if(isInformOKLocked() == FALSE){
				    LOG(LOG_DEBUG, "Active notification (%s)\n", path);
				    EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
				    memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
				    ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
				} else
				    LOG(LOG_DEBUG, "wait inform unlock (%s)\n", path);
			}
		} else
			LOG(LOG_DEBUG, "Ignore value change (%s)\n", path);
#else
		node->ValueChange = TRUE;
		if (node->NotificationType == ACTIVE_NOTIFICATION)
		{
			EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
			memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
			ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
		}
#endif
	}
FUNC_EXIT:
	return iReturn;
}

int TR069_GetParameterValue(char* path, char* value, size_t iLen)
{
	int iReturn = _SUCCESS;
	char* result = NULL;
	struct TreeNode* node = NULL;
	struct TreeNode* ObjectNode = NULL;
	char szObjectPath[MAXSTRINGSIZE];
	node = DM_GetNode(ROOT, path);

	if (node == NULL)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	else
	{
		//---Alvin add 2011/12/29
		if (node->GETOptCallBackFunc !=NULL)
		{
#ifndef _JPR_AM_
			printf("%s: path:%s callback!\n", __func__, path);
#endif
			ObjectNode = DM_GetObjectNode(node);
			DM_GetTotalName(ObjectNode, szObjectPath);
			result = node->GETOptCallBackFunc(szObjectPath);
#ifdef _JNR_ES_
			result = node->GETOptCallBackFunc(path);
#else
			result = node->GETOptCallBackFunc(szObjectPath);
#endif
			if (result == NULL || DM_WriteDataToNode(node, result) == _ERROR)
			{
				iReturn = _ERROR;
				goto FUNC_EXIT;
			}
		}
		memset(value, 0, iLen);
		strncpy(value, node->Value, iLen);
		//---
	}
FUNC_EXIT:
	return iReturn;
}

void DM_Browse(struct TreeNode* node)
{
	unsigned int count = 0;
	struct TreeNode* Tmp;

	Tmp = node;
	while (Tmp->Parent != NULL)
	{
		count = count + strlen(Tmp->Parent->Name) + 1;
		Tmp = Tmp->Parent;
	}
	while (count != 0)
	{
		printf(" ");
		count--;
	}
	printf("%s", node->Name);
	
	if (node->Child != NULL)
	{
		printf(".\n");
		Traversal(node->Child);
	}
	else
		printf("\n");

}

unsigned int DM_GetElementNum(struct TreeNode* node, unsigned int OnlyNextLevel)
{
	unsigned int NodeCount = 1; //start node 
	struct TreeNode* Tmp;
	if (node->Child != NULL)
	{
		if (OnlyNextLevel == FALSE)
		{
			GetSubElementNum(node->Child, &NodeCount);
		}
		else if (OnlyNextLevel == TRUE)
		{
			Tmp = node->Child;
			while(Tmp != NULL)
			{
				NodeCount++;
				Tmp = Tmp->Sibling;
			}
		}
	}
	return NodeCount;
}

void DM_GetTotalName(struct TreeNode* node, char* szReturnName)
{
	char szName[MAXSTRINGSIZE];
	struct TreeNode* Tmp;
	struct StringNode* Head;
	struct StringNode* NewNode;
	struct StringNode* CurrNode;
	struct StringNode* TmpNode;
	//---create string node
	CurrNode = (struct StringNode*)malloc(sizeof(struct StringNode));
	CurrNode->Name = (char*)malloc(strlen(node->Name) + 1); //--- + 1:some string has no '\0' in the end.
	strncpy_pack(CurrNode->Name, node->Name, strlen(node->Name) + 1);
	CurrNode->next = NULL;
	if (node->DataType == XSD_OBJECT)
		CurrNode->isObject = TRUE;
	else
		CurrNode->isObject = FALSE;
	//---
	//---create parent string node
	Tmp = node;
	while (Tmp->Parent != NULL)
	{
		Tmp = Tmp->Parent;

		NewNode = (struct StringNode*)malloc(sizeof(struct StringNode));
		NewNode->Name = (char*)malloc(strlen(Tmp->Name) + 1);  //--- + 1:some string has no '\0' in the end.
		strncpy_pack(NewNode->Name, Tmp->Name, strlen(Tmp->Name) + 1);
		NewNode->next = CurrNode;
		if  (Tmp->DataType == XSD_OBJECT)
			NewNode->isObject = TRUE;
		else
			NewNode->isObject = FALSE;
		CurrNode = NewNode;
	}
	//---
	//---prepare total name
	Head = CurrNode;
	memset(szName, 0, sizeof(szName));
	while(CurrNode != NULL)
	{
		strcat(szName, CurrNode->Name);
		if (CurrNode->isObject == TRUE)
			strcat(szName,".");
		CurrNode = CurrNode->next;
	}
	//---
	//---free string node
	CurrNode = Head;
	while(CurrNode != NULL)
	{
		TmpNode = CurrNode;
		CurrNode = CurrNode->next;
		memset(TmpNode->Name, 0, strlen(TmpNode->Name));
		free(TmpNode->Name);
		free(TmpNode);
	}
	//---
	strncpy_pack(szReturnName, szName, MAXSTRINGSIZE);
}

int DM_ExportDataModel(char* pszFilePath, struct TreeNode* Root)
{
	FILE *fp = fopen(pszFilePath, "w");
	StoreData(fp, Root);
	fclose(fp);
	return _SUCCESS;
}

//---ignore number char
//---szSrc = InternetGatewayDevice.aaa.1.bbb.1.ccc.
//---szDst = InternetGatewayDevice.aaa.bbb.ccc.
static void IgnoreNumberChar(char* szSrc, char* szDst)
{
	char szName[MAXSTRINGSIZE];
	char *pszSplit = NULL, *pszTmp = NULL;

	strncpy_pack(szName, szSrc, MAXSTRINGSIZE);
	memset(szDst, 0, sizeof(szDst));

	pszSplit = strtok(szName, ".");
	if (pszSplit != NULL)
	{
		strcat(szDst, pszSplit);
		while (1)
		{
			pszSplit = strtok(NULL, ".");
			if (pszSplit != NULL)
			{
				if (isNumber(pszSplit) == FALSE)
				{
					strcat(szDst, ".");
					strcat(szDst, pszSplit);
				}
			}
			else
				break;
		}
	}

	//check if the last char is '.'
	pszTmp = strrchr(szSrc, '.');
	if (pszTmp != NULL)
	{
		pszTmp = pszTmp + 1;
		if (strlen(pszTmp) <= 0)
			strcat(szDst, ".");	
	}
}

static int SetNodePluginFunc(struct TreeNode* node)
{
	int iReturn = _SUCCESS;
	char szName[MAXSTRINGSIZE];
	char szNameClone[MAXSTRINGSIZE];
	char* pszDot = NULL;
	char* pszSplit = NULL;
	struct TreeNode* CurrNode = NULL;
	struct TreeNode* SearchNode = NULL;
	
	if (FUNCROOT == NULL)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	CurrNode = FUNCROOT;
	memset(szName, 0, sizeof(szName));
	DM_GetTotalName(node, szName);
#ifdef _JPR_AM_
	unsigned int match_num = 0;
	strncpy_pack(szNameClone, szName, MAXSTRINGSIZE);
	pszSplit = strtok(szNameClone, ".");
	while(pszSplit != NULL)
	{
		if (strcmp(pszSplit, node->Name) == EQUALITY)
			match_num++;
		pszSplit = strtok(NULL, ".");
	}

	pszSplit = strtok(szName, ".");
#else
	IgnoreNumberChar(szName, szNameClone);
	pszSplit = strtok(szNameClone, ".");
#endif
	while (pszSplit != NULL)
	{
		SearchNode = SiblingSearch(CurrNode, pszSplit);
		if (SearchNode != NULL)
		{
			if (strcmp(node->Name, pszSplit) == EQUALITY) //---match
			{
			#ifdef _JPR_AM_ //InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.WEPKey.1.WEPKey
				match_num--;
				if (match_num == 0){
				    if (SearchNode->GETOptCallBackFunc != NULL || SearchNode->SETOptCallBackFunc != NULL)
				    {
					    node->GETOptCallBackFunc = SearchNode->GETOptCallBackFunc;
					    node->SETOptCallBackFunc = SearchNode->SETOptCallBackFunc;
				    }
				    goto FUNC_EXIT;
				} else
				    CurrNode = SearchNode->Child;
			#else
				if (SearchNode->GETOptCallBackFunc != NULL || SearchNode->SETOptCallBackFunc != NULL)
				{
					node->GETOptCallBackFunc = SearchNode->GETOptCallBackFunc;
					node->SETOptCallBackFunc = SearchNode->SETOptCallBackFunc;
				}
				goto FUNC_EXIT;
			#endif
			}
			else
				CurrNode = SearchNode->Child;
		}
		else
		{
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}
		pszSplit = strtok(NULL, ".");
	}

FUNC_EXIT:
	return iReturn;
}

void DM_SetupTreePluginFunc(struct TreeNode* node)
{
	if (node == NULL)
		return;
	else
	{
		//---Inorder
		SetNodePluginFunc(node);
		DM_SetupTreePluginFunc(node->Child);
		DM_SetupTreePluginFunc(node->Sibling);
	}
}

struct TreeNode* DM_ImportDataModel(char* pszFilePath)
{
	FILE *fp = fopen(pszFilePath, "r");
	char szName[MAXSTRINGSIZE];
	char szNameClone[MAXSTRINGSIZE];
	char szValue[MAXSTRINGSIZE];
	char szParsedValue[MAXSTRINGSIZE];
	unsigned int uiDataType, uiStringSize, uiWritable, uiNotification, uiValueChange;
	char *pszSplit = NULL, *pszDot = NULL;
	char *pszValueHead = NULL, *pszValueTail = NULL;
	struct TreeNode* Root = NULL;
	struct TreeNode* SearchNode = NULL;
	struct TreeNode* CurrNode = NULL;
	memset(szName, 0, sizeof(szName));
	memset(szValue, 0, sizeof(szValue));
#ifdef _JPR_AM_
	while (fscanf(fp,"%s %d %d %d %d %d %[^\n]", szName, &uiDataType, &uiStringSize, &uiWritable, &uiNotification, &uiValueChange, szValue) != EOF)
#else
	while (fscanf(fp,"%s %d %d %s %d %d %d", szName, &uiDataType, &uiStringSize, szValue, &uiWritable, &uiNotification, &uiValueChange) != EOF)
#endif
	{
		pszDot = strrchr(szName, '.');
		if (pszDot != NULL)
			pszDot+=1;

		strncpy_pack(szNameClone, szName, MAXSTRINGSIZE);

		pszSplit = strtok(szNameClone, ".");
		while (pszSplit != NULL)
		{
			if (CurrNode != NULL)
			{
				if (SiblingSearch(CurrNode, pszSplit) == NULL)
				{
					SearchNode = SiblingSearch(CurrNode->Child, pszSplit);
					if (SearchNode == NULL)
					{
						if (pszDot == NULL || (pszDot != NULL && strcmp(pszSplit, pszDot) == EQUALITY))
						{
							pszValueHead = strchr(szValue, '"') + 1;
							pszValueTail = strrchr(szValue, '"');
							memset(szParsedValue, 0, sizeof(szParsedValue));
							strncpy(szParsedValue, pszValueHead, pszValueTail-pszValueHead);
							CurrNode = DM_AddNode(CurrNode, pszSplit, szParsedValue, uiDataType, uiStringSize, uiWritable, uiNotification, uiValueChange);
						}
						else 
							CurrNode = DM_AddNode(CurrNode, pszSplit, "", XSD_OBJECT, 0, FALSE, NOTIFICATION_OFF, FALSE);
					}
					else
						CurrNode = SearchNode;
				}
			}
			else//---initial
			{
				CurrNode = DM_AddNode(NULL, pszSplit, "", XSD_OBJECT, 0, FALSE, NOTIFICATION_OFF, FALSE);
				Root = CurrNode;
			}
			pszSplit = strtok(NULL, ".");
		}
		SetNodePluginFunc(CurrNode);
		CurrNode = Root;
	}
	fclose(fp);
	return Root;
}

struct TreeNode* DM_GetInstanceFromText(char* pszFilePath, char* pszName)
{
	FILE *fp = fopen(pszFilePath, "r");
	unsigned int state = 0;
	char szPath[MAXSTRINGSIZE];
	char szNameClone[MAXSTRINGSIZE];
	char szLocalPath[MAXSTRINGSIZE];
	char szValue[MAXSTRINGSIZE];
	char szParsedValue[MAXSTRINGSIZE];
	unsigned int uiDataType, uiStringSize, uiWritable, uiNotification, uiValueChange;
	char *pszSplit = NULL, *pszDot = NULL;
	char *pszValueHead = NULL, *pszValueTail = NULL;
	struct TreeNode* Root = NULL;
	struct TreeNode* SearchNode = NULL;
	struct TreeNode* CurrNode = NULL;

	IgnoreNumberChar(pszName, szNameClone);
	//--- 

	memset(szPath, 0, sizeof(szPath));
	memset(szValue, 0, sizeof(szValue));
	while (fscanf(fp,"%s %d %d %s %d %d %d", szPath, &uiDataType, &uiStringSize, szValue, &uiWritable, &uiNotification, &uiValueChange) != EOF)
	{
		switch(state)
		{
		case 0:
			if (strcmp(szNameClone, szPath) == EQUALITY)
			{
				Root = DM_AddNode(NULL, "-", "", XSD_OBJECT, 0, FALSE, NOTIFICATION_OFF, FALSE);
				CurrNode = Root;
				state = 1;
			}
			break;
		case 1:
			if (strchr(szPath, '#'))
				goto FUNC_EXIT;

			pszDot = strrchr(szPath, '.');
			if (pszDot != NULL)
				pszDot+=1;

			strncpy_pack(szLocalPath, szPath, MAXSTRINGSIZE);

			pszSplit = strtok(szLocalPath, ".");
			while (pszSplit != NULL)
			{
				if (SiblingSearch(CurrNode, pszSplit) == NULL)
				{
					SearchNode = SiblingSearch(CurrNode->Child, pszSplit);
					if (SearchNode == NULL)
					{
						if (pszDot == NULL || (pszDot != NULL && strcmp(pszSplit, pszDot) == EQUALITY))
						{
							pszValueHead = strchr(szValue, '"') + 1;
							pszValueTail = strrchr(szValue, '"');
							memset(szParsedValue, 0, sizeof(szParsedValue));
							strncpy(szParsedValue, pszValueHead, pszValueTail-pszValueHead);	
							CurrNode = DM_AddNode(CurrNode, pszSplit, szParsedValue, uiDataType, uiStringSize, uiWritable, uiNotification, uiValueChange);
						}
						else 
							CurrNode = DM_AddNode(CurrNode, pszSplit, "", XSD_OBJECT, 0, FALSE, NOTIFICATION_OFF, FALSE);
					}
					else
						CurrNode = SearchNode;
				}
				pszSplit = strtok(NULL, ".");
			}
			CurrNode = Root;
			break;
		}
	}
FUNC_EXIT:
	fclose(fp);
	return Root;
}

struct TreeNode* DM_AddNode(struct TreeNode* ParentNode, char* pszName, char* pszValue, unsigned int uiDataType, unsigned int uiStringSize, unsigned int uiWritable, unsigned int uiNotificationType, unsigned int uiValueChange)
{
	int iReturn = _SUCCESS;
	struct TreeNode* NewNode;
	struct TreeNode* CurrNode;

	NewNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
	NewNode->Name = (char*)malloc(strlen(pszName) + 1);  //--- + 1:some string has no '\0' in the end.
	strncpy_pack(NewNode->Name, pszName, strlen(pszName) + 1);
	if (uiDataType == XSD_STRING)
	{
		NewNode->Value = (char*)malloc(uiStringSize + 1);  //--- + 1:for some string limit just fills the content has no add '\0' char
		NewNode->StringSize = uiStringSize + 1;
		strncpy_pack(NewNode->Value, pszValue, uiStringSize + 1);
	}
#ifdef _JPR_AM_
	else if (uiDataType == XSD_DATETIME)
	{
	    int DATETIMEVALUESIZE = 24;
		NewNode->Value = (char*)malloc(DATETIMEVALUESIZE + 1);
		NewNode->StringSize = DATETIMEVALUESIZE + 1;
		strncpy_pack(NewNode->Value, pszValue, DATETIMEVALUESIZE + 1);
	}
#endif
	else
	{
		NewNode->Value = (char*)malloc(MAXVALUESIZE + 1);
		NewNode->StringSize = MAXVALUESIZE + 1;
		strncpy_pack(NewNode->Value, pszValue, MAXVALUESIZE + 1);
	}

	NewNode->DataType = uiDataType;
	NewNode->Writable = uiWritable;
	NewNode->NotificationType = uiNotificationType;
	NewNode->ValueChange = uiValueChange;
	NewNode->Parent = NULL;
	NewNode->Sibling = NULL;
	NewNode->Child = NULL;
	NewNode->GETOptCallBackFunc = NULL;
	NewNode->SETOptCallBackFunc = NULL;

	if (ParentNode != NULL)
	{
		if (ParentNode->Child == NULL)
			ParentNode->Child = NewNode;
		else
		{
			CurrNode = ParentNode->Child;
			while (CurrNode->Sibling != NULL)
				CurrNode = CurrNode->Sibling;
			CurrNode->Sibling = NewNode;
		}
		NewNode->Parent = ParentNode;
	}
	

	if (iReturn == _SUCCESS)
		return NewNode;
	else
		return NULL;
}

struct TreeNode* DM_GetNode(struct TreeNode* Root, char* pszName)
{
	unsigned int match_num = 0;
	unsigned int bIsNumber = FALSE;
	char *pszSplit = NULL, *pszDot = NULL;
	char szNameClone[MAXSTRINGSIZE], szTmp[MAXSTRINGSIZE];
	struct TreeNode* CurrNode = NULL;
	struct TreeNode* SearchNode = NULL;
	if (strlen(pszName) == 0)//for fit empty szName return root node
		return Root;

	//---get the last string
	//---InternetGatewayDevice.ManagementServer. ==> pszDot = ManagementServer
	//---InternetGatewayDevice.ManagementServer.URL ==> pszDot = URL
	pszDot = strrchr(pszName, '.');
	if (pszDot != NULL)
	{
		if (strlen(pszDot + 1) == 0)
		{
			memset(szTmp, 0, sizeof(szTmp));
			strncpy(szTmp, pszName, pszDot-pszName);
			pszDot = strrchr(szTmp, '.');
			if (pszDot != NULL)
				pszDot = pszDot+1;
			else
				pszDot = szTmp;
		}
		else
			pszDot += 1;
	}
	else
		return NULL;
	//---

	//---If the last string is a number, check the match number in total path
#ifndef _JPR_AM_ //InternetGatewayDevice.LANDevice.1.WLANConfiguration.1.WEPKey.1.WEPKey
	if (isNumber(pszDot) == TRUE)
#endif
	{
		bIsNumber = TRUE;
		strncpy_pack(szNameClone, pszName, MAXSTRINGSIZE);
		pszSplit = strtok(szNameClone, ".");
		while(pszSplit != NULL)
		{
			if (strcmp(pszSplit, pszDot) == EQUALITY)
				match_num++;
			pszSplit = strtok(NULL, ".");
		}
	}
	//---
	strncpy_pack(szNameClone, pszName, MAXSTRINGSIZE);
	CurrNode = Root;

	//---Sibling fast search
	//---  path = InternetGatewayDevice.ManagementServer.URL  split to  InternetGatewayDevice  ManagementServer  URL
	//---
	//---				    ->(search InternetGatewayDevice)
	//---         InternetGatewayDevice
	//---		 /		->(search ManagementServer)
	//---		DeviceSummary--DeviceInfo--LANDeviceNumberOfEntries--ManagementServer--...
	//---					  /										/->(search URL)
	//---					 Manufacturer--ManufacturerOUI--....   URL
	pszSplit = strtok(szNameClone, ".");
	
	while (pszSplit != NULL)
	{
		SearchNode = SiblingSearch(CurrNode, pszSplit);
		if (SearchNode != NULL) //---if hit, go to next level
		{	
			if (strcmp(pszDot, pszSplit) == EQUALITY) //---check if it is the last split
			{
				if (bIsNumber == TRUE) //---when the last string is a number
				{
					match_num--;
					if (match_num == 0)
						return SearchNode;
				}
				else
					return SearchNode;
			}
			CurrNode = SearchNode->Child;
		}
		else
			return NULL;
		pszSplit = strtok(NULL, ".");
	}
	//---
	return NULL;
}

static struct TreeNode* CreateFuncNode(char *path)
{
	char szNameClone[MAXSTRINGSIZE];
	char *pszSplit = NULL, *pszDot = NULL;
	struct TreeNode* SearchNode = NULL;
	struct TreeNode* CurrNode = FUNCROOT;
	pszDot = strrchr(path, '.');
	if (pszDot != NULL)
		pszDot+=1;
	strncpy_pack(szNameClone, path, MAXSTRINGSIZE);
	pszSplit = strtok(szNameClone, ".");
	while (pszSplit != NULL)
	{
		if (CurrNode != NULL)
		{
			if (SiblingSearch(CurrNode, pszSplit) == NULL)
			{
				SearchNode = SiblingSearch(CurrNode->Child, pszSplit);
				if (SearchNode == NULL)
				{
					CurrNode = DM_AddNode(CurrNode, pszSplit, "", XSD_OBJECT, 0, FALSE, NOTIFICATION_OFF, FALSE);
				}
				else
					CurrNode = SearchNode;
			}
		}
		else//---initial
		{
			CurrNode = DM_AddNode(NULL, pszSplit, "", XSD_OBJECT, 0, FALSE, NOTIFICATION_OFF, FALSE);
			FUNCROOT = CurrNode;
		}
		pszSplit = strtok(NULL, ".");
	}
	return CurrNode;
}

struct TreeNode* DM_GetObjectNode(struct TreeNode* node)
{
	struct TreeNode* Tmp = NULL;
	if (node->DataType == XSD_OBJECT)
		return node;
	else
	{
		Tmp = node;
		while(Tmp != NULL)
		{
			if (Tmp->DataType == XSD_OBJECT)
				break;	
			Tmp = Tmp->Parent;
		}
	}
	return Tmp;
}

int TR069_RegisterSETOptFunc(char *path, TR069_SETOptCallBackFunc SETOptCallBackFunc)
{
	int iReturn = _SUCCESS;
	struct TreeNode* node;
	node = CreateFuncNode(path);
	node->SETOptCallBackFunc = SETOptCallBackFunc;
	return iReturn;
}

int TR069_RegisterGETOptFunc(char *path, TR069_GETOptCallBackFunc GETOptCallBackFunc)
{
	int iReturn = _SUCCESS;
	struct TreeNode* node;
	node = CreateFuncNode(path);
	node->GETOptCallBackFunc = GETOptCallBackFunc;
	return iReturn;
}

#ifdef _JNR_ES_
int TR069_RegisterDOWNLOADOptFunc( TR069_DOWNLOADOptCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.DOWNLOADOptCallBackFunc = CallBackFunc;
	return iReturn;
}

int TR069_RegisterRESETOptFunc( TR069_RESETOptCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.RESETOptCallBackFunc = CallBackFunc;
	return iReturn;
}

int TR069_RegisterREBOOTOptFunc( TR069_REBOOTOptCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.REBOOTOptCallBackFunc = CallBackFunc;
	return iReturn;
}
#endif

int DM_WriteDataToNode(struct TreeNode* Node, char* value)
{
	int iReturn = _SUCCESS;

//---format check
	switch(Node->DataType)
	{
	case XSD_STRING: case XSD_DATETIME: case XSD_BASE64:
		strncpy_pack(Node->Value, value, Node->StringSize);
		break;
	case XSD_INT:
		strncpy_pack(Node->Value, value, Node->StringSize);
		break;
	case XSD_UNSIGNEDINT:
		strncpy_pack(Node->Value, value, Node->StringSize);
		break;
	case XSD_BOOLEAN:
		strncpy_pack(Node->Value, value, Node->StringSize);
		break;
	default:
		iReturn = _ERROR;
		break;
	}
//---
	return iReturn;
}

#ifdef _JNR_ES_
char *TR069_GetParameterValue2(char* path)
{
	int status = _SUCCESS;
	char szValue[MAXSTRINGSIZE];

	status = TR069_GetParameterValue(path, szValue, sizeof(szValue));
	if ( status != _SUCCESS )
	{
		LOG(LOG_INFO, "TR069_GetParameterValue (%s)..Error\n", path);
	}
	else
	{
	#ifdef _JPR_AM_
	    return (DM_GetNode(ROOT, path))->Value;
	#else
		return szValue;
	#endif
	}
}
#endif
#ifdef _JPR_AM_
int TR069_SetParameterValueInternal(char* path, char* value,int* notification)
{
	int iReturn = _SUCCESS;
	struct TreeNode* node = NULL;
	struct TreeNode* ObjectNode = NULL;
	char szObjectPath[MAXSTRINGSIZE];
	unsigned int* EVENT_FLAG = NULL;

	if(value == NULL)
	    goto FUNC_EXIT;

	node = DM_GetNode(ROOT, path);
	if (node == NULL)
	{
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	if (strcmp(node->Value, value) != EQUALITY)
	{
		DM_WriteDataToNode(node, value);
		if(isBOOTInformOK()==TRUE) {
			node->ValueChange = TRUE;
			if (node->NotificationType == ACTIVE_NOTIFICATION && *notification == TRUE)
			{
				if(isInformOKLocked() == FALSE){
				    *notification = FALSE;
				    LOG(LOG_DEBUG, "Active notification (%s)\n", path);
				    EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
				    memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
				    ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
				} else
				    LOG(LOG_DEBUG, "wait inform unlock (%s)\n", path);
			}
		} else
			LOG(LOG_DEBUG, "Ignore value change (%s)\n", path);
	}
FUNC_EXIT:
	return iReturn;
}

int TR069_RegisterSETDoneFunc( TR069_SETDoneCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.SETDoneCallBackFunc = CallBackFunc;
	return iReturn;
}
int TR069_RegisterSaveLogFunc( TR069_SaveLogCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.SaveLogCallBackFunc = CallBackFunc;
	return iReturn;
}
int TR069_RegisterUPLOADOptFunc( TR069_UPLOADOptCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.UPLOADOptCallBackFunc = CallBackFunc;
	return iReturn;
}
int TR069_RegisterPingDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.PingCallBackFunc = CallBackFunc;
	return iReturn;
}
int TR069_RegisterTraceRouteDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.TraceRouteCallBackFunc = CallBackFunc;
	return iReturn;
}
int TR069_RegisterUploadDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.UploadDiagnosticCallBackFunc = CallBackFunc;
	return iReturn;
}
int TR069_RegisterDownloadDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.DownloadDiagnosticCallBackFunc = CallBackFunc;
	return iReturn;
}
int TR069_RegisterUDPEchoDiagnosticFunc( TR069_SetupCallBackFunc CallBackFunc )
{
	int iReturn = _SUCCESS;
	Method.UDPEchoDiagnosticCallBackFunc = CallBackFunc;
	return iReturn;
}

struct TreeNode* DM_AddNodeRegisterSetFunc(TR069_SETOptCallBackFunc SETOptCallBackFunc, struct TreeNode* ParentNode, 
  char* pszName, char* pszValue, unsigned int uiDataType, unsigned int uiStringSize, 
  unsigned int uiWritable, unsigned int uiNotificationType, unsigned int uiValueChange)
{
	struct TreeNode* node;
	node = DM_AddNode(ParentNode,pszName,pszValue,uiDataType,uiStringSize,uiWritable,uiNotificationType,uiValueChange);
	if(node!=NULL)
        node->SETOptCallBackFunc = SETOptCallBackFunc;
	return node;	
}
#endif
