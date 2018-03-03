/* adpCmdLib.c - provides api to work with command execution */

/* Copyright (c) 2009, TeamF1, Inc. */

/*
modification history
--------------------
01f,15apr09,sdk  added facility to reap a process.
01e,19dec08,pnm  Added adpCmdExecArgv routine.
01d,15jul08,rnp  added 'Invalid Process Id' string to ADPCMD_ERROR_STRINGS
01c,29jan07,smv  waiting based on PID (changes suggested by Rajendra).
01b,17jun06,chd  added support for QUOTE
01a,12feb06,rnp  written
*/

/*
DESCRIPTION
This file implements api to work with command execution for linux
*/

/* includes */
#undef ADPCMD_DEBUG
#include "adpCmdLib.h"

/* defines */
#define SPACE   ' '     /* the space character */
#define QUOTE   '"'     /* the double-quotes character */

#ifdef ADPCMD_DEBUG
#define adpdprintf(x...) fprintf(stderr,x)
#define adpddprintf(x...)
#else
#define adpdprintf(x...)
#define adpddprintf(x...)
#endif

#undef ADP_PRINTF
#define ADP_PRINTF(x...)

#define ADPCMD_RET_OK                    ADP_RET_OK(adpCmdRetOk)
#define ADPCMD_RET_ERROR(errCode)        ADP_RET_ERROR(errCode, adpCmdRetError)
#define ADPCMD_RET_NESTED_ERROR()        ADP_RET_NESTED_ERROR(adpCmdRetError)
#define ADPCMD_RET_SYSTEM_ERROR(sysCode) \
            ADP_RET_SYSTEM_ERROR(sysCode, adpCmdRetError)

#define adpCmdSpawnPid adpCmdExecStatus

#define ADPCMD_ERROR_STRINGS     \
    {                            \
    "Invalid Argument",          \
    "No Tokens",                 \
    "No Memory",                 \
    "Failed To Execute Command", \
    "Command Returned Error" ,   \
    "Command Aborted",           \
    "Invalid Process Id",        \
    }

/* globals */
ADP_DEFINE_ERROR_TABLE(adpCmdRetError, ADPCMD_ERROR_STRINGS);
int adpCmdExecStatus;

/* imports */

/* locals */

/*******************************************************************************
*
* adpStrBreak - tokenize a string based on spaces into an argv[] array for main
*
* This routine breaks a string using spaces as delimiters between tokens and
* returns an array of strings suitable for adpCmdExec(). The array must be
* freed later using adpFree().
*
* RETURNS: pointer to array of strings or NULL
*/
char ** adpStrBreak 
    (
    char * pString
    )
    {
    char ** ppArray = NULL;
    char * pPtr = pString;
    int tokenCount = 0;
    int i = 0;
    int quote = 0;
        
    if (pString == NULL)
        ADPCMD_RET_ERROR(ADPCMD_ERR_INVALID_ARGUMENT);

    /* count number of tokens separated by spaces. skip spaces within quotes */
    while (1)
        {
        char c = *pPtr;
        char prev = *(pPtr-1);
        if (c == SPACE && prev != SPACE && quote == 0)
            tokenCount++;
        if (c == QUOTE)
            quote = ~quote;
        if (c == 0)
            {
            tokenCount++;
            break;
            }
        pPtr++;
        }

    if (quote != 0)
        {
        /* all text beginning at an unbalanced quote will be single token */
        adpddprintf("adpStrBreak: unbalanced quote found\n");
        quote = 0;
        }

    adpddprintf("adpStrBreak: %d tokens found\n", tokenCount);
    if (tokenCount == 0)
        ADPCMD_RET_ERROR(ADPCMD_ERR_NO_TOKENS);

    /* 
     * allocate an array of pointers. 
     * number of pointers = number of tokens + termination null pointer 
     */
    ppArray = adpMalloc((tokenCount + 1) * sizeof(char *));
    if (ppArray == NULL)
        ADPCMD_RET_ERROR(ADPCMD_ERR_NO_MEM);

    memset(ppArray, 0, (tokenCount + 1) * sizeof(char *));

    /* allocate memory for each pointer and copy words skipping all spaces */
    pPtr = pString;
    while (i < tokenCount)
        {
        char * pStart = NULL;
        char * pEnd = NULL;
        while (1)
            {
            if (pStart == NULL && (*pPtr) != SPACE && (*pPtr != QUOTE))
                {
                pStart = pPtr;
                }
            if ((*pPtr == SPACE && pStart != NULL && quote == 0) || *pPtr == 0
                || (*pPtr == QUOTE && pStart != NULL && quote != 0))
                {
                /* + 1 for null-termination */
                pEnd = pPtr + 1;
                break;
                }
            /* we're in quoted text */
            if (*pPtr == QUOTE && quote == 0)
                quote = ~quote;
            pPtr++;
            }
        
        adpddprintf("adpStrBreak: token[%d] is of length %d\n", 
                   i, pEnd - pStart - 1);
        ppArray[i] = adpMalloc(pEnd - pStart);
        if (ppArray[i] == NULL)
            ADPCMD_RET_ERROR(ADPCMD_ERR_NO_MEM);
        memcpy(ppArray[i], pStart, pEnd - pStart);
        ppArray[i][pEnd - pStart - 1] = 0;
        adpddprintf("adpStrBreak: token[%d]: '%s'\n", i, ppArray[i]);
        i++;
        }

    return ppArray;

    adpCmdRetError:
        {
        if (ppArray != NULL)
            {
            int i = tokenCount;
            while (i--)
                adpFree(ppArray[i]);
            adpFree(ppArray);
            }
        return NULL;
        }
    }

/*******************************************************************************
*
* adpCmdExec - execute or spawn a command
* 
* This routine is used to execute or spawn a command
*
* RETURNS: ST_OK or ERROR
*/
STATUS adpCmdExec
    (
    char * pCmdString,   /* command string */
    char * pOutFile,     /* output file redirection */
    int    spawn         /* spawn the command */
    )
    {
    char ** ppArgArray = NULL;
    int cmdStatus = 1;
    int cmdOutput;
    STATUS result = ST_ERROR;
    pid_t pid;

    adpdprintf("adpCmdExec: exec'ing -> %s\n", pCmdString);
    ppArgArray = adpStrBreak(pCmdString);
    if (ppArgArray == NULL)
        ADPCMD_RET_NESTED_ERROR();
    pid = vfork();
    switch (pid)
        {
        case -1:
            {
            ADPCMD_RET_SYSTEM_ERROR(ADP_FORK);
            break;
            }
        case 0:
            {
            int fd;
            adpClose(STDOUT_FILENO);
            dup2(STDERR_FILENO, STDOUT_FILENO);
            /* open a file to redirect stdout */
            if (pOutFile != NULL)
                {
                cmdOutput = adpOpen(pOutFile, O_CREAT | O_RDWR | O_TRUNC, 
                                    S_IRUSR | S_IWUSR);
                adpClose(STDOUT_FILENO);
                dup2(cmdOutput, STDOUT_FILENO);
                dup2(cmdOutput, STDERR_FILENO);
                adpClose(cmdOutput);
                /* now <pOutFile> is stdout */
                }

            /* close all descriptors except 0,1,2 */
            for (fd = 3; fd <= 1024; fd++)
                adpClose(fd);

            result = execvp(ppArgArray[0], ppArgArray);
            if (result < 0)
                {
                adpdprintf("adpCmdExec: Failed(%s) exec'ing -> %s\n", 
                           pCmdString, strerror(errno));
                /* after a vfork() _exit() should be used instead of exit() */
                _exit(1);
                }
            /* exec doesnt return and this is never reached */
            }
        default:
            {
            /* parent will not wait when <spawn> is set */
            if (spawn == ADP_CMD_SPAWN)
                {
                result = ST_OK;
                adpCmdSpawnPid = pid;
                ADPCMD_RET_ERROR(ADPCMD_ERR_CMD_EXEC);
                }
            /* parent will wait for child to exit */
            result = waitpid (pid, &cmdStatus, 0);
            adpdprintf("adpCmdExec: cmdStatus: %#x <- '%s'\n", 
                        cmdStatus, pCmdString);
            if (result < 0)
                ADPCMD_RET_SYSTEM_ERROR(ADP_WAIT);
            }
        }

    result = ST_ERROR;
    /* check the exit status of the child process */
    if (WIFEXITED(cmdStatus))
        {
        cmdStatus = WEXITSTATUS(cmdStatus); 
        if (cmdStatus != 0)
            {
            adpCmdExecStatus = cmdStatus;
            ADPCMD_RET_ERROR(ADPCMD_ERR_CMD_ERROR);
            }
        result = ST_OK;
        }
    else
        {
        adpCmdExecStatus = cmdStatus;
        ADPCMD_RET_ERROR(ADPCMD_ERR_CMD_ABORTED);
        }

    adpCmdRetError:
        {
        if (ppArgArray != NULL)
            {
            int i = 0;
            while (ppArgArray[i] != NULL)
                {
                adpFree(ppArgArray[i]);
                i++;
                }
            adpFree(ppArgArray);
            }
        return result;
        }
    }

/*******************************************************************************
*
* adpCmdKill - kill a command that was spawned earlier
* 
* This routine is used to terminate a running command. It uses a commonly used
* sequence of sending a SIGTERM, followed by a one second pause and issuing a
* SIGKILL to ensure the spawned command(process) is killed.
*
* RETURNS: ST_OK or ERROR
*/
STATUS adpCmdKill
    (
    int    cmdId,        /* command identifier */
    int    wait          /* wait for the program to terminate(not yet imple.) */
    )
    {
    pid_t pid = cmdId;
    STATUS result = ST_ERROR;

    if (pid <= 0)
        ADPCMD_RET_ERROR(ADPCMD_ERR_INVALID_CMDID);

    adpdprintf("adpCmdKill: killing -> %d\n", cmdId);

    errno = 0;
    while(result < 0 && errno != ESRCH)
        {
        result = kill(pid, SIGTERM);
        if (wait == ADP_CMD_KILL_DONTWAIT)
            break;
        sleep(1);
        adpdprintf("adpCmdKill: Trying to kill again -> %d\n", cmdId);
        }

    /* we're done if process didnt exist */
    if (result < 0 && errno == ESRCH)
        ADPCMD_RET_OK;
    else if (result < 0)
        ADPCMD_RET_SYSTEM_ERROR(ADP_KILL);

    sleep(1);

    /* 
     * this one can fail if above kill did the job. Besides, if this also 
     * fails to kill the proces, we no recourse anyways. So we ignore 
     * the result 
     */
    kill(pid, SIGKILL);

    adpCmdRetOk:
        {
        result = ST_OK;
        }
    adpCmdRetError:
        {
        return result;
        }
    }


/*******************************************************************************
*
* adpCmdReap - reap a process spawned by adpCmdExec
* 
* This routine is used to reap a process spawned by adpCmdExec after the process
* has ended
*
* RETURNS: ST_OK or ERROR
*/
STATUS adpCmdReap
    (
    int    cmdId        /* command identifier */
    )
    {
    pid_t pid = cmdId;
    STATUS result = ST_ERROR;
    int status = 0;

    if (pid <= 0)
        ADPCMD_RET_ERROR(ADPCMD_ERR_INVALID_CMDID);

    adpdprintf("adpCmdReap: reaping-> %d\n", cmdId);

    result = waitpid(pid, &status, 0);

    adpdprintf("adpCmdReap: cmdStatus: %#x\n", 
               status);

    if (result < 0)
        ADPCMD_RET_SYSTEM_ERROR(ADP_REAP);

    adpCmdRetError:
        {
        return result;
        }
    }

/*******************************************************************************
*
* adpCmdExecArgv - execute or spawn a command using the argv array passed in
* 
* This routine is used to execute or spawn a command using the argv array
* passed in.
*
* RETURNS: ST_OK or ERROR
*/
STATUS adpCmdExecArgv
    (
    char ** argvAry,    /* command string */
    char *  pOutFile,   /* output file redirection */
    int     spawn       /* spawn the command */
    )
    {
    char ** ppArgArray = argvAry;
    int cmdStatus = 1;
    int cmdOutput;
    STATUS result = ST_ERROR;
    pid_t pid;
    
    if (ppArgArray == NULL)
        ADPCMD_RET_NESTED_ERROR();

    adpdprintf("adpCmdExecArgv: exec'ing -> %s\n", pCmdString);
    pid = vfork();
    switch (pid)
        {
        case -1:
            {
            ADPCMD_RET_SYSTEM_ERROR(ADP_FORK);
            break;
            }
        case 0:
            {
            int fd;
            adpClose(STDOUT_FILENO);
            dup2(STDERR_FILENO, STDOUT_FILENO);
            /* open a file to redirect stdout */
            if (pOutFile != NULL)
                {
                cmdOutput = adpOpen(pOutFile, O_CREAT | O_RDWR | O_TRUNC, 
                                    S_IRUSR | S_IWUSR);
                adpClose(STDOUT_FILENO);
                dup2(cmdOutput, STDOUT_FILENO);
                dup2(cmdOutput, STDERR_FILENO);
                adpClose(cmdOutput);
                /* now <pOutFile> is stdout */
                }

            /* close all descriptors except 0,1,2 */
            for (fd = 3; fd <= 1024; fd++)
                adpClose(fd);

            result = execvp(ppArgArray[0], ppArgArray);
            if (result < 0)
                {
                adpdprintf("adpCmdExec: Failed(%s) exec'ing -> %s\n", 
                           pCmdString, strerror(errno));
                /* after a vfork() _exit() should be used instead of exit() */
                _exit(1);
                }
            /* exec doesnt return and this is never reached */
            }
        default:
            {
            /* parent will not wait when <spawn> is set */
            if (spawn == ADP_CMD_SPAWN)
                {
                result = ST_OK;
                adpCmdSpawnPid = pid;
                ADPCMD_RET_ERROR(ADPCMD_ERR_CMD_EXEC);
                }
            /* parent will wait for child to exit */
            result = waitpid (pid, &cmdStatus, 0);
            adpdprintf("adpCmdExec: cmdStatus: %#x <- '%s'\n", 
                        cmdStatus, pCmdString);
            if (result < 0)
                ADPCMD_RET_SYSTEM_ERROR(ADP_WAIT);
            }
        }

    result = ST_ERROR;
    /* check the exit status of the child process */
    if (WIFEXITED(cmdStatus))
        {
        cmdStatus = WEXITSTATUS(cmdStatus); 
        if (cmdStatus != 0)
            {
            adpCmdExecStatus = cmdStatus;
            ADPCMD_RET_ERROR(ADPCMD_ERR_CMD_ERROR);
            }
        result = ST_OK;
        }
    else
        {
        adpCmdExecStatus = cmdStatus;
        ADPCMD_RET_ERROR(ADPCMD_ERR_CMD_ABORTED);
        }

    adpCmdRetError:
        {
        return result;
        }
    }

