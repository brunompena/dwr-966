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

function pageValidate()
{
	if(!CheckLoginInfo())
		return false;
	var profile_name=document.getElementById("wan_ProfileName").value;
 if(profile_name == "")
 {
   alert(gettext("Profile Name can't be empty."));
   return false;
 }
 if (checkCommonNameField('wan_ProfileName', gettext("Profile Name")) == false) return false;

 var user_name = document.getElementById('wan_UserName').value;
 if (user_name.length > 0)
 {
   if (checkCommonNameField('wan_UserName', gettext("User Name")) == false) return false;
 }

 var pwd = document.getElementById('wan_Password').value;
 if (pwd.length > 0)
 {
   if (checkCommonNameField('wan_Password', gettext("Password")) == false) return false;
 }

	var APN = document.getElementById('wan_APN').value;
 if (APN.length > 0)
 {
 	 if (checkCommonNameField('wan_APN', gettext("APN")) == false) return false;
 }

	return true;
}

function internet_profile_editor_init()
{
	var ProfileConfig = <%getWanProfileConfig();%>;
	var ProfileConfigArr;
	var RoamingPDPType = <%getRoamingPDPType();%>;
	var RoamingPDPTypeArr;
	if (ProfileConfig != "")
	{
		ProfileConfigArr = ProfileConfig.split("#");
		RoamingPDPTypeArr = RoamingPDPType.split("#");
		document.getElementById("wan_ProfileName").value=ProfileConfigArr[0];
		if (ProfileConfigArr[3] == "(none)" || ProfileConfigArr[3] == "")
		{
		  document.getElementById("wan_UserName").value="";
		} else {
    document.getElementById("wan_UserName").value=ProfileConfigArr[3];
		}
		if (ProfileConfigArr[4] == "(none)" || ProfileConfigArr[4] == "")
		{
		  document.getElementById("wan_Password").value="";
		} else {
    document.getElementById("wan_Password").value=ProfileConfigArr[4];
		}
		document.getElementById("wan_AuthMethod").value=ProfileConfigArr[5];
		if (ProfileConfigArr[2] == "(none)" || ProfileConfigArr[2] == "")
		{
		  document.getElementById("wan_APN").value="";
		} else {
    document.getElementById("wan_APN").value=ProfileConfigArr[2];
		}
		document.getElementById("wan_PDPType").value=RoamingPDPTypeArr[0];
	}
	else
	{
		document.getElementById("wan_ProfileName").value="";
		document.getElementById("wan_UserName").value="";
		document.getElementById("wan_Password").value="";
		document.getElementById("wan_AuthMethod").value="0";
		document.getElementById("wan_APN").value="";
		document.getElementById("wan_PDPType").value="0";
	}
	document.getElementById("roaming_PDPType").value=RoamingPDPTypeArr[1];
}
</script>
</head>

<body onload="internet_profile_editor_init();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("lte");%>
<script type="text/javascript">menuChange("lte_menu");leftSubMenuChange("internet_lte_wan_submenu", "internet_lte_wan","internet_profile","internet_profile_href");</script>

<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Profile Configuration"));</script></div>
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo">
		<br /><script>document.write(gettext("You can edit a specific profile in this page."));</script>
		&nbsp;&nbsp;<a class="secLable1" href="internet_profile.asp">&#187; <script>document.write(gettext("Back to Profile List page"));</script></a>
		<br />
		</div>
  <form name="internet_profile_editor" method="post" action="/goform/setWanProfileConfig?<%getUniName();%>=<%getUniId();%>">
		<table cellspacing="0" class="configTbl">
   <tr>
     <td class="secH2" colspan="2"><script>document.write(gettext("Network Profile Content"));</script></td>
   </tr>
			<tr>
				<td width="165"><script>document.write(gettext("Profile Name"));</script></td>
				<td>
				<input type="text" name="wan_ProfileName" id="wan_ProfileName" value="" size="20" maxlength="14" class="configF1" />
				</td>
			</tr>
			<tr>
				<td width="165"><script>document.write(gettext("User Name"));</script></td>
				<td>
				<input type="text" name="wan_UserName" id="wan_UserName" value="" size="20" maxlength="32" class="configF1" />&nbsp; <span class="spanText">[Optional]</span>
				</td>
			</tr>
			<tr>
				<td width="165"><script>document.write(gettext("Password"));</script></td>
				<td>
				<input type="password" name="wan_Password" id="wan_Password" value="" size="20" maxlength="64" class="configF1" />&nbsp; <span class="spanText">[Optional]</span>
				</td>
			</tr>
			<tr>
				<td width="165"><script>document.write(gettext("Auth Method"));</script></td>
				<td>
       <select name="wan_AuthMethod" id="wan_AuthMethod" size="1" class="configF1">
        <option value="0"><script>document.write(gettext("None"));</script></option>
        <option value="1">PAP</option>
        <option value="2">CHAP</option>
        <option value="3"><script>document.write(gettext("PAP and CHAP"));</script></option>
       </select>
				</td>
			</tr>
			<tr>
				<td width="165"><script>document.write(gettext("APN(Access Point Name)"));</script></td>
				<td>
				<input type="text" name="wan_APN" id="wan_APN" value="" size="20" maxlength="64" class="configF1" />&nbsp; <span class="spanText">[<script>document.write(gettext("Optional"));</script>]</span>
				</td>
			</tr>
			<tr>
				<td width="165"><script>document.write(gettext("PDP Type"));</script></td>
				<td>
       				<select name="wan_PDPType" id="wan_PDPType" size="1" class="configF1">
         				<option value="0">IPv4</option>
         				<!--<option value="2">IPv6</option>-->
         				<option value="3"><script>document.write(gettext("IPv4 and IPv6"));</script></option>
       				</select>
				</td>
			</tr>
			<tr>
				<td width="165"><script>document.write(gettext("Roaming PDP Type"));</script></td>
				<td>
       				<select name="roaming_PDPType" id="roaming_PDPType" size="1" class="configF1">
         				<option value="0">IPv4</option>
         				<option value="3"><script>document.write(gettext("IPv4 and IPv6"));</script></option>
       				</select>
				</td>
			</tr>
		</table>
		<div>
			<input type="submit" value="Apply" class="submit" id="button.apply" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" onclick="return pageValidate();" />
			<input type="button" value="Reset" class="submit" id="button.reset" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" onclick="doRedirect();" />
		</div>
	 </form>
	</div>
</div>
</div>
<script type="text/javascript">
 document.getElementById('button.apply').value = gettext("Apply");
 document.getElementById('button.reset').value = gettext("Reset");
</script>
</body>
</html>
