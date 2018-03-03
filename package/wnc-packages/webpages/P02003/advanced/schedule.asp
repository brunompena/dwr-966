<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script language="javascript" src="../js/textValidations.js" type="text/javascript"></script>
<script language="javascript" src="../js/schedule.js" type="text/javascript"></script>
<script language="javascript" src="../js/mgmt.js" type="text/javascript"></script>
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

function pageLoad()
{
	var ScheduleConfig = <%getSchedule();%>;
	var ScheduleConfigArr;
	if (ScheduleConfig!="")
	{
		ScheduleConfigArr = ScheduleConfig.split("#");
		document.getElementById("txtscheduleName").value=ScheduleConfigArr[0];
		document.getElementById("txtscheduleName").disabled = true;
		document.getElementById("selTimeofDay").value=ScheduleConfigArr[1];
		document.getElementById("selDayofWeek").value=ScheduleConfigArr[9];
		document.getElementById("txtSchedStartTimeHrs").value=ScheduleConfigArr[2];
		document.getElementById("txtSchedStartTimeMns").value=ScheduleConfigArr[3];
		document.getElementById("selStartMeridian").value=parseInt(ScheduleConfigArr[4],10)?"PM":"AM";
		document.getElementById("txtSchedEndTimeHrs").value=ScheduleConfigArr[5];
		document.getElementById("txtSchedEndTimeMns").value=ScheduleConfigArr[6];
		document.getElementById("selEndMeridian").value=parseInt(ScheduleConfigArr[7],10)?"PM":"AM";			

		var selDayOfWeek = document.getElementById("selDayofWeek").value;
		var days=parseInt(ScheduleConfigArr[8],10);			
		var chkSun=Math.floor(days/64);
		var chkSat=Math.floor(days%64/32);
		var chkFri=Math.floor(days%64%32/16);
		var chkThu=Math.floor(days%64%32%16/8);
		var chkWed=Math.floor(days%64%32%16%8/4);
		var chkTue=Math.floor(days%64%32%16%8%4/2);
		var chkMon=Math.floor(days%64%32%16%8%4%2);
		if (selDayOfWeek=="3") //custom
		{
			document.getElementById("chkSun").checked=chkSun?true:false;
			document.getElementById("chkSat").checked=chkSat?true:false;
			document.getElementById("chkFri").checked=chkFri?true:false;
			document.getElementById("chkThu").checked=chkThu?true:false;
			document.getElementById("chkWed").checked=chkWed?true:false;
			document.getElementById("chkTue").checked=chkTue?true:false;
			document.getElementById("chkMon").checked=chkMon?true:false;						
		}
	}
	else
	{
		document.getElementById("txtscheduleName").value="";
		document.getElementById("selTimeofDay").value="0";
		document.getElementById("selDayofWeek").value="0";
		document.getElementById("txtscheduleName").disabled = false;
	}
}

function pageValidate()
{
	if(!CheckLoginInfo())
		return false;
	
	var i18n_enterValidSch = gettext("Please enter valid Schedule Name.");
	var txtFieldIdArr = new Array ();
	txtFieldIdArr[0] = "txtscheduleName,"+i18n_enterValidSch;
	if (txtFieldArrayCheck (txtFieldIdArr) == false)
		return false;
	//if (isProblemCharArrayCheck (txtFieldIdArr) == false)
	//	return false;
 	if (checkCommonNameField('txtscheduleName', gettext("Schedule Name")) == false) {
		return false;
 	}

	if (specificDaySelectionCheck () == false)
		return false;

	/* Start : Time Validation */
	var selValue= comboSelectedValueGet ('selTimeofDay');
	if (!selValue) return;
	if (parseInt(selValue, 10) == 5)
	{
	   	if (dateValidate ('txtSchedStartTimeHrs', 'txtSchedStartTimeMns','date1Err') == false)
			return false;
		if (dateValidate ('txtSchedEndTimeHrs', 'txtSchedEndTimeMns','date2Err') == false)
			return false;
		if (!timeValidate())
			return false;
   	}

	if (timeValidate()==false)
		return false;
   	/* End : Time Validation */
	  return true;
}

</script>

</head>	

<body onload="pageLoad(); scheduleInit(); chkSelOptTmDay(); chkSelOptDayWeek();">
<input type="hidden" value="<%getLanguage();%>" id="select_language" />
<div id="all">

<!-- Main Menu and Submenu Start -->
<%writeMainMenu();%>
<%writeLeftMenu("adv");%>
<script type="text/javascript">menuChange("adv_menu");leftSubMenuChange("firewall_submenu","firewall_submenu_focus","rules_for_schedule","rules_for_schedule_href");</script>
<!-- Main Menu and Submenu End -->

<!-- Right Content start -->
<div class="contentBg">
	<div class="secH1"><script>document.write(gettext("Schedules"));</script></div>
	<!-- Section Begin -->
	<div class="secBg">
		<div class="statusMsg"></div>
		<form name="frmSchedule" method="post" action="/goform/setSchedule?<%getUniName();%>=<%getUniId();%>">
		<div class="secH2"><script>document.write(gettext("Schedule Configuration"));</script></div>
		
		<div class="secInfo">
			<br><script>document.write(gettext("Schedules define the time frames for security policies, parental control, and other system features."));</script>
			<br>
			<a class="secLable1" href="rules_for_schedule.asp">>> <script>document.write(gettext("Back to Rules for Schedule page"));</script></a>
		</div>
		<table cellspacing="0" class="configTbl">
			<tr>
				<td><script>document.write(gettext("Schedule Name"));</script></td>
				<td>
					<input type="text" name="name" class="configF1" value="" id="txtscheduleName" size="20" maxlength="32">
				</td>
				<td></td>
			</tr>
		</table>
		<div class="secH2"><script>document.write(gettext("Time of Day"));</script></div>
		
		<div class="secInfo">
			<br><script>document.write(gettext("If desired you can specify the specific time duration for this schedule."));</script>
			<br>
		</div>
		<table cellspacing="0" class="configTbl">
			<tr>
				<td colspan="3"><script>document.write(gettext("Do you want this schedule to be active all day or at specific times during the day?"));</script></td>
			</tr>
			<tr>
				<td><script>document.write(gettext("Time of Day"));</script></td>
				<td>
				<select name="time_of_day.value" size="1" id="selTimeofDay" class="configF1" onchange="chkSelOptTmDay();">
					<!-- Given Security list -->
					<option  value="0"><script>document.write(gettext("Morning (5 AM - 12 AM )"));</script></option>
					<option  value="1"><script>document.write(gettext("Afternoon (12 AM - 4 PM )"));</script></option>
					<option  value="2"><script>document.write(gettext("Evenings(4 PM - 7 PM)"));</script></option>
					<option  value="3"><script>document.write(gettext("Nights (7 PM - 12 PM)"));</script></option>
					<option  value="4"><script>document.write(gettext("Entire Day"));</script></option>
					<option  value="5"><script>document.write(gettext("Custom"));</script></option>
					<!-- Ending Lua loop -->
				</select> 
				</td>
				<td></td>
			</tr>
			<tr>
				<td><script>document.write(gettext("Start Time"));</script></td>
				<td>
				<input type="text" name="time_of_day.start_hr" class="configF1" id="txtSchedStartTimeHrs" value="" size="6" maxlength="2" onkeypress="return numericValueCheck (event)" onkeydown="if (event.keyCode == 9) {return numericValueRangeCheck (this, '', '', 1, 12, true, gettext('Invalid Start time.'), gettext('hours'));}"> 
				<span class="spanText"><script>document.write(gettext("Hour"));</script> </span>
				<input type="text" name="time_of_day.start_min" class="configF1" id="txtSchedStartTimeMns" value=""  size="6" maxlength="2" onkeypress="return numericValueCheck (event)" onkeydown="if (event.keyCode == 9) {return numericValueRangeCheck (this, '', '', 0, 59, true, gettext('Invalid Start time.'), gettext('minutes'));}"> 
				<span class="spanText"><script>document.write(gettext("Minute"));</script> </span>
				<select size="1" name="time_of_day.start_am_pm" class="configF1" id="selStartMeridian">
				<option  value="AM">AM</option>
				<option  value="PM">PM</option>
				</select></td>
				<td></td>
			</tr>
			<tr>
				<td><script>document.write(gettext("End Time"));</script></td>
				<td>
				<input type="text" name="time_of_day.end_hr" value="" class="configF1" id="txtSchedEndTimeHrs" value size="6" maxlength="2" onkeypress="return numericValueCheck (event)" onkeydown="if (event.keyCode == 9) {return numericValueRangeCheck (this, '', '', 1, 12, true, gettext('Invalid End time.'), gettext('hours'));}"> 
				<span class="spanText"><script>document.write(gettext("Hour"));</script> </span>
				<input type="text" name="time_of_day.end_min" class="configF1" value="" id="txtSchedEndTimeMns" value size="6" maxlength="2" onkeypress="return numericValueCheck (event)" onkeydown="if (event.keyCode == 9) {return numericValueRangeCheck (this, '', '', 0, 59, true, gettext('Invalid End time.'), gettext('minutes'));}"> 
				<span class="spanText"><script>document.write(gettext("Minute"));</script> </span>
				<select size="1" name="time_of_day.end_am_pm" class="configF1" id="selEndMeridian">
				<option value="AM">AM</option>
				<option value="PM">PM</option>
				</select></td>
				<td></td>
			</tr>
		</table>
		<div class="secH2"><script>document.write(gettext("Days of Week"));</script></div>
		
		<div class="secInfo">
			<br><script>document.write(gettext("Schedules define the time frames for security policies, parental control, and other system features."));</script>
			<br>
		</div>		
		<table cellspacing="0" class="configTbl">
			<tr>
				<td colspan="3"><script>document.write(gettext("Do you want this schedule to be active on all days or specific days?"));</script></td>
			</tr>
			<tr>
				<td><script>document.write(gettext("Days of Week"));</script></td>
				<td>
				<select name="day_of_week.value" size="1" class="configF1" id="selDayofWeek" onchange="chkSelOptDayWeek ();">
				<!-- Given Security list , , , -->
				<option  value="0"><script>document.write(gettext("Entire Week"));</script></option>
				<option  value="1"><script>document.write(gettext("Weekends"));</script></option>
				<option  value="2"><script>document.write(gettext("Week Days"));</script></option>
				<option  value="3"><script>document.write(gettext("Custom"));</script></option>
				<!-- Ending Lua loop -->
				</select> 
				</td>
				<td></td>
			</tr>
			<tr>
				<td colspan="3">
				<table border="0" cellpadding="0" cellspacing="0" width="100%" id="tblScheduleDays">
					<tr>
						<td>
						<input type="checkbox" name="day_of_week.sunday"  value="1" id="chkSun">
						<input type="hidden" name="day_of_week.sunday" value="0">
						</td>
						<td><script>document.write(gettext("Sunday"));</script></td>
						<td>
						<input type="checkbox" name="day_of_week.monday"  value="1" id="chkMon">
						<input type="hidden" name="day_of_week.monday" value="0">
						</td>
						<td><script>document.write(gettext("Monday"));</script></td>
					</tr>
					<tr>
						<td>
						<input type="checkbox" name="day_of_week.tuesday"  value="1" id="chkTue">
						<input type="hidden" name="day_of_week.tuesday" value="0">
						</td>
						<td><script>document.write(gettext("Tuesday"));</script></td>
						<td>
						<input type="checkbox" name="day_of_week.wednesday"  value="1" id="chkWed">
						<input type="hidden" name="day_of_week.wednesday" value="0">
						</td>
						<td><script>document.write(gettext("Wednesday"));</script></td>
					</tr>
					<tr>
						<td>
						<input type="checkbox" name="day_of_week.thursday"  value="1" id="chkThu">
						<input type="hidden" name="day_of_week.thursday" value="0">
						</td>
						<td><script>document.write(gettext("Thursday"));</script></td>
						<td>
						<input type="checkbox" name="day_of_week.friday"  value="1" id="chkFri">
						<input type="hidden" name="day_of_week.friday" value="0">
						</td>
						<td><script>document.write(gettext("Friday"));</script></td>
					</tr>
					<tr>
						<td>
						<input type="checkbox" name="day_of_week.saturday"  value="1" id="chkSat">
						<input type="hidden" name="day_of_week.saturday" value="0">
						</td>
						<td><script>document.write(gettext("Saturday"));</script></td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
				</table>
			</td>
				</tr>
		</table>
		<div class="submitBg">
			<input type="submit" id="schedule_apply" value="Apply" class="submit" title="Apply" onclick="return pageValidate(); " onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'">
			<input type="button" id="schedule_reset" value="Reset" class="submit" title="Reset" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'"  onclick="scheduleInit(); chkSelOptTmDay(); chkSelOptDayWeek(); doRedirect();">
		</div>
		</form>
	</div>
	<!-- Section End -->
</div>
<!-- contentBg End -->
</div>
<script type="text/javascript">
	document.getElementById('schedule_apply').value=gettext("Apply");
	document.getElementById('schedule_reset').value=gettext("Reset");
</script>
</body>
</html>
