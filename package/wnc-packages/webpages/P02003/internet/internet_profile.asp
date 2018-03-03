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

var radioMode = <%getLTERadio();%>;
var radioArr = radioMode.split("#");
var lte_roaming="";
function internet_profile_list_action(action)
{
	if(!CheckLoginInfo())
		return false;

	var editor_action = document.getElementById('profileAction');
	var index = document.getElementById('profileIndex').value;
	editor_action.value = action;

	if (action == "Profile_Connect") {
		if((lte_roaming == "Roaming") && ((document.getElementById("profile1Status").value=="1" && index == 1) ||(document.getElementById("profile2Status").value=="1" && index == 2)))
		{
			var confirm_select;
			confirm_select = window.confirm(gettext("You're now roaming on a foreigner network. This will incur additional roaming charges."));
			if(!confirm_select)
				return false;
		}
		document.getElementById("connectBtn").style.display = "none";
		document.getElementById("editBtn").style.display = "none";
	} else {
		document.getElementById("connectBtn").disabled = true;
	}
	
	return true;
}

function clickProfile(profileIndex)
{
	document.getElementById('profileIndex').value = profileIndex;
	if(profileIndex == 1){
		document.getElementById("radio_profile_1").checked=true;
		document.getElementById("radio_profile_2").checked=false;
		if(document.getElementById("status_1").innerHTML == "Disconnected")
			document.getElementById("connectBtn").value=gettext("Connect");
		else
			document.getElementById("connectBtn").value=gettext("Disconnect");
	}else{
		document.getElementById("radio_profile_1").checked=false;
		document.getElementById("radio_profile_2").checked=true;
		if(document.getElementById("status_2").innerHTML == "Disconnected")
			document.getElementById("connectBtn").value=gettext("Connect");
		else
			document.getElementById("connectBtn").value=gettext("Disconnect");
	}
}

function internet_profile_init()
{
 	//document.getElementById("radio_profile_1").checked=true;
	var ProfileConnectIndex = <%getWanProfileListConnectIndex();%>;
	var ProfileConnectIndexArr = ProfileConnectIndex.split("#");
	document.getElementById('profileIndex').value = ProfileConnectIndexArr[0];
	if(ProfileConnectIndexArr[0] == 2)
		document.getElementById("radio_profile_2").checked=true;
	else
		document.getElementById("radio_profile_1").checked=true;

	var NetworkType = <%getNetworkType();%>;

	var ProfileData1 = <%getWanProfileListData1();%>;
	var ProfileData1_info = ProfileData1.split("####");
	var ProfileData1Arr;
	lte_roaming = ProfileData1_info[1];

	if (NetworkType == "1") {	//Ethernet WAN is on
		fieldStateChangeWr("connectBtn","","","");
		document.getElementById("StatusMsg").innerHTML = gettext("Ethernet WAN is on. The network profile cannot connect.");
	}
	
	if (ProfileData1_info[0] != "")
	{
		ProfileData1Arr = ProfileData1_info[0].split("#");
		if ((ProfileData1Arr[0] == "0.0.0.0") && (ProfileData1Arr[1] == "0000:0000:0000:0000:0000:0000:0000:0000/00")) {
				document.getElementById("status_1").innerHTML=gettext("Disconnected");
				document.getElementById("status_2").innerHTML=gettext("Disconnected");
				document.getElementById("connectBtn").value=gettext("Connect");
				document.getElementById("profile1Status").value="1";
				document.getElementById("profile2Status").value="1";
		} else {
			if(ProfileConnectIndexArr[0] == 2) {
				document.getElementById("status_2").innerHTML=gettext("Connected to APN2");
				document.getElementById("status_1").innerHTML=gettext("Disconnected");
				document.getElementById("connectBtn").value=gettext("Disconnect");
				document.getElementById("profile1Status").value="1";
				document.getElementById("profile2Status").value="0";
			}else{
				document.getElementById("status_1").innerHTML=gettext("Connected to APN1");
				document.getElementById("status_2").innerHTML=gettext("Disconnected");
				document.getElementById("connectBtn").value=gettext("Disconnect");
				document.getElementById("profile1Status").value="0";
				document.getElementById("profile2Status").value="1";
			}
		}
	} else {
		document.getElementById("status_1").innerHTML="";
		document.getElementById("status_2").innerHTML="";
		document.getElementById("connectBtn").value=gettext("Connect");
		document.getElementById("connectBtn").disabled=true;
		document.getElementById("profile1Status").value="1";
		document.getElementById("profile2Status").value="1";
	}

	var ProfileData2 = <%getWanProfileListData2();%>;
	var ProfileData2Arr;
	if (ProfileData2 != "")
	{
		ProfileData2Arr = ProfileData2.split("#");
		document.getElementById("profile_name_1").innerHTML=ProfileData2Arr[0];
		document.getElementById("profile_name_2").innerHTML=ProfileData2Arr[1];
	}
	else
	{
		document.getElementById("profile_name_1").innerHTML="";
		document.getElementById("profile_name_2").innerHTML="";
	}

}
</script>
</head>

<body onload="internet_profile_init();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("lte");%>
<script type="text/javascript">menuChange("lte_menu");leftMenuChange("internet_profile", "internet_profile_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Profile Configuration"));</script></div>
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo">
			<br /><script>document.write(gettext("This page displays current list of profiles."));</script>
			<br />
			<table cellspacing="0" class="configTbl">
			<tr align="center"><td class="msg" id="StatusMsg"></td></tr>
			</table>
		</div>
  <form name="internet_profile" method="post" action="/goform/setWanProfile?<%getUniName();%>=<%getUniId();%>">
  <input type="hidden" name="profileAction" id="profileAction" value="" />
  <input type="hidden" name="profileIndex" id="profileIndex" value="" />
  <input type="hidden" name="profile1Status" id="profile1Status" value="" />
  <input type="hidden" name="profile2Status" id="profile2Status" value="" />
		<table border="0" cellpadding="0" cellspacing="0" width="500">
   <tr>
     <td class="secH2"><script>document.write(gettext("Network Profile List"));</script></td>
   </tr>
   <tr>
     <td>
       <table border="0" cellpadding="0" cellspacing="0" id="tblProfileList" width="100%">
         <tr>
          <td class="tdH" width="10%">&nbsp;</td>
          <td class="tdH" width="20%"><script>document.write(gettext("APN Index"));</script></td>
          <td class="tdH" width="35%"><script>document.write(gettext("Profile Name"));</script></td>
          <td class="tdH"><script>document.write(gettext("Status"));</script></td>
         </tr>
         <tr>
          <td class="tdOdd">
            <input type="radio" name="radio_profile_1" id="radio_profile_1" value="1" onclick="clickProfile(1);" />
          </td>
          <td class="tdOdd">APN 1</td>
          <td class="tdOdd" name="profile_name_1" id="profile_name_1"></td>
          <td class="tdOdd" name="status_1" id="status_1"></td>
         </tr>
         <tr>
          <td class="tdOdd">
            <input type="radio" name="radio_profile_2" id="radio_profile_2" value="" onclick="clickProfile(2);" />
          </td>
          <td class="tdOdd">APN 2</td>
          <td class="tdOdd" name="profile_name_2" id="profile_name_2"></td>
          <td class="tdOdd" name="status_2" id="status_2"></td>
         </tr>
       </table>
     </td>
   </tr>
		</table>
		<div>
			<input type="submit" value="" class="submit" id="connectBtn" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" onclick="return internet_profile_list_action('Profile_Connect');" />
			<input type="submit" value="Edit" class="submit" id="editBtn" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" onclick="return internet_profile_list_action('Profile_Edit');" />
		</div>
	 </form>
	</div>
</div>
</div>
<script type="text/javascript">
	document.getElementById('editBtn').value=gettext("Edit");
</script>
</body>
</html>
