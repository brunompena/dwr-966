<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
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

function UpnpPortInit()
{
	var enabled = <%getUpnpPortEnable();%>;
	if (enabled == 1) {
	  document.getElementById('enable_upnp').checked = true;
	} else {
	  document.getElementById('enable_upnp').checked = false;
	}
}

function setRefresh()
{
	if(!CheckLoginInfo("ResetPage"))
		return false;
	document.upnp_list.action="/goform/setUpnpListRefresh?<%getUniName();%>=<%getUniId();%>";
	document.upnp_list.submit();
	return true;	
}

function setClear()
{
	if(!CheckLoginInfo())
		return false;
	document.upnp_list.action="/goform/setUpnpListClear?<%getUniName();%>=<%getUniId();%>";
	document.upnp_list.submit();
	return true;	
}

function checkApply()
{
	if(!CheckLoginInfo())
		return false;
	return true;
}
</script>
</head>

<body onload="UpnpPortInit();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("nat_submenu","nat_submenu_focus","upnp_port","upnp_port_href");</script>
<!-- Main Menu and Submenu End -->
<div class="contentBg">
<div class="secH1"><script>document.write(gettext("UPnP Port Mapping"));</script></div>
<!-- Section Begin -->
	<div class="secBg">
	<div class="statusMsg"></div>
	<div class="secH2"><script>document.write(gettext("UPnP Settings"));</script></div>

	<form name="upnp_setting" method="post" action="/goform/setUpnpEnable?<%getUniName();%>=<%getUniId();%>">
		<input type="hidden" name="enable_upnp__dummy" />
		<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
			<tr>
				<td>&nbsp;</td>
				<td><input type="checkbox" id="enable_upnp" name="enable_upnp">
				<input type="hidden" name="table.enable" value="0">
				</td>
				<br>
				<td><script>document.write(gettext("Turn UPnP On"));</script></td>
				<td>&nbsp;</td>
			</tr>
		</table>
		<div class="submitBg">
			<input type="submit" id="upnp_apply" value="Apply" class="submit" title="Apply" onclick="return checkApply();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
			<input type="button" id="upnp_reset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
		</div>
	</form>
	<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr>
			<td class="secBot">&nbsp;</td>
		</tr>
	</table>

	<div class="secH2"><script>document.write(gettext("UPnP Portmap Table"));</script></div>
	<form name="upnp_list" action="/goform/setUpnpList?<%getUniName();%>=<%getUniId();%>" method="post">
	<input type="hidden" name="upnp_list_dummy" value="0">
	<table border="0" cellpadding="0" cellspacing="0">
		<tr>
			<td class="secMid">
			<table border="0" cellpadding="0" cellspacing="0" width="100%">
				<tr>
					<td class="tdH"><script>document.write(gettext("Active"));</script></td>
					<td class="tdH"><script>document.write(gettext("Protocol"));</script></td>
					<td class="tdH"><script>document.write(gettext("Internal Port"));</script></td>
					<td class="tdH"><script>document.write(gettext("External Port"));</script></td>
					<td class="tdH"><script>document.write(gettext("IP Address"));</script></td>
				</tr>
				<tbody><%getUpnpPortList();%></tbody>  
			</table>
			</td>
		</tr>
		<tr>
			<td class="secBot">&nbsp;</td>
		</tr>
		<tr>
			<td>
				<div class="submitBg">
				<form method="post" action="platform.cgi">
					<input type="hidden" name="thispage" value="upnpPortMapping.htm">
					<table border="0" cellpadding="0" cellspacing="0">
						<tr>
							<td><input type="submit" id="upnp_refresh" value="Refresh Table" class="submit" title="Refresh Table" onclick="return setRefresh();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
							<td><input type="submit" id="upnp_clear" value="Clear Table" class="submit" title="Clear Table" onclick="return setClear();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>							
						</tr>
					</table>
				</form>
				</div>
			</td>
		</tr>
		<!--
		<tr>
		<td class="blank1" height="48">&nbsp;</td>
		</tr>
		-->
	</table>
	</form>
</div>
<!-- Section End -->
</div>
<!-- Content Bg End -->
</div><!-- all end -->
<script type="text/javascript">
	document.getElementById('upnp_apply').value=gettext("Apply");
	document.getElementById('upnp_reset').value=gettext("Reset");
	document.getElementById('upnp_refresh').value=gettext("Refresh Table");
	document.getElementById('upnp_clear').value=gettext("Clear Table");
</script>
</body>
</html>
