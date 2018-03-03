#create radvd config files
RADVD_CONFIG_FILE=/tmp/radvd.conf
PREFIX_TMP_FILE=/tmp/prefix.tmp

create_6to4prefix() {
    if [ -n ${PREFIX_FROM_6to4} ]; then
        echo     "    prefix ${PREFIX_FROM_6to4}:1::1/64 {"  >> $PREFIX_TMP_FILE
        echo     "        AdvOnLink        off;" >> $PREFIX_TMP_FILE
        if [ "${LAN_V6_TYPE}" = "stateful" ]; then
            echo "        AdvAutonomous    off;" >> $PREFIX_TMP_FILE
        else
            echo "        AdvAutonomous    on;" >> $PREFIX_TMP_FILE
        fi
        echo     "        AdvRouterAddr    on;" >> $PREFIX_TMP_FILE
        echo     "        AdvValidLifetime 40;" >> $PREFIX_TMP_FILE
        echo     "        AdvPreferredLifetime 30;" >> $PREFIX_TMP_FILE
        echo     "        Base6to4Interface $IF6TO4;" >> $PREFIX_TMP_FILE
        echo     "    };" >> $PREFIX_TMP_FILE
    fi
}

create_lan_prefix() {
    if [ "${PREFIX_FROM_LAN}" != "" ]; then
        echo     "    prefix $PREFIX_FROM_LAN {"  >> $PREFIX_TMP_FILE
        echo     "        AdvOnLink        on;" >> $PREFIX_TMP_FILE
        if [ "${LAN_V6_TYPE}" = "stateful" ]; then
            echo "        AdvAutonomous    off;" >> $PREFIX_TMP_FILE
        else
            echo "        AdvAutonomous    on;" >> $PREFIX_TMP_FILE
        fi
        echo     "        AdvRouterAddr    off;" >> $PREFIX_TMP_FILE
        if [ "${LAN_DHCPPD_VTIME}" != "" ]; then
            echo "        AdvValidLifetime $LAN_DHCPPD_VTIME;" >> $PREFIX_TMP_FILE
        fi
        if [ "${LAN_DHCPPD_PTIME}" != "" ]; then
           echo "        AdvPreferredLifetime $LAN_DHCPPD_PTIME;" >> $PREFIX_TMP_FILE
        fi
        echo     "    };" >> $PREFIX_TMP_FILE
    fi
}

create_wan_prefix() {
    if [ "${PREFIX_FROM_WAN}" != "" ]; then
        echo     "    prefix $PREFIX_FROM_WAN {"  >> $PREFIX_TMP_FILE
		echo     "        DeprecatePrefix        on;" >> $PREFIX_TMP_FILE
        echo     "        AdvOnLink        off;" >> $PREFIX_TMP_FILE
        if [ "${LAN_V6_TYPE}" = "stateful" ]; then
            echo "        AdvAutonomous    off;" >> $PREFIX_TMP_FILE
        else
            echo "        AdvAutonomous    on;" >> $PREFIX_TMP_FILE
        fi
        echo     "        AdvRouterAddr    off;" >> $PREFIX_TMP_FILE
        if [ "${DHCPPD_VTIME}" != "" ]; then
            echo "        AdvValidLifetime $DHCPPD_VTIME;" >> $PREFIX_TMP_FILE
        else
            echo "        AdvValidLifetime 40;" >> $PREFIX_TMP_FILE
        fi
        if [ "${DHCPPD_PTIME}" != "" ]; then
           echo "        AdvPreferredLifetime $DHCPPD_PTIME;" >> $PREFIX_TMP_FILE
        else
           echo "        AdvPreferredLifetime 30;" >> $PREFIX_TMP_FILE
        fi
        echo     "    };" >> $PREFIX_TMP_FILE
		echo     "    route ::/0 {"  >> $PREFIX_TMP_FILE
		echo     "        AdvRoutePreference        high;" >> $PREFIX_TMP_FILE
		echo     "        RemoveRoute        on;" >> $PREFIX_TMP_FILE
		echo     "    };" >> $PREFIX_TMP_FILE
	else
		echo     "    AdvDefaultLifetime    0;" >> $RADVD_CONFIG_FILE
    fi
}

create_radvd_conf() {
    rm -f $PREFIX_TMP_FILE

    echo     "interface br0 {" > $RADVD_CONFIG_FILE
    echo     "    AdvSendAdvert        on;" >> $RADVD_CONFIG_FILE
    echo     "    UnicastOnly          off;" >> $RADVD_CONFIG_FILE
    echo     "    AdvHomeAgentFlag     off;" >> $RADVD_CONFIG_FILE
    echo     "    AdvSourceLLAddress   on;" >> $RADVD_CONFIG_FILE
    echo     "    MinDelayBetweenRAs   3;" >> $RADVD_CONFIG_FILE
    echo     "    AdvReachableTime     0;" >> $RADVD_CONFIG_FILE
    echo     "    AdvRetransTimer      0;" >> $RADVD_CONFIG_FILE
    echo     "    MaxRtrAdvInterval    15;" >> $RADVD_CONFIG_FILE
    echo     "    MinRtrAdvInterval    5;" >> $RADVD_CONFIG_FILE

    if [ "${LAN_V6_TYPE}" = "stateful" ]; then
        echo "    AdvManagedFlag       on;" >> $RADVD_CONFIG_FILE
    else
        echo "    AdvManagedFlag       off;" >> $RADVD_CONFIG_FILE
    fi

    if [ "${LAN_V6_TYPE}" != "stateless" -a "${PREFIX_FROM_WAN}" != "" ]; then
        echo "    AdvOtherConfigFlag   on;" >> $RADVD_CONFIG_FILE
    else
        echo "    AdvOtherConfigFlag   off;" >> $RADVD_CONFIG_FILE
    fi

    if [ "${IPV6_TYPE}" != "6to4" ]; then
        create_wan_prefix
        create_lan_prefix
    else 
        create_6to4prefix
    fi
    if [ -e "$PREFIX_TMP_FILE" ]; then
        cat $PREFIX_TMP_FILE >> $RADVD_CONFIG_FILE
    fi

    if [ "${LAN_V6_TYPE}" = "stateless" ] ; then
        if [ -n "$SECOND_V6DNS" ]; then
          echo "    RDNSS $PRIMARY_V6DNS $SECOND_V6DNS {" >> $RADVD_CONFIG_FILE
        else
          echo "    RDNSS $PRIMARY_V6DNS {" >> $RADVD_CONFIG_FILE
        fi
        echo "        AdvRDNSSOpen on;" >> $RADVD_CONFIG_FILE
        echo "    };" >> $RADVD_CONFIG_FILE
    fi
    echo     "};" >> $RADVD_CONFIG_FILE
}

run_radvd() {
    create_radvd_conf
    if [ -e "$PREFIX_TMP_FILE" ]; then
		count=`pidof radvd | wc -w`
		if [ $count -ne 0 ]; then
			killall -9 radvd
			sleep 1
		fi
		
        radvd -C $RADVD_CONFIG_FILE &
        echo "restart radvd $PREFIX_FROM_WAN $PREFIX_FROM_LAN" >> /var/log/messages
    fi
}
