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

function getAntennaMode()
{
 var antennaMode = <%getAntennaMode();%>;
 if (antennaMode == "0")
 {
   document.getElementById("antenna_int").checked=true;
 } else if (antennaMode == "1") {
   document.getElementById("antenna_ext").checked=true;
 }
 document.getElementById('org_antenna').value = antennaMode;
	return;
}

function getRadioMode()
{
 var radioMode = <%getLTERadio();%>;
 var radioArr = radioMode.split("#");

 switch (radioArr[0])
 {
  case 'auto':
   document.getElementById("selLteRadioMode").selectedIndex = 0;
   break;
  case '2G':
   document.getElementById("selLteRadioMode").selectedIndex = 1;
   break;
  case '3G':
   document.getElementById("selLteRadioMode").selectedIndex = 2;
   break;
  case '4G':
   document.getElementById("selLteRadioMode").selectedIndex = 3;
   break;
 }
	switch (radioArr[1])
	{
  case 'auto':
   document.getElementById("selLteRadioFrequency").selectedIndex = 0;
   break;
  case '1800':
   document.getElementById("selLteRadioFrequency").selectedIndex = 1;
   break;
  case '2600':
   document.getElementById("selLteRadioFrequency").selectedIndex = 2;
   break;
  case '900':
   document.getElementById("selLteRadioFrequency").selectedIndex = 3;
   break;
  case '800':
   document.getElementById("selLteRadioFrequency").selectedIndex = 4;
   break;
 }
 document.getElementById('org_radio_mode').value = radioArr[0];
 document.getElementById('org_radio_frequency').value = radioArr[1];
 var roamingArr = radioArr[2].split("=");
 if(roamingArr[1] == "0")
	document.getElementById('lte_roaming').checked = false;
 else
 	document.getElementById('lte_roaming').checked = true;
 return;
}

function chkLteRadioMode()
{
	 var radio_mode = document.getElementById('selLteRadioMode').value;
  if (radio_mode == "3G" || radio_mode == "2G")
    fieldStateChangeWr ('selLteRadioFrequency','','','');
  else
	   fieldStateChangeWr ('','','selLteRadioFrequency','');
  return;
}

function checkApply()
{
	if(!CheckLoginInfo())
		return false;
	var radioMode = <%getLTERadio();%>;
 	var radioArr = radioMode.split("#");
	var roamingArr = radioArr[2].split("=");
	 
	if(roamingArr[1] == "0"){
		if(document.getElementById("lte_roaming").checked == true)
		{
			var confirm_select;
			confirm_select = window.confirm(gettext("You have selected \"Allow Data Roaming\". This will incur additional roaming charges."));
			if(!confirm_select)
				return false;
		}
 	}
	
	return true;
}
</script>
</head>

<body onload="getRadioMode();getAntennaMode();chkLteRadioMode();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("lte");%>
<script type="text/javascript">menuChange("lte_menu");leftMenuChange("internet_radio", "internet_radio_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Radio Configuration"));</script></div>
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo">
		<br /><script>document.write(gettext("This page will provide you to set LTE related configurations."));</script>
		<br />
		</div>
		<form name="RADIO_CONF" action="/goform/setLTERadio?<%getUniName();%>=<%getUniId();%>" method="post">
  <input type="hidden" id="org_radio_mode" name="org_radio_mode" value="auto" />
  <input type="hidden" id="org_radio_frequency" name="org_radio_frequency" value="auto" />
  <input type="hidden" id="org_antenna" name="org_antenna" value="1" />
		<table cellspacing="0" class="configTbl">
		<tr>
		 <td width="150"><script>document.write(gettext("Allow Data Roaming"));</script></td>
		 <td><input name="lte_roaming" id="lte_roaming" type="checkbox"/></td>
		</tr>
			<tr>
				<td width="150"><script>document.write(gettext("Radio Mode"));</script></td>
				<td>
					<select class="configF1" name="lte_radioMode" id="selLteRadioMode" onclick="return chkLteRadioMode();">
      <option value="auto"><script>document.write(gettext("LTE Preferred"));</script></option>
      <option value="2G"><script>document.write(gettext("Only 2G"));</script></option>
      <option value="3G"><script>document.write(gettext("Only 3G"));</script></option>
      <option value="4G"><script>document.write(gettext("Only LTE"));</script></option>
					</select>
				</td>
			</tr>
			<tr>
				<td width="150"><script>document.write(gettext("Radio Frequency"));</script></td>
				<td>
      <select class="configF1" name="lte_radioFrequency" id="selLteRadioFrequency">
       <option value="auto"><script>document.write(gettext("Auto"));</script></option>
       <option value="1800"><script>document.write(gettext("Band 3"));</script></option>
       <option value="2600"><script>document.write(gettext("Band 7"));</script></option>
       <option value="900"><script>document.write(gettext("Band 8"));</script></option>
       <option value="800"><script>document.write(gettext("Band 20"));</script></option>
      </select>
     </td>
			</tr>
			<tr>
				<td width="150"><script>document.write(gettext("Antenna"));</script></td>
    <td>
      <input type="radio" name="antenna_mode" id="antenna_ext" value="1">&nbsp;<script>document.write(gettext("External"));</script>
      &nbsp;&nbsp;&nbsp;
      <input type="radio" name="antenna_mode" id="antenna_int" value="0">&nbsp;<script>document.write(gettext("Internal"));</script>
    </td>
			</tr>
		</table>
		<div>
			<input type="submit" value="Apply" onclick="return checkApply();" class="submit" id="button.apply" title="Apply" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" />
		</div>
		</form>
	</div>
</div>
</div>
<script type="text/javascript">
 document.getElementById('button.apply').value = gettext("Apply");
</script>
</body>
</html>
