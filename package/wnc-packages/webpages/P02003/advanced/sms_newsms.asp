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
<script type="text/javascript" src="../js/j_sms.js"></script>
    
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
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("sms_submenu","sms_submenu_focus","sms_newsms","sms_newsms_href");</script>
<!-- Main Menu and Submenu End -->


<!-- Right Content start -->
<div class="contentBg">

  <div class="secH1"><script>document.write(gettext("New SMS"));</script></div>
  <div class="secBg">

  <div id="get_new_sms_info">  <!-- get_new_sms_info start -->
  <div>
    <h3><script>document.write(gettext("Receiver"));</script></h3>    
    <input type="text" class="box" maxlength="630" id="new_sms_send_to" name="new_sms_send_to" style="width:600px; margin-left:25px;"/>	
  </div>

  <div>
    <h3><script>document.write(gettext("Content"));</script></h3>    
    <textarea class="box" 
    	id="new_sms_content" 
    	name="new_sms_content" 
    	maxLength="1000" 
    	style="width:600px; height:100px; margin-left:25px; resize:none;"
    	onkeyup="textarea_maxlen_isMax('new_sms_content')" 
		onfocus="textarea_maxlen_disabledRightMouse()" 
		onblur="textarea_maxlen_enabledRightMouse()" 
		onkeypress="return checkMaxLength('new_sms_content',1000)"		
    >
    </textarea>			  
  </div>
  </div> <!-- get_new_sms_info end -->

   
	<!-- Button start -->
	<div class="submitBg">
		<input type="button" id="set_new_sms" value="Send" class="submit" />
		<input type="button" id="reset_new_sms" value="Discard" class="submit" onclick="doRedirect();" />
		<input type="button" id="save_new_sms" value="Save" class="submit" />		
	</div>
	<!-- Button end -->    
  
  </div><!-- secBg end -->

</div><!-- contentBg end -->

</div> <!-- all end -->

<!-- Apply Progress -->
<div id="progressMask" class="progressMask"></div>
<div id="progressImg" class="progressImg" >
  <img src="/images/common/progress.gif" alt="Loading..." />
</div>


</body>
<script type="text/javascript">

document.getElementById('set_new_sms').value=gettext("Send");	
document.getElementById('reset_new_sms').value=gettext("Discard");
document.getElementById('save_new_sms').value=gettext("Save");
</script>
</html>


