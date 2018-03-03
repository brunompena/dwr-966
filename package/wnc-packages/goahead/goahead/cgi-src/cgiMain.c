#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/stat.h>

/************************************************** *************************
* For file upload, CGI variables look like this:
* CONTENT_TYPE=multipart/form-data;
boundary=---------------------------14422580032340
CONTENT_LENGTH=216
STDIN contains the following information:

-----------------------------14422580032340
Content-Disposition: form-data;
name="file";
filename="/home/user/myup.jpg"
Content-Type: text/plain
Data from file goes here-----------------------------14422580032340

*************************************************************************/

//want to check file extensions? 1=yes 0=no
#define CHECKEXT 0

#define MAX_ERRNO	4095
#define IS_ERR_PTR(x) (((void*)x) >= (void*)-MAX_ERRNO)

#define DEBUG_MODE	0
#define DEBUG_FILE	"cgi_debug_message.txt"
#define UPLOADIR "/data/upload"
// buffer for storing boundary line and file name
#define MAXLINE 512
#define BUFFSIZE 16*1024

#define MAXFILE_SIZE 62914560

#define FW_UPGRADE_STATUS_FILE "/data/fw_upgrade/upgrade_status/upgrade_status"
#define FW_UPGRADE_STATUS_MSG_FILE "/data/fw_upgrade/upgrade_status/status_msg"

//define max size of uploads to 60MB
#define MAXSIZE 62914560

#define MAX_NAME 50
//#define W_RUN_IN_PC 1
#ifdef W_RUN_IN_PC

#define SOCKET_FOR_CGI "cgi-2-sys"  // must the same as sys manager's setting

#else

#define SOCKET_FOR_CGI "/tmp/cgi-2-sys"  // must the same as sys manager's setting

#endif // W_RUN_IN_PC

#define SESSION_COOKIE_NAME "CGISID"

static int hextable[256];

// general purpose linked list. Actualy isn't very portable
// because uses only 'name' and 'value' variables to store data.
// Problably, in a future release, this will be replaced by
// another type of struct
typedef struct cookievarsA {
        char *name;
        char *value;
        struct cookievars *next;
} cookievars;

static cookievars *cookies_start;
static cookievars *cookies_end;

typedef enum {
	ACTION_PROFILE_IMPORT = 0,
	ACTION_CONTACT_IMPORT,
	ACTION_FW_UPGRADE,
	ACTION_MAX
} do_action_type;

char *extractParam(char *str,char *Name,char *Val);

// multi-language supports
//	cgiMain asks SysManager for the translated string
#define _(str) cgi_gettext(str)


int handle_query_ex(char const * const jin, char* resp, int resp_len);

/*
cgi_gettext: get the translated string from sysmanager via "get_translated_str" command
	[in] msgid: the string to be translated
	return: the translated msgid
	
 {"translated_str":{"errmsg":"","errno":0,"msgstr":"...."}}

*/
const char* cgi_gettext(const char* msgid)
{
	static char jout[300]; // todo: usually enough
	
	int ret = 0;
	char jin[300]; // todo: usually enough	 
	char *p;
	const char jout_key[] = "\"msgstr\":\"";
	const char jout_end[] = "\"}}";	
	
	sprintf(jin, "{ \"action\": \"get_translated_str\", \"args\": { \"msgid\": \"%s\"}}", msgid);
	memset(jout, 0, sizeof(jout));
	handle_query_ex(jin, jout, sizeof(jout));

	p = strstr(jout, jout_end);
	if(p != 0) { // found
		*p = 0; // end of the msgstr
		p = strstr(jout, jout_key);
		if(p != 0) { // should found
			p = p + strlen(jout_key);	
		}
	}

	if(p == 0)  // if no found, use ori string
		p = msgid;
	
	return p;
}


/* Handling Special Characters */
/******************************************************************************/
/*!
**  \brief decodes a string from HTML format (%hex)
**
**  This function will translate HTML special characters from the HTML form
**  of %xx to the equivalent 8 bit character.  Used to process input from
**  GET/POST transactions.
**
**  \param src Pointer to source string
**  \param dest Pointer to destination string
**  \return N/A
*/
void unencode(char *src, char *dest)
{
    for(; *src != '\0'; src++)
    {
        if(*src == '+') 
        {
            *dest = ' ';
            dest++;
        }
        //decoding special symbols
        else if(*src == '%') 
        {
            int code;
            if(sscanf(src+1, "%2x", &code) != 1) code = '?';
            // ignoring all newline symbols that come from form - we put our own instead of "&varname1=" --> '\n'
            if(code != 10 && code != 12 && code != 13)
            {
                *dest = code;
                dest++;
            }
            src +=2;
        }
        else
        {
            *dest = *src;
            dest++;
        }
    } //outer for loop

    *dest = 0;
}

/*****************************************************************************
**
** extractVal
**
** This function returns both the value name and value string for the next item in
** the list.  It returns a pointer to the next value, or NULL if the string has
** "run out".
**
**  PARAMETERS:
**
**  RETURNS:
**
*/
char *extractParam(char *str,char *Name,char *Val)
{
    int     param=0;
    int     val = 0;

    /*
    ** Code Begins
    ** Search for the ? or & to start the string
    */

    while(*str)
    {
        /*
        ** Check for the beginning ? or &.  This signifies the start or
        ** end of the parameter. start is null at the start
        */

        if(*str == '?' || *str=='&' || *str == 0x0a || *str == 0x0d )
        {
            if(!param)
            {
                param = 1;
            }
            else
            {
                /*
                ** All Done.  Return this pointer
                */

                *Val = 0;
                return (str);
            }
        }
        else if(*str == '=')
        {
            val = 1;
            *Name = 0;  // Null terminate
        }
        else if(!val)
        {
            param = 1;
            *Name++ = *str;
        }
        else
            *Val++ = *str;

        str++;
    }

    /*
    ** If we get here, we have run out before getting a complete
    ** parameter. End of the line
    */

    return (NULL);
}

void print_http_header()
{
	printf("Content-type: text/html\n\n");
}

/* Handle Socket --start */
static int connectToServer(const char* fileName)
{
	int sock = -1;
	int cc;

	struct sockaddr_un addr;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("UNIX domain socket create failed (errno=%d)\n", errno);
		return -1;
	}

	/* connect to socket; fails if file doesn't exist */
	strcpy(addr.sun_path, fileName);    // max 108 bytes
	addr.sun_family = AF_UNIX;
	cc = connect(sock, (struct sockaddr*) &addr, SUN_LEN(&addr));
	if (cc < 0) {
		// ENOENT means socket file doesn't exist
		// ECONNREFUSED means socket exists but nobody is listening
		//LOGW("AF_UNIX connect failed for '%s': %s\n",
		//    fileName, strerror(errno));

		//printf("Connect fail\n");
		close(sock);
		return -1;
	}

	return sock;
}

static int fd_cgi_socket = 0;

// init socket
static void init()
{
	int fd, ret;

	const char* file = SOCKET_FOR_CGI;
	fd_cgi_socket = connectToServer(file);
	#if 0
	if(-1 == fd_cgi_socket)
		printf("Error:Can't connect to socket:'%s'\n", file);
	else
		printf("OK to connect to socket:%s\n", file);
	#endif
}

// clean socket
static void clean()
{
	if(fd_cgi_socket > 0)
		close(fd_cgi_socket);
}
/* Handle Socket --end */

int handle_query(char const * const jin)
{
	int ret = 0;
	init(); //init socket

	ret = write(fd_cgi_socket, jin, strlen(jin) + 1);
	if (ret < 0) {
		//printf("socket write fail\n");
		goto error;
	}	

	// a loop to read all response
	while(ret > 0) {
		char resp[1024]; // response from socket
		memset(resp, 0, sizeof(resp));
		ret = read(fd_cgi_socket, &resp, sizeof(resp) - 1);
		if (ret < 0) {
			//printf("socket read fail\n");
			goto error;
		}else {
			//printf("<---:%s\n", resp);
			printf("%s", resp);
		}
	}
	printf("\n");

error:
	clean();
	return ret;
}

/*
handle_query_ex: send json string to socket and save the return value to resp
	[in] jin: json string
	[out] resp: to store the response from socket, should be json string too
	[in]  resp_len: max length of resp
*/
int handle_query_ex(char const * const jin, char* resp, int resp_len)
{
	int ret = 0;
	int count;
	init(); //init socket

	ret = write(fd_cgi_socket, jin, strlen(jin) + 1);
	if (ret < 0) {
		//printf("socket write fail\n");
		goto handle_query_ex_error;
	}	

	count = 0;
	// a loop to read all response
	while(ret > 0) {
		ret = read(fd_cgi_socket, resp + count , resp_len - count - 1);
		if (ret < 0) {
			// printf("socket read fail\n");
			goto handle_query_ex_error;
		} else {
			count += ret;
		}
	}

handle_query_ex_error:
	clean();
	return ret;
}

static int chk_is_substring(char *s1, char *s2)
{
	int str_len=0, k=0, i = 0, j = 0;

	//assign length of be to blen
	str_len =strlen(s1);

	while (s2[i] != '\0') {
		if (tolower(s2[i]) == tolower(s1[j])) {
			j++;
			if (j == str_len)
				return 1;
		} else {
			j=0;
			if (tolower(s2[i]) == tolower(s1[j]))
				j++;
		}
		i++;
	}
	return 0;
}

int upload_file(long size,char *file_name, do_action_type *do_action, char* err_str)
{
	int rc = 0;
	char *up= UPLOADIR;
	char *psz1; char *psz2;
	FILE *out = NULL;
	long i, total, count;
	char *ContentLength;
	char szBoundary[MAXLINE];
	char szFile[MAXLINE];
	char szBuff[BUFFSIZE];
	char p[512];
	//filename to write to
	char myFile[MAXLINE];
	struct stat sb;

	char action_type[MAX_NAME];
	/* The supposed number of incoming bytes. */
	char *filename;
	if (size > MAXSIZE) {
		rc=7;
		sprintf(p,"echo \"%ld\" > /dev/ttyHSL0",size);
		system(p);
		goto Error;
	}

	// null out file name buffer
	memset(szFile, 0, sizeof(szFile));

	// first line should be MIME boundary, prepend cr/lf
	szBoundary[0] = '\r';
	szBoundary[1] = '\n';
	szBoundary[2] = '\0';

	if (fgets(&szBoundary[2], sizeof(szBoundary)-2, stdin) == NULL)
	{
	rc = 1;
	goto Error;
	}

	//strip terminating CR / LF
	if ((psz1=strchr(&szBoundary[2],'\r')) != NULL)
	{
	*psz1 = '\0';
	}

	if ((psz1=strchr(&szBoundary[2],'\n')) != NULL)
	{
	*psz1 = '\0';
	}

	// second line should contain "Content-Disposition:
	if (fgets(szBuff, sizeof(szBuff), stdin) == NULL)
	{
		rc = 2;
		goto Error;
	}

	sprintf(p,"echo \"%s\" > /dev/ttyHSL0",szBuff);
	system(p);

	if ((psz1=strstr(szBuff, "name=")) == NULL) {
		rc = 12;
		goto Error;
	}
	// get pointer to actual name (it's in quotes)
	psz1+=strlen("name=");

	//check action type
	strncpy(action_type,psz1,MAX_NAME);

	sprintf(p,"echo \"action_type(%s)\" > /dev/ttyHSL0",action_type);
	system(p);

	if (chk_is_substring("system_profile_file", action_type)) {
		*do_action = ACTION_PROFILE_IMPORT;
	}else if (chk_is_substring("contact_import_file", action_type)) {
		*do_action = ACTION_CONTACT_IMPORT;
	}else if (chk_is_substring("system_fw_upgrade", action_type)){
		*do_action = ACTION_FW_UPGRADE;
		system("rm -rf /data/fw_upgrade");
		system("mkdir -p /data/fw_upgrade/upgrade_status");
		system("echo \"1\" > "FW_UPGRADE_STATUS_FILE);
		system("echo \"Uploading FW\" > "FW_UPGRADE_STATUS_MSG_FILE);
	}else
		*do_action = -1;

	// get filename keyword
	if ((psz1=strstr(szBuff, "filename=")) == NULL) {
		rc = 3;
		goto Error;
	}

	// get pointer to actual filename (it's in quotes)
	psz1+=strlen("filename=");

	if ((psz1 = strtok(psz1, " \"")) == NULL) {
		rc = 4;
		goto Error;
	}

	// remove leading path for both PC and UNIX systems
	if ((psz2 = strrchr(psz1,'\\')) != NULL) {
		psz1 = psz2+1;
	}
	if ((psz2 = strrchr(psz1,'/')) != NULL) {
		psz1 = psz2+1;
	}

	//psz1 now points to a file name, try to create it in our system
	sprintf(szFile, "%s/%s",up,psz1);

	sprintf(p,"echo \"szFile(%s)\" > /dev/ttyHSL0",szFile);
	system(p);

	//file to actually write
	sprintf(myFile, "%s", psz1);

	sprintf(p,"echo \"myFile(%s)\" > /dev/ttyHSL0",myFile);
	system(p);

	//basename of file
	filename= myFile;

	//check for valid extension
	if (CHECKEXT) {
		if ((getext(filename))== 0) {
			rc = 8;
			goto Error;
		}
	}

	// determine if file exists, and don't allow overwritting
//	if ((out = fopen(szFile, "rb")) != NULL) {
		// file already exists!
//		rc = 5;
//		goto Error;
//	}

	if (stat(UPLOADIR, &sb) != 0) {
		/* Directory does not exist */
		if (mkdir(UPLOADIR, 0777) != 0) {
			rc = 11;
			goto Error;
		}
	}

	if ((out = fopen(szFile, "wb+")) == NULL)
	{
		rc = 6;
		goto Error;
	}

	// throw away until we get a blank line
	while (fgets(szBuff, sizeof(szBuff), stdin) != NULL) {
		if (strlen(szBuff) <= 2) {
			break;
		}
	}

	// copy the file
	while ((count=fread(szBuff, 1, sizeof(szBuff), stdin)) != 0) {
		if ((i=fwrite(szBuff, 1, count, out)) != count) {
			rc = 7;
			sprintf(p,"echo \"count = %ld, i= %ld\" > /dev/ttyHSL0",count,i);
			system(p);
			goto Error;
		}
		//disk write error
	}

	// re read last MAXFILE_SIZE bytes of file, handling files < MAXFILE_SIZE bytes
	if ((count = ftell(out)) == -1) {
		rc = 8;
		goto Error;
	}

	if (count > MAXFILE_SIZE) {
		count = MAXFILE_SIZE;
	}

	if (fseek(out, 0-count, SEEK_END) != 0) {
		rc = 9;
		goto Error;
	}

	// get the new position
	if ((total = ftell(out)) == -1) {
		rc = 10;
		goto Error;
	}

	// and read the data
	count = fread(szBuff, 1, sizeof(szBuff), out);
	szBuff[count] = '\0';

	// determine offset of terminating boundary line
	rc = 11;
	for (i=0; i<=(count-(long)strlen(szBoundary)); i++) {
		if ((szBuff[i] == szBoundary[0]) 
			&& (strncmp(szBoundary, &szBuff[i], strlen(szBoundary)) == 0)) {
			total+=i;
			rc = 0;
			break;
		}
	}

	// if rc is still set, we didn't find the terminating boundary line
	if (rc != 0) {
		goto Error;
	}

	if (total == 0) {
		rc = 11;
		goto Error;
	}

	// truncate the file at the correct length by writing 0 bytes
	fflush(out);

	Error:
	if (out != NULL) {
		fclose(out);
	}

	switch (rc) {
		case 0: // success
//			printf("The file <b> %s %d bytes</b> was uploaded sucessfully.",myFile,size);
			sprintf(p,"echo \"The file <b> %s %d bytes</b> was uploaded sucessfully\" > /dev/ttyHSL0",myFile,size);
			sprintf(err_str, _("The file %s %d bytes was uploaded successfully"),myFile);
			break;

		case 5: // file exists
//			printf ("The file %s already exists and cannot be overwritten.Please try again with a different file.\n", myFile);
			sprintf(p,"echo \"The file %s already exists and cannot be overwritten.Please try again with a different file.\" > /dev/ttyHSL0",myFile);
			sprintf(err_str, _("The file %s already exists and cannot be overwritten.Please try again with a different file."),myFile);
			break;

		case 7: // file too big
//			printf ("The file <b> %s </b> is too big. Try again.",myFile);
			sprintf(p,"echo \"The file <b> %s </b> is too big. Try again.\" > /dev/ttyHSL0",myFile);
			sprintf(err_str, _("The file %s is too big. Please try again."),myFile);
			break;

		case 8: // file is not an allowed type
//			printf ("The file <b> %s </b> is not an allowed type. Try again.",myFile);
			sprintf(p,"echo \"The file <b> %s </b> is not an allowed type. Try again.\" > /dev/ttyHSL0",myFile);
			sprintf(err_str, _("The file %s is not an allowed type. Please try again."),myFile);
			break;

		case 11: // 0 byte file
//			printf("The file <b>%s </b>contains no data.<br>Please try again with a different file.", myFile);
			sprintf(p,"echo \"The file <b>%s </b>contains no data.<br>Please try again with a different file.\" > /dev/ttyHSL0",myFile);
			sprintf(err_str, _("The file %s contains no data. Please try again with a different file."),myFile);
			unlink(szFile);
			break;

		default: // all other cases
//			printf("Error %d uploading file<b>%s </b>Please try again.", rc, myFile);
			sprintf(p,"echo \"Error %d uploading file<b>%s </b>Please try again.\" > /dev/ttyHSL0",rc,myFile);
			sprintf(err_str, _("Error %d uploading file %s. Please try again."),rc,myFile);
			unlink(szFile);
			break;
	}
	if (rc) {
		if (*do_action == ACTION_FW_UPGRADE) {
			system("rm -rf /data/fw_upgrade");
		}

		system(p);
//		if ((psz1=getenv("HTTP_REFERER")) != NULL) {
//			printf("<p><A HREF =\"%s\">Back</A>", psz1);
//		}
//		printf("</html>\n");

	}else
		strcpy(file_name,szFile);

	return rc;
}

static int cgiStrEqNc(char *s1, char *s2) {
	while(1) {
		if (!(*s1)) {
			if (!(*s2)) {
				return 1;
			} else {
				return 0;
			}
		} else if (!(*s2)) {
			return 0;
		}
		if (isalpha(*s1)) {
			if (tolower(*s1) != tolower(*s2)) {
				return 0;
			}
		} else if ((*s1) != (*s2)) {
			return 0;
		}
		s1++;
		s2++;
	}
}

// Original idea from cgic library
void init_hex_table()
{
	memset(hextable, 0, 255);

	hextable['1'] = 1;
	hextable['2'] = 2;
	hextable['3'] = 3;
	hextable['4'] = 4;
	hextable['5'] = 5;
	hextable['6'] = 6;
	hextable['7'] = 7;
	hextable['8'] = 8;
	hextable['9'] = 9;
	hextable['a'] = 10;
	hextable['b'] = 11;
	hextable['c'] = 12;
	hextable['d'] = 13;
	hextable['e'] = 13;
	hextable['f'] = 15;
	hextable['A'] = 10;
	hextable['B'] = 11;
	hextable['C'] = 12;
	hextable['D'] = 13;
	hextable['E'] = 14;
	hextable['F'] = 15;
}

/**
* Transforms' URL special chars.
* Search for special chars ( like %%E1 ) in str, converting them to the ascii character correspondent.
* @param str String containing data to parse
* @return The new string
* @see cgi_escape_special_chars
**/
char *cgi_unescape_special_chars(char *str)
{
	char *tmp;
	register int i, len, pos = 0;

	len = strlen(str);
	tmp = (char *)malloc(len + 1);
	if (tmp == NULL) {
		system("echo \"No memory in cgi_unescape_special_chars\" > /dev/ttyHSL0");
		return NULL;
	}

	for (i = 0; i < len; i++) {
		// If we found a '%' character, then the next two are the character
		// hexa code. Converting a hexadecimal code to their decimal is easy:
		// The first character needs to be multiplied by 16 ( << 4 ), and the another
		// one we just get the value from hextable variable
		if ((str[i] == '%') && isalnum(str[i+1]) && isalnum(str[i+2])) {
			tmp[pos] = (hextable[(unsigned char) str[i+1]] << 4) + hextable[(unsigned char) str[i+2]];
			i += 2;
		}
		else if (str[i] == '+')
			tmp[pos] = ' ';
		else
			tmp[pos] = str[i];

		pos++;
	}

	tmp[pos] = '\0';

	return tmp;
}

// Add a new item to the list
void slist_add(cookievars *item, cookievars **start, cookievars **last)
{
	// if *start is empty, then our list is also empty. So, the only
	// task to do is fill the variables *start and *last with the data pointed by item
	// *start contains the firts item in the list, and we need *last to know the 
	// end of the list
	if (!*start) {
		// item->next needs to be NULL, otherwise we never will
		// find the end of list
		item->next = NULL;

		*start = item;
		*last = item;

		return;
	}

	// Ok, in this case, the next item will contain the
	// new data. Note that, before it, (*last)->next contains
	// NULL ( the end of the list )
	(*last)->next = item;

	// The 'next' argument of item variable need to contain
	// NULL, or in other words, it will be the end ;0
	item->next = NULL;

	// Finaly, the actual end of list will contain the new item
	*last = item;
}

// Free linked list allocated memory
void slist_free(cookievars **start)
{
	while (*start) {
		void *p = *start;
		free((*start)->name);
		free((*start)->value);

		*start = (*start)->next;

		free(p);
	}

	*start = NULL;
}

cookievars *cgi_get_cookies()
{
	register cookievars *data;
	register size_t position;
	char *cookies, *aux;
	char err_str[512];

	if ((cookies = getenv("HTTP_COOKIE")) == NULL){
		system("echo \"There is no cookies\" > /dev/ttyHSL0");
		return NULL;
	}

#if 0
	memset(err_str,0,sizeof(err_str));
	sprintf(err_str,"echo \"%s\" > /dev/ttyHSL0",cookies);
	system(err_str);
#endif

	cookies = cgi_unescape_special_chars(cookies);
	aux = cookies;

	while (cookies) {
		position = 0;
		data = (cookievars *)malloc(sizeof(cookievars));
		if (!data) {
			system("echo \"No memory when allocate memory in cgi_get_cookies\" > /dev/ttyHSL0");
			exit(EXIT_FAILURE);
		}

		while (*aux++ != '=')
			position++;

		data->name = (char *)malloc(position+1);
		if (!data->name) {
			system("echo \"No memory when allocate memory in cgi_get_cookies\" > /dev/ttyHSL0");

			exit(EXIT_FAILURE);
		}

		strncpy(data->name, cookies, position);
		data->name[position] = '\0';

		position = 0;
		cookies = aux;
		if ((strchr(cookies, ';')) == NULL) {
			aux = NULL;
			position = strlen(cookies);
		}
		else {
			while (*aux++ != ';')
				position++;
			// Eliminate the blank space after ';'
			aux++;
		}

		data->value = (char *)malloc(position + 1);
		if (!data->value) {
			exit(-1);
		}

		strncpy(data->value, cookies, position);
		data->value[position] = '\0';

		slist_add(data, &cookies_start, &cookies_end);
		cookies = aux;
	}

	return cookies_start;
}

int cgi_init()
{
	init_hex_table();

	cgi_get_cookies();

	return 1;
}

// Returns the value of the item pointed by name
char *slist_item(const char *name, cookievars *start)
{
	cookievars *begin;
	begin = start;

	while (begin) {
		if (!strcasecmp(begin->name, name))
			return (!begin->value[0] ? NULL : begin->value);

		begin = begin->next;
	}

	return NULL;
}

/**
* Gets cookie value.
* Like cgi_param(), cgi_cookie_value() returns the data contained in cookie_name cookie
* @param cookie_name Cookie name to get the value
* @return The cookie value
**/
char *cgi_cookie_value(const char *cookie_name)
{
	return slist_item(cookie_name, cookies_start);
}

char * getSplitStr(char * pSources, char * pSep, int nIdx)
{
    char * pRet = (char *)malloc(24);
    char * temp1;
    char *temp2 = (char *)malloc(128);
    strcpy(temp2,pSources);
    temp1 = strtok (temp2, pSep);
    int count = 1;
    while (temp1 != NULL)
    {
        if (count == nIdx)
            strcpy(pRet,temp1);

        temp1 = strtok (NULL, pSep);
        count++;
    }

    return pRet;
}

int main(int argc,char** argv)
{
	char *lenStr;
	char *data;
	char *req_method;
	char *cont_type;
	char* valBuff = 0;
	long len;
	int ERR;
/* Pointer to CONTENT_LENGTH environment variable. */
	long InCount;
	do_action_type do_action;
	// char redirect_page[512];
#if DEBUG_MODE
	FILE *ptr;
#endif
#if 1
	char err_str[512];
#endif
	char file_name[256];
	const char * redirect_page_format =
		"<html>\n"
		"<head>\n"
		"<meta http-equiv=\"REFRESH\"\n"
		"content=\"0;url=%s\">\n"
		"</head>\n"
		"</html>\n";
	char* ptr_action = NULL;
	char* ptr_cgisid = NULL;
	char* sid = NULL;
	char* remote_host = NULL;
	char* remote_addr = NULL;
	char* remote_user = NULL;

	cgi_init();

	print_http_header();

	req_method = getenv("REQUEST_METHOD");
	lenStr = getenv("CONTENT_LENGTH");
	cont_type = getenv("CONTENT_TYPE");
	remote_host = getenv("REMOTE_HOST");
	remote_addr = getenv("REMOTE_ADDR");
	remote_user = getenv("REMOTE_USER");

#if 0
	memset(err_str,0,sizeof(err_str));
	sprintf(err_str,"echo \"length(%s)\" > /dev/ttyHSL0",lenStr);
	system(err_str);
	memset(err_str,0,sizeof(err_str));
	sprintf(err_str,
		"echo \"cgiMain...REQUEST_METHOD(%s), CONTENT_LENGTH(%s), CONTENT_TYPE(%s), "
		"REMOTE_HOST(%s), REMOTE_ADDR(%s), REMOTE_USER(%s).\" > /dev/ttyHSL0",
		req_method,lenStr,cont_type,remote_host,remote_addr,remote_user);
	system(err_str);
#endif

	InCount = atol(lenStr);

	if (chk_is_substring("get",req_method)) {
		char Name[32];
		char Data[512];
		*(char*)0 = 0; // unexpetecd run path, force crashing

		sscanf(lenStr,"%ld",&len);
		len;
		valBuff = malloc(len);
		memset(valBuff,'\0', len);
		memset(Data,'\0',sizeof(Data));
		data = getenv("QUERY_STRING");
		while(data)
		{						
			data = extractParam(data,Name,valBuff);
			unencode(valBuff,Data);
		}

		//URL?parameter
		//Handler parameter, please use variable Data.
		//printf("{\"result\" : {\"get\" : \"ok\"}}");
		
		return 0;
	} else if (chk_is_substring("post",req_method)) {
		if (chk_is_substring("application/x-www-form-urlencoded",cont_type)) {
			system("echo \"CONTENT_TYPE is application/x-www-form-urlencoded\" > /dev/ttyHSL0");
		} else if (chk_is_substring("multipart/form-data",cont_type)) {
			char err_str[128];
			system("echo \"CONTENT_TYPE is multipart/form-data\" > /dev/ttyHSL0");
			valBuff = malloc(512);
			memset(valBuff,'\0', 512);
			ERR = upload_file(InCount,file_name,&do_action,err_str);
			if (!ERR) {
				memset(valBuff,'\0', 512);
				switch (do_action) {
					case ACTION_PROFILE_IMPORT:
//						sprintf(redirect_page,"http://%s",getenv("HTTP_HOST"));
						sprintf(valBuff,"{ \"action\": \"set_system_profile_import\", \"args\": { \"file_name\": \"%s\"}}",file_name);
						ERR = handle_query(valBuff);
						break;
					case ACTION_CONTACT_IMPORT:
//						sprintf(redirect_page,"http://%s",getenv("HTTP_HOST"));
						sprintf(valBuff,"{ \"action\": \"set_contacts_device_import\", \"args\": { \"file_name\": \"%s\"}}",file_name);
						ERR = handle_query(valBuff);
						break;
					case ACTION_FW_UPGRADE:
						sprintf(valBuff,"{ \"action\": \"set_system_fw_upgrade\", \"args\": { \"file_name\": \"%s\"}}",file_name);
						ERR = handle_query(valBuff);
						break;
					default:
						*(char*)0 = 0; // unexpetecd run path, force crashing
						break;
				}
//				printf (redirect_page_format, redirect_page);
			}else {
				switch (do_action) {
					case ACTION_PROFILE_IMPORT:
//						sprintf(redirect_page,"http://%s",getenv("HTTP_HOST"));
						printf(" { \"system_profile_import\": { \"errno\": -1, \"errmsg\": \"%s\"}}",err_str);
						break;
					case ACTION_CONTACT_IMPORT:
//						sprintf(redirect_page,"http://%s",getenv("HTTP_HOST"));
						printf(" { \"contacts_device_import\": { \"errno\": -1, \"errmsg\": \"%s\"}}",err_str);
						break;
					case ACTION_FW_UPGRADE:
						printf(" { \"system_fw_upgrade\": { \"errno\": -1, \"errmsg\": \"%s\"}}",err_str);
						break;
					default:
						*(char*)0 = 0; // unexpetecd run path, force crashing
						break;
				}
			}
		} else {
			sscanf(lenStr,"%ld",&len);
			valBuff = malloc(len + 200);
			memset(valBuff,'\0', len + 200);
			fgets(valBuff, len+1, stdin);

			// Get the session ID
			sid = cgi_cookie_value(SESSION_COOKIE_NAME);

			// pass IP information to sysmgr.
			sprintf(valBuff + len - 1, ",\"HTTP_HOST\": \"%s\",\"CGISID\": \"%s\",\"REMOTE_ADDR\":\"%s\"}", getenv("HTTP_HOST"),sid ? sid : "NULL",getenv("REMOTE_ADDR"));
			//unencode(valBuff, Data);

			/************************************************************
			 * Support get_ping_info command
			 ***********************************************************/			
			if (strstr(valBuff,"get_ping_info")!=NULL)
			{
				char* start_symbol = 0;
				char* end_symbol = 0;
				int start_pos;
				int end_pos;
				char hostName[MAX_NAME];
				char ipv6_type[MAX_NAME];
				char ttl[MAX_NAME];
				char reply_time[MAX_NAME];
				memset(hostName, 0x00, sizeof(hostName));
				memset(ipv6_type, 0x00, sizeof(ipv6_type));
				memset(ttl, 0x00, sizeof(ttl));
				memset(reply_time, 0x00, sizeof(reply_time));
			
                start_symbol = strstr(valBuff, "hostname");
                end_symbol = strstr(start_symbol, "\",");
                start_pos = start_symbol ? start_symbol-valBuff+11:-1;
                end_pos = end_symbol ? end_symbol-valBuff:-1;
				//printf("start_symbol=%s, end_symbol=%s, start_pos=%d, end_pos=%d\n",start_symbol,end_symbol,start_pos,end_pos);				
				if (start_pos!=-1 && end_pos!=-1 && end_pos > start_pos)
				{
					strncpy(hostName,valBuff+start_pos,end_pos-start_pos);
					hostName[end_pos-start_pos+1]=0x00;
					//printf("hostName=%s\n",hostName);
				}
				
                start_symbol = strstr(valBuff, "ipv6_type");
                end_symbol = strstr(start_symbol, "\"}");
                start_pos = start_symbol ? start_symbol-valBuff+12:-1;
                end_pos = end_symbol ? end_symbol-valBuff:-1;
				//printf("start_symbol=%s, end_symbol=%s, start_pos=%d, end_pos=%d\n",start_symbol,end_symbol,start_pos,end_pos);
				
				if (start_pos!=-1 && end_pos!=-1 && end_pos > start_pos)
				{
					strncpy(ipv6_type,valBuff+start_pos,end_pos-start_pos);
					ipv6_type[end_pos-start_pos+1]=0x00;
					//printf("ipv6_type=%s\n",ipv6_type);
				}

				char cmdbuffer[100] = {0};	
				memset(cmdbuffer, '\0', sizeof(cmdbuffer));
				char line[128];
				memset(line, '\0', sizeof(line));		
				FILE *fd = -1;				
				if (!strcmp(ipv6_type,"1"))
					sprintf(cmdbuffer,"ping -6 -c 1 %s | grep 'bytes from' | cut -d' ' -f6-7", hostName);
				else
					sprintf(cmdbuffer,"ping -4 -c 1 %s | grep 'bytes from' | cut -d' ' -f6-7", hostName);
				
				//printf("cmd=%s\n", cmdbuffer);
				fd = popen(cmdbuffer,"r");	
				if(fgets(line,sizeof(line),fd) == NULL)
				{
				}
				else
				{
					if (line[strlen(line)-1] == '\n')
					 line[strlen(line)-1] = '\0';					
				}	
				pclose(fd); 
				//printf("line = %s\n", line);

				strcpy(ttl,getSplitStr(getSplitStr(line," ",1), "=", 2));
				strcpy(reply_time,getSplitStr(getSplitStr(line," ",2), "=", 2));
				printf("{\"ping_info\" : {\"ttl\" : \"%s\", \"reply_time\" : \"%s\", \"errmsg\" : \"\", \"errno\":0}}",ttl,reply_time);		
				ERR = 0;				
			}		
			else			
				ERR = handle_query(valBuff);
		}
	}

#if DEBUG_MODE
	ptr = fopen(DEBUG_FILE, "a");
	if(ptr != NULL) {
		fputs("getenv:CONTENT_LENGTH lenStr = ", ptr);
		fputs(lenStr, ptr);
		fputs("\n", ptr);
		fputs("getenv:QUERY_STRING valBuff= ", ptr);
		fputs(valBuff, ptr);
		fputs("\n", ptr);
		fclose(ptr);
	}
#endif
	if(valBuff)
		free(valBuff);

	return 0;
}
