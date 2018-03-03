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
<script type="text/javascript" src="../js/j_ussd.js"></script>
    
<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);

var nat_enable=<%getFwNatEnable();%>;
if(nat_enable!="1")
{
	window.location.href="../login.asp";
} 
function getPINPUKCfg()
{
  var PinCfg = <%getPinConfig();%>;
  var pinArr = PinCfg.split("#");
  document.getElementById("PIN_VAR").value = pinArr[0];    
  if (pinArr[0] == "1") {          //PIN not verified
      //document.getElementById("srclte").setAttribute("src", "../images/menu/top_menu_internet_disabled.png");
      document.getElementById("hreflte").removeAttribute("href");
  }else{
      //document.getElementById("srclte").setAttribute("src", "../images/menu/top_menu_internet.png");
      document.getElementById("hreflte").href="../internet/internet_radio.asp";
  }  
  return;
}
</script>

</head>

<body>
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<input type="hidden" id="PIN_VAR">
<div id="all">
<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftMenuChange("ussd", "ussd_href");</script>
<!-- Main Menu and Submenu End -->


<!-- Right Content start -->
<div class="contentBg">

  <div class="secH1"><script>document.write(gettext("USSD"));</script></div>
  <div class="secBg">

  <div id="get_ussd_info">  <!-- get_ussd_info start -->
  <div>
    <h3><script>document.write(gettext("USSD Code"));</script></h3>    
    <input type="text" class="box" maxlength="100" id="ussdCode" name="ussdCode" style="width:400px; margin-left:25px;" onKeypress="if (event.keyCode == 13) { return false; }"/>	
  </div>

	<div id="ussdWaitStatus"> &nbsp; </div>
	<div id="ussdProgress"> &nbsp; </div>
		
	<div id="response_result"></div>

  </div> <!-- get_ussd_info end -->

   
	<!-- Button start -->
	<div class="submitBg">
		<input type="button" value="Send"  class="submit" id="ussdSendBtn" onclick="ussdValidation()" />
		<input type="button" value="Stop"  class="submit" id="ussdStopBtn"  onclick="ussd_stop()" style="display:none;" />
		<input type="button" value="Cancel"  class="submit" id="btnClose" onclick="doRedirect();" />
	</div>
	<!-- Button end -->    
  
  </div><!-- secBg end -->

</div><!-- contentBg end -->

</div> <!-- all end -->

<!-- Apply Progress -->
<!--
<div id="progressMask" class="progressMask"></div>
<div id="progressImg" class="progressImg" >
  <img src="/images/common/progress.gif" alt="Loading..." />
</div>
-->


</body>
<script type="text/javascript">

document.getElementById('ussdSendBtn').value=gettext("Send");	
document.getElementById('ussdStopBtn').value=gettext("Stop");
document.getElementById('btnClose').value=gettext("Cancel");
</script>
</html>


