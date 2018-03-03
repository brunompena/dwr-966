<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
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
<script language="JavaScript" src="../js/ipv4AddrValidations.js" type="text/javascript"></script>
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
function PortForwardingAction(state)
{
	if(!CheckLoginInfo())
		return false;
	document.getElementById('port_rowid').value="";
	var editor_action = document.getElementById('port_action');

	if (state=="add")
	{
		editor_action.value = "add";
	}
	else if (state=="edit")
	{

		var index_flag = editAllow('tblPortMapping');
		
		if(index_flag == false)
		{
			return false;
		}
		else		
		{
			editor_action.value = "edit";
			var port_forward_check = document.getElementsByName('port_forward_ck');
			var index="";
			if(port_forward_check.length>0)
			{
				for(var i=0;i<port_forward_check.length;i++)
				{
					if(port_forward_check[i].checked)
					{
						index=i;
						break;
					}
				}
			}
			document.getElementById('port_rowid').value=(index+1);
		}
	}
	else if (state=="delete")
	{
		if(deleteAllow1 ('tblPortMapping'))
		{
			editor_action.value = "delete";
			var port_forward_check = document.getElementsByName('port_forward_ck');
			var index="";
			var count = 0;
			if(port_forward_check.length>0)
			{
				for(var i=0;i<port_forward_check.length;i++)
				{
					if(port_forward_check[i].checked)
					{
						count++;
						if (count == 1)
							index=(i+1);
						else
							index=index+","+(i+1);
					}
				}
			}
			document.getElementById('port_rowid').value=index;
		}
		else
		{
			return false;
		}
	}
	return true;
}

function pageLoad()
{
	var portforwarding_status=<%getPortFwdStatus();%>;
	if(portforwarding_status == "1")
	{
		alert(gettext("The Service name and IP address already exist."));
	}
}
</script>
</head>

<body onload="pageLoad();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftMenuChange("nat_portforwarding", "nat_portforwarding_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg" id="main_forwarding">
	<div class="secH1"><script>document.write(gettext("Port Forwarding"));</script></div>
	<!-- Section Begin -->
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo">
			<br><script>document.write(gettext("Port Forwarding can be used to translate the port of common services to a custom port inside your local network. This page displays all the port forwarding rules configured on the gateway."));</script>
			<br>
		</div>
		<div class="secH2"><script>document.write(gettext("List of Port Forwarding Services"));</script></div>
		<form name="port_fwd_del" action="/goform/setPortFwdAction?<%getUniName();%>=<%getUniId();%>" method="post">
		<input type="hidden" id="port_rowid" name="port_rowid" value="0">
		<input type="hidden" id="port_action" name="port_action" value="0">
		<table border="0" cellpadding="0" cellspacing="0" id="tblPortMapping" width="550px" style="table-layout: fixed;" class="specTbl">
			<tr>
				<td class="tdH" width="10%"><input type="checkbox" id="imgSelectAll" title="Select All" onclick="secChkBoxSelectOrUnselectAll ('tblPortMapping', 'umiId', this)"></td>
				<td class="tdH" width="45%"><script>document.write(gettext("Service"));</script></td>
				<td class="tdH" width="25%"><script>document.write(gettext("IP Address"));</script></td>
				<td class="tdH" width="20%"><script>document.write(gettext("Port Translation"));</script></td>
			</tr>
			<tbody>
      			<%getPortFwdList();%>
    		</tbody>
		</table>								
		<div>
			<input type="submit" id="port_add" value="Add" class="tblbtn" title="Add" name="" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return PortForwardingAction('add');">
			<input type="submit" id="port_edit" value="Edit" class="tblbtn" title="Edit" name="" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return PortForwardingAction('edit');">
			<input type="submit" id="port_delete" value="Delete" class="tblbtn" title="Delete" name="" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return PortForwardingAction('delete');">
		</div>
		</form>									
	</div>
	<!-- Section End -->
</div>
</div>
<script type="text/javascript">
	document.getElementById('port_add').value=gettext("Add");
	document.getElementById('port_edit').value=gettext("Edit");
	document.getElementById('port_delete').value=gettext("Delete");
</script>
</body>
</html>
