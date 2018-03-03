#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

long get_uptime();
char * getSplitStr(char * pSources, char * pSep, int nIdx);
char * getSplitStr2(char * pSources, char * pSep, int nIdx);

// get up time in seconds
long get_uptime()
{
    struct sysinfo s_info;
    int error;
    error = sysinfo(&s_info);
    if(error != 0)
    {
        printf("code error = %d\n", error);
    }

    return s_info.uptime;
}

char * getSplitStr(char * pSources, char * pSep, int nIdx)
{
    char * pRet = (char *)malloc(24);
    char * temp1;
    char *temp2 = (char *)malloc(128);
    strcpy(temp2,pSources);
    temp1 = strtok (temp2, pSep);
    int count = 1;
    while (temp1 != NULL)
    {
        if (count == nIdx)
            strcpy(pRet,temp1);

        temp1 = strtok (NULL, pSep);
        count++;
    }
    return pRet;
}

char * getSplitStr2(char * pSources, char * pSep, int nIdx)
{
	int pSourcesLen = strlen(pSources);
    char * temp1;
    char *temp2 = (char *)malloc(pSourcesLen);
    strcpy(temp2,pSources);
    temp1 = strtok (temp2, pSep);
    int count = 1;
    while (temp1 != NULL)
    {
        if (count == nIdx)
        {
        	return temp1;
        }
        temp1 = strtok (NULL, pSep);
        count++;
    }
	return "";
}

/*
 * description: parse va and do system
 */
int doSystem(char_t *fmt, ...)
{
	va_list		vargs;
	char_t		*cmd = NULL;
	int			rc = 0;

	va_start(vargs, fmt);
	if (fmtValloc(&cmd, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
		trace(0, T("doSystem: lost data, buffer overflow\n"));
	}
	va_end(vargs);

	if (cmd) {
		trace(0, T("%s\n"), cmd);
		rc = system(cmd);
		bfree(B_L, cmd);
	}
	return rc;
}

int showSystemCommandASP(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[1024];

	fp = fopen(SYSTEM_COMMAND_LOG, "r");
	if(!fp){
		websWrite(wp, T(""));
		return 0;
	}

	while(fgets(buf, 1024, fp)){
		websWrite(wp, T("%s"), buf);
	}
	fclose(fp);

	return 0;
}
