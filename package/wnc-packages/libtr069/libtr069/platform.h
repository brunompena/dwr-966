#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sched.h>
#include <pthread.h>

struct platformThread_t{
	pthread_t hThread;
	void* (*pfFnStartRoutine)(void *);
	void* pThreadParm;
};

int PLATFORM_SetSignal(int iSigNum, void (*pFnSignalHandler)(int));
void PLATFORM_KillProcess(void);
int PLATFORM_CreateTimer(unsigned long ulTriggerSec, unsigned long ulIntervalSec);
void PLATFORM_SleepSec(unsigned int uiSecond);
void PLATFORM_SleepMSec(unsigned int uiMilliSecond);
int PLATFORM_ThreadCreate(struct platformThread_t* tPlatform);//, void *(*pFnStartRoutine)(void *), void *pThreadParm);
int PLATFORM_ThreadCancel(struct platformThread_t* tPlatform);
int PLATFORM_FileDownload(char *pszDownloadTarget, char *pszUserName, char *pszPasswd);
void PLATFORM_Mutex_lock(pthread_mutex_t* hMutex);
void PLATFORM_Mutex_unlock(pthread_mutex_t* hMutex);
