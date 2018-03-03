<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="JavaScript" src="../js/lteProgressBar.js" type="text/javascript"></script>
<script language="javascript" type="text/javascript">
function pageInit()
{
	var sim_imsi = document.getElementById('sim_imsi').value;
	var select_language = document.getElementById('select_language').value;
	if (select_language == "pl"){
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
}
</script>
</head>

<body onload="pageInit();lteProgressBarCall();">
	<input type="hidden" value="<%getLanguage();%>" id="select_language" />
	<input type="hidden" value="<%getIMSI_mem();%>" id="sim_imsi" />
	<table border="0" cellpadding="0" cellspacing="0" width="100%" height="100%">
		<tr>
			<td align="center" valign="top">
				<div class="w1000">
					<div class="midBg" id="upgrade_module" style="display:none;">
						<table border="0" cellpadding="0" cellspacing="0">
							<tr>
								<td align="center" valign="top">
								<div id="div_contentBg_1">
									<div class="secH1_new"><script>document.write(gettext("LTE Module Firmware Upgrade"));</script></div>
									<div class="secBg_xx">
										<div class="secInfo">&nbsp;</div>
										<div align="center">
										<form name="statusForm" method="post" action="">
										<input type="hidden" id="i18n_modelUpgradeFailed" value= "LTE module firmware upgrade failed" />
										</form>
										<table cellspacing="0" class="configTbl">
											<tr><td id="lblStatusMsgLte">&nbsp;</td></tr>
											<tr><td>&nbsp;</td></tr>
											<tr>
												<td align="center">
												<table id="tblProgressLte" frame="box" rules="none" border="0" style="border-collapse: collapse" width="80%" height="10" cellpadding="0">
													<tr>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
														<td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td><td name="tblTdProgressLte" width="1">&nbsp;</td>
													</tr>
												</table>
												</td>
											</tr>
											<tr><td>&nbsp;</td></tr>
										</table>
										</div>
										<div class="secInfo">&nbsp;</div>
									</div>
								</div>
							</td></tr>
						</table>
					</div>
					<div class="midBg" id="module_upgrage_status">
						<table border="0" cellpadding="0" cellspacing="0">
							<tr>
								<td align="center" valign="top">
					            <div id="div_contentBg_2">
							    	<div class="secH1_new"><script>document.write(gettext("LTE Module Firmware Upgrade"));</script></div>
									<div class="secBg_xx">
										<div class="secInfo">&nbsp;</div>
										<div align="center">
										<table cellspacing="0" class="configTbl">
											<tr><td>&nbsp;</td></tr>
											<tr><td class="statusMsg"><script>document.write(gettext("The upgrade process will take a few minutes, please wait patiently."));</script></td></tr>
											<tr><td class="statusMsg" id="mdm_message"><script>document.write(gettext("Starting upgrade LTE module firmware"));</script></td></tr>
											<tr><td class="statusMsg" id="mdm_progressing"><script>document.write(gettext("Progressing"));</script>: 0 %</td></tr>
											<tr><td>&nbsp;</td></tr>
										</table>
										</div>
										<div class="secInfo">&nbsp;</div>
									</div>
								</div>
							</td></tr>
						</table>
					</div>
					<div class="midBg" id="module_reboot" style="display:none;">
						<table border="0" cellpadding="0" cellspacing="0">
							<tr>
								<td align="center" valign="top">
								<div id="div_contentBg_3">
									<div class="secH1_new"><script>document.write(gettext("Status Message"));</script></div>
									<div class="secBg_new">
										<div class="secInfo" style="color:#FF6600">&nbsp;</div>
										<div align="center">
										<form name="statusForm" method="post" action="platform.cgi">
											<input type="hidden" name="thispage" value="statusPage.htm">
											<input type="hidden" name="button.status.index" value="1">
										</form>
										<table cellspacing="0" class="configTbl">
											<tr><td></td></tr>
											<tr><td >&nbsp;</td></tr>
											<tr><td class="statusMsg" id="lblStatusMsg"><script>document.write(gettext("Please wait"));</script>...&nbsp;&nbsp;</td></tr>
											<tr><td >&nbsp;</td></tr>
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
</body>
</html>
