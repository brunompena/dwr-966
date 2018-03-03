--BEGIN;
CREATE TABLE wifiRadio
(
	interfaceName  text NOT NULL,
	radioNo  integer NOT NULL,
	cardNo  integer NOT NULL,
	band  text NOT NULL,
	currentChannel  integer NOT NULL,
	configuredChannel  text NOT NULL,
	radioenabled  integer NOT NULL,
	minTxPower  integer,
	maxTxPower  integer,
	txPower  integer NOT NULL,
	path  integer NOT NULL,
	rogueAPEnabled  integer NOT NULL,
	rxDiversity  integer NOT NULL,
    dothEnabled integer NOT NULL,
    dothMaxPower integer NOT NULL,
    dothMarkDfs integer NOT NULL,
    opMode  text NOT NULL,
	beaconInterval  integer NOT NULL,
	dtimInterval  integer NOT NULL,
	rtsThreshold  integer NOT NULL,
	fragThreshold  integer NOT NULL,
	preambleMode  text NOT NULL,
	rtsCtsProtect  integer NOT NULL,
	shortRetry  integer NOT NULL,
	longRetry  integer NOT NULL,
	uapsd  integer NOT NULL,
    chanWidth integer NOT NULL,
    sideBand integer,                       -- 0 (below)/ 1 (above)
    puren integer NOT NULL,                 -- 0 (Disable)/ 1 (Enable)
    mimoPreamble  integer NOT NULL,         -- 0 (for MM) / 1 (for GF)
    rxAntennas  integer,                    -- 0 for auto, 1,2,3
    txAntennas  integer,                    -- 0 for auto, 1,2,3
    radarDetected text,
    subband_515_525   integer,
    subband_525_535   integer,
    subband_547_572   integer,
    subband_572_586   integer,
    ackTimeoutDefault integer,
    ackTimeout        integer,
    radioCost        integer,
    enableAMSDU      integer,
    maxClientsPerRadio  integer,
    bandwidth      text,
	PRIMARY KEY (radioNo),
    FOREIGN KEY (cardNo) REFERENCES dot11Card(cardNo)
)
;

CREATE TABLE wifiInterface
(
    interfaceName  text NOT NULL,
    LogicalIfName  text NOT NULL,
    radioNo  integer NOT NULL,
    wlanName  text NOT NULL,
    macAddress  text NOT NULL,
    PRIMARY KEY (interfaceName),
    FOREIGN KEY (interfaceName) REFERENCES networkInterface(interfaceName),
    FOREIGN KEY (radioNo) REFERENCES wifiRadio(radioNo)
)
;


-- This table is not used.
CREATE TABLE wifiACL
(
	hostName	text,
	macAddress  text NOT NULL,
    wlanName  text NOT NULL,
    PRIMARY KEY (wlanName, macAddress),
    FOREIGN KEY (wlanName) REFERENCES wifiAP(wlanName)
)
;

CREATE TABLE wifiACLconfig
(
    profileName text NOT NULL, -- for index
    ACLPolicy1 integer NOT NULL, -- SSID 1 ACL policy
    ACLPolicy2 integer NOT NULL, -- SSID 2 ACL Policy
    ACLPolicy3 integer NOT NULL, -- SSID 3 ACL Policy, Reserved
    ACLPolicy4 integer NOT NULL, -- SSID 4 ACL Policy, Reserved
    ACLUpdate integer NOT NULL, -- for update table
    PRIMARY KEY (profileName)
)
;


CREATE TABLE wifiACLTable1
(
    ACL_MAC    text NOT NULL,  -- target MAC Address
    comment    text            -- Reserved for ACL comment
)
;

CREATE TABLE wifiACLTable2
(
    ACL_MAC    text NOT NULL,  -- target MAC Address
    comment    text            -- Reserved for ACL comment
)
;
CREATE TABLE wifiACLTable3
(
    ACL_MAC    text NOT NULL,  -- target MAC Address
    comment    text            -- Reserved for ACL comment
)
;
CREATE TABLE wifiACLTable4
(
    ACL_MAC    text NOT NULL,  -- target MAC Address
    comment    text            -- Reserved for ACL comment
)
;


CREATE TABLE wifiAP
(
	wlanName  text NOT NULL,
	wlanEnabled  integer NOT NULL,
	profileName  text ,
	beaconInterval  integer ,
	dtimInterval  integer ,
	maxClients  integer ,
	rtsThreshold  integer ,
	fragThreshold  integer ,
	preambleMode  text ,
	rtsCtsProtect  integer ,
	shortRetry  integer ,
	longRetry  integer ,
	txPower  integer ,
	dot11Mode  text NOT NULL,
	apIsolation  integer ,
    defACLPolicy  text ,
    iappEnabled  integer ,
    uapsd  integer ,
    ucastRate text,
    mcastRate text,
    radioList text, -- we use this only to trigger updates,
                    -- in case radio selection has changed
    WPSEnabled  integer ,  -- 0: disable, 1: no configured, 2: configured
    PRIMARY KEY (wlanName),
    FOREIGN KEY (profileName) REFERENCES wifiProfile(profileName)
)
;

CREATE TABLE wifiProfile
(
	profileName  text NOT NULL,
	ssid  text NOT NULL,
	authserver  text ,
	broadcastSSID  integer NOT NULL,
	pskPassAscii  text ,
	pskPassHex  text ,
	wepkeyPassphrase text,
	wepkey0  text ,
	wepkey1  text ,
	wepkey2  text ,
	wepkey3  text ,
	defWepkeyIdx  integer ,
	groupCipher  text ,
	pairwiseCiphers  text ,
	authMethods  text ,
	security  text NOT NULL,
	authTimeout  integer ,
	assocTimeout  integer ,
	groupKeyUpdateInterval  integer ,
	pmksaLifetime  integer ,
	dot1xReauthInterval  integer ,
	wepAuth  text ,
    preAuthStatus integer NOT NULL,
    qosEnable integer NOT NULL,
	 PRIMARY KEY (profileName)
    --FOREIGN KEY (authserver) REFERENCES radiusClient(authserver)
)
;

CREATE TABLE wifiProfilecommit
(
	profileName text NOT NULL,
	profilecommit integer NOT NULL,
	PRIMARY KEY (profileName)
)
;

CREATE TABLE wifiWPS
(
      profileName  text NOT NULL,
      WPSmethod  text NOT NULL,
      WPSEnrollPIN  text ,
      WPSAPPIN  text ,
      PRIMARY KEY (profileName)
)
;

CREATE TABLE wifiStatus
(
      profileName  text NOT NULL,
      peers1 integer , --SSID 1 connected peers 
      peers2 integer , --SSID 2 connected peers
      peers3 integer , --ac connected peers
      peers4 integer , --reserved
      WPSStatus text , --reserverd
      PRIMARY KEY (profileName)
)
;

CREATE TABLE wifiClient
(
      profileName  text NOT NULL,
      triger integer , -- value is meanless, for trigger the handler.
      PRIMARY KEY (profileName)
)
;
CREATE TABLE wifiAOCSstatus
(
    profileName  text NOT NULL,
    availableCh integer NOT NULL,  --available channel for this country code
    debugFlag integer NOT NULL,    --1: enable debug, 0: disable debug AOCS
    scanTime text,                 -- reserved.      
    AOCSenable  integer NOT NULL,  --1: enable, 0 disable
    PRIMARY KEY (profileName)
)
;

CREATE TABLE wifiAOCSChstatus
(
    profileCh  integer NOT NULL,
    channelFreq text NOT NULL,  --mapping the channel index to channel frequency
    channelLoad integer NOT NULL,  --channel loading.
    visiableAP integer           -- extra AP detected in this channel
)
;

CREATE TABLE wifiAOCStrigger
(
    profileName  text NOT NULL,
    scanTrigger integer NOT NULL, --input arbitrarily value to trigger the channel scan
    PRIMARY KEY (profileName)
)
;


--insert into wifiProfile ('profileName','ssid','broadcastSSID','pskPassAscii','security','preAuthStatus','qosEnable') values ('wlan1','iad2_ap1','iad2_ap1','1234567890','WPA2',0,0);
--insert into wifiProfile ('profileName','ssid','broadcastSSID','pskPassAscii','security','preAuthStatus','qosEnable') values ('wlan2','iad2_ap2','iad2_ap2','1234567890','WPA2',0,0);
--insert into wifiProfile ('profileName','ssid','broadcastSSID','pskPassAscii','security','preAuthStatus','qosEnable') values ('wlan3','iad2_ap3','iad2_ap3','1234567890','WPA2',0,0);
--insert into wifiProfile ('profileName','ssid','broadcastSSID','pskPassAscii','security','preAuthStatus','qosEnable') values ('wlan4','iad2_ap4','iad2_ap4','1234567890','WPA2',0,0);
--COMMIT;
