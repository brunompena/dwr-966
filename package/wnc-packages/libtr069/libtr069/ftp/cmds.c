/*
 * Copyright (c) 1985, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


/*
 * FTP User Program -- Command Routines.
 * this is extracted from the original code from cmds.c by Dimark
 */

#ifdef HAVE_FTP_DOWNLOAD

#include <arpa/ftp.h>
#include <stdio.h>
#include <string.h> 
#include "ftp_var.h"
#include "cmds.h"


extern FILE *cout;
extern int data;
extern int code;

static void do_settype( const char *);


/** Connect to a host with username and password 
*/
int 
doLogin( const char *host, const char *user, const char *pass )
{
	int n;
	char *nHost;
	unsigned short port;
	
	port = ftp_port;

	nHost = hookup(host, port);
	if ( nHost ) {
		// set username
		if(user == NULL)
		    n = command("USER anonymous");
		else
		n = command("USER %s", user);
		if (n == CONTINUE) {
			// password
			if(pass == NULL)
			    n = command("PASS dummypwd@");
			else
			n = command("PASS %s", pass);
		}
		if (n != COMPLETE) {
			return (0);
		}
		do_settype( "binary" );
		return (1);
	} else {
		return (0);
	}
}

struct	types {
	const char *t_name;
	const char *t_mode;
	int t_type;
	const char *t_arg;
} types[] = {
	{ "ascii",	"A",	TYPE_A,	NULL },
	{ "binary",	"I",	TYPE_I,	NULL },
	{ "image",	"I",	TYPE_I,	NULL },
	{ "ebcdic",	"E",	TYPE_E,	NULL },
	{ "tenex",	"L",	TYPE_L,	bytename },
	{ NULL, NULL, 0, NULL }
};

/*
 * Set transfer type.
 */
static 
void
do_settype(const char *thetype) 
{
	struct types *p;
	int comret;

	for (p = types; p->t_name; p++)
		if (strcmp(thetype, p->t_name) == 0)
			break;
	if (p->t_name == 0) {
		debug("%s: unknown mode\n", thetype);
		code = -1;
		return;
	}
	if ((p->t_arg != NULL) && (*(p->t_arg) != '\0'))
		comret = command("TYPE %s %s", p->t_mode, p->t_arg);
	else
		comret = command("TYPE %s", p->t_mode);
	if (comret == COMPLETE) {
		(void) strcpy(typename, p->t_name);
		curtype = type = p->t_type;
	}
}

/*
 * Internal form of settype; changes current type in use with server
 * without changing our notion of the type for data transfers.
 * Used to change to and from ascii for listings.
 */
void
changetype(int newtype, int show)
{
        register struct types *p;
        int comret;

        if (newtype == 0)
                newtype = TYPE_I;
        if (newtype == curtype)
                return;
        for (p = types; p->t_name; p++)
                if (newtype == p->t_type)
                        break;
        if (p->t_name == 0) {
                debug("ftp: internal error: unknown type %d\n", newtype);
                return;
        }
        if (newtype == TYPE_L && bytename[0] != '\0')
                comret = command("TYPE %s %s", p->t_mode, bytename);
        else
                comret = command("TYPE %s", p->t_mode);
        if (comret == COMPLETE)
                curtype = newtype;
}

/** Get a file from the remote host.
 * 
 * \param remoteFile	remotePathname relative to ftp home or absolute 
 * \param localFile		localPathname relative or better absolute
 * \return errorcode	
 */
int 
dimget( char *remoteFile, char *localFile, long *size )
{
	return recvrequest("RETR", localFile, remoteFile, "w", size );
}

/** Put a file to the remote host.
 * 
 * \param remoteFile	remotePathname relative to ftp home or absolute 
 * \param localFile		localPathname relative or better absolute
 * \return errorcode	
 */
int 
dimput( char *remoteFile, char *localFile, long *size )
{
	return sendrequest("STOR", localFile, remoteFile, size );
}

/**
 * Terminate session, but don't exit.
 */
void
disconnect(void)
{
	if (!connected)
		return;
	(void) command("QUIT");
	if (cout) {
		(void) fclose(cout);
	}
	cout = NULL;
	connected = 0;
	data = -1;
}

/*
 * Start up passive mode interaction
 */
void
setpassive(void)
{
        passivemode = !passivemode;
        code = passivemode;
}

#ifdef WITH_TR143
static 
int
do_settype_TR143(const char *thetype,int isDownload) 
{
	struct types *p;
	int comret;

	for (p = types; p->t_name; p++)
		if (strcmp(thetype, p->t_name) == 0)
			break;
	if (p->t_name == 0) {
		debug("%s: unknown mode\n", thetype);
		code = -1;
		return (0);
	}
	if ((p->t_arg != NULL) && (*(p->t_arg) != '\0'))
		comret = command("TYPE %s %s", p->t_mode, p->t_arg);
	else
		comret = command("TYPE %s", p->t_mode);
	if (comret == COMPLETE) {
		(void) strcpy(typename, p->t_name);
		curtype = type = p->t_type;
		return (1);
	} else {
	    if(isDownload == 0){
	    #ifdef WITH_TR098
	        DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_LoginFailed");
	    #else
	        DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_LoginFailed");
	    #endif
	    } else {
	    #ifdef WITH_TR098
	        DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_LoginFailed");
	    #else
	        DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_LoginFailed");
	    #endif
		}
		return (0);
	}
}

int 
doLogin_TR143( const char *host, const char *user, const char *pass,int isDownload, char *interface)
{
	int n;
	char *nHost;
	unsigned short port;
	
	port = ftp_port;

	nHost = hookup_adv(host, port, interface);
	if ( nHost ) {
		// set username
		if(user == NULL)
		    n = command("USER anonymous");
		else
		    n = command("USER %s", user);
		if (n == CONTINUE) {
			// password
			if(pass == NULL)
			    n = command("PASS dummypwd@");
			else
			    n = command("PASS %s", pass);
		    if (n != COMPLETE) {
	            if(isDownload == 0){
	            #ifdef WITH_TR098
	                DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_PasswordRequestFailed");
	            #else
	                DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_PasswordRequestFailed");
	            #endif
	            } else {
	            #ifdef WITH_TR098
	                DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_PasswordRequestFailed");
	            #else
	                DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_PasswordRequestFailed");
	            #endif
	            }
			    return (0);
		    }
		} else if (n != COMPLETE) {
			goto InitConnectionFailed;
		}
		return do_settype_TR143( "binary" ,isDownload);
	} else {
		goto InitConnectionFailed;
	}
InitConnectionFailed:
	if(isDownload == 0){
	#ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	#else
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	#endif
	} else {
	#ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	#else
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	#endif
	}
    return (0);
}

int 
dimget_TR143( char *remoteFile, long *size, char *interface )
{
	return recvrequest_TR143("RETR", remoteFile, "w", size, interface );
}

int 
dimput_TR143( char *remoteFile, long TestFileLength, long *size, char *interface )
{
	return sendrequest_TR143("STOR", TestFileLength, remoteFile, size, interface );
}
#endif
#endif /* HAVE_FTP_DOWNLOAD */
