#include "logger.h"
#include "platform.h"

int PLATFORM_SetSignal(int iSigNum, void (*pFnSignalHandler)(int))
{
	if (pFnSignalHandler == SIG_DFL)
	{
		(void) signal(iSigNum, SIG_DFL);
		return 0;
	}
	else if (pFnSignalHandler != NULL)
	{
		(void) signal(iSigNum, pFnSignalHandler);
		return 0;
	}
	else
		return -1;
}

void PLATFORM_KillProcess()
{
	kill(getpid(),SIGINT);
}

int PLATFORM_CreateTimer(unsigned long ulTriggerSec, unsigned long ulIntervalSec)
{
	struct itimerval itv, oldtv;
	
	// it_value: timeout to run function first time
	itv.it_value.tv_sec = ulTriggerSec;     // sec.
	itv.it_value.tv_usec = 0;               // micro sec.
	// it_interval: interval time to run function
	itv.it_interval.tv_sec = ulIntervalSec; // sec.
	itv.it_interval.tv_usec = 0;            // micro sec.

	// set timer, ITIMER_REAL: real-time to decreate timer send
	// SIGALRM when timeout
	if (setitimer(ITIMER_REAL, &itv, &oldtv) != 0)
	{
		perror("setitimer");
		return -1;
	}
	else
		return 0;
}

void PLATFORM_SleepSec(unsigned int uiSecond)
{
	sleep(uiSecond);
}

void PLATFORM_SleepMSec(unsigned int uiMilliSecond)
{
	usleep(uiMilliSecond * 1000);
}

int PLATFORM_ThreadCreate(struct platformThread_t* tPlatform)//, void *(*pFnStartRoutine)(void *), void *pThreadParm)
{
	int iReturn = 0;
	iReturn = pthread_create(&tPlatform->hThread, NULL, tPlatform->pfFnStartRoutine, tPlatform->pThreadParm);
	if (iReturn != 0)
	{
		LOG(LOG_DEBUG, "PLATFORM_ThreadCreate is fail!!\n");
		return -1;
	}
	else{
		pthread_detach(tPlatform->hThread);
	    return 0;
	}
}

int PLATFORM_ThreadCancel(struct platformThread_t* tPlatform)
{
	int iReturn = 0;
	iReturn = pthread_cancel(tPlatform->hThread);
	if (iReturn != 0)
		return -1;
	else
		return 0;
}

int PLATFORM_FileDownload(char *pszDownloadTarget, char *pszUserName, char *pszPasswd)
{
	return 0;
}

void PLATFORM_Mutex_lock(pthread_mutex_t* hMutex)
{
	pthread_mutex_lock(hMutex);
}

void PLATFORM_Mutex_unlock(pthread_mutex_t* hMutex)
{
	pthread_mutex_unlock(hMutex);
}
