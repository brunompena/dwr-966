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
<script language="JavaScript" src="../js/ipv4AddrValidations.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/jquery-1.4.2.min.js"></script>
<script type="text/javascript" src="../js/jquery.json-2.2.min.js"></script>
<script type="text/javascript" src="../js/j_common.js"></script>

<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);

var ip_rule_V6=/^([\da-fA-F]{1,4}:){6}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^::([\da-fA-F]{1,4}:){0,4}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:):([\da-fA-F]{1,4}:){0,3}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){2}:([\da-fA-F]{1,4}:){0,2}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){3}:([\da-fA-F]{1,4}:){0,1}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){4}:((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){7}[\da-fA-F]{1,4}$|^:((:[\da-fA-F]{1,4}){1,6}|:)$|^[\da-fA-F]{1,4}:((:[\da-fA-F]{1,4}){1,5}|:)$|^([\da-fA-F]{1,4}:){2}((:[\da-fA-F]{1,4}){1,4}|:)$|^([\da-fA-F]{1,4}:){3}((:[\da-fA-F]{1,4}){1,3}|:)$|^([\da-fA-F]{1,4}:){4}((:[\da-fA-F]{1,4}){1,2}|:)$|^([\da-fA-F]{1,4}:){5}:([\da-fA-F]{1,4})?$|^([\da-fA-F]{1,4}:){6}:$/;
function checkIpAddr (thisObj, ipStr, alertStr)
{
	var thisObjVal = thisObj.value;
	var i18n_invalidIPAddr = gettext("Invalid IP Address.");
	var i18n_forOctet = 'for octet ';	
	if(thisObjVal.indexOf('.') != -1)
	{
		if(ipStr == 'IP')
		{
			if( ipv4Validate (thisObj.id, gettext("IP"), false, true, i18n_invalidIPAddr, i18n_forOctet, true) == false)
			{
				thisObj.focus();
				return false;
			}
			else
				return true;
		}
	}	
	alert(alertStr);
	thisObj.focus();
	return false;
}

function pageValidate()
{	
	if(!CheckLoginInfo())
		return false;
	var txtFieldIdArr = new Array ();	
	txtFieldIdArr[0] = "host_name," + gettext("Please enter valid Computer Name");
	//txtFieldIdArr[1] = "ip_address,Please enter valid IP Address";	
	//txtFieldIdArr[2] = "ipv6_address,Please enter valid IP Address";	
	if (txtFieldArrayCheck (txtFieldIdArr) == false)		
		return false;
	if (checkCommonNameField('host_name', gettext("Computer Name"), gettext("Only allow to input [a-z], [A-Z], [0-9], '_', '-', '.' and '@' characters.")) == false)
		return false; 

	var ipAddrObj = document.getElementById('ip_address');		//IPv4 Address check
	if (ipAddrObj.value=="<%getLanBasic("IpAddress");%>")
	{
		alert(gettext("Please enter valid IPv4 Address.\nRouter IP Address is not allowed."));
		return false;
	}
	
	if(ipAddrObj.value != ""){				
		if (checkIpAddr(ipAddrObj, 'IP', gettext('Please enter valid IPv4 Address')) == false) 
	  		return false;
	}

	var ipv6AddrObj = document.getElementById('ipv6_address');	//IPv6 Address check
	if (ipv6AddrObj.value=="<%getLanBasic("IpAddress");%>")
	{
		alert(gettext("Please enter valid IPv6 Address.\nRouter IP Address is not allowed."));
		return false;
	}

	if(ipv6AddrObj.value != ""){
		if(!ip_rule_V6.test(ipv6AddrObj.value)){
			alert(gettext("Please enter valid IPv6 Address.\n(xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx, eg: 1000:2000:3000:4000:5000:6000:7000:8000)"));
			return false;
		}
	}

	//Check IPv4 and IPv6 address all is null
	if((ipAddrObj.value == "")&&(ipv6AddrObj.value == "")){
		alert(gettext("Please input IPv4 or IPv6 address."));
		return false;
	}	

	return true;	
}

function pageLoad()
{
	var HostConfig = <%getHostConfig();%>;
	var HostConfigArr;
	if (HostConfig!="")	{
		document.getElementById("host_name").disabled = true;
		HostConfigArr = HostConfig.split("#");
		document.getElementById("host_name").value=HostConfigArr[0];
		if (HostConfigArr[1] != "")
			document.getElementById("ip_address").value=HostConfigArr[1];
		else
			document.getElementById("ip_address").value="";
		if (HostConfigArr[2] != "")
			document.getElementById("ipv6_address").value=HostConfigArr[2];
		else
			document.getElementById("ipv6_address").value="";
	} else {
		document.getElementById("host_name").disabled = false;
		document.getElementById("host_name").value="";
		document.getElementById("ip_address").value="";	
		document.getElementById("ipv6_address").value="";	
	}
}
</script>
</head>	

<body onload="pageLoad();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftMenuChange("firewall_submenu_focus", "firewall_submenu_focus_href");leftSubMenuChange("firewall_submenu","firewall_submenu_focus","known_computers","known_computers_href");</script>
<!-- Main Menu and Submenu End -->


<!-- Add new known computer -->
<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Known Computer Configuration"));</script></div>
	<!-- Section Begin -->
	<div class="secBg">
		<div class="secInfo">
			<br><script>document.write(gettext("In this section you can assign an IPv4 and IPv6 address to a hostname."));</script>
			<br>
			<a class="secLable1" href="known_computers.asp">>> <script>document.write(gettext("Back to Known Computers page"));</script></a>
		</div>
		<form name="frmKnownCompConfig" method="post" action="/goform/setHostConfig?<%getUniName();%>=<%getUniId();%>">
		<table cellspacing="0" class="configTbl">
			<tr>
				<td><script>document.write(gettext("Computer Name"));</script></td>
				<td>
				<input type="text" name="host_name" value="" id="host_name" size="30" class="txtbox" maxlength="32">
				</td>
			</tr>
			<tr>
				<td><script>document.write(gettext("IPv4 Address"));</script></td>
				<td>
				<input type="text" name="ip_address" value="" id="ip_address" size="40" class="txtbox" maxlength="15" onkeypress="return keypress_ip_format(event)">
				</td>
			</tr>
			<tr>
				<td><script>document.write(gettext("IPv6 Address"));</script></td>
				<td>
				<input type="text" name="ipv6_address" value="" id="ipv6_address" size="40" class="txtbox" maxlength="39">
				</td>
			</tr>
		</table>
		<div>
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td class="blank1" height="30">&nbsp;</td>
				</tr>
			</table>
		</div>
		<div class="submitBg">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td><input type="submit" id="HostTblApply" value="Apply" class="submit" title="Apply" onclick="return pageValidate();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
					<td><input type="button" id="HostTblReset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
				</tr>
			</table>
		</div>
		</form>
	</div><!-- Section End -->
</div>
</div>
<script type="text/javascript">
	document.getElementById('HostTblApply').value=gettext("Apply");
	document.getElementById('HostTblReset').value=gettext("Reset");
</script>
</body>
</html>
