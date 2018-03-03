<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="JavaScript" src="../js/textValidations.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/jquery-1.4.2.min.js"></script>
<script type="text/javascript" src="../js/jquery.json-2.2.min.js"></script>
<script type="text/javascript" src="../js/j_common.js"></script>

<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);

var IPv6FirewallConfig = <%getIPv6FirewallConfig();%>;
var IPv6FirewallConfigArr = IPv6FirewallConfig.split("#");
function servicechk ()
{
	var selOptVal = comboSelectedValueGet ('selPortservice');
	if (!selOptVal) return;
	switch (parseInt(selOptVal, 10))
	{
	case -1: /* Clickable for Add NEW Service */
		fieldStateChangeWr ('', '', 'AddNewServce', '');
		break;
	default:
		fieldStateChangeWr ('AddNewServce', '', '', '');
	}
}
function addCmpNamechk ()
{
	var selObjVal = comboSelectedValueGet ('selIPv6Schedule');
	if (!selObjVal) return;
	switch (parseInt(selObjVal, 10))
	{
	case -1: /* OTHER - Clickable Add New Computer */
		fieldStateChangeWr ('', '', 'addNewCmp', '');
		break;
	default:
		fieldStateChangeWr ('addNewCmp', '', '', '');
	}
}

function ipv6FirewallConfigAction(action)
{
	if(!CheckLoginInfo())
		return false;
	if(action == "apply")
	{
		if(document.getElementById("selPortservice").value == IPv6FirewallConfigArr[0] && 
			document.getElementById("selIPv6Schedule").value == IPv6FirewallConfigArr[1])
		{
			document.getElementById("EditFlag").value="same";
		} else
			document.getElementById("EditFlag").value="change";
	}
	document.getElementById('IPv6FirewallConfigAction').value=action;
	return true;
}

function pageLoad()
{
	document.getElementById("selPortservice").value=IPv6FirewallConfigArr[0];
	document.getElementById("selIPv6Schedule").value = IPv6FirewallConfigArr[1];
	if (IPv6FirewallConfigArr[2]=='0')
		document.getElementById("status").value="Allow";
	else
		document.getElementById("status").value="Block";
}

</script>
</head>	

<body onload="pageLoad(); servicechk(); addCmpNamechk();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("ipv6_submenu","ipv6_submenu_focus","ipv6_filtering","ipv6_filtering_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Frewall Rule Settings"));</script></div>
	<!-- Section Begin -->
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo">
			<br><script>document.write(gettext("You can allow or block common or custom services from being used by a computer in the known computer list."));</script>
			<br><a class="secLable1" href="ipv6_filtering.asp">>> <script>document.write(gettext("Back to IPv6 Firewall page"));</script></a>
		</div>
		<form name="IPv6FirewallEdit" method="post" action="/goform/setIPv6FirewallConfig?<%getUniName();%>=<%getUniId();%>">
		<input type="hidden" name="IPv6FirewallConfigAction" id="IPv6FirewallConfigAction" value="">
		<input type="hidden" name="EditFlag" id="EditFlag" value="">
		<table cellspacing="0" class="configTbl">
			<tr>
				<td><script>document.write(gettext("Application"));</script></td>
				<td >
					<select size="1" name="service" id="selPortservice" class="configF1" onchange="return servicechk();" style="width:182px;">
						<%getServiceList();%>                 
					</select>
				</td>
				<td>
				<input type="submit" class="tblbtn_dis" value="Add Application" name="button.add.splAppsConfig" id="AddNewServce" title="Add Application" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6FirewallConfigAction('addService');"></td>
			</tr>
			<tr>
				<td><script>document.write(gettext("Computer Name"));</script></td>
				<td>
					<select size="1" name="name" class="configF1" id="selIPv6Schedule" onchange="return addCmpNamechk();" style="width:182px;">                        
						<%getIPv6HostList();%>      						
					</select>
			    </td>
				<td>
				<input type="submit" class="tblbtn_dis" value="Add Computer" name="button.addCmp.dnsKnownComputerConfig" id="addNewCmp" title="Add Computer" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6FirewallConfigAction('addHost');"></td>
			</tr>
			<tr>
				<td><script>document.write(gettext("Status"));</script></td>
				<td colspan="2">
					<select name="status" id="status" size="1" class="configF1">
						<option value="Allow"><script>document.write(gettext("Allow"));</script></option>
						<option value="Block"><script>document.write(gettext("Block"));</script></option>
					</select>
				</td>
			</tr>
		</table>
		<div>
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td class="secBot">&nbsp;</td>
				</tr>
			</table>
		</div>
		<div class="submitBg">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td><input type="submit" value="Apply" class="submit" title="Apply" id="apply" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" onclick="return ipv6FirewallConfigAction('apply');"></td>
					<td><input type="button" value="Reset" class="submit" title="Reset" id="reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
				</tr>
			</table>
		</div>
		</form>
	</div><!-- Section End -->
</div>
</div>
<script type="text/javascript">
  document.getElementById('apply').value=gettext("Apply");
  document.getElementById('reset').value=gettext("Reset");
  document.getElementById('AddNewServce').value=gettext("Add Application");
  document.getElementById('addNewCmp').value=gettext("Add Computer");
</script>
</body>
</html>
