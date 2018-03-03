<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"><html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="JavaScript" src="../js/textValidations.js" type="text/javascript"></script>
<script language="JavaScript" src="../js/ipv4AddrValidations.js" type="text/javascript"></script>
<script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/jquery-1.4.2.min.js"></script>
<script type="text/javascript" src="../js/jquery.json-2.2.min.js"></script>
<script type="text/javascript" src="../js/j_common.js"></script>

<script language="javascript" type="text/javascript">
CheckInitLoginInfo(<%getuser_login();%>);
var IPv6RoutingEditInfo=<%getIPv6RoutingEdit();%>;
var IPv6RoutingEditArr = IPv6RoutingEditInfo.split("#");
var IPv6RoutingName = <%getIPv6RoutingName();%>;
var IPv6RoutingNameArr = IPv6RoutingName.split("#");

var user_rule =/^[a-zA-Z0-9]+$/;
var ip_rule_V6=/^([\da-fA-F]{1,4}:){6}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^::([\da-fA-F]{1,4}:){0,4}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:):([\da-fA-F]{1,4}:){0,3}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){2}:([\da-fA-F]{1,4}:){0,2}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){3}:([\da-fA-F]{1,4}:){0,1}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){4}:((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){7}[\da-fA-F]{1,4}$|^:((:[\da-fA-F]{1,4}){1,6}|:)$|^[\da-fA-F]{1,4}:((:[\da-fA-F]{1,4}){1,5}|:)$|^([\da-fA-F]{1,4}:){2}((:[\da-fA-F]{1,4}){1,4}|:)$|^([\da-fA-F]{1,4}:){3}((:[\da-fA-F]{1,4}){1,3}|:)$|^([\da-fA-F]{1,4}:){4}((:[\da-fA-F]{1,4}){1,2}|:)$|^([\da-fA-F]{1,4}:){5}:([\da-fA-F]{1,4})?$|^([\da-fA-F]{1,4}:){6}:$/;
var gw_rule_v6=/^([\da-fA-F]{1,4}:){6}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^::([\da-fA-F]{1,4}:){0,4}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:):([\da-fA-F]{1,4}:){0,3}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){2}:([\da-fA-F]{1,4}:){0,2}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){3}:([\da-fA-F]{1,4}:){0,1}((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){4}:((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[01]?\d\d?)$|^([\da-fA-F]{1,4}:){7}[\da-fA-F]{1,4}$|^:((:[\da-fA-F]{1,4}){1,6}|:)$|^[\da-fA-F]{1,4}:((:[\da-fA-F]{1,4}){1,5}|:)$|^([\da-fA-F]{1,4}:){2}((:[\da-fA-F]{1,4}){1,4}|:)$|^([\da-fA-F]{1,4}:){3}((:[\da-fA-F]{1,4}){1,3}|:)$|^([\da-fA-F]{1,4}:){4}((:[\da-fA-F]{1,4}){1,2}|:)$|^([\da-fA-F]{1,4}:){5}:([\da-fA-F]{1,4})?$|^([\da-fA-F]{1,4}:){6}:$/;

function qos_edit_init()
{
	if (IPv6RoutingEditInfo == ""){  //Add
		document.getElementById('ipv6Routing_name').value = "";
		document.getElementById('ipv6Routing_desIP').value = "";
		document.getElementById('ipv6Routing_prelength').value = "";
		document.getElementById('ipv6Routing_network').selectedIndex = 0;
		document.getElementById('ipv6Routing_gateway').value = "";
	}else{							//Edit
		document.getElementById('ipv6Routing_name').value = IPv6RoutingEditArr[0];
		document.getElementById('ipv6Routing_desIP').value = IPv6RoutingEditArr[1];  
		document.getElementById('ipv6Routing_prelength').value = IPv6RoutingEditArr[2];
		document.getElementById('ipv6Routing_network').selectedIndex = IPv6RoutingEditArr[3];
		document.getElementById('ipv6Routing_gateway').value = IPv6RoutingEditArr[4];
	}
}

function selectNetworkType()
{
	var selNetworkType = document.getElementById("ipv6Routing_network").selectedIndex;
 	if (selNetworkType == 1) {
   		document.getElementById("ipv6Routing_gateway").disabled = true;
 	} else {
   		document.getElementById("ipv6Routing_gateway").disabled = false;
 	}
 	return;
}

function ipv6_routing_edit_validate()
{
	if(!CheckLoginInfo())
		return false;

	//Check routing name
	if(document.getElementById ("ipv6Routing_name").value == "") {
      alert(gettext("Please enter a valid name for the static route."));
      return false;
    } else {
  	  if(!user_rule.test(document.getElementById ("ipv6Routing_name").value)) {
  		alert(gettext("Name: Accept 0-9,a-z,A-Z or not accept Chinese format."));
  		document.getElementById("ipv6Routing_name").focus();
  		return false;
  	  }
    }
		
	//Check destination network format (option)
	var ipv6AddrObj = document.getElementById('ipv6Routing_desIP');	//IPv6 Address check
	if (ipv6AddrObj.value=="<%getLanBasic("IpAddress");%>")
	{
		alert(gettext("Please enter valid IPv6 Address.\nRouter IP Address is not allowed."));
		return false;
	}
	
	var routing_ip = document.getElementById('ipv6Routing_desIP');
	if(routing_ip.value != ""){				
		if(!ip_rule_V6.test(ipv6AddrObj.value)){
			alert(gettext("Please enter valid IPv6 Address.\n(xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx, eg: 1000:2000:3000:4000:5000:6000:7000:8000)"));
			return false;
		}
	}else{
		var msg = gettext("Destination Network should not be empty!!!");
		alert(msg);
		document.ipv6_routing_edit_set.ipv6Routing_desIP.focus();
		return false;
	}

	//Check prefix length is (0~128)
	var routing_prelength = parseInt(document.getElementById('ipv6Routing_prelength').value, 10);
	if(document.getElementById('ipv6Routing_prelength').value == ""){					
	   alert(gettext("Prefix length can't be empty."));
	   document.ipv6_routing_edit_set.ipv6Routing_prelength.focus();
	   return false;
	}
	if ((routing_prelength < 0)||(routing_prelength > 128)){
    	alert(gettext("Prefix length value is between 0 to 128."));
		document.ipv6_routing_edit_set.ipv6Routing_prelength.focus();
        return false;
    } 

	//Check gateway format
	var selNetworkType = document.getElementById("ipv6Routing_network").selectedIndex;
 	if (selNetworkType == 0) {		//LAN
   		var routing_gateway = document.getElementById('ipv6Routing_gateway');
		var routing_gatewayValue = document.getElementById('ipv6Routing_gateway').value;
		var IPv6RoutingGatewayArr = routing_gatewayValue.split(":");
		var gateway1 = parseInt(IPv6RoutingGatewayArr[1], 10);
		var gateway2 = parseInt(IPv6RoutingGatewayArr[2], 10);
		var gateway3 = parseInt(IPv6RoutingGatewayArr[3], 10);
		if(routing_gateway.value == ""){
			alert(gettext("Gateway can't be empty."));
			document.ipv6_routing_edit_set.ipv6Routing_gateway.focus();
		    return false;
	  	}else {
			if(!gw_rule_v6.test(routing_gateway.value))
			{
		  		alert(gettext("The Gateway is invalid."));
		  		document.ipv6_routing_edit_set.ipv6Routing_gateway.focus();
		  		return false;
		  	}
			if(IPv6RoutingGatewayArr[0] == "fc00"){
				if(gateway1 == 0){
					if(gateway2 == 0){
						if(gateway3 == 0){
							alert(gettext("The Gateway fc00:0:0:0:xxxx:xxxx:xxxx:xxxx is invalid."));
							document.ipv6_routing_edit_set.ipv6Routing_gateway.focus();
		  					return false;
						}
					}
				}	
			}
			if(IPv6RoutingGatewayArr[0] == "2001"){
				if(gateway1 == 1){
					if(gateway2 == 2){
						if(gateway3 == 3){
							alert(gettext("The Gateway 2001:1:2:3:xxxx:xxxx:xxxx:xxxx is invalid."));
							document.ipv6_routing_edit_set.ipv6Routing_gateway.focus();
		  					return false;
						}
					}
				}	
			}
	  	}	
 	} 
	
	//Check routing name duplicated
	var count = 0;
	if (IPv6RoutingEditInfo == ""){    //Add
	    for (var i=0; i < IPv6RoutingNameArr.length; i++){
	  	  var name = document.getElementById('ipv6Routing_name').value;
	  	  if (IPv6RoutingNameArr[i]==name){  
	  	    count++;
	  	  }
	    }
	  
	    if (count > 0){
	      alert(gettext("The static route name is duplicate."));
	  	  document.getElementById("starout_name").focus();
	  	  return false;
	    }
  	}	
	
	document.ipv6_routing_edit_set.action="/goform/setIPv6RoutingEdit?<%getUniName();%>=<%getUniId();%>";
	document.ipv6_routing_edit_set.submit();
    return true;
}

</script>
</head>

<body onload="qos_edit_init()">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("ipv6_submenu","ipv6_submenu_focus","ipv6_routing","ipv6_routing_href");</script>
<!-- Main Menu and Submenu End -->

<!-- Right Content start -->
<div class="contentBg">
  <div class="secH1"><script>document.write(gettext("IPv6 Static Routing Settings"));</script></div>
  <div class="secBg">  
  <div class="secInfo"></div>
  <!--div class="secH2"><script>document.write(gettext("Traffic Selector Configuration"));</script></div-->
  <form name="ipv6_routing_edit_set" method="post">
  <div style="display:block;" id="qos_add" name="qos_add" style="margin:10px 0 0 15px;">
    <div class="statusMsg"></div>
    <table class="configTbl" cellspacing="0">
      <tr>
	    <td><script>document.write(gettext("Route Name"));</script></td>
		<td><input id="ipv6Routing_name" name="ipv6Routing_name" type="text" class="configF1" maxlength="24" value="" size="20"/></td>
 	  </tr>
 	  <tr>
		<td><script>document.write(gettext("Destination Network"));</script></td>
		<td><input type="text" name="ipv6Routing_desIP" id="ipv6Routing_desIP" size="40" class="configF1" maxlength="39"></td>
	  </tr>
	  <tr>
		<td><script>document.write(gettext("Prefix Length"));</script></td>
		<td><input type="text" name="ipv6Routing_prelength" value="" id="ipv6Routing_prelength" size="20" class="configF1" maxlength="17"></td>
	  </tr>
	  <tr>
	    <td><script>document.write(gettext("Network Type"));</script></td>
		<td><select size="1" name="ipv6Routing_network" id="ipv6Routing_network" class="configF1" onchange="selectNetworkType();">
		  <option value=0>LAN</option>
		  <option value=1><script>document.write(gettext("Internet"));</script></option>
		  </select>
		</td>
 	  </tr>
 	  <tr>
		<td><script>document.write(gettext("Gateway IP Address"));</script></td>
		<td><input type="text" name="ipv6Routing_gateway" value="" id="ipv6Routing_gateway" size="40" class="configF1" maxlength="39"/></td>
	  </tr>
    </table>      
    <div>
      <td><input type="submit" id="apply" value="Apply" class="submit" title="Apply" onclick="return ipv6_routing_edit_validate();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
	  <td><input type="button" id="reset" value="Reset" class="submit" title="Reset" onclick="doRedirect();" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"></td>
	</div>
  </div>
  </form>
</div>
</div>
</div>
<script type="text/javascript">
  document.getElementById('apply').value=gettext("Apply");
  document.getElementById('reset').value=gettext("Reset");
</script>
</body>
</html>
