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

char copyright[] =
  "@(#) Copyright (c) 1985, 1989 Regents of the University of California.\n"
  "All rights reserved.\n";

/*
 * from: @(#)main.c	5.18 (Berkeley) 3/1/91
 */


/*
 * FTP User Program -- Command Interface.
 */
//#include <stdio.h>

#ifdef HAVE_FTP_DOWNLOAD
#include <string.h>
#include "cmds.h"

// #define Extern
#include "ftp_var.h"

extern int data;
extern int NCMDS;

void intr(int);
void lostpeer(int);

#define OK							0
#define ERR_DOWNLOAD_FAILURE		9010

int
ftp_login(char *hostname, const char *username, const char *password )
{
	ftp_port = htons(21);
	passivemode = 0;

	cpend = 0;	//* no pending replies 
	proxy = 0;	//* proxy not active 
	sendport = -1;	//* not using ports 
	memset(bind_device, 0, sizeof(bind_device));
	/*
	 * Connect to the remote server
	 */
	connected = doLogin( hostname, username, password);
	return ( connected == 1 ? OK : ERR_DOWNLOAD_FAILURE );
}

int 
ftp_get( char *remoteFile, char *localFile, long *size )
{
	*size = 0;
	return dimget( remoteFile, localFile, size );
}

int 
ftp_put( char *remoteFile, char *localFile, long *size )
{
	*size = 0;
	return dimput( remoteFile, localFile, size );
}

void 
ftp_disconnect( )
{
	disconnect();
}

int
ftp_login_adv(char *hostname, const char *username, const char *password ,int port, char *interface)
{
    if(port == 0)
	    ftp_port = htons(21);
	else
	    ftp_port = htons(port);
	passivemode = 0;

	cpend = 0;	//* no pending replies 
	proxy = 0;	//* proxy not active 
	sendport = -1;	//* not using ports 
	memset(bind_device, 0, sizeof(bind_device));
	/*
	 * Connect to the remote server
	 */
	if(interface != NULL)
	    strcpy(bind_device,interface);
	
	connected = doLogin( hostname, username, password);
	return ( connected == 1 ? OK : ERR_DOWNLOAD_FAILURE );
}

#ifdef WITH_TR143
int
ftp_login_TR143(char *hostname, const char *username, const char *password ,int port, char *interface,int isDownload)
{
    if(port == 0)
	    ftp_port = htons(21);
	else
	    ftp_port = htons(port);
	passivemode = 0;

	cpend = 0;	//* no pending replies 
	proxy = 0;	//* proxy not active 
	sendport = -1;	//* not using ports 
	/*
	 * Connect to the remote server
	 */
	if(isDownload == 0){
	    passivemode = 1;
	#ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Requested");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.ROMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.BOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.EOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.TotalBytesSent")),"0");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.TCPOpenRequestTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.TCPOpenResponseTime")),"");
	#else
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Requested");
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.ROMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.BOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.EOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.TotalBytesSent")),"0");
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.TCPOpenRequestTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.TCPOpenResponseTime")),"");
	#endif
	} else {
	    passivemode = 1;
	#ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Requested");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.ROMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.BOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.EOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TestBytesReceived")),"0");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TotalBytesReceived")),"0");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TCPOpenRequestTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TCPOpenResponseTime")),"");
	#else
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Requested");
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.ROMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.BOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.EOMTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TotalBytesSent")),"0");
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TotalBytesReceived")),"0");
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TCPOpenRequestTime")),"");
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TCPOpenResponseTime")),"");
	#endif
	}
	
	connected = doLogin_TR143( hostname, username, password,isDownload,interface);
	return ( connected == 1 ? OK : ERR_DOWNLOAD_FAILURE );
}

int 
ftp_get_TR143( char *remoteFile, long *size , char *interface)
{
	*size = 0;
	return dimget_TR143( remoteFile, size, interface );
}

int 
ftp_put_TR143( char *remoteFile, long TestFileLength, long *size, char *interface )
{
	*size = 0;
	return dimput_TR143( remoteFile, TestFileLength, size, interface );
}
#endif
#endif /* HAVE_FTP_DOWNLOAD */
