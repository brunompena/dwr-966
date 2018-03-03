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

function loadIPv6StaticRoutingPage()
{
	secChkBoxSelectOrUnselectAll ('tblIPv6RoutingList',null);
	
	var ipv6_set = <%getConf("IPV6_ENABLE");%>;
	
	if (ipv6_set != "on") {		//IPv6 Off
		document.getElementById("StatusMsg").innerHTML = gettext("IPv6 is off. Please enable IPv6 first!");
		document.getElementById("add").disabled=true;
		document.getElementById("add").className="tblbtn_dis";
		document.getElementById("edit").disabled=true;
		document.getElementById("edit").className="tblbtn_dis";
		document.getElementById("delete").disabled=true;
		document.getElementById("delete").className="tblbtn_dis";		
	} else {					//IPv6 On
		document.getElementById("add").disabled=false;
		document.getElementById("add").className="tblbtn";
		document.getElementById("edit").disabled=false;
		document.getElementById("edit").className="tblbtn";
		document.getElementById("delete").disabled=false;
		document.getElementById("delete").className="tblbtn";
	}
}

function ipv6_routing_action(action)
{
	if(!CheckLoginInfo())
		return false;
	var ipv6_routing_list_idx = -1;
	var ipv6_routing_arr="";
	var editor_action = document.getElementById('IPv6RoutingAction');
	var editor_count = document.getElementById('IPv6RoutingCount');
	var secObj = document.getElementById('tblIPv6RoutingList');
	var objArr = secObj.getElementsByTagName("INPUT");
	var count=0;
	
	for (var i=0; i < objArr.length; i++) {
		if (objArr[i].type == 'checkbox' && !objArr[i].disabled && objArr[i].name!="imgSelectAllChk") {
			if (objArr[i].checked==true) {
				count++;
				ipv6_routing_list_idx=i; //host_list_idx starts from 1	
				ipv6_routing_arr+=ipv6_routing_list_idx+",";
			}
		}
	}
	
	editor_count.value=count;
	var ret=true;
	if (action=="add") {
		editor_action.value = "add";			
	} else if (action=="edit") {
		if (count > 1) {
			alert(gettext("Please select a row to edit."));
			ret=false;
		} else if (count==0) {
			alert(gettext("Please select a row from the list to be edited."));
			ret=false;
		} else {
			editor_action.value = "edit";			
			document.getElementById('IPv6RoutingIdx').value=ipv6_routing_list_idx;								
		}
	} else if (action=="delete") {
		if (count==0) {
			alert(gettext("Please select items from the list to be deleted."));
			ret=false;
		} else {
			editor_action.value = "del";		
			ipv6_routing_arr=ipv6_routing_arr.substr(0,ipv6_routing_arr.length-1);
			document.getElementById('IPv6RoutingIdx').value=ipv6_routing_arr;						
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

<body onload="loadIPv6StaticRoutingPage()">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("ipv6_submenu","ipv6_submenu_focus","ipv6_routing","ipv6_routing_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
<div class="secH1"><script>document.write(gettext("IPv6 Static Routing"));</script></div>
<!-- Section Begin -->
<div class="secBg">
	<div class="secInfo">
		<br><script>document.write(gettext("Static routing allows network administrators to manually determine a packet delivery path by defining the destination network range and the gateway that the packets must pass through."));</script><br>
		<table cellspacing="0" class="configTbl">
			<tr align="center"><td class="msg" id="StatusMsg"></td></tr>
		</table>
	</div>
  	<div class="secH2"><script>document.write(gettext("Current IPV6 static routing in system"));</script></div>
	<form method="post" action="/goform/setIPv6StaticRoutingTable?<%getUniName();%>=<%getUniId();%>">
		<input type="hidden" id="IPv6RoutingAction" name="IPv6RoutingAction" value="">
  		<input type="hidden" id="IPv6RoutingIdx" name="IPv6RoutingIdx" value="">
  		<input type="hidden" id="IPv6RoutingCount" name="IPv6RoutingCount" value="">
		<table border="0" cellpadding="0" cellspacing="0" id="tblIPv6RoutingList" width="720px" style="table-layout: fixed;" class="specTbl">
			<tr>
				<td class="tdH" width="5%"><input type="checkbox" name="imgSelectAllChk" id="imgSelectAll" title="Select All" onclick="secChkBoxSelectOrUnselectAll ('tblIPv6RoutingList', this)"></td>
				<td class="tdH" width="10%"><script>document.write(gettext("Name"));</script></td>
				<td class="tdH" width="34%"><script>document.write(gettext("Destination IP"));</script></td>
				<td class="tdH" width="8%"><script>document.write(gettext("Prefix"));</script></td>
				<td class="tdH" width="10%"><script>document.write(gettext("Network"));</script></td>
				<td class="tdH" width="33%"><script>document.write(gettext("Gateway"));</script></td>
			</tr>
			<tbody><%getIPv6StaticRoutingTable();%></tbody>					
		</table>
		<div id="tblButtons">
			<input type="submit" value="Add" class="tblbtn" name="add" id="add" title="Add" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6_routing_action('add');">
			<input type="submit" value="Edit" class="tblbtn" name="edit" id="edit" title="Edit" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6_routing_action('edit');">
			<input type="submit" value="Delete" class="tblbtn" name="delete" id="delete" title="Delete" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return ipv6_routing_action('delete');">
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
