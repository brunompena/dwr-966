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

var timerId = null;
var apn1_status;
function InternetStatusInit()
{
	var WanInfo = <%getWanStatistics();%>;
 var WanArr = WanInfo.split("#");

 InternetStatusDisplay(WanArr);

 setTimeout(function () { autoRefresh_internet_info(); }, 10000);

	return;
}

function get_pkt_string(total_bytes)
{
  var result_str = total_bytes;
  if(total_bytes >= 1024)
  {
    result_str = result_str/1024;
    result_str = Math.round(result_str*100)/100;
    if(result_str >= 1024)
    {
        result_str = result_str/1024;
        result_str = Math.round(result_str*100)/100;
	 if(result_str >= 1024) 
	 {
	        result_str = result_str/1024;
	        result_str = Math.round(result_str*100)/100;
        	return result_str + " GB";
	 }else
        	return result_str + " MB";
    } else
        return result_str + " KB";        
  } else
    return result_str + " B";
}

function InternetStatusDisplay(WanArr)
{
	var CAInfo = <%getCAInfo();%>;
 	var CAInfoArr = CAInfo.split("#");
	var imsi = <%getIMSI_db1();%>;
	var imsi1 = <%getIMSI_db();%>;
	var imei = <%getIMEI_db();%>;
 
	//Network Status:: APN1
	if(WanArr[0] == -1)
		document.getElementById("apn1_tx_packets").innerHTML="0";
	else
  document.getElementById("apn1_tx_packets").innerHTML=WanArr[0];

	if(WanArr[1] == -1)
		document.getElementById("apn1_tx_bytes").innerHTML="0";
	else
  document.getElementById("apn1_tx_bytes").innerHTML=get_pkt_string(WanArr[1]);

	if(WanArr[2] == -1)
  document.getElementById("apn1_tx_errors").innerHTML="0";
	else
  document.getElementById("apn1_tx_errors").innerHTML=WanArr[2];

	if(WanArr[3] == -1)
	 document.getElementById("apn1_tx_overflows").innerHTML="0";
	else
  document.getElementById("apn1_tx_overflows").innerHTML=WanArr[3];

 if(WanArr[4] == -1)
		document.getElementById("apn1_rx_packets").innerHTML="0";
	else
  document.getElementById("apn1_rx_packets").innerHTML=WanArr[4];

	if(WanArr[5] == -1)
		document.getElementById("apn1_rx_bytes").innerHTML="0";
	else
  document.getElementById("apn1_rx_bytes").innerHTML=get_pkt_string(WanArr[5]);

	if(WanArr[6] == -1)
  document.getElementById("apn1_rx_errors").innerHTML="0";
	else
  document.getElementById("apn1_rx_errors").innerHTML=WanArr[6];

	if(WanArr[7] == -1)
  document.getElementById("apn1_rx_overflows").innerHTML="0";
	else
  document.getElementById("apn1_rx_overflows").innerHTML=WanArr[7];

	//* APN1 Status *//
	//Registration state
	var reg_state = gettext(WanArr[8]);
	document.getElementById("apn1_status").innerHTML=reg_state;

 if(WanArr[8] == "Registered") {
   //Signal Strength
   document.getElementById("apn1_signals").innerHTML=WanArr[9] + " dBm";
   //Network Name

   if (imsi1 == "26012") {
     document.getElementById('apn1_network_name').innerHTML="Cyfrowy Polsat";
     document.getElementById('apn1_network_name').title="Cyfrowy Polsat";
   } else if (imsi1 == "26001") {
     document.getElementById('apn1_network_name').innerHTML="Plus";
     document.getElementById('apn1_network_name').title="Plus";
   } else {
     document.getElementById("apn1_network_name").innerHTML=WanArr[10];
   }
   
   
   //Mobile Country Code(MCC)
   document.getElementById("apn1_mcc").innerHTML=WanArr[11];
   //Mobile Network Code(MNC)
   document.getElementById("apn1_mnc").innerHTML=WanArr[12];
   //Cell ID
   if (WanArr[13] == "-1")
     document.getElementById("apn1_cell_id").innerHTML="";
   else
     document.getElementById("apn1_cell_id").innerHTML=WanArr[13];
   //RSSI
   document.getElementById("apn1_rssi").innerHTML=WanArr[14];
   //Radio Interference
   document.getElementById("apn1_radio_interference").innerHTML=WanArr[15];
   //IMSI
   document.getElementById("apn1_imsi").innerHTML=imsi;
 } else {
   //Signal Strength
   document.getElementById("apn1_signals").innerHTML="-128 dBm";
   //Network Name
   document.getElementById("apn1_network_name").innerHTML=gettext("None");
   //Mobile Country Code(MCC)
   document.getElementById("apn1_mcc").innerHTML="";
   //Mobile Network Code(MNC)
   document.getElementById("apn1_mnc").innerHTML="";
   //Cell ID
   document.getElementById("apn1_cell_id").innerHTML="";
   //RSSI
   document.getElementById("apn1_rssi").innerHTML="1,(128,0)";
   //Radio Interference
   document.getElementById("apn1_radio_interference").innerHTML=gettext("None");
   //IMSI
   document.getElementById("apn1_imsi").innerHTML="";
 }

	document.getElementById("apn1_imei").innerHTML=imei;
	document.getElementById("apn1_rscp").innerHTML=WanArr[16] + " dBm";
	document.getElementById("apn1_rsrp").innerHTML=WanArr[17] + " dBm";
	document.getElementById("apn1_sinr").innerHTML=WanArr[18] + " dBm";
	
 	//CA Information
	if (CAInfoArr[0] == "0"){
		document.getElementById('status_ca_title').style.visibility ="hidden";
     	document.getElementById('status_ca').style.visibility ="hidden";
		document.getElementById('status_band1_title').style.visibility ="hidden";
     	document.getElementById('status_band1').style.visibility ="hidden";
		document.getElementById('status_band2_title').style.visibility ="hidden";
     	document.getElementById('status_band2').style.visibility ="hidden";
	}else if (CAInfoArr[0] == "1"){	
		document.getElementById('status_ca_title').style.visibility ="visible";
     	document.getElementById('status_ca').style.visibility ="visible";
		document.getElementById('status_band1_title').style.visibility ="visible";
     	document.getElementById('status_band1').style.visibility ="visible";
		document.getElementById('status_band2_title').style.visibility ="visible";
     	document.getElementById('status_band2').style.visibility ="visible";
		document.getElementById("status_ca").innerHTML=gettext("Available");
		if (CAInfoArr[1] == "-1"){
			document.getElementById("status_band1").innerHTML=gettext("None");
		}else{
			document.getElementById("status_band1").innerHTML=CAInfoArr[1];
		}
		if (CAInfoArr[2] == "-1"){
			document.getElementById("status_band2").innerHTML=gettext("None");
		}else{
			document.getElementById("status_band2").innerHTML=CAInfoArr[2];
		}
	} else if (CAInfoArr[0] == "2"){
		document.getElementById('status_ca_title').style.visibility ="visible";
     	document.getElementById('status_ca').style.visibility ="visible";
		document.getElementById('status_band1_title').style.visibility ="visible";
     	document.getElementById('status_band1').style.visibility ="visible";
		document.getElementById('status_band2_title').style.visibility ="visible";
     	document.getElementById('status_band2').style.visibility ="visible";
		document.getElementById("status_ca").innerHTML=gettext("Active");
		if (CAInfoArr[1] == "-1"){
			document.getElementById("status_band1").innerHTML=gettext("None");
		}else{
			document.getElementById("status_band1").innerHTML=CAInfoArr[1];
		}
		if (CAInfoArr[2] == "-1"){
			document.getElementById("status_band2").innerHTML=gettext("None");
		}else{
			document.getElementById("status_band2").innerHTML=CAInfoArr[2];
		}
	}

	if(WanArr[8] == "Registered")
		apn1_status = true;
	else
		apn1_status = false;

	return;
}
</script>
</head>

<body onload="InternetStatusInit();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("lte");%>
<script type="text/javascript">menuChange("lte_menu");leftMenuChange("internet_status", "internet_status_href");</script>
<!-- Main Menu and Submenu End -->

<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("3G/LTE Status"));</script></div>
	<div class="secBg">
		<div class="statusMsg"></div>
		<div class="secInfo">
		<br><script>document.write(gettext("This page shows port statistics for 3G/LTE."));</script>
		<br>
		</div>
  <div class="secH2"><script>document.write(gettext("Network Status"));</script></div>
  <table border="0" cellpadding="0" cellspacing="0" width="654">
    <!--<tr>
      <td class="tdH" width="45%"><script>document.write(gettext("LTE Device Name"));</script></td>
      <td class="tdH" width="55%"><script>document.write(gettext("APN1"));</script></td>
    </tr>-->
    <tr>
      <td class="tdH"><script>document.write(gettext("Registration State"));</script></td>
      <td class="tdEven" id="apn1_status"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("Network"));</script></td>
      <td class="tdEven" id="apn1_radio_interference"></td>
    </tr>
    <tr style="display:none">
      <td class="tdH"><script>document.write(gettext("RSSI"));</script></td>
      <td class="tdEven" id="apn1_rssi"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("RSSI"));</script></td>
      <td class="tdEven" id="apn1_signals"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("RSCP"));</script></td>
      <td class="tdEven" id="apn1_rscp"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("RSRP"));</script></td>
      <td class="tdEven" id="apn1_rsrp"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("SINR"));</script></td>
      <td class="tdEven" id="apn1_sinr"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("Network Name"));</script></td>
      <td class="tdEven" id="apn1_network_name"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("Mobile Country Code(MCC)"));</script></td>
      <td class="tdEven" id="apn1_mcc"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("Mobile Network Code(MNC)"));</script></td>
      <td class="tdEven" id="apn1_mnc"></td>
    </tr>
    <tr>
      <td class="tdH" id="apn1_imsi_title"><script>document.write(gettext("IMSI"));</script></td>
      <td class="tdEven" id="apn1_imsi"></td>
    </tr>
    <tr>
      <td class="tdH" id="apn1_imei_title"><script>document.write(gettext("IMEI"));</script></td>
      <td class="tdEven" id="apn1_imei"></td>
    </tr>
    <tr>
      <td class="tdH"><script>document.write(gettext("Cell ID"));</script></td>
      <td class="tdEven" id="apn1_cell_id"></td>
    </tr>
    <tr>
      <td class="tdH" id="status_ca_title"><script>document.write(gettext("CA state"));</script></td>
      <td class="tdEven" id="status_ca"></td>
    </tr>
    <tr>
      <td class="tdH" id="status_band1_title"><script>document.write(gettext("Primary band"));</script></td>
      <td class="tdEven" id="status_band1"></td>
    </tr>
    <tr>
      <td class="tdH" id="status_band2_title"><script>document.write(gettext("Secondary band"));</script></td>
      <td class="tdEven" id="status_band2"></td>
    </tr>
  </table>
  <br /><br />
  <div class="secH2"><script>document.write(gettext("Transmit"));</script></div>
  <table border="0" cellpadding="0" cellspacing="0" width="654">
    <tr>
      <!--<td class="tdH" width="20%"><script>document.write(gettext("LTE Device Name"));</script></td>-->
      <td class="tdH" width="20%"><script>document.write(gettext("Tx Packets"));</script></td>
      <td class="tdH" width="20%"><script>document.write(gettext("Tx Errors"));</script></td>
      <td class="tdH" width="20%"><script>document.write(gettext("Tx Overflows"));</script></td>
      <td class="tdH" width="20%"><script>document.write(gettext("Tx Bytes"));</script></td>
    </tr>
    <tr>
      <!--<td class="tdOdd">APN1</td>-->
      <td id="apn1_tx_packets" class="tdOdd"></td>
      <td id="apn1_tx_errors" class="tdOdd"></td>
      <td id="apn1_tx_overflows" class="tdOdd"></td>
      <td id="apn1_tx_bytes" class="tdOdd"></td>
    </tr>
  </table>
  <br /><br />
  <div class="secH2"><script>document.write(gettext("Receive"));</script></div>
  <table border="0" cellpadding="0" cellspacing="0" width="654">
    <tr>
      <!--<td class="tdH" width="20%"><script>document.write(gettext("LTE Device Name"));</script></td>-->
      <td class="tdH" width="20%"><script>document.write(gettext("Rx Packets"));</script></td>
      <td class="tdH" width="20%"><script>document.write(gettext("Rx Errors"));</script></td>
      <td class="tdH" width="20%"><script>document.write(gettext("Rx Overflows"));</script></td>
      <td class="tdH" width="20%"><script>document.write(gettext("Rx Bytes"));</script></td>
    </tr>
    <tr>
      <!--<td class="tdOdd">APN1</td>-->
      <td id="apn1_rx_packets" class="tdOdd"></td>
      <td id="apn1_rx_errors" class="tdOdd"></td>
      <td id="apn1_rx_overflows" class="tdOdd"></td>
      <td id="apn1_rx_bytes" class="tdOdd"></td>
    </tr>
  </table>
  <br />
		<div>
			<input type="button" value="Refresh" class="submit" title="Refresh" id="button.refresh" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" onclick="doRedirect();" />
		</div>
	</div>
</div>
</div>
<script type="text/javascript">
function autoRefresh_internet_info()
{
	var ajax = createAjax();

	if (timerId) {
	  clearTimeout(timerId);
	}

 ajax.open('GET', "get_Internet_info.asp?time="+ new Date(), true);
 ajax.send(null);
	ajax.onreadystatechange = function ()
	{
  if (ajax.readyState == 4)
		{
   if (ajax.status == 200)
			{
    var data = eval('(' + ajax.responseText + ')');
    if (data)
				{
       var WanArr = data.split("#");
       InternetStatusDisplay(WanArr);

	      if(apn1_status == false)
	      {
         timerId = setTimeout(function () { autoRefresh_internet_info(); }, 30000);
	      }
	      else
	      {
		       timerId = setTimeout(function () { autoRefresh_internet_info(); }, 10000);
	      }
    }
   }
  }
 }
}
</script>
<script type="text/javascript">
	document.getElementById('button.refresh').value=gettext("Refresh");
</script>
</body>
</html>
