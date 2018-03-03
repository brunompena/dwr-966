--BEGIN;
create table dbUpdateRegisterTbl (
    compName text NOT NULL,
    stopIfError integer NOT NULL,
    waitForMe   integer NOT NULL,
    tableName text NOT NULL,
    rowIndex  integer,
    onUpdate integer NOT NULL,
    onAdd   integer NOT NULL,
    onDelete integer NOT NULL,
    isPrograms integer NOT NULL
);

create table sqliteLock
(
lock integer
);

--create table dbUpdateRegisterProgram
--(
  --  progName text NOT NULL,
  --  stopIfError integer NOT NULL,
  --  waitForMe   integer NOT NULL,
  --  tableName text NOT NULL,
  --  rowIndex  integer,
  --  onUpdate integer NOT NULL,
  --  onAdd   integer NOT NULL,
  --  onDelete integer NOT NULL
--);

--insert into dbUpdateRegisterTbl values ("p2p_limit.sh",1,1,"voipwatchdog", 0, 1,0,0,1);
--insert into dbUpdateRegisterTbl values ("UMI_COMP_VOIP_WDOG",1,1,"voipwatchdog", 0, 1,1,1,0);

CREATE TABLE system
(
    name    text NOT NULL,
    domain  text,
    description text,
    firmwareVer text,
    hardwareVer text,
    voipVer     text,
    tr069Ver    text,
    jnrpVer     text,
    vendor      text,
    serNum      text,
    sysContact  text,
    sysLoc      text,
    PRIMARY KEY (name)
)
;

CREATE TABLE users
(
    username  text NOT NULL,
    password  text,
    groupname  text NOT NULL,
    loginTimeout  integer NOT NULL,
    descr text,                     -- brief description about the user
    --PRIMARY KEY (username,groupname),
    FOREIGN KEY (groupname) REFERENCES groups(name)
)
;

CREATE TABLE groups
(
    name text NOT NULL,               -- group name
    descr text,                     -- brief description about the group
    accessLevel integer NOT NULL,    -- 0 - for readonly, 1 - for conditional read/write, 2 - for fully read/write
    PRIMARY KEY (name)
);
create table voipwatchdog (
    Id  integer NOT NULL,
    context text,
    PRIMARY KEY (Id)
);

--insert into voipwatchdog values (1,"test1");
--insert into voipwatchdog values (2,"test2");



CREATE TABLE Lan
    (
    LogicalIfName   text    NOT NULL,
    Mtu             integer NOT NULL,
    IpAddress       text    NOT NULL,
    SubnetMask      text    NOT NULL,
    IfStatus        boolean NOT NULL,
    MacAddress      text    NOT NULL,
    PRIMARY KEY (LogicalIfName),
    FOREIGN KEY (LogicalIfName) REFERENCES networkInterface(LogicalIfName)
    );
--insert into Lan values ("LAN1",1500,"192.168.0.1","255.255.255.0",1,"00:00:00:00");

CREATE TABLE networkInterface
    (
    interfaceName   text    NOT NULL,
    LogicalIfName   text    NOT NULL,
    VirtualIfName   text    NOT NULL,
    ConnectionType  text    NOT NULL,
    ConnectionType6 text    NOT NULL,
    ifType          text,
    MTU             integer,
    ipaddr          text,
    subnetmask      text,
    gateway         text,
    dns1            text,
    dns2            text,
    DHCP            integer,
    IfStatus        integer NOT NULL,
    refreshInterval integer NOT NULL,
    autoRefreshFlag integer NOT NULL,
    capabilities    integer NOT NULL,
    PRIMARY KEY (interfaceName,LogicalIfName)
)
;
--insert into networkInterface values ("br0", "LAN1", "bdg1", "ifStatic", "", "bridge",
--                   1500, "192.168.0.1", "255.255.255.0",
--                   "0.0.0.0", "0.0.0.0", "0.0.0.0", 0, 1, 5,
--                   0,1);

--insert into networkInterface values ("eth1", "WAN", "eth1", "dhcpc",
--                   "","ethernet", 1500, "0.0.0.0", "0.0.0.0",
--                   "0.0.0.0", "0.0.0.0", "0.0.0.0",
--                   0, 0, 5, 0,1);

--insert into networkInterface values ("sit0", "sit0-WAN", "sit0", "",
--                   "","tunnel", 1500, "0.0.0.0", "0.0.0.0",
--                   "0.0.0.0", "0.0.0.0", "0.0.0.0",
--                   0, 0, 5, 0,2);

--insert into networkInterface values ("usb0", "LTE-WAN1", "usb0", "",
--                   "","lte1", 1500, "0.0.0.0", "0.0.0.0",
--                   "0.0.0.0", "0.0.0.0", "0.0.0.0",
--                   0, 0, 5, 0,1);
--insert into networkInterface values ("usb1", "LTE-WAN2", "usb1", "",
--                   "","lte2", 1500, "0.0.0.0", "0.0.0.0",
--                   "0.0.0.0", "0.0.0.0", "0.0.0.0",
--                   0, 0, 5, 0,1);
-- real time update tables
create table ipAddressTable
(
    LogicalIfName text NOT NULL,        -- hardware interface name
    addressFamily integer,              -- Address family
    isStatic boolean,                   -- static assigned or dynamic
    ipAddress text,                     -- IP address
    ipDstAddres text,                   -- End point for P-to-P
    ipv6PrefixLen integer,              -- IPv6 prefix length
    subnetMask text,                    -- subnet mask
    FOREIGN KEY (LogicalIfName) REFERENCES networkInterface(LogicalIfName)
);

create table resolverConfig
(
    LogicalIfName    text    NOT NULL,  -- WAN1, WAN2, LAN
    addressFamily    integer NOT NULL,  -- Address family
    nameserver1      text            ,  -- DNS nameserver 1
    nameserver2      text            ,  -- DNS nameserver 2
    PRIMARY KEY (LogicalIfName, addressFamily),
    FOREIGN KEY (LogicalIfName) REFERENCES networkInterface(LogicalIfName)
);

create table  defaultRouters
(
  LogicalIfName    text    NOT NULL,    -- WAN1, WAN2, LAN
  addressFamily    integer NOT NULL,    -- Address family
  nextHopGateway   text            ,    -- Next hop gateway
  interfaceName    text            ,    -- for interface route
  PRIMARY KEY (LogicalIfName, addressFamily),
  FOREIGN KEY (LogicalIfName) REFERENCES networkInterface(LogicalIfName)
);

create table  StaticRoute
(
    Enable          boolean NOT NULL,
    Name            text NOT NULL,
    DestineNetwork  text NOT NULL,
    DestineNetmask  text NOT NULL,
    Local  text NOT NULL,
    LocalGateway  text
);

CREATE TABLE LANStatus
(
    portNumber   text NOT NULL,
    ipAddress       text,
    macAddress      text,
    speed          text,
    linkstatus    integer NOT NULL,
    PRIMARY KEY (portNumber)
);
-- end of real time update tables

CREATE TABLE DhcpServerBasic
(
    Id  integer NOT NULL,
    Enable          boolean NOT NULL,
    StartIpAddress  text NOT NULL,
    EndIpAddress    text NOT NULL,
    Range           integer NOT NULL,
    LeaseTime       integer NOT NULL,   -- number of seconds
    IpReserveEnable   boolean NOT NULL,
	IpPhoneVendorID	text,
    PRIMARY KEY (Id)
);

--insert into DhcpServerBasic values (1,1,"192.168.0.2","192.168.0.254",253,24);

CREATE TABLE DhcpServerPools
(
    Enable                  boolean NOT NULL,
    PoolID                  integer NOT NULL,
    PoolOrder               integer,
    LogicalIfName           text    NOT NULL,

    VendorClassID           text,
    VendorClassIDExclude    boolean,
    VendorClassIDMode       text,
    VendorClassMatchOff     integer,
    VendorClassMatchLen     integer,

    ClientID                text,
    ClientIDExclude         boolean,

    UserClassID             text,
    UserClassIDExclude      boolean,

    Chaddr                  text,
    ChaddrMask              text,
    ChaddrExclude           boolean,

    MinAddress              text NOT NULL,
    MaxAddress              text NOT NULL,

    DNSServers              integer,
    DNSServer1              text,
    DNSServer2              text,
    DNSServer3              text,

    DomainName              text,
    IPRouters               text,
    DHCPLeaseTime           integer,

    DefaultPool             boolen NOT NULL,    -- default pool for this interface

    PRIMARY KEY   (PoolID),
    FOREIGN KEY   (DomainName) REFERENCES system (domain)
    FOREIGN KEY   (LogicalIfName) REFERENCES networkInterface (LogicalIfName)
);

CREATE TABLE DhcpOption
(
    PoolID  integer NOT NULL,
    Enable      boolean NOT NULL,
    OptionScope integer NOT NULL,
    OptionCode  integer NOT NULL,
    value1      text,
    value2      text,
    value3      text,
    FOREIGN KEY (PoolID) REFERENCES DhcpServerPools (PoolID)
);

-- dhcpd real time table
CREATE TABLE DhcpLeasedClients
(
   LogicalIfName text  NOT NULL,
   hostName  text,
   IPAddr        text,
   MacAddr       text NOT NULL,
   Starts        text,
   Ends          text,
   clientIf  text,
   Timeout  text,
   State         text
   --State         text,
   --PRIMARY KEY   (IPAddr,hostName)
);

CREATE TABLE DhcpfixedIpAddress
(
    PoolID    integer NOT NULL,
    Enable  boolean NOT NULL,
    IpAddr        text,
    MacAddr       text NOT NULL
    --PRIMARY KEY   (IpAddr,MacAddr),
    --FOREIGN KEY (PoolID) REFERENCES DhcpServerPools (PoolID)
);

CREATE TABLE ddnsConfig
(
    Id          integer NOT NULL,
    Enable      boolean NOT NULL,
    TypeName    text NOT NULL,
    Comment     text,
    PRIMARY KEY (Id)
);
--insert into ddnsConfig values (1,0,'qdns','')
--Type=qdns(3322), dyndns, noip

CREATE TABLE ddns
(
    TypeName    text NOT NULL,
    userName    text,
    pass        text,
    hostName    text,
    interface   text NOT NULL,  -- usb0
    Comment     text,
    PRIMARY KEY (TypeName)
);

-- for bridge mode
-- for bridge mode
CREATE TABLE FirewallNatConfig
(
    Id  integer NOT NULL,
    Enable  boolean NOT NULL,
    PortForwardingEnable    boolean NOT NULL,
    PRIMARY KEY (Id)
);
--insert into FirewallNatConfig values (1,1,0);

CREATE TABLE FirewallConfig
(
    Id  integer NOT NULL,
    Enable  boolean NOT NULL,
    PacketFilterEnable  boolean NOT NULL,
    MacFilterEnable boolean NOT NULL,
    UrlFilterEnable boolean NOT NULL,
    ICMPBLOCK   boolean NOT NULL,
    TCPSynEnable    boolean NOT NULL,
    TCPFinEnable    boolean NOT NULL,
    TCPSynFlood         integer NOT NULL,
    TCPFinFlood         integer NOT NULL,
    PRIMARY KEY (Id)
);
--insert into FirewallConfig values (1,1,0,0,0);

-- Remote Manager Config
create table RmtMgrConfig
(
	Id	integer NOT NULL,
	Enable_TR69	boolean NOT NULL,	-- 1:Enable, 0:Disable (default : 1)
	Enable_VOIP	boolean NOT NULL,	-- 1:Enable, 0:Disable (default : 1)
	Enable_HTTP	boolean NOT NULL,	-- 1:Enable, 0:Disable (default : 0)
	Enable_TELNET	boolean NOT NULL,	-- 1:Enable, 0:Disable (default : 0)
	Enable_SSH	boolean NOT NULL,	-- 1:Enable, 0:Disable (default : 0)
	PRIMARY	KEY	(Id)
);

--CREATE TABLE FirewallRules
--(

--);

CREATE TABLE FirewallDmz
(
    Id  integer NOT NULL,
    Enable      boolean NOT NULL,
    DmzIpAddress    text NOT NULL,
    PRIMARY KEY (Id)
);


CREATE TABLE PacketFilter
(
    Enable      boolean NOT NULL,
    IpAddr      text NOT NULL,
    Protocol    text NOT NULL,
    portRange   text NOT NULL,
    Comment     text
);

CREATE TABLE MacFilter
(
    Enable      boolean NOT NULL,
    MacAddr     text NOT NULL,
    Comment     text
);

CREATE TABLE UrlFilter
(
    Enable      boolean NOT NULL,
    Url         text NOT NULL,
    Comment     text
);

CREATE TABLE DoS
(
    Id          integer NOT NULL,
    Enable      boolean NOT NULL,
    TCPSynEnable    boolean NOT NULL,
    TCPFinEnable    boolean NOT NULL,
    UDPEnable   boolean NOT NULL,
    ICMPEnable  boolean NOT NULL,
    TCPPERIPEnable  boolean NOT NULL,
    TCPFinPERIPEnable   boolean NOT NULL,
    UDPPERIPEnable  boolean NOT NULL,
    ICMPPERIPEnable boolean NOT NULL,
    IPSpoofEnable   boolean NOT NULL,
    TCPSynFlood         integer NOT NULL,
    TCPFinFlood         integer NOT NULL,
    UDPFlood            integer NOT NULL,
    ICMPFlood           integer NOT NULL,
    TCPSynFlood_perip   integer NOT NULL,
    TCPFinFlood_perip   integer NOT NULL,
    UDPFlood_perip      integer NOT NULL,
    ICMPFlood_perip     integer NOT NULL,
    Comment     text,
    PRIMARY KEY (Id)
);

CREATE TABLE SPI
(
    Id  integer NOT NULL,
    Enable      boolean NOT NULL,
    PRIMARY KEY (Id)
);

CREATE TABLE PortForwarding
(
    Enable      boolean NOT NULL,
    Ip          text NOT NULL,
    Protocol    text NOT NULL,
    portRange   text NOT NULL,
    Comment     text
);

CREATE TABLE tr69Config
(
    Enable integer DEFAULT 1,
    URL         text,
    Username    text,
    Password    text,
    STUNEnable  text,
    STUNServerAddress   text,
    STUNUsername    text,
    STUNPassword    text,
    SerialNumber    text,
    ConnectionRequestUsername   text,
    ConnectionRequestPassword   text,
    ConnectionRequestPort   text,
    CACertPath      text,
    ClientCertPath  text,
    Passphrase      text,
    ProvisioningCode text,
	InterfaceName   text,
    PeriodicInformEnable text DEFAULT ('1'),
    PeriodicInformInterval INTEGER DEFAULT (3600),
    FirstUseDate  text DEFAULT ('Null'),
    PRIMARY KEY (URL)
);
--insert into tr69Config values('http://cosmos.bredband.com:8080/ACS-server/ACS','','','0','132.177.123.13','','','','','','/flash/ca.pem','/flash/agent.pem','','og4610-TR069');

-- MacTable - table for MAC refresh
create table MacTable
    (
    interfaceName            text     NOT NULL, -- interface name
    MacAddress               text     NOT NULL,
    PRIMARY KEY (interfaceName),                -- primary key
    FOREIGN KEY (interfaceName) REFERENCES networkInterface (interfaceName)
    );

--insert into MacTable values('br0',"00:00:00:00");
--insert into MacTable values('eth1',"00:00:00:00");
--insert into MacTable values('wlan0',"00:00:00:00");
--insert into MacTable values('wlan0.0',"00:00:00:00");
--insert into MacTable values('wlan0.1',"00:00:00:00");
--insert into MacTable values('wlan0.2',"00:00:00:00");


CREATE TABLE ntpConfig
(
   timezone  integer NOT NULL,
   enabled  integer NOT NULL,
   autoDaylight  integer NOT NULL,
   useDefServers  integer NOT NULL,
   server1  text NOT NULL,
   server2  text NOT NULL,
   server3  text NOT NULL,
   reSyncNtpVal integer NOT NULL,
   networkMode integer  NOT NULL
)
;

CREATE TABLE RemoteLog
(
    serverName  text NOT NULL,      -- servername or ip
    serverPort  integer DEFAULT 514,
    facilityId  integer,            --Ex: kernel 0
    severity    integer,            --Ex: emergency 1
    Enable      integer,            -- 1- enable , 0- disable
    serverId    integer             -- i th syslog server (0 to 7)
)
;


CREATE TABLE dlnaConfig
(
   enable integer DEFAULT 0        -- 1: enable, 0: disable
)
;


-- saveTables for export ascii
create table saveTables (
    tableName text NOT NULL,
    PRIMARY KEY(tableName)
);
--insert into saveTables values("voipwatchdog");
--insert into saveTables values("Lan");
--insert into saveTables values("DhcpServerBasic");
--insert into saveTables values("FirewallDmz");
--insert into saveTables values("FirewallNatConfig");
--insert into saveTables values("FirewallConfig");
--insert into saveTables values("tr69Config");
--insert into saveTables values("system");
--insert into saveTables values("users");
--insert into saveTables values("groups");
--COMMIT;

create table UPNP
(
	Id	integer NOT NULL,
	Enable	boolean NOT NULL,	-- 1:Enable, 0:Disable
	PRIMARY	KEY	(Id)
);

CREATE TABLE RoutingConfig
(
    Id	integer NOT NULL,
    DynamicRoutingEnable      boolean NOT NULL,
    StaticRoutingEnable      boolean NOT NULL,
    PRIMARY KEY (Id)
);

CREATE TABLE StaticRouting
(
    Enable      boolean NOT NULL,
    DestIP      text NOT NULL,
    DestMask    text NOT NULL,
    device              text NOT NULL,
    gateway          text NOT NULL
);

CREATE TABLE ApFwUpgrade
(
	EngUpgrade integer NOT NULL,
	P01001_KEY text NOT NULL,
	P01002_KEY text NOT NULL,
	P01003_KEY text NOT NULL,
	P02001_KEY text NOT NULL,
	P02002_KEY text NOT NULL,
	P02003_KEY text NOT NULL
);
