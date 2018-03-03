-- Tables used for LTE component.
--
-- This table stores the configuration related to a LTE WAN connection.
--
--BEGIN;
CREATE TABLE ltePinStatus
    (
    pinId                  Integer, -- 1-PIN1
                                    -- 0-PIN2
    pinStatus              Integer, -- 0 : PIN not initialized
                                        -- 1 : PIN enabled, not verified
                                        -- 2 : PIN enabled, verified
                                        -- 3 : PIN disabled
                                        -- 4 : PIN blocked
                                        -- 5 : PIN permanently blocked
    pinVerifyRetryLeft     Integer, -- pin 1 verify retry count
    pinUnblockRetryLeft    Integer, -- pin 1 unblock retry count
    pinValue                Text     -- pin value given by user
    --PRIMARY KEY (pinId)
    );

CREATE TABLE lteNwProfile
    (
    id                     Integer NOT NULL,
    name                   Text,
    PDPType                Integer,
    APNName                Text,
    Username               Text,
    Password               Text,
    Authentication         Integer,
    interfaceName           Text,  -- usb0 or usb1

    PRIMARY KEY (id)        -- primary key
    );

CREATE TABLE lteHwInfo
    (
    id                     Integer NOT NULL,
    ESN                    Text,
    IMEI                   Text,
    MEID                   Text,
    ModelID                Text,
    RevID                  Text,
    HardwareRev            Text,
    ICCID                  Text,
    IMSI                   Text,
    PRIMARY KEY (id)        -- primary key
    );
--insert into lteHwInfo ('id','IMEI') values (1,"");

CREATE TABLE lteOperatorList
    (
        providerName           Text,  -- provider name
        mcc                    Integer,        -- valid value range from 0 to 999
        mnc                    Integer,              -- valid value range from 0 to 999
        rat                    Integer,
        PRIMARY KEY(providerName)
    );


CREATE TABLE lteRadio                                                -- This table is used to log user's LTE settings
    (
    id                      Integer NOT NULL,
    lte_mode               Text,            -- lte mode preference "auto", "3G", "4G"
    lte_band               Text,            -- lte band preference "auto", "2600", "900", "800"
    apn_status             Integer,          -- 1: use 1 apn, 2: use 2 apn
    PRIMARY KEY(id)
    );

CREATE TABLE lteNwStatus
    (
    qid                     Integer NOT NULL,
    rx_packets_ok          Integer,
    rx_errors              Integer,
    rx_overflows           Integer,
    rx_bytes_ok            Integer,
    tx_packets_ok          Integer,
    tx_errors              Integer,
    tx_overflows           Integer,
    tx_bytes_ok            Integer,
    registrationState      Text,
    radioInterference      Text,
    rssi                   Text,
    rsrq                   Text,
    rsrp                   Text,
    snr                    Text,
    signalStrength         Integer,
    MCC                    Integer,
    MNC                    Integer,
    networkName            Text,
    CellID                 Integer,
    lteRegMethod           Text,                    -- {"auto", "manual"}
	Roaming				   Text,
    PRIMARY KEY (qid)        -- primary key
    );

CREATE TABLE lteIPStatus
    (
    qid                     Integer,
    status                 Text,
    ip                     Text,
    gateway                Text,
    netmask                Text,
    primary_dns                Text,
    secondary_dns              Text,
    PRIMARY KEY (qid)        -- primary key
    );

CREATE TABLE lteIPv6Status
    (
    qid                     Integer,
    status                 Text,
    ip                     Text,
    gateway                Text,
    primary_dns                Text,
    secondary_dns              Text,
    PRIMARY KEY (qid)        -- primary key
    );
--COMMIT;
