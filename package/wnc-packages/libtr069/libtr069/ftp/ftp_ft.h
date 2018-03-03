/***************************************************************************
 *   Copyright (C) 2004 by Dimark Tecnologies Inc.                         *
 *   info@dimark.com                                                       *
 *                                                                         *
 ***************************************************************************/
#ifndef ftp_ft_h
#define ftp_ft_h

int ftp_login(char *, const char *, const char *);

int ftp_get( char *, char *, int *);

void ftp_disconnect( );
int ftp_login_adv(char *, const char *, const char * ,int , char *);

#endif 
