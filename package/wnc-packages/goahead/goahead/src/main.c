/*
 * main.c -- Main program for the GoAhead WebServer (LINUX version)
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: main.c,v 1.5 2003/09/11 14:03:46 bporter Exp $
 */

/******************************** Description *********************************/

/*
 *  Main program for for the GoAhead WebServer. This is a demonstration
 *  main program to initialize and configure the web server.
 */

/********************************* Includes ***********************************/

#include    "../uemf.h"
#include    "../wsIntrn.h"
#include    <signal.h>
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#ifdef WEBS_SSL_SUPPORT
#include    "../websSSL.h"
#endif

#ifdef USER_MANAGEMENT_SUPPORT
#include    "../um.h"
void    formDefineUserMgmt(void);
#endif

#define _JNR_MDW_
#include "jnrp_cgi.h"
#define __USED_DB__

/*********************************** Locals ***********************************/
/*
 *  Change configuration here
 */

#ifdef _JNR_MDW_
static char_t       *rootWeb = T("www");            /* Root web directory */
#else
static char_t       *rootWeb = T("web");            /* Root web directory */
#endif
static char_t       *password = T("");              /* Security password */
static int          port = 80;                      /* Server port */
static int          retries = 5;                    /* Server port retries */
static int          finished;                       /* Finished flag */

/****************************** Forward Declarations **************************/

static int  initWebs(char * dbfile);
static int  aspTest(int eid, webs_t wp, int argc, char_t **argv);


//static int    getVoipValue(int eid, webs_t wp, int argc, char_t **argv);
//static void setVoipValue(webs_t wp, char_t *path, char_t *query);

static void formTest(webs_t wp, char_t *path, char_t *query);
static int  websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
                int arg, char_t *url, char_t *path, char_t *query);
extern void defaultErrorHandler(int etype, char_t *msg);
extern void defaultTraceHandler(int level, char_t *buf);
#ifdef B_STATS
static void printMemStats(int handle, char_t *fmt, ...);
static void memLeaks();
#endif
/*********************************** Code *************************************/
/*
 *  Main -- entry point from LINUX
 */

int main(int argc, char** argv)
{

/*
 *  Initialize the memory allocator. Allow use of malloc and start
 *  with a 60K heap.  For each page request approx 8KB is allocated.
 *  60KB allows for several concurrent page requests.  If more space
 *  is required, malloc will be used for the overflow.
 */
    bopen(NULL, (60 * 1024), B_USE_MALLOC);
    signal(SIGPIPE, SIG_IGN);

    //printf("webs start !!!\n");
/*
 *  Initialize the web server
 */
    if (initWebs(argv[2]) < 0) {
        printf("initWebs error.\n");
        return -1;
    }

#ifdef WEBS_SSL_SUPPORT
    websSSLOpen();
#endif

/*
 *  Basic event loop. SocketReady returns true when a socket is ready for
 *  service. SocketSelect will block until an event occurs. SocketProcess
 *  will actually do the servicing.
 */
    while (!finished) {
        if (socketReady(-1) || socketSelect(-1, 1000)) {
            socketProcess(-1);
        }
        websCgiCleanup();
        emfSchedProcess();
    }

#ifdef WEBS_SSL_SUPPORT
    websSSLClose();
#endif

#ifdef USER_MANAGEMENT_SUPPORT
    umClose();
#endif

/*
 *  Close the socket module, report memory leaks and close the memory allocator
 */


    websCloseServer();
    socketClose();
#ifdef B_STATS
    memLeaks();
#endif
    bclose();

    // close db
    //jnrpCloseDb(pWebDb);
#ifdef __USED_WEBS_DB__
    jnrpCloseDb_Webs();
#endif

    return 0;
}

/******************************************************************************/
/*
 *  Initialize the web server.
 */

static int initWebs(char * dbfile)
{
    struct hostent  *hp;
    struct in_addr  intaddr;
    char            host[128], dir[128], webdir[128];
    char            *cp;
    char_t          wbuf[128];

/*
 *  Initialize the socket subsystem
 */
    socketOpen();

#if 1
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "br0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    //printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
#endif

/*
 *  Define the local Ip address, host name, default home page and the
 *  root web directory.
 */
#if 0
    if (gethostname(host, sizeof(host)) < 0) {
        error(E_L, E_LOG, T("Can't get hostname"));
        printf("Can't get hostname\n");
        return -1;
    }
    if ((hp = gethostbyname(host)) == NULL) {
        error(E_L, E_LOG, T("Can't get host address"));
        printf("Can't get host address\n");
        return -1;
    }

    memcpy((char *) &intaddr, (char *) hp->h_addr_list[0],
      (size_t) hp->h_length);
#else

#ifdef __USED_WEBS_DB__
  // init sqlite3 db
  if (dbfile)
  {
    //printf("initWebs(), db init\n");
    jnrpInitDb_Webs(dbfile);
    pDBFile = dbfile;
  }
  else
  {
    //printf("initWebs(), use default db path\n");
    jnrpInitDb_Webs(IMPORT_DB_FILE);
    pDBFile = IMPORT_DB_FILE;
  }
#endif

#ifdef __USED_DB__

    char * addr = getIpAddress(dbfile);
    intaddr.s_addr = inet_addr(addr);
#else
    char * addr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    //printf("addr:%s \n",addr);
    intaddr.s_addr = inet_addr(addr);
#endif
    printf("addr:%s \n",addr);
#endif

#ifdef WEBS_LOGOUT_SUPPORT
    system ("echo 0 > /tmp/loginsession");
	system ("echo 1qaz2wsx%^wer > /tmp/loginsessionuserid");
	system ("echo rew^%xsw2zaq1 > /tmp/loginsessionpassword");
#endif

	//enhanced security issues
	genToken(3);

/*
 *  Set ../web as the root web. Modify this to suit your needs
 */
    getcwd(dir, sizeof(dir));
    if ((cp = strrchr(dir, '/'))) {
        *cp = '\0';
    }

#ifdef _JNR_MDW_
    sprintf(webdir, "/usr/local/%s", rootWeb);
#else
    sprintf(webdir, "%s/%s", dir, rootWeb);
#endif

    //printf("webdir:%s \n",webdir);
/*
 *  Configure the web server options before opening the web server
 */
    websSetDefaultDir(webdir);
    cp = inet_ntoa(intaddr);
    ascToUni(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
    websSetIpaddr(wbuf);
    ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
    websSetHost(wbuf);

/*
 *  Configure the web server options before opening the web server
 */
    websSetDefaultPage(T("default.asp"));
    websSetPassword(password);

/*
 *  Open the web server on the given port. If that port is taken, try
 *  the next sequential port for up to "retries" attempts.
 */
    websOpenServer(port, retries);

/*
 *  First create the URL handlers. Note: handlers are called in sorted order
 *  with the longest path handler examined first. Here we define the security
 *  handler, forms handler and the default web page handler.
 */
 /*   websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler,
        WEBS_HANDLER_FIRST);
   */     
    websUrlHandlerDefine(T("/goform"), NULL, 0, websFormHandler, 0);
    websUrlHandlerDefine(T("/cgi-bin"), NULL, 0, websCgiHandler, 0);
    websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler,
        WEBS_HANDLER_LAST);

/*
 *  Now define two test procedures. Replace these with your application
 *  relevant ASP script procedures and form functions.
 */
    websAspDefine(T("aspTest"), aspTest);
    websFormDefine(T("formTest"), formTest);

    // anita add
    jnrpWebInit(dbfile);
// end of anita add

#ifdef USER_MANAGEMENT_SUPPORT
/*
 *  Initialize the User Management database
 */

    umOpen();
    umRestore(T("umconfig.txt"));
#endif
/*
 *  Create the Form handlers for the User Management pages
 */
#ifdef USER_MANAGEMENT_SUPPORT
    formDefineUserMgmt();
#endif

/*
 *  Create a handler for the default home page
 */
    websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0);
    return 0;
}

/******************************************************************************/
/*
 *  Test Javascript binding for ASP. This will be invoked when "aspTest" is
 *  embedded in an ASP page. See web/asp.asp for usage. Set browser to
 *  "localhost/asp.asp" to test.
 */

static int aspTest(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t  *name, *address;

    if (ejArgs(argc, argv, T("%s %s"), &name, &address) < 2) {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }
    return websWrite(wp, T("Name: %s, Address %s"), name, address);
}


/******************************************************************************/
/*
 *  Test form for posted data (in-memory CGI). This will be called when the
 *  form in web/forms.asp is invoked. Set browser to "localhost/forms.asp" to test.
 */

static void formTest(webs_t wp, char_t *path, char_t *query)
{
    char_t  *name, *address;

    name = websGetVar(wp, T("name"), T("Joe Smith"));
    address = websGetVar(wp, T("address"), T("1212 Milky Way Ave."));

    websHeader(wp);
    websWrite(wp, T("<body><h2>Name: %s, Address: %s</h2>\n"), name, address);
    websFooter(wp);
    websDone(wp, 200);
}

/******************************************************************************/
/*
 *  Home page handler
 */

static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
    int arg, char_t *url, char_t *path, char_t *query)
{
/*
 *  If the empty or "/" URL is invoked, redirect default URLs to the home page
 */
    if (*url == '\0' || gstrcmp(url, T("/")) == 0) {
        websRedirect(wp, T("login.asp"));
        return 1;
    }
    return 0;
}

/******************************************************************************/
/*
 *  Default error handler.  The developer should insert code to handle
 *  error messages in the desired manner.
 */

void defaultErrorHandler(int etype, char_t *msg)
{
#if 0
    write(1, msg, gstrlen(msg));
#endif
}

/******************************************************************************/
/*
 *  Trace log. Customize this function to log trace output
 */

void defaultTraceHandler(int level, char_t *buf)
{
/*
 *  The following code would write all trace regardless of level
 *  to stdout.
 */
#if 0
    if (buf) {
        write(1, buf, gstrlen(buf));
    }
#endif
}

/******************************************************************************/
/*
 *  Returns a pointer to an allocated qualified unique temporary file name.
 *  This filename must eventually be deleted with bfree();
 */

char_t *websGetCgiCommName()
{
    char_t  *pname1, *pname2;

    pname1 = tempnam(NULL, T("cgi"));
    pname2 = bstrdup(B_L, pname1);
    free(pname1);
    return pname2;
}

/******************************************************************************/
/*
 *  Launch the CGI process and return a handle to it.
 */

int websLaunchCgiProc(char_t *cgiPath, char_t **argp, char_t **envp,
                      char_t *stdIn, char_t *stdOut)
{
    int pid, fdin, fdout, hstdin, hstdout, rc;

    fdin = fdout = hstdin = hstdout = rc = -1;
    if ((fdin = open(stdIn, O_RDWR | O_CREAT, 0666)) < 0 ||
        (fdout = open(stdOut, O_RDWR | O_CREAT, 0666)) < 0 ||
        (hstdin = dup(0)) == -1 ||
        (hstdout = dup(1)) == -1 ||
        dup2(fdin, 0) == -1 ||
        dup2(fdout, 1) == -1) {
        goto DONE;
    }

    rc = pid = fork();
    if (pid == 0) {
/*
 *      if pid == 0, then we are in the child process
 */
        if (execve(cgiPath, argp, envp) == -1) {
            printf("content-type: text/html\n\n"
                "Execution of cgi process failed\n");
        }
        exit (0);
    }

DONE:
    if (hstdout >= 0) {
        dup2(hstdout, 1);
      close(hstdout);
    }
    if (hstdin >= 0) {
        dup2(hstdin, 0);
      close(hstdin);
    }
    if (fdout >= 0) {
        close(fdout);
    }
    if (fdin >= 0) {
        close(fdin);
    }
    return rc;
}

/******************************************************************************/
/*
 *  Check the CGI process.  Return 0 if it does not exist; non 0 if it does.
 */

int websCheckCgiProc(int handle, int *status)
{
/*
 *	Check to see if the CGI child process has terminated or not yet.  
 */
	if (waitpid(handle, status, WNOHANG) == handle) {
		return 0;
	} else {
		return 1;
	}
}

/******************************************************************************/

#ifdef B_STATS
static void memLeaks()
{
    int     fd;

    if ((fd = gopen(T("leak.txt"), O_CREAT | O_TRUNC | O_WRONLY, 0666)) >= 0) {
        bstats(fd, printMemStats);
        close(fd);
    }
}

/******************************************************************************/
/*
 *  Print memory usage / leaks
 */

static void printMemStats(int handle, char_t *fmt, ...)
{
    va_list     args;
    char_t      buf[256];

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    write(handle, buf, strlen(buf));
}
#endif

/******************************************************************************/
