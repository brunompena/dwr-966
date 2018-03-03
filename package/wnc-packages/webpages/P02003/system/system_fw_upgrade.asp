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

function pressApply(state)
{
	if(!CheckLoginInfo())
		return false;
	var mode_flag=<%get_user_engin_mode();%>;
	if(state == "router")
	{
	    return isLocalFirmwareSelected('txtUploadFile', 'img', 2, mode_flag);
		//return isLocalFirmwareSelected('txtUploadFile', 'none', 2, mode_flag);
	} else {
		return isLocalFirmwareSelected('txtUploadLteFile', 'bin', 2, mode_flag);
	}
}

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
</script>
</head>

<body onload="pageInit();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("sys");%>
<script type="text/javascript">menuChange("sys_menu");leftMenuChange("system_fw_upgrade", "system_fw_upgrade_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg" id="contentBg">
	<div class="secH1"><script>document.write(gettext("Firmware Upgrade "));</script></div>
	<!-- Section Begin -->
	<div class="secBg">
		<div class="statusMsg"></div>																						
		<div class="secH2"><script>document.write(gettext("Router Firmware Upgrade"));</script></div>
		<div class="secInfo">
		<br><script>document.write(gettext("The gateways firmware can be upgraded by locating it on your computer and uploading it to the system."));</script>
		<br></div>
		<form name="SYSTEM_FW_UPGRADE" action="/cgi-bin/upload.cgi" method=POST enctype="multipart/form-data">
		<table cellspacing="0" class="configTbl">
	        <tr>
	            <td style="display:none;"><script>document.write(gettext("Select firmware"));</script></td>
	            <td><input type="file" name="txtUploadFile" id="txtUploadFile" size="30" class="configF1" ></td>

	        </tr>
	    </table>
	    <div class="submitBg">
			<input type="submit" value="Router Upgrade" name="router_upgrade" id="router_upgrade" class="submit" title="Router Upgrade" onclick="return pressApply('router');" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
		</div>

		</form>
		<div class="secH2"><script>document.write(gettext("LTE Module Firmware Upgrade"));</script></div>
		<div class="secInfo"><br><script>document.write(gettext("The LTE module firmware can be upgraded by locating it on your computer and uploading it to the system."));</script>
		<br></div>
		 <form name="MODULE_FW_UPGRADE" action="/cgi-bin/uploadMD.cgi" method=POST enctype="multipart/form-data">
		<table cellspacing="0" class="configTbl">						        
	        <tr>
	            <td style="display:none;"><script>document.write(gettext("Select LTE module firmware"));</script></td>
	            <td><input type="file" name="txtUploadLteFile" id="txtUploadLteFile" size="30" class="configF1" /></td>
	        </tr>
	    </table>
	    <div class="submitBg">
			<input type="submit" value="Module Upgrade" name="module_upgrade" id="module_upgrade" class="submit" title="Module Upgrade" onclick="return pressApply('module');" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" />
		</div>	
		</form>
	</div>
	<!-- Section End -->
</div>
<table id="fullpagemsg_table" border="0" cellpadding="0" cellspacing="0" width="100%" height="100%" style="display:none;">
  <tr>
    <td align="center" valign="top">
    <div class="w1000">
	  <div class="midBg" id="waitmsg" style="display:none;">
	    <table border="0" cellpadding="0" cellspacing="0">
		  <tr><td align="center" valign="top">
			<div id="div_contentBg">
			  <div class="secH1_new"><script>document.write(gettext("Status Message"));</script></div>
			  <div class="secBg_xx">
			    <div class="secInfo">&nbsp;</div>
				<div align="center">
				  <table cellspacing="0" class="configTbl">
  				    <tr><td>&nbsp;</td></tr>
  					<tr align="center"><td class="statusMsg"><script>document.write(gettext("The file is uploading, please wait..."));</script></td></tr>
  					<tr align="center"><td class="statusMsg"><script>document.write(gettext("Please be patient and do not remove the cable if a cable is connected."));</script></td></tr>
  					<tr><td>&nbsp;</td>	</tr>
				  </table>
				</div>
				<div class="secInfo">&nbsp;</div>
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
  document.getElementById('router_upgrade').value=gettext("Router Upgrade");
  document.getElementById('module_upgrade').value=gettext("Module Upgrade");
  if (document.getElementById('select_language').value != "en") {
  	document.getElementById('router_upgrade').style.fontSize = "9px";
    document.getElementById('module_upgrade').style.fontSize = "9px";    
  }
</script>
</body>
</html>
