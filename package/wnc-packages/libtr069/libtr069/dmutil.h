#ifndef __DMUTIL_H__
#define __DMUTIL_H__

#include "Common.h"
struct TreeNode* DM_AddNode(struct TreeNode* ParentNode, char* pszName, char* pszValue, unsigned int uiDataType, unsigned int uiStringSize, unsigned int uiWritable, unsigned int uiNotificationType, unsigned int uiValueChange);
struct TreeNode* DM_ImportDataModel(char* pszFilePath);
struct TreeNode* DM_GetNode(struct TreeNode* Root, char* szName);
struct TreeNode* DM_GetInstanceFromText(char* pszFilePath, char* pszName);
struct TreeNode* DM_GetObjectNode(struct TreeNode* node);
unsigned int DM_GetElementNum(struct TreeNode* node, unsigned int OnlyNextLevel);
int DM_ExportDataModel(char* pszFilePath, struct TreeNode* Root);
int DM_WriteDataToNode(struct TreeNode* Node, char* value);
void DM_SetupTreePluginFunc(struct TreeNode* node);
void DM_GetTotalName(struct TreeNode* node, char* szReturnName);
void DM_Browse(struct TreeNode* node);

#ifdef _JNR_ES_
char *TR069_GetParameterValue2(char* path);
#endif
#endif
