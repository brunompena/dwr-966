
<html>
<head>
<meta http-equiv="Content-Language" content="en-us">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>D-Link DWR-966</title>
<link rel="stylesheet" href="style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script language="javascript" src="../js/textValidations.js" type="text/javascript"></script>
<script language="JavaScript" type="text/javascript">
//<!--
function loginValidate()
{
	var txtFieldIdArr = new Array ();
	txtFieldIdArr[0] = "txtUserName," + gettext("Please enter a valid username");
	txtFieldIdArr[1] = "txtLoginPwd," + gettext("Please enter a valid password");

	if (txtFieldArrayCheck (txtFieldIdArr) == false)
		return false;

	return true;
}

function loginInit ()
{
	var imsi = "<%getIMSI_mem();%>";
	var language = "<%getLanguage();%>";
	if (language == "pl"){
		if (imsi == "26012")
		{
			document.getElementById('div_contentBg').className = "contentBg0_cp_pl";
		} else if (imsi == "26001") {
			document.getElementById('div_contentBg').className = "contentBg0_plk_pl";
		} else {
			document.getElementById('div_contentBg').className = "contentBg0_new_pl";
		}
	}else{
		if (imsi == "26012")
		{
			document.getElementById('div_contentBg').className = "contentBg0_cp_en";
		} else if (imsi == "26001") {
			document.getElementById('div_contentBg').className = "contentBg0_plk_en";
		} else {
			document.getElementById('div_contentBg').className = "contentBg0_new_en";
		}
	}
	/*
	if (imsi == "26012")
	{
		document.getElementById('div_contentBg').className = "contentBg0_cp";
	} else if (imsi == "26001") {
		document.getElementById('div_contentBg').className = "contentBg0_plk";
	} else {
		document.getElementById('div_contentBg').className = "contentBg0_new";
	}
	*/

	var usrObj = document.getElementById ('txtUserName');
	if (!usrObj) return;
	usrObj.focus ();
}
//-->
</script>
</head>
<body onload="loginInit()">
	<input type="hidden" value="<%getLanguage();%>" id="select_language" />
	<table border="0" cellpadding="0" cellspacing="0" width="100%" height="100%">
		<tr><td align="center" valign="top">
			<div class="w1000">
				<!-- Main menu Start -->
				<div class="mainMenuBg"></div>
				<!-- Main menu End -->

				<div class="midBg">
					<table border="0" cellpadding="0" cellspacing="0">
						<tr>
							<td align="center" valign="top">
							<div id="div_contentBg">
							<div class="secH1_new"><script>document.write(gettext("Web Access Login"));</script></div>
                        	<form method="post" action="/goform/loginUrlFilter">
	                    		<input type="hidden" name="thispage" value="urlFilterLogin.htm">
								<div class="secBg_new">
								<div class="secInfo">&nbsp;</div>
								<div class="statusMsg"><%getActionResult();%></div>
								<div align="center">
								<table cellspacing="0" class="configTbl">
								<tr>
									<td>&nbsp;</td>
									<td>&nbsp;</td>
									<td>&nbsp;</td>
								</tr>
								<tr>
									<td><script>document.write(gettext("Username"));</script>:</td>
									<td><input type="text" name="users.username" id="txtUserName" size="20" class="configF1" maxlength="32"></td>
									<td>&nbsp;</td>
								</tr>
								<tr>
									<td><script>document.write(gettext("Password"));</script>:</td>
									<td><input type="password" name="users.password" id="txtLoginPwd" size="20" class="configF1" maxlength="32"></td>
									<td>&nbsp;</td>
								</tr>
								<tr>
									<td>&nbsp;</td>
									<td>&nbsp;</td>
									<td>&nbsp;</td>
								</tr>
								</table>
								</div>
								<div class="submitBg_new"><input type="submit" id="pctrl_login" value="Login" class="submit" title="Login" name="button.login.urlFilterLogout" onclick="return loginValidate();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></div>
								</div>
								<!-- Warning message -->
								<div class="secInfo"><script>document.write(gettext("You can't visit the website. You may not authenticate successfully or you are not allowing to access internet during this time period."));</script></div>
							</form>
							</div>
					</td></tr>
					</table>
				</div>
			</div>
		</td></tr>
	</table>
<script type="text/javascript">
	document.getElementById('pctrl_login').value=gettext("Login");
</script>
</body>
</html>
