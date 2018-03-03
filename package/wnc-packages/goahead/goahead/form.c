/*
 * form.c -- Form processing (in-memory CGI) for the GoAhead Web server
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: form.c,v 1.5 2010-02-03 11:00:16 chhung Exp $
 */

/********************************** Description *******************************/

/*
 *	This module implements the /goform handler. It emulates CGI processing
 *	but performs this in-process and not as an external process. This enables
 *	a very high performance implementation with easy parsing and decoding 
 *	of query strings and posted data.
 */

/*********************************** Includes *********************************/

#include	"wsIntrn.h"

/************************************ Locals **********************************/

static sym_fd_t	formSymtab = -1;			/* Symbol table for form handlers */

//enhanced security issues
static int checkSpecialChar(char_t *data, int exception)
{
	if (gstrstr(data,"<") || gstrstr(data,">") ||
		gstrstr(data,"(") || gstrstr(data,")") ||
		gstrstr(data,"\"") || gstrstr(data,"\'") ||
		gstrstr(data,"#") || gstrstr(data,";") ||
		/*gstrstr(data,"/") ||*/ gstrstr(data,"|") ||
		gstrstr(data,"\\") || gstrstr(data,"?") ||
		gstrstr(data,"*"))
	{
		#if 0
		if (exception == 1)
		{
			if (gstrstr(data,"/"))
				return 1;
		}
		#endif
		return 0;
	}
	return 1;
}

/************************************* Code ***********************************/
/*
 *	Process a form request. Returns 1 always to indicate it handled the URL
 */

int websFormHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	char_t *url, char_t *path, char_t *query)
{
	sym_t		*sp;
	char_t		formBuf[FNAMESIZE];
	char_t		*cp, *formName;
	int			(*fn)(void *sock, char_t *path, char_t *args);
	//enhanced security issues
	int valid=0;
	FILE *fp;
	char line_uniName[64];
	char line_uniId[64];

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path == '/');

	websStats.formHits++;

/*
 *	Extract the form name
 */
	gstrncpy(formBuf, path, TSZ(formBuf));
	if ((formName = gstrchr(&formBuf[1], '/')) == NULL) {
		websError(wp, 200, T("Missing form name"));
		return 1;
	}
	formName++;
	if ((cp = gstrchr(formName, '/')) != NULL) {
		*cp = '\0';
	}

/*
 *	Lookup the C form function first and then try tcl (no javascript support 
 *	yet).
 */
	sp = symLookup(formSymtab, formName);
	if (sp == NULL) {
		websError(wp, 200, T("Form %s is not defined"), formName);
	} else {
		fn = (int (*)(void *, char_t *, char_t *)) sp->content.value.integer;
		a_assert(fn);

		//enhanced security issues		
		fp = fopen("/tmp/csrf.uniName", "r");
		if (fp)
		{
			memset(line_uniName, 0, sizeof(line_uniName));
			if (fgets(line_uniName, sizeof(line_uniName), fp) != NULL)
			{
				line_uniName[strlen(line_uniName)-1] = 0;
			}
			fclose(fp);
		} else {
			printf("%s(%d)--: failed to open /tmp/csrf.uniName\n", __FILE__, __LINE__);
		}

		fp = fopen("/tmp/csrf.uniId", "r");
		if (fp)
		{
			memset(line_uniId, 0, sizeof(line_uniId));
			if (fgets(line_uniId, sizeof(line_uniId), fp) != NULL)
			{
				line_uniId[strlen(line_uniId)-1] = 0;
			}
			fclose(fp);
		} else {
			printf("%s(%d)--: failed to open /tmp/csrf.uniId\n", __FILE__, __LINE__);
		}

		if ((gstrstr(query, line_uniName) && gstrstr(query, line_uniId)))
		{
			valid = 1;
		} else if (gstrcmp(formName, "user_login") == 0 || gstrcmp(formName, "user_logout") == 0 ||
			gstrcmp(formName, "setRebootform") == 0	|| gstrcmp(formName, "setTR69Reboot") == 0 ||
			gstrcmp(formName, "loginUrlFilter") == 0 || gstrcmp(formName, "logoutUrlFilter") == 0) {
			valid = 2;
		}

		if (valid == 1)
		{
			char_t *data;
			data = malloc(strlen(query));			
			gstrcpy(data, query);
			websDecodeUrl(data, data, gstrlen(data));
			printf("%s, formName=%s, data=%s\n", __FILE__, formName, data);

			#if 0
			if (gstrcmp(formName, "setSambaShareConfig") == 0) {
				char *token;
				token = gstrtok(data, "&");
				while (token != NULL)
				{
					if (gstrstr(token, "samba_path")) {
						valid = checkSpecialChar(token, 1);
					} else {
						valid = checkSpecialChar(token, 0);
					}
					if (valid == 0)
						break;
					token = gstrtok(NULL, "&");
				}
			} else {
				valid = checkSpecialChar(data, 0);
			}
			#else
			valid = checkSpecialChar(data, 0);
			#endif
			free(data);
		}
		printf("%s, valid=%d\n", __FILE__, valid);
		genToken(1);
		if (valid == 0)
		{
			char redirect_page[64];
			char *url_path;

			sprintf(redirect_page, "http://%s", websGetIpaddrUrl());
			url_path = websGetVar(wp, T("HTTP_REFERER"), redirect_page);
			websRedirect(wp, url_path);
			return 1;
		}
		genToken(2);

		if (fn) {
/*
 *			For good practice, forms must call websDone()
 */
			(*fn)((void*) wp, formName, query);

/*
 *			Remove the test to force websDone, since this prevents
 *			the server "push" from a form>
 */
#if 0 /* push */
			if (websValid(wp)) {
				websError(wp, 200, T("Form didn't call websDone"));
			}
#endif /* push */
		}
	}
	return 1;
}

/******************************************************************************/
/*
 *	Define a form function in the "form" map space.
 */

int websFormDefine(char_t *name, void (*fn)(webs_t wp, char_t *path, 
	char_t *query))
{
	a_assert(name && *name);
	a_assert(fn);

	if (fn == NULL) {
		return -1;
	}

	symEnter(formSymtab, name, valueInteger((int) fn), (int) NULL);
	return 0;
}

/******************************************************************************/
/*
 *	Open the symbol table for forms.
 */

void websFormOpen()
{
	formSymtab = symOpen(WEBS_SYM_INIT);
}

/******************************************************************************/
/*
 *	Close the symbol table for forms.
 */

void websFormClose()
{
	if (formSymtab != -1) {
		symClose(formSymtab);
		formSymtab = -1;
	}
}

/******************************************************************************/
/*
 *	Write a webs header. This is a convenience routine to write a common
 *	header for a form back to the browser.
 */

void websHeader(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *	By license terms the following line of code must not be modified
 */
	websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);

	websWrite(wp, T("Pragma: no-cache\n"));
	websWrite(wp, T("Cache-control: no-cache\n"));
	websWrite(wp, T("Content-Type: text/html\n"));
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html>\n"));
}

/******************************************************************************/
/*
 *	Write a webs footer
 */

void websFooter(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("</html>\n"));
}

/******************************************************************************/

