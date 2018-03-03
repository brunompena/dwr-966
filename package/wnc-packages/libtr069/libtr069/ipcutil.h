#ifndef __IPCUTIL_H__
#define __IPCUTIL_H__

struct ReqForm{
	unsigned int Method;
	char Name[MAXSTRINGSIZE];
	char Value[MAXSTRINGSIZE];
};

struct RespForm{
	int iReturn;
	char Name[MAXSTRINGSIZE];
	char Value[MAXSTRINGSIZE];
};

int IPC_StartDaemon();
int IPC_CreateSocket();
int IPC_CloseSocket(int sockfd);
int IPC_SendRequest(int sockfd, struct ReqForm* Req);
int IPC_RecvResponse(int sockfd, struct RespForm* Resp);

#endif
