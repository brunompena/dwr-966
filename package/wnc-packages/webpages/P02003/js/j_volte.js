
function isDebugMode()
{	
   return false;
}


function GetVolteInband(msg)
{
	if(typeof(msg)!="undefined")
	{
		if(msg.inband)
		{
			$("#enable_inband").attr("checked", true);
		}
		else
		{
			$("#enable_inband").attr("checked", false);
		}		
	}
}

function GetVoltePlusSymbol(msg)
{
	if(typeof(msg)!="undefined")
	{
		if(msg.plus_symbol)
		{
			$("#enable_plus_symbol").attr("checked", true);
		}
		else
		{
			$("#enable_plus_symbol").attr("checked", false);
		}		
	}
}

function GetVolteCallWaiting(msg)
{
	if(typeof(msg)!="undefined")
	{
		if(msg.call_waiting)
		{
			$("#enable_call_waiting").attr("checked", true);
		}
		else
		{
			$("#enable_call_waiting").attr("checked", false);
		}		
	}
}

function GetVolteDisplayCalledNumber(msg)
{
	if(typeof(msg)!="undefined")
	{
		if(msg.display_caller_number)
		{
			$("#display_caller_number").attr("checked", true);
		}
		else
		{
			$("#display_caller_number").attr("checked", false);
		}		
	}
}

function GetVolteIMSRegStatus(ims_info)
{
	if(typeof(ims_info)!="undefined")
	{
		$("#ims_reg_state").text(ims_info.ims_registered);
		$("#sms_service_status").text(ims_info.sms_service_status);
		$("#voip_service_status").text(ims_info.voip_service_status);
		$("#vt_service_status").text(ims_info.vt_service_status);
		$("#ut_service_status").text(ims_info.ut_service_status);
		$("#sms_service_rat").text(ims_info.sms_service_rat);
		$("#voip_service_rat").text(ims_info.voip_service_rat);
		$("#vt_service_rat").text(ims_info.vt_service_rat);
		$("#ut_service_rat").text(ims_info.ut_service_rat);		
	}

	setTimeout(function () {GetVolteSetting('get_voice_ims_reg_status');}, 10000);
}

function GetVolteDigitMapping(digit_info)
{		
	var digit_mapping = digit_info.digit_mapping;
	
	if(typeof(digit_info)!="undefined")
	{			
		$("#digit_0").val(digit_mapping[0]);
		$("#digit_1").val(digit_mapping[1]);
		$("#digit_2").val(digit_mapping[2]);
		$("#digit_3").val(digit_mapping[3]);
		$("#digit_4").val(digit_mapping[4]);
		$("#digit_5").val(digit_mapping[5]);		
		$("#digit_6").val(digit_mapping[6]);
		$("#digit_7").val(digit_mapping[7]);
		$("#digit_8").val(digit_mapping[8]);
		$("#digit_9").val(digit_mapping[9]);
		$("#digit_star").val(digit_mapping[10]);
		$("#digit_hash").val(digit_mapping[11]);		
		$("#digit_plus").val(digit_mapping[12]);	
	}
	else
	{
		$("#digit_0").val("NA");
		$("#digit_1").val("NA");
		$("#digit_2").val("NA");
		$("#digit_3").val("NA");
		$("#digit_4").val("NA");
		$("#digit_5").val("NA");		
		$("#digit_6").val("NA");
		$("#digit_7").val("NA");
		$("#digit_8").val("NA");
		$("#digit_9").val("NA");
		$("#digit_star").val("NA");
		$("#digit_hash").val("NA");		
		$("#digit_plus").val("NA");	
	}	
}

function GetVolteSetting(item)
{
	var targetUrl = isDebugMode() ? '/testdata/get_application_status.json' : '/cgi-bin/gui.cgi' ;
	var senddata = { "action" : item };

	$.ajax({
		type: 'POST',
		url: targetUrl,
		timeout : 30000, 
		async: false,
		contentType: 'json',
		dataType: 'json',
		data: $.toJSON(senddata),
		beforeSend : function () 
		{
			if(isDebugMode()) { alert("Sending apply JSON: \n" + $.toJSON(senddata)); }			
		},
		error: function(xhr, textStatus, errorThrown) {		
						
		},
		success: function(msg, textStatus, xhr) {
			if (xhr.status) {				
				if (msg == null)
				{					
					alert("Failed to get volte setting.\nResponse is null.");
				}
				else				
				{	
					if (item=="get_voice_inband" && msg.get_voice_inband)
						GetVolteInband(msg.get_voice_inband);	
					else if (item=="get_voice_plus_symbol" && msg.get_voice_plus_symbol)
						GetVoltePlusSymbol(msg.get_voice_plus_symbol);
					else if (item=="get_voice_call_waiting" && msg.get_voice_call_waiting)
						GetVolteCallWaiting(msg.get_voice_call_waiting);
					else if (item=="get_voice_display_caller_number" && msg.get_voice_display_caller_number)
						GetVolteDisplayCalledNumber(msg.get_voice_display_caller_number);
					else if (item=="get_voice_ims_reg_status" && msg.get_voice_ims_reg_status)
						GetVolteIMSRegStatus(msg.get_voice_ims_reg_status);
					else if (item=="get_voice_digit_mapping" && msg.get_voice_digit_mapping)
						GetVolteDigitMapping(msg.get_voice_digit_mapping);
				}									
			}				
		}
	});
	targetUrl = null;
}

function SetVolteSetting(senddata, onsuccess, onerror)
{
	var targetUrl = isDebugMode() ? '/testdata/get_application_status.json' : '/cgi-bin/gui.cgi' ;
	var send_data_obj = eval('(' + $.toJSON(senddata) + ')');
	var item = send_data_obj.action;
	
	$.ajax({
		type: 'POST',
		url: targetUrl,
		contentType: 'json',
		dataType: 'json',
		async: false,
		data: $.toJSON(senddata),
		beforeSend : function () 
		{
			if(isDebugMode()) { alert("Sending apply JSON: \n" + $.toJSON(senddata)); }
		},
		error: function(xhr, textStatus, errorThrown) {										
		},
		success: function(msg, textStatus, xhr) {
			if (xhr.status) {					
				var ok = true;

				if (msg == null)
				{
					ok = false;
					alert("Failed to apply action.\nResponse is null.");
				}
				else
				{
				
					if (item=="set_voice_inband" && typeof(msg.set_voice_inband)!="undefined" && msg.set_voice_inband.errno != 0)
					{
						ok = false;	
						alert(msg.set_voice_inband.errmsg);
					}
					else if (item=="set_voice_plus_symbol" && typeof(msg.set_voice_plus_symbol)!="undefined" && msg.set_voice_plus_symbol.errno != 0)
					{
						ok = false;
						alert(msg.set_voice_plus_symbol.errmsg);
					}
					else if (item=="set_voice_call_waiting" && typeof(msg.set_voice_call_waiting)!="undefined" && msg.set_voice_call_waiting.errno != 0)
					{
						ok = false;
						alert(msg.set_voice_call_waiting.errmsg);
					}
				}
					
				if (ok) 
				{
					if(onsuccess) 
						onsuccess();
			 	}
				if (!ok)
				{
					if(onerror)
						onerror();
				}
				ok = null;				
			}				
		}
	});
	targetUrl = null;
}

function _set_plus_symbol()
{
	SetVolteSetting
	({
		"action":"set_voice_plus_symbol", 
		"args":
		{
			"plus_symbol" : $('#enable_plus_symbol').attr('checked')					
	    }
	}, doRedirect);		
}

$(document).ready(function()
{
	if ($("#get_volte_setting").length > 0) 
	{
		$(function(){GetVolteSetting('get_voice_inband');})	
		$(function(){GetVolteSetting('get_voice_plus_symbol');})
		$(function(){GetVolteSetting('get_voice_call_waiting');})
		$(function(){GetVolteSetting('get_voice_display_caller_number');})
	}	

	/*
	if ($("#get_volte_digit_setting").length > 0)
	{
		$(function(){GetVolteSetting('get_voice_digit_mapping');})
	}
	*/

	if ($("#get_ims_status").length > 0)
	{
		$(function(){GetVolteSetting('get_voice_ims_reg_status');})
	}
	
	$('#set_volte_setting').die();
	$('#set_volte_setting').live('click', function(){	
			
			SetVolteSetting
			({
				"action":"set_voice_inband", 
				"args":
				{
					"inband" : $('#enable_inband').attr('checked')					
			    }
			}, null);	

			SetVolteSetting
			({
				"action":"set_voice_plus_symbol", 
				"args":
				{
					"plus_symbol" : $('#enable_plus_symbol').attr('checked')					
			    }
			}, null);		

			SetVolteSetting
			({
				"action":"set_voice_display_caller_number", 
				"args":
				{
					"display_caller_number" : $('#display_caller_number').attr('checked')					
			    }
			}, null);				

			doRedirect();			
	});

	$('#set_volte_call_waiting').die();
	$('#set_volte_call_waiting').live('click', function(){	
			
			SetVolteSetting
			({
				"action":"set_voice_call_waiting", 
				"args":
				{
					"call_waiting" : $('#enable_call_waiting').attr('checked')					
			    }
			}, doRedirect);		
			
	});	

	
	/*
	$('#set_volte_digit_mapping').die();
	$('#set_volte_digit_mapping').live('click', function(){	
			var digit_mapping = 
				$('#digit_0').val()+$('#digit_1').val()+
				$('#digit_2').val()+$('#digit_3').val()+
				$('#digit_4').val()+$('#digit_5').val()+
				$('#digit_6').val()+$('#digit_7').val()+
				$('#digit_8').val()+$('#digit_9').val()+
				$('#digit_star').val()+$('#digit_hash').val()+
				$('#digit_plus').val();
								
			SetVolteSetting
			({
				"action":"set_voice_digit_mapping", 
				"args":
				{
					"digit_mapping" : digit_mapping
			    }
			}, doRedirect);					
	});	
	*/

});

