/* adpPasswd.c - trivial user database implementation */

/* Copyright (c) 2007 - 2008, TeamF1, Inc. */

/*
modification history
--------------------
01c,25jun08,rnp  added HAVE_ADP_PASSWD, because the adpPasswdUsers doesnt
                 compile on integrity, since it has a different passwd structure
01b,02jan08,rnp  fixed warning in adpPasswdCrypt()
01a,06dec07,rnp  written
*/

/* include files */
#include "adpPasswd.h"

#ifndef HAVE_ADP_PASSWD
/* defines */
#define ADP_PASSWD_NUM_USERS  (sizeof(adpPasswdUsers)/sizeof(ADP_PASSWD))

/* typedefs */

/* externs */

/* imports */

/* globals */
LOCAL ADP_PASSWD adpPasswdUsers[] =
    {
        { "user", "pass", 1, 1, "", "/", NULL },
        { "root", "root", 0, 0, "", "/", NULL },
    };


/* locals */

/* forward declarations */

/*******************************************************************************
*
* adpPasswdGetPwNam - trivial implementation of unix getpwnam()
*
* RETURNS: passwd entry
*/
ADP_PASSWD * adpPasswdGetPwNam
    (
    const char *  pName      /* user name */
    )
    {
    int i = 0;
    for (i = 0; i < ADP_PASSWD_NUM_USERS; i++)
        {
        if (strcmp(adpPasswdUsers[i].pw_name, pName) == 0)
            return &adpPasswdUsers[i];
        }
    return NULL;
    }

/*******************************************************************************
*
* adpPasswdCrypt - trivial implementation of unix crypt()
*
* RETURNS: crypt'ed string
*/
char * adpPasswdCrypt
    (
    const char * pPasswd,   /* password */
    const char * pSalt      /* salt     */
    )
    {
    return (char *)pPasswd;
    }
#endif /* HAVE_ADP_PASSWD */
