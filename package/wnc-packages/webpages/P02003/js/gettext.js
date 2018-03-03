//*********************************************************************
//* 
//* @(#) gettext.js - Multi lingual translation of strings
//* 
//* 
//* Should be functional under all browsers supporting javascript
//*********************************************************************
//* @(#) 2006-05-08/Erik Bachmann (e_bachmann@hotmail.com)
//*********************************************************************

//--- Globale variables -----------------------------------------------

var browser_name	= "browser unknown";	// Identified browser
var browser_language	= "language unknown";	// Browser language code
var gettext_debug	= 0;	// 0 = no debug; 1 = debugging messages

var ahMessages = new Object;	// Main hash for local messages

// Find browser
if ((navigator.userAgent).indexOf("Opera")!=-1) 
	{browser_name='Opera';}
else if(navigator.appName.lastIndexOf("Netscape") != -1) // incl. Firefox
	{browser_name='Mozilla';}
else if(navigator.appName.lastIndexOf("Explore") != -1)
	{browser_name='Microsoft Internet Explorer';}

//find langage
if(browser_name=='Mozilla') 
{
	if (navigator.language=="zh-tw") 
                browser_language='zh';
 	else browser_language="en";
}
if(browser_name=='Microsoft Internet Explorer') 
{
	if (navigator.browserLanguage=="zh-tw") 
		browser_language='zh';
	else browser_language="en";
	//if (navigator.browserLanguage=="en") {browser_language='en';}
}


function change_lang(lang)
{
	if (lang=="en")
		browser_language="en";
	else browser_language="zh";
}
//---------------------------------------------------------------------

//*********************************************************************
// 
// TITLE:
//	gettext
// 
// DESCRIPTION: 
// 	Get a translation from message array
// 
// SOURCE:
// 	selfmade
// 
// ARGUMENTS:
// 	msg_tag	Tag of message to return
// 
// NOTE:
// 	
// EXAMPLE:
// 
//	<scripts>document.write( gettext( "Hello_world" ) );</scripts>
// 
//*********************************************************************
// 2006-05-08/Erik Bachmann
//*********************************************************************
function gettext( msg_tag )
{
	//browser_language="zh";
	//browser_language="en";
	
	//var iLanguage = parent.document.getElementById("multi_language").selectedIndex;
	var browser_language = parent.document.getElementById("select_language").value;

	if (! ahMessages[browser_language])
	{	// Return tag if translation not defined
		if (gettext_debug)
		{
			return msg_tag 
		+	"(<b>ERROR!</b> Language: \""
		+	browser_language 
		+	"\" not defined)";
		}
		else {return msg_tag + "(" + browser_language + ")";}
	}


	if (ahMessages[browser_language][msg_tag])
	{
		return ahMessages[browser_language][msg_tag];
	}
	else
	{	// Return tag if translation not defined
		//return msg_tag + "(" + browser_language + ")"
		if (gettext_debug)
		{
			return msg_tag 
		+	"(<b>ERROR!</b> Tag: \"" 
		+	msg_tag + "\" not defined in lang \""
		+	browser_language 
		+	"\")";
		}
		else 
		{
			return msg_tag;
		//+	"(" 
		//+	browser_language 
		//+	")";
		}
	}
}	//*** gettext() ***

//*** End of File *****************************************************
