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

function loadIPv6FirewallPage()
{
	secChkBoxSelectOrUnselectAll ('tblIPv6Firewall',null);
	
	var ipv6_set = <%getConf("IPV6_ENABLE");%>;
	
	if (ipv6_set != "on") {		//IPv6 Off
		document.getElementById("StatusMsg").innerHTML = gettext("IPv6 is off. Please enable IPv6 first!");
		document.getElementById("add").disabled=true;
		document.getElementById("add").className="tblbtn_dis";
		document.getElementById("edit").disabled=true;
		document.getElementById("edit").className="tblbtn_dis";
		document.getElementById("delete").disabled=true;
		document.getElementById("delete").className="tblbtn_dis";
	}else{						//IPv6 On
		document.getElementById("add").disabled=false;
		document.getElementById("add").className="tblbtn";
		document.getElementById("edit").disabled=false;
		document.getElementById("edit").className="tblbtn";
		document.getElementById("delete").disabled=false;
		document.getElementById("delete").className="tblbtn";	
	}
}


function ipv6_firewall_list_action(action)
{	
	if(!CheckLoginInfo())
		return false;

	var ipv6_firewall_list_idx = -1;
	var ipv6_firewall_list_arr = "";
	
	var editor_action = document.getElementById('IPv6FirewallTblAction');
	var editor_count = document.getElementById('IPv6FirewallTblCount');
	var secObj = document.getElementById('tblIPv6Firewall');
	var objArr = secObj.getElementsByTagName("INPUT");
	var count=0;
	for (var i=0; i < objArr.length; i++) {
		if (objArr[i].type == 'checkbox' && !objArr[i].disabled && objArr[i].name!="imgSelectAllChk"){
			if (objArr[i].checked==true){
				count++;
				ipv6_firewall_list_idx=i; 	
				ipv6_firewall_list_arr+=ipv6_firewall_list_idx+",";
			}
		}
	}
	editor_count.value=count;
	var ret=true;
	if (action=="add"){
		editor_action.value = "add";			
	} else if (action=="edit") {
		if (count > 1){
			alert(gettext("Please select a row to edit."));
			ret=false;
		} else if (count==0) {
			alert(gettext("Please select a row from the list to be edited."));
			ret=false;
		} else {
			editor_action.value = "edit";			
			document.getElementById('IPv6FirewallTblIdx').value=ipv6_firewall_list_idx;
		}
	} else if (action=="delete") {
		if (count==0) {
			alert(gettext("Please select items from the list to be deleted."));
			ret=false;
		} else {
			editor_action.value = "del";		
			ipv6_firewall_list_arr=ipv6_firewall_list_arr.substr(0,ipv6_firewall_list_arr.length-1);
			document.getElementById('IPv6FirewallTblIdx').value=ipv6_firewall_list_arr;
		}
	}
	return ret;
}

function secChkBoxSelectOrUnselectAll(sectionId, chkObj)
{
	if (!sectionId) return;
	secObj = document.getElementById(sectionId);
	if (!secObj) return;
	objArr = secObj.getElementsByTagName("INPUT");
	if (!objArr) return;
	if (chkObj)
	{
		for (i=0; i < objArr.length; i++)
		{
			if (objArr[i].type == 'checkbox' && !objArr[i].disabled)
			{
				if (chkObj.id == "imgSelectAll")
					objArr[i].checked = true;
				else if (chkObj.id == "imgUnCheckAll")
					objArr[i].checked = false;
			}
		}
		/* Change icon */		
		if (chkObj.id == "imgSelectAll")
		{
			chkObj.id = "imgUnCheckAll";
			chkObj.title = "Deselect All";
		}
		else if (chkObj.id == "imgUnCheckAll")
		{
			chkObj.id = "imgSelectAll"
			chkObj.title = "Select All"
		}		
	}
	else
	{
		for (i=0; i < objArr.length; i++)
		{
			if (objArr[i].type == 'checkbox' && !objArr[i].disabled)
			{
				objArr[i].checked = false;
			}
		}	
	}		
	return; 
}
</script>
</head>	

<body onload="loadIPv6FirewallPage();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("ipv6_submenu","ipv6_submenu_focus","ipv6_filtering","ipv6_filtering_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
<div class="secH1"><script>document.write(gettext("IPv6 Firewall"));</script></div>
<!-- Section Begin -->
<div class="secBg">
	<div class="statusMsg"><%getActionResult();%></div>
	<div class="secInfo">
		<br><script>document.write(gettext("A list of all configured filtering rules is displayed here. Filtering rules for computers indicate whether a defined application is specifically allowed or blocked for use for that computer."));</script><br>
		<table cellspacing="0" class="configTbl">
			<tr align="center"><td class="msg" id="StatusMsg"></td></tr>
		</table>
	</div>
	<div class="secH2"><script>document.write(gettext("Firewall Rule List"));</script></div>
	<form method="post" action="/goform/setIPv6FirewallTable?<%getUniName();%>=<%getUniId();%>">
		<input type="hidden" id="IPv6FirewallTblAction" name="IPv6FirewallTblAction" value="">
		<input type="hidden" id="IPv6FirewallTblIdx" name="IPv6FirewallTblIdx" value="">		
		<input type="hidden" id="IPv6FirewallTblCount" name="IPv6FirewallTblCount" value="">
		<table border="0" cellpadding="0" cellspacing="0" id="tblIPv6Firewall" width="450px" style="table-layout: fixed;" class="specTbl">
			<tr>
				<td class="tdH" width="10%"><input type="checkbox" name="imgSelectAllChk" id="imgSelectAll" onclick="secChkBoxSelectOrUnselectAll ('tblIPv6Firewall', this)"></td>
				<td class="tdH" width="35%"><script>document.write(gettext("Application"));</script></td>
				<td class="tdH" width="35%"><script>document.write(gettext("Computer Name"));</script></td>
				<td class="tdH" width="20%"><script>document.write(gettext("Status"));</script></td>
			</tr>
			<tbody><%getIPv6FirewallTable();%></tbody> 
		</table>	
		</br>
		<div class="submitBg">
			<table border="0" cellpadding="0" cellspacing="0">
				<tr>
					<td><input type="submit" class="tblbtn" value="Add" title="Add" id="add" name="add" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6_firewall_list_action('add');"></td>
					<td><input type="submit" class="tblbtn" value="Edit" title="Edit" id="edit" name="edit" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6_firewall_list_action('edit');"></td>
					<td><input type="submit" class="tblbtn" value="Delete" title="Delete" id="delete" name="delete" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6_firewall_list_action('delete');"></td>
				</tr>
			</table>
		</div>
	</form>
</div>
<!-- Section End -->
</div>
</div>
<script type="text/javascript">
  document.getElementById('add').value = gettext("Add");
  document.getElementById('edit').value = gettext("Edit");
  document.getElementById('delete').value = gettext("Delete");
</script>
</body>
</html>
