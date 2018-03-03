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
<script type="text/javascript" src="../js/j_volte.js"></script>

<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);

var nat_enable=<%getFwNatEnable();%>;
if(nat_enable!="1")
{
	window.location.href="../login.asp";
}

function checkApply()
{
	if(!CheckLoginInfo())
		return false;
	return true;
}

</script>

</head>


<body>
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftMenuChange("volte", "volte_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
<div class="secH1">VoLTE</div>
<!-- Section Begin -->
<div class="secBg">	

	<!-- get volte setting start -->
	<div id="get_volte_setting">
	<div class="secH2"><script>document.write(gettext("VoLTE Settings"));</script></div><br>

	<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr>
			<td>&nbsp;</td>
			<td>
				<input type="checkbox" id="enable_call_waiting" name="enable_call_waiting"/>      
			</td>
			<td></td>
			<td>&nbsp;&nbsp;<input type="button" id="set_volte_call_waiting" value="Apply" class="submit" /></td>			
		</tr>		
	</table>	
	<br>
	<br>
	<table border="0" cellpadding="0" cellspacing="0" class="configTbl">
		<tr>
			<td>&nbsp;</td>
			<td>
				<input type="checkbox" id="enable_inband" name="enable_inband"/>      
			</td>
			<td><script>document.write(gettext("Enable Inband"));</script></td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>
				<input type="checkbox" id="enable_plus_symbol" name="enable_plus_symbol"/>      
			</td>
			<td><script>document.write(gettext("Enable Plus Symbol"));</script></td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>
				<input type="checkbox" id="display_caller_number" name="display_caller_number"/>      
			</td>
			<td><script>document.write(gettext("Display Caller Number"));</script></td>
			<td>&nbsp;</td>
		</tr>		
		
	</table>
	
	<div class="submitBg">
		<input type="button" id="set_volte_setting" value="Apply" class="submit" />
		<input type="button" id="reset_volte_setting" value="Reset" class="submit" onclick="doRedirect();" />
	</div>
	</div>
	<!-- get volte setting end -->

	<br><br>

	<!-- digit mapping start -->
	<div id="get_volte_digit_setting" style="display:none">
	<div class="secH2">Digit Mapping</div>
	<table border="0" cellpadding="0" cellspacing="0" width="150">
	<tr>
	  <td class="tdH">0</td>
	  <td class="tdEven"><input type="text" name="digit_0" id="digit_0" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">1</td>
	  <td class="tdEven"><input type="text" name="digit_1" id="digit_1" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">2</td>
	  <td class="tdEven"><input type="text" name="digit_2" id="digit_2" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">3</td>
	  <td class="tdEven"><input type="text" name="digit_3" id="digit_3" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">4</td>
	  <td class="tdEven"><input type="text" name="digit_4" id="digit_4" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">5</td>
	  <td class="tdEven"><input type="text" name="digit_5" id="digit_5" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">6</td>
	  <td class="tdEven"><input type="text" name="digit_6" id="digit_6" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">7</td>
	  <td class="tdEven"><input type="text" name="digit_7" id="digit_7" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">8</td>
	  <td class="tdEven"><input type="text" name="digit_8" id="digit_8" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">9</td>
	  <td class="tdEven"><input type="text" name="digit_9" id="digit_9" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">*</td>
	  <td class="tdEven"><input type="text" name="digit_star" id="digit_star" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdEven">#</td>
	  <td class="tdEven"><input type="text" name="digit_hash" id="digit_hash" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	<tr>
	  <td class="tdH">+</td>
	  <td class="tdEven"><input type="text" name="digit_plus" id="digit_plus" class="configF1" size="12"  maxlength="1" ></td>
	</tr>
	</table>
	
	<div class="submitBg">
		<input type="button" id="set_volte_digit_mapping" value="Apply" class="submit" />
	</div>	
	</div>
	<!-- digit mapping end -->

	<br><br>
	
	<!-- IMS registration status start -->
	<div id="get_ims_status">
	<div class="secH2"><script>document.write(gettext("IMS Registration Status"));</script></div>
	<table border="0" cellpadding="0" cellspacing="0" width="654">
	<!-- ims register state -->
	<tr>
	  <td class="tdH"><script>document.write(gettext("Registration State"));</script></td>
	  <td class="tdEven" id="ims_reg_state"></td>
	</tr>

	<!-- sms service -->
	<tr>
	  <td class="tdH"><script>document.write(gettext("SMS Service Status"));</script></td>
	  <td class="tdEven" id="sms_service_status"></td>
	</tr>
	<tr>
	  <td class="tdH"><script>document.write(gettext("SMS Connection Type"));</script></td>
	  <td class="tdEven" id="sms_service_rat"></td>
	</tr>

	<!-- voip service -->
	<tr>
	  <td class="tdH"><script>document.write(gettext("VOIP Service Status"));</script></td>
	  <td class="tdEven" id="voip_service_status"></td>
	</tr>
	<tr>
	  <td class="tdH"><script>document.write(gettext("VOIP Connection Type"));</script></td>
	  <td class="tdEven" id="voip_service_rat"></td>
	</tr>	

	<!-- vt service -->
	<tr>
	  <td class="tdH"><script>document.write(gettext("VT Service Status"));</script></td>
	  <td class="tdEven" id="vt_service_status"></td>
	</tr>
	<tr>
	  <td class="tdH"><script>document.write(gettext("VT Connection Type"));</script></td>
	  <td class="tdEven" id="vt_service_rat"></td>
	</tr>	

	<!-- ut service -->
	<tr>
	  <td class="tdH"><script>document.write(gettext("UT Service Status"));</script></td>
	  <td class="tdEven" id="ut_service_status"></td>
	</tr>
	<tr>
	  <td class="tdH"><script>document.write(gettext("UT Connection Type"));</script></td>
	  <td class="tdEven" id="ut_service_rat"></td>
	</tr>

	</table>
	<br /><br />		
	</div>
	<!-- IMS registration status end -->
</div>

<!-- Section End -->
</div>
</div>
<script type="text/javascript">
	document.getElementById('set_volte_call_waiting').value=gettext("Apply");
	document.getElementById('set_volte_setting').value=gettext("Apply");
	document.getElementById('reset_volte_setting').value=gettext("Reset");
	document.getElementById('set_volte_digit_mapping').value=gettext("Apply");
</script>
</body>
</html>
