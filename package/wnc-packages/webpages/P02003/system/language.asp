<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/jquery-1.4.2.min.js"></script>
<script type="text/javascript" src="../js/jquery.json-2.2.min.js"></script>
<script type="text/javascript" src="../js/j_common.js"></script>

<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);

function selectLanguage()
{
	var language = "<%getLanguage();%>";

	switch(language)
	{
		case 'en':
			document.getElementById("multi_language").selectedIndex = 0;
			break;
		case 'pl':
			document.getElementById("multi_language").selectedIndex = 1;
			break;
		default:
			document.getElementById("multi_language").selectedIndex = 0;
			break;
	}
	return;
}
</script>
</head>

<body onload="selectLanguage();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("sys");%>
<script type="text/javascript">menuChange("sys_menu");leftMenuChange("language", "language_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Language"));</script></div>
	<form method="post" action="/goform/setLanguage?<%getUniName();%>=<%getUniId();%>">
	<!-- Section Begin -->
	<div class="secBg">
		<div class="statusMsg"></div>		
		<div class="secInfo">
			<br><script>document.write(gettext("On clicking the button to set the language of the web pages."));</script>
			<br>
		</div>
		<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
			<tr>
				<td>
					<div class="secH2"><script>document.write(gettext("Select your language:"));</script></div>
				</td>
				<td>
					<select id="multi_language" name="multi_language" size="1" class="configF1">
						<option value="en"><script>document.write(gettext("English"));</script></option>
						<option value="pl"><script>document.write(gettext("Polish"));</script></option>
					</select>
				</td>
			</tr>
		</table>
		<div class="submitBg">
			<input type="submit" id="language_apply" title="Apply" value="Apply" class="submit" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" />
		</div>
	</div>
	</form>
</div>
<script type="text/javascript">
	document.getElementById('language_apply').value=gettext("Apply");
</script>
</body>
</html>
