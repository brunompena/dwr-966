<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="JavaScript" src="../js/textValidations.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/jquery-1.4.2.min.js"></script>
<script type="text/javascript" src="../js/jquery.json-2.2.min.js"></script>
<script type="text/javascript" src="../js/j_common.js"></script>

<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);
</script>
</head>	

<body>
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("ipv6_submenu","ipv6_submenu_focus","ipv6_clientlist","ipv6_clientlist_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
<div class="secH1"><script>document.write(gettext("IPv6 Device List"));</script></div>
<!-- Section Begin -->
<div class="secBg">
	<div class="secInfo"><br><script>document.write(gettext("Please press [Update] to obtain the device connection list."));</script><br></div>
	<form method="post" action="/goform/updateIPv6ClientList?<%getUniName();%>=<%getUniId();%>">
		<table border="0" cellpadding="0" cellspacing="0" width="600px" style="table-layout: fixed;" class="specTbl">
			<tr>
				<td class="tdH" width="25%"><script>document.write(gettext("Host Name (if any)"));</script></td>
				<td class="tdH" width="50%"><script>document.write(gettext("IP Address"));</script></td>
				<td class="tdH" width="25%"><script>document.write(gettext("MAC Address"));</script></td>
			</tr>
			<%getIPv6Clients();%>
		</table>	
		</br>
		<div class="submitBg">
		<table border="0" cellpadding="0" cellspacing="0">
		  <tr><td><input type="submit" class="tblbtn" value="Update" title="Update" id="update" name="update" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'"></td></tr>
		</table>
		</div>
	</form>
</div>
<!-- Section End -->
</div>
</div>
<script type="text/javascript">
  document.getElementById('update').value = gettext("Update");
</script>
</body>
</html>
