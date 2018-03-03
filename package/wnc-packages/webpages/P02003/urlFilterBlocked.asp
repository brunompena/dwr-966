<html>
<head>
<title>Website Blocked</title>
<meta http-equiv="Content-Language" content="en-us">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<style type="text/css">
.infoMsg, .infoMsg a, .infoMsg a:hover {
	font-family:Trebuchet MS,Verdana,sans-serif,Helvetica,Arial;
	color:#000000;
	font-size:14px;
	font-weight: bold;
	line-height:32px;
}
.infoMsg a { text-decoration: underline; }
.infoMsg a:hover { text-decoration: none; }
</style>
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script language="JavaScript" type="text/javascript">
//<!--
//-->
</script>
</head>
<body marginwidth="0" marginheight="0" topmargin="0" leftmargin="0" rightmargin="0" bottommargin="0" bgcolor="dedede">
	<input type="hidden" value="<%getLanguage();%>" id="select_language" />
	<table border="0" cellpadding="2" cellspacing="2" width="100%" height="100%">
		<tr valign="middle">
			<td align="center" class="infoMsg">
				<script>document.write(gettext("You have been denied access to this web page due to restrictions in your web browsing profile."));</script>
				<br>
				<script>document.write(gettext("Please contact your administrator or follow this link to login with a different web browsing profile."));</script>
				<br>
				 (<a href="http://<%getLanBasic('IpAddress');%>/urlFilterLogin.asp">http://<%getLanBasic("IpAddress");%>/urlFilterLogin.asp</a>)
			</td>
		</tr>
	</table>
</body>
</html>
