/* testComp.c - test component to test UMI apis */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01a,28feb05,rks written.
*/

/*
DESCRIPTION
This file implements routines to implement a test component. Test component
register its ioctl handler to umi and process the umi ioctl requests.
*/

/* includes */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/string.h>

#include <umi.h>

/* defines */


/* typedefs */


/* externs */


/* locals */

static void * pCookie = NULL;

static int  testType = 1;
static int  destId   = 0;

MODULE_PARM (testType,"i");
MODULE_PARM_DESC(testType, "1 to wait for request, 2 for send the request");

MODULE_PARM (destId, "i");
MODULE_PARM_DESC(destId, "destination component id for testType value 2");

/* globals */


/* imports */


/* forward declarations */


/*******************************************************************************
*
* testCompHandler - component test handler
*
* This is component test handler. This copies the pOutBuf to pInBuf to test if 
* results are getting reflected in the caller.
*
* RETURNS: 0
*/

int testCompHandler
    (
    void *          arg1,
    UMI_REQ_INFO *  pUmiReq,
    int             cmd,
    void *          pInBuf,
    unsigned int    inLen,
    void *          pOutBuf,
    unsigned int *  pOutLen
    )
    {
    /* this handler just prints the message */

    printk (KERN_ERR "testCompHandler: received %s from %d\n", (char *)pInBuf,
                      pUmiReq->srcId);

    /* 
     * copy pOutBuf to pInBuf to test if results are getting reflected in the
     * caller.
     */

    if (inLen > *pOutLen)
        {
        inLen = *pOutLen;
        }
    memcpy(pInBuf, pOutBuf, inLen);
    
    strncpy(pOutBuf, "message from kernel test component",*pOutLen);
    return (0);
    }
    
/*******************************************************************************
*
* testCompLoad - loads the test component.
*
* This registers the test component with UMI.
*
* RETURNS: 0 for success , -1 for error
*/

static int __init testCompLoad (void)
    {
    
    if (testType == 1)
        {
        pCookie = umiRegister ( UMI_COMP_TEST, UMI_OPT_ANY_TASK_CTX, 
                                testCompHandler,
                                NULL);
        }
    else if (testType == 2 && destId != 0)
        {
        char inp[]="request from kernel test component";
        char outp[]="output message";
        int  outpLen = strlen(outp) + 1;
        
        umiIoctl (NULL, destId, 0 , inp, sizeof(inp), outp, &outpLen,0,0,0);
        }
    else
        {
        return -1;
        }
    return (0);
    }


/*******************************************************************************
*
* testCompUnLoad - unloads the test component.
*
* This unregisters the test component from UMI.
*
* RETURNS: N/A
*/

static void  __exit testCompUnLoad (void)
    {
    if (pCookie)
        {
        umiUnRegister (pCookie);
        }
    }
/* Module entry/exit declaration */
module_init(testCompLoad);
module_exit(testCompUnLoad);

