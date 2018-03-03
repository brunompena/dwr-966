/* adpPasswd.h - adpPasswd header file */

/* Copyright (c) 2007, TeamF1, Inc. */

/*
modification history
--------------------
01a,06dec07,rnp  written
*/

#ifndef __INCadpPasswdh
#define __INCadpPasswdh

/* includes */
#include <adaptos.h>

/* typedefs */
#ifdef HAVE_ADP_PASSWD
typedef struct passwd ADP_PASSWD;
#else /* HAVE_ADP_PASSWD */
typedef struct passwd
    {
    char *       pw_name;     /* user name     */
    char *       pw_passwd;   /* password      */
    u_int32_t    pw_uid;      /* user id       */
    u_int32_t    pw_gid;      /* group id      */
    char *       pw_gecos;    /* real name     */
    char *       pw_dir;      /* home dir      */
    char *       pw_shell;    /* shell program */
    } ADP_PASSWD;
#endif /* HAVE_ADP_PASSWD */

IMPORT char * adpPasswdCrypt(const char * pPasswd, const char * pSalt);
IMPORT ADP_PASSWD * adpPasswdGetPwNam(const char * pName);
#endif /* __INCadpPasswdh */
