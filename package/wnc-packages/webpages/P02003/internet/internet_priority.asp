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

function initPriorityInfo()
{
	var priority_status = <%getInternetPriority();%>;
	document.getElementById("internet_priority_status").selectedIndex=priority_status; 
}

function submitPriority()
{
	if(!CheckLoginInfo())
	return false;

	document.internet_priority.action="/goform/setInternetPriority?<%getUniName();%>=<%getUniId();%>";
	document.internet_priority.submit();
    return true;
}

</script>
</head>

<body onload="initPriorityInfo();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("lte");%>
<script type="text/javascript">menuChange("lte_menu");leftMenuChange("internet_priority", "internet_priority_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("WAN Priority"));</script></div>
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo"><br /><script>document.write(gettext("This page is for you to select WAN priority."));</script><br /></div>
  		<form name="internet_priority" method="post">
  			<input type="hidden" id="InternetStatus" name="InternetStatus" />
			<table cellspacing="0" class="configTbl">
   			<tr id="width2">
				<td><script>document.write(gettext("Internet Priority"));</script></td>
				<td><select class="configF1" name="internet_priority_status" id="internet_priority_status" size="1">
					<option value="0"><script>document.write(gettext("LTE LAN"));</script></option>
					<option value="1"><script>document.write(gettext("Ethernet WAN"));</script></option>
				</select></td>      
			</tr>
   			</table>
   			<div>
	  			<input type="submit" id="apply" value="Apply" class="tblbtn" title="Apply" name="" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return submitPriority();">
	  			<input type="submit" id="reset" value="Reset" class="tblbtn" title="Reset" name="" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="doRedirect();">
			</div>
	 	</form>
	</div>
</div>
</div>
<script type="text/javascript">
  document.getElementById('apply').value = gettext("Apply");
  document.getElementById('reset').value = gettext("Reset");
</script>
</body>
</html>
