var branch = "dora";

var global_ussdsendTimeoutHandler = false;
var global_ussdreleaseTimeoutHandler = false;
var global_ussdstatus = "";
var global_ussdtype = "";
var global_ussddata = "";

function isDebugMode()
{	
   return false;
}

function doRedirect()
{
    window.location = document.location.href;
}

function redirectToPage(url)
{
	window.location = url;
}

function textarea_maxlen_isMax( text )
{
	var textarea = document.getElementById(text);
	var max_length = textarea.maxLength;
	if(textarea.value.length > max_length)
	{
  		textarea.value = textarea.value.substring(0, max_length);
	}	
}

function textarea_maxlen_disabledRightMouse ()
{
	document.oncontextmenu = function ()
	{
		return false; 
	}	
}

function textarea_maxlen_enabledRightMouse()
{
	document.oncontextmenu = null;
}

function checkMaxLength(description_val, max_val)
{
	if(document.getElementById(description_val).value.length > max_val)
	{
		document.getElementById(description_val).value = document.getElementById(description_val).value.substr(0, max_val);
		return false;
	}
	return true;
}

function SendJsonAction(action, senddata, onsuccess, onerror)
{
  var targetUrl = isDebugMode() ? ('/testdata/'+action+'.json') : '/cgi-bin/gui.cgi' ;

  $.ajax({
    type: 'POST',
    url: targetUrl + '?_=' + Math.random(),
    contentType: 'json',
    dataType: 'json',
    data: $.toJSON(senddata),
    beforeSend : function () 
    {
      blockUI();
    },
    error: function(xhr, textStatus, errorThrown) {
      unblockUI();
      if(onerror) { onerror(); }
    },
    success: function(msg, textStatus, xhr) 
    {
      if (xhr.status) 
      {
        unblockUI();
        if(onsuccess) { onsuccess(msg); }
      }
    }
  });
  targetUrl = null;
}

function ussdValidation()
{
	var ussd_code =$("#ussdCode").val();

	if (ussd_code == ""){		
		alert(gettext("Please input USSD code."));		
		return false;	
	}
	
	if(ussd_code.charAt(0) == "*" && ussd_code.charAt(ussd_code.length-1) == "#")
	{
		ussd_stop();
	}

	$("#ussdWaitStatus").html(gettext("Please wait..."));
	$("#ussdStopBtn").show();
	applicationApi_ussd_send(ussd_code);
}

function application_set_ussd_send_CB(msg) {
	if(msg.set_ussd_start_async.errno == 0){
		global_ussddata = "";
		global_ussdtype = "";
		global_ussdsendTimeoutHandler = setInterval(function(){ application_ussd_loop()},3000); 

	} 
}

function application_set_ussd_answer_CB(msg) {
	if(msg.set_ussd_answer_async.errno == 0){
		global_ussddata = "";
		global_ussdtype = "";
		global_ussdsendTimeoutHandler = setInterval(function(){ application_ussd_loop()},3000); 
	}
}

function application_get_ussd_state_CB(msg) {
	if(msg.get_ussd_nw_data.errno == 0) {
		global_ussdstatus = msg.get_ussd_nw_data.status;
		if(global_ussdstatus == 2) {
			global_ussdtype = msg.get_ussd_nw_data.type;
			global_ussddata= msg.get_ussd_nw_data.data;
		}
		var str = global_ussddata;
		var find = String.fromCharCode(92)+String.fromCharCode(110);
		var re = new RegExp(find, 'g');
		str = str.replace(re, '<br>');
		if( !$('#response_result').html(str).length ) {
			$("#ussdWaitStatus").html(gettext("Please wait..."));
			$("#response_result").html("");
		}
		else
		{
			$("#ussdWaitStatus").html("");
			$("#ussdStopBtn").hide();
		}
		$("#response_result").html(str);

		if (global_ussdstatus == 2) {
			$("#ussdWaitStatus").html("");
			$("#btnClose").removeAttr("disabled");
			$("#ussdStopBtn").hide();
			$("#ussdCode").val("");
			clearInterval(global_ussdsendTimeoutHandler);
		}else if(global_ussdstatus == 4) {
			alert(gettext("USSD session is released or invalid USSD code."));
			$("#ussdCode").val("");
			doRedirect();
			//global_ussddata = "";
			//global_ussdtype = "";
			//ussd_stop();
			//$("#response_result").html("");
		}

		/*
		if(global_ussdstatus == 2) {
			global_ussdtype = msg.get_ussd_nw_data.type;
			global_ussddata= msg.get_ussd_nw_data.data;
			var str = global_ussddata;
			var find = String.fromCharCode(92)+String.fromCharCode(110);
			var re = new RegExp(find, 'g');
			str = str.replace(re, '<br>');
			$("#response_result").html(str);
			$("#ussdWaitStatus").html("");
			$("#ussdStopBtn").hide();
			clearInterval(global_ussdsendTimeoutHandler);
			
		}else{
			if(global_ussdstatus != 4){
				$("#ussdWaitStatus").html(gettext("Please wait..."));
			}
			$("#response_result").html("");
			global_ussddata = "";
			global_ussdtype = "";
		}
		*/
		
	}

}

function application_ussd_get() {
	SendJsonAction("get_ussd_nw_data", {"action":"get_ussd_nw_data","args":{}}, application_get_ussd_state_CB, null);
}

function application_ussd_loop() {
	application_ussd_get();
	/*
	if (global_ussdstatus == 2) {
		$("#ussdWaitStatus").html("");
		$("#btnClose").removeAttr("disabled");
		$("#ussdStopBtn").hide();
		clearInterval(global_ussdsendTimeoutHandler);
	}
	*/
}

function application_get_ussd_release_state_CB(msg) {
	if(msg.get_ussd_nw_data.errno == 0) {
 		if(msg.get_ussd_nw_data.status == 4) {
			alert(gettext("USSD session is released or invalid USSD code."));
			$("#ussdCode").val("");
			//global_ussddata = "";
			//global_ussdtype = "";
			//ussd_stop();
			doRedirect();
 		}else if(msg.get_ussd_nw_data.status == 2) {
			global_ussdstatus = msg.get_ussd_nw_data.status;
			global_ussdtype = msg.get_ussd_nw_data.type;
			global_ussddata= msg.get_ussd_nw_data.data;
			var str = global_ussddata;
			var find = String.fromCharCode(92)+String.fromCharCode(110);
			var re = new RegExp(find, 'g');
			str = str.replace(re, '<br>');
			$("#ussdWaitStatus").html("");
			$("#ussdStopBtn").hide();
			$("#response_result").html(str);
			$("#btnClose").removeAttr("disabled");
			$("#ussdCode").val("");
			clearInterval(global_ussdsendTimeoutHandler);
 		}
 	}
}

function application_ussd_release_loop() {
	SendJsonAction("get_ussd_nw_data", {"action":"get_ussd_nw_data","args":{}}, application_get_ussd_release_state_CB, null);
}

function applicationApi_ussd_send(code) {
	clearInterval(global_ussdsendTimeoutHandler);
	$("#response_result").html("");
	if(global_ussdtype ==2 )
		SendJsonAction("set_ussd_answer_async", {"action":"set_ussd_answer_async","args":{"data":code}}, application_set_ussd_answer_CB, null);
	else
		SendJsonAction("set_ussd_start_async", {"action":"set_ussd_start_async","args":{"data":code}}, application_set_ussd_send_CB, null);

	global_ussddata = "";
	global_ussdtype = "";
	global_ussdreleaseTimeoutHandler = setInterval(function(){ application_ussd_release_loop()},10000);
}

function ussd_stop() {
	
	$("#ussdWaitStatus").html("");


	$("#ussdSendBtn").removeAttr("disabled");
	$("#ussdStopBtn").hide();
	clearInterval(global_ussdsendTimeoutHandler);
	clearInterval(global_ussdreleaseTimeoutHandler);
	applicationApi_ussd_cancel();
}

function application_set_ussd_cancel_CB(msg)
{
  if(msg.set_ussd_reset.errno == 0){

	$("#response_result").html("");
  } 
}

function applicationApi_ussd_cancel() {
			
	SendJsonAction("set_ussd_reset", {"action":"set_ussd_reset","args":{}}, application_set_ussd_cancel_CB, null);
}

function USSD_Onload(){
	
	ussd_stop();
	//global_ussdreleaseTimeoutHandler = setInterval(function(){ application_ussd_release_loop()},10000);
}

$(document).ready(function()
{
	USSD_Onload();
	
});

