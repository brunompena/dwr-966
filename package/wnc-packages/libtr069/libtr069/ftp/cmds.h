#ifndef cmds_H
#define cmds_H


int doLogin( const char *host, const char *user, const char *pass );
int dimget( char *remoteFile, char *localFile, long *size );
int dimput( char *remoteFile, char *localFile, long *size );
void disconnect(void);
void setpassive(void);

#ifdef WITH_TR143
int doLogin_TR143( const char *host, const char *user, const char *pass,int isDownload, char *interface);
int dimget_TR143( char *remoteFile, long *size, char *interface );
int dimput_TR143( char *remoteFile, long TestFileLength, long *size, char *interface );
#endif

#endif
