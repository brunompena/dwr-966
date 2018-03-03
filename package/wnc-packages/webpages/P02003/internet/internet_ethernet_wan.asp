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

function initEthernetWanInfo()
{
	var WanInfo = <%getEthernetWANSetting();%>;
	var WanInfoArr = WanInfo.split("#");
	document.getElementById("ethernet_wan_type").selectedIndex=WanInfoArr[0]; 
	document.getElementById("wan_servicename").value=WanInfoArr[1]; 
	document.getElementById("wan_username").value=WanInfoArr[2];
	document.getElementById("wan_password").value=WanInfoArr[3];
	document.getElementById("wan_ip").value=WanInfoArr[4];
	document.getElementById("wan_mac").value=WanInfoArr[5];
	document.getElementById("wan_subnet").value=WanInfoArr[6];
	document.getElementById("wan_gw").value=WanInfoArr[7];
	document.getElementById("wan_mtu").value=WanInfoArr[8];
	document.getElementById("wan_hostname").value=WanInfoArr[9];
	document.getElementById("wan_dns1").value=WanInfoArr[10];
	document.getElementById("wan_dns2").value=WanInfoArr[11];
		
	selectWanType();
}

function selectWanType()
{
	var wan_type = document.getElementById("ethernet_wan_type").value;
	if (wan_type == "0"){		//PPPoE
		document.getElementById("servicename").style.display = "block";
		document.getElementById("username").style.display = "block";
		document.getElementById("password").style.display = "block";
		document.getElementById("ip").style.display = "block";
		document.getElementById("macaddr").style.display = "block";
		document.getElementById("subnetmask").style.display = "none";
		document.getElementById("gateway").style.display = "none";
		document.getElementById("mtu").style.display = "none";
		document.getElementById("hostname").style.display = "none";
		document.getElementById("dns1").style.display = "block";
		document.getElementById("dns2").style.display = "block";	
	}else if (wan_type == "1"){//Dynamic IP
		document.getElementById("servicename").style.display = "none";
		document.getElementById("username").style.display = "none";
		document.getElementById("password").style.display = "none";
		document.getElementById("ip").style.display = "none";
		document.getElementById("macaddr").style.display = "none";
		document.getElementById("subnetmask").style.display = "none";
		document.getElementById("gateway").style.display = "none";
		document.getElementById("mtu").style.display = "none";
		document.getElementById("hostname").style.display = "block";
		document.getElementById("dns1").style.display = "block";
		document.getElementById("dns2").style.display = "block";
	}else if (wan_type == "2"){//Static IP
		document.getElementById("servicename").style.display = "none";
		document.getElementById("username").style.display = "none";
		document.getElementById("password").style.display = "none";
		document.getElementById("ip").style.display = "block";
		document.getElementById("macaddr").style.display = "none";
		document.getElementById("subnetmask").style.display = "block";
		document.getElementById("gateway").style.display = "block";
		document.getElementById("mtu").style.display = "block";
		document.getElementById("hostname").style.display = "none";
		document.getElementById("dns1").style.display = "block";
		document.getElementById("dns2").style.display = "block";
	}
}

function submitWANsetting()
{
	if(!CheckLoginInfo())
	return false;

	var wan_type = document.getElementById("ethernet_wan_type").value;
	var ip_rule=/^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$/;
	var ip_check_rule=/^((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))\.((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))\.((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))\.((\d)|(([1-9])\d)|(1\d\d)|(2(([0-4]\d)|5([0-5]))))$/;
	var mac_rule=/[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}/;
    var netmask_rule=/^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$/;
	var gw_rule=/^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-4]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])$/;
	
	var wan_service = document.getElementById("wan_servicename").value;
	var wan_user = document.getElementById('wan_username').value;
	var wan_pwd = document.getElementById('wan_password').value;
	var wan_ip = document.getElementById('wan_ip');
	var wan_mac = document.getElementById('wan_mac');

	var wan_subnet = document.getElementById ("wan_subnet");
	var wan_gateway = document.getElementById ("wan_gw");
	var wan_mtu = document.getElementById ("wan_mtu").value;

	var wan_hostname=document.getElementById("wan_hostname").value;

	var wan_dns1 = document.getElementById('wan_dns1').value;
	var wan_dns2 = document.getElementById('wan_dns2').value;

	if (wan_type == "0"){		//PPPoE
		if(wan_service != ""){ 				//Service Name
 			if (checkCommonNameField('wan_servicename', gettext("Service Name")) == false) return false;
		}	

		if (wan_user.length > 0){			//User Name
		   if (checkCommonNameField('wan_username', gettext("User Name")) == false) return false;
		} else {
		   alert(gettext("User Name can't be empty."));
		   return false;
		}

		if (wan_pwd.length > 0){			//Password
		   if (checkCommonNameField('wan_password', gettext("Password")) == false) return false;
		} else {
		   alert(gettext("Password can't be empty."));
		   return false;
		}
		
		if(wan_ip.value != ""){				//IP Address
			if(!ip_rule.test(wan_ip.value)){
				var msg = wan_ip.value+ " " + gettext("format is illegal.");
				alert(msg);
				return false;
			}
			if(!ip_check_rule.test(wan_ip.value)){
				var msg = wan_ip.value+ " " + gettext("format is illegal.");
				alert(msg);
				return false;
			}
		}
		
		if(wan_mac.value != ""){			//MAC Address
			if(!mac_rule.test(wan_mac.value)){
				var msg = wan_mac.value+ " " + gettext("format is illegal.");
				alert(msg);
				return false;
			}
		}
		
		if(wan_dns1 != ""){					//DNS1			
			if (checkCommonNameField('wan_dns1', gettext("Domain Name")) == false) return false;

			if (checkDotChar(wan_dns1) == false) {
			   alert(gettext("Invalid Domain Name"));
			   return false;
			}
		}

		if(wan_dns2 != ""){					//DNS2
			if (checkCommonNameField('wan_dns2', gettext("Domain Name")) == false) return false;

			if (checkDotChar(wan_dns2) == false) {
			   alert(gettext("Invalid Domain Name"));
			   return false;
			}
		}
	}else if (wan_type == "1"){//Dynamic IP
		if(wan_hostname != "") {			//Host Name
			if (checkCommonNameField('wan_hostname', gettext("Host Name")) == false) return false;

			if (checkDotChar(wan_hostname) == false) {
			   alert(gettext("Invalid Host Name"));
			   return false;
			}
		}

		if(wan_dns1 != ""){					//DNS1
		   	if (checkCommonNameField('wan_dns1', gettext("Domain Name")) == false) return false;

			if (checkDotChar(wan_dns1) == false) {
			   alert(gettext("Invalid Domain Name"));
			   return false;
			}
		}
		
		if(wan_dns2 != ""){					//DNS2
			if (checkCommonNameField('wan_dns2', gettext("Domain Name")) == false) return false;

			if (checkDotChar(wan_dns2) == false) {
			   alert(gettext("Invalid Domain Name"));
			   return false;
			}
		}		
	}else if (wan_type == "2"){//Static IP
		if(wan_ip.value == ""){				//IP Address
			alert(gettext("IP address can't be empty."));
			return false;
		}else{
			if(!ip_rule.test(wan_ip.value)){
				var msg = wan_ip.value+ " " + gettext("format is illegal.");
				alert(msg);
				return false;
			}
			if(!ip_check_rule.test(wan_ip.value)){
				var msg = wan_ip.value+ " " + gettext("format is illegal.");
				alert(msg);
				return false;
			}
		}
		
		if( wan_subnet.value == ""){		//Subnet
		    alert(gettext("Subnet Mask can't be empty."));
		    return false;
	  	} else {
		  	if(!netmask_rule.test(wan_subnet.value)) {
		  		alert(gettext("Subnet Mask is invalid."));
		  		wan_subnet.focus();
		  		return false;
		  	}
	  	}
		
		if(wan_gateway.value == ""){		//Gateway
			alert(gettext("Local Gateway can't be empty."));
		    return false;
	  	} else {
			if(!gw_rule.test(wan_gateway.value))
			{
		  		alert(gettext("Local Gateway is invalid."));
		  		wan_gateway.focus();
		  		return false;
		  	}
	  	}
		
		wan_mtu = parseInt(wan_mtu, 10); 	//MTU
		if(wan_mtu == ""){					
		   alert(gettext("MTU can't be empty."));
		   return false;
		}
		if (wan_mtu <= 0){
        	alert(gettext("MTU can't less than 1."));
            return false;
        } 
		
		if(wan_dns1 == ""){					//DNS1
		   alert(gettext("Domain Name 1 can't be empty."));
		   return false;
		}
		if (checkCommonNameField('wan_dns1', gettext("Domain Name")) == false) return false;

		if (checkDotChar(wan_dns1) == false) {
		   alert(gettext("Invalid Domain Name"));
		   return false;
		}
		if(wan_dns2 != ""){					//DNS2
			if (checkCommonNameField('wan_dns2', gettext("Domain Name")) == false) return false;

			if (checkDotChar(wan_dns2) == false) {
			   alert(gettext("Invalid Domain Name"));
			   return false;
			}
		}	
	}

	document.ethernet_wan_set.action="/goform/setEthernetWanSetting?<%getUniName();%>=<%getUniId();%>";
	document.ethernet_wan_set.submit();
    return true;
}

function checkDotChar(elem)
{
 if (elem.charCodeAt(elem.length-1) == 46)
   return false;
 return true;
}

</script>
</head>

<body onload="initEthernetWanInfo();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("lte");%>
<script type="text/javascript">menuChange("lte_menu");leftMenuChange("internet_ethernet_wan", "internet_ethernet_wan_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Ethernet WAN"));</script></div>
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo"><br /><script>document.write(gettext("This page is for you to set ethernet WAN."));</script><br /></div>
  		<form name="ethernet_wan_set" method="post">
  			<input type="hidden" id="WanStatus" name="WanStatus" />
  			<!--Ethernet WAN Type-->
			<table cellspacing="0" class="configTbl">
			<tr id="width2">
				<td><script>document.write(gettext("Ethernet WAN"));</script></td>
				<td><select class="configF1" name="ethernet_wan_type" id="ethernet_wan_type" size="1" onChange="selectWanType();">
					<option value="0"><script>document.write(gettext("PPPoE"));</script></option>
					<option value="1"><script>document.write(gettext("Dynamic IP"));</script></option>
					<option value="2"><script>document.write(gettext("Static IP"));</script></option>
				</select></td>      
			</tr>
			</table>
			<!--Ethernet WAN Setting-->
			<table cellspacing="0" class="configTbl" >
				<tr id="servicename" style="display:none;">
					<td width="100"><script>document.write(gettext("Service Name"));</script></td>
					<td><input type="text" name="wan_servicename" value="" id="wan_servicename" size="20" maxlength="64" class="configF1"/></td>
			    </tr>
				<tr id="username" style="display:none;">
					<td width="100"><script>document.write(gettext("User Name"));</script></td>
					<td><input type="text" name="wan_username" value="" id="wan_username" size="20" maxlength="32" class="configF1"/></td>
				</tr>
				<tr id="password" style="display:none;">
					<td width="100"><script>document.write(gettext("Password"));</script></td>
					<td><input type="password" name="wan_password" id="wan_password" value="" size="20" maxlength="64" class="configF1"/></td>
				</tr>
				<tr id="ip" style="display:none;">
					<td width="100"><script>document.write(gettext("IP Address"));</script></td>
					<td><input type="text" name="wan_ip" value="" id="wan_ip" size="20" class="configF1" maxlength="15" onkeypress="return keypress_ip_format(event)"></td>
				</tr>
				<tr id="macaddr" style="display:none;">
					<td width="100"><script>document.write(gettext("MAC Address"));</script></td>
					<td><input type="text" name="wan_mac" value="" id="wan_mac" size="20" class="configF1" maxlength="17"></td>
				</tr>
				<tr id="subnetmask" style="display:none;">
      				<td width="100"><script>document.write(gettext("Subnet Mask"));</script></td>
      				<td><input type="text" name="wan_subnet" value="" id="wan_subnet" size="20" class="configF1" maxlength="17" /></td>
    			</tr> 
    			<tr id="gateway" style="display:none;">
      				<td width="100"><script>document.write(gettext("Local Gateway"));</script></td>
      				<td><input type="text" name="wan_gw" value="" id="wan_gw" size="20" class="configF1" maxlength="17"/></td>
    			</tr>
    			<tr id="mtu" style="display:none;">
					<td width="100"><script>document.write(gettext("MTU"));</script></td>
					<td><input type="text" name="wan_mtu" value="" id="wan_mtu" size="20" class="configF1" maxlength="17"></td>
				</tr>
				<tr id="hostname" style="display:none;">
					<td  width="100"><script>document.write(gettext("Host Name"));</script></td>
					<td><input type="text" name="wan_hostname" value="<%getLanBasic("hostname");%>" id="wan_hostname" size="20" class="configF1" maxlength="32"/></td>
				</tr>
    			<tr id="dns1" style="display:none;">
					<td width="100"><script>document.write(gettext("DNS1"));</script></td>
					<td><input type="text" name="wan_dns1" id="wan_dns1" value="" size="20" maxlength="64" class="configF1" /></td>
				</tr>
				<tr id="dns2" style="display:none;">
					<td width="100"><script>document.write(gettext("DNS2"));</script></td>
					<td><input type="text" name="wan_dns2" id="wan_dns2" value="" size="20" maxlength="64" class="configF1" /></td>
				</tr>
			</table>

			<div>
	  			<input type="submit" id="apply" value="Apply" class="tblbtn" title="Apply" name="" onmouseover="this.className = 'tblbtnHover'" onmouseout="this.className = 'tblbtn'" onclick="return submitWANsetting();">
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
