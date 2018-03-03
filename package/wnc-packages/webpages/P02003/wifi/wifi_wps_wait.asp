<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>D-Link DWR-966</title>
<meta http-equiv="Content-Language" content="en-us" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" href="../style/all.css" type="text/css" />
<script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
<script language="javascript" src="../js/msg.js" type="text/javascript"></script>
<script lainguage="javascript" type="text/javascript">
var count=20;
var counter;
setTimeout(function () {autoRefresh_progress()},1000);
function autoRefresh_progress()
{
  counter=setInterval(displayRebootMsg, 1000); 
}

function displayRebootMsg()
{
	var chart = document.getElementById("countdown");
	chart.innerHTML = gettext("WPS processing.....") + "<br/>" + gettext("Remaining Time") + ":&nbsp;" + count + "&nbsp;&nbsp;" + gettext("seconds");
    count=count-1;
    if (count <= 0)
    {
       clearInterval(counter);
	   window.location.href="wifi_wps.asp";
       return;
	}
}
</script>
</head>

<body>
  <input type="hidden" value="<%getLanguage();%>" id="select_language" />
  <hr/>
  <div id="fullpagemsg_bg"></div>
  <div id="PercentBar" class="msg_outline" style="margin-top:-50px; ">
      <div id="RebootMsg" class="msg_outline" style="margin-top:-50px; display:block;">
	    <div id="countdown" class="msg_container" style="line-height:62px; text-align:center;">
          <script>document.write(gettext("WPS processing....."));</script><br/>
          <script>document.write(gettext("Remaining Time"));</script>:&nbsp;20&nbsp;&nbsp;
          <script>document.write(gettext("seconds"));</script>
	    </div>
	  </div>
  </div>
</body>
</html>
