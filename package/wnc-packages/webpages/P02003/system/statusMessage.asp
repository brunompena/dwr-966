<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="JavaScript" type="text/javascript">
var count=120;
var counter=setInterval(displayRebootMsg, 1000);
function displayRebootMsg()
{
	var chart = document.getElementById("lblStatusMsg");
	if(count<=1)
		chart.innerHTML = gettext('Router will be up in') + ' ' + count + ' ' + gettext('second');
	else
		chart.innerHTML = gettext('Router will be up in') + ' ' + count + ' ' + gettext('seconds');
	count=count-1;
	if (count <= 0)
	{
		clearInterval(counter);
		window.location.href="../login.asp";
		return;
	}
}

function pageInit()
{
	var imsi = "<%getIMSI_mem();%>";
	var language = "<%getLanguage();%>";
	if (language == "pl"){
		if (imsi == "26012")
		{
			document.getElementById('div_contentBg').className = "contentBg0_cp_pl";
		} else if (imsi == "26001") {
			document.getElementById('div_contentBg').className = "contentBg0_plk_pl";
		} else {
			document.getElementById('div_contentBg').className = "contentBg0_new_pl";
		}
	}else{
		if (imsi == "26012")
		{
			document.getElementById('div_contentBg').className = "contentBg0_cp_en";
		} else if (imsi == "26001") {
			document.getElementById('div_contentBg').className = "contentBg0_plk_en";
		} else {
			document.getElementById('div_contentBg').className = "contentBg0_new_en";
		}
	}
	/*
	if (imsi == "26012")
	{
		document.getElementById('div_contentBg').className = "contentBg0_cp";
	} else if (imsi == "26001") {
		document.getElementById('div_contentBg').className = "contentBg0_plk";
	} else {
		document.getElementById('div_contentBg').className = "contentBg0_new";
	}
	*/
}
</script>
</head>
<body onload="pageInit();">
	<input type="hidden" value="<%getLanguage();%>" id="select_language" />
	<div align="center">
		<table width="100%" height="100%">
			<tr>
				<td align="center" valign="top">
				<div class="w1000">
					<table>
						<tr>
							<td align="center" valign="top">
							<div id="div_contentBg">
								<div class="secH1_new"><script>document.write(gettext("Status Message"));</script></div>
								<div class="secBg_new">
									<div class="secInfo">&nbsp;</div>
										<div align="center">
											<table class="configTbl">
												<tr>
													<td>&nbsp;</td>
												</tr>
												<tr>
													<td>&nbsp;</td>
												</tr>
												<tr>
													<td class="statusMsg" id="lblStatusMsg"><script>document.write(gettext("Router will be up in 120 seconds"));</script></td>
												</tr>
												<tr>
													<td>&nbsp;</td>
												</tr>
											</table>
										</div>
										<div class="secInfo">&nbsp;</div>
									</div>
								</div>
							</td>
						</tr>
					</table>
				</div>
				</td>
			</tr>
		</table>
	</div>
</body>
</html>
