DHCP6S_CONFIG_FILE=/tmp/dhcp6s.conf

create_dhcp6s_conf() {
	rm -f $DHCP6S_CONFIG_FILE
	
	#create dhcpv6 config file by the settings
	if [ "$PRIMARY_V6DNS" != "" -o "$SECOND_V6DNS" != "" ]; then
		echo     "option domain-name-servers $PRIMARY_V6DNS $SECOND_V6DNS;" > $DHCP6S_CONFIG_FILE
	fi	
    #echo     "option domain-name \"$DOMAIN_NAME\";" >> $DHCP6S_CONFIG_FILE
    if [ "${LAN_V6_TYPE}" = "stateful" ]; then
        echo "interface br0 {" >> $DHCP6S_CONFIG_FILE
        if [ "${V6DHCP_RAPID}" = "on" ]; then
            echo "    allow rapid-commit;" >> $DHCP6S_CONFIG_FILE
        fi
        if [ ${PREFIX_FROM_WAN} != "" ]; then
	        if [ "${DHCPPD_PTIME}" != "" -a "${DHCPPD_VTIME}" != "" ]; then
	            echo "    address-pool wx55_wan $DHCPPD_PTIME $DHCPPD_VTIME;" >> $DHCP6S_CONFIG_FILE
	        else
	            echo "    address-pool wx55_wan 3600;" >> $DHCP6S_CONFIG_FILE
	        fi
        fi
        if [ ${PREFIX_FROM_LAN} != "" ]; then
	        if [ "${LAN_DHCPPD_PTIME}" != "" -a "${LAN_DHCPPD_VTIME}" != "" ]; then
	            echo "    address-pool wx55_lan $LAN_DHCPPD_PTIME $LAN_DHCPPD_VTIME;" >> $DHCP6S_CONFIG_FILE
	        else
	            echo "    address-pool wx55_lan 30 40;" >> $DHCP6S_CONFIG_FILE
	        fi
        fi
        echo "};" >> $DHCP6S_CONFIG_FILE

        #from wan configuration
        if [ ${PREFIX_FROM_WAN} != "" ]; then
            echo "pool wx55_wan {" >> $DHCP6S_CONFIG_FILE
            echo "    range $DHCP_START_FROM_WAN to $DHCP_END_FROM_WAN ;" >> $DHCP6S_CONFIG_FILE
            echo "};" >> $DHCP6S_CONFIG_FILE
        fi
        #from lan configuration
        if [ ${PREFIX_FROM_LAN} != "" ]; then
            echo "pool wx55_lan {" >> $DHCP6S_CONFIG_FILE
            echo "    range $DHCP_START_FROM_LAN to $DHCP_END_FROM_LAN ;" >> $DHCP6S_CONFIG_FILE
            echo "};" >> $DHCP6S_CONFIG_FILE
        fi
    fi
}

run_dhcpv6s() {
    create_dhcp6s_conf
	
	# for dhcp6-duid where store in /var/db/
	if [ ! -d /var/db/ ]; then
		mkdir -p /var/db/
	fi	
		
	if [ -e "$DHCP6S_CONFIG_FILE" ]; then
		dhcp6s br0 -c $DHCP6S_CONFIG_FILE &
		echo "restart dhcp6s $PREFIX_FROM_WAN $PREFIX_FROM_LAN" >> /var/log/messages
	fi
}

