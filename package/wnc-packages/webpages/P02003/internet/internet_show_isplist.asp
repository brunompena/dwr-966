<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
 <title>D-Link DWR-966</title>
 <meta http-equiv="Pragma" content="no-cache">
 <meta http-equiv="Cache-Control" content="no-cache">
 <meta http-equiv="Expires" content="0">
 <meta http-equiv="Content-Language" content="en-us" />
 <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
 <link rel="stylesheet" href="../style/all.css" type="text/css" />
 <script language="javascript" src="../js/gettext.js" type="text/javascript"></script>
 <script language="javascript" src="../js/msg.js" type="text/javascript"></script>
 <script language="Javascript" src="../js/mgmt.js" type="text/javascript"></script>
</head>

<body style="background-color:transparent">
 <input type="hidden" value="<%getLanguage();%>" id="select_language" />
 <form action="/goform/setNetworkProvider?<%getUniName();%>=<%getUniId();%>" method="post">
 <%showNetworkProvider();%>

 <div id="show_reg_manual" style="display:block;">
  <input type="submit" class="submit" name="applySelType" id="selModeManual" value="Apply" onmouseover="this.className = 'submitHover'" onmouseout="this.className = 'submit'" />
 </div>
 </form>

 <script language="javascript" type="text/javascript">
  if (document.getElementById('no_network_provider')) {
    document.getElementById('show_reg_manual').style.display = "none";
  }
  document.getElementById('selModeManual').value = gettext("Apply");
 </script>

</body>
</html>
