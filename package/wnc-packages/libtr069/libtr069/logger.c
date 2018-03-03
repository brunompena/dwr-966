#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "Common.h"
#include "logger.h"

extern unsigned int ENABLE_VERBOSE;

#ifdef _JNR_ES_
extern unsigned int ENABLE_DBG;
#endif

static PrintOut(char* szMsg)
{
	printf("%s", szMsg);
}

void LOG(unsigned int level, char* format, ...)
{
	char  szMsg[MAXLOGSIZE], szDBGMsg[MAXLOGSIZE + 16];
	va_list  marker;
	va_start(marker, format);
	memset(szMsg, 0, sizeof(szMsg));
	memset(szDBGMsg, 0, sizeof(szDBGMsg));
	vsnprintf(szMsg, MAXLOGSIZE, format, marker);
#ifdef _JPR_AM_
	if (Method.SaveLogCallBackFunc != NULL)//---have plugfunc
	{
		sprintf(szDBGMsg,"%s",szMsg);
		Method.SaveLogCallBackFunc(level, szDBGMsg);
	} else
#endif
	switch (level)
	{
	case LOG_DEBUG:
#ifdef _JNR_ES_
		if (ENABLE_VERBOSE == TRUE && ENABLE_DBG == TRUE)
#else
		if (ENABLE_VERBOSE == TRUE)
#endif
		{
			sprintf(szDBGMsg,"[DEBUG] %s",szMsg);
			PrintOut(szDBGMsg);
		}
		break;
	case LOG_INFO:
#ifdef _JNR_ES_
		if (ENABLE_DBG == TRUE)
#endif
		{
			sprintf(szDBGMsg,"[INFO] %s",szMsg);
			PrintOut(szDBGMsg);
		}
		break;
	case LOG_WARNING:
#ifdef _JNR_ES_
		if (ENABLE_VERBOSE == TRUE && ENABLE_DBG == TRUE)
#else
		if (ENABLE_VERBOSE == TRUE)
#endif
		{
			sprintf(szDBGMsg,"[WARNING] %s",szMsg);
			PrintOut(szDBGMsg);
		}
		break;
	case LOG_ERROR:
#ifdef _JNR_ES_
		if (ENABLE_DBG == TRUE)
#endif
		{
		sprintf(szDBGMsg,"[ERROR] %s",szMsg);
		PrintOut(szDBGMsg);
		}
		break;
	}
}
