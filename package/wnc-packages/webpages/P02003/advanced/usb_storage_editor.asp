<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<link rel="stylesheet" href="../style/dtree.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="JavaScript" src="../js/dtree.js" type="text/javascript"></script>
<script language="JavaScript" src="../js/textValidations.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/jquery-1.4.2.min.js"></script>
<script type="text/javascript" src="../js/jquery.json-2.2.min.js"></script>
<script type="text/javascript" src="../js/j_common.js"></script>

<script language="javascript" type="text/javascript">

CheckInitLoginInfo(<%getuser_login();%>);

function pageValidate()
{	
	if(!CheckLoginInfo())
		return false;
	if(isBlank(get_by_id("file_share_name").value))
	{
	    alert(gettext("Enter valid File Share Name"));
	    return false;
	}
	if(isSpace(get_by_id("file_share_name").value))
	{
	    alert(gettext("Enter valid File Share Name without whitespace"));
	    return false;
	}
	if(!isValidName(get_by_id("file_share_name").value) || isDecimal(get_by_id("file_share_name").value))
	{
        alert(gettext("File Share Name cannot have characters from the set \"<>%\^[].`+$,='#&:\t"));
        return false;
	}	
	if(isBlank(get_by_id("file_share_path").value))
	{
		alert(gettext("Please select a Folder"));
	    return false;
	}	
	if(isSpace(get_by_id("file_share_path").value))
	{
	    alert(gettext("Folder Path cannot have whitespace"));
	    return false;
	}	
	if(!isValidName(get_by_id("file_share_path").value) || isDecimal(get_by_id("file_share_path").value))
	{
        alert(gettext("Folder Path cannot have characters from the set \"<>%\^[].`+$,='#&:\t"));
        return false;
	}

	return true;
}
 
function pageLoad()
{	
	var SambaConfig = <%getSambaShareConfig();%>;
	var SambaConfigArr;
	if (SambaConfig!="")
	{
		SambaConfigArr = SambaConfig.split("#");
		get_by_id("file_share_name").value=SambaConfigArr[0];
		get_by_id("file_share_path").value=SambaConfigArr[1];
		get_by_id("file_share_access_level").value=SambaConfigArr[2];
		get_by_id("file_share_users").value=SambaConfigArr[3];
		get_by_id("samba_path").value="/mnt/usb"+SambaConfigArr[1];
	}
	else
	{
		//default value
		get_by_id("file_share_name").value="";
		get_by_id("file_share_path").value="";
		get_by_id("file_share_access_level").value="0";
		get_by_id("file_share_users").value="all";		
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
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("usb_submenu","usb_submenu_focus","usb_storage","usb_storage_href");</script>
<!-- Main Menu and Submenu End -->


<!-- Add usb storage -->
<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Add/Modify File Share"));</script></div>
	<!-- Section Begin -->
	<div class="secBg">
	<div class="secInfo">
		<br><script>document.write(gettext("In this section you can add/modify a sharing profile."));</script>
		<br>
		<a class="secLable1" href="usb_storage.asp">>> <script>document.write(gettext("Back to Usb Storage page"));</script></a>
	</div>
	<form name="SambaConfig" method="post" action="/goform/setSambaShareConfig?<%getUniName();%>=<%getUniId();%>">
	<input type="hidden" id="samba_path" name="samba_path" value="">
	<table cellspacing="0" class="configTbl">
		<tr>
			<td><script>document.write(gettext("Sharing Profile Name"));</script></td>
			<td><input type="text" id="file_share_name" name="file_share_name" class="txtbox" value="" size="20" maxlength="24"></td>
		</tr>
		<tr>
			<td><script>document.write(gettext("Folder Path"));</script></td>
			<td>
				<input type="text" id="file_share_path" name="file_share_path" class="txtbox" value="" size="20" maxlength="24" disabled=true></input>
			</td>
		</tr>			
		<tr>
			<td><script>document.write(gettext("Access Level"));</script></td>
			<td>
				<select size="1" class="configF1" name="file_share_access_level" id="file_share_access_level" onclick="" style="width:182px;">
					<option value="0"><script>document.write(gettext("Read-Only"));</script></option>
					<option value="1"><script>document.write(gettext("Read-Write"));</script></option>
				</select>
			</td>
		</tr>		
		<tr>
			<td><script>document.write(gettext("Users"));</script></td>
			<td>
				<select size="1" class="configF1" name="file_share_users" id="file_share_users" onclick="" style="width:182px;">
					<option value="all">all</option>
					<option value="admin">admin</option>
				</select>
			</td>
		</tr>			
	</table>	
	<!-- TreeView -->
	<div class="dtree">

		<p><a href="javascript: d.openAll();"><script>document.write(gettext("open all"));</script></a> | <a href="javascript: d.closeAll();"><script>document.write(gettext("close all"));</script></a></p>

		<script type="text/javascript">
			<%getUsbTreeView();%>
			//<!--			

			/*
			d = new dTree('d');

			d.add(0,-1,'My example tree');
			d.add(1,0,'Node 1','usb_storage_editor.asp');
			d.add(2,0,'Node 2','usb_storage_editor.asp');
			d.add(3,1,'Node 1.1','usb_storage_editor.asp');
			d.add(4,0,'Node 3','usb_storage_editor.asp');
			d.add(5,3,'Node 1.1.1','usb_storage_editor.asp');
			d.add(6,5,'Node 1.1.1.1','usb_storage_editor.asp');
			d.add(7,0,'Node 4','usb_storage_editor.asp');
			d.add(8,1,'Node 1.2','usb_storage_editor.asp');
			d.add(9,0,'My Pictures','usb_storage_editor.asp','Pictures I\'ve taken over the years','','','../images/dtree/imgfolder.gif');
			d.add(10,9,'The trip to Iceland','usb_storage_editor.asp','Pictures of Gullfoss and Geysir');
			d.add(11,9,'Mom\'s birthday','usb_storage_editor.asp');
			d.add(12,0,'Recycle Bin','usb_storage_editor.asp','','','../images/dtree/trash.gif');

			document.write(d);
			*/
			//-->
		</script>
	</div>	
	<div>
		<table border="0" cellpadding="0" cellspacing="0">
			<tr>
				<td class="blank1" height="30">&nbsp;</td>
			</tr>
		</table>
	</div>
	<div class="submitBg">
		<table border="0" cellpadding="0" cellspacing="0">
			<tr>
				<td><input type="submit" id="samba_apply" value="Apply" class="submit" title="Apply" onclick="return pageValidate();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
				<td><input type="button" id="samba_reset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
			</tr>
		</table>
	</div>	
	</form>
	</div>
	<!-- Section End -->
</div>
</div> <!-- End of all -->
<script type="text/javascript">
	document.getElementById('samba_apply').value=gettext("Apply");
	document.getElementById('samba_reset').value=gettext("Reset");
</script>
</body>
</html>
