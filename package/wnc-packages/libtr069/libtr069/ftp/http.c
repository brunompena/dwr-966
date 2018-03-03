#include "stdsoap2.h"
#include "Common.h"
#include "httpda.h"
#include "logger.h"

#define CONNECTION_REALM  "JNR"

#ifdef WITH_TR143
void setTR143TimeInfo(char * param, int isDownload);
#endif


typedef struct UploadFile {
	char *filepath;
	char *filename;
	char *filetype;
} UploadFile;

long getTime( )
{
    struct timeval now;

    gettimeofday( &now, NULL );
    return (long)(now.tv_sec*1000 + now.tv_usec/1000);
}

static int sendFile( struct soap *tmpSoap, const UploadFile *srcFile )
{
	int ret = _SUCCESS;
	int fd;
	int bufsize = 1023;
	int rSize;
	
	if ((fd = open (srcFile->filepath, O_RDONLY )) > 0) {
		while(( rSize = read( fd, tmpSoap->tmpbuf, bufsize)) > 0 ) {
			tmpSoap->tmpbuf[rSize] = '\0';
			if ( soap_send_raw( tmpSoap, tmpSoap->tmpbuf, rSize ) != SOAP_OK ) {
				ret = UPLOAD_FAILURE;
				break;
			} 
		}
		close(fd);
		return ret;
	} else {
		return UPLOAD_FAILURE;
	}	
}
/** Read the file into memory and send it via soap_send
 *  returns OK or UPLOAD_FAILURE 
 */
static int uploadFile( struct soap *tmpSoap, const UploadFile *srcFile, long fileSize, int useAuth )
{
	int ret = _SUCCESS;

	long contentLength;
	char fileSizeStr[20];
	char boundary[20];

	sprintf (tmpSoap->tmpbuf, "POST %s HTTP/1.1\r\n", tmpSoap->path);
	soap_send( tmpSoap, tmpSoap->tmpbuf );
	sprintf (tmpSoap->tmpbuf, "%s:%d", tmpSoap->host, tmpSoap->port);
	tmpSoap->fposthdr (tmpSoap, "Host", tmpSoap->tmpbuf);
	if (tmpSoap->userid && tmpSoap->passwd 
		&& strlen(tmpSoap->userid) + strlen(tmpSoap->passwd) < 761)
	  { 
	  	sprintf(tmpSoap->tmpbuf+262, "%s:%s", tmpSoap->userid, tmpSoap->passwd);
    	strcpy(tmpSoap->tmpbuf, "Basic ");
    	soap_s2base64(tmpSoap, 
    		(const unsigned char*)(tmpSoap->tmpbuf + 262), 
    		tmpSoap->tmpbuf + 6, 
    		strlen(tmpSoap->tmpbuf+ 262));
    	tmpSoap->fposthdr(tmpSoap, "Authorization", tmpSoap->tmpbuf);
	  }

//	tmpSoap->fposthdr (tmpSoap, "Authorization", "BASIC" );
	tmpSoap->fposthdr (tmpSoap, "User-Agent", "CPE");
	tmpSoap->fposthdr (tmpSoap, "Connection",
			   tmpSoap->keep_alive ? "keep_alive" : "close");
//
	sprintf (boundary, "%ld", getTime ());
	sprintf (tmpSoap->tmpbuf, "multipart/form-data; boundary=%s", boundary);
	sprintf (fileSizeStr, "--%s", boundary);
	sprintf (boundary, "%s", fileSizeStr);
	tmpSoap->fposthdr (tmpSoap, "Content-Type", tmpSoap->tmpbuf);
	sprintf (tmpSoap->tmpbuf,
		 "%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n",
		 boundary, srcFile->filename, srcFile->filepath, srcFile->filetype );
	// Calc contentlength
	// contentlength = filesize + 2 * boundary + CType + CDisposition + "--\r\n"
	contentLength = strlen(tmpSoap->tmpbuf) + fileSize + strlen(boundary) + 4;
	sprintf (fileSizeStr, "%ld", contentLength);
	tmpSoap->fposthdr (tmpSoap, "Content-Length", fileSizeStr);
	soap_send( tmpSoap, "\r\n" );
	soap_send( tmpSoap, tmpSoap->tmpbuf );
	if ( sendFile( tmpSoap, srcFile ) != _SUCCESS ) {
		soap_end( tmpSoap );
		soap_done( tmpSoap );
		return UPLOAD_FAILURE;
	}
	soap_send( tmpSoap, boundary );
	soap_send( tmpSoap, "--\r\n\r\n" );
//	soap_flush(tmpSoap);
	return _SUCCESS;
}

int exeHttpUpload (char *URL, char *username, char *password, const UploadFile *srcFile,  int fileSize, char *interface)
{
	int ret = _SUCCESS;
	int bSetDigest = FALSE;
	// Data for digest authorization
	struct http_da_info da_info;
	// get a soap structure and use it's buf for input
	struct soap soap;
	struct soap *tmpSoap;
		
	soap_init (&soap);
	tmpSoap = &soap;

	if(strncmp( URL, "https", 5 )==0){
        //soap_ssl_init();
		//if(soap_ssl_client_context(tmpSoap, SOAP_SSL_DEFAULT, NULL/*key file*/, NULL/*"password"*/, "uccert.pem"/*CA file*/, NULL, NULL))
		//	LOG(LOG_DEBUG, "Execute soap_ssl_client_context..Fail\n");
		return FILE_TRANSFER_SERVER_AUTHENTICATION_FAILURE;
	}
	// Register Digest Authentication Plugin
	soap_register_plugin (tmpSoap, http_da);
	
	tmpSoap->keep_alive = 1;
	soap_set_endpoint(tmpSoap, URL);
#ifdef _JPR_AM_
    if(interface != NULL  && strlen(interface)>2)
        tmpSoap->bind_device = interface;
#endif
	tmpSoap->socket = tmpSoap->fopen(tmpSoap, URL, tmpSoap->host, tmpSoap->port);
    if (tmpSoap->error)
        return UPLOAD_FAILURE;
	
	ret = uploadFile( tmpSoap, srcFile, fileSize, FALSE );
	if( ret != _SUCCESS )
		return ret;
		
	// Parse the HTML response header and remove it from the work data
	ret = tmpSoap->fparse (tmpSoap);
	
	switch( ret ) {
		case SOAP_OK:
			soap_end (tmpSoap);
			soap_done (tmpSoap);
			break;
		case 401:		/* Authorization failure */
		    LOG (LOG_DEBUG, "%s\n", tmpSoap->buf);
			if (strstr(tmpSoap->buf, "WWW-Authenticate: Digest") != NULL){
			    bSetDigest = TRUE;
			    http_da_save (tmpSoap, 
				    &da_info, 
				    (tmpSoap->authrealm != NULL ? tmpSoap->authrealm : "CPE"),
		 		    username, password);
			} else {
				tmpSoap->userid = username;
				tmpSoap->passwd = password;
			}
			
			tmpSoap->fprepareinit (tmpSoap);
			ret = uploadFile( tmpSoap, srcFile, fileSize, TRUE );
			if ( ret != _SUCCESS )
				return ret;
			ret = tmpSoap->fparse (tmpSoap);
			if (ret == SOAP_OK)
				break;
			LOG (LOG_DEBUG, "401 Request %d \n", ret);
			soap_end (tmpSoap);
			if(bSetDigest == TRUE)
			    http_da_release(tmpSoap, &da_info);
			soap_done (tmpSoap);
			return FILE_TRANSFER_SERVER_AUTHENTICATION_FAILURE;
		default:			
			LOG (LOG_DEBUG, "Request %d \n", ret);
			soap_end (tmpSoap);
			soap_done (tmpSoap);
			return UPLOAD_FAILURE;
	}
	return ret;
}

int make_connect (struct soap *server, const char *endpoint)
{
	/*
	 * Add connect_timeout
         * Dimark [20090713 0000207]
         */
	server->connect_timeout = 60;
	server->recv_timeout    = 300;
	server->send_timeout    = 60;

	soap_set_endpoint( server, endpoint);

    LOG ( LOG_INFO,"make_connect: endpoint %s\n", endpoint);
	return soap_connect_command (server, SOAP_GET, endpoint, "");
}

int exeHttpDownload (char *URL, char *username, char *password, char *localFile,  long *filesize, char *interface)
{
	int ret = 0;
	int readSize = 0;
	unsigned char *inpPtr;
	int cnt = 0;
	int fd = 0;
	char* TimeBuf;
	BOOL isError = FALSE;
	BOOL digest_init_done = 0;
	struct soap soap;

	// get a soap structure and use it's buf for input
	struct soap *tmpSoap;

	// Data for digest authorization
	struct http_da_info da_info;

	soap_init (&soap);
	tmpSoap = &soap;

	// Register Digest Authentication Plugin
	soap_register_plugin (tmpSoap, http_da);

	if (username != NULL && strlen (username) > 0)
	{
		tmpSoap->userid = username;
		tmpSoap->passwd = password;
	}
	if (password != NULL && strlen (password) > 0)
	{
		tmpSoap->passwd = password;
	}

	soap_begin (tmpSoap);
	tmpSoap->keep_alive = 1;
	tmpSoap->status = SOAP_GET;
	tmpSoap->authrealm = "";
	LOG( LOG_DEBUG, "Get: %s Host: %s Port: %d interface: %s\n", URL, tmpSoap->host, tmpSoap->port, interface);

	if(interface != NULL  && strlen(interface)>2)
        tmpSoap->bind_device = interface;
		
	if (make_connect(tmpSoap, URL))
    {
        ret = tmpSoap->error;
		LOG( LOG_DEBUG, "make_connect Request send %d %d\n", ret, tmpSoap->errmode);
		soap_end (tmpSoap);
		soap_done (tmpSoap);
        return DOWNLOAD_FAILURE;
    }

    if (tmpSoap->fpost (tmpSoap, URL, "", 0, tmpSoap->path, "", 0))
	{
		ret = tmpSoap->error;
		LOG( LOG_DEBUG, "fpost Request send %d %d\n", ret, tmpSoap->errmode);
		soap_end (tmpSoap);
		soap_done (tmpSoap);
		return DOWNLOAD_FAILURE;
	}


	// Parse the HTML response header and remove it from the work data
	LOG( LOG_DEBUG, "before parsing...\n");
	ret = tmpSoap->fparse (tmpSoap);
	LOG( LOG_DEBUG, "ret:%d\n", ret);
	switch (ret)
	{
	case SOAP_OK:
		break;
	case 401:		/* Authorization failure */
		http_da_save (tmpSoap,
				&da_info,
				(tmpSoap->authrealm != NULL ? tmpSoap->authrealm : CONNECTION_REALM),
				 username, password);
		digest_init_done = 1;
		make_connect (tmpSoap, URL);
		tmpSoap->fprepareinit (tmpSoap);
		tmpSoap->fpost (tmpSoap, URL, "", 0, tmpSoap->path, "", 0);
		ret = tmpSoap->fparse (tmpSoap);
		if (ret == SOAP_OK)
			break;
		LOG( LOG_DEBUG, "401 Request %d \n", ret);
		soap_closesock (tmpSoap);
		soap_destroy (tmpSoap);
		soap_end (tmpSoap);
		http_da_release(tmpSoap, &da_info);
		soap_done (tmpSoap);
		return FILE_TRANSFER_SERVER_AUTHENTICATION_FAILURE;
	default:
		LOG( LOG_DEBUG, "Request %d \n", ret);
		soap_closesock (tmpSoap);
		soap_destroy (tmpSoap);
		soap_end (tmpSoap);
		soap_done (tmpSoap);
		return DOWNLOAD_FAILURE;
	}

	LOG( LOG_DEBUG, "Request 2 %d endpoint: %s \n", ret, tmpSoap->endpoint);

	inpPtr = tmpSoap->buf;

	tmpSoap->recv_timeout = 30;	// Timeout after 30 seconds stall on recv
	tmpSoap->send_timeout = 60;	// Timeout after 1 minute stall on send

	/*
	 * Add connect_timeout
         * Dimark [20090713 0000207]
         */
	tmpSoap->connect_timeout = 60;

	// open the outputfile, must be given with the complete path
	if ( strlen( localFile ) > 0 ) {
		if ((fd = open (localFile, O_RDWR | O_CREAT, 0777)) < 0)
		{
			LOG( LOG_DEBUG, "Can't open File: %s", localFile);
			return DOWNLOAD_FAILURE;
		}
	} else {
		if ((fd = open ("/tmp/download.dwn", O_RDWR | O_CREAT, 0777)) < 0)
		{
			LOG( LOG_DEBUG, "Can't open File: %s", localFile);
			return DOWNLOAD_FAILURE;
		}
	}

	// the first part of the data is already in the buffer,
	// therefor calculate the offset and write it to the outputfile
	// bufidx is the index to the first data after the HTML header
	cnt = soap.buflen - soap.bufidx;
	inpPtr = (unsigned char *) &soap.buf;
	inpPtr += soap.bufidx;
	if (write (fd, inpPtr, cnt) < 0)
		isError = TRUE;

	// Get the rest of the data
	while (isError == FALSE
	       && (readSize =
		   tmpSoap->frecv (tmpSoap, inpPtr, SOAP_BUFLEN)) > 0)
	{
		if (write (fd, inpPtr, readSize) < 0)
		{
			isError = TRUE;
			break;
		}
		cnt += readSize;

		/*if (cnt % 10)
			LOG( LOG_DEBUG, "cnt: %d  readSize: %d \n", cnt, readSize);*/
	}
    *filesize = cnt;
	
	LOG( LOG_DEBUG, "Read: %d  soap->err: %d \n",
	    	  cnt, tmpSoap->errnum);

	if (close (fd) < 0) {
		LOG( LOG_DEBUG, "Can't close file");
		return DOWNLOAD_FAILURE;
	}
//		return soap_receiver_fault (tmpSoap, "9010", "Can't close file");

	soap_closesock (tmpSoap);
	soap_destroy (tmpSoap);
	soap_end (tmpSoap);
	if ( digest_init_done ) {
		http_da_release(tmpSoap, &da_info);
	}
	soap_done (tmpSoap);

    return _SUCCESS;
}

#ifdef WITH_TR143
static int uploadFile_TR143( struct soap *tmpSoap, long TestFileLength , char *interface, long* tx_bytes)
{
	int ret = _SUCCESS;
	long contentLength;
	char fileSizeStr[20];
	char boundary[20];
	char buf1[64];
    char buf2[64];
    FILE * fp;
	int bufsize = 1023;
	int i = 0;
	
	sprintf (tmpSoap->tmpbuf, "PUT %s HTTP/1.1\r\n", tmpSoap->path);
	soap_send( tmpSoap, tmpSoap->tmpbuf );
    setTR143TimeInfo("ROMTime",0);

	sprintf (tmpSoap->tmpbuf, "%s:%d", tmpSoap->host, tmpSoap->port);
	tmpSoap->fposthdr (tmpSoap, "Host", tmpSoap->tmpbuf);

	tmpSoap->fposthdr (tmpSoap, "User-Agent", "CPE");
	tmpSoap->fposthdr (tmpSoap, "Connection",
			   tmpSoap->keep_alive ? "keep_alive" : "close");

	sprintf (boundary, "%ld", getTime ());
	sprintf (tmpSoap->tmpbuf, "multipart/form-data; boundary=%s", boundary);
	sprintf (fileSizeStr, "--%s", boundary);
	sprintf (boundary, "%s", fileSizeStr);
	tmpSoap->fposthdr (tmpSoap, "Content-Type", tmpSoap->tmpbuf);
	sprintf (tmpSoap->tmpbuf,
		 "%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n",
		 boundary, "UploadDiagnostics", "UploadDiagnostics.txt", "text/plain" );
	// Calc contentlength
	contentLength = strlen(tmpSoap->tmpbuf) + TestFileLength + strlen(boundary) + 4;
	sprintf (fileSizeStr, "%ld", contentLength);
	tmpSoap->fposthdr (tmpSoap, "Content-Length", fileSizeStr);
	soap_send( tmpSoap, "\r\n" );
	soap_send( tmpSoap, tmpSoap->tmpbuf );
	
    setTR143TimeInfo("BOMTime",0);
	//record the current value of the Ethernet bytes sent on the Interface
    memset(buf1,'\0',sizeof(buf1));
    memset(buf2,'\0',sizeof(buf2));
	if(interface != NULL && strlen(interface)>2){
		if(strcmp(interface,"usb0") == 0)
			sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'TX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		else if(strcmp(interface,"usb1") == 0)
			sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 1 | grep 'TX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		else
			sprintf(buf1,"cat /sys/class/net/%s/statistics/tx_bytes",interface);
	} else
		sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'TX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
    fp = popen(buf1,"r");
	if(fp){
        fgets(buf2,sizeof(buf2),fp);
        pclose(fp);
        *tx_bytes = atol(buf2);
	}
	
	LOG (LOG_DEBUG,"contentLength=%ld\n",contentLength);	
	bufsize = sizeof(tmpSoap->tmpbuf);
	memset(tmpSoap->tmpbuf,'1',sizeof(tmpSoap->tmpbuf));
	for(i=(TestFileLength/bufsize);i > 0; i--){
	    if ( soap_send_raw( tmpSoap, tmpSoap->tmpbuf, bufsize ) != SOAP_OK ){
		    soap_end( tmpSoap );
		    soap_done( tmpSoap );
		    return UPLOAD_FAILURE;
		}
	}
	if((TestFileLength%bufsize)>0 ){
	    tmpSoap->tmpbuf[TestFileLength%bufsize] = '\0';
	    if ( soap_send_raw( tmpSoap, tmpSoap->tmpbuf, TestFileLength%bufsize ) != SOAP_OK ){
		    soap_end( tmpSoap );
		    soap_done( tmpSoap );
		    return UPLOAD_FAILURE;
		}
	}

	soap_send( tmpSoap, boundary );
	soap_send( tmpSoap, "--\r\n\r\n" );
//	soap_flush(tmpSoap);
	return _SUCCESS;
}

int exeHttpUpload_TR143 (char *URL, long TestFileLength, char *interface)
{
	int ret = _SUCCESS;
	// get a soap structure and use it's buf for input
	struct soap soap;
	struct soap *tmpSoap;
	char buf1[64];
    char buf2[64];
    FILE * fp;
	long tx_bytes = 0;

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

	soap_init (&soap);
	tmpSoap = &soap;
	tmpSoap->keep_alive = 1;
	soap_set_endpoint(tmpSoap, URL);

    if(interface != NULL  && strlen(interface)>2)
        tmpSoap->bind_device = interface;

    setTR143TimeInfo("TCPOpenRequestTime",0);
	tmpSoap->socket = tmpSoap->fopen(tmpSoap, URL, tmpSoap->host, tmpSoap->port);
    if (tmpSoap->error){
	    #ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	    #else
	    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	    #endif
        return UPLOAD_FAILURE;
	}
	setTR143TimeInfo("TCPOpenResponseTime",0);

	ret = uploadFile_TR143( tmpSoap, TestFileLength, interface, &tx_bytes );
	if( ret != _SUCCESS )
		return ret;
		
	// Parse the HTML response header and remove it from the work data
	ret = tmpSoap->fparse (tmpSoap);
	
	switch( ret ) {
	    //case 200:
		//case 201:
		//case 204:
		case SOAP_OK:
			soap_end (tmpSoap);
			soap_done (tmpSoap);
			
			setTR143TimeInfo("EOMTime",0);
			//Record the current value of the Ethernet bytes sent on the Interface and calculate the TotalBytesSent using previous value sampled at BOM Time.
		    memset(buf1,'\0',sizeof(buf1));
		    memset(buf2,'\0',sizeof(buf2));
			if(interface != NULL && strlen(interface)>2){
				if(strcmp(interface,"usb0") == 0)
					sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'TX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
				else if(strcmp(interface,"usb1") == 0)
					sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 1 | grep 'TX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
				else
					sprintf(buf1,"cat /sys/class/net/%s/statistics/tx_bytes",interface);
			} else
				sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'TX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
			fp = popen(buf1,"r");
			if(fp){
		        fgets(buf2,sizeof(buf2),fp);
		        pclose(fp);
			}
		    sprintf(buf1,"%ld",atol(buf2) - tx_bytes);	
			#ifdef WITH_TR098
			DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.TotalBytesSent")),buf1);
			DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Completed");
			#else
			DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.TotalBytesSent")),buf1);
			DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Completed");
			#endif
			break;
		
		default:			
			LOG (LOG_DEBUG, "Request %d \n", ret);
			soap_end (tmpSoap);
			soap_done (tmpSoap);
			#ifdef WITH_TR098
			DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_NoResponse");
			#else
			DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_NoResponse");
			#endif
			return UPLOAD_FAILURE;
	}
	return ret;
}

int exeHttpDownload_TR143 (char *URL, char *interface)
{
	int ret = _SUCCESS;
	// get a soap structure and use it's buf for input
	struct soap soap;
	struct soap *tmpSoap;
	char buf1[64];
    char buf2[64];
    FILE * fp;
	soap_wchar c;
    long rx_bytes_t = 0;
    long rx_bytes = 0;
	long i=0;
	
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
	DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TotalBytesReceived")),"0");
	DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TCPOpenRequestTime")),"");
	DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TCPOpenResponseTime")),"");
	#endif
	
	soap_init (&soap);
	tmpSoap = &soap;
	tmpSoap->keep_alive = 1;
	soap_set_endpoint(tmpSoap, URL);
    if(interface != NULL && strlen(interface)>2)
        tmpSoap->bind_device = interface;

    setTR143TimeInfo("TCPOpenRequestTime",1);
	tmpSoap->socket = tmpSoap->fopen(tmpSoap, URL, tmpSoap->host, tmpSoap->port);
    if (tmpSoap->error){
	    #ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	    #else
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_InitConnectionFailed");
	    #endif
        return UPLOAD_FAILURE;
	}
	setTR143TimeInfo("TCPOpenResponseTime",1);		
		
    if(soap_connect_command(tmpSoap, SOAP_GET, URL, NULL))
	    return UPLOAD_FAILURE;
    setTR143TimeInfo("ROMTime",1);
	if(soap_begin_recv(tmpSoap)){
		#ifdef WITH_TR098
		DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_NoResponse");
		#else
		DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_NoResponse");
		#endif
		LOG (LOG_DEBUG,"status=%d, error=%d\n", tmpSoap->status, tmpSoap->error);
	} else {
		LOG (LOG_DEBUG,"soap->length=%ld\n",tmpSoap->length);
		sprintf(buf1,"%ld",tmpSoap->length);
        #ifdef WITH_TR098
        DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TestBytesReceived")),buf1);
        #else
        DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TestBytesReceived")),buf1);
        #endif	
	    setTR143TimeInfo("BOMTime",1);
		
		memset(buf1,'\0',sizeof(buf1));
		memset(buf2,'\0',sizeof(buf2));
		if(interface != NULL && strlen(interface)>2){
		    if(strcmp(interface,"usb0") == 0)
		        sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'RX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		    else if(strcmp(interface,"usb1") == 0)
		        sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 1 | grep 'RX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		    else
		        sprintf(buf1,"cat /sys/class/net/%s/statistics/rx_bytes",interface);
		} else
		    sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'RX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		fp = popen(buf1,"r");
		if(fp){
			fgets(buf2,sizeof(buf2),fp);
			pclose(fp);
			rx_bytes_t = atol(buf2);
		}

		if ((tmpSoap->mode & SOAP_IO) == SOAP_IO_CHUNK){
            for (;;)
            { 
		        if ((c = soap_getchar(&soap)) == (int)EOF)
                    break;
                //c is a 8-bit char
		        rx_bytes++;
            }
		} else if(tmpSoap->length){
            for (i = tmpSoap->length; i; i--)
            { 
		        if ((c = soap_getchar(&soap)) == (int)EOF)
                    break;
		        rx_bytes++;
            }
		}
		LOG (LOG_DEBUG,"rx_bytes=%ld\n",rx_bytes);
		setTR143TimeInfo("EOMTime",1);
		//Record the current value of the Ethernet bytes sent on the Interface and calculate the TotalBytesSent using previous value sampled at BOM Time.
		memset(buf1,'\0',sizeof(buf1));
		memset(buf2,'\0',sizeof(buf2));
		if(interface != NULL && strlen(interface)>2){
		    if(strcmp(interface,"usb0") == 0)
		        sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'RX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		    else if(strcmp(interface,"usb1") == 0)
		        sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 1 | grep 'RX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		    else
		        sprintf(buf1,"cat /sys/class/net/%s/statistics/rx_bytes",interface);
		} else
		    sprintf(buf1,"TEST=`lte_get --pkt-statistics -q 0 | grep 'RX Packet Statistics' | cut -d '=' -f2 | cut -d ' ' -f4`;echo -n $TEST");
		fp = popen(buf1,"r");
		if(fp){
			fgets(buf2,sizeof(buf2),fp);
			pclose(fp);
		}
		sprintf(buf1,"%ld",atol(buf2) - rx_bytes_t);	
		#ifdef WITH_TR098
		DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TotalBytesReceived")),buf1);
		#else
		DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TotalBytesReceived")),buf1);
		#endif
		
		if(tmpSoap->length > 0 && tmpSoap->length != rx_bytes){
			#ifdef WITH_TR098
			DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_TransferFailed");
			#else
			DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_TransferFailed");
			#endif
		}else{
			#ifdef WITH_TR098
			DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Completed");
			#else
			DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Completed");
			#endif
		}
	}
	soap_end_recv(tmpSoap);
	soap_end (tmpSoap);
	soap_done (tmpSoap);
	return ret;
}  
#endif
