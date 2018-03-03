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

#ifdef HAVE_FTP_DOWNLOAD

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/file.h>
#ifdef WITH_TR143
#include <sys/timeb.h>
#endif

#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/ftp.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <pwd.h>
#include <stdarg.h>
#include <net/if.h>

#include "ftp_var.h"
#include "logger.h"

/*
 * Options and other state info.
 */
int	rl_inhibit;	/* disable readline support */
int	traceflag;	/* trace packets exchanged */
int	sendport;	/* use PORT cmd for each data connection */
int	connected;	/* connected to server */
int	autologin;	/* establish user account on connection */
int	proxy;		/* proxy server connection active */
int	proxflag;	/* proxy connection exists */
int	sunique;	/* store files on server with unique name */
int	runique;	/* store local files with unique name */
int	mcase;		/* map upper to lower case for mget names */
int	ntflag;		/* use ntin ntout tables for name xlation */
int	mapflag;	/* use mapin mapout templates on file names */
int	code;		/* return/reply code for ftp command */
int	crflag;		/* if 1, strip car. rets. on ascii gets */
char     pasv[64];       /* passive port for proxy data connection */
int      passivemode;    /* passive mode enabled */
char	*altarg;	/* argv[1] with no shell-like preprocessing  */
char	ntin[17];	/* input translation table */
char	ntout[17];	/* output translation table */
char	mapin[MAXPATHLEN];	/* input map template */
char	mapout[MAXPATHLEN];	/* output map template */
char	typename[32];		/* name of file transfer type */
int	type;			/* requested file transfer type */
int	curtype;		/* current file transfer type */
char	structname[32];		/* name of file transfer structure */
int	stru;			/* file transfer structure */
char	formname[32];		/* name of file transfer format */
int	form;			/* file transfer format */
char	modename[32];		/* name of file transfer mode */
int	mode;			/* file transfer mode */
char	bytename[32];		/* local byte size in ascii */
int	bytesize;		/* local byte size in binary */

char	*hostname;	/* name of host connected to */
int	unix_server;	/* server is unix, can use binary for ascii */
int	unix_proxy;	/* proxy is unix, can use binary for ascii */
/*Extern struct	servent *sp;*/	/* service spec for tcp/ftp */
int	ftp_port;	/* htons'd port number for ftp service */

sigjmp_buf toplevel;	/* non-local goto stuff for cmd scanner */

char	line[200];	/* input line buffer */
char	*stringbase;	/* current scan point in line buffer */
char	argbuf[200];	/* argument storage buffer */
char	*argbase;	/* current storage point in arg buffer */
int	cpend;		/* flag: if != 0, then pending server reply */
int	mflag;		/* flag: if != 0, then active multi command */

int	options;	/* used during socket creation */

int data = -1;

static struct sockaddr_in hisctladdr;
static struct sockaddr_in data_addr;
static struct sockaddr_in myctladdr;
static int ptflag = 0;
static sigjmp_buf recvabort;
static sigjmp_buf sendabort;
static sigjmp_buf ptabort;
static int ptabflg = 0;
static int abrtflag = 0;

char	bind_device[8];
#ifdef WITH_TR143
void setTR143TimeInfo(char * param,int isDownload);
#endif

void lostpeer(int);
extern int connected;

static char *gunique(char *);
static void proxtrans(const char *cmd, char *local, char *remote);
static int initconn(void);
static void abort_remote(FILE *din);
static int getreply(int );
static FILE *dataconn(const char *);

FILE *cin, *cout;

char *
hookup(const char *host, int port)
{
	register struct hostent *hp = 0;
	int s;
	socklen_t len;
	static char hostnamebuf[256];

	memset(&hisctladdr, 0, sizeof(hisctladdr));
	if (inet_aton(host, &hisctladdr.sin_addr)) {
		hisctladdr.sin_family = AF_INET;
		strncpy(hostnamebuf, host, sizeof(hostnamebuf));
		hostnamebuf[sizeof(hostnamebuf)-1]=0;
	} 
	else {
		hp = gethostbyname(host);
		if (hp == NULL) {
			// fprintf(stderr, "ftp: %s: ", host);
			code = -1;
			return((char *) 0);
		}
		hisctladdr.sin_family = hp->h_addrtype;
		if (hp->h_length > (int)sizeof(hisctladdr.sin_addr)) {
			hp->h_length = sizeof(hisctladdr.sin_addr);
		}
		memcpy(&hisctladdr.sin_addr, hp->h_addr_list[0], hp->h_length);
		(void) strncpy(hostnamebuf, hp->h_name, sizeof(hostnamebuf));
		hostnamebuf[sizeof(hostnamebuf)-1] = 0;
	}
	hostname = hostnamebuf;
	s = socket(hisctladdr.sin_family, SOCK_STREAM, 0);
	if (s < 0) {
		; // perror("ftp: socket");
		code = -1;
		return (0);
	}
//#ifdef _JPR_AM_
	if (strlen(bind_device) > 2) { 
		struct ifreq ifr;
		memset(&ifr,0,sizeof(ifr));
		snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),bind_device);
		
		if (setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, 
							sizeof(ifr)) < 0 ) {
			LOG(LOG_DEBUG, "%s bind to device %s failed\n",__func__,bind_device);
		} else
		    LOG(LOG_DEBUG, "%s bind to device %s OK\n",__func__,bind_device);
	}
//#endif
	hisctladdr.sin_port = port;
	while (connect(s, (struct sockaddr *)&hisctladdr, sizeof (hisctladdr)) < 0) {
		if (hp && hp->h_addr_list[1]) {
			int oerrno = errno;
			errno = oerrno;
			; // perror((char *) 0);
			hp->h_addr_list++;
			memcpy(&hisctladdr.sin_addr, hp->h_addr_list[0], 
			       hp->h_length);
			(void) close(s);
			s = socket(hisctladdr.sin_family, SOCK_STREAM, 0);
			if (s < 0) {
				code = -1;
				return (0);
			}
			continue;
		}
		code = -1;
		goto bad;
	}
	len = sizeof (myctladdr);
	if (getsockname(s, (struct sockaddr *)&myctladdr, &len) < 0) {
		code = -1;
		goto bad;
	}
	cin = fdopen(s, "r");
	cout = fdopen(s, "w");
	if (cin == NULL || cout == NULL) {
		if (cin)
			(void) fclose(cin);
		if (cout)
			(void) fclose(cout);
		code = -1;
		goto bad;
	}
	if (getreply(0) > 2) { 	/* read startup message from server */
		if (cin)
			(void) fclose(cin);
		if (cout)
			(void) fclose(cout);
		code = -1;
		goto bad;
	}
#ifdef SO_OOBINLINE
	{
	int on = 1;

	if (setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on)) < 0 ) {
			; // perror("ftp: setsockopt");
		}
	}
#endif /* SO_OOBINLINE */

	return (hostname);
bad:
	(void) close(s);
	return ((char *)0);
}

int
command(const char *fmt, ...)
{
	va_list ap;
	int r;
	void (*oldintr)(int) = NULL;

	abrtflag = 0;
	if (cout == NULL) {
		; // perror ("No control connection for command");
		code = -1;
		return (0);
	}
	va_start(ap, fmt);
	vfprintf(cout, fmt, ap);
	va_end(ap);
	fprintf(cout, "\r\n");
	(void) fflush(cout);
	cpend = 1;
	r = getreply(!strcmp(fmt, "QUIT"));
	if (abrtflag && oldintr != SIG_IGN)
		(*oldintr)(SIGINT);
	(void) signal(SIGINT, oldintr);
	return(r);
}

char reply_string[BUFSIZ];		/* last line of previous reply */

#include <ctype.h>

static int
getreply(int expecteof)
{
	register int c, n;
	register int dig;
	register char *cp;
	int originalcode = 0, continuation = 0;
	void (*oldintr)(int) = NULL;
	int pflag = 0;
	size_t px = 0;
	size_t psize = sizeof(pasv);

	for (;;) {
		dig = n = code = 0;
		cp = reply_string;
		while ((c = getc(cin)) != '\n') {
			if (c == IAC) {     /* handle telnet commands */
				switch (c = getc(cin)) {
				case WILL:
				case WONT:
					c = getc(cin);
					fprintf(cout, "%c%c%c", IAC, DONT, c);
					(void) fflush(cout);
					break;
				case DO:
				case DONT:
					c = getc(cin);
					fprintf(cout, "%c%c%c", IAC, WONT, c);
					(void) fflush(cout);
					break;
				default:
					break;
				}
				continue;
			}
			dig++;
			if (c == EOF) {
				if (expecteof) {
					(void) signal(SIGINT,oldintr);
					code = 221;
					return (0);
				}
				lostpeer(0);
				code = 421;
				return(4);
			}
			if (dig < 4 && isdigit(c))
				code = code * 10 + (c - '0');
			if (!pflag && code == 227)
				pflag = 1;
			if (dig > 4 && pflag == 1 && isdigit(c))
				pflag = 2;
			if (pflag == 2) {
				if (c != '\r' && c != ')') {
					if (px < psize-1) pasv[px++] = c;
				}
				else {
					pasv[px] = '\0';
					pflag = 3;
				}
			}
			if (dig == 4 && c == '-') {
				if (continuation)
					code = 0;
				continuation++;
			}
			if (n == 0)
				n = c;
			if (cp < &reply_string[sizeof(reply_string) - 1])
				*cp++ = c;
		}
		if (continuation && code != originalcode) {
			if (originalcode == 0)
				originalcode = code;
			continue;
		}
		*cp = '\0';
		if (n != '1')
			cpend = 0;
		if (code == 421 || originalcode == 421)
			lostpeer(0);
		return (n - '0');
	}
}

static int
empty(fd_set *mask, int hifd, int sec)
{
	struct timeval t;

	t.tv_sec = (long) sec;
	t.tv_usec = 0;
	return(select(hifd+1, mask, (fd_set *) 0, (fd_set *) 0, &t));
}

static void
abortsend(int ignore)
{
	(void)ignore;

	mflag = 0;
	abrtflag = 0;
	// printf("\nsend aborted\nwaiting for remote to finish abort\n");
#ifndef TEAMF1
	(void) fflush(stdout);
#endif
	siglongjmp(sendabort, 1);
}



/** Send a file from local filename to remote 
 * no restart support
 *
 * @return errorcode 	0 = OK , > 1 = Error
 */
int 
sendrequest(const char *cmd, char *local, char *remote, long *size )
{
	register int c, d;
	FILE *volatile fin, *volatile dout = 0;
	int (*volatile closefunc)(FILE *);
	void (*volatile oldintr)(int);
	void (*volatile oldintp)(int);
	volatile long bytes = 0;
	char buf[BUFSIZ], *bufp;
	const char *volatile lmode;

	*size = -1L;
	
	if (proxy) {
		proxtrans(cmd, local, remote);
		return 0;
	}
	if (curtype != type)
		changetype(type, 0);
	closefunc = NULL;
	oldintr = NULL;
	oldintp = NULL;
	lmode = "w";
	if (sigsetjmp(sendabort, 1)) {
		while (cpend) {
			(void) getreply(0);
		}
		if (data >= 0) {
			(void) close(data);
			data = -1;
		}
		if (oldintr)
			(void) signal(SIGINT,oldintr);
		if (oldintp)
			(void) signal(SIGPIPE,oldintp);
		code = -1;
		return 1;
	}
	oldintr = signal(SIGINT, abortsend);
	// Pipe and stdin are not allowed as filesource
	/*	if (strcmp(local, "-") == 0)
		fin = stdin;
	else 
	if (*local == '|') {
		oldintp = signal(SIGPIPE,SIG_IGN);
		fin = popen(local + 1, "r");
		if (fin == NULL) {
			; // perror(local + 1);
			(void) signal(SIGINT, oldintr);
			(void) signal(SIGPIPE, oldintp);
			code = -1;
			return 1;
		}
		closefunc = pclose;
	} else { */
		fin = fopen(local, "r");
		if (fin == NULL) {
			// fprintf(stderr, "local: %s: %s\n", local, strerror(errno));
			(void) signal(SIGINT, oldintr);
			code = -1;
			return 1;
		}
		closefunc = fclose;
/* It is very convenient to be able to send block and character special files too */
#ifdef ONLY_SEND_PLAIN_FILES
		if (fstat(fileno(fin), &st) < 0 ||
		    (st.st_mode&S_IFMT) != S_IFREG) {
			// fprintf(stdout, "%s: not a plain file.\n", local);
			(void) signal(SIGINT, oldintr);
			fclose(fin);
			code = -1;
			return 1;
		}
#endif
//	}
	if (initconn()) {
		(void) signal(SIGINT, oldintr);
		if (oldintp)
			(void) signal(SIGPIPE, oldintp);
		code = -1;
		if (closefunc != NULL)
			(*closefunc)(fin);
		return 1;
	}
	if (sigsetjmp(sendabort, 1))
		goto abort;

	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM) {
			(void) signal(SIGINT, oldintr);
			if (oldintp)
				(void) signal(SIGPIPE, oldintp);
			if (closefunc != NULL)
				(*closefunc)(fin);
			return 1;
		}
	} else
		if (command("%s", cmd) != PRELIM) {
			(void) signal(SIGINT, oldintr);
			if (oldintp)
				(void) signal(SIGPIPE, oldintp);
			if (closefunc != NULL)
				(*closefunc)(fin);
			return 1;
		}
	dout = dataconn(lmode);
	if (dout == NULL)
		goto abort;
	oldintp = signal(SIGPIPE, SIG_IGN);
	switch (curtype) {

	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		while ((c = read(fileno(fin), buf, sizeof (buf))) > 0) {
			bytes += c;
			for (bufp = buf; c > 0; c -= d, bufp += d)
				if ((d = write(fileno(dout), bufp, c)) <= 0)
					break;
		}
		if (d < 0) {
			if (errno != EPIPE) 
				; // perror("netout");
			bytes = -1;
		} else {
			*size = bytes;
		}
		break;
	}
	if (closefunc != NULL)
		(*closefunc)(fin);
	(void) fclose(dout);
	/* closes data as well, so discard it */
	data = -1;
	(void) getreply(0);
	(void) signal(SIGINT, oldintr);
	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	return 0;
abort:
	(void) signal(SIGINT, oldintr);
	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	if (!cpend) {
		code = -1;
		return 1;
	}
	if (dout) {
		(void) fclose(dout);
	}
	if (data >= 0) {
		/* if it just got closed with dout, again won't hurt */
		(void) close(data);
		data = -1;
	}
	(void) getreply(0);
	code = -1;
	if (closefunc != NULL && fin != NULL)
		(*closefunc)(fin);
	return 1;
}

static void
abortrecv(int ignore)
{
	(void)ignore;

	mflag = 0;
	abrtflag = 0;
	debug("\nreceive aborted\nwaiting for remote to finish abort\n");
#ifndef TEAMF1
	(void) fflush(stdout);
#endif
	siglongjmp(recvabort, 1);
}

/** receive a remote file and store it in local file
 *  only binary is supported.
 *  no support for rest transfers.
 * 
 * @return errorcode	0 = OK, > 1 Error
 */
int
recvrequest(const char *cmd, char *volatile local, char *remote, const char *lmode, long *size )
{
	FILE *volatile fout, *volatile din = 0;
	int (*volatile closefunc)(FILE *);
	void (*volatile oldintp)(int);
	void (*volatile oldintr)(int);
	volatile int is_retr, tcrflag;
	static unsigned bufsize;
	static char *buf;
	volatile long bytes = 0;
	register int c, d;
	struct stat st;

	*size = -1;
	is_retr = strcmp(cmd, "RETR") == 0;

	if (proxy && is_retr) {
		proxtrans(cmd, local, remote);
		return 0;
	}
	closefunc = NULL;
	oldintr = NULL;
	oldintp = NULL;
	tcrflag = !crflag && is_retr;
	if (sigsetjmp(recvabort, 1)) {
		while (cpend) {
			(void) getreply(0);
		}
		if (data >= 0) {
			(void) close(data);
			data = -1;
		}
		if (oldintr)
			(void) signal(SIGINT, oldintr);
		code = -1;
		return 1;
	}
	oldintr = signal(SIGINT, abortrecv);
	if (strcmp(local, "-") && *local != '|') {
		if (access(local, W_OK) < 0) {
			char *dir = rindex(local, '/');

			if (errno != ENOENT && errno != EACCES) {
				(void) signal(SIGINT, oldintr);
				code = -1;
				return 1;
			}
			if (dir != NULL)
				*dir = 0;
			d = access(dir ? local : ".", W_OK);
			if (dir != NULL)
				*dir = '/';
			if (d < 0) {
//				fprintf(stderr, "local: %s: %s\n", local, strerror(errno));
				(void) signal(SIGINT, oldintr);
				code = -1;
				return 1;
			}
			if (!runique && errno == EACCES &&
			    chmod(local, 0600) < 0) {
				/*
				 * Believe it or not, this was actually
				 * repeated in the original source.
				 */
				(void) signal(SIGINT, oldintr);
				/*(void) signal(SIGINT, oldintr);*/
				code = -1;
				return 1;
			}
			if (runique && errno == EACCES &&
			   (local = gunique(local)) == NULL) {
				(void) signal(SIGINT, oldintr);
				code = -1;
				return 1;
			}
		}
		else if (runique && (local = gunique(local)) == NULL) {
			(void) signal(SIGINT, oldintr);
			code = -1;
			return 1;
		}
	}
	if (curtype != type) {
		changetype(type, 0);
	}
	if (initconn()) {
		(void) signal(SIGINT, oldintr);
		code = -1;
		return 1;
	}
	if (sigsetjmp(recvabort, 1))
		goto abort;
	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM) {
			(void) signal(SIGINT, oldintr);
			return 1;
		}
	} 
	else {
		if (command("%s", cmd) != PRELIM) {
			(void) signal(SIGINT, oldintr);
			return 1;
		}
	}
	din = dataconn("r");
	if (din == NULL)
		goto abort;
	fout = fopen(local, lmode);
	if (fout == NULL) {
		goto abort;
	}
	closefunc = fclose;
	
	if (fstat(fileno(fout), &st) < 0 || st.st_blksize == 0)
		st.st_blksize = BUFSIZ;
	if (st.st_blksize > bufsize) {
		if (buf)
			(void) free(buf);
		buf = malloc((unsigned)st.st_blksize);
		if (buf == NULL) {
			bufsize = 0;
			goto abort;
		}
		bufsize = st.st_blksize;
	}
	switch (curtype) {

	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		while ((c = read(fileno(din), buf, bufsize)) > 0) {
			if ((d = write(fileno(fout), buf, c)) != c)
				break;
			bytes += c;
		}
		if (c < 0) {
			if (errno != EPIPE)
				; // perror("netin");
			bytes = -1;
		}
		if (d < c) {
			// don't do any thing to break this 
/*			if (d < 0)
				fprintf(stderr, "local: %s: %s\n", local,strerror(errno));
			else
				fprintf(stderr, "%s: short write\n", local);
*/
		}
		*size = bytes;
		break;

	}
	if (closefunc != NULL)
		(*closefunc)(fout);
	(void) signal(SIGINT, oldintr);
	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	(void) fclose(din);
	/* closes data as well, so discard it */
	data = -1;
	(void) getreply(0);
	if (bytes > 0 && is_retr)
		*size = bytes;
	return 0;
abort:

/* abort using RFC959 recommended IP,SYNC sequence  */

	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	(void) signal(SIGINT, SIG_IGN);
	if (!cpend) {
		code = -1;
		(void) signal(SIGINT, oldintr);
		return 1;
	}

	abort_remote(din);
	code = -1;
	if (closefunc != NULL && fout != NULL)
		(*closefunc)(fout);
	if (din) {
		(void) fclose(din);
	}
	if (data >= 0) {
		/* if it just got closed with din, again won't hurt */
		(void) close(data);
		data = -1;
	}
	if (bytes > 0)
		*size = bytes;
	(void) signal(SIGINT, oldintr);
	return 1;
}

/*
 * Need to start a listen on the data channel before we send the command,
 * otherwise the server's connect may fail.
 */
static int
initconn(void)
{
	register char *p, *a;
	int result, tmpno = 0;
	socklen_t len;
	int on = 1;
	int tos;
	u_long a1,a2,a3,a4,p1,p2;

	if (passivemode) {
		data = socket(AF_INET, SOCK_STREAM, 0);
		if (data < 0) {
			;// perror("ftp: socket");
			return(1);
		}
//#ifdef _JPR_AM_
		if (strlen(bind_device) > 2) { 
			struct ifreq ifr;
			memset(&ifr,0,sizeof(ifr));
			snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),bind_device);
			
			if (setsockopt(data, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, 
								sizeof(ifr)) < 0 ) {
				LOG(LOG_DEBUG, "%s bind to device %s failed\n",__func__,bind_device);
			} else
			    LOG(LOG_DEBUG, "%s bind to device %s OK\n",__func__,bind_device);
		}
//#endif
		if (options & SO_DEBUG &&
		    setsockopt(data, SOL_SOCKET, SO_DEBUG, (char *)&on,
			       sizeof (on)) < 0)
			; //perror("ftp: setsockopt (ignored)");
		if (command("PASV") != COMPLETE) {
			// printf("Passive mode refused.\n");
			return(1);
		}

		/*
		 * What we've got at this point is a string of comma separated
		 * one-byte unsigned integer values, separated by commas.
		 * The first four are the an IP address. The fifth is the MSB
		 * of the port number, the sixth is the LSB. From that we'll
		 * prepare a sockaddr_in.
		 */

		if (sscanf(pasv,"%ld,%ld,%ld,%ld,%ld,%ld",
			   &a1,&a2,&a3,&a4,&p1,&p2)
		    != 6) 
		{
			// printf("Passive mode address scan failure. Shouldn't happen!\n");
			return(1);
		}

		data_addr.sin_family = AF_INET;
		data_addr.sin_addr.s_addr = htonl((a1 << 24) | (a2 << 16) |
						  (a3 << 8) | a4);
		data_addr.sin_port = htons((p1 << 8) | p2);

		if (connect(data, (struct sockaddr *) &data_addr,
		    sizeof(data_addr))<0) {
			; // perror("ftp: connect");
			return(1);
		}

#ifdef IP_TOS
		tos = IPTOS_THROUGHPUT;
		if (setsockopt(data, IPPROTO_IP, IP_TOS, (char *)&tos,
		    sizeof(tos)) < 0)
			; // perror("ftp: setsockopt TOS (ignored)");
#endif
		return(0);
	}
noport:
	data_addr = myctladdr;
	if (sendport)
		data_addr.sin_port = 0;	/* let system pick one */ 
	if (data != -1)
		(void) close(data);
	data = socket(AF_INET, SOCK_STREAM, 0);
	if (data < 0) {
		; // perror("ftp: socket");
		if (tmpno)
			sendport = 1;
		return (1);
	}
//#ifdef _JPR_AM_
	if (strlen(bind_device) > 2) { 
		struct ifreq ifr;
		memset(&ifr,0,sizeof(ifr));
		snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),bind_device);
		
		if (setsockopt(data, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, 
							sizeof(ifr)) < 0 ) {
			LOG(LOG_DEBUG, "%s bind to device %s failed\n",__func__,bind_device);
		} else
		    LOG(LOG_DEBUG, "%s bind to device %s OK\n",__func__,bind_device);
	}
//#endif
	if (!sendport)
		if (setsockopt(data, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof (on)) < 0) {
			; // perror("ftp: setsockopt (reuse address)");
			goto bad;
		}
	if (bind(data, (struct sockaddr *)&data_addr, sizeof (data_addr)) < 0) {
		; // perror("ftp: bind");
		goto bad;
	}
	if (options & SO_DEBUG &&
	    setsockopt(data, SOL_SOCKET, SO_DEBUG, (char *)&on, sizeof (on)) < 0)
		; // perror("ftp: setsockopt (ignored)");
	len = sizeof (data_addr);
	if (getsockname(data, (struct sockaddr *)&data_addr, &len) < 0) {
		; // perror("ftp: getsockname");
		goto bad;
	}
	if (listen(data, 1) < 0)
		; // perror("ftp: listen");
	if (sendport) {
		a = (char *)&data_addr.sin_addr;
		p = (char *)&data_addr.sin_port;
#define	UC(b)	(((int)b)&0xff)
		result =
		    command("PORT %d,%d,%d,%d,%d,%d",
		      UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]),
		      UC(p[0]), UC(p[1]));
		if (result == ERROR && sendport == -1) {
			sendport = 0;
			tmpno = 1;
			goto noport;
		}
		return (result != COMPLETE);
	}
	if (tmpno)
		sendport = 1;
	return (0);
bad:
	(void) close(data), data = -1;
	if (tmpno)
		sendport = 1;
	return (1);
}

static FILE *
dataconn(const char *lmode)
{
	struct sockaddr_in from;
	int s;
	socklen_t fromlen = sizeof(from);

        if (passivemode)
            return (fdopen(data, lmode));

	s = accept(data, (struct sockaddr *) &from, &fromlen);
	if (s < 0) {
		; // perror("ftp: accept");
		(void) close(data), data = -1;
		return (NULL);
	}
	(void) close(data);
	data = s;
	return (fdopen(data, lmode));
}


static 
void
psabort(int ignore)
{
	(void)ignore;
	abrtflag++;
}

void
pswitch(int flag)
{
	void (*oldintr)(int);
	static struct comvars {
		int connect;
		char name[MAXHOSTNAMELEN];
		struct sockaddr_in mctl;
		struct sockaddr_in hctl;
		FILE *in;
		FILE *out;
		int tpe;
		int curtpe;
		int cpnd;
		int sunqe;
		int runqe;
		int mcse;
		int ntflg;
		char nti[17];
		char nto[17];
		int mapflg;
		char mi[MAXPATHLEN];
		char mo[MAXPATHLEN];
	} proxstruct, tmpstruct;
	struct comvars *ip, *op;

	abrtflag = 0;
	oldintr = signal(SIGINT, psabort);
	if (flag) {
		if (proxy)
			return;
		ip = &tmpstruct;
		op = &proxstruct;
		proxy++;
	} 
	else {
		if (!proxy)
			return;
		ip = &proxstruct;
		op = &tmpstruct;
		proxy = 0;
	}
	ip->connect = connected;
	connected = op->connect;
	if (hostname) {
		(void) strncpy(ip->name, hostname, sizeof(ip->name) - 1);
		ip->name[strlen(ip->name)] = '\0';
	} 
	else {
		ip->name[0] = 0;
	}
	hostname = op->name;
	ip->hctl = hisctladdr;
	hisctladdr = op->hctl;
	ip->mctl = myctladdr;
	myctladdr = op->mctl;
	ip->in = cin;
	cin = op->in;
	ip->out = cout;
	cout = op->out;
	ip->tpe = type;
	type = op->tpe;
	ip->curtpe = curtype;
	curtype = op->curtpe;
	ip->cpnd = cpend;
	cpend = op->cpnd;
	ip->sunqe = sunique;
	sunique = op->sunqe;
	ip->runqe = runique;
	runique = op->runqe;
	ip->mcse = mcase;
	mcase = op->mcse;
	ip->ntflg = ntflag;
	ntflag = op->ntflg;
	(void) strncpy(ip->nti, ntin, 16);
	(ip->nti)[strlen(ip->nti)] = '\0';
	(void) strcpy(ntin, op->nti);
	(void) strncpy(ip->nto, ntout, 16);
	(ip->nto)[strlen(ip->nto)] = '\0';
	(void) strcpy(ntout, op->nto);
	ip->mapflg = mapflag;
	mapflag = op->mapflg;
	(void) strncpy(ip->mi, mapin, MAXPATHLEN - 1);
	(ip->mi)[strlen(ip->mi)] = '\0';
	(void) strcpy(mapin, op->mi);
	(void) strncpy(ip->mo, mapout, MAXPATHLEN - 1);
	(ip->mo)[strlen(ip->mo)] = '\0';
	(void) strcpy(mapout, op->mo);
	(void) signal(SIGINT, oldintr);
	if (abrtflag) {
		abrtflag = 0;
		(*oldintr)(SIGINT);
	}
}

static
void
abortpt(int ignore)
{
	(void)ignore;
	debug("\n");
#ifndef TEAMF1
	fflush(stdout);
#endif
	ptabflg++;
	mflag = 0;
	abrtflag = 0;
	siglongjmp(ptabort, 1);
}

static void
proxtrans(const char *cmd, char *local, char *remote)
{
	void (*volatile oldintr)(int);
	volatile int secndflag = 0, prox_type, nfnd;
	const char *volatile cmd2;
	fd_set mask;

	if (strcmp(cmd, "RETR"))
		cmd2 = "RETR";
	else
		cmd2 = runique ? "STOU" : "STOR";
	if ((prox_type = type) == 0) {
		if (unix_server && unix_proxy)
			prox_type = TYPE_I;
		else
			prox_type = TYPE_A;
	}
	if (curtype != prox_type)
		changetype(prox_type, 1);
	if (command("PASV") != COMPLETE) {
		debug("proxy server does not support third party transfers.\n");
		return;
	}
	pswitch(0);
	if (!connected) {
		debug("No primary connection\n");
		pswitch(1);
		code = -1;
		return;
	}
	if (curtype != prox_type)
		changetype(prox_type, 1);
	if (command("PORT %s", pasv) != COMPLETE) {
		pswitch(1);
		return;
	}
	if (sigsetjmp(ptabort, 1))
		goto abort;
	oldintr = signal(SIGINT, abortpt);
	if (command("%s %s", cmd, remote) != PRELIM) {
		(void) signal(SIGINT, oldintr);
		pswitch(1);
		return;
	}
	sleep(2);
	pswitch(1);
	secndflag++;
	if (command("%s %s", cmd2, local) != PRELIM)
		goto abort;
	ptflag++;
	(void) getreply(0);
	pswitch(0);
	(void) getreply(0);
	(void) signal(SIGINT, oldintr);
	pswitch(1);
	ptflag = 0;
	debug("local: %s remote: %s\n", local, remote);
	return;
abort:
	(void) signal(SIGINT, SIG_IGN);
	ptflag = 0;
	if (strcmp(cmd, "RETR") && !proxy)
		pswitch(1);
	else if (!strcmp(cmd, "RETR") && proxy)
		pswitch(0);
	if (!cpend && !secndflag) {  /* only here if cmd = "STOR" (proxy=1) */
		if (command("%s %s", cmd2, local) != PRELIM) {
			pswitch(0);
			if (cpend)
				abort_remote((FILE *) NULL);
		}
		pswitch(1);
		if (ptabflg)
			code = -1;
		(void) signal(SIGINT, oldintr);
		return;
	}
	if (cpend)
		abort_remote((FILE *) NULL);
	pswitch(!proxy);
	if (!cpend && !secndflag) {  /* only if cmd = "RETR" (proxy=1) */
		if (command("%s %s", cmd2, local) != PRELIM) {
			pswitch(0);
			if (cpend)
				abort_remote((FILE *) NULL);
			pswitch(1);
			if (ptabflg)
				code = -1;
			(void) signal(SIGINT, oldintr);
			return;
		}
	}
	if (cpend)
		abort_remote((FILE *) NULL);
	pswitch(!proxy);
	if (cpend) {
		FD_ZERO(&mask);
		FD_SET(fileno(cin), &mask);
		if ((nfnd = empty(&mask, fileno(cin), 10)) <= 0) {
			if (nfnd < 0) {
				; // perror("abort");
			}
			if (ptabflg)
				code = -1;
			lostpeer(0);
		}
		(void) getreply(0);
		(void) getreply(0);
	}
	if (proxy)
		pswitch(0);
	pswitch(1);
	if (ptabflg)
		code = -1;
	(void) signal(SIGINT, oldintr);
}

void
reset(void)
{
	fd_set mask;
	int nfnd = 1;

	FD_ZERO(&mask);
	while (nfnd > 0) {
		FD_SET(fileno(cin), &mask);
		if ((nfnd = empty(&mask, fileno(cin), 0)) < 0) {
			; // perror("reset");
			code = -1;
			lostpeer(0);
		}
		else if (nfnd) {
			(void) getreply(0);
		}
	}
}

static char *
gunique(char *local)
{
	static char new[MAXPATHLEN];
	char *cp = rindex(local, '/');
	int d, count=0;
	char ext = '1';

	if (cp)
		*cp = '\0';
	d = access(cp ? local : ".", W_OK);
	if (cp)
		*cp = '/';
	if (d < 0) {
		fprintf(stderr, "local: %s: %s\n", local, strerror(errno));
		return((char *) 0);
	}
	(void) strcpy(new, local);
	cp = new + strlen(new);
	*cp++ = '.';
	while (!d) {
		if (++count == 100) {
			debug("runique: can't find unique file name.\n");
			return((char *) 0);
		}
		*cp++ = ext;
		*cp = '\0';
		if (ext == '9')
			ext = '0';
		else
			ext++;
		if ((d = access(new, F_OK)) < 0)
			break;
		if (ext != '0')
			cp--;
		else if (*(cp - 2) == '.')
			*(cp - 1) = '1';
		else {
			*(cp - 2) = *(cp - 2) + 1;
			cp--;
		}
	}
	return(new);
}

static void
abort_remote(FILE *din)
{
	char buf[BUFSIZ];
	int nfnd, hifd;
	fd_set mask;

	/*
	 * send IAC in urgent mode instead of DM because 4.3BSD places oob mark
	 * after urgent byte rather than before as is protocol now
	 */
	snprintf(buf, sizeof(buf), "%c%c%c", IAC, IP, IAC);
	if (send(fileno(cout), buf, 3, MSG_OOB) != 3)
		; // perror("abort");
	fprintf(cout,"%cABOR\r\n", DM);
	(void) fflush(cout);
	FD_ZERO(&mask);
	FD_SET(fileno(cin), &mask);
	hifd = fileno(cin);
	if (din) { 
		FD_SET(fileno(din), &mask);
		if (hifd < fileno(din)) hifd = fileno(din);
	}
	if ((nfnd = empty(&mask, hifd, 10)) <= 0) {
		if (nfnd < 0) {
			; // perror("abort");
		}
		if (ptabflg)
			code = -1;
		lostpeer(0);
	}
	if (din && FD_ISSET(fileno(din), &mask)) {
		while (read(fileno(din), buf, BUFSIZ) > 0)
			/* LOOP */;
	}
	if (getreply(0) == ERROR && code == 552) {
		/* 552 needed for nic style abort */
		(void) getreply(0);
	}
	(void) getreply(0);
}

void
lostpeer(int ignore)
{
        (void)ignore;

        if (connected) {
                if (cout != NULL) {
                        shutdown(fileno(cout), 1+1);
                        fclose(cout);
                        cout = NULL;
                }
                if (data >= 0) {
                        shutdown(data, 1+1);
                        close(data);
                        data = -1;
                }
                connected = 0;
        }
        pswitch(1);
        if (connected) {
                if (cout != NULL) {
                        shutdown(fileno(cout), 1+1);
                        fclose(cout);
                        cout = NULL;
                }
                connected = 0;
        }
        proxflag = 0;
        pswitch(0);
}

char *
hookup_adv(const char *host, int port, char *interface)
{
	register struct hostent *hp = 0;
	int s;
	socklen_t len;
	static char hostnamebuf[256];

	memset(&hisctladdr, 0, sizeof(hisctladdr));
	if (inet_aton(host, &hisctladdr.sin_addr)) {
		hisctladdr.sin_family = AF_INET;
		strncpy(hostnamebuf, host, sizeof(hostnamebuf));
		hostnamebuf[sizeof(hostnamebuf)-1]=0;
	} 
	else {
		hp = gethostbyname(host);
		if (hp == NULL) {
			// fprintf(stderr, "ftp: %s: ", host);
			code = -1;
			return((char *) 0);
		}
		hisctladdr.sin_family = hp->h_addrtype;
		if (hp->h_length > (int)sizeof(hisctladdr.sin_addr)) {
			hp->h_length = sizeof(hisctladdr.sin_addr);
		}
		memcpy(&hisctladdr.sin_addr, hp->h_addr_list[0], hp->h_length);
		(void) strncpy(hostnamebuf, hp->h_name, sizeof(hostnamebuf));
		hostnamebuf[sizeof(hostnamebuf)-1] = 0;
	}
	hostname = hostnamebuf;
	s = socket(hisctladdr.sin_family, SOCK_STREAM, 0);
	if (s < 0) {
		; // perror("ftp: socket");
		code = -1;
		return (0);
	}
//#ifdef _JPR_AM_
	if (interface!= NULL && strlen(interface) > 2) {
		struct ifreq ifr;
		memset(&ifr,0,sizeof(ifr));
		snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),interface);
		
		if (setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, 
							sizeof(ifr)) < 0 ) {
			LOG(LOG_DEBUG, "%s bind to device %s failed\n",__func__,interface);
		} else
		    LOG(LOG_DEBUG, "%s bind to device %s OK\n",__func__,interface);
	}
//#endif
	hisctladdr.sin_port = port;
	while (connect(s, (struct sockaddr *)&hisctladdr, sizeof (hisctladdr)) < 0) {
		if (hp && hp->h_addr_list[1]) {
			int oerrno = errno;
			errno = oerrno;
			; // perror((char *) 0);
			hp->h_addr_list++;
			memcpy(&hisctladdr.sin_addr, hp->h_addr_list[0], 
			       hp->h_length);
			(void) close(s);
			s = socket(hisctladdr.sin_family, SOCK_STREAM, 0);
			if (s < 0) {
				code = -1;
				return (0);
			}
			continue;
		}
		code = -1;
		goto bad;
	}
	len = sizeof (myctladdr);
	if (getsockname(s, (struct sockaddr *)&myctladdr, &len) < 0) {
		code = -1;
		goto bad;
	}
	cin = fdopen(s, "r");
	cout = fdopen(s, "w");
	if (cin == NULL || cout == NULL) {
		if (cin)
			(void) fclose(cin);
		if (cout)
			(void) fclose(cout);
		code = -1;
		goto bad;
	}
	if (getreply(0) > 2) { 	/* read startup message from server */
		if (cin)
			(void) fclose(cin);
		if (cout)
			(void) fclose(cout);
		code = -1;
		goto bad;
	}
#ifdef SO_OOBINLINE
	{
	int on = 1;

	if (setsockopt(s, SOL_SOCKET, SO_OOBINLINE, (char *)&on, sizeof(on)) < 0 ) {
			; // perror("ftp: setsockopt");
		}
	}
#endif /* SO_OOBINLINE */

	return (hostname);
bad:
	(void) close(s);
	return ((char *)0);
}

#ifdef WITH_TR143
static int
initconn_TR143(int isDownload, char *interface)
{
	register char *p, *a;
	int result, tmpno = 0;
	socklen_t len;
	int on = 1;
	int tos;
	u_long a1,a2,a3,a4,p1,p2;

	if (passivemode) {
		data = socket(AF_INET, SOCK_STREAM, 0);
		if (data < 0) {
			;// perror("ftp: socket");
			return(1);
		}
//#ifdef _JPR_AM_
		if (interface != NULL && strlen(interface) > 2) { 
			struct ifreq ifr;
			memset(&ifr,0,sizeof(ifr));
			snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),interface);
			
			if (setsockopt(data, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, 
								sizeof(ifr)) < 0 ) {
				LOG(LOG_DEBUG, "%s bind to device %s failed\n",__func__,interface);
			} else
			    LOG(LOG_DEBUG, "%s bind to device %s OK\n",__func__,interface);
		}
//#endif
		if (options & SO_DEBUG &&
		    setsockopt(data, SOL_SOCKET, SO_DEBUG, (char *)&on,
			       sizeof (on)) < 0)
			; //perror("ftp: setsockopt (ignored)");
		if (command("PASV") != COMPLETE) {
			// printf("Passive mode refused.\n");
			
			if(isDownload == 0){
	    #ifdef WITH_TR098
	            DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_NoTransferMode");
	    #else
	            DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_NoTransferMode");
	    #endif
			} else{
	    #ifdef WITH_TR098
	            DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_NoTransferMode");
	    #else
	            DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_NoTransferMode");
	    #endif
		    }
			return(1);
		}

		setTR143TimeInfo("TCPOpenRequestTime",isDownload);

		/*
		 * What we've got at this point is a string of comma separated
		 * one-byte unsigned integer values, separated by commas.
		 * The first four are the an IP address. The fifth is the MSB
		 * of the port number, the sixth is the LSB. From that we'll
		 * prepare a sockaddr_in.
		 */

		if (sscanf(pasv,"%ld,%ld,%ld,%ld,%ld,%ld",
			   &a1,&a2,&a3,&a4,&p1,&p2)
		    != 6) 
		{
			// printf("Passive mode address scan failure. Shouldn't happen!\n");
			return(1);
		}

		data_addr.sin_family = AF_INET;
		data_addr.sin_addr.s_addr = htonl((a1 << 24) | (a2 << 16) |
						  (a3 << 8) | a4);
		data_addr.sin_port = htons((p1 << 8) | p2);

		if (connect(data, (struct sockaddr *) &data_addr,
		    sizeof(data_addr))<0) {
			; // perror("ftp: connect");

			if(isDownload == 0){
	    #ifdef WITH_TR098
	            DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_NoResponse");
	    #else
	            DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_NoResponse");
	    #endif
			} else{
	    #ifdef WITH_TR098
	            DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_NoResponse");
	    #else
	            DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_NoResponse");
	    #endif
		    }

			return(1);
		}

	    setTR143TimeInfo("TCPOpenResponseTime",isDownload);

#ifdef IP_TOS
		tos = IPTOS_THROUGHPUT;
		if (setsockopt(data, IPPROTO_IP, IP_TOS, (char *)&tos,
		    sizeof(tos)) < 0)
			; // perror("ftp: setsockopt TOS (ignored)");
#endif
		return(0);
	}
noport:
	data_addr = myctladdr;
	if (sendport)
		data_addr.sin_port = 0;	/* let system pick one */ 
	if (data != -1)
		(void) close(data);
	data = socket(AF_INET, SOCK_STREAM, 0);
	if (data < 0) {
		; // perror("ftp: socket");
		if (tmpno)
			sendport = 1;
		return (1);
	}
//#ifdef _JPR_AM_
	if (interface!=NULL && strlen(interface) > 2) { 
		struct ifreq ifr;
		memset(&ifr,0,sizeof(ifr));
		snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),interface);
		
		if (setsockopt(data, SOL_SOCKET, SO_BINDTODEVICE, (void*)&ifr, 
							sizeof(ifr)) < 0 ) {
			LOG(LOG_DEBUG, "%s bind to device %s failed\n",__func__,interface);
		} else
		    LOG(LOG_DEBUG, "%s bind to device %s OK\n",__func__,interface);
	}
//#endif
	if (!sendport)
		if (setsockopt(data, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof (on)) < 0) {
			; // perror("ftp: setsockopt (reuse address)");
			goto bad;
		}
	if (bind(data, (struct sockaddr *)&data_addr, sizeof (data_addr)) < 0) {
		; // perror("ftp: bind");
		goto bad;
	}
	if (options & SO_DEBUG &&
	    setsockopt(data, SOL_SOCKET, SO_DEBUG, (char *)&on, sizeof (on)) < 0)
		; // perror("ftp: setsockopt (ignored)");
	len = sizeof (data_addr);
	if (getsockname(data, (struct sockaddr *)&data_addr, &len) < 0) {
		; // perror("ftp: getsockname");
		goto bad;
	}
	if (listen(data, 1) < 0)
		; // perror("ftp: listen");
	if (sendport) {
		a = (char *)&data_addr.sin_addr;
		p = (char *)&data_addr.sin_port;
#define	UC(b)	(((int)b)&0xff)
		result =
		    command("PORT %d,%d,%d,%d,%d,%d",
		      UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]),
		      UC(p[0]), UC(p[1]));
		if (result == ERROR && sendport == -1) {
			sendport = 0;
			tmpno = 1;
			goto noport;
		}
		return (result != COMPLETE);
	}
	if (tmpno)
		sendport = 1;
	return (0);
bad:
	(void) close(data), data = -1;
	if (tmpno)
		sendport = 1;
	return (1);
}

int
recvrequest_TR143(const char *cmd, char *remote, const char *lmode, long *size , char *interface)
{
	FILE *volatile din = 0;
	int (*volatile closefunc)(FILE *);
	void (*volatile oldintp)(int);
	void (*volatile oldintr)(int);
	volatile int is_retr, tcrflag;
	static unsigned bufsize;
	static char *buf;
	volatile long bytes = 0;
	register int c, d;
	struct stat st;
	char buf1[128];
    char buf2[64];
    FILE * fp;
	long rx_bytes = 0;
	
	*size = -1;
	is_retr = strcmp(cmd, "RETR") == 0;

	if (proxy && is_retr) {
		//proxtrans(cmd, local, remote);
		return 0;
	}
	closefunc = NULL;
	oldintr = NULL;
	oldintp = NULL;
	tcrflag = !crflag && is_retr;
	if (sigsetjmp(recvabort, 1)) {
		while (cpend) {
			(void) getreply(0);
		}
		if (data >= 0) {
			(void) close(data);
			data = -1;
		}
		if (oldintr)
			(void) signal(SIGINT, oldintr);
		code = -1;
		return 1;
	}
	oldintr = signal(SIGINT, abortrecv);

	if (curtype != type) {
		changetype(type, 0);
	}
	if (initconn_TR143(1,interface)) {
		(void) signal(SIGINT, oldintr);
		code = -1;
		return 1;
	}
	if (sigsetjmp(recvabort, 1))
		goto abort;
	
	if (command("SIZE %s",remote) != COMPLETE) {
	    (void) signal(SIGINT, oldintr);

	    #ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_IncorrectSize");
	    #else
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_IncorrectSize");
	    #endif
		return 1;
	}
    if(code == 213){
	    sscanf(reply_string,"%s %s", buf1, buf2);
	#ifdef WITH_TR098
	    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TestBytesReceived")),buf2);
	#else
	    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TestBytesReceived")),buf2);
	#endif
	}
	
    setTR143TimeInfo("ROMTime",1);
	
	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM) {
			(void) signal(SIGINT, oldintr);
	    #ifdef WITH_TR098
	        DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #else
	        DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #endif
			return 1;
		}
	} 
	else {
		if (command("%s", cmd) != PRELIM) {
			(void) signal(SIGINT, oldintr);	
	    #ifdef WITH_TR098
	        DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #else
	        DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #endif
			return 1;
		}
	}
	
	din = dataconn("r");
	if (din == NULL)
		goto abort;
	
	if (buf)
		(void) free(buf);
	bufsize = BUFSIZ;

	buf = malloc((unsigned)BUFSIZ);
	if (buf == NULL) {
		bufsize = 0;
		goto abort;
	}
		
	switch (curtype) {

	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		setTR143TimeInfo("BOMTime",1);
	    //record the current value of the Ethernet bytes received on the Interface
        memset(buf1,'\0',sizeof(buf1));
        memset(buf2,'\0',sizeof(buf2));
		if(interface != NULL){
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
            rx_bytes = atol(buf2);
		}
		
		while ((c = read(fileno(din), buf, bufsize)) > 0) {
			bytes += c;
		}
	
		if (c < 0) {
			if (errno != EPIPE)
				; // perror("netin");
			bytes = -1;
		    #ifdef WITH_TR098
		    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Error_NoTransferComplete");
		    #else
		    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Error_NoTransferComplete");
		    #endif
		}else{
            setTR143TimeInfo("EOMTime",1);
		    //Record the current value of the Ethernet bytes received on the Interface and calculate the TotalBytesReceived using previous value sampled at BOM Time.
	        memset(buf1,'\0',sizeof(buf1));
	        memset(buf2,'\0',sizeof(buf2));
			if(interface != NULL){
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
	        sprintf(buf1,"%ld",atol(buf2) - rx_bytes);
		    #ifdef WITH_TR098
		    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.TotalBytesReceived")),buf1);
			DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.DownloadDiagnostics.DiagnosticsState")),"Completed");
		    #else
		    DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.TotalBytesReceived")),buf1);
			DM_WriteDataToNode((TR069_GetNode("Device.DownloadDiagnostics.DiagnosticsState")),"Completed");
		    #endif
		}
		*size = bytes;
		break;
	}
	
	(void) signal(SIGINT, oldintr);
	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	(void) fclose(din);
	/* closes data as well, so discard it */
	data = -1;
	(void) getreply(0);
	if (bytes > 0 && is_retr)
		*size = bytes;
	return 0;
abort:

/* abort using RFC959 recommended IP,SYNC sequence  */

	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	(void) signal(SIGINT, SIG_IGN);
	if (!cpend) {
		code = -1;
		(void) signal(SIGINT, oldintr);
		return 1;
	}

	abort_remote(din);
	code = -1;
	if (din) {
		(void) fclose(din);
	}
	if (data >= 0) {
		/* if it just got closed with din, again won't hurt */
		(void) close(data);
		data = -1;
	}
	if (bytes > 0)
		*size = bytes;
	(void) signal(SIGINT, oldintr);
	return 1;
}

int 
sendrequest_TR143(const char *cmd, long TestFileLength, char *remote, long *size , char *interface)
{
	register int c, d;
	FILE *volatile dout = 0;
	int (*volatile closefunc)(FILE *);
	void (*volatile oldintr)(int);
	void (*volatile oldintp)(int);
	volatile long bytes = 0;
	char buf[BUFSIZ], *bufp;
	const char *volatile lmode;
	int i;
	char buf1[128];
    char buf2[64];
    FILE * fp;
	long tx_bytes = 0;
	
	*size = -1L;
	
	if (proxy) {
		//proxtrans(cmd, local, remote);
		return 0;
	}
	if (curtype != type)
		changetype(type, 0);
	closefunc = NULL;
	oldintr = NULL;
	oldintp = NULL;
	lmode = "w";
	if (sigsetjmp(sendabort, 1)) {
		while (cpend) {
			(void) getreply(0);
		}
		if (data >= 0) {
			(void) close(data);
			data = -1;
		}
		if (oldintr)
			(void) signal(SIGINT,oldintr);
		if (oldintp)
			(void) signal(SIGPIPE,oldintp);
		code = -1;
		return 1;
	}
	oldintr = signal(SIGINT, abortsend);

	if (initconn_TR143(0,interface)) {
		(void) signal(SIGINT, oldintr);
		if (oldintp)
			(void) signal(SIGPIPE, oldintp);
		code = -1;
		return 1;
	}
	if (sigsetjmp(sendabort, 1))
		goto abort;

    setTR143TimeInfo("ROMTime",0);

	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM) {
			(void) signal(SIGINT, oldintr);
			if (oldintp)
				(void) signal(SIGPIPE, oldintp);
	    #ifdef WITH_TR098
	        DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #else
	        DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #endif
			return 1;
		}
	} else if (command("%s", cmd) != PRELIM) {
			(void) signal(SIGINT, oldintr);
			if (oldintp)
				(void) signal(SIGPIPE, oldintp);
	    #ifdef WITH_TR098
	        DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #else
	        DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_NoSTOR");
	    #endif
			return 1;
	}
	
	setTR143TimeInfo("BOMTime",0);
	//record the current value of the Ethernet bytes sent on the Interface
    memset(buf1,'\0',sizeof(buf1));
    memset(buf2,'\0',sizeof(buf2));
	if(interface != NULL && strlen(interface) > 2){
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
    tx_bytes = atol(buf2);
	
	dout = dataconn(lmode);
	if (dout == NULL)
		goto abort;
	oldintp = signal(SIGPIPE, SIG_IGN);
	switch (curtype) {

	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		memset(buf,'0',sizeof(buf));
		for(i=(TestFileLength/BUFSIZ);i > 0; i--){
			if ((d = write(fileno(dout), buf, BUFSIZ)) <= 0)
				break;
            bytes += BUFSIZ;
		}
		if(d >=0 && (TestFileLength%BUFSIZ)>0 ){
		    d = write(fileno(dout), buf, (TestFileLength%BUFSIZ));
			bytes += (TestFileLength%BUFSIZ);
		}
		
		if (d < 0) {
			if (errno != EPIPE) 
				; // perror("netout");
			bytes = -1;
		    #ifdef WITH_TR098
		    DM_WriteDataToNode((TR069_GetNode("InternetGatewayDevice.UploadDiagnostics.DiagnosticsState")),"Error_NoTransferComplete");
		    #else
		    DM_WriteDataToNode((TR069_GetNode("Device.UploadDiagnostics.DiagnosticsState")),"Error_NoTransferComplete");
		    #endif
		} else {
			*size = bytes;
			setTR143TimeInfo("EOMTime",0);
			//Record the current value of the Ethernet bytes sent on the Interface and calculate the TotalBytesSent using previous value sampled at BOM Time.
		    memset(buf1,'\0',sizeof(buf1));
		    memset(buf2,'\0',sizeof(buf2));
			if(interface != NULL && strlen(interface) > 2){
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
		}
		break;
	}
	
	(void) fclose(dout);
	/* closes data as well, so discard it */
	data = -1;
	(void) getreply(0);
	(void) signal(SIGINT, oldintr);
	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	return 0;
abort:
	(void) signal(SIGINT, oldintr);
	if (oldintp)
		(void) signal(SIGPIPE, oldintp);
	if (!cpend) {
		code = -1;
		return 1;
	}
	if (dout) {
		(void) fclose(dout);
	}
	if (data >= 0) {
		/* if it just got closed with dout, again won't hurt */
		(void) close(data);
		data = -1;
	}
	(void) getreply(0);
	code = -1;
	return 1;
}

void setTR143TimeInfo(char * param, int isDownload){
	struct timeb tp;
	struct tm *timeinfo;
	char paramName[256];
	char currenttime[64];

	memset(paramName, 0, sizeof(paramName));
	memset(currenttime, 0, sizeof(currenttime));
	memset((void*)&tp, 0, sizeof(tp));
	
	ftime(&tp);
	timeinfo = localtime(&tp.time);
	sprintf(currenttime, "%d-%d-%dT%d:%d:%d.%d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,tp.millitm);
	
	if(isDownload == 0){
	#ifdef WITH_TR098
	    sprintf(paramName,"InternetGatewayDevice.UploadDiagnostics.%s",param);
	#else
	    sprintf(paramName,"Device.UploadDiagnostics.%s",param);
	#endif
	    DM_WriteDataToNode((TR069_GetNode(paramName)),currenttime);
	} else{
	#ifdef WITH_TR098
	    sprintf(paramName,"InternetGatewayDevice.DownloadDiagnostics.%s",param);
	#else
	    sprintf(paramName,"Device.DownloadDiagnostics.%s",param);
	#endif
	    DM_WriteDataToNode((TR069_GetNode(paramName)),currenttime);
	}
}
#endif

#endif /* HAVE_FTP_DOWNLOAD */
