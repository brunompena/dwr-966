/* adpCryptoApi.h - adaptos wrapper header file for Crypto and Engine */

/* Copyright (c) 2005, TeamF1, Inc. */

/*
modification history
--------------------
01d,07may05,sam  restructured adaptos.
01c,28may04,rks  added ctrl cmds for setting M,L parameter for CCM mode.
01b,16aug03,ksn  added arguments for int functions.
01a,03jun03,ws   written
*/

#ifndef ADPCRYPTOAPI_H
#define ADPCRYPTOAPI_H

#include "adaptos.h"

#define ADP_ENCRYPT 1
#define ADP_DECRYPT 2

/* operation supported by adpCipherContextCtrl */
#define ADP_CTRL_BASE                   0xFFFF
#define ADP_CTRL_SET_KEY_LEN            (ADP_CTRL_BASE + 0x01)
#define ADP_CTRL_GET_KEY_LEN            (ADP_CTRL_BASE + 0x02)
#define ADP_CTRL_SET_PADDING            (ADP_CTRL_BASE + 0x03)
#define ADP_CTRL_GET_PADDING            (ADP_CTRL_BASE + 0x04)
#define ADP_CTRL_GET_BLOCK_SIZE         (ADP_CTRL_BASE + 0x05)
#define ADP_CTRL_SET_INIT_VECTOR        (ADP_CTRL_BASE + 0x06)
#define ADP_CTRL_GET_INIT_VECTOR        (ADP_CTRL_BASE + 0x07)
#define ADP_CTRL_GET_INIT_VECTOR_LEN    (ADP_CTRL_BASE + 0x08)
#define ADP_CTRL_SET_APP_DATA           (ADP_CTRL_BASE + 0x09)
#define ADP_CTRL_GET_APP_DATA           (ADP_CTRL_BASE + 0x0A)
#define ADP_CTRL_GET_FLAGS              (ADP_CTRL_BASE + 0x0B)
#define ADP_CTRL_GET_DIGEST_LEN         (ADP_CTRL_BASE + 0x0C)
#define ADP_CTRL_GET_DIGEST_BLOCK_SIZE  (ADP_CTRL_BASE + 0x0D)
#define ADP_CTRL_SET_CCM_M              (ADP_CTRL_BASE + 0x0E)
#define ADP_CTRL_SET_CCM_L              (ADP_CTRL_BASE + 0x0F)
    
/* error codes returned by adaptos Crypto API */
#define ADP_CRYPTO_ERR_UNKNOWN          (M_adpCryptoLib + 0x01)
#define ADP_CRYPTO_ERR_INV_PARAM        (M_adpCryptoLib + 0x02)
#define ADP_CRYPTO_ERR_INIT_FUNC        (M_adpCryptoLib + 0x03)
#define ADP_CRYPTO_ERR_MALLOC           (M_adpCryptoLib + 0x04)
#define ADP_CRYPTO_ERR_DO_CIPHER        (M_adpCryptoLib + 0x05)
#define ADP_CRYPTO_ERR_UNSUPP_CIPHER    (M_adpCryptoLib + 0x06)
#define ADP_CRYPTO_ERR_UNSUPP_DIGEST    (M_adpCryptoLib + 0x07)
#define ADP_CRYPTO_ERR_NO_CRYPTO_LIB    (M_adpCryptoLib + 0x08)
#define ADP_CRYPTO_ERR_NO_ENGINE_LIB    (M_adpCryptoLib + 0x09)
#define ADP_CRYPTO_ERR_INV_OP_CODE      (M_adpCryptoLib + 0x0A)

#ifndef HAVE_ADP_CIPHER_PROV_CTX_ID
typedef ULONG ADP_CIPHER_PROV_CTX_ID;
#define HAVE_ADP_CIPHER_PROV_CTX_ID
#endif

#ifndef HAVE_ADP_DIGEST_PROV_CTX_ID
typedef ULONG ADP_DIGEST_PROV_CTX_ID;
#define HAVE_ADP_DIGEST_PROV_CTX_ID
#endif

#ifndef HAVE_ADP_CIPHER
typedef ULONG ADP_CIPHER;
#define HAVE_ADP_CIPHER
#endif

#ifndef HAVE_ADP_DIGEST
typedef ULONG ADP_DIGEST;
#define HAVE_ADP_DIGEST
#endif 

/* engine component structure */
#ifndef HAVE_ADP_ENGINE_COMPS
typedef struct adpEngineCompsStruct
    {
    void *  rsa_meth;   /* pointer to rsa methods structure */
    void *  dsa_meth;   /* pointer to dsa methods structure */
    void *  dh_meth;    /* pointer to dh methods structure */
    void *  rand_meth;  /* pointer to rand methods structure */
    int     (*ciphers) (void *, ADP_CIPHER **, const int **, int);
                /* callback function to add ciphers */
    int     (*digests) (void *, ADP_DIGEST **, const int **, int);
                /* callback function to add digests */
    int     (*destroy) (void *); /* callback function to destroy engine */
    int     (*init) (void *); /* callback function to initialize engine */
    int     (*finish) (void *); /* callback function to cleanup engine */
    int     (*ctrl) (void *, int, long, void *, void (*f)());
                /* callback function to perform control ops */
    void *  (*load_privkey) (void *, const char *, void *, void *);
                /* callback function to load private key */
    void *  (*load_pubkey) (void *, const char *, void *, void *);
                /* callback function to load public key */
    void *  cmd_defns;  /* pointer to control command structure */
    } ADP_ENGINE_COMPS;
#define HAVE_ADP_ENGINE_COMPS
#endif

#ifndef HAVE_ADP_ENGINE_COMPS_ID
typedef ADP_ENGINE_COMPS* ADP_ENGINE_COMPS_ID;
#define HAVE_ADP_ENGINE_COMPS_ID
#endif

/* adaptos cipher context */ 
#ifndef HAVE_ADP_CIPHER_CTX
typedef struct adpCipherContextStruct
    {
    void *  provCtxId;  /* provider cipher ctx */
    int     encrypt;        /* to indicate the operation being performed */
    ULONG   arg1;       /* argument to wait/notify */
    } ADP_CIPHER_CTX;
#define HAVE_ADP_CIPHER_CTX
#endif

#ifndef HAVE_ADP_CIPHER_CTX_ID
typedef ADP_CIPHER_CTX* ADP_CIPHER_CTX_ID; 
#define HAVE_ADP_CIPHER_CTX_ID
#endif

/* adaptos digest context */
#ifndef HAVE_ADP_DIGEST_CTX
typedef struct adpDigestContextStruct
    {
    void *  provCtxId;  /* provider digest ctx */
    ULONG   arg1;       /* argument to wait/notify */
    } ADP_DIGEST_CTX;
#define HAVE_ADP_DIGEST_CTX
#endif

#ifndef HAVE_ADP_DIGEST_CTX_ID
typedef ADP_DIGEST_CTX* ADP_DIGEST_CTX_ID;
#define HAVE_ADP_DIGEST_CTX_ID
#endif

/* adaptos hmac context */
#ifndef HAVE_ADP_HMAC_CTX
typedef struct adpHmacContextStruct
    {
    void *  provCtxId;  /* provider digest ctx */
    ULONG   arg1;       /* argument to wait/notify */
    } ADP_HMAC_CTX;
#define HAVE_ADP_HMAC_CTX
#endif

#ifndef HAVE_ADP_HMAC_CTX_ID
typedef ADP_HMAC_CTX* ADP_HMAC_CTX_ID;
#define HAVE_ADP_HMAC_CTX_ID
#endif

/* parameter structure for scheduler. The parameters will be interpreted
 * based on the function that is being scheduled */
#ifndef HAVE_ADP_SCHED_PARAMS
typedef struct adpSchedParamsStruct
    {
    int param1;
    int param2;
    int param3;
    int param4;
    int param5;
    } ADP_SCHED_PARAMS;
#define HAVE_ADP_SCHED_PARAMS
#endif

#ifndef HAVE_ADP_SCHED_PARAMS_ID
typedef ADP_SCHED_PARAMS* ADP_SCHED_PARAMS_ID;
#define HAVE_ADP_SCHED_PARAMS_ID
#endif

/* crypto function pointer table */
#ifndef HAVE_CRYPTO_FUNC_TBL
typedef struct cryptoFuncTblStruct
    {
    void * (*cipherContextCreate)(void);
    STATUS (*cipherContextInit)(void *provCtxId);
    STATUS (*cipherContextCleanup)(void *provCtxId);
    STATUS (*cipherContextDestroy)(void *provCtxId);
    STATUS (*cipherContextCtrl)(void *provCtxId, int type, int arg, void *ptr);
    STATUS (*encryptInit)(void *provCtxId, unsigned char *cipherName,
                          void *engine, const unsigned char *key,
                          const unsigned char *iv);
    STATUS (*encryptUpdate)(void *provCtxId, unsigned char *out,
                            int *outl, const unsigned char *in, int inl);
    STATUS (*encryptFinal)(void *provCtxId, unsigned char *out,
                           int *outl);
    STATUS (*decryptInit)(void *provCtxId, unsigned char *cipherName,
                          void *engine, const unsigned char *key,
                          const unsigned char *iv);
    STATUS (*decryptUpdate)(void *provCtxId, unsigned char *out,
                            int *outl, const unsigned char *in, int inl);
    STATUS (*decryptFinal)(void *provCtxId, unsigned char *out, int *outl);
    void * (*digestContextCreate) (void);
    STATUS (*digestContextInit) (void *provCtxId);
    STATUS (*digestContextCleanup) (void *provCtxId);
    STATUS (*digestContextDestroy) (void *provCtxId);
    STATUS (*digestContextCtrl)(void *provCtxId, int type, int arg, void *ptr);
    STATUS (*digestGetDigestLen)(unsigned char *digestName, int *len);
    STATUS (*digestInit)(void *provCtxId, unsigned char *digestName,
                         void *engine);
    STATUS (*digestUpdate)(void *provCtxId, unsigned char *data,
                           unsigned int dataLen);
    STATUS (*digestFinal)(void *provCtxId, unsigned char *md,
                          unsigned int *mdLen);
    void * (*hmacContextCreate) (void);
    STATUS (*hmacContextInit) (void *provCtxId);
    STATUS (*hmacContextCleanup) (void *provCtxId);
    STATUS (*hmacContextDestroy) (void *provCtxId);
    STATUS (*hmacContextCtrl)(void *provCtxId, int type, int arg, void *ptr);
    STATUS (*hmacInit)(void *provCtxId, const void *key, int len,
                       unsigned char *digestName, void *engine);
    STATUS (*hmacUpdate)(void *provCtxId, unsigned char *data,
                           unsigned int dataLen);
    STATUS (*hmacFinal)(void *provCtxId, unsigned char *md,
                          unsigned int *mdLen);
    STATUS (*waitFunc)(ULONG arg);
    void (*notifyFunc)(ULONG arg);
    STATUS (*schedule)(ADP_FUNCPTR sched, ADP_SCHED_PARAMS *pParams,
                       ADP_VOIDFUNCPTR notifyFunc, int notifyParam);
    } CRYPTO_FUNC_TBL;
#define HAVE_CRYPTO_FUNC_TBL
#endif

#ifndef HAVE_ENGINE_FUNC_TBL
typedef struct engineFuncTblStruct
    {
    void * (*engineCreate)(char *id, char *name);
    STATUS (*engineDestroy)(void *engine);
    STATUS (*engineAdd)(void *engine, ADP_ENGINE_COMPS_ID comps);
    STATUS (*engineRemove)(void *engine);
    STATUS (*engineLoad)(void *engine);
    STATUS (*engineSetDefault)(void *engine, unsigned int flags);
    } ENGINE_FUNC_TBL;
#define HAVE_ENGINE_FUNC_TBL 
#endif

#ifndef HAVE_CRYPTO_PROV_INIT_FUNC
typedef CRYPTO_FUNC_TBL * (*CRYPTO_PROV_INIT_FUNC)(int);
#define HAVE_CRYPTO_PROV_INIT_FUNC
#endif

#ifndef HAVE_ENGINE_PROV_INIT_FUNC
typedef ENGINE_FUNC_TBL * (*ENGINE_PROV_INIT_FUNC)(int);
#define HAVE_ENGINE_PROV_INIT_FUNC
#endif

IMPORT STATUS adpCryptoApiLibInstall (CRYPTO_PROV_INIT_FUNC pInitFunc,
                                      int arg1);
IMPORT STATUS adpCipherContextCreate (ADP_CIPHER_CTX_ID ctxId);
IMPORT STATUS adpCipherContextInit (ADP_CIPHER_CTX_ID ctxId);
IMPORT STATUS adpCipherContextCleanup (ADP_CIPHER_CTX_ID ctxId);
IMPORT STATUS adpCipherContextDestroy (ADP_CIPHER_CTX_ID ctxId);
IMPORT STATUS adpCipherContextCtrl (ADP_CIPHER_CTX_ID ctxId, int type,
                                    int arg, void *ptr);
IMPORT STATUS adpEncryptInit (ADP_CIPHER_CTX_ID ctxId,
                              unsigned char *cipherName,
                              void *engine,
                              const unsigned char *key,
                              const unsigned char *iv);
IMPORT STATUS adpEncryptUpdate (ADP_CIPHER_CTX_ID ctxId, unsigned char *out,
                                int *outl, const unsigned char *in, int inl);
IMPORT STATUS adpEncryptFinal (ADP_CIPHER_CTX_ID ctxId, unsigned char *out,
                               int *outl);
IMPORT STATUS adpDecryptInit (ADP_CIPHER_CTX_ID ctxId,
                              unsigned char *cipherName,
                              void *engine,
                              const unsigned char *key,
                              const unsigned char *iv);
IMPORT STATUS adpDecryptUpdate (ADP_CIPHER_CTX_ID ctxId, unsigned char *out,
                                int *outl, const unsigned char *in, int inl);
IMPORT STATUS adpDecryptFinal (ADP_CIPHER_CTX_ID ctxId, unsigned char *out,
                               int *outl);
IMPORT STATUS adpDigestContextCreate (ADP_DIGEST_CTX_ID ctxId);
IMPORT STATUS adpDigestContextInit (ADP_DIGEST_CTX_ID ctxId);
IMPORT STATUS adpDigestContextCleanup (ADP_DIGEST_CTX_ID ctxId);
IMPORT STATUS adpDigestContextDestroy (ADP_DIGEST_CTX_ID ctxId);
IMPORT STATUS adpDigestContextCtrl (ADP_DIGEST_CTX_ID ctxId, int type,
                                    int arg, void *ptr);
IMPORT STATUS adpDigestGetDigestLen (unsigned char *digestName,
                                     int *len);
IMPORT STATUS adpDigestInit (ADP_DIGEST_CTX_ID ctxId,
                             unsigned char *digestName,
                             void *engine);
IMPORT STATUS adpDigestUpdate (ADP_DIGEST_CTX_ID ctxId, unsigned char *buf,
                               unsigned int len);
IMPORT STATUS adpDigestFinal (ADP_DIGEST_CTX_ID ctxId, unsigned char *buf,
                              unsigned int *len);
IMPORT STATUS adpHmacContextCreate (ADP_HMAC_CTX_ID ctxId);
IMPORT STATUS adpHmacContextInit (ADP_HMAC_CTX_ID ctxId);
IMPORT STATUS adpHmacContextCleanup (ADP_HMAC_CTX_ID ctxId);
IMPORT STATUS adpHmacContextDestroy (ADP_HMAC_CTX_ID ctxId);
IMPORT STATUS adpHmacContextCtrl (ADP_HMAC_CTX_ID ctxId, int type,
                                  int arg, void *ptr);
IMPORT STATUS adpHmacInit (ADP_HMAC_CTX_ID ctxId, const void *key,
                           int len, unsigned char *digestName,
                           void *engine);
IMPORT STATUS adpHmacUpdate (ADP_HMAC_CTX_ID ctxId, unsigned char *data,
                             unsigned int dataLen);
IMPORT STATUS adpHmacFinal (ADP_HMAC_CTX_ID ctxId, unsigned char *md,
                            unsigned int *mdLen);

IMPORT STATUS adpEngineApiLibInstall (ENGINE_PROV_INIT_FUNC pInitFunc,
                                      int arg1);
IMPORT void * adpEngineCreate (char *id, char *name);
IMPORT STATUS adpEngineDestroy (void *engine);
IMPORT STATUS adpEngineAdd (void *engine, ADP_ENGINE_COMPS_ID comps);
IMPORT STATUS adpEngineRemove (void *engine);
IMPORT STATUS adpEngineLoad (void *engine);
IMPORT STATUS adpEngineSetDefault (void *engine, unsigned int flags);

#endif /* ADPCRYPTOAPI_H */
