/* adpCryptoApi.c - Adaptos Crypto API */

/* Copyright (c) 2003 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01d,20jun08,rnp  modified the warning-fixes to avoid 80-col overrun
01c,07oct07,san  fixed compiler warning.  
01b,04jul03,ws   added documentation for hmac.
01a,02jun03,ws   written
*/

/*
DESCRIPTION

This module defines the Adaptos Crypto API. This API is used by the
application layer for all crypto functions, like encryption, decryption
and message digest calculation.

The functioning of the Adaptos Crypto API is dependent on the
implementation of crypto provider library. The crypto provider library
implementation is registered with the Crypto API by calling
adpCryptoApiLibInstall.


EXTERNAL INTERFACE
The functioning of Adaptos Crypto API relies on the implementation
of the following functions by the crypto provider library. For more
information check the documentation of the individual routines.

.CS
cipherContextCreate()   -   create cipher context
cipherContextInit() -   initialize cipher context
cipherContextCleanup()  -   clear cipher context
cipherContextDestroy()  -   destroy cipher context
cipherContextCtrl() -   get/set cipher context params
encryptInit()       -   initialize encryption process
encryptUpdate()     -   update encryption state with new data
encryptFinal()      -   complete encryption process
decryptInit()       -   initialize decryption process
decryptUpdate()     -   update decryption state with new data
decryptFinal()      -   complete decryption process
digestContextCreate()   -   create digest context
digestContextInit() -   initialize digest context
digestContextCleanup()  -   clear digeset context
digestContextDestroy()  -   destroy digest context
digestContextCtrl() -   get/set digest context params
digestGetDigestLen()    -   get digest len given digest name
digestInit()        -   initialize digest calculation
digestUpdate()      -   update digest state with new data
digestFinal()       -   finish digest calculation
hmacContextCreate() -   create hmac context
hmacContextInit()   -   initialize hmac context
hmacContextCleanup()    -   clear hmac context
hmacContextDestroy()    -   destroy hmac context
hmacContextCtrl()   -   get/set hmac context params
hmacInit()      -   initialize hmac calculation
hmacUpdate()        -   update hmac state with new data
hmacFinal()     -   finish hmac calculation
waitFunc()      -   implementation of blocking function
notifyFunc()        -   implementation of notify function
schedule()      -   funciton for scheduling crypto jobs
.CE

The above functions are exported to the calling function, by the crypto
provider library through CRYPTO_FUNC_TBL structure by the crypto provider
init function.

If the schedule function is not implemented by the crypto provider, the
crypto job is done synchronously. Otherwise, it is scheduled using the
schedule function.

If the waitFunc and notifyFunc is not implemented by the crypto provider,
semaphore based Adaptos Crypto API implementation of these functions
will be used.

INTERNAL

Here is a small ascii diagram that shows how the Adaptos Crypto API
fits in the overall crypto system.

          Applications
            |
            v
        Adaptos Crypto API
            |
            v
        crypto provider library
*/

#include "adaptos.h"

/* defines */


/* globals */


/* locals */
LOCAL STATUS adpBlockingFunc (ULONG arg1);
LOCAL void   adpWakeupFunc   (ULONG arg1);

/* initialize all the members of the following struct to NULL */
LOCAL CRYPTO_FUNC_TBL   cryptoFuncTbl;
LOCAL CRYPTO_FUNC_TBL * pCryptoFuncTbl = &cryptoFuncTbl;

/*******************************************************************************
*
* adpCryptoApiLibInstall - install crypto API library
*
* This routine installs the user specified crypto implementation.
*
* <pInitFunc>   - pointer to crypto provider init func
*
* <arg1>    - argument required by crypto provider init func
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpCryptoApiLibInstall
    (
    CRYPTO_PROV_INIT_FUNC   pInitFunc,  /* crypto provider init func */
    int             arg1        /* argument to init func */
    )
    {
    CRYPTO_FUNC_TBL *pFuncTbl = NULL;
    
    /* initialize the function table */
    memset (pCryptoFuncTbl, 0, sizeof(*pCryptoFuncTbl));
    
    if ((pFuncTbl = (*pInitFunc)(arg1)) == NULL)
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INIT_FUNC);
        return (ST_ERROR);
        }

    pCryptoFuncTbl->cipherContextCreate   = pFuncTbl->cipherContextCreate;
    pCryptoFuncTbl->cipherContextInit     = pFuncTbl->cipherContextInit;
    pCryptoFuncTbl->cipherContextCleanup  = pFuncTbl->cipherContextCleanup;
    pCryptoFuncTbl->cipherContextDestroy  = pFuncTbl->cipherContextDestroy;
    pCryptoFuncTbl->cipherContextCtrl     = pFuncTbl->cipherContextCtrl;
    pCryptoFuncTbl->encryptInit           = pFuncTbl->encryptInit;
    pCryptoFuncTbl->encryptUpdate         = pFuncTbl->encryptUpdate;
    pCryptoFuncTbl->encryptFinal          = pFuncTbl->encryptFinal;
    pCryptoFuncTbl->decryptInit           = pFuncTbl->decryptInit;
    pCryptoFuncTbl->decryptUpdate         = pFuncTbl->decryptUpdate;
    pCryptoFuncTbl->decryptFinal          = pFuncTbl->decryptFinal;
    pCryptoFuncTbl->digestContextCreate   = pFuncTbl->digestContextCreate;
    pCryptoFuncTbl->digestContextInit     = pFuncTbl->digestContextInit;
    pCryptoFuncTbl->digestContextCleanup  = pFuncTbl->digestContextCleanup;
    pCryptoFuncTbl->digestContextDestroy  = pFuncTbl->digestContextDestroy;
    pCryptoFuncTbl->digestContextCtrl     = pFuncTbl->digestContextCtrl;
    pCryptoFuncTbl->digestGetDigestLen    = pFuncTbl->digestGetDigestLen;
    pCryptoFuncTbl->digestInit            = pFuncTbl->digestInit;
    pCryptoFuncTbl->digestUpdate          = pFuncTbl->digestUpdate;
    pCryptoFuncTbl->digestFinal           = pFuncTbl->digestFinal;
    pCryptoFuncTbl->hmacContextCreate     = pFuncTbl->hmacContextCreate;
    pCryptoFuncTbl->hmacContextInit       = pFuncTbl->hmacContextInit;
    pCryptoFuncTbl->hmacContextCleanup    = pFuncTbl->hmacContextCleanup;
    pCryptoFuncTbl->hmacContextDestroy    = pFuncTbl->hmacContextDestroy;
    pCryptoFuncTbl->hmacContextCtrl       = pFuncTbl->hmacContextCtrl;
    pCryptoFuncTbl->hmacInit              = pFuncTbl->hmacInit;
    pCryptoFuncTbl->hmacUpdate            = pFuncTbl->hmacUpdate;
    pCryptoFuncTbl->hmacFinal             = pFuncTbl->hmacFinal;
    pCryptoFuncTbl->schedule              = pFuncTbl->schedule;
    if (pFuncTbl->schedule != NULL)
        {
        if (pFuncTbl->waitFunc != NULL)
            {
            pCryptoFuncTbl->waitFunc    = pFuncTbl->waitFunc;
            }
        else
            {
            pCryptoFuncTbl->waitFunc    = adpBlockingFunc;
            }
        if (pFuncTbl->notifyFunc != NULL)
            {
            pCryptoFuncTbl->notifyFunc  = pFuncTbl->notifyFunc;
            }
        else
            {
            pCryptoFuncTbl->notifyFunc  = adpWakeupFunc;
            }
    }
    
    return (ST_OK);
    }

/*******************************************************************************
*
* adpCipherContextCreate - create adaptos cipher context
*
* This function creates adaptos cipher context.
*
* <ctxId>   - adaptos cipher context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise
*/

STATUS adpCipherContextCreate 
    (
    ADP_CIPHER_CTX_ID ctxId /* cipher context */
    )
    {

    if (ctxId == NULL)
        {
    adpErrnoSet (ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->cipherContextCreate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    ctxId->provCtxId = (void *)(*(pCryptoFuncTbl->cipherContextCreate))();
    ctxId->encrypt = 0;
    ctxId->arg1 = (ULONG)0;

    if (ctxId->provCtxId == NULL)
        {
    adpErrnoSet (ADP_CRYPTO_ERR_MALLOC);
        return (ST_ERROR);  
        }

    return (ST_OK);
    }


/*******************************************************************************
*
* adpCipherContextInit - initialize adaptos cipher context
*
* This function initializes the specified adaptos cipher context
*
* <ctxId>   - cipher context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpCipherContextInit
    (
    ADP_CIPHER_CTX_ID   ctxId       /* cipher context */
    )
    {

    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->cipherContextInit == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->cipherContextInit))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }
   
    if (pCryptoFuncTbl->waitFunc == adpBlockingFunc)
        {
        /* initialize the semaphore */
        if ((ctxId->arg1 = (ULONG) adpSemBCreate(NULL, ADP_SEM_Q_PRIORITY |
                                         ADP_SEM_DEL_SAFE, 1)) == 0)
            {
            return (ST_ERROR);
            }
        }

    return (ST_OK);
    }


/*******************************************************************************
*
* adpCipherContextCleanup - cleanup adaptos cipher context
*
* This funtion cleans adaptos cipher context by freeing all the
* memory that was allocated during encryption/decryption process.
*
* <ctxId>   - cipher context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpCipherContextCleanup
    (
    ADP_CIPHER_CTX_ID   ctxId       /* cipher context */
    )
    {

    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->cipherContextCleanup == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->cipherContextCleanup))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }
    
    if (pCryptoFuncTbl->waitFunc == adpBlockingFunc)
        {
        /* delete the semaphore */
        if (adpSemBDelete ((ADP_SEM_ID)ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
    
    return (ST_OK);
    }


/*******************************************************************************
*
* adpCipherContextDestroy - destroy and free adaptos cipher context
*
* This function free the specified the adaptos cipher context
*
* <ctxId>   - cipher context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpCipherContextDestroy
    (
    ADP_CIPHER_CTX_ID   ctxId       /* adaptos cipher context */
    )
    {
      if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->cipherContextDestroy == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->cipherContextDestroy))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }

    ctxId->provCtxId = NULL;
    ctxId->arg1 = (ULONG)0;

    return (ST_OK);
    }


/*******************************************************************************
*
* adpCipherContextCtrl - control cipher specific parameters
*
* This function manipulates cipher parameters
*
* <ctxId>   - cipher context
*
* <type>    - operation to perform
*
* <arg>     - numerical value to set, if applicable
*
* <ptr>     - pointer for querying the current value, if applicable
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpCipherContextCtrl
    (
    ADP_CIPHER_CTX_ID   ctxId,  /* cipher context */
    int         type,   /* operation code */
    int         arg,    /* value to set, if applicable */
    void *      ptr /* current value, if applicable */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->cipherContextCtrl == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->cipherContextCtrl))(ctxId->provCtxId, type, arg,
                                               ptr) != ST_OK)
        {
        return (ST_ERROR);
        }
    
    return (ST_OK);
    }


/*******************************************************************************
*
* adpEncryptInit - initialize adaptos encryption context
*
* This function initializes the cipher
*
* <ctxId>   - cipher context
*
* <cipher>  - cipher to be used for encryption
*
* <engine>  - engine implementation (NULL for default)
*
* <key>     - encryption key
*
* <iv>      - initialization vector for cipher
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEncryptInit
    (
    ADP_CIPHER_CTX_ID       ctxId,  /* cipher context */
    unsigned char *         cipher, /* cipher to be used for encryption */
    void *          engine, /* ENGINE implementation */
    const unsigned char *   key,    /* encryption key */
    const unsigned char *   iv  /* initialization vector */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->encryptInit == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) cipher;
        schedParams.param3 = (ULONG) engine;
        schedParams.param4 = (ULONG) key;
        schedParams.param5 = (ULONG) iv;

        /* The scheduler should make a copy of the parameters passed to it */
        if((*pCryptoFuncTbl->schedule)((ADP_FUNCPTR)pCryptoFuncTbl->encryptInit,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->encryptInit))(ctxId->provCtxId, cipher,
                                             engine, key, iv) != ST_OK)
            {
            return (ST_ERROR);
            }
        }

    ctxId->encrypt = ADP_ENCRYPT;

    return (ST_OK);
    }


/*******************************************************************************
*
* adpEncryptUpdate - update adaptos encryption context
*
* This function updates the cipher context with the specified data
*
* <ctxId>   - cipher context
*
* <out>     - output buffer to store encrypted data
*
* <outl>    - returns the number of bytes written to data buffer
*
* <in>      - input buffer containing data to be encrypted
*
* <inl>     - input buffer length
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEncryptUpdate
    (
    ADP_CIPHER_CTX_ID       ctxId,  /* cipher context */
    unsigned char *     out,    /* buffer to store encrypted data */
    int *           outl,   /* length of data encrypted data */
    const unsigned char *   in, /* data to be encrypted */
    int             inl /* length of data to be encrypted */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->encryptUpdate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) out;
        schedParams.param3 = (ULONG) outl;
        schedParams.param4 = (ULONG) in;
        schedParams.param5 = (ULONG) inl;

        /* The scheduler should make a copy of the parameters passed to it */
        if(pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->encryptUpdate,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->encryptUpdate))(ctxId->provCtxId, out, 
                                               outl, in, inl) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
    
    return (ST_OK);
    }


/*******************************************************************************
*
* adpEncryptFinal - finish adaptos encryption
*
* This function finishes encryption process
*
* <ctxId>   - cipher context
*
* <out>     - output buffer to store encrypted data
*
* <outl>    - returns the number of bytes written to data buffer
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEncryptFinal
    (
    ADP_CIPHER_CTX_ID   ctxId,      /* cipher context */
    unsigned char * out,        /* buffer to store encrypted data */
    int *       outl        /* length of encrypted data */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->encryptFinal == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

    schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) out;
        schedParams.param3 = (ULONG) outl;

        /* The scheduler should make a copy of the parameters passed to it */
        if(pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->encryptFinal,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->encryptFinal))(ctxId->provCtxId, out,
                                              outl) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDecryptInit - initialize adaptos decryption context
*
* This function initializes decryption process
*
* <ctxId>   - cipher context
*
* <cipher>  - cipher to be used for decryption
*
* <engine>  - engine implementation (NULL for default)
*
* <key>     - decryption key
*
* <iv>      - initialization vector for cipher
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDecryptInit
    (
    ADP_CIPHER_CTX_ID       ctxId,  /* cipher context */
    unsigned char *         cipher, /* cipher to be used for decryption */
    void *          engine, /* ENGINE implementation */
    const unsigned char *   key,    /* decryption key */
    const unsigned char *   iv  /* initialization vector */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->decryptInit == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) cipher;
        schedParams.param3 = (ULONG) engine;
        schedParams.param4 = (ULONG) key;
        schedParams.param5 = (ULONG) iv;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->decryptInit,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->decryptInit))(ctxId->provCtxId, cipher,
                                             engine, key, iv) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
        
    ctxId->encrypt = ADP_DECRYPT;

    return (ST_OK);
    }

/*******************************************************************************
*
* adpDecryptUpdate - update adaptos decryption context
*
* This function updates the cipher context with the specified data
*
* <ctxId>   - cipher context
*
* <out>     - output buffer to store decrypted data
*
* <outl>    - returns the number of bytes written to data buffer
*
* <in>      - input buffer containing data to be decrypted
*
* <inl>     - input buffer length
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDecryptUpdate
    (
    ADP_CIPHER_CTX_ID       ctxId,  /* cipher context */
    unsigned char *     out,    /* buffer to store decrypted data */
    int *           outl,   /* length of decrypted data */
    const unsigned char *   in, /* buffer containing encrypted data */
    int             inl /* length of encrypted data */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->decryptUpdate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) out;
        schedParams.param3 = (ULONG) outl;
        schedParams.param4 = (ULONG) in;
        schedParams.param5 = (ULONG) inl;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->decryptUpdate,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->decryptUpdate))(ctxId->provCtxId, out,
                                               outl, in, inl) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDecryptFinal - finish adaptos decryption
*
* This function finishes the decryption process
*
* <ctxId>   - cipher context
*
* <out>     - output buffer to store decrypted data
*
* <outl>    - returns the number of bytes written to data buffer
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDecryptFinal
    (
    ADP_CIPHER_CTX_ID   ctxId,  /* cipher context */
    unsigned char * out,    /* buffer to store decrypted data */
    int *       outl    /* length of decrypted data */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->decryptFinal == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

    schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) out;
        schedParams.param3 = (ULONG) outl;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->decryptFinal,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->decryptFinal))(ctxId->provCtxId, out,
                                              outl) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
            
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestContextCreate - create adaptos digest context
*
* This function initializes adaptos digest context.
*
* <ctxId>   - adaptos digest context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise
*/

STATUS adpDigestContextCreate
    (
    ADP_DIGEST_CTX_ID ctxId /* adaptos digest context */
    )
    {

    if (ctxId == NULL)
        {
        adpErrnoSet (ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->digestContextCreate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    ctxId->provCtxId = (void *)(*(pCryptoFuncTbl->digestContextCreate))();

    if (ctxId->provCtxId == NULL)
        {
    adpErrnoSet (ADP_CRYPTO_ERR_MALLOC);
    return (ST_ERROR);
        }

    ctxId->arg1 = (ULONG)0;

    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestContextInit - initialize adaptos digest context
*
* This function initializes specified adaptos digest context
*
* <ctxId>   - digest context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestContextInit
    (
    ADP_DIGEST_CTX_ID ctxId /* digest context */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->digestContextInit == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->digestContextInit))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }
   
    if (pCryptoFuncTbl->waitFunc == adpBlockingFunc)
        {
        /* initialize the semaphore */
        if ((ctxId->arg1 = (ULONG) adpSemBCreate(NULL, ADP_SEM_Q_PRIORITY |
                                         ADP_SEM_DEL_SAFE, 1)) == 0)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestContextCleanup - cleanup adaptos digest context
*
* This function frees all the memory dynamically allocated during digest
* calculation process.
*
* <ctxId>   - digest context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestContextCleanup
    (
    ADP_DIGEST_CTX_ID ctxId /* digest context */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->digestContextCleanup == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->digestContextCleanup))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }
    
    if (pCryptoFuncTbl->waitFunc == adpBlockingFunc)
        {
        /* delete the semaphore */
        if (adpSemBDelete ((ADP_SEM_ID)ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestContextDestroy - destroy and free adaptos digest context
*
* This function frees the specified digest context
*
* <ctxId>   - digest context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestContextDestroy
    (
    ADP_DIGEST_CTX_ID ctxId /* adaptos digest context */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->digestContextDestroy == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->digestContextDestroy))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }

    ctxId->provCtxId = NULL;
    ctxId->arg1 = (ULONG)0;

    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestContextCtrl - control digest specific parameters
*
* This function controls digest parameters
*
* <ctxId>   - digest context
*
* <type>    - operation to perform
*
* <arg>     - numerical value to set, if applicable
*
* <ptr>     - pointer for querying the current value, if applicable
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestContextCtrl
    (
    ADP_DIGEST_CTX_ID   ctxId,  /* digest context */
    int         type,   /* operation code */
    int         arg,    /* value to set, if applicable */
    void *      ptr /* current value, if applicable */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->digestContextCtrl == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->digestContextCtrl))(ctxId->provCtxId, type, \
                                               arg, ptr) != ST_OK)
        {
        return (ST_ERROR);
        }
    
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestInit - initialize adaptos digest context object
*
* This funciton initializes the specified digest
*
* <ctxId>   - digest context
*
* <digest>  - message digest algorithm to be used
*
* <engine>  - engine implementation (NULL for default)
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestInit
    (
    ADP_DIGEST_CTX_ID   ctxId,  /* digest context */
    unsigned char * digest, /* message digest algorithm */
    void *      engine  /* ENGINE implementation to be used */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->digestInit == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) digest;
        schedParams.param3 = (ULONG) engine;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->digestInit,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->digestInit))(ctxId->provCtxId, digest,
                                            engine) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestUpdate - update adaptos digest context
*
* This function updates the digest context with the specified data
*
* <ctxId>   - digest context
*
* <buf>     - buffer containing data to be included in digest computation
*
* <len>     - length of data
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestUpdate
    (
    ADP_DIGEST_CTX_ID   ctxId,  /* digest context */
    unsigned char * buf,    /* data to be used in digest computation */
    unsigned int    len /* length of data */
    )
    {
   if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->digestUpdate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) buf;
        schedParams.param3 = (ULONG) len;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->digestUpdate,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->digestUpdate))(ctxId->provCtxId, buf, 
                                              len) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
            
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestFinal - finish adaptos digest calculation
*
* This funciton finishes the digest calculation process and returns the
* computed digest
*
* <ctxId>   - digest context
*
* <buf>     - buffer to store computed digest
*
* <len>     - length of the computed digest
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestFinal
    (
    ADP_DIGEST_CTX_ID   ctxId,  /* digest context */
    unsigned char * buf,    /* buffer to store computed digest */
    unsigned int *  len /* buffer to store length of digest */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->digestFinal == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) buf;
        schedParams.param3 = (ULONG) len;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->digestFinal,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->digestFinal))(ctxId->provCtxId, buf, len) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
                
    return (ST_OK);
    }


/*******************************************************************************
*
* adpDigestGetDigestLen - get digest length from digest name
*
* This function returns the digest length given the digest name
*
* <digestName>  - digest name
*
* <len>     - variable to return the length
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpDigestGetDigestLen
    (
    unsigned char * digestName, /* digest name */
    int *       len         /* buffer to store digest len */
    )
    {
    if ((digestName == NULL) || (len == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->digestGetDigestLen == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    if ((*(pCryptoFuncTbl->digestGetDigestLen))(digestName, len) != ST_OK)
        {
        return (ST_ERROR);
        }
                
    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacContextCreate - create adaptos hmac context
*
* This function creates adaptos Hmac context.
*
* <ctxId>   - adaptos hmac context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise
*/

STATUS adpHmacContextCreate
    (
    ADP_HMAC_CTX_ID ctxId   /* adaptos hmac context */
    )
    {

    if (ctxId == NULL)
        {
        adpErrnoSet (ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->hmacContextCreate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    ctxId->provCtxId = (void *)(*(pCryptoFuncTbl->hmacContextCreate))();

    if (ctxId->provCtxId == NULL)
        {
        adpErrnoSet (ADP_CRYPTO_ERR_MALLOC);
        return (ST_ERROR);
        }

    ctxId->arg1 = (ULONG)0;

    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacContextInit - initialize adaptos hmac context
*
* This funciton initializes the specified Hmac context
*
* <ctxId>   - hmac context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpHmacContextInit
    (
    ADP_HMAC_CTX_ID ctxId   /* hmac context */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->hmacContextInit == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->hmacContextInit))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }
   
    if (pCryptoFuncTbl->waitFunc == adpBlockingFunc)
        {
        /* initialize the semaphore */
        if ((ctxId->arg1 = (ULONG) adpSemBCreate(NULL, ADP_SEM_Q_PRIORITY |
                                         ADP_SEM_DEL_SAFE, 1)) == 0)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacContextCleanup - cleanup adaptos hmac context
*
* This function clears the specified Hmac context
*
* <ctxId>   - hmac context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpHmacContextCleanup
    (
    ADP_HMAC_CTX_ID ctxId   /* hmac context */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->hmacContextCleanup == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->hmacContextCleanup))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }
    
    if (pCryptoFuncTbl->waitFunc == adpBlockingFunc)
        {
        /* delete the semaphore */
        if (adpSemBDelete ((ADP_SEM_ID)ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacContextDestroy - destroy adaptos hmac context
*
* This function frees the specified the hmac context
*
* <ctxId>   - hmac context
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpHmacContextDestroy
    (
    ADP_HMAC_CTX_ID ctxId   /* adaptos hmac context */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->hmacContextDestroy == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->hmacContextDestroy))(ctxId->provCtxId) != ST_OK)
        {
        return (ST_ERROR);
        }

    ctxId->provCtxId = NULL;
    ctxId->arg1 = (ULONG)0;

    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacContextCtrl - control hmac specific parameters
*
* This function controls hmac parameters
*
* <ctxId>   - hmac context
*
* <type>    - operation to perform
*
* <arg>     - numerical value to set, if applicable
*
* <ptr>     - pointer for querying the current value, if applicable
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpHmacContextCtrl
    (
    ADP_HMAC_CTX_ID ctxId,  /* hmac context */
    int         type,   /* operation code */
    int         arg,    /* value to set, if applicable */
    void *      ptr /* current value, if applicable */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->hmacContextCtrl == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }
    
    if ((*(pCryptoFuncTbl->hmacContextCtrl))(ctxId->provCtxId, type, \
                                               arg, ptr) != ST_OK)
        {
        return (ST_ERROR);
        }
    
    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacInit - initialize adaptos hmac context object
*
* This function initializes the specified hmac
*
* <ctxId>   - hmac context
*
* <key>     - key to be in hmac calculation
* 
* <keyLen>  - key length
*
* <digest>  - message digest algorithm to be used
*
* <engine>  - engine implementation (NULL for default)
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpHmacInit
    (
    ADP_HMAC_CTX_ID     ctxId,  /* hmac context */
    const void *    key,    /* key to be used */
    int         keyLen, /* key length */
    unsigned char * digest, /* message digest algorithm */
    void *      engine  /* ENGINE implementation to be used */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) ||
        (pCryptoFuncTbl->hmacInit == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) key;
        schedParams.param3 = (ULONG) keyLen;
        schedParams.param4 = (ULONG) digest;
        schedParams.param5 = (ULONG) engine;

        /* The scheduler should make a copy of the parameters passed to it */
        if ((*(pCryptoFuncTbl->schedule))((ADP_FUNCPTR)pCryptoFuncTbl->hmacInit,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->hmacInit))(ctxId->provCtxId, key, keyLen,
                      digest, engine) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
        
    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacUpdate - update adaptos hmac context
*
* This function updates the hmac context with the specified data
*
* <ctxId>   - hmac context
*
* <data>    - data to be used in digest computation
*
* <dataLen> - length of data
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpHmacUpdate
    (
    ADP_HMAC_CTX_ID ctxId,  /* hmac context */
    unsigned char * data,   /* data to be used in digest computation */
    unsigned int    dataLen /* length of data */
    )
    {
   if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->hmacUpdate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) data;
        schedParams.param3 = (ULONG) dataLen;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->hmacUpdate,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->hmacUpdate))(ctxId->provCtxId, data, 
                                              dataLen) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
            
    return (ST_OK);
    }


/*******************************************************************************
*
* adpHmacFinal - finish adaptos hmac calculation
*
* This function completes the hmac calculation
*
* <ctxId>   - hmac context
*
* <md>      - buffer to store computed digest
*
* <mdLen>   - length of the computed digest
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpHmacFinal
    (
    ADP_HMAC_CTX_ID ctxId,  /* hmac context */
    unsigned char * md, /* buffer to store computed digest */
    unsigned int *  mdLen   /* buffer to store length of digest */
    )
    {
    if ((ctxId == NULL) || (ctxId->provCtxId == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return(ST_ERROR);
        }
    
    if ((pCryptoFuncTbl == NULL) || (pCryptoFuncTbl->hmacFinal == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_CRYPTO_LIB);
        return (ST_ERROR);
        }

    /* if a scheduler is defined, call it */
    if (pCryptoFuncTbl->schedule != NULL)
        {
        ADP_SCHED_PARAMS schedParams;

        schedParams.param1 = (ULONG) ctxId->provCtxId;
        schedParams.param2 = (ULONG) md;
        schedParams.param3 = (ULONG) mdLen;

        /* The scheduler should make a copy of the parameters passed to it */
        if (pCryptoFuncTbl->schedule((ADP_FUNCPTR)pCryptoFuncTbl->hmacFinal,
            &schedParams, pCryptoFuncTbl->notifyFunc, ctxId->arg1) != ST_OK)
            {
            return (ST_ERROR);
            }

        /* call the installed wait function */
        (*(pCryptoFuncTbl->waitFunc))(ctxId->arg1);
        }
    else
        {
        if ((*(pCryptoFuncTbl->hmacFinal))(ctxId->provCtxId, md, mdLen) != ST_OK)
            {
            return (ST_ERROR);
            }
        }
                
    return (ST_OK);
    }


/*******************************************************************************
*
* adpBlockingFunc - semaphore based blocking funtion
*
* This funtion implements semaphore based blocking function
*
* <arg1>    - semaphore to block on
*
* RETURNS: N/A
*/

LOCAL STATUS adpBlockingFunc
    (
    ULONG   arg1        /* semaphore to take */
    )
    {
    ADP_SEM_ID semId = (ADP_SEM_ID)arg1;

    /* block on the semaphore */
    if (adpSemTake (semId, ADP_SEM_WAIT_FOREVER, 0) != ST_OK)
        {
        return (ST_ERROR);
        }

    return (ST_OK);
    }


/*******************************************************************************
*
* adpWakeupFunc - semaphore based wakeup funtion
*
* This function implements semaphore based wakeup funciton
*
* <arg1>    - semaphore to release
*
* RETURNS: N/A
*/

LOCAL void adpWakeupFunc
    (
    ULONG   arg1        /* semaphore to release */
    )
    {
    ADP_SEM_ID semId = (ADP_SEM_ID)arg1;

    /* release the semaphore */
    adpSemGive (semId);

    return;
    }
