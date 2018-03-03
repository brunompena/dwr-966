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

function getTR69Info()
{
  var TR69Enable = "<%getTR69Enable();%>";
	  
  if (TR69Enable == "1"){
	document.getElementById("tr69_en").checked = true;
  }else{
	document.getElementById("tr69_en").checked = false;
  }
}
function getTR69Data()
{
  var TR69Data = "<%getTR69Data();%>";
	  
  if (TR69Data == "usb0"){
	document.getElementById("interface_set").selectedIndex = 0;
  }else if (TR69Data == "usb1"){
	document.getElementById("interface_set").selectedIndex = 1;
  }
}
function TR69Disable(state)
{   
  document.getElementById("tr69_en").checked = state;
}

function submitActionTR69()
{
  if (document.getElementById("tr69_en").checked == false){
     document.getElementById("tr69_en").value = "off";
  }else{
     document.getElementById("tr69_en").value = "on";
	 if (document.getElementById("acsURL").value == ""){
	 	alert("Please input ACS server URL");
		return false;
	 }
  }  
  document.lan_basic.action="/goform/setCWMPSettings?<%getUniName();%>=<%getUniId();%>";
  return true;
}

</script>
</head>

<body id="TR69Page" onload="getTR69Info();getTR69Data();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu start -->
<%writeMainMenu();%>
<%writeLeftMenu("sys");%>
<script type="text/javascript">menuChange("sys_menu");leftMenuChange("tr69", "tr69_href");</script>
<!-- Main Menu end -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Tr-69 Settings"));</script></div>
	<div class="secBg">
  		<hr/>
  		<form name="lan_basic" method="post">
  		<table border="0" cellpadding="0" cellspacing="0" style="margin:10px 0 20px;">
	      <tr><td>
	        <label for="" class="text_gray"><script>document.write(gettext("Enable"));</script></label>
	        <input name="tr69_en" id="tr69_en" type="checkbox" style="margin-left:10px;" checked=false onclick="TR69Disable(this.checked);"/>
	      </td></tr>
	      <tr>
	        <td><label for="provisioningCode" class="text_gray" ><script>document.write(gettext("Device provisioning Code"));</script></label></td>
	        <td><input class="box" id="provisioningCode" name="provisioningCode" type="text" size="20" value="<%getCWMPSettings("provisioningCode");%>"/></td>
	      </tr>
	      <tr>
	        <td><label for="acsURL" class="text_gray" ><script>document.write(gettext("ACS Server URL"));</script></label></td>
	        <td><input class="box" id="acsURL" name="acsURL" type="text" size="50" value="<%getCWMPSettings("acsURL");%>"/></td>
	      </tr>
	      <tr>
	        <td><label for="username" class="text_gray" ><script>document.write(gettext("Username"));</script></label></td>
	        <td><input class="box" id="username" name="username" type="text" size="20" value="<%getCWMPSettings("username");%>" /></td>
	      </tr>
	      <tr>
	        <td><label for="password" class="text_gray" ><script>document.write(gettext("Password"));</script></label></td>
	        <td><input class="box" id="password" name="password" type="text" size="20" value="<%getCWMPSettings("password");%>" /></td>
	      </tr>
	      <tr>
	        <td><label for="cruUsername" class="text_gray" ><script>document.write(gettext("Connection Request Username"));</script></label></td>
	        <td><input class="box" id="cruUsername" name="cruUsername" type="text" size="20" value="<%getCWMPSettings("cruUsername");%>" /></td>
	      </tr>
	      <tr>
	        <td><label for="cruPassword" class="text_gray" ><script>document.write(gettext("Connection Request Password"));</script></label></td>
	        <td><input class="box" id="cruPassword" name="cruPassword" type="text" size="20" value="<%getCWMPSettings("cruPassword");%>" /></td>
	      </tr>
	      <tr>
	        <td><label for="certPassword" class="text_gray" ><script>document.write(gettext("Client Certificate Password"));</script></label></td>
	        <td><input class="box" id="certPassword" name="certPassword" type="text" size="20" value="<%getCWMPSettings("certPassword");%>" /></td>
	      </tr>
	    </table>

	    <div style="margin:10px 0 0;">
	      <label for="" class="text_gray">Enable STUN</label>
	      <input type="checkbox">
	    </div>
    
	    <table border="0" cellpadding="0" cellspacing="0" style="margin:10px 0 0 20px;">
	      <tr>
	        <td><label for="stunURL" class="text_gray" ><script>document.write(gettext("STUN Server"));</script></label></td>
	        <td><input class="box" id="stunURL" name="stunURL" type="text" size="20" value="<%getCWMPSettings("stunURL");%>" /></td>
	      </tr>
	      <tr>
	        <td><label for="stunUsername" class="text_gray" ><script>document.write(gettext("STUN Usename"));</script></label></td>
	        <td><input class="box" id="stunUsername" name="stunUsername" type="text" size="20" value="<%getCWMPSettings("stunUsername");%>" /></td>
	      </tr>
	      <tr>
	        <td><label for="stunPassword" class="text_gray" ><script>document.write(gettext("STUN Password"));</script></label></td>
	        <td><input class="box" id="stunPassword" name="stunPassword" type="text" size="20" value="<%getCWMPSettings("stunPassword");%>" /></td>
	      </tr>
	    </table>

	    <div style="margin:10px 0 0;">
	    <table><tr>
	      <td><label class="text_gray"><script>document.write(gettext("Interface Setting"));</script></label></td>
	      <td><select class="box" name="interface_set" id="interface_set" style="width:180px;">
	        <option value="usb0">USB 0</option>
	        <option value="usb1">USB 1</option>
	      </select></td>
	    </tr></table>
	    </div>

	    <div style="margin:10px 0 0;">
	    <table><tr>
	      <td><label class="text_gray"><script>document.write(gettext("Port"));</script></label></td>
	      <td><input class="box" id="port" name="port" type="text" size="10" value="<%getPort();%>"/></td>
	    </tr></table>
	    </div>

	    <!-- Button start -->
	    <div class="submitBg">
	      <input type="submit" value="Apply" id="apply" class="submit" onclick="return submitActionTR69(this);"/>
	      <input type="reset" value="Discard" id="discard" class="submit" onclick="doRedirect();"/>
	    </div>
	    <!-- Button end -->
  		</form>
</div><!-- secBg end -->
</div><!-- contentBg end -->
</div><!-- all end -->
<script type="text/javascript">
	document.getElementById('apply').value=gettext("Apply");
	document.getElementById('discard').value=gettext("Discard");
</script>
</body>
</html>
