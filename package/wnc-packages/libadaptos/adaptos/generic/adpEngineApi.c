/* adpEngineApi.c - Adaptos Engine API */

/* Copyright (c) 2003, TeamF1, Inc. */

/*
modification history
--------------------
01b,07jul03,ws   made pEngineFuncTbl NULL by default
01a,02jul03,ws   written
*/

/*
DESCRIPTION

This module defines the Adaptos Engine API. This API is used by the
application layer for all engine related functions, like adding and engine
to the system, deleting an engine from the system etc.,

The functioning of the Adaptos Engine API is dependent on the
implementation of engine provider library. The engine provider library
implementation is registered with the Engine API by calling
adpEngineApiLibInstall.


EXTERNAL INTERFACE
The functioning of Adaptos Engine API relies on the implementation
of the following functions by the engine provider library. For more
information check the documentation of the individual routines.

.CS
engineCreate()      -   create engine context
engineDestroy()     -   free engine context
engineAdd()         -   add engine to the system
engineRemove()      -   remove engine from the system
engineLoad()        -   load the specified engine
engineSetDefault()  -   set the specified engine as the default
.CE

The above functions are exported to the calling function, by the crypto
provider library through ENGINE_FUNC_TBL structure by the engine provider
init function.

INTERNAL

Here is a small ascii diagram that shows how the Adaptos Crypto API
fits in the overall crypto system.

          Applications
            |
            v
        Adaptos Engine API
            |
            v
        engine provider library
*/

#include "adaptos.h"

/* defines */


/* globals */


/* locals */
LOCAL ENGINE_FUNC_TBL   engineFuncTbl = {NULL, NULL, NULL,
                                         NULL, NULL, NULL};
LOCAL ENGINE_FUNC_TBL * pEngineFuncTbl = NULL;

/*******************************************************************************
*
* adpEngineApiLibInstall - install engine API library
*
* This function installs user specified engine implementation
* into the system.
*
* <pInitFunc>   - pointer to engine provider init func
*
* <arg1>    - argument required by engine provider init func
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEngineApiLibInstall
    (
    ENGINE_PROV_INIT_FUNC   pInitFunc,  /* crypto provider init func */
    int             arg1        /* argument to init func */
    )
    {
    ENGINE_FUNC_TBL *pFuncTbl = NULL;
    
    pEngineFuncTbl = &engineFuncTbl;
    
    /* initialize the function table */
    memset (pEngineFuncTbl, 0, sizeof(*pEngineFuncTbl));
    
    if ((pFuncTbl = (*pInitFunc)(arg1)) == NULL)
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INIT_FUNC);
        return (ST_ERROR);
        }

    pEngineFuncTbl->engineCreate    = pFuncTbl->engineCreate;
    pEngineFuncTbl->engineDestroy   = pFuncTbl->engineDestroy;
    pEngineFuncTbl->engineAdd       = pFuncTbl->engineAdd;
    pEngineFuncTbl->engineRemove    = pFuncTbl->engineRemove;
    pEngineFuncTbl->engineLoad      = pFuncTbl->engineLoad;
    pEngineFuncTbl->engineSetDefault    = pFuncTbl->engineSetDefault;

    return (ST_OK);
    }

/*******************************************************************************
*
* adpEngineCreate - create implementation specific engine structure
*
* This function creates engine context
*
* <id>      - engine id
*
* <name>    - engine name
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

void * adpEngineCreate
    (
    char *  id, /* Engine ID */
    char *  name    /* Engine Name */
    )
    {
    void *pEngine = NULL;

    if ((id == NULL) || (name == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return (NULL);
        }

    if ((pEngineFuncTbl == NULL) || (pEngineFuncTbl->engineCreate == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_ENGINE_LIB);
        return (NULL);
        }

    pEngine = pEngineFuncTbl->engineCreate(id, name);

    return (pEngine);
    }

/*******************************************************************************
*
* adpEngineDestroy - add engine to the list of known engines
*
* This function frees the specified adaptos engine context
*
* <engine>  - engine to be initialized
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEngineDestroy
    (
    void *  engine      /* engine implementation */
    )
    {
    
    if (engine == NULL)
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pEngineFuncTbl == NULL) || (pEngineFuncTbl->engineDestroy == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_ENGINE_LIB);
        return (ST_ERROR);
        }

    if (pEngineFuncTbl->engineDestroy(engine) != ST_OK)
        {
        return (ST_ERROR);
        }

    return (ST_OK);
    }

/*******************************************************************************
*
* adpEngineAdd - add engine to the list of known engines
*
* This function adds the specified engine to the system
*
* <engine>  - engine to be initialized
*
* <comps>   - pointer to engine components that need to be added
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEngineAdd
    (
    void *      engine,     /* engine implementation */
    ADP_ENGINE_COMPS_ID comps       /* components to be installed */
    )
    {
    if ((engine == NULL) || (comps == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pEngineFuncTbl == NULL) || (pEngineFuncTbl->engineAdd == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_ENGINE_LIB);
        return (ST_ERROR);
        }

    if (pEngineFuncTbl->engineAdd(engine, comps) != ST_OK)
        {
        return (ST_ERROR);
        }

    return (ST_OK);
    }


/*******************************************************************************
*
* adpEngineRemove - remove engine from the list of known engines
*
* This function removes the specified engine from the system
*
* <engine>  - engine to be initialized
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEngineRemove
    (
    void *  engine      /* engine implementation */
    )
    {
    if (engine == NULL)
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pEngineFuncTbl == NULL) || (pEngineFuncTbl->engineRemove == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_ENGINE_LIB);
        return (ST_ERROR);
        }

    if (pEngineFuncTbl->engineRemove(engine) != ST_OK)
        {
        return (ST_ERROR);
        }

    return (ST_OK);
    }


/*******************************************************************************
*
* adpEngineLoad - load this engine into the system
*
* This function loads the specified engine into the system
*
* <engine>  - engine to be loaded
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEngineLoad
    (
    void *  engine      /* engine implementation */
    )
    {
    if (engine == NULL)
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pEngineFuncTbl == NULL) || (pEngineFuncTbl->engineLoad == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_ENGINE_LIB);
        return (ST_ERROR);
        }

    if (pEngineFuncTbl->engineLoad(engine) != ST_OK)
        {
        return (ST_ERROR);
        }

    return (ST_OK);
    }


/*******************************************************************************
*
* adpEngineSetDefault - set this engine as the default
*
* This function sets the specified engine as the default for all
* cipher related operations
*
* <engine>  - engine to be set default to
*
* <flags>   - flags specifying modules
*
* RETURNS: ST_OK on success and ST_ERROR otherwise.
*/

STATUS adpEngineSetDefault
    (
    void *      engine,     /* engine implementation */
    unsigned int    flags       /* engine modules */
    )
    {
    if (engine == NULL)
        {
        adpErrnoSet(ADP_CRYPTO_ERR_INV_PARAM);
        return (ST_ERROR);
        }

    if ((pEngineFuncTbl == NULL) || (pEngineFuncTbl->engineSetDefault == NULL))
        {
        adpErrnoSet(ADP_CRYPTO_ERR_NO_ENGINE_LIB);
        return (ST_ERROR);
        }

    if (pEngineFuncTbl->engineSetDefault(engine, flags) != ST_OK)
        {
        return (ST_ERROR);
        }

    return (ST_OK);
    }

