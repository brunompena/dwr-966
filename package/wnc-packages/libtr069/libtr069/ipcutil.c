#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include "dmutil.h"
#include "logger.h"
#include "platform.h"
#include "ipcutil.h"
#include "Common.h"

static void* StartUpDataModelService(void *vParam)
{
	int iReturn = _SUCCESS, addr_size, numbytes;
	int sockfd, cli_fd;
	BYTE RecvBuf[MAXRECVSIZE];
	char szValue[MAXSTRINGSIZE];
	struct sockaddr_un svr_addr, cli_addr;
	struct ReqForm Req;
	struct RespForm Resp;
	int *bWaitFlag = (int*)vParam;

	svr_addr.sun_family = AF_UNIX;
	strcpy(svr_addr.sun_path, DM_SERVICE_NAME);
	unlink(svr_addr.sun_path);

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET)
	{
		LOG(LOG_ERROR, "Create DataModel socket failure.\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}

	if (bind(sockfd, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) == _ERROR)
	{
		LOG(LOG_ERROR, "Bind DataModel address failure.\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
		 
	if (listen(sockfd, 10) == _ERROR)
	{
		LOG(LOG_ERROR, "Prepare DataModel listen failure.\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	
	*bWaitFlag = FALSE;//ready for request

	while(TRUE)
	{
		memset(&cli_addr, 0, sizeof(cli_addr));
		addr_size = sizeof(cli_addr);
        	cli_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &addr_size);
		if (!fork())
		{
			numbytes = recv(cli_fd, RecvBuf, sizeof(struct ReqForm), 0);

			if (numbytes > 0)
			{
				memset(&Req, 0, sizeof(struct ReqForm));
				memcpy(&Req, RecvBuf, sizeof(RecvBuf));

				memset(&Resp, 0, sizeof(struct RespForm));
				if (Req.Method == DM_GET)
				{
					Resp.iReturn = TR069_GetParameterValue(Req.Name, szValue, sizeof(szValue));
					strncpy(Resp.Name, Req.Name, sizeof(Resp.Name));
					strncpy(Resp.Value, szValue, sizeof(szValue));
				}
				else if (Req.Method == DM_SET)
				{
					Resp.iReturn = TR069_SetParameterValue(Req.Name, Req.Value);
				}
				numbytes = send(cli_fd, (const char*)&Resp, sizeof(struct RespForm), 0);
			}
			close(cli_fd);
			exit(0);
		}
		close(cli_fd);
	}
FUNC_EXIT:
	return;
}

int IPC_StartDaemon()
{
	struct platformThread_t Platform;
#ifdef _JPR_AM_
	volatile int bWaitFlag = TRUE;
#else
	int bWaitFlag = TRUE;
#endif
	Platform.pThreadParm = (void*)&bWaitFlag;
	Platform.pfFnStartRoutine = StartUpDataModelService;
	PLATFORM_ThreadCreate(&Platform);
	while(bWaitFlag == TRUE);//wait for daemon
	return _SUCCESS;
}

int IPC_CreateSocket()
{
	int sockfd;
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET)
	{
		LOG(LOG_ERROR, "Create socket failure.\n");
		return _ERROR;
	}
	return sockfd;
}

int IPC_CloseSocket(int sockfd)
{
	return close(sockfd);
}

int IPC_SendRequest(int sockfd, struct ReqForm* Req)
{
	int iReturn = _SUCCESS, len;
	struct sockaddr_un address;
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, DM_SERVICE_NAME);
	len = sizeof(address);

	if (connect(sockfd, (struct sockaddr*)&address, len) != _SUCCESS)
	{
		LOG(LOG_ERROR, "IPC_SendRequest..connect fail\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	if (send(sockfd, (const char*)Req, sizeof(struct ReqForm), 0) <= 0)
	{
		LOG(LOG_ERROR, "IPC_SendRequest..send fail\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
FUNC_EXIT:
	return iReturn;
}

int IPC_RecvResponse(int sockfd, struct RespForm* Resp)
{
	int iReturn = _SUCCESS;
	char RecvBuf[MAXRECVSIZE];
	if (recv(sockfd, RecvBuf, sizeof(struct RespForm), 0) <= 0)
	{
		LOG(LOG_ERROR, "IPC_RecvResponse.. recv fail\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	memset(Resp, 0, sizeof(Resp));
	memcpy(Resp, RecvBuf, sizeof(RecvBuf));
FUNC_EXIT:
	return iReturn;
}

int TR069DB_SetParameterService(char* path, char* value)
{
	int iReturn = _SUCCESS;
	int sockfd;
	struct ReqForm Req;
	struct RespForm Resp;

	memset(&Req, 0, sizeof(Req));
	Req.Method = DM_SET;
	strncpy(Req.Name, path, strlen(path));
	strncpy(Req.Value, value, strlen(value));
	sockfd = IPC_CreateSocket();
	if (sockfd != _ERROR)
	{
		IPC_SendRequest(sockfd, &Req);
		IPC_RecvResponse(sockfd, &Resp);
	}
	else
	{
		LOG(LOG_ERROR, "TR069_SetParameterValue socket create fail.\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	iReturn = Resp.iReturn;

FUNC_EXIT:
	IPC_CloseSocket(sockfd);
	return iReturn;
}

int TR069DB_GetParameterService(char* path, char* value, size_t iLen)
{
	int iReturn = _SUCCESS;
	int sockfd;
	struct ReqForm Req;
	struct RespForm Resp;
 
	memset(&Req, 0, sizeof(Req));
	Req.Method = DM_GET;
	strncpy(Req.Name, path, strlen(path));
	sockfd = IPC_CreateSocket();
	if (sockfd != _ERROR)
	{
		IPC_SendRequest(sockfd, &Req);
		IPC_RecvResponse(sockfd, &Resp);
	}
	else
	{
		LOG(LOG_ERROR, "TR069_GetParameterValue socket create fail.\n");
		iReturn = _ERROR;
		goto FUNC_EXIT;
	}
	memset(value, 0, iLen);
	strncpy(value, Resp.Value, strlen(Resp.Value));
	iReturn = Resp.iReturn;

FUNC_EXIT:
	IPC_CloseSocket(sockfd);
	return iReturn;
}
