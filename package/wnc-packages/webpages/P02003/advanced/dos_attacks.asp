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
<script language="JavaScript" src="../js/textValidations.js" type="text/javascript"></script>
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
function initDosAttack()
{
  var DosAttackCfg = <%getDosAttackConfig();%>;
  var dosAttackArr = DosAttackCfg.split("#");

  //SYN Flood
  document.getElementById("syn_flood").value = dosAttackArr[0];
  //ECHO Storm
  document.getElementById("echo_storm").value = dosAttackArr[1];
  //ICMP Flood
  document.getElementById("icmp_flood").value = dosAttackArr[2];
  return;
}
function setFirewall()
{
	if(!CheckLoginInfo())
		return false;
  var syn_flood = document.DosAttack.syn_flood.value;
  var echo_storm = document.DosAttack.echo_storm.value;
  var icmp_flood = document.DosAttack.icmp_flood.value;
  var flood_rule=/^[0-9]*$/;
  if(syn_flood == "")
  {
  	alert(gettext("The SYN Flood can not be empty."));
	document.DosAttack.syn_flood.focus();
	return false;
  }
  if(echo_storm == "")
  {
  	alert(gettext("The Echo Storm can not be empty."));
	document.DosAttack.echo_storm.focus();
	return false;
  }
  if(icmp_flood == "")
  {
  	alert(gettext("The ICMP Flood can not be empty."));
	document.DosAttack.icmp_flood.focus();
	return false;
  }  
  if(!flood_rule.test(syn_flood))
  {
  	alert(gettext("The SYN Flood should be number."));
  	document.DosAttack.syn_flood.focus();
  	return false;
  }
  if(!flood_rule.test(echo_storm))
  {
  	alert(gettext("The Echo Storm should be number."));
  	document.DosAttack.echo_storm.focus();
  	return false;
  }
  if(!flood_rule.test(icmp_flood))
  {
  	alert(gettext("The ICMP Flood should be number."));
  	document.DosAttack.icmp_flood.focus();
  	return false;
  }  
  syn_flood = parseInt(syn_flood, 10);	
  echo_storm = parseInt(echo_storm, 10);
  icmp_flood = parseInt(icmp_flood, 10);
  
  if((syn_flood<=0)||(syn_flood>10000))
  {
  	alert(gettext("The SYN Flood should be between 1 and 10000."));
  	document.DosAttack.syn_flood.focus();
  	return false;
  }
  if((echo_storm<=0)||(echo_storm>10000))
  {
  	alert(gettext("The Echo Storm should be between 1 and 10000."));
  	document.DosAttack.echo_storm.focus();
  	return false;
  }
  if((icmp_flood<=0)||(icmp_flood>10000))
  {
  	alert(gettext("The ICMP Flood should be between 1 and 10000."));
  	document.DosAttack.icmp_flood.focus();
  	return false;
  }    
  return true;	
}
</script>
</head>	

<body onload="initDosAttack();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("firewall_submenu","firewall_submenu_focus","dos_attacks","dos_attacks_href");</script>
<!-- Main Menu and Submenu End -->

<!-- Right Content start -->
<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("DoS Attacks"));</script></div>
	<form name="DosAttack" method="post" action="/goform/setDosAttackConfig?<%getUniName();%>=<%getUniId();%>">	
	<!-- Section Begin -->
	<div class="secBg">
		<div class="statusMsg"><%getActionResult();%></div>			
		<div class="secInfo">
			<br><script>document.write(gettext("A denial-of-service attack (DoS attack) is an attempt to make a device resource unavailable to its intended users."));</script>
			<br>
		</div>
		<table cellspacing="0" class="configTbl">
			<tr>
				<td><script>document.write(gettext("SYN Flood"));</script></td>
				<td><input type="text" name="syn_flood" class="configF1" value="" id="syn_flood" size="12"  maxlength="5" onkeypress="return numericValueCheck (event)" onkeydown="if (event.keyCode == 9) {return numericValueRangeCheck (this, '', '', 1, 10000, true, gettext('Invalid SYN Flood Detect Rate:'), '');}"><span class="spanText"><script>document.write(gettext("[Max/Sec] (1-10000)"));</script></span></td>
				<td>&nbsp;</td>
			</tr>					
			<tr>
				<td><script>document.write(gettext("Echo Storm"));</script></td>
				<td><input type="text" name="echo_storm" class="configF1" value="" id="echo_storm" size="12"  maxlength="5" onkeypress="return numericValueCheck (event)" onkeydown="if (event.keyCode == 9) {return numericValueRangeCheck (this, '', '', 1, 10000, true, gettext('Invalid Echo Storm:'), '');}"><span class="spanText"><script>document.write(gettext("[Ping Pkts/Sec] (1-10000)"));</script></span></td>
				<td>&nbsp;</td>
			</tr>			
			<tr>
				<td><script>document.write(gettext("ICMP Flood"));</script></td>
				<td ><input type="text" name="icmp_flood" class="configF1" value="" id="icmp_flood" size="12"  maxlength="5" onkeypress="return numericValueCheck (event)" onkeydown="if (event.keyCode == 9) {return numericValueRangeCheck (this, '', '', 1, 10000, true, gettext('Invalid ICMP Flood:'), '');}"><span class="spanText"><script>document.write(gettext("[ICMP Pkts/Sec] (1-10000)"));</script></span></td>
				<td>&nbsp;</td>
			</tr>			
		</table>
		<div>
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td class="blank1" height="26">&nbsp;</td>
				</tr>
			</table>
		</div>
		<div class="submitBg">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td><input type="submit" id="dos_apply" value="Apply" class="submit" name="button.config.dosAttacks" title="Apply" onclick="return setFirewall();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
					<td><input type="button" id="dos_reset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
				</tr>
			</table>
		</div>
	</div>
	</form>
</div>	
</div> <!-- end of all -->
<script type="text/javascript">
	document.getElementById('dos_apply').value=gettext("Apply");
	document.getElementById('dos_reset').value=gettext("Reset");
</script>
</body>
</html>
