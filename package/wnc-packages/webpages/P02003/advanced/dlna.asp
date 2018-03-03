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

var nat_enable=<%getFwNatEnable();%>;
if(nat_enable!="1")
{
	window.location.href="../login.asp";
}
function getDLNAStatus()
{  
  var dlna_status = <%getDLNA();%>;
  if (dlna_status)
  {
	document.getElementById("dlna_enable").checked = true;
  }
  else
  {
	document.getElementById("dlna_enable").checked = false;
  }  
}

function enable_dlna()
{
  if (document.getElementById("dlna_enable").checked==true)
  {
  	document.getElementById("dlna_enable").value = "on";
  }
  else
  {
  	document.getElementById("dlna_enable").value = "off";	
  }
}

function checkApply()
{
	if(!CheckLoginInfo())
		return false;
	return true;
}

</script>

</head>


<body onload="getDLNAStatus();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftMenuChange("dlna", "dlna_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
<div class="secH1">DLNA</div>
<!-- Section Begin -->
<div class="secBg">
	<div class="statusMsg"><%getActionResult();%></div>
	<div class="secH2"><script>document.write(gettext("Media Server Settings"));</script></div><br>
	<form name="dlna_setting" method="post" action="/goform/setDLNA?<%getUniName();%>=<%getUniId();%>">
	<input type="hidden" name="dlna_dummy" value="0">  
	<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr>
			<td>&nbsp;</td>
			<td>
				<input type="checkbox" id="dlna_enable" name="dlna_enable" onclick="enable_dlna();" />      
			</td>
			<td><script>document.write(gettext("Enable Media Server"));</script></td>
			<td>&nbsp;</td>
		</tr>
	</table>
	<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr>
			<td class="secMid">
				<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
					<tr>
						<td>&nbsp;</td>
						<td><script>document.write(gettext("Media Server Name"));</script>:</td>
						<td>DWR-966-DLNA</td>
						<td>&nbsp;</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>

	<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr><td>
		<div class="submitBg">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td><input type="submit" id="dlna_apply" value="Apply" class="submit" title="Apply" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" onclick="return checkApply();" /></td>
					<td><input type="button" id="dlna_reset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" /></td>
				</tr>
			</table>
		</div>
		</td></tr>
	</table>
	</form>

</div>
<!-- Section End -->
</div>
</div>
<script type="text/javascript">
	document.getElementById('dlna_apply').value=gettext("Apply");
	document.getElementById('dlna_reset').value=gettext("Reset");
</script>
</body>
</html>
