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

function v6EnableDisplay(On)
{
   if(On == "1"){
     document.getElementById("ipv6_lan").style.display = "";
   }else{
     document.getElementById("ipv6_lan").style.display = "none";
   }
}

function v6Enable(ele)
{
  switch(ele.value)
    {
      case "off": 
        document.IPV6_FORM.IPV6_ENABLE.value="off";
        v6EnableDisplay("0");
      break;
      case "on": 
        document.IPV6_FORM.IPV6_ENABLE.value="on";
        v6EnableDisplay("1");
      break;
   }
}

function v6ULAEnable(ele)
{
  switch(ele.value)
    {
      case "off": 
        document.IPV6_FORM.IPV6_ULA_ENABLE.value="off";
      break;
      case "on": 
        document.IPV6_FORM.IPV6_ULA_ENABLE.value="on";
      break;
   }
}

function loadIPV6Page()
{
	var v6_sw = <%getConf("IPV6_ENABLE");%>;
	var v6ula_sw = <%getConf("LAN_V6_CONFIG");%>;
	if (v6ula_sw == "off")
	{
		document.IPV6_FORM.IPV6_ULA_ENABLE.value="off";
		document.getElementById('ipv6ula_on').checked = false;
		document.getElementById('ipv6ula_off').checked = true;
	}
	else if (v6ula_sw == "on")
	{
		document.IPV6_FORM.IPV6_ULA_ENABLE.value="on";
		document.getElementById('ipv6ula_on').checked = true;
		document.getElementById('ipv6ula_off').checked = false;					
	}
	
	if (v6_sw == "off")
	{
		document.IPV6_FORM.IPV6_ENABLE.value="off";
		document.getElementById('ipv6_on').checked = false;
		document.getElementById('ipv6_off').checked = true;
		v6EnableDisplay("0");
	}
	else if (v6_sw == "on")
	{
		document.IPV6_FORM.IPV6_ENABLE.value="on";
		document.getElementById('ipv6_on').checked = true;
		document.getElementById('ipv6_off').checked = false;		
		v6EnableDisplay("1");			
	}
}

function ApplySetting()
{
	document.IPV6_FORM.submit();
	return true;
}

</script>
</head>

<body onload="loadIPV6Page()">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("ipv6_submenu","ipv6_submenu_focus","ipv6_setting","ipv6_setting_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
<div class="secH1"><script>document.write(gettext("IPv6 Setting"));</script></div>
<!-- Section Begin -->
<div class="secBg">
	<div class="statusMsg"><%getActionResult();%></div>
	<div class="secInfo">
		<br><script>document.write(gettext("You can enable the Gateway's IPv6 networking on this page. After activating IPv6, the Gateway will be able to communicate with networks that use IPv6."));</script><br>
	</div>
	<form method="post" name="IPV6_FORM" action="/goform/setIPv6OnOff?<%getUniName();%>=<%getUniId();%>">  
	<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr>
			<td>
				<div class="secH2"><script>document.write( gettext("IPv6 Settings:"));</script></div>									
			</td>
			<td>
				<input type="radio" id="ipv6_on" name="IPV6_ENABLE" ~cIPV6_ENABLE:on~ value="on" onclick="v6Enable(this);"/><script>document.write( gettext("on"));</script>
				<input type="radio" id="ipv6_off" name="IPV6_ENABLE" ~cIPV6_ENABLE:off~ value="off" onclick="v6Enable(this);"/><script>document.write( gettext("off"));</script>
			</td>
			<td>&nbsp;</td>
		</tr>
	</table>	
	
	<div id="ipv6_lan" style="display:none">
	  <table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr>
			<td>
				<div class="secH2"><script>document.write( gettext("ULA Settings") );</script>:</div>									
			</td>
			<td>
				<input type="radio" id="ipv6ula_on" name="IPV6_ULA_ENABLE" ~cIPV6_ULA_ENABLE:on~ value="on" onclick="v6ULAEnable(this);"/><script>document.write( gettext("on"));</script>
				<input type="radio" id="ipv6ula_off" name="IPV6_ULA_ENABLE" ~cIPV6_ULA_ENABLE:off~ value="off" onclick="v6ULAEnable(this);"/><script>document.write( gettext("off"));</script>
			</td>
			<td>&nbsp;</td>
		</tr>
	  </table>
  </div>

   <div class="submitBg">
   	<input type="submit" id="apply" value="Apply" name="btnApply" onclick="return ApplySetting();" class="submit" title="Apply" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
   	<input type="button" id="reset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
   </div>  
  </form> 
</div>
<!-- Section End -->
</div>
</div>
<script type="text/javascript">
  document.getElementById('apply').value = gettext("Apply");
  document.getElementById('reset').value = gettext("Reset");
</script>
</body>
</html>
