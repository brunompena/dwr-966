/****************************************************************************
* Copyright: Institute for Information Industry. All rights reserved.
*
* File: TR111.c
*
* Description: Implement TR-111 function of TR-069 agent module 
*
* Version: 1.0
* Date: 2009/08/24
* 
* Change History:
*   V1.0, 2009/08/24
*   New created.
*
****************************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Common.h"
#include "platform.h"
#include "logger.h"
#include "dmutil.h"
#include "stun.h"
#include "udp.h"
#include "tr111.h"

BOOL bSendUDPExit	= FALSE;
BOOL bGetUDPExit	= FALSE;

StunAddress4 STUNAddr_Server;
StunAddress4 STUNAddr_Mapped;

static DWORD TR111_SendUDP(void *arg)
{
	char			acBuf[STUN_MAX_MESSAGE_SIZE] = {0};
	int				iLength;
	int				iSocketfd = (int )arg;
	StunMessage		STUNMsgRequest;
	StunAtrString   STUNInfo_Username, STUNInfo_Passwd;
	

	memset(&STUNInfo_Username, 0, sizeof(StunAtrString));
	memset(&STUNInfo_Passwd, 0, sizeof(StunAtrString));
	memset(&STUNMsgRequest, 0, sizeof(StunMessage));
	bSendUDPExit = FALSE;
	while(1)
	{
		STUNMsgRequest.msgHdr.msgType = BindRequestMsg;
		STUNMsgRequest.msgHdr.id.octet[0] = 1;
		iLength = stunEncodeMessage(&STUNMsgRequest, acBuf, STUN_MAX_MESSAGE_SIZE, &STUNInfo_Passwd, FALSE);

		if(!sendMessage(iSocketfd, acBuf, iLength, STUNAddr_Server.addr, STUNAddr_Server.port, FALSE))
			return _ERROR;
		PLATFORM_SleepSec(10);
		if(bSendUDPExit == TRUE)
			break;
	}
	LOG(LOG_INFO, "TR111_SendUDP terminate\n");

	return _SUCCESS;
}

static DWORD TR111_GetUDP(void *arg)
{
	
	int		fd = (int )arg;
	int		iRecvBytes = 0;
	int		iSourceSckLength = sizeof(struct sockaddr);
	unsigned int* EVENT_FLAG = NULL;
	char	szBuf[STUN_MAX_MESSAGE_SIZE] = {0};
	char	UDPConnReqAddr[MAXURLSIZE] = {0};
	BOOL	bResult = FALSE;
	StunMessage			STUNMsgResponse;
	struct sockaddr_in	SourceSocket;
	struct in_addr	addr;
	memset(&STUNMsgResponse, 0, sizeof(StunMessage));

	bGetUDPExit = FALSE;
	while (1)
	{
		iRecvBytes = recvfrom(fd, (void *)szBuf, STUN_MAX_MESSAGE_SIZE, 0, (struct sockaddr *)&SourceSocket, (int *)&iSourceSckLength);
		if (szBuf[0] == '?')//GET Packet from ACS
		{
			//LOG(LOG_DEBUG, "[Received Packet from ACS] Addr:%d Port:%d\n", ntohl(SourceSocket.sin_addr.s_addr), ntohs(SourceSocket.sin_port));
			LOG(LOG_DEBUG, "[Received Packet from ACS]\n");
			EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
			memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
			EVENT_FLAG[CONNECTION_REQUEST] = TRUE;
			ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
		}
		else//GET Packet from STUN
		{
			//LOG(LOG_DEBUG, "[Received Packet from STUN] Addr : %d  Port : %d\n", ntohl(SourceSocket.sin_addr.s_addr),ntohs(SourceSocket.sin_port));
			LOG(LOG_DEBUG, "[Received Packet from STUN]\n");
			bResult = stunParseMessage(szBuf, iRecvBytes, &STUNMsgResponse, FALSE);
			if (bResult)
			{
				if ((STUNAddr_Mapped.port != STUNMsgResponse.mappedAddress.ipv4.port) ||
					(STUNAddr_Mapped.addr != STUNMsgResponse.mappedAddress.ipv4.addr)) 
				{
					LOG(LOG_DEBUG, "Binding change from port %d to port %d...\n",STUNAddr_Mapped.port, STUNMsgResponse.mappedAddress.ipv4.port);
					STUNAddr_Mapped.port = STUNMsgResponse.mappedAddress.ipv4.port;
					LOG(LOG_DEBUG, "Binding change from addr %d to addr %d...\n",STUNAddr_Mapped.addr, STUNMsgResponse.mappedAddress.ipv4.addr);
					STUNAddr_Mapped.addr = STUNMsgResponse.mappedAddress.ipv4.addr;
					addr.s_addr = ntohl(STUNAddr_Mapped.addr);
					sprintf(UDPConnReqAddr, "%s:%d", inet_ntoa(addr), STUNAddr_Mapped.port);
					TR069_SetParameterValue("InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress", UDPConnReqAddr);
					EVENT_FLAG = (unsigned int*)malloc(sizeof(unsigned int) * MAXEVENT);
					memset(EVENT_FLAG, 0, sizeof(unsigned int) * MAXEVENT);
					ExecuteInform(INFORM_TYPE_IMMEDIATE, EVENT_FLAG);
				}
			}
			//---
		}
		if(bGetUDPExit == TRUE)
			break;
	}
	LOG(LOG_DEBUG, "TR111_GetUDP terminate\n");
	return _SUCCESS;
}

int TR111_Initial()
{
	unsigned int	iReturn = _SUCCESS;
	int             iLength;
	BOOL			bVerbose, bPreServerPort, bHairpin, bResult;
	BOOL			bNAT = FALSE;
	char            acBuf[STUN_MAX_MESSAGE_SIZE] = {0};
	char			UDPConnReqAddr[MAXURLSIZE] = {0};
	SOCKET			fd;
	NatType         eNATType;
	StunAtrString   Username;
	StunAtrString	Password;
	StunAddress4    STUNAddr_Source;  
	StunMessage     STUNMsg_Request, STUNMsg_Response;
	
	struct in_addr	addr;
	struct TreeNode* STUNaddr = NULL;
	struct platformThread_t SendUDPThread, GetUDPThread;

	fd			= INVALID_SOCKET;
	iLength		= STUN_MAX_MESSAGE_SIZE;
	bVerbose	= bPreServerPort = bHairpin = bResult = FALSE;
		
	memset(&Username, 0, sizeof(StunAtrString));
	memset(&Password, 0, sizeof(StunAtrString));
	memset(&STUNAddr_Source, 0, sizeof(StunAddress4));
	memset(&STUNMsg_Request, 0, sizeof(StunMessage));
	memset(&STUNMsg_Response, 0, sizeof(StunMessage));
	memset(&STUNAddr_Server, 0, sizeof(StunAddress4));
	memset(&STUNAddr_Mapped, 0, sizeof(StunAddress4));
    
	// Initial functions
	initNetwork(); // initial UDP network function

	/* Get the STUN url */
#ifdef WITH_TR098
	STUNaddr = DM_GetNode(ROOT, "InternetGatewayDevice.ManagementServer.STUNServerAddress");
#elif WITH_TR106
	STUNaddr = DM_GetNode(ROOT, "Device.ManagementServer.STUNServerAddress");
#endif
	if (STUNaddr == NULL || strlen(STUNaddr->Value) <= 1)
	{
		LOG(LOG_ERROR, "Can't STUNServerAddress node");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	else
		LOG(LOG_INFO, "STUNServerAddress is \"%s\"\n", STUNaddr->Value);
	
	// Verify STUN server name
	if (!stunParseServerName(STUNaddr->Value, &STUNAddr_Server))
	{
		LOG(LOG_ERROR, "Not a valid host name\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	else
	{
		LOG(LOG_INFO, "Start to connect STUN_server...\n");
		// Get STUN NAT network type
		eNATType = stunNatType(&STUNAddr_Server, bVerbose, &bPreServerPort, &bHairpin, 0, 0);
		switch(eNATType)
		{
			case StunTypeUnknown:
				LOG(LOG_DEBUG, "Some unknown type error detetecting NAT type\n");
				break;
			case StunTypeFailure:
				LOG(LOG_DEBUG, "Some stun error detetecting NAT type\n");
				break;
			case StunTypeOpen:
				LOG(LOG_DEBUG, "Open\n");
				break;
			case StunTypeFirewall:
				LOG(LOG_DEBUG, "Firewall!! but it may have no NAT!\n");
				bNAT = TRUE;
				break;
			case StunTypeBlocked:
				LOG(LOG_DEBUG, "Blocked or could not reach STUN server\n");
				break;
			case StunTypeIndependentFilter:
				LOG(LOG_DEBUG, "Independent Mapping, Independent Filter\n");
				bNAT = TRUE;
				break;
			case StunTypeDependentFilter:
				LOG(LOG_DEBUG, "Independent Mapping, Address Dependent Filter\n");
				bNAT = TRUE;
				break;
			case StunTypePortDependedFilter:
				LOG(LOG_DEBUG, "Independent Mapping, Port Dependent Filter\n");
				bNAT = TRUE;
				break;
			case StunTypeDependentMapping:
				LOG(LOG_DEBUG, "Dependent Mapping\n");
				bNAT = TRUE;
				break;
			default:
				LOG(LOG_DEBUG, "Unknown NAT type\n");
				bNAT = TRUE;
				break;
		}
	}
	
	// In the NAT environment case
	if (bNAT)
	{
		LOG(LOG_INFO, "STUN connect success!!\n");
		// Open STUN Socket 
		fd = stunOpenSocket(&STUNAddr_Server, &STUNAddr_Mapped, 0, 0, bVerbose);
		if (fd == INVALID_SOCKET) 
		{
			LOG(LOG_ERROR, "Can't open a socket to a destination\n");
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}
		else 
		{
			addr.s_addr = ntohl(STUNAddr_Mapped.addr);
			LOG(LOG_DEBUG, "Mapped address : %s\n",inet_ntoa(addr));
			LOG(LOG_DEBUG, "Mapped port    : %d\n",STUNAddr_Mapped.port);
			sprintf(UDPConnReqAddr, "%s:%d", inet_ntoa(addr), STUNAddr_Mapped.port);
#ifdef WITH_TR098
			TR069_SetParameterValue("InternetGatewayDevice.ManagementServer.UDPConnectionRequestAddress", UDPConnReqAddr);
			TR069_SetParameterValue("InternetGatewayDevice.ManagementServer.NATDetected" , "true");
#elif WITH_TR106
			TR069_SetParameterValue("Device.ManagementServer.UDPConnectionRequestAddress", UDPConnReqAddr);
			TR069_SetParameterValue("Device.ManagementServer.NATDetected" , "true");
#endif
		}

		// Create TR111 Send UDP thread
		SendUDPThread.pfFnStartRoutine = (void *)TR111_SendUDP;
		SendUDPThread.pThreadParm = (void *)fd;
		iReturn = PLATFORM_ThreadCreate(&SendUDPThread);
		if (iReturn != _SUCCESS)
		{
			LOG(LOG_DEBUG, "Send UDP thread creation failed !\n");
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}

		// Create TR111 Get UDP thread
		GetUDPThread.pfFnStartRoutine = (void *)TR111_GetUDP;
		GetUDPThread.pThreadParm = (void *)fd;
		iReturn = PLATFORM_ThreadCreate(&GetUDPThread);//, (void *)TR111_GetUDP, (void *)iSocketfd);
		if (iReturn != _SUCCESS)
		{
			LOG(LOG_DEBUG, "Get UDP thread creation failed !\n");
			PLATFORM_ThreadCancel(&SendUDPThread);			
			iReturn = _ERROR;
			goto FUNC_EXIT;
		}
		//LOG(LOG_DEBUG, "Get UDP thread ID :%d \n", (int)g_getUDP_thread.hThread);
	}
	else  
	{
		LOG(LOG_DEBUG, "Connect STUN server fail...\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

FUNC_EXIT:
	return iReturn;
}

int TR111_Release()
{
	bSendUDPExit = TRUE;
	bGetUDPExit = TRUE;
	return _SUCCESS;
}

