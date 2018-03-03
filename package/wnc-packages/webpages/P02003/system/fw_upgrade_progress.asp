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
<script language="JavaScript" src="../js/progressBar.js" type="text/javascript"></script>
<script type="text/javascript" src="../js/j_common.js"></script>
<script language="javascript" type="text/javascript">
function upgradeInit()
{
	var sim_imsi = document.getElementById('sim_imsi').value;
	var language = "<%getLanguage();%>";
	if (language == "pl"){
		if (sim_imsi == "26012")
		{
			document.getElementById('div_contentBg_1').className = "contentBg0_cp_pl";
			document.getElementById('div_contentBg_2').className = "contentBg0_cp_pl";
			document.getElementById('div_contentBg_3').className = "contentBg0_cp_pl";
		} else if (sim_imsi == "26001") {
			document.getElementById('div_contentBg_1').className = "contentBg0_plk_pl";
			document.getElementById('div_contentBg_2').className = "contentBg0_plk_pl";
			document.getElementById('div_contentBg_3').className = "contentBg0_plk_pl";
		} else {
			document.getElementById('div_contentBg_1').className = "contentBg0_new_pl";
			document.getElementById('div_contentBg_2').className = "contentBg0_new_pl";
			document.getElementById('div_contentBg_3').className = "contentBg0_new_pl";
		}
	}else{
		if (sim_imsi == "26012")
		{
			document.getElementById('div_contentBg_1').className = "contentBg0_cp_en";
			document.getElementById('div_contentBg_2').className = "contentBg0_cp_en";
			document.getElementById('div_contentBg_3').className = "contentBg0_cp_en";
		} else if (sim_imsi == "26001") {
			document.getElementById('div_contentBg_1').className = "contentBg0_plk_en";
			document.getElementById('div_contentBg_2').className = "contentBg0_plk_en";
			document.getElementById('div_contentBg_3').className = "contentBg0_plk_en";
		} else {
			document.getElementById('div_contentBg_1').className = "contentBg0_new_en";
			document.getElementById('div_contentBg_2').className = "contentBg0_new_en";
			document.getElementById('div_contentBg_3').className = "contentBg0_new_en";
		}
	}
	/*
	if (sim_imsi == "26012")
	{
		document.getElementById('div_contentBg_1').className = "contentBg0_cp";
		document.getElementById('div_contentBg_2').className = "contentBg0_cp";
		document.getElementById('div_contentBg_3').className = "contentBg0_cp";
	} else if (sim_imsi == "26001") {
		document.getElementById('div_contentBg_1').className = "contentBg0_plk";
		document.getElementById('div_contentBg_2').className = "contentBg0_plk";
		document.getElementById('div_contentBg_3').className = "contentBg0_plk";
	} else {
		document.getElementById('div_contentBg_1').className = "contentBg0_new";
		document.getElementById('div_contentBg_2').className = "contentBg0_new";
		document.getElementById('div_contentBg_3').className = "contentBg0_new";
	}
	*/

	setTimeout(function () { upgradeCall(); }, 3000);
	return;
}
</script>
</head>

<body onload="upgradeInit();progressBarCall();">
	<input type="hidden" value="<%getLanguage();%>" id="select_language" />
	<input type="hidden" value="<%getIMSI_mem();%>" id="sim_imsi" />
	<table border="0" cellpadding="0" cellspacing="0" width="100%" height="100%">
		<tr>
			<td align="center" valign="top">
				<div class="w1000">

					<div class="midBg" id="fw_progress">
						<table border="0" cellpadding="0" cellspacing="0">
							<tr>
								<td align="center" valign="top">
							<div id="div_contentBg_1">
									<div class="secH1_new"><script>document.write(gettext("Progress Bar"));</script></div>
									<div class="secBg_xx">
										<div class="secInfo">&nbsp;</div>
										<div align="center">
										<form name="statusForm" method="post" action="">
										</form>
										<table cellspacing="0" class="configTbl">
											<tr>
												<td id="lblStatusMsgProgress">&nbsp;</td>
											</tr>
											<tr>
												<td>&nbsp;</td>
											</tr>
											<tr>
												<td align="center">
													<table id="tblProgress" frame="box" rules="none" border="0" style="border-collapse: collapse" width="80%" height="10" cellpadding="0">
														<tr>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
															<td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td><td name="tblTdProgress" width="1">&nbsp;</td>
														</tr>
													</table>
												</td>
											</tr>
											<tr>
												<td>&nbsp;</td>
											</tr>
											</table>
											</div>
										<div class="secInfo">&nbsp;</div>
									</div>
								</div>
						</td></tr>
						</table>
					</div>
					<div class="midBg" id="fw_progress_msg" style="display:none;">
						<table border="0" cellpadding="0" cellspacing="0">
						<tr><td align="center" valign="top">
							<div id="div_contentBg_2">
								<div class="secH1_new"><script>document.write(gettext("Progress Bar"));</script></div>
									<div class="secBg_xx">
										<div class="secInfo">&nbsp;</div>
										<div align="center">
										<table cellspacing="0" class="configTbl">
											<tr><td>&nbsp;</td></tr>
											<tr align="center">
												<td class="statusMsg" id="FwUpStatusMsg"></td>
											</tr>
										</table>
										</div>
										<div class="secInfo">&nbsp;</div>
									</div>
							</div>
						</td></tr></table>
					</div>
					<div class="midBg" id="fw_progress_reboot" style="display:none;">
						<table border="0" cellpadding="0" cellspacing="0">
							<tr>
								<td align="center" valign="top">
					<div id="div_contentBg_3">
							<div class="secH1_new"><script>document.write(gettext("Status Message"));</script></div>
							<div class="secBg_xx">
								<div class="secInfo">&nbsp;</div>
								<div align="center">
								<table cellspacing="0" class="configTbl">
									<tr>
										<td>&nbsp;</td>
									</tr>
									<tr align="center">
										<td class="statusMsg" id="lblStatusMsg"><script>document.write(gettext("Router will be up in 150 seconds"));</script></td>
									</tr>
									<tr align="center">
										<td class="statusMsg"><script>document.write(gettext("After reboot, Please clean all the history records then open browser."));</script></td>
									</tr>
									<tr>
										<td>&nbsp;</td>
									</tr>
									</table>
									</div>
								<div class="secInfo">&nbsp;</div>
							</div>
						</div>
						</td></tr>
						</table>
					</div>
				</div>
			</td>
		</tr>
	</table>
<script language="javascript" type="text/javascript">
function upgradeCall()
{
	//Check image:tmp/og4610.img and upgrade
	var ajax = createAjax();
	ajax.open('GET', "get_fwup_info.asp?time="+ new Date(), true);
 	ajax.send(null);
	ajax.onreadystatechange = function ()
	{	
  		if (ajax.readyState == 4){
   			if (ajax.status == 200){
			  var result = ajax.responseText;
   	          if (result == 200) {
   	            //document.getElementById('fw_progress_reboot').style.display = "none";
			  	//document.getElementById('fw_progress_msg').style.display = "none";
				//document.getElementById('fw_progress').style.display = "block";
   	            //document.getElementById("FwUpStatusMsg").innerHTML="";
   	          } else if (result == 201) { //Error: Incorrect FW
   	           	document.getElementById('fw_progress_reboot').style.display = "none";
			  	document.getElementById('fw_progress_msg').style.display = "block";
				document.getElementById('fw_progress').style.display = "none";
   	            document.getElementById("FwUpStatusMsg").innerHTML=gettext("Error: Incorrect FW.");
   	          } else if (result == 202) {
   	            document.getElementById('fw_progress_reboot').style.display = "none";
			  	document.getElementById('fw_progress_msg').style.display = "block";
				document.getElementById('fw_progress').style.display = "none";
   	            document.getElementById("FwUpStatusMsg").innerHTML=gettext("Warning: Please check firmware file.");
   	          } 
      		}
  		}
 	}
}
</script>
</body>
</html>
