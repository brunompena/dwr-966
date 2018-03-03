<div id="left_bg">
  <ul>
   <li class="leftbu" id="advanced_ddns"><a href="#" onclick="RedirectSubMenu('advanced_ddns');" id="advanced_ddns_href"><script>document.write(gettext("DNS & DynDNS"));</script></a></li>
   <li class="leftbu" id="firewall_submenu_focus"><a href="#" onclick="RedirectSubMenu('firewall');" id="firewall_submenu_focus_href"><script>document.write(gettext("Firewall"));</script></a></li>
     <dl style="display:none;" id="firewall_submenu">
      <dd class="ddbu" id="access_control"><a href="#" onclick="RedirectSubMenu('access_control');" id="access_control_href"><script>document.write(gettext("Access Control"));</script></a></dd>
      <dd class="ddbu" id="known_computers"><a href="#" onclick="RedirectSubMenu('known_computers');" id="known_computers_href"><script>document.write(gettext("Known Computers"));</script></a></dd>
      <dd class="ddbu" id="url_blocking"><a href="#" onclick="RedirectSubMenu('url_blocking');" id="url_blocking_href"><script>document.write(gettext("URL Blocking"));</script></a></dd>
      <dd class="ddbu" id="dos_attacks"><a href="#" onclick="RedirectSubMenu('dos_attacks');" id="dos_attacks_href"><script>document.write(gettext("DoS Attacks"));</script></a></dd>
      <dd class="ddbu" id="rules_for_schedule"><a href="#" onclick="RedirectSubMenu('rules_for_schedule');" id="rules_for_schedule_href"><script>document.write(gettext("Rules for Schedule"));</script></a></dd>
     </dl>
   <li class="leftbu" id="QoSPolicy"><a href="#" onclick="RedirectSubMenu('QoSPolicy');" id="QoSPolicy_href"><script>document.write(gettext("QoS Settings"));</script></a></li>
   <li class="leftbu" id="nat_portforwarding"><a href="#" onclick="RedirectSubMenu('nat_portforwarding');" id="nat_portforwarding_href"><script>document.write(gettext("Port Forwarding"));</script></a></li>
   <li class="leftbu" id="nat_submenu_focus"><a href="#" onclick="RedirectSubMenu('nat_general');" id="nat_submenu_focus_href"><script>document.write(gettext("NAT"));</script></a></li>
     <dl style="display:none;" id="nat_submenu">
      <dd class="ddbu" id="nat_dmz"><a href="#" onclick="RedirectSubMenu('nat_dmz');" id="nat_dmz_href"><script>document.write(gettext("Virtual DMZ"));</script></a></dd>
      <dd class="ddbu" id="upnp_port"><a href="#" onclick="RedirectSubMenu('upnp_port');" id="upnp_port_href"><script>document.write(gettext("UPnP Port Mapping"));</script></a></dd>
      <dd class="ddbu" id="special_apps"><a href="#" onclick="RedirectSubMenu('special_apps');" id="special_apps_href"><script>document.write(gettext("Special Applications"));</script></a></dd>
     </dl>
   <li class="leftbu" id="advanced_staticroute"><a href="#" onclick="RedirectSubMenu('advanced_staticroute');" id="advanced_staticroute_href"><script>document.write(gettext("Static Routing"));</script></a></li>
   <li class="leftbu" id="usb_submenu_focus"><a href="#" onclick="RedirectSubMenu('usb_storage');" id="usb_submenu_focus_href"><script>document.write(gettext("USB"));</script></a></li>
     <dl style="display:none;" id="usb_submenu">
      <dd class="ddbu" id="usb_storage"><a href="#" onclick="RedirectSubMenu('usb_storage');" id="usb_storage_href"><script>document.write(gettext("USB Storage"));</script></a></dd>
     </dl>
   <li class="leftbu" id="dlna"><a href="#" onclick="RedirectSubMenu('dlna');" id="dlna_href">DLNA</a></li>
   <li class="leftbu" id="sms_submenu_focus"><a href="#" onclick="RedirectSubMenu('sms_newsms')" id="sms_submenu_focus_href"><script>document.write(gettext("Short Message"));</script></a></li>
   <dl style="display:none;" id="sms_submenu">
      <dd class="ddbu" id="sms_newsms"><a href="#" onclick="RedirectSubMenu('sms_newsms')" id="sms_newsms_href"><script>document.write(gettext("New SMS"));</script></a></dd>
      <dd class="ddbu" id="sms_inbox"><a href="#" onclick="RedirectSubMenu('sms_inbox')" id="sms_inbox_href"><script>document.write(gettext("Inbox"));</script></a></dd>
       <dd class="ddbu" id="sms_outbox"><a href="#" onclick="RedirectSubMenu('sms_outbox')" id="sms_outbox_href"><script>document.write(gettext("Outbox"));</script></a></dd>
       <dd class="ddbu" id="sms_draft"><a href="#" onclick="RedirectSubMenu('sms_draft')" id="sms_draft_href"><script>document.write(gettext("Draftbox"));</script></a></dd>
       <dd class="ddbu" id="sms_simsms"><a href="#" onclick="RedirectSubMenu('sms_simsms')" id="sms_simsms_href"><script>document.write(gettext("SMS on SIM Card"));</script></a></dd>
      <dd class="ddbu" id="sms_setting"><a href="#" onclick="RedirectSubMenu('sms_setting')" id="sms_setting_href"><script>document.write(gettext("SIM Setting"));</script></a></dd>
  </dl>
</li>
   <li class="leftbu" id="dlna"><a href="#" onclick="RedirectSubMenu('ussd');" id="ussd_href"><script>document.write(gettext("USSD"));</script></a></li>

   <li class="leftbu" id="ipv6_submenu_focus"><a href="#" onclick="RedirectSubMenu('ipv6_clientlist');" id="ipv6_submenu_focus_href"><script>document.write(gettext("IPv6"));</script></a></li>
     <dl style="display:none;" id="ipv6_submenu">
      <dd class="ddbu" id="ipv6_clientlist"><a href="#" onclick="RedirectSubMenu('ipv6_clientlist');" id="ipv6_clientlist_href"><script>document.write(gettext("Device List"));</script></a></dd>
      <dd class="ddbu" id="ipv6_setting"><a href="#" onclick="RedirectSubMenu('ipv6');" id="ipv6_setting_href"><script>document.write(gettext("Settings"));</script></a></dd>
      <dd class="ddbu" id="ipv6_routing"><a href="#" onclick="RedirectSubMenu('ipv6_routing');" id="ipv6_routing_href"><script>document.write(gettext("Static Routing"));</script></a></dd>
      <dd class="ddbu" id="ipv6_filtering"><a href="#" onclick="RedirectSubMenu('ipv6_filtering');" id="ipv6_filtering_href"><script>document.write(gettext("Firewall"));</script></a></dd>
     </dl>
   <!-- 
   <li class="leftbu" id="vpn_submenu_focus"><a href="#" onclick="RedirectSubMenu('vpn_passthrough');" id="vpn_submenu_focus_href"><script>document.write(gettext("VPN"));</script></a></li>
     <dl style="display:none;" id="vpn_submenu">
      <dd class="ddbu" id="vpn_passthrough"><a href="#" onclick="RedirectSubMenu('vpn_passthrough');" id="vpn_passthrough_href">&#45;<script>document.write(gettext("Passthrough"));</script></a></dd>
      <dd class="ddbu" id="vpn_profile"><a href="#" onclick="RedirectSubMenu('vpn_profile');" id="vpn_profile_href">&#45;<script>document.write(gettext("Profile"));</script></a></dd>
      <dd class="ddbu" id="vpn_ipsec"><a href="#" onclick="RedirectSubMenu('vpn_ipsec');" id="vpn_ipsec_href">&#45;<script>document.write(gettext("IPSEC"));</script></a></dd>
      <dd class="ddbu" id="vpn_pptp"><a href="#" onclick="RedirectSubMenu('vpn_pptp');" id="vpn_pptp_href">&#45;<script>document.write(gettext("PPTP"));</script></a></dd>
      <dd class="ddbu" id="vpn_l2tp"><a href="#" onclick="RedirectSubMenu('vpn_l2tp');" id="vpn_l2tp_href">&#45;<script>document.write(gettext("L2TP"));</script></a></dd>
     </dl>
    -->     
  </ul>
</div>

