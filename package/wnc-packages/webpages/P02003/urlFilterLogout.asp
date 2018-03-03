<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Language" content="en-us">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>D-Link DWR-966</title>
<link rel="stylesheet" href="style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script language="JavaScript" type="text/javascript">
//<!--
function pageInit()
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
}
//-->
</script>
</head>
<body onload="pageInit();">
	<input type="hidden" value="<%getLanguage();%>" id="select_language" />
	<table border="0" cellpadding="0" cellspacing="0" width="100%" height="100%">
		<tr>
			<td align="center" valign="top">
				<div class="w1000">
					<div class="midBg">
						<table border="0" cellpadding="0" cellspacing="0">
							<tr>
								<td align="center" valign="top">
					        <div id="div_contentBg">
							<div class="secH1_new"><script>document.write(gettext("Web Access Login Succeeded"));</script></div>
							<div class="secBg_new">
							    <form name="action_setup_confirm" method="post" action="/goform/logoutUrlFilter">
								<div class="secInfo">&nbsp;</div>
								<div class="statusMsg"></div>
								<div align="center">
								<table cellspacing="0" class="configTbl">
									<tr>
										<td >&nbsp;</td>
									</tr>
									<tr>
   										<td>
   										<script>document.write(gettext("You have been successfully authenticated by Parental Control"));</script>
   										<br>
   										<script>document.write(gettext("Type the web address you wish to access in your browser's address bar and hit enter"));</script>
										<br>
										<script>document.write(gettext("Press the 'Logout' button below to delete your session."));</script>
										</td>
									</tr>
									<tr>
										<td >&nbsp;</td>
									</tr>
									</table>
									</div>
								<div class="submitBg_new">
									<input type="submit" id="pctrl_logout" value="Logout" class="submit" title="Logout" name="button.logout.urlFilterLogin" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
								</div>
								</form>
							</div>
						</div>
						</td></tr>
						</table>
					</div>
				</div>
			</td>
		</tr>
	</table>
<script type="text/javascript">
	document.getElementById('pctrl_logout').value=gettext("Logout");
</script>
</body>
</html>
