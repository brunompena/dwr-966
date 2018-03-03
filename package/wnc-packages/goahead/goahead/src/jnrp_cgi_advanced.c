#include "jnrp_cgi.h"
#include <stdio.h>
#include <errno.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */

//#define debug_ipv6
enum
{
	ipv6_disable=0,
	ipv6_static=1,
	ipv6_autoconfig=2,
	ipv6_pppoe=3,
	ipv6_6in4=4,
	ipv6_6to4=5,
	ipv6_6rd=6,
	ipv6_gogoc=7
};

enum
{
	vpn_disable=0, 
	vpns_pptp=1, 
	vpns_l2tp=2, 
	vpnc_pptp=3, 
	vpnc_l2tp=4
};
void initFirewallWeb();

// firewall functions
static int getFwGeneralEnable(int eid, webs_t wp, int argc, char_t **argv);
static void setFwGeneralEnable(webs_t wp, char_t *path, char_t *query);
static int getFwPktFilter(int eid, webs_t wp, int argc, char_t **argv);
static void setFwPktFilter(webs_t wp, char_t *path, char_t *query);
static int getFwPktFilterEnable(int eid, webs_t wp, int argc, char_t **argv);
static void setFwPktFilterEnable(webs_t wp, char_t *path, char_t *query);
static void setFwPktFilterDel(webs_t wp, char_t *path, char_t *query);
static int getFwMacFiltEnable(int eid, webs_t wp, int argc, char_t **argv);
static void setFwMacFiltEnable(webs_t wp, char_t *path, char_t *query);
static int getFwMacFilt(int eid, webs_t wp, int argc, char_t **argv);
static void setFwMacFilt(webs_t wp, char_t *path, char_t *query);
static void setFwMacFiltDel(webs_t wp, char_t *path, char_t *query);
static int getAdvanceFirewallInfo (int eid, webs_t wp, int argc, char_t **argv);

static int getFwUrlFiltEnable(int eid, webs_t wp, int argc, char_t **argv);
static void setFwUrlFiltEnable(webs_t wp, char_t *path, char_t *query);
static int getFwUrlFilt(int eid, webs_t wp, int argc, char_t **argv);
static void setFwUrlFilt(webs_t wp, char_t *path, char_t *query);
static void setFwUrlFiltDel(webs_t wp, char_t *path, char_t *query);
#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
static int getFirewallConfig (int eid, webs_t wp, int argc, char_t **argv);
static void setFirewallConf(webs_t wp, char_t *path, char_t *query);
#endif

// nat functions
static int getFwNatEnable (int eid, webs_t wp, int argc, char_t **argv);

static int getFwNatConf(int eid, webs_t wp, int argc, char_t **argv);
static void setFwNatConf(webs_t wp, char_t *path, char_t *query);
static int getFwDmzConf(int eid, webs_t wp, int argc, char_t **argv);
static void setFwDmzConf(webs_t wp, char_t *path, char_t *query);
static int getPortFwd(int eid, webs_t wp, int argc, char_t **argv);

static int getPortFwdEnable(int eid, webs_t wp, int argc, char_t **argv);
static int setPortFwdEnable(webs_t wp, char_t *path, char_t *query);
static int getPortFwdEdit(int eid, webs_t wp, int argc, char_t **argv);
static void setPortFwd(webs_t wp, char_t *path, char_t *query);
#if defined(__P02003_Dora)
static void setPortFwdAction(webs_t wp, char_t *path, char_t *query);
static int getPortFwdList(int eid, webs_t wp, int argc, char_t **argv);
static int getPortFwdStatus(int eid, webs_t wp, int argc, char_t **argv);

enum
{
	portforwarding_status_none=0,
	portforwarding_status_err=1
};
int check_portforwarding_status = portforwarding_status_none;
#endif
static void setPortFwdDel(webs_t wp, char_t *path, char_t *query);

// ddns function
static int getDDNSConf(int eid, webs_t wp, int argc, char_t **argv);
static void setDDNSConf(webs_t wp, char_t *path, char_t *query);
static int getDDNSList(int eid, webs_t wp, int argc, char_t **argv);

//staticroute functions
static int getStaRoutEnable(int eid, webs_t wp, int argc, char_t **argv);
static void setStaRoutEnable(webs_t wp, char_t *path, char_t *query);
static int getStaRout(int eid, webs_t wp, int argc, char_t **argv);
static void setStaRout(webs_t wp, char_t *path, char_t *query);
static void setStaRoutDel(webs_t wp, char_t *path, char_t *query);

//Reboot
static int setRebootform(webs_t wp, char_t *path, char_t *query);
int port_fwd_idx=-1;
int nEnable = 0;

//ipv6 function
static int getIPv6DynamicConf(int eid, webs_t wp, int argc, char_t **argv);
static void setIPv6(webs_t wp, char_t *path, char_t *query);
void getIPv6Prefix(char_t* ip, char_t* prefixLen, char_t* ap_ipaddr_prefix);
static int WebsgetIPv6Prefix(int eid, webs_t wp, int argc, char_t **argv);
static void setIPv6LAN(webs_t wp, char_t *path, char_t *query);
static void setIPv6OnOff(webs_t wp, char_t *path, char_t *query);
static int  getIPv6StaticRoutingTable(int eid, webs_t wp, int argc, char_t **argv);
static void setIPv6StaticRoutingTable(webs_t wp, char_t *path, char_t *query);
static int  getIPv6RoutingName(int eid, webs_t wp, int argc, char_t **argv);
static int  getIPv6RoutingEdit(int eid, webs_t wp, int argc, char_t **argv);
static void setIPv6RoutingEdit(webs_t wp, char_t *path, char_t *query);
static int  getIPv6FirewallTable(int eid, webs_t wp, int argc, char_t **argv);
static void setIPv6FirewallTable(webs_t wp, char_t *path, char_t *query);
static int  getIPv6HostList(int eid, webs_t wp, int argc, char_t **argv);
static int  getIPv6FirewallConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setIPv6FirewallConfig(webs_t wp, char_t *path, char_t *query);
static int  getIPv6Clients(int eid, webs_t wp, int argc, char_t **argv);
static void updateIPv6ClientList(webs_t wp, char_t *path, char_t *query);
int ipv6_routing_edit_idx=-1;
int ipv6_filter_edit_idx=-1;

typedef struct DumpConnectDevice_t
{
	char ip_address[40];
	char mac_address[20];
    char connect_type[10];
    char host_name[50];
}DumpConnectDevice;

//vpn function
static void setVPNPassthrough(webs_t wp, char_t *path, char_t *query);
static void setVPN(webs_t wp, char_t *path, char_t *query);

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
// special apps
static int getSpecialAppsName(int eid, webs_t wp, int argc, char_t **argv);
static int getSpecialAppsList(int eid, webs_t wp, int argc, char_t **argv);
static int getSpecialAppsServiceName(int eid, webs_t wp, int argc, char_t **argv);
static void setSpecialAppsListDel(webs_t wp, char_t *path, char_t *query);
static void setSpecialApps(webs_t wp, char_t *path, char_t *query);
static void setAppListTable(webs_t wp, char_t *path, char_t *query);
static int getSpecialAppsInfo(int eid, webs_t wp, int argc, char_t **argv);
static int getSpecialAppsIsDefault(int eid, webs_t wp, int argc, char_t **argv);
static void setSpecialAppsConfig(webs_t wp, char_t *path, char_t *query);
int special_apps_edit_idx=-1;
enum
{
	spec_apps_none=0,
	spec_apps_err_same_name=1,
	spec_apps_err_access_control=2,
	spec_apps_err_qos_settings=3,
	spec_apps_err_port_forwarding=4
};
int P02003_special_applications_status = spec_apps_none;
static int getSpecialAppsErrorStatus(int eid, webs_t wp, int argc, char_t **argv);

//QoS
static int getQoSPolicy(int eid, webs_t wp, int argc, char_t **argv);
static void QosPolicy(webs_t wp, char_t *path, char_t *query);
static void setQosRule(webs_t wp, char_t *path, char_t *query);
static int getQosList(int eid, webs_t wp, int argc, char_t **argv);
static void setQosListDel(webs_t wp, char_t *path, char_t *query);

//Upnp
static int getUpnpPortEnable(int eid, webs_t wp, int argc, char_t **argv);
static void setUpnpEnable(webs_t wp, char_t *path, char_t *query);
static int getUpnpPortList(int eid, webs_t wp, int argc, char_t **argv);
static void setUpnpListRefresh(webs_t wp, char_t *path, char_t *query);
static void setUpnpListClear(webs_t wp, char_t *path, char_t *query);
#endif

#if defined(__P02003_Dora)
#define DEBUG_MSG

//firewall page
static int getSecurityConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setSecurityConfig(webs_t wp, char_t *path, char_t *query);

//dos attack page
static int getDosAttackConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setDosAttackConfig(webs_t wp, char_t *path, char_t *query);

//known computer page
static int getHostTable(int eid, webs_t wp, int argc, char_t **argv);
static void setHostTable(webs_t wp, char_t *path, char_t *query);
static int getHostConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setHostConfig(webs_t wp, char_t *path, char_t *query);

//url blocking page
static int getFltrTable(int eid, webs_t wp, int argc, char_t **argv);
static void setFltrTable(webs_t wp, char_t *path, char_t *query);
static int getFltrRule(int eid, webs_t wp, int argc, char_t **argv);
static void setFltrRule(webs_t wp, char_t *path, char_t *query);

//access control page
static int getACLTable(int eid, webs_t wp, int argc, char_t **argv);
static void setACLTable(webs_t wp, char_t *path, char_t *query);
int acl_tbl_idx=-1;
static int getACLConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setACLConfig(webs_t wp, char_t *path, char_t *query);
static int getServiceList(int eid, webs_t wp, int argc, char_t **argv);
static int getHostList(int eid, webs_t wp, int argc, char_t **argv);

//rule for schedule page
static int getParCtrl(int eid, webs_t wp, int argc, char_t **argv);
static void setParCtrl(webs_t wp, char_t *path, char_t *query);
static int getUrlProfileTable(int eid, webs_t wp, int argc, char_t **argv);
static void setUrlProfileTable(webs_t wp, char_t *path, char_t *query);
static int getUrlProfileConfig(int eid, webs_t wp, int argc, char_t **argv);
static void setUrlProfileConfig(webs_t wp, char_t *path, char_t *query);
static int getFltrList(int eid, webs_t wp, int argc, char_t **argv);

static int getScheduleTable(int eid, webs_t wp, int argc, char_t **argv);
static void setScheduleTable(webs_t wp, char_t *path, char_t *query);
static int getSchedule(int eid, webs_t wp, int argc, char_t **argv);
static void setSchedule(webs_t wp, char_t *path, char_t *query);

//Static Route Page
static void setStaRouteTable(webs_t wp, char_t *path, char_t *query);
static void setStaRoutAdd(webs_t wp, char_t *path, char_t *query);
static int getStaticRouteInfo(int eid, webs_t wp, int argc, char_t **argv);
static int getLANGatewayStartIP(int eid, webs_t wp, int argc, char_t **argv);
static int getLANGatewayEndIP(int eid, webs_t wp, int argc, char_t **argv);
static int getInternetGatewayStartIP(int eid, webs_t wp, int argc, char_t **argv);
static int getInternetGatewayEndIP(int eid, webs_t wp, int argc, char_t **argv);
static void setStaticRouteEdit(webs_t wp, char_t *path, char_t *query);
static int getStaticRouteName(int eid, webs_t wp, int argc, char_t **argv);
int static_route_edit_idx=-1;

//QoS Settings page
static int getQosInfo(int eid, webs_t wp, int argc, char_t **argv);
static void setSessionLimit(webs_t wp, char_t *path, char_t *query);
static void setBandWidthManage(webs_t wp, char_t *path, char_t *query);
static int getQosBandwidthProfiles(int eid, webs_t wp, int argc, char_t **argv);
static void setQoSTable(webs_t wp, char_t *path, char_t *query);
int qos_edit_idx=-1;

//QoS Add page
static int getQosServiceOptions(int eid, webs_t wp, int argc, char_t **argv);
static void setNewServerAdd(webs_t wp, char_t *path, char_t *query);
static void setQosAdd(webs_t wp, char_t *path, char_t *query);

//QoS Edit page
static int getQosEditInfo(int eid, webs_t wp, int argc, char_t **argv);
static void setQosEdit(webs_t wp, char_t *path, char_t *query);
static int getServiceName(int eid, webs_t wp, int argc, char_t **argv);
static int getServiceTraffic(int eid, webs_t wp, int argc, char_t **argv);
static int getServiceContent(int eid, webs_t wp, int argc, char_t **argv);

//vpn function
static int getVPNPT(int eid, webs_t wp, int argc, char_t **argv);
static void setVPNPT(webs_t wp, char_t *path, char_t *query);
static int getVpnProfile(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnProfile(webs_t wp, char_t *path, char_t *query);
static int getVpnIPSecProfile(int eid, webs_t wp, int argc, char_t **argv);
static int getVpnPPTP(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnServerPPTP(webs_t wp, char_t *path, char_t *query);
static void setVpnClientPPTP(webs_t wp, char_t *path, char_t *query);
static int getVpnL2TP(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnServerL2TP(webs_t wp, char_t *path, char_t *query);
static void setVpnClientL2TP(webs_t wp, char_t *path, char_t *query);	
enum
{
	vpn_default=0,
	vpn_pptp=1,
	vpn_l2tp=2
};
int vpn_edit_type=vpn_default;
int vpn_tbl_idx=-1;
static int getVpnUserTbl(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnUserTbl(webs_t wp, char_t *path, char_t *query);	
static int getVpnUser(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnUser(webs_t wp, char_t *path, char_t *query);	
static int getVpnNAT(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnNAT(webs_t wp, char_t *path, char_t *query);	
static int getVpnIPSecTable(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnIPSecTable(webs_t wp, char_t *path, char_t *query);	
static int getVpnIPSec(int eid, webs_t wp, int argc, char_t **argv);
static void setVpnIPSec(webs_t wp, char_t *path, char_t *query);	

#endif

/*howard add*/
#include <signal.h>
int get_pid(const char* name)
{
        char cmd[64];
        sprintf(cmd, "/var/run/%s", name);
        FILE *fs;
        int pid = -1;
        fs = fopen(cmd, "r");
        if (fs != NULL) {
            fgets(cmd, sizeof(cmd), fs);
            pid = atoi(cmd);
            fclose(fs);
        }
        return pid;
}

void kill_process_by_pid(int pid)
{
        if(pid>0 && kill(pid, 0) == 0){
                kill(pid,15);
                sleep(2);
                while(kill(pid, 0) == 0 )
                        sleep(1);
        }
}
/*howard end*/

void initFirewallWeb()
{
    websAspDefine(T("getFwGeneralEnable"), getFwGeneralEnable);
    websFormDefine(T("setFwGeneralEnable"), setFwGeneralEnable);
	websAspDefine(T("getAdvanceFirewallInfo"), getAdvanceFirewallInfo);

    websAspDefine(T("getFwPktFilter"), getFwPktFilter);
    websFormDefine(T("setFwPktFilter"), setFwPktFilter);
    websAspDefine(T("getFwPktFilterEnable"), getFwPktFilterEnable);
    websFormDefine(T("setFwPktFilterEnable"), setFwPktFilterEnable);
    websFormDefine(T("setFwPktFilterDel"), setFwPktFilterDel);

    websAspDefine(T("getFwMacFiltEnable"), getFwMacFiltEnable);
    websFormDefine(T("setFwMacFiltEnable"), setFwMacFiltEnable);
    websAspDefine(T("getFwMacFilt"), getFwMacFilt);
    websFormDefine(T("setFwMacFilt"), setFwMacFilt);
    websFormDefine(T("setFwMacFiltDel"), setFwMacFiltDel);

    websAspDefine(T("getFwUrlFiltEnable"), getFwUrlFiltEnable);
    websFormDefine(T("setFwUrlFiltEnable"), setFwUrlFiltEnable);
    websAspDefine(T("getFwUrlFilt"), getFwUrlFilt);
    websFormDefine(T("setFwUrlFilt"), setFwUrlFilt);
    websFormDefine(T("setFwUrlFiltDel"), setFwUrlFiltDel);

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
	websAspDefine(T("getFirewallConfig"), getFirewallConfig);
	websFormDefine(T("setFirewallConf"), setFirewallConf);
#endif		

    websAspDefine(T("getFwNatConf"), getFwNatConf);
    websFormDefine(T("setFwNatConf"), setFwNatConf);
	websAspDefine(T("getFwNatEnable"), getFwNatEnable);

    websAspDefine(T("getFwDmzConf"), getFwDmzConf);
    websFormDefine(T("setFwDmzConf"), setFwDmzConf);

    websAspDefine(T("getPortFwd"), getPortFwd);
    websAspDefine(T("getPortFwdEnable"), getPortFwdEnable);
    websFormDefine(T("setPortFwdEnable"), setPortFwdEnable);
    websAspDefine(T("getPortFwdEdit"), getPortFwdEdit);
    websFormDefine(T("setPortFwd"), setPortFwd);
#if defined(__P02003_Dora)	
	websFormDefine(T("setPortFwdAction"), setPortFwdAction);
	websAspDefine(T("getPortFwdList"), getPortFwdList);
	websAspDefine(T("getPortFwdStatus"), getPortFwdStatus);
#endif
	
    websFormDefine(T("setPortFwdDel"), setPortFwdDel);
    websAspDefine(T("getDDNSConf"), getDDNSConf);
    websFormDefine(T("setDDNSConf"), setDDNSConf);
    websAspDefine(T("getDDNSList"), getDDNSList);
	websAspDefine(T("getStaRout"), getStaRout);
    websFormDefine(T("setStaRout"), setStaRout);
    websAspDefine(T("getStaRoutEnable"), getStaRoutEnable);
    websFormDefine(T("setStaRoutEnable"), setStaRoutEnable);
    websFormDefine(T("setStaRoutDel"), setStaRoutDel);
	websFormDefine(T("setRebootform"), setRebootform);

	//IPv6
	websAspDefine(T("getIPv6DynamicConf"), getIPv6DynamicConf);
    websFormDefine(T("setIPv6"), setIPv6);
	websAspDefine(T("WebsgetIPv6Prefix"), WebsgetIPv6Prefix);
	websFormDefine(T("setIPv6LAN"), setIPv6LAN);
	websFormDefine(T("setIPv6OnOff"), setIPv6OnOff);
	websAspDefine(T("getIPv6StaticRoutingTable"), getIPv6StaticRoutingTable);
	websFormDefine(T("setIPv6StaticRoutingTable"), setIPv6StaticRoutingTable);
	websAspDefine(T("getIPv6RoutingName"), getIPv6RoutingName);
	websAspDefine(T("getIPv6RoutingEdit"), getIPv6RoutingEdit);
	websFormDefine(T("setIPv6RoutingEdit"), setIPv6RoutingEdit);
	websAspDefine(T("getIPv6FirewallTable"), getIPv6FirewallTable);
	websFormDefine(T("setIPv6FirewallTable"), setIPv6FirewallTable);
	websAspDefine(T("getIPv6HostList"), getIPv6HostList);
	websAspDefine(T("getIPv6FirewallConfig"), getIPv6FirewallConfig);
	websFormDefine(T("setIPv6FirewallConfig"), setIPv6FirewallConfig);
	websAspDefine(T("getIPv6Clients"), getIPv6Clients);
	websFormDefine(T("updateIPv6ClientList"), updateIPv6ClientList);
	
	//VPN
	websFormDefine(T("setVPNPassthrough"), setVPNPassthrough);
    websFormDefine(T("setVPN"), setVPN);

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)	
	//Special Apps
	websAspDefine(T("getSpecialAppsName"), getSpecialAppsName);
	websAspDefine(T("getSpecialAppsList"), getSpecialAppsList);
	websAspDefine(T("getSpecialAppsServiceName"), getSpecialAppsServiceName);
	websFormDefine(T("setSpecialAppsListDel"), setSpecialAppsListDel);
	websFormDefine(T("setSpecialApps"), setSpecialApps);
	websFormDefine(T("setAppListTable"), setAppListTable);
	websAspDefine(T("getSpecialAppsInfo"), getSpecialAppsInfo);
	websAspDefine(T("getSpecialAppsIsDefault"), getSpecialAppsIsDefault);
	websFormDefine(T("setSpecialAppsConfig"), setSpecialAppsConfig);
	websAspDefine(T("getSpecialAppsErrorStatus"), getSpecialAppsErrorStatus);

	//QoS    
	websAspDefine(T("getQoSPolicy"), getQoSPolicy);
	websFormDefine(T("QosPolicy"), QosPolicy);
	websFormDefine(T("setQosRule"), setQosRule);
	websAspDefine(T("getQosList"), getQosList);
	websFormDefine(T("setQosListDel"), setQosListDel); 
	//Upnp
	websAspDefine(T("getUpnpPortEnable"), getUpnpPortEnable);
	websFormDefine(T("setUpnpEnable"), setUpnpEnable); 
	websAspDefine(T("getUpnpPortList"), getUpnpPortList);
	websFormDefine(T("setUpnpListRefresh"), setUpnpListRefresh); 
	websFormDefine(T("setUpnpListClear"), setUpnpListClear); 
#endif

#if defined(__P02003_Dora)
	websAspDefine(T("getSecurityConfig"), getSecurityConfig);
	websFormDefine(T("setSecurityConfig"), setSecurityConfig);
	websAspDefine(T("getDosAttackConfig"), getDosAttackConfig);
	websFormDefine(T("setDosAttackConfig"), setDosAttackConfig);	
	websAspDefine(T("getHostTable"), getHostTable);
	websFormDefine(T("setHostTable"), setHostTable);	
	websAspDefine(T("getHostConfig"), getHostConfig);
	websFormDefine(T("setHostConfig"), setHostConfig);	
	websAspDefine(T("getFltrTable"), getFltrTable);
	websFormDefine(T("setFltrTable"), setFltrTable);	
	websAspDefine(T("getFltrRule"), getFltrRule);
	websFormDefine(T("setFltrRule"), setFltrRule);		
	websAspDefine(T("getACLTable"), getACLTable);
	websFormDefine(T("setACLTable"), setACLTable);		
	websAspDefine(T("getACLConfig"), getACLConfig);
	websFormDefine(T("setACLConfig"), setACLConfig);
	websAspDefine(T("getServiceList"), getServiceList);
	websAspDefine(T("getHostList"), getHostList);	
	websAspDefine(T("getParCtrl"), getParCtrl);	
	websFormDefine(T("setParCtrl"), setParCtrl);	
	websAspDefine(T("getUrlProfileTable"), getUrlProfileTable);	
	websFormDefine(T("setUrlProfileTable"), setUrlProfileTable);	
	websAspDefine(T("getUrlProfileConfig"), getUrlProfileConfig);	
	websFormDefine(T("setUrlProfileConfig"), setUrlProfileConfig);	
	websAspDefine(T("getFltrList"), getFltrList);	
	websAspDefine(T("getScheduleTable"), getScheduleTable);
	websFormDefine(T("setScheduleTable"), setScheduleTable);		
	websAspDefine(T("getSchedule"), getSchedule);
	websFormDefine(T("setSchedule"), setSchedule);	
	websFormDefine(T("setStaRouteTable"), setStaRouteTable);
	websFormDefine(T("setStaRoutAdd"), setStaRoutAdd);
	websAspDefine(T("getStaticRouteInfo"), getStaticRouteInfo);	
	websAspDefine(T("getLANGatewayStartIP"), getLANGatewayStartIP);
	websAspDefine(T("getLANGatewayEndIP"), getLANGatewayEndIP);
	websAspDefine(T("getInternetGatewayStartIP"), getInternetGatewayStartIP);
	websAspDefine(T("getInternetGatewayEndIP"), getInternetGatewayEndIP);
	websFormDefine(T("setStaticRouteEdit"), setStaticRouteEdit);
	websAspDefine(T("getStaticRouteName"), getStaticRouteName);	
	websAspDefine(T("getQosInfo"), getQosInfo);	
	websFormDefine(T("setSessionLimit"), setSessionLimit);
	websFormDefine(T("setBandWidthManage"), setBandWidthManage);
	websAspDefine(T("getQosBandwidthProfiles"), getQosBandwidthProfiles);
	websFormDefine(T("setQoSTable"), setQoSTable);
	websAspDefine(T("getQosServiceOptions"), getQosServiceOptions);
	websFormDefine(T("setNewServerAdd"), setNewServerAdd);
	websFormDefine(T("setQosAdd"), setQosAdd);
	websAspDefine(T("getQosEditInfo"), getQosEditInfo);
	websFormDefine(T("setQosEdit"), setQosEdit);
	websAspDefine(T("getServiceName"), getServiceName);
	websAspDefine(T("getServiceTraffic"), getServiceTraffic);
	websAspDefine(T("getServiceContent"), getServiceContent);

	//VPN
	websAspDefine(T("getVPNPT"), getVPNPT);
	websFormDefine(T("setVPNPT"), setVPNPT);
	websAspDefine(T("getVpnProfile"), getVpnProfile);
	websFormDefine(T("setVpnProfile"), setVpnProfile);
	websAspDefine(T("getVpnIPSecProfile"), getVpnIPSecProfile);
	websAspDefine(T("getVpnPPTP"), getVpnPPTP);
	websFormDefine(T("setVpnServerPPTP"), setVpnServerPPTP);
	websFormDefine(T("setVpnClientPPTP"), setVpnClientPPTP);
	websAspDefine(T("getVpnL2TP"), getVpnL2TP);	
	websFormDefine(T("setVpnServerL2TP"), setVpnServerL2TP);
	websFormDefine(T("setVpnClientL2TP"), setVpnClientL2TP);
	websAspDefine(T("getVpnUserTbl"), getVpnUserTbl);
	websFormDefine(T("setVpnUserTbl"), setVpnUserTbl);
	websAspDefine(T("getVpnUser"), getVpnUser);
	websFormDefine(T("setVpnUser"), setVpnUser);
	websAspDefine(T("getVpnNAT"), getVpnNAT);
	websFormDefine(T("setVpnNAT"), setVpnNAT);		
	websAspDefine(T("getVpnIPSecTable"), getVpnIPSecTable);
	websFormDefine(T("setVpnIPSecTable"), setVpnIPSecTable);	
	websAspDefine(T("getVpnIPSec"), getVpnIPSec);
	websFormDefine(T("setVpnIPSec"), setVpnIPSec);

#endif

    return ;
}

static int getFwGeneralEnable(int eid, webs_t wp, int argc, char_t **argv)
{
    char pFwInfo[128];

    int nRows=0,nCols=0;
    char** ppQueryRes = NULL;
    char * pQueryStr = "select * from FirewallConfig";

    char* pEnable = NULL;
    char* pPktFilter = NULL;
    char* pMACFilter = NULL;
	char* pURLFilter = NULL;

    memset(pFwInfo,'\0',sizeof(pFwInfo));
#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        return websWrite(wp, T("\"%s\""),pFwInfo);
    }

    if (nRows > 0) {
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Enable", pEnable, getSysInfoOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "PacketFilterEnable", pPktFilter, getSysInfoOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "MacFilterEnable", pMACFilter, getSysInfoOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UrlFilterEnable", pURLFilter, getSysInfoOut);

      gstrcat(pFwInfo,pEnable);
      gstrcat(pFwInfo,pSep);
      gstrcat(pFwInfo,pPktFilter);
      gstrcat(pFwInfo,pSep);
      gstrcat(pFwInfo,pMACFilter);
	  gstrcat(pFwInfo,pSep);
      gstrcat(pFwInfo,pURLFilter);
      gstrcat(pFwInfo,pSep);
    }

getSysInfoOut:
    if (ppQueryRes)
    {
      sqlite3_free_table(ppQueryRes);
    }

    return websWrite(wp, T("\"%s\""),pFwInfo);
}

static int getAdvanceFirewallInfo (int eid, webs_t wp, int argc, char_t **argv)
{
  char pFirewallInfo[256];

  int nRows=0,nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "SELECT PacketFilterEnable, MacFilterEnable, UrlFilterEnable,Enable FROM FirewallConfig";

  char* pPacketFilterEnable = NULL;
  char* pMacFilterEnable = NULL;
  char* pUrlFilterEnable = NULL;
  char* pEnable = NULL;

  memset(pFirewallInfo,'\0',sizeof(pFirewallInfo));
#ifdef __USED_WEBS_DB__
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
  sqlite3 * pWebDB = NULL;
  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
    //sqlite3_free(pQueryStr);
    return websWrite(wp, T("\"%s\""),pFirewallInfo);
  }
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "PacketFilterEnable", pPacketFilterEnable, getFirewallInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "MacFilterEnable", pMacFilterEnable, getFirewallInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UrlFilterEnable", pUrlFilterEnable, getFirewallInfoOut);
  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Enable", pEnable, getFirewallInfoOut);

  gstrcat(pFirewallInfo,pPacketFilterEnable);
  gstrcat(pFirewallInfo,pSep);
  gstrcat(pFirewallInfo,pMacFilterEnable);
  gstrcat(pFirewallInfo,pSep);
  gstrcat(pFirewallInfo,pUrlFilterEnable);
  gstrcat(pFirewallInfo,pSep);
  gstrcat(pFirewallInfo,pEnable);
getFirewallInfoOut:
  if (ppQueryRes)
  {
    sqlite3_free_table(ppQueryRes);
  }

  return websWrite(wp, T("\"%s\""), pFirewallInfo);
}

static void setFwGeneralEnable(webs_t wp, char_t *path, char_t *query)
{
    //printf("setFwGeneralEnable test\n");
    //char_t * pFwDummy = websGetVar(wp, T("fw_dummy"), T(""));
    char_t * pFwEnable = websGetVar(wp, T("fw_gen_enable"), T("off"));
    //printf("pFwDummy: %s \n",pFwDummy);
    //printf("pFwEnable :%s \n",pFwEnable);

    int nFwEnable = 0, nPkgFltEnable=0,nMacFltEnable=0,nUrlFltEnble=0;

    if (!gstrcmp(pFwEnable,"on"))
    {
        nFwEnable = 1;
        char_t * pPktFilterEnable = websGetVar(wp, T("pkt_filter_enable"), T("off"));
        char_t * pMacFilterEnable = websGetVar(wp, T("mac_filter_enable"), T("off"));
        char_t * pUrlFilterEnable = websGetVar(wp, T("url_filter_enable"), T("off"));
        if (!gstrcmp(pPktFilterEnable,"on"))
            nPkgFltEnable = 1;
        if (!gstrcmp(pMacFilterEnable,"on"))
            nMacFltEnable = 1;
        if (!gstrcmp(pUrlFilterEnable,"on"))
            nUrlFltEnble = 1;
    }

    char pFwConfQry [1024] = {0};
    memset (pFwConfQry, 0x00, sizeof(pFwConfQry));

    snprintf (pFwConfQry, sizeof(pFwConfQry),"update FirewallConfig set Enable='%d', PacketFilterEnable='%d', "
        "MacFilterEnable='%d', UrlFilterEnable='%d'",nFwEnable,nPkgFltEnable,nMacFltEnable,nUrlFltEnble);

    //printf("pFwConfQry: %s \n",pFwConfQry);
#ifdef __USED_WEBS_DB__
    int status =  jnrp_sql_dbEXE_Webs (pFwConfQry);
#else
    int status =  jnrp_sql_dbEXE (pDBFile, pFwConfQry);
#endif
    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
        system("fwnat.sh");
    }

    websRedirect(wp, T("advanced/firewall_general.asp"));
}

static int getFwPktFilter(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from PacketFilter";

 int index = 0;
 char* pEnable = NULL;
 char* pIpAddr = NULL;
 char* pProtocol = NULL;
 char* pportRange = NULL;
 char* pComment = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getFwPktFilterOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "IpAddr", pIpAddr, getFwPktFilterOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Protocol", pProtocol, getFwPktFilterOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "portRange", pportRange, getFwPktFilterOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Comment", pComment, getFwPktFilterOut);

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    }
    websWrite(wp, T("<td align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"pktfilt_ck\" id=\"pktfilt_ck_%d\" onclick=\"setPktfiltEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"pktfilt_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"pktfilt_ip_%d\">%s</span></td>\n"), index+1, pIpAddr);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"pktfilt_protocol_%d\">%s</td>\n"), index+1, pProtocol);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"pktfilt_range_%d\">%s ~ %s</td>\n"), index+1, getSplitStr(pportRange, ":", 1), getSplitStr(pportRange, ":", 2));
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"pktfilt_desc_%d\">%s</td>\n"), index+1, pComment);
#else
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
    }
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"pktfilt_ck\" id=\"pktfilt_ck_%d\" onclick=\"setPktfiltEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"pktfilt_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\"><span id=\"pktfilt_ip_%d\">%s</span></td>\n"), index+1, pIpAddr);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"pktfilt_protocol_%d\">%s</td>\n"), index+1, pProtocol);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"pktfilt_range_%d\">%s ~ %s</td>\n"), index+1, getSplitStr(pportRange, ":", 1), getSplitStr(pportRange, ":", 2));
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"pktfilt_desc_%d\">%s</td>\n"), index+1, pComment);
#endif
  }
 }

getFwPktFilterOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    //int count = jnrp_sql_getRowNum("PacketFilter",pDBFile);
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    //char   sqlCmd [1024] = {0};
    char * sqlCmd = "select * from PacketFilter";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    //snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);
    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    
     while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
        //char* data = (const char*)sqlite3_column_text( stmt, i );
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"pktfilt_ck\" id=\"pktfilt_ck_%d\" onclick=\"setPktfiltEditRowId('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td id=\"pktfilt_enable_%d\">%s</td>\n"),row_count,
        gatoi((const char*)sqlite3_column_text( stmt, 0 ))?"Enabled":"Disabled");
        websWrite(wp, T("<td><span id=\"pktfilt_ip_%d\">%s</span></td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 1));
        websWrite(wp, T("<td id=\"pktfilt_protocol_%d\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 2 ));
        websWrite(wp, T("<td id=\"pktfilt_range_%d\">%s ~ %s</td>\n"),row_count,
        getSplitStr((const char*)sqlite3_column_text( stmt, 3 ),":",1)
        ,getSplitStr((const char*)sqlite3_column_text( stmt, 3 ),":",2));
        websWrite(wp, T("<td id=\"pktfilt_desc_%d\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 4 ));
    }
    //printf("row_count:%d \n",row_count);
    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
#endif
}

static void setFwPktFilter(webs_t wp, char_t *path, char_t *query)
{
    char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char_t * pRowId = websGetVar(wp, T("pktfilt_rowid_edit"), T(""));
    //printf("editflag:%s ,pRowId: %s \n",pEditMode,pRowId);

    char * pErrMsg = NULL;
    int status = -1;

    char_t * pEnable = websGetVar(wp, T("pktfilt_enable"), T("off"));
    char_t * pIp = websGetVar(wp, T("pktfilt_ip"), T(""));
    char_t * pProtocol = websGetVar(wp, T("protocol"), T(""));
    char_t * pStartPort = websGetVar(wp, T("start_port"), T(""));
    char_t * pEndPort = websGetVar(wp, T("end_port"), T(""));
    char_t * pPortDesc = websGetVar(wp, T("pktfilt_desc"), T(""));
    //printf("pEnable:%s , pIp:%s, pProtocol:%s, pStartPort:%s, pEndPort:%s, pPortDesc:%s \n",pEnable,pIp,pProtocol,pStartPort,pEndPort,pPortDesc);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setPktFilt add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into PacketFilter values ('%s','%s','%s','%s:%s','%s')"
        ,gstrcmp(pEnable,"off")?"1":"0",pIp,pProtocol,pStartPort,pEndPort,pPortDesc);
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }
    else if (!gstrcmp(pEditMode,"edit"))
    {
        //printf("setPktFilt edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update PacketFilter set Enable='%d', IpAddr='%s', Protocol='%s', portRange='%s:%s', Comment='%s' where _ROWID_='%s'"
        ,gstrcmp(pEnable,"off")?1:0,pIp,pProtocol,pStartPort,pEndPort,pPortDesc,pRowId);
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
        system("firewall.sh");
    }
    
    websRedirect(wp, T("advanced/firewall_packetfilter.asp"));
}

static int getFwPktFilterEnable(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("FirewallConfig", "PacketFilterEnable", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "PacketFilterEnable", 1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

static void setFwPktFilterEnable(webs_t wp, char_t *path, char_t *query)
{
	//printf("setPortFwdEnable: \n");
    char_t * pEnable = websGetVar(wp, T("enpktfilt"), T("off"));
	int nRows=0,nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "SELECT Enable FROM FirewallConfig";
	
	char* pFirewallEnable = NULL;
#ifdef __USED_WEBS_DB__
	  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
	  sqlite3 * pWebDB = NULL;
	  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
	  {
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	  }
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Enable", pFirewallEnable, getFirewallInfoOut);
	getFirewallInfoOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
#ifdef __USED_WEBS_DB__
    if (!gstrcmp(pEnable,"on"))
    {
        jnrp_sql_setAttr_Webs ("FirewallConfig", "PacketFilterEnable", "1", 1);
		if(!gstrncmp(pFirewallEnable,"1",1) )
		{
			
		}
		else
		{
			jnrp_sql_setAttr_Webs ("FirewallConfig", "Enable", "1", 1);	
		}
    }
    else
    {
        jnrp_sql_setAttr_Webs ("FirewallConfig", "PacketFilterEnable", "0", 1);
    }
#else
    if (!gstrcmp(pEnable,"on"))
    {
        jnrp_sql_setAttr (pDBFile, "FirewallConfig", "PacketFilterEnable", "1", 1);
    }
    else
    {
        jnrp_sql_setAttr (pDBFile, "FirewallConfig", "PacketFilterEnable", "0", 1);
    }
#endif

    system("fwnat.sh");

    websRedirect(wp, T("advanced/firewall_packetfilter.asp"));
}

static void setFwPktFilterDel(webs_t wp, char_t *path, char_t *query)
{
	//printf("setFwPktFilterDel \n");
    //char_t * pId = websGetVar(wp, T("pktfilt_rowid_del"), T("0"));
    char * pId = websGetVar(wp, T("pktfilt_rowid_del"), T(""));
    //char * pDelId = websGetVar(wp, T("rowid_del"), T(""));
    //printf("pId: %s\n",pId);
	//printf("pDelId: %s\n",pDelId);
    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM PacketFilter WHERE _ROWID_='%s'",pId);
        //printf("DELETE FROM PacketFilter WHERE _ROWID_='%s' \n",pId);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
			//printf("pPortDelQry: %s \n",pPortDelQry);
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
            system("firewall.sh");
        }
    }

    websRedirect(wp, T("advanced/firewall_packetfilter.asp"));
}

static int getFwMacFiltEnable(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
		char * pQryResult = jnrp_sql_getAttr_Webs ("FirewallConfig", "MacFilterEnable", 1);
#else
  char * pQryResult = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "MacFilterEnable", 1);
#endif
  return websWrite(wp, T("%s"),pQryResult);
}

static void setFwMacFiltEnable(webs_t wp, char_t *path, char_t *query)
{
		int nRows=0,nCols=0;
		char** ppQueryRes = NULL;
		char * pQueryStr = "SELECT Enable FROM FirewallConfig";
		
		char_t * pEnable = websGetVar(wp, T("enmacfilt"), T("off"));
		char* pFirewallEnable = NULL;
#ifdef __USED_WEBS_DB__
		  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
		  sqlite3 * pWebDB = NULL;
		  if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
		  {
			printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		  }
		  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Enable", pFirewallEnable, getFirewallInfoOut);
		getFirewallInfoOut:
		if (ppQueryRes)
		{
			sqlite3_free_table(ppQueryRes);
		}


#ifdef __USED_WEBS_DB__
    if (!gstrcmp(pEnable,"on"))
    {
        jnrp_sql_setAttr_Webs ("FirewallConfig", "MacFilterEnable", "1", 1);
		if(!gstrncmp(pFirewallEnable,"1",1) )
		{
			
		}
		else
		{
			jnrp_sql_setAttr_Webs ("FirewallConfig", "Enable", "1", 1); 
		}
    }
    else
    {
        jnrp_sql_setAttr_Webs ("FirewallConfig", "MacFilterEnable", "0", 1);
    }
#else
    if (!gstrcmp(pEnable,"on"))
    {
        jnrp_sql_setAttr (pDBFile, "FirewallConfig", "MacFilterEnable", "1", 1);
    }
    else
    {
        jnrp_sql_setAttr (pDBFile, "FirewallConfig", "MacFilterEnable", "0", 1);
    }
#endif

    system("fwnat.sh");

    websRedirect(wp, T("advanced/firewall_macfilter.asp"));
}

static int getFwMacFilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from MacFilter";

 int index = 0;
 char* pEnable = NULL;
 char* pMacAddr = NULL;
 char* pComment = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getFwMacFiltOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "MacAddr", pMacAddr, getFwMacFiltOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Comment", pComment, getFwMacFiltOut);
#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    }    
    websWrite(wp, T("<td align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"mac_ck\" id=\"mac_ck_%d\" onclick=\"setMacEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"mac_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"mac_ip_%d\">%s</span></td>\n"), index+1, pMacAddr);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"mac_desc_%d\">%s</td>\n"), index+1, pComment);
#else
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
    }    
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"mac_ck\" id=\"mac_ck_%d\" onclick=\"setMacEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"mac_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\"><span id=\"mac_ip_%d\">%s</span></td>\n"), index+1, pMacAddr);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"mac_desc_%d\">%s</td>\n"), index+1, pComment);
#endif
  }
 }

getFwMacFiltOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
	sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    //char   sqlCmd [1024] = {0};
    char * sqlCmd = "select * from MacFilter";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    //snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);
    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    // get column numbers
    //int count = sqlite3_column_count(stmt);
    while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
        //char* data = (const char*)sqlite3_column_text( stmt, i );
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"mac_ck\" id=\"mac_ck_%d\" onclick=\"setMacEditRowId('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td id=\"mac_enable_%d\">%s</td>\n"),row_count,
        gatoi((const char*)sqlite3_column_text( stmt, 0 ))?"Enabled":"Disabled");
        websWrite(wp, T("<td><span id=\"mac_ip_%d\">%s</span></td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 1));       
        websWrite(wp, T("<td id=\"mac_desc_%d\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 2 ));
    }
    //printf("row_count:%d \n",row_count);
    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    
    return 0;
#endif
}

static void setFwMacFilt(webs_t wp, char_t *path, char_t *query)
{
	char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char_t * pRowId = websGetVar(wp, T("macfilt_rowid_edit"), T(""));
    //printf("editflag:%s ,pRowId: %s \n",pEditMode,pRowId);

    char * pErrMsg = NULL;
    int status = -1;

    char_t * pEnable = websGetVar(wp, T("mac_enable"), T("off"));
    char_t * pMacIp = websGetVar(wp, T("mac_ip"), T(""));    
    char_t * pMacDesc = websGetVar(wp, T("mac_desc"), T(""));
    //printf("pEnable:%s , pIp:%s, pProtocol:%s, pStartPort:%s, pEndPort:%s, pPortDesc:%s \n",pEnable,pIp,pProtocol,pStartPort,pEndPort,pPortDesc);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setmacFilt add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into MacFilter values ('%s','%s','%s')"
        ,gstrcmp(pEnable,"off")?"1":"0",pMacIp,pMacDesc);
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }
    else if (!gstrcmp(pEditMode,"edit"))
    {
        //printf("setmacFilt edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update MacFilter set Enable='%d', MacAddr='%s', Comment='%s' where _ROWID_='%s'",gstrcmp(pEnable,"off")?1:0,pMacIp,pMacDesc,pRowId);
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
        system("firewall.sh");
    }
    websRedirect(wp, T("advanced/firewall_macfilter.asp"));
}

static void setFwMacFiltDel(webs_t wp, char_t *path, char_t *query)
{
    char * pId = websGetVar(wp, T("macfilt_rowid_del"), T(""));
    //char * pDelId = websGetVar(wp, T("rowid_del"), T(""));
    //printf("pId: %s\n",pId);
	//printf("pDelId: %s\n",pDelId);

    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM MacFilter WHERE _ROWID_='%s'",pId);
        //printf("pPortDelQry: %s \n",pPortDelQry);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
            system("firewall.sh");
        }
    }

    websRedirect(wp, T("advanced/firewall_macfilter.asp"));
}


static int getFwUrlFiltEnable(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
		char * pQryResult = jnrp_sql_getAttr_Webs ("FirewallConfig", "UrlFilterEnable", 1);
#else
		char * pQryResult = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "UrlFilterEnable", 1);
#endif
		return websWrite(wp, T("%s"),pQryResult);
}

static void setFwUrlFiltEnable(webs_t wp, char_t *path, char_t *query)
{

	int nRows=0,nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "SELECT Enable FROM FirewallConfig";
    char_t * pEnable = websGetVar(wp, T("enurlfilt"), T("off"));
			
	char* pFirewallEnable = NULL;
#ifdef __USED_WEBS_DB__
	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
	sqlite3 * pWebDB = NULL;
	if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	}
	DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Enable", pFirewallEnable, getFirewallInfoOut);
	getFirewallInfoOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}

#ifdef __USED_WEBS_DB__
    if (!gstrcmp(pEnable,"on"))
    {
        jnrp_sql_setAttr_Webs ("FirewallConfig", "UrlFilterEnable", "1", 1);
		if(!gstrncmp(pFirewallEnable,"1",1) )
		{
			
		}
		else
		{
			jnrp_sql_setAttr_Webs ("FirewallConfig", "Enable", "1", 1); 
		}
    }
    else
	{
        jnrp_sql_setAttr_Webs ("FirewallConfig", "UrlFilterEnable", "0", 1);
	}
#else
    if (!gstrcmp(pEnable,"on"))
	{
        jnrp_sql_setAttr (pDBFile, "FirewallConfig", "UrlFilterEnable", "1", 1);
	}
    else
	{
        jnrp_sql_setAttr (pDBFile, "FirewallConfig", "UrlFilterEnable", "0", 1);
	}
#endif

    system("fwnat.sh");

    websRedirect(wp, T("advanced/firewall_urlfilter.asp"));
}

static int getFwUrlFilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from UrlFilter";

 int index = 0;
 char* pEnable = NULL;
 char* pUrl = NULL;
 char* pComment = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getFwUrlFiltOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Url", pUrl, getFwUrlFiltOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Comment", pComment, getFwUrlFiltOut);

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    }
    websWrite(wp, T("<td align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"url_ck\" id=\"url_ck_%d\" onclick=\"setUrlEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"url_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\"><span id=\"url_addr_%d\">%s</span></td>\n"), index+1, pUrl);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"url_desc_%d\">%s</td>\n"), index+1, pComment);
#else
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
    }
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"url_ck\" id=\"url_ck_%d\" onclick=\"setUrlEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"url_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\"><span id=\"url_addr_%d\">%s</span></td>\n"), index+1, pUrl);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"url_desc_%d\">%s</td>\n"), index+1, pComment);
#endif
  }
 }

getFwUrlFiltOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
	sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    //char   sqlCmd [1024] = {0};
    char * sqlCmd = "select * from UrlFilter";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    //snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);
    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    // get column numbers
    //int count = sqlite3_column_count(stmt);
    while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
        //char* data = (const char*)sqlite3_column_text( stmt, i );
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"url_ck\" id=\"url_ck_%d\" onclick=\"setUrlEditRowId('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td id=\"url_enable_%d\">%s</td>\n"),row_count,
        gatoi((const char*)sqlite3_column_text( stmt, 0 ))?"Enabled":"Disabled");
        websWrite(wp, T("<td><span id=\"url_addr_%d\">%s</span></td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 1));       
        websWrite(wp, T("<td id=\"url_desc_%d\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 2 ));
    }
    //printf("row_count:%d \n",row_count);
    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    
    return 0;
#endif
}

static void setFwUrlFilt(webs_t wp, char_t *path, char_t *query)
{
	char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char_t * pRowId = websGetVar(wp, T("url_rowid_edit"), T(""));
    //printf("editflag:%s ,pRowId: %s \n",pEditMode,pRowId);

    char * pErrMsg = NULL;
    int status = -1;

    char_t * pEnable = websGetVar(wp, T("url_enable"), T("off"));
    char_t * pUrlAddr = websGetVar(wp, T("url_addr"), T(""));    
    char_t * pUrlDesc = websGetVar(wp, T("url_desc"), T(""));
    //printf("pEnable:%s , pIp:%s, pProtocol:%s, pStartPort:%s, pEndPort:%s, pPortDesc:%s \n",pEnable,pIp,pProtocol,pStartPort,pEndPort,pPortDesc);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setUrlFilt add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into UrlFilter values ('%s','%s','%s')"
        ,gstrcmp(pEnable,"off")?"1":"0",pUrlAddr,pUrlDesc);
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }
    else if (!gstrcmp(pEditMode,"edit"))
    {
        //printf("setUrlFilt edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update UrlFilter set Enable='%d', Url='%s', Comment='%s' where _ROWID_='%s'",gstrcmp(pEnable,"off")?1:0,pUrlAddr,pUrlDesc,pRowId);
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
        system("firewall.sh");
    }
    websRedirect(wp, T("advanced/firewall_urlfilter.asp"));
}

static void setFwUrlFiltDel(webs_t wp, char_t *path, char_t *query)
{
    char * pId = websGetVar(wp, T("url_rowid_del"), T(""));
    //char * pDelId = websGetVar(wp, T("rowid_del"), T(""));
    printf("pId: %s\n",pId);
	//printf("pDelId: %s\n",pDelId);
	
    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM UrlFilter WHERE _ROWID_='%s'",pId);
        //printf("pPortDelQry: %s \n",pPortDelQry);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
            system("firewall.sh");
        }
    }

    websRedirect(wp, T("advanced/firewall_urlfilter.asp"));
}

#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
static int getFirewallConfig (int eid, webs_t wp, int argc, char_t **argv)
{
 char pFirewallConfig[256];

 int nRows=0,nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from SPI";

 char* pSPIenable = NULL;
 memset(pFirewallConfig,'\0',sizeof(pFirewallConfig));

 //SPI
#ifdef __USED_WEBS_DB__
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
 sqlite3 * pWebDB = NULL;
 if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   return websWrite(wp, T("\"%s\""),pFirewallConfig);
 }

 //Ignore ICMP Ping
#ifdef __USED_WEBS_DB__
	char * pICMPblock = jnrp_sql_getAttr_Webs ("FirewallConfig", "ICMPBLOCK ", 1);
#else
    char * pICMPblock = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "ICMPBLOCK ", 1);
#endif

 //SYN Flood
#ifdef __USED_WEBS_DB__
	char * pSYNflood = jnrp_sql_getAttr_Webs ("FirewallConfig", "TCPSynFlood ", 1);
#else
    char * pSYNflood = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "TCPSynFlood ", 1);
#endif

 //FIN Flood
#ifdef __USED_WEBS_DB__
	char * pFINflood = jnrp_sql_getAttr_Webs ("FirewallConfig", "TCPFinFlood ", 1);
#else
    char * pFINflood = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "TCPFinFlood ", 1);
#endif 

  //SYN Flood enable
#ifdef __USED_WEBS_DB__
	char * pSYNenable = jnrp_sql_getAttr_Webs ("FirewallConfig", "TCPSynEnable ", 1);
#else
    char * pSYNenable = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "TCPSynEnable ", 1);
#endif 

 //FIN Flood enable
#ifdef __USED_WEBS_DB__
	char * pFINenable = jnrp_sql_getAttr_Webs ("FirewallConfig", "TCPFinEnable ", 1);
#else
    char * pFINenable = jnrp_sql_getAttr (pDBFile,"FirewallConfig", "TCPFinEnable ", 1);
#endif 

 if (nRows > 0) {
   DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Enable", pSPIenable, getFirewallConfigOut);
   gstrcat(pFirewallConfig,pSPIenable);  //0
   gstrcat(pFirewallConfig,pSep);
   gstrcat(pFirewallConfig,pICMPblock);  //1
   gstrcat(pFirewallConfig,pSep);
   gstrcat(pFirewallConfig,pSYNflood);   //2
   gstrcat(pFirewallConfig,pSep);
   gstrcat(pFirewallConfig,pFINflood);   //3
   gstrcat(pFirewallConfig,pSep);
   gstrcat(pFirewallConfig,pSYNenable);  //4
   gstrcat(pFirewallConfig,pSep);
   gstrcat(pFirewallConfig,pFINenable);  //5
   gstrcat(pFirewallConfig,pSep);
 }

getFirewallConfigOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return websWrite(wp, T("\"%s\""),pFirewallConfig);
}

static void setFirewallConf(webs_t wp, char_t *path, char_t *query)
{
  char_t * pSPIenable = websGetVar(wp, T("spi_value"), T("off"));  //SPI
  char_t * pICMPblock = websGetVar(wp, T("icmp_value"), T("off")); //ICMP Ping
  char_t * pSYNFlood = websGetVar(wp, T("syn_flood"), T(""));      //SYN Flood
  char_t * pFINFlood = websGetVar(wp, T("fin_flood"), T("")); 	   //FIN Flood
  char_t * pSYNenable = websGetVar(wp, T("syn_value"), T("off"));  //SYN Flood Enable
  char_t * pFINenable = websGetVar(wp, T("fin_value"), T("off"));  //FIN Flood Enable
  
  int nSPIenable = 0;
  int nICMPblock = 0;
  int nSYNenable = 0;
  int nFINenable = 0;
  
  char pFirewallConfQry [1024] = {0};
  memset (pFirewallConfQry, 0x00, sizeof(pFirewallConfQry));
  int status = -1;

  //SPI
  if (!gstrcmp(pSPIenable,"1")){
  	nSPIenable=1;
  }else{
  	nSPIenable=0;
  }	
  snprintf (pFirewallConfQry, sizeof(pFirewallConfQry),"update SPI set Enable='%d'" ,nSPIenable);

  #ifdef __USED_WEBS_DB__
      status = jnrp_sql_dbEXE_Webs (pFirewallConfQry);
  #else
      status = jnrp_sql_dbEXE (pDBFile, pFirewallConfQry);
  #endif

  memset (pFirewallConfQry, 0x00, sizeof(pFirewallConfQry));
  //ICMP Ping
  if (!gstrcmp(pICMPblock,"1")){
  	nICMPblock=1;
  }else{
  	nICMPblock=0;
  }	

  //SYN Flood Enable
  if (!gstrcmp(pSYNenable,"1")){
  	nSYNenable=1;
  }else{
  	nSYNenable=0;
  }	

  //FIN Flood Enable
  if (!gstrcmp(pFINenable,"1")){
  	nFINenable=1;
  }else{
  	nFINenable=0;
  }	

  snprintf (pFirewallConfQry, sizeof(pFirewallConfQry),"update FirewallConfig set ICMPBLOCK='%d',"
	"TCPSynFlood='%s', TCPFinFlood='%s', TCPSynEnable='%d', TCPFinEnable='%d'" 
  ,nICMPblock, pSYNFlood, pFINFlood, nSYNenable, nFINenable);
  
  #ifdef __USED_WEBS_DB__
      status = jnrp_sql_dbEXE_Webs (pFirewallConfQry);
  #else
      status = jnrp_sql_dbEXE (pDBFile, pFirewallConfQry);
  #endif
  
  if (status>=0)
  {
  	// export db to ascii
  	system(EXPORT_CMD);
	system("firewall.sh");
  }  
  
  websRedirect(wp, T("advanced/firewall.asp"));	
}

#endif

static int getFwNatEnable (int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	char * pNatEnable = (char *)malloc(4);
	memset (pNatEnable, 0x00, sizeof(pNatEnable));
	strcpy(pNatEnable,"");
#ifdef __USED_WEBS_DB__
		char * pTmp = jnrp_sql_getAttr_Webs ("FirewallNatConfig", "Enable", 1);
#else
		char * pTmp = jnrp_sql_getAttr (pDBFile, "FirewallNatConfig", "Enable",1);
#endif
	
	if(pTmp)
		strcpy(pNatEnable,pTmp);
	else
		strcpy(pNatEnable,"0");

	ret = websWrite(wp, T("\"%s\""),pNatEnable);
	free(pNatEnable);
	return ret;
}
static int getFwNatConf(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
    char *pNatEnables = (char *)malloc(128);
    memset (pNatEnables, 0x00, sizeof(pNatEnables));
    char * pQry = "select * from FirewallNatConfig";
#ifdef __USED_WEBS_DB__
    char ** ppQryResult = jnrp_sql_getByQry_Webs (pQry);
#else
    char ** ppQryResult = jnrp_sql_getByQry (pDBFile, pQry);
#endif
    int i =0;
    int nTblSize = 6;
    strcpy(pNatEnables,"");

    if (ppQryResult)
    {
        while(ppQryResult[i] && (i<nTblSize))
        {
            //printf("ppQryResult[%d]: %s\n",i,ppQryResult[i]);
            strcat(pNatEnables,ppQryResult[i]);
            strcat(pNatEnables,pSep);
            i++;
        }
    }

    char * pDmzEnbale = (char *)malloc(128);
    memset (pDmzEnbale, 0x00, sizeof(pDmzEnbale));
#ifdef __USED_WEBS_DB__
    char * pTmp = jnrp_sql_getAttr_Webs ("FirewallDmz", "Enable", 1);
#else
    char * pTmp = jnrp_sql_getAttr (pDBFile, "FirewallDmz", "Enable",1);
#endif

    if(pTmp)
        strcpy(pDmzEnbale,pTmp);
    else
        strcpy(pDmzEnbale,"0");
    if (pDmzEnbale)
    {
        strcat(pNatEnables,pDmzEnbale);
        strcat(pNatEnables,pSep);
    }

    //printf("pNatEnables: %s \n",pNatEnables);
    ret = websWrite(wp, T("\"%s\""),pNatEnables);
	free(pNatEnables);
	free(pDmzEnbale);
    return ret; 
}

static void setFwNatConf(webs_t wp, char_t *path, char_t *query)
{
    //printf("setFwNatConf \n");
    char_t * pNatMode = websGetVar(wp, T("nat_mode"), T("off"));
    char_t * pPortFwdEnable = websGetVar(wp, T("port_forwd_enable"), T("off"));
    char_t * pDmzEnable = websGetVar(wp, T("dmz_enable"), T("off"));
#if !defined(__P02003_Dora)
	char_t * pReboogFlag = websGetVar(wp, T("reboot_flag"), T(""));
#endif	
    int nNatMode = 0, nPortFwdEnable=0, nDmzEnable=0;
	
    //printf("pNatMode: %s, pPortFwdEnable:%s, pDmzEnable:%s \n",pNatMode,pPortFwdEnable,pDmzEnable);
    char pNatConfQry [1024] = {0};
    memset (pNatConfQry, 0x00, sizeof(pNatConfQry));
	char pDhcpSerQry [1024] = {0};
    memset (pDhcpSerQry, 0x00, sizeof(pDhcpSerQry));
	int status = -1;

    if (!gstrcmp(pNatMode,"1"))
    {
        nNatMode=1;
		nEnable = 1;
        if (!gstrcmp(pPortFwdEnable,"on"))
            nPortFwdEnable = 1;
        if (!gstrcmp(pDmzEnable,"on"))
            nDmzEnable = 1;
		
		snprintf (pNatConfQry, sizeof(pNatConfQry),"update FirewallNatConfig set Enable='%d', PortForwardingEnable='%d'"
        ,nNatMode,nPortFwdEnable);

		//printf("pNatConfQry: %s \n",pNatConfQry);
#ifdef __USED_WEBS_DB__
    	status = jnrp_sql_setAttr_Webs ("FirewallDmz","Enable",nDmzEnable ? "1" : "0" ,1);
#else
    	status = jnrp_sql_setAttr (pDBFile, "FirewallDmz","Enable",nDmzEnable ? "1" : "0" ,1);
#endif
    }else{
        nNatMode=5;
		nEnable = 0;
		snprintf (pNatConfQry, sizeof(pNatConfQry),"update FirewallNatConfig set Enable='%d'", nNatMode);
	}	
  	snprintf (pDhcpSerQry, sizeof(pDhcpSerQry),"update DhcpServerBasic set Enable='%d'" ,nEnable);
#ifdef __USED_WEBS_DB__
    status = jnrp_sql_dbEXE_Webs (pNatConfQry);
#else
    status = jnrp_sql_dbEXE (pDBFile, pNatConfQry);
#endif
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pNatConfQry);
		sqlite3_free (pDhcpSerQry);
        system(EXPORT_CMD);
        system("fwnat.sh");
    }
#if !defined(__P02003_Dora)	
	if(!gstrcmp(pReboogFlag,"reboot"))
	{
		websRedirect(wp, T("advanced/nat_general_wait.asp"));
	}
	else
	{
		websRedirect(wp, T("advanced/nat_general.asp"));
	}
#else
	websRedirect(wp, T("advanced/nat_general_wait.asp"));
#endif
}

static int getFwDmzConf(int eid, webs_t wp, int argc, char_t **argv)
{
   int ret = 0;
   char** ppQueryRes = NULL;
   int nRows=0,nCols=0;
   char* pQueryStr = NULL;
   char *pDmzEnables = (char *)malloc(128);
   memset (pDmzEnables, 0x00, sizeof(pDmzEnables));
   char * pQry = "select Enable,DmzIpAddress from FirewallDmz";
#ifdef __USED_WEBS_DB__
   char ** ppQryResult = jnrp_sql_getByQry_Webs (pQry);
#else
   sqlite3 * pWebDB = NULL;
   char ** ppQryResult = jnrp_sql_getByQry (pDBFile, pQry);
#endif

   char* pIpAddr = NULL;
   pQueryStr = "select * from Lan";
#ifdef __USED_WEBS_DB__
	  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
   if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
	  {
	    printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
	    sqlite3_free (pQueryStr);
		free(pDmzEnables);
	    return -1;
	  }

	  DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "IpAddress",pIpAddr, getAdvInfoOut);

    if (ppQryResult)
    {
        strcpy(pDmzEnables,ppQryResult[2]);
        strcat(pDmzEnables,pSep);
        strcat(pDmzEnables,ppQryResult[3]);
        strcat(pDmzEnables,pSep);
        strcat(pDmzEnables,pIpAddr);
    }

getAdvInfoOut:
	ret = websWrite(wp, T("\"%s\""),pDmzEnables);
	free(pDmzEnables);
    return ret;
}

static void setFwDmzConf(webs_t wp, char_t *path, char_t *query)
{
    char_t * pDmzEnable = websGetVar(wp, T("dmz_enable"), T(""));
    char_t * pDmzIp = websGetVar(wp, T("dmz_fullip"), T(""));

    char pDmzConfQry [1024] = {0};
    memset (pDmzConfQry, 0x00, sizeof(pDmzConfQry));
    int nDmzEnable = 0;
    if (!gstrcmp(pDmzEnable,"on"))
        nDmzEnable = 1;
	if(nDmzEnable == 1)
	{
    	snprintf (pDmzConfQry, sizeof(pDmzConfQry),"update FirewallDmz set Enable='%d', DmzIpAddress='%s'"
        	,nDmzEnable,pDmzIp);
	}
	else
	{
		snprintf (pDmzConfQry, sizeof(pDmzConfQry),"update FirewallDmz set Enable='%d'",nDmzEnable);
	}

#ifdef __USED_WEBS_DB__
    int status =  jnrp_sql_dbEXE_Webs (pDmzConfQry);
#else
    int status =  jnrp_sql_dbEXE (pDBFile, pDmzConfQry);
#endif
    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
#if defined(__P02003_Dora)

#else		
        system("nat.sh");
#endif		
    }

    websRedirect(wp, T("advanced/nat_dmz.asp"));
}

static int getDDNSConf(int eid, webs_t wp, int argc, char_t **argv)
{
    int ret = 0;
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0;
    char* pQueryStr = "SELECT * FROM ddnsConfig";

#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
        return -1;
    }

    char *pDDNSConf = (char *)malloc(128);
    gstrcpy(pDDNSConf,"");

    char_t *pEnalbe, *pType;
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Enable",pEnalbe, getDDNSConfOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "TypeName",pType, getDDNSConfOut);

    gstrcat(pDDNSConf,pEnalbe);
    gstrcat(pDDNSConf,pSep);
    gstrcat(pDDNSConf,pType);


getDDNSConfOut:
    ret = websWrite(wp, T("\"%s\""),pDDNSConf);
	free(pDDNSConf);
    return ret;
}

static int getDDNSList(int eid, webs_t wp, int argc, char_t **argv)
{
    int ret = 0;
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0;
    char* pQueryStr = "select ddns.* from ddns, ddnsconfig where ddns.typename=ddnsconfig.typename ";

#ifdef __USED_WEBS_DB__
    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
#else
    sqlite3 * pWebDB = NULL;
    if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pQueryStr, &nCols, &nRows)) == NULL)
#endif                                    
    {
        printf ("sqlite3QueryResGet failed: query=%s\n",pQueryStr);
        sqlite3_free (pQueryStr);
        return -1;
    }

    char *pDDNSList = (char *)malloc(128);
    gstrcpy(pDDNSList,"");

    char_t *pUserName, *pPass, *pHostName;
    //sqlGetTablePrint(ppQueryRes,nCols,nRows);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "userName",pUserName, getDDNSListOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "pass",pPass, getDDNSListOut);
    DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "hostName",pHostName, getDDNSListOut);

    gstrcat(pDDNSList,pUserName);
    gstrcat(pDDNSList,pSep);
    gstrcat(pDDNSList,pPass);
    gstrcat(pDDNSList,pSep);
    gstrcat(pDDNSList,pHostName);

getDDNSListOut:
    ret = websWrite(wp, T("\"%s\""), pDDNSList);
    free(pDDNSList);
    return ret;
}

static void setDDNSConf(webs_t wp, char_t *path, char_t *query)
{
    char_t * pDDNSEnable = websGetVar(wp, T("chkDdns"), T("off"));

    char * pErrMsg = NULL;
    int status = -1;

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    #ifndef __USED_DB_TRIGGER__
    //status = sqlite3_exec (pDB, "BEGIN;", NULL, NULL, &pErrMsg);
    #endif


    if (!gstrcmp(pDDNSEnable,"off") && pDDNSEnable)
    {
        status = sqlite3_exec (pDB, "update ddnsconfig set Enable='0' where _ROWID_='1'", NULL, NULL, &pErrMsg);
    }
    else
    {
        char_t * pDDNSType = websGetVar(wp, T("selDdnsSrv"), T(""));
        if (pDDNSType)
        {
            char pDDNSConfQry [1024] = {0};
            memset (pDDNSConfQry, 0x00, sizeof(pDDNSConfQry));
            snprintf(pDDNSConfQry,sizeof(pDDNSConfQry), "update ddnsconfig set Enable='1', TypeName='%s' where _ROWID_='1'",pDDNSType);
            status = sqlite3_exec (pDB, pDDNSConfQry, NULL, NULL, &pErrMsg);
            char_t * pHost = websGetVar(wp, T("txtDomainName"), T(""));
            char_t * pUser = websGetVar(wp, T("txtUserName"), T(""));
            char_t * pPasswd = websGetVar(wp, T("txtPwd"), T(""));
            char pDDNSQry [1024] = {0};
            memset (pDDNSQry, 0x00, sizeof(pDDNSQry));
            snprintf(pDDNSQry,sizeof(pDDNSQry),
            "update ddns set userName='%s', pass='%s', hostName='%s' where TypeName='%s'"
            ,pUser,pPasswd,pHost,pDDNSType);
            status = sqlite3_exec (pDB, pDDNSQry, NULL, NULL, &pErrMsg);
        }

    }

    #ifndef __USED_DB_TRIGGER__
    // remove if use DB trigger
    //status = sqlite3_exec (pDB, "COMMIT;", NULL, NULL, &pErrMsg);
    #endif


    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }

    websRedirect(wp, T("advanced/advanced_ddns.asp"));
}
#if defined(__P02003_Dora) //temp
static int getPortFwdStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int err_status = check_portforwarding_status;
	check_portforwarding_status = portforwarding_status_none;
	return websWrite(wp,T("\"%d\""),err_status);
}

static int getPortFwd(int eid, webs_t wp, int argc, char_t **argv)
{
	if (port_fwd_idx==-1) //add
	{
		return websWrite(wp, T("\"%s\""),"");
	}
	else if (port_fwd_idx>0) //edit
	{
		int ret = 0;
		char * pServiceName = jnrp_sql_getAttr_Webs("PortForwarding","ServiceName",port_fwd_idx);
		char * pIp = jnrp_sql_getAttr_Webs("PortForwarding","Ip",port_fwd_idx);
		char * pEnable = jnrp_sql_getAttr_Webs("PortForwarding","DNATPortEnable",port_fwd_idx);
		char * pPort = jnrp_sql_getAttr_Webs("PortForwarding","DNATPort",port_fwd_idx);
		char* pPortRwdItem = (char *)malloc(128);
		memset(pPortRwdItem, '\0', 128);
		gstrcat(pPortRwdItem, pServiceName);
		gstrcat(pPortRwdItem, pSep);
		gstrcat(pPortRwdItem, pIp);
		gstrcat(pPortRwdItem, pSep);
		gstrcat(pPortRwdItem, pEnable);
		gstrcat(pPortRwdItem, pSep);
		gstrcat(pPortRwdItem, pPort);
		ret = websWrite(wp, T("\"%s\""), pPortRwdItem);
		free(pPortRwdItem);
		return ret;
	}
}


static int getPortFwdList(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from PortForwarding";

 int index = 0;
 char* pEnable = NULL;
 char* pIp = NULL;
 char* pPort = NULL;
 char* pServiceName = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getPortFwdOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Ip", pIp, getPortFwdOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DNATPortEnable", pEnable, getPortFwdOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DNATPort", pPort, getPortFwdOut);
    websWrite(wp, T("<tr>"));
	if (index%2)
	{
		websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"port_forward_ck\" id=\"port_forward_ck_%d\" /></td>\n"),index+1);
		websWrite(wp, T("<td class=\"tdOdd\"><span id=\"port_forward_service_name_%d\">%s</span></td>\n"),index+1, pServiceName);
		websWrite(wp, T("<td class=\"tdOdd\" id=\"port_forward_ip_%d\">%s</td>\n"),index+1, pIp);
		if (gatoi(pEnable) == 1)
			websWrite(wp, T("<td class=\"tdOdd\" id=\"port_forward_port_%d\">%s</td>\n"),index+1, pPort);
		else
			websWrite(wp, T("<td class=\"tdOdd\" id=\"port_forward_port_%d\">--</td>\n"),index+1);
		websWrite(wp, T("<td class=\"tdOdd\"><input type=\"hidden\" id=\"port_forward_enable_%d\" value=\"%s\"/></td>\n"),index+1, pEnable);
	}
	else
	{
		websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"port_forward_ck\" id=\"port_forward_ck_%d\" /></td>\n"),index+1);
		websWrite(wp, T("<td class=\"tdEven\"><span id=\"port_forward_service_name_%d\">%s</span></td>\n"),index+1, pServiceName);
		websWrite(wp, T("<td class=\"tdEven\" id=\"port_forward_ip_%d\">%s</td>\n"),index+1, pIp);
		if (gatoi(pEnable) == 1)
			websWrite(wp, T("<td class=\"tdEven\" id=\"port_forward_port_%d\">%s</td>\n"),index+1, pPort);
		else 
			websWrite(wp, T("<td class=\"tdEven\" id=\"port_forward_port_%d\">--</td>\n"),index+1);
		websWrite(wp, T("<td class=\"tdEven\"><input type=\"hidden\" id=\"port_forward_enable_%d\" value=\"%s\"/></td>\n"),index+1, pEnable);
	}

  }
 }

getPortFwdOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    return 0;
#endif
}

#else
static int getPortFwd(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from PortForwarding";

 int index = 0;
 char* pEnable = NULL;
 char* pIp = NULL;
 char* pProtocol = NULL;
 char* pportRange = NULL;
 char* pComment = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getPortFwdOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Ip", pIp, getPortFwdOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Protocol", pProtocol, getPortFwdOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "portRange", pportRange, getPortFwdOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Comment", pComment, getPortFwdOut);
#if defined(__P02001_Dora) || defined(__P02002_Dora) || defined(__P02003_Dora)
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    }
    websWrite(wp, T("<td align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"port_ck\" id=\"port_ck_%d\" onclick=\"setPortEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"port_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"port_ip_%d\">%s</span></td>\n"), index+1, pIp);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"port_protocol_%d\">%s</td>\n"), index+1, pProtocol);
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"port_range_%d\">%s ~ %s</td>\n"), index+1, getSplitStr(pportRange, ":", 1), getSplitStr(pportRange, ":", 2));
    websWrite(wp, T("<td align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"port_desc_%d\">%s</td>\n"), index+1, pComment);
#else
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
    }
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"port_ck\" id=\"port_ck_%d\" onclick=\"setPortEditRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"port_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\"><span id=\"port_ip_%d\">%s</span></td>\n"), index+1, pIp);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"port_protocol_%d\">%s</td>\n"), index+1, pProtocol);
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" id=\"port_range_%d\">%s ~ %s</td>\n"), index+1, getSplitStr(pportRange, ":", 1), getSplitStr(pportRange, ":", 2));
    websWrite(wp, T("<td class=\"list_td\" align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"port_desc_%d\">%s</td>\n"), index+1, pComment);
#endif
  }
 }

getPortFwdOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    //char   sqlCmd [1024] = {0};
    char * sqlCmd = "select * from PortForwarding";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    //snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);
    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    // get column numbers
    //int count = sqlite3_column_count(stmt);
    while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
        //char* data = (const char*)sqlite3_column_text( stmt, i );
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"port_ck\" id=\"port_ck_%d\" onclick=\"setPortEditRowId('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td id=\"port_enable_%d\">%s</td>\n"),row_count,
        gatoi((const char*)sqlite3_column_text( stmt, 0 ))?"Enabled":"Disabled");
        websWrite(wp, T("<td><span id=\"port_ip_%d\">%s</span></td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 1));
        websWrite(wp, T("<td id=\"port_protocol_%d\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 2 ));
        websWrite(wp, T("<td id=\"port_range_%d\">%s ~ %s</td>\n"),row_count,
        getSplitStr((const char*)sqlite3_column_text( stmt, 3 ),":",1)
        ,getSplitStr((const char*)sqlite3_column_text( stmt, 3 ),":",2));
        websWrite(wp, T("<td id=\"port_desc_%d\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 4 ));
    }
    //printf("row_count:%d \n",row_count);
    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    
    return 0;
#endif
}
#endif

static int getPortFwdEnable(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("FirewallNatConfig", "PortForwardingEnable", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"FirewallNatConfig", "PortForwardingEnable", 1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

static int setPortFwdEnable(webs_t wp, char_t *path, char_t *query)
{
    //printf("setPortFwdEnable: \n");
    char_t * pEnable = websGetVar(wp, T("enpoford"), T("off"));

#ifdef __USED_WEBS_DB__
    if (!gstrcmp(pEnable,"on"))
        jnrp_sql_setAttr_Webs ("FirewallNatConfig", "PortForwardingEnable", "1", 1);
    else
        jnrp_sql_setAttr_Webs ("FirewallNatConfig", "PortForwardingEnable", "0", 1);
#else
    if (!gstrcmp(pEnable,"on"))
        jnrp_sql_setAttr (pDBFile, "FirewallNatConfig", "PortForwardingEnable", "1", 1);
    else
        jnrp_sql_setAttr (pDBFile, "FirewallNatConfig", "PortForwardingEnable", "0", 1);
#endif

    system("fwnat.sh");

    websRedirect(wp, T("advanced/nat_portforwarding.asp"));
}

static int getPortFwdEdit(int eid, webs_t wp, int argc, char_t **argv)
{
    int ret = 0;
    char** ppQueryRes = NULL;
    int nRows=0,nCols=0;
    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));
    char *pPortEdit = (char *)malloc(128);
    gstrcpy(pPortEdit,"");
    if (argv[0])
    {
        snprintf(pPortQry,sizeof(pPortQry),"select * from PortForwarding where _ROWID_='%s'",argv[0]);
#ifdef __USED_WEBS_DB__
        if ((ppQueryRes = sqlite3QueryResGet_Webs (pPortQry, &nCols, &nRows)) == NULL)
#else
        sqlite3 * pWebDB = NULL;
        if ((ppQueryRes = sqlite3QueryResGet (pWebDB, pDBFile, pPortQry, &nCols, &nRows)) == NULL)        	
#endif
        {
            printf ("sqlite3QueryResGet failed: query=%s\n",pPortQry);
            sqlite3_free (pPortQry);
			free(pPortEdit);
            return -1;
        }

        char_t *pEnable, *pIp, *pProtocol, *pRange, *pComment;
        DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Enable",pEnable, getPortFwdEditOut);
        DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Ip",pIp, getPortFwdEditOut);
        DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Protocol",pProtocol, getPortFwdEditOut);
        DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "portRange",pRange, getPortFwdEditOut);
        DB_TABLE_VALUE_GET (ppQueryRes, nCols, 0, "Comment",pComment, getPortFwdEditOut);

        gstrcat(pPortEdit,pEnable);
        gstrcat(pPortEdit,pSep);
        gstrcat(pPortEdit,pIp);
        gstrcat(pPortEdit,pSep);
        gstrcat(pPortEdit,pRange);
        gstrcat(pPortEdit,pSep);
        gstrcat(pPortEdit,pComment);

    }
    else
    {
    	free(pPortEdit);
        return ret;
    }

getPortFwdEditOut:
    ret = websWrite(wp, T("\"%s\""), pPortEdit);
	free(pPortEdit);
    return ret;
}
#if defined(__P02003_Dora)
static void setPortFwdAction(webs_t wp, char_t *path, char_t *query)
{
	char * pPortFwdAction = websGetVar(wp, T("port_action"), T(""));	
	char_t * pRowId = websGetVar(wp, T("port_rowid"), T(""));
	if (!gstrcmp(pPortFwdAction,"add"))
	{
		port_fwd_idx=-1;
		websRedirect(wp, T("advanced/nat_portforwarding_editor.asp"));
	}
	else if (!gstrcmp(pPortFwdAction,"edit"))
	{
		port_fwd_idx=gatoi(pRowId);
		websRedirect(wp, T("advanced/nat_portforwarding_editor.asp"));
	}
	else if (!gstrcmp(pPortFwdAction,"delete"))
	{
		char * tempRowId;
		tempRowId = strtok (pRowId, pSep2);
		int count = 0;
	    char * pErrMsg = NULL;
	    int status = -1;

		while (tempRowId != NULL)
		{
			if (gatoi(tempRowId) > 0)
		    {
		        sqlite3 * pDB = NULL;
		        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		        {
		            printf ("unable to open the DB file %s\n", pDBFile);
		            return NULL;
		        }
		        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

		        char pPortDelQry [1024] = {0};
		        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
				int row_id=gatoi(tempRowId)-count;
		        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM PortForwarding WHERE _ROWID_='%d'",row_id);
		        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
		        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

		        if (pDB)
		            sqlite3_close (pDB);

		        // export db to ascii
		        if (status>=0)
		        {
		            // export db to ascii
		            sqlite3_free (pErrMsg);
		            system(EXPORT_CMD); // use export lib in future
		            system("/usr/local/bin/forward.sh");
		        }
		    }
			tempRowId = strtok (NULL, pSep2);
			count++;
		}
		websRedirect(wp, T("advanced/nat_portforwarding.asp"));
	}
}

static void setPortFwd(webs_t wp, char_t *path, char_t *query)
{	
    char * pErrMsg = NULL;
    int status = -1;
	char ** ppQryResult = NULL;
    char_t * pService = websGetVar(wp, T("service"), T(""));
    char_t * pIp = websGetVar(wp, T("ip_address"), T(""));
    char_t * pEnable = websGetVar(wp, T("enable_port_translation"), T("off"));
    char_t * pPort = websGetVar(wp, T("port"), T(""));

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
	char pCheckQry [256] = {0};
	memset(pCheckQry, 0x00, sizeof(pCheckQry));
	check_portforwarding_status = portforwarding_status_none;
	if (port_fwd_idx==-1) //add
    {
        snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from PortForwarding where ServiceName='%s' and Ip='%s'"
			, pService,pIp);
	    ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
	    if (gatoi(ppQryResult[1]) > 0) 
	    {
	    	check_portforwarding_status = portforwarding_status_err;
			ppQryResult = NULL;
	 		websRedirect(wp, T("advanced/nat_portforwarding.asp"));
			return;
	    }
    }
   	else if (port_fwd_idx>0) //edit
    {
    	snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from PortForwarding where ServiceName='%s' and Ip='%s' and _ROWID_<>'%d'"
			, pService,pIp,port_fwd_idx);
	    ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
	    if (gatoi(ppQryResult[1]) > 0) 
	    {
	    	check_portforwarding_status = portforwarding_status_err;
			ppQryResult = NULL;
	 		websRedirect(wp, T("advanced/nat_portforwarding.asp"));
			return;
	    }
   	}
	
    char pPortQry [512] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));
    if (port_fwd_idx==-1) //add
    {
        snprintf(pPortQry,sizeof(pPortQry), "insert into PortForwarding values ('%s','%s','%s','%s','%s','%s','%s','%s')"
        ,"1",pIp,"0","0","0",pService,gstrcmp(pEnable,"off")?"1":"0",pPort);
        
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }
   else if (port_fwd_idx>0) //edit
    {
        snprintf(pPortQry,sizeof(pPortQry), "update PortForwarding set DNATPortEnable='%d', Ip='%s', ServiceName='%s'"
        ",DNATPort='%s' where _ROWID_='%d'",gstrcmp(pEnable,"off")?1:0,pIp,pService,pPort,port_fwd_idx);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }
    websRedirect(wp, T("advanced/nat_portforwarding.asp"));
}

#else
static void setPortFwd(webs_t wp, char_t *path, char_t *query)
{
    //printf("setPortFwd\n");
    char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char_t * pRowId = websGetVar(wp, T("port_rowid_edit"), T(""));
    //printf("editflag:%s ,pRowId: %s \n",pEditMode,pRowId);

    char * pErrMsg = NULL;
    int status = -1;

    char_t * pEnable = websGetVar(wp, T("port_enable"), T("off"));
    char_t * pIp = websGetVar(wp, T("port_ip"), T(""));
    char_t * pProtocol = websGetVar(wp, T("protocol"), T(""));
    char_t * pStartPort = websGetVar(wp, T("start_port"), T(""));
    char_t * pEndPort = websGetVar(wp, T("end_port"), T(""));
    char_t * pPortDesc = websGetVar(wp, T("port_desc"), T(""));
    //printf("pEnable:%s , pIp:%s, pProtocol:%s, pStartPort:%s, pEndPort:%s, pPortDesc:%s \n",pEnable,pIp,pProtocol,pStartPort,pEndPort,pPortDesc);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setPortFwd add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into PortForwarding values ('%s','%s','%s','%s:%s','%s','%s','%s','%s')"
        ,gstrcmp(pEnable,"off")?"1":"0",pIp,pProtocol,pStartPort,pEndPort,pPortDesc,"0","0","0");
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }
    else if (!gstrcmp(pEditMode,"edit"))
    {
        //printf("setPortFwd edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update PortForwarding set Enable='%d', Ip='%s', Protocol='%s'"
        ",portRange='%s:%s',Comment='%s' where _ROWID_='%s'",gstrcmp(pEnable,"off")?1:0,pIp,pProtocol,pStartPort,pEndPort,pPortDesc,pRowId);
        //printf("pPortQry: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
        system("nat.sh");
    }
    websRedirect(wp, T("advanced/nat_portforwarding.asp"));
}
#endif
#if defined(__P02003_Dora) //temp
static void setPortFwdDel(webs_t wp, char_t *path, char_t *query)
{
    char * tempRowId;
    char_t * pRowId = websGetVar(wp, T("port_rowid_del"), T(""));
	tempRowId = strtok (pRowId, pSep2);
	int count = 0;
    char * pErrMsg = NULL;
    int status = -1;

	while (tempRowId != NULL)
	{
		if (gatoi(tempRowId) > 0)
	    {
	        sqlite3 * pDB = NULL;
	        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
	        {
	            printf ("unable to open the DB file %s\n", pDBFile);
	            //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
	            return NULL;
	        }
	        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

	        char pPortDelQry [1024] = {0};
	        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
			int row_id=gatoi(tempRowId)-count;
	        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM PortForwarding WHERE _ROWID_='%d'",row_id);
	        //printf("pPortDelQry: %s \n",pPortDelQry);
	        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
	        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

	        if (pDB)
	            sqlite3_close (pDB);

	        // export db to ascii
	        if (status>=0)
	        {
	            // export db to ascii
	            sqlite3_free (pErrMsg);
	            system(EXPORT_CMD); // use export lib in future
	            system("/usr/local/bin/forward.sh");
	        }
	    }
		tempRowId = strtok (NULL, pSep2);
		count++;
	}
    websRedirect(wp, T("advanced/nat_portforwarding.asp"));
}
#else
static void setPortFwdDel(webs_t wp, char_t *path, char_t *query)
{
    char_t * pId = websGetVar(wp, T("port_rowid_del"), T(""));
    //printf("pId: %s\n",pId);

    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM PortForwarding WHERE _ROWID_='%s'",pId);
        //printf("pPortDelQry: %s \n",pPortDelQry);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
            system("nat.sh");
        }
    }

    websRedirect(wp, T("advanced/nat_portforwarding.asp"));
}
#endif
static int getStaRout(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from StaticRoute";

 int index = 0;
 char* pEnable = NULL;
 char* pName = NULL;
 char* pDestineNetwork = NULL;
 char* pDestineNetmask = NULL;
 char* pLocal = NULL;
 char* pLocalGateway = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getStaRoutOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Name", pName, getStaRoutOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DestineNetwork", pDestineNetwork, getStaRoutOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DestineNetmask", pDestineNetmask, getStaRoutOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Local", pLocal, getStaRoutOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "LocalGateway", pLocalGateway, getStaRoutOut);
#if defined(__P02001_Dora) || defined(__P02002_Dora)|| defined(__P02003_Dora)
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#D9D9D9\" align=\"center\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#D9D9D9\" align=\"center\">"));  //#EEEEEE
    }
  #if defined(__P02003_Dora)
    websWrite(wp, T("<td class=\"tdOdd w50\"><input type=\"checkbox\" name=\"staticroute.checkbox\" id=\"starout_ck_%d\"></td>\n"), index+1);
    websWrite(wp, T("<td class=\"tdOdd w50\"><span id=\"starout_name_%d\">%s</span></td>\n"), index+1, pName);
    websWrite(wp, T("<td class=\"tdOdd w50\"><span id=\"starout_dest_net_%d\">%s</span></td>\n"), index+1, pDestineNetwork);
    websWrite(wp, T("<td class=\"tdOdd w50\"><span id=\"starout_dest_msk_%d\">%s</span></td>\n"), index+1, pDestineNetmask);
	if (!gstrcmp(pLocal,"Default")){
      websWrite(wp, T("<td class=\"tdOdd w50\"><span id=\"starout_lcl_%d\">%s</span></td>\n"), index+1, "Local Network");
	}else if (!gstrcmp(pLocal,"WAN1")) { 
	  websWrite(wp, T("<td class=\"tdOdd w50\"><span id=\"starout_lcl_%d\">%s</span></td>\n"), index+1, "Internet");
 	}
    websWrite(wp, T("<td class=\"tdOdd w50\"><span id=\"starout_lcl_gw_%d\">%s</span></td></tr>\n"), index+1, pLocalGateway);
  #else
    websWrite(wp, T("<td><input type=\"radio\" name=\"starout_ck\" id=\"starout_ck_%d\" onclick=\"setStaticRouteRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td id=\"starout_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td id=\"starout_name_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"), index+1, pName);
    websWrite(wp, T("<td id=\"starout_dest_net_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"), index+1, pDestineNetwork);
    websWrite(wp, T("<td id=\"starout_dest_msk_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"), index+1, pDestineNetmask);
    //websWrite(wp, T("<td id=\"starout_lcl_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"), index+1, pLocal);
    websWrite(wp, T("<td id=\"starout_lcl_gw_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td></tr>\n"), index+1, pLocalGateway);
  #endif	
#else
    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#F6F6F6\" align=\"center\">"));
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#EEEEEE\" align=\"center\">"));
    }
    websWrite(wp, T("<td class=\"list_td\" ><input type=\"radio\" name=\"starout_ck\" id=\"starout_ck_%d\" onclick=\"setStaticRouteRowId('%d')\"/></td>\n"), index+1, index+1);
    websWrite(wp, T("<td class=\"list_td\" id=\"starout_enable_%d\">%s</td>\n"), index+1, gatoi(pEnable) ? "Enabled" : "Disabled");
    websWrite(wp, T("<td class=\"list_td\" ><span id=\"starout_name_%d\">%s</span></td>\n"), index+1, pName);
    websWrite(wp, T("<td class=\"list_td\" id=\"starout_dest_net_%d\">%s</td>\n"), index+1, pDestineNetwork);
    websWrite(wp, T("<td class=\"list_td\" id=\"starout_dest_msk_%d\">%s</td>\n"), index+1, pDestineNetmask);
    websWrite(wp, T("<td class=\"list_td\" id=\"starout_lcl_%d\">%s</td>\n"), index+1, pLocal);
    websWrite(wp, T("<td class=\"list_td\" id=\"starout_lcl_gw_%d\">%s</td></tr>\n"), index+1, pLocalGateway);
#endif
	websWrite(wp, T("</tr>"));
  }
 }

getStaRoutOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    //char   sqlCmd [1024] = {0};
    char * sqlCmd = "select * from StaticRoute";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    //snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);
    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    // get column numbers
    //int count = sqlite3_column_count(stmt);
    while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
        //char* data = (const char*)sqlite3_column_text( stmt, i );
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\" align=\"center\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\" align=\"center\">"));

        websWrite(wp, T("<td><input type=\"radio\" name=\"starout_ck\" id=\"starout_ck_%d\" onclick=\"setStaticRouteRowId('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td id=\"starout_enable_%d\">%s</td>\n"),row_count,
        gatoi((const char*)sqlite3_column_text( stmt, 0 ))?"Enabled":"Disabled");
        websWrite(wp, T("<td id=\"starout_name_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 1));
        websWrite(wp, T("<td id=\"starout_dest_net_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 2 ));
        websWrite(wp, T("<td id=\"starout_dest_msk_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 3 ));
        //websWrite(wp, T("<td id=\"starout_lcl_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td>\n"),row_count,
        //(const char*)sqlite3_column_text( stmt, 4 ));
        websWrite(wp, T("<td id=\"starout_lcl_gw_%d\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\">%s</td></tr>\n"),row_count,
        (const char*)sqlite3_column_text( stmt, 5 ));
    }
    //printf("row_count:%d \n",row_count);
    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();

    return 0;
#endif
}

static int getLANGatewayStartIP(int eid, webs_t wp, int argc, char_t **argv)
{
	char cmdbuffer[100] = {0};
    char buffer[1024] = {0};
    FILE *fd = -1;
    	
    memset(cmdbuffer, '\0', sizeof(cmdbuffer));
    memset(buffer, '\0', sizeof(buffer));

    char * pQry = "select IpAddress, SubnetMask from Lan";
    char ** ppQryResult = jnrp_sql_getByQry_Webs (pQry);

	sprintf(cmdbuffer,"ipcalc.sh %s %s | grep 'NETWORK' | sed 's/NETWORK=//g'", ppQryResult[2],ppQryResult[3]);
    fd = popen(cmdbuffer,"r");
    if(fgets(buffer,sizeof(buffer),fd) == NULL){
      websWrite(wp, T("\"%s\""),"");
    }else{
      if (buffer[strlen(buffer)-1] == '\n')
		buffer[strlen(buffer)-1] = '\0';			
	  websWrite(wp, T("\"%s\""),buffer);		
    }
    pclose(fd);	
    return 0;	
}

static int getLANGatewayEndIP(int eid, webs_t wp, int argc, char_t **argv)
{
	char cmdbuffer[100] = {0};
    char buffer[1024] = {0};
    FILE *fd = -1;
    	
    memset(cmdbuffer, '\0', sizeof(cmdbuffer));
    memset(buffer, '\0', sizeof(buffer));

    char * pQry = "select IpAddress, SubnetMask from Lan";
    char ** ppQryResult = jnrp_sql_getByQry_Webs (pQry);

	sprintf(cmdbuffer,"ipcalc.sh %s %s | grep 'BROADCAST' | sed 's/BROADCAST=//g'", ppQryResult[2],ppQryResult[3]);
    fd = popen(cmdbuffer,"r");
    if(fgets(buffer,sizeof(buffer),fd) == NULL){
      websWrite(wp, T("\"%s\""),"");
    }else{
      if (buffer[strlen(buffer)-1] == '\n')
		buffer[strlen(buffer)-1] = '\0';			
	  websWrite(wp, T("\"%s\""),buffer);		
    }
    pclose(fd);	
    return 0;	
}

static int getInternetGatewayStartIP(int eid, webs_t wp, int argc, char_t **argv)
{
	system("lte_get --ip-status -q 0 --timeout 5000 > /tmp/getHomeInternetInfo2");
    FILE *fp = fopen("/tmp/getHomeInternetInfo2", "r");
    char line[128];
    char pIp[128];
    char pMask[128];
    memset(line, '\0', sizeof(line));
    memset(pIp, '\0', sizeof(pIp));
    memset(pMask, '\0', sizeof(pMask));
    
    if(fp) {
      while(fgets(line, sizeof(line), fp) != NULL){
        if (line[strlen(line)-1] == '\n')
          line[strlen(line)-1] = '\0';
        if (gstrstr(line,"CLIENT:ip address:")){
    			gstrcat(pIp,getSplitStr(line, ":", 3));     
        }
        if (gstrstr(line,"CLIENT:netmask:")){
    			gstrcat(pMask,getSplitStr(line, ":", 3));     
        }
      }
      fclose(fp);
    } else {
     	gstrcat(pIp,"");  
     	gstrcat(pMask,""); 
    }
	
	char cmdbuffer[100] = {0};
    char buffer[1024] = {0};
    FILE *fd = -1;
    	
    memset(cmdbuffer, '\0', sizeof(cmdbuffer));
    memset(buffer, '\0', sizeof(buffer));

	sprintf(cmdbuffer,"ipcalc.sh %s %s | grep 'NETWORK' | sed 's/NETWORK=//g'", pIp, pMask);
    fd = popen(cmdbuffer,"r");
    if(fgets(buffer,sizeof(buffer),fd) == NULL){
      websWrite(wp, T("\"%s\""),"");
    }else{
      if (buffer[strlen(buffer)-1] == '\n')
		buffer[strlen(buffer)-1] = '\0';			
	  websWrite(wp, T("\"%s\""),buffer);		
    }
    pclose(fd);	
    return 0;	
}

static int getInternetGatewayEndIP(int eid, webs_t wp, int argc, char_t **argv)
{
	system("lte_get --ip-status -q 0 --timeout 5000 > /tmp/getHomeInternetInfo2");
    FILE *fp = fopen("/tmp/getHomeInternetInfo2", "r");
    char line[128];
    char pIp[128];
    char pMask[128];
    memset(line, '\0', sizeof(line));
    memset(pIp, '\0', sizeof(pIp));
    memset(pMask, '\0', sizeof(pMask));
    
    if(fp) {
      while(fgets(line, sizeof(line), fp) != NULL){
        if (line[strlen(line)-1] == '\n')
          line[strlen(line)-1] = '\0';
        if (gstrstr(line,"CLIENT:ip address:")){
    			gstrcat(pIp,getSplitStr(line, ":", 3));     
        }
        if (gstrstr(line,"CLIENT:netmask:")){
    			gstrcat(pMask,getSplitStr(line, ":", 3));     
        }
      }
      fclose(fp);
    } else {
     	gstrcat(pIp,"");  
     	gstrcat(pMask,""); 
    }
	
	char cmdbuffer[100] = {0};
    char buffer[1024] = {0};
    FILE *fd = -1;
    	
    memset(cmdbuffer, '\0', sizeof(cmdbuffer));
    memset(buffer, '\0', sizeof(buffer));

	sprintf(cmdbuffer,"ipcalc.sh %s %s | grep 'BROADCAST' | sed 's/BROADCAST=//g'", pIp, pMask);
    fd = popen(cmdbuffer,"r");
    if(fgets(buffer,sizeof(buffer),fd) == NULL){
      websWrite(wp, T("\"%s\""),"");
    }else{
      if (buffer[strlen(buffer)-1] == '\n')
		buffer[strlen(buffer)-1] = '\0';			
	  websWrite(wp, T("\"%s\""),buffer);		
    }
    pclose(fd);	
    return 0;
}

static int getStaRoutEnable(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("StaticRoute", "Enable", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"StaticRoute", "Enable", 1);
#endif
    return websWrite(wp, T("%s"),pQryResult);
}

static void setStaRoutEnable(webs_t wp, char_t *path, char_t *query)
{
    //printf("setPortFwdEnable: \n");
    char_t * pEnable = websGetVar(wp, T("enstarout"), T("off"));
#ifdef __USED_WEBS_DB__
    if (!gstrcmp(pEnable,"on"))
        jnrp_sql_setAttr_Webs ("StaticRoute", "Enable", "1", 1);
    else
        jnrp_sql_setAttr_Webs ("StaticRoute", "Enable", "0", 1);
#else
    if (!gstrcmp(pEnable,"on"))
        jnrp_sql_setAttr (pDBFile, "StaticRoute", "Enable", "1", 1);
    else
        jnrp_sql_setAttr (pDBFile, "StaticRoute", "Enable", "0", 1);
#endif
    websRedirect(wp, T("advanced/advanced_staticroute.asp"));       
}

#if defined(__P02001_Dora) || defined(__P02002_Dora)|| defined(__P02003_Dora)
static int getSpecialAppsName(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from Services";

 int ret= 0;
 int index = 0;
 char* pServiceName = NULL;
 char* pSpecialAppsName = (char *)malloc(1024);
 memset(pSpecialAppsName,'\0', 1024);
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   free(pSpecialAppsName);
   return 0;
 }
 //printf("BpSpecialAppsName: nRows='%s', nCols='%s'\n", nRows, nCols);
 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getAppsListOut);
	  gstrcat(pSpecialAppsName,pServiceName);
      gstrcat(pSpecialAppsName,pSep);
	  printf("pSpecialAppsName:index: '%d', pServiceName\n", index, pServiceName);
	  //sleep(1);
    }
 }
 //printf("pSpecialAppsName:%s\n", pSpecialAppsName);
getAppsListOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }
 ret = websWrite(wp, T("\"%s\""), pSpecialAppsName);
 free(pSpecialAppsName);
 return ret;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    char * sqlCmd = "select * from Services";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

	int ret = 0;
	char* pSpecialAppsName = (char *)malloc(1024);
 	memset(pSpecialAppsName,'\0', 1024);
    
    while( sqlite3_step( stmt ) == SQLITE_ROW ){
		gstrcat(pSpecialAppsName,(const char*)sqlite3_column_text( stmt, 0));
        gstrcat(pSpecialAppsName,pSep);
    }

    sqlite3_finalize( stmt );
    sqlite3_close(db);
    sqlite3_shutdown();
	ret = websWrite(wp, T("\"%s\""), pSpecialAppsName);
	free(pSpecialAppsName);
    return ret;
#endif
}

static int getSpecialAppsServiceName(int eid, webs_t wp, int argc, char_t **argv)
{
	{
#ifdef __USED_WEBS_DB__
	 int nRows=0, nCols=0;
	 char** ppQueryRes = NULL;
	 char * pQueryStr = "select * from Services";
	
	 int index = 0;
	 char* pServiceName = NULL;
	 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	 {
	   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	   return 0;
	 }
	 if (nRows > 0) 
	 {
		for (index = 0; index < nRows; index++)
		{
		  DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getAppsListOut);
		  //gstrcat(pSpecialAppsName,pServiceName);
		  //gstrcat(pSpecialAppsName,pSep);
		  websWrite(wp, T("<option value=%s>%s</option>"), pServiceName, pServiceName);
		  //printf("pSpecialAppsName:index: '%d', pServiceName\n", index, pServiceName);
		}
	 }
	 websWrite(wp, T("<option value=\"-1\"><script>document.write(gettext(\"Other\"));</script></option>"));

	getAppsListOut:
	 if (ppQueryRes)
	 {
	  sqlite3_free_table(ppQueryRes);
	 }
	 return 0;
#else
		
#endif
	}

}


static int getSpecialAppsList(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from Services";

 int index = 0;
 char* pServiceName = NULL;
 char* pProtocol = NULL;
 char* pDestPortStart = NULL;
 char* pDestPortEnd = NULL;
 char* pType = NULL;
 char* pIsDefault = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++)
 	{
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getAppsListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Protocol", pProtocol, getAppsListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DestinationPortStart", pDestPortStart, getAppsListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DestinationPortEnd", pDestPortEnd, getAppsListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "TypeOfService", pType, getAppsListOut);
	DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "IsDefault", pIsDefault, getAppsListOut);

    if (index%2)
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    }
    else
    {
      websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    }
    websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"applist.checkbox\" id=\"applist_ck_%d\"></td>\n"),index+1);
    websWrite(wp, T("<td class=\"tdEven\"><span id=\"applist_servicename_%d\">%s</span></td>\n"), index+1, pServiceName);
	if (!gstrcmp(pProtocol,"6"))
      websWrite(wp, T("<td class=\"tdEven\"><span id=\"applist_protocol_%d\">%s</span></td>\n"), index+1, "TCP");
    else if (!gstrcmp(pProtocol,"17"))
      websWrite(wp, T("<td class=\"tdEven\"><span id=\"applist_protocol_%d\">%s</span></td>\n"), index+1, "UDP");
    websWrite(wp, T("<td class=\"tdEven\"><span id=\"applist_portstart_%d\">%s</span></td>\n"), index+1, pDestPortStart);
    websWrite(wp, T("<td class=\"tdEven\"><span id=\"applist_portend_%d\">%s</span></td>\n"), index+1, pDestPortEnd);	
	websWrite(wp, T("</tr>"));
  }
 }

getAppsListOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    //int count = jnrp_sql_getRowNum("PacketFilter",pDBFile);
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    //char   sqlCmd [1024] = {0};
    char * sqlCmd = "select * from Services";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    //snprintf (sqlCmd, sizeof(sqlCmd),"select * from %s",pTblName);
    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    
     while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
        //char* data = (const char*)sqlite3_column_text( stmt, i );
        if(!(row_count%2))
            websWrite(wp, T("<tr bgcolor=\"#F6F6F6\">"));
        else
            websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));

        websWrite(wp, T("<td align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"applist_ck\" id=\"applist_ck_%d\" onclick=\"setApplistEditRowId('%d')\"/></td>\n"),row_count,row_count);
        websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"applist_servicename_%d\">%s</span></td>\n"),row_count, (const char*)sqlite3_column_text( stmt, 0));
        //websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"applist_protocol_%d\">%s</span></td>\n"), row_count, (const char*)sqlite3_column_text( stmt, 1));
        if (!gstrcmp((const char*)sqlite3_column_text( stmt, 1),"6"))
          websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"applist_protocol_%d\">%s</span></td>\n"), row_count, "TCP");
    	else if (!gstrcmp((const char*)sqlite3_column_text( stmt, 1),"17"))
          websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"applist_protocol_%d\">%s</span></td>\n"), row_count, "UDP");
        websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"applist_portstart_%d\">%s</span></td>\n"),row_count, (const char*)sqlite3_column_text( stmt, 2 ));
        websWrite(wp, T("<td align=\"center\" valign=\"middle\"><span id=\"applist_portend_%d\">%s</span></td>\n"),row_count, (const char*)sqlite3_column_text( stmt, 3));
		websWrite(wp, T("<input type=\"hidden\" id=\"applist_isdefault_%d\" value=\"%s\">\n"),row_count, (const char*)sqlite3_column_text( stmt, 5));
    }
    //printf("row_count:%d \n",row_count);
    sqlite3_finalize( stmt );

    sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
#endif
}

static void setSpecialAppsListDel(webs_t wp, char_t *path, char_t *query)
{
	//printf("setSpecialAppsListDel\n");
    char * pId = websGetVar(wp, T("applist_rowid_del"), T(""));
    char * pErrMsg = NULL;
    int status = -1;
    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pDelQry [1024] = {0};
        memset (pDelQry, 0x00, sizeof(pDelQry));
        snprintf(pDelQry,sizeof(pDelQry), "DELETE FROM Services WHERE _ROWID_='%s'",pId);
        status = sqlite3_exec (pDB, pDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); 
        }
    }

    websRedirect(wp, T("advanced/special_apps.asp"));
}

static void setSpecialApps(webs_t wp, char_t *path, char_t *query)
{
  //printf("setSpecialApps \n");
  char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
  char_t * pRowId = websGetVar(wp, T("applist_rowid_edit"), T(""));
  //printf("setSpecialApps: pRowId='%s'\n", pRowId);
  char * pErrMsg = NULL;
  int status = -1;
  
  char_t * pServiceName = websGetVar(wp, T("applist_service_name"), T(""));  
  char_t * pProtocol = websGetVar(wp, T("applist_protocol"), T("")); 
  char_t * pPortStart = websGetVar(wp, T("applist_port_start"), T(""));      
  char_t * pPortEnd = websGetVar(wp, T("applist_port_end"), T("")); 
  char_t * pDefault = websGetVar(wp, T("applist_isDefault"), T("")); 

  sqlite3 * pDB = NULL;
  if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
  {
      printf ("unable to open the DB file %s\n", pDBFile);
      return NULL;
  }
  sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
  
  char pSpecialAppsQry [1024] = {0};
  memset (pSpecialAppsQry, 0x00, sizeof(pSpecialAppsQry));

  if (!gstrcmp(pEditMode,"add"))
  {
      //printf("setSpecialApps add test\n");
      snprintf(pSpecialAppsQry,sizeof(pSpecialAppsQry), "insert into Services values ('%s','%d','%d','%d','%d','%d')"
      ,pServiceName, gatoi(pProtocol), gatoi(pPortStart), gatoi(pPortEnd), 0, gatoi(pDefault));
      //printf("pSpecialAppsQry: %s \n",pSpecialAppsQry);
      status = sqlite3_exec (pDB, pSpecialAppsQry, NULL, NULL, &pErrMsg);
  }
  else if (!gstrcmp(pEditMode,"edit"))
  {
      //printf("setSpecialApps edit test\n");
      snprintf(pSpecialAppsQry,sizeof(pSpecialAppsQry), "update Services set ServiceName='%s', Protocol='%d', DestinationPortStart='%d', DestinationPortEnd='%d', TypeOfService='%d', IsDefault='%d' where _ROWID_='%s'"
      ,pServiceName, gatoi(pProtocol), gatoi(pPortStart), gatoi(pPortEnd), 0, gatoi(pDefault),pRowId);
      //printf("pSpecialAppsQry: %s \n",pSpecialAppsQry);
      status = sqlite3_exec (pDB, pSpecialAppsQry, NULL, NULL, &pErrMsg);
  }
   
  if (pDB)
  {
      sqlite3_close (pDB);
  }

  // export db to ascii
  if (status>=0){
    system(EXPORT_CMD); 
  }
  sleep(1);
  websRedirect(wp, T("advanced/special_apps.asp"));
}

static void setAppListTable(webs_t wp, char_t *path, char_t *query)
{
    char * pAppListAction = websGetVar(wp, T("AppListAction"), T(""));	
	char_t * pAppListId = websGetVar(wp, T("AppListIdx"), T(""));
	char_t * pAppListCount = websGetVar(wp, T("AppListCount"), T("0"));
	int count=gatoi(pAppListCount);	
	int index=0;
	
	//printf("pAppListAction = %s, pAppListId=%s, pAppListCount=%s\n",pAppListAction,pAppListId,pAppListCount);	
	if (!gstrcmp(pAppListAction,"AppListTblAdd"))
	{
		special_apps_edit_idx=-1;
		websRedirect(wp, T("advanced/special_apps_editor.asp"));
	}
	else if (!gstrcmp(pAppListAction,"AppListTblEdit"))
	{
		special_apps_edit_idx=gatoi(pAppListId);
		websRedirect(wp, T("advanced/special_apps_editor.asp"));
	}
	else if (!gstrcmp(pAppListAction,"AppListTblDel"))
	{
		printf("pAppListId=%s\n",pAppListId);
		char * pErrMsg = NULL;
		int status = -1;	

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("advanced/special_apps_editor.asp"));
		}
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from Services where _ROWID_ in (%s)",pAppListId);		
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
		status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);	
		if (pDB)
			sqlite3_close (pDB);
	
		if (status>=0)
		{
			sqlite3_free (pErrMsg);
			system(EXPORT_CMD);
		}				
		websRedirect(wp, T("advanced/special_apps.asp"));
	}	
}

static int getSpecialAppsInfo(int eid, webs_t wp, int argc, char_t **argv)
{
 //printf("getSpecialAppsInfo, special_apps_edit_idx=%d\n",special_apps_edit_idx);
	if (special_apps_edit_idx==-1) //add
	{
		return websWrite(wp, T("\"%s\""),"");
	}
	else if (special_apps_edit_idx>0) //edit
	{
  char * pServiceName = jnrp_sql_getAttr_Webs("Services","ServiceName",special_apps_edit_idx);
  char * pProtocol = jnrp_sql_getAttr_Webs("Services","Protocol",special_apps_edit_idx);
  char * pStartPort = jnrp_sql_getAttr_Webs("Services","DestinationPortStart",special_apps_edit_idx);
  char * pEndPort = jnrp_sql_getAttr_Webs("Services","DestinationPortEnd",special_apps_edit_idx);
  char * pIsDefault = jnrp_sql_getAttr_Webs("Services","IsDefault",special_apps_edit_idx);

		char pServiceAppsItem [128] = {0};
		memset (pServiceAppsItem, 0x00, sizeof(pServiceAppsItem));
  gstrcat(pServiceAppsItem, pServiceName);
  gstrcat(pServiceAppsItem, pSep);
  gstrcat(pServiceAppsItem, pProtocol);
  gstrcat(pServiceAppsItem, pSep);
  gstrcat(pServiceAppsItem, pStartPort);
  gstrcat(pServiceAppsItem, pSep);
  gstrcat(pServiceAppsItem, pEndPort);
  gstrcat(pServiceAppsItem, pSep);
  gstrcat(pServiceAppsItem, pIsDefault);
  gstrcat(pServiceAppsItem, pSep);
  return websWrite(wp, T("\"%s\""),pServiceAppsItem);
 }
}

static int getSpecialAppsIsDefault(int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getSpecialAppsIsDefault\n");
  int nRows=0, nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "select * from Services";

  int ret = 0;
  int index = 0;
  char* pIsDefault = NULL;
  char* pSpecialAppsDefault = (char *)malloc(1024);
  memset(pSpecialAppsDefault,'\0', 1024);

  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
  {
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	free(pSpecialAppsDefault);
    return 0;
  }

  if (nRows > 0) {
  	for (index = 0; index < nRows; index++)
  	{
       DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "IsDefault", pIsDefault, getAppsListOut);
 	   gstrcat(pSpecialAppsDefault,pIsDefault);
       gstrcat(pSpecialAppsDefault,pSep);
     }
  }

 getAppsListOut:
  if (ppQueryRes)
  {
   sqlite3_free_table(ppQueryRes);
  }
  ret = websWrite(wp, T("\"%s\""),pSpecialAppsDefault);
  free(pSpecialAppsDefault);
  return ret;
}

int checkSpecialAppsConfig(char * ServiceName)
{
	char ** ppQryResult = NULL;
 char pCheckQry [256] = {0};
 memset(pCheckQry, 0x00, sizeof(pCheckQry));

	if (special_apps_edit_idx==-1) //add
	{
   snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from Services where ServiceName='%s'", ServiceName);
   ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
   if (gatoi(ppQryResult[1]) > 0) //Same Name
   {
 	   return spec_apps_err_same_name;
   }
   ppQryResult = NULL;
 }

 snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from FirewallRule where ServiceName='%s'", ServiceName);
 ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
 if (gatoi(ppQryResult[1]) > 0) //Access Control
 {
 	return spec_apps_err_access_control;
 }
 ppQryResult = NULL;

 snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from QosRule where Name='%s'", ServiceName);
 ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
 if (gatoi(ppQryResult[1]) > 0) //QoS Settings
 {
 	return spec_apps_err_qos_settings;
 }
 ppQryResult = NULL;

 snprintf(pCheckQry, sizeof(pCheckQry), "select COUNT(*) from PortForwarding where ServiceName='%s'", ServiceName);
 ppQryResult = jnrp_sql_getByQry_Webs (pCheckQry);
 if (gatoi(ppQryResult[1]) > 0) //Port Forwarding
 {
 	return spec_apps_err_port_forwarding;
 }

 return spec_apps_none;
}

static void setSpecialAppsConfig(webs_t wp, char_t *path, char_t *query)
{
	//printf("%s\n", __func__);
 char * pServiceName = websGetVar(wp, T("applist_service_name"), T(""));
 char * pProtocol = websGetVar(wp, T("applist_protocol"), T(""));
 char * pPortStart = websGetVar(wp, T("applist_port_start"), T(""));
 char * pPortEnd = websGetVar(wp, T("applist_port_end"), T(""));
 char * pDefault = websGetVar(wp, T("applist_isDefault"), T(""));
	char * pErrMsg = NULL;
	int status = -1;

	// check Access Control, QoS Settings, Port Forwarding if use Service Name
 P02003_special_applications_status = checkSpecialAppsConfig(pServiceName);
 if (P02003_special_applications_status == spec_apps_none)
 {
 } else {
 	 websRedirect(wp, T("advanced/special_apps.asp"));
 	 return;
 }

	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	if (special_apps_edit_idx==-1) //add
	{
  snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into Services values ('%s','%d','%d','%d','%d','%d')"
    ,pServiceName, gatoi(pProtocol), gatoi(pPortStart), gatoi(pPortEnd), 0, 0);
		status = jnrp_sql_dbEXE_Webs (pSQLCmd);
	}
	else if (special_apps_edit_idx>0) //edit
	{
  snprintf(pSQLCmd,sizeof(pSQLCmd), "update Services set ServiceName='%s', Protocol='%d', DestinationPortStart='%d', DestinationPortEnd='%d', TypeOfService='%d', IsDefault='%d' where _ROWID_=%d"
    ,pServiceName, gatoi(pProtocol), gatoi(pPortStart), gatoi(pPortEnd), 0, gatoi(pDefault), special_apps_edit_idx);
		status = jnrp_sql_dbEXE_Webs (pSQLCmd);
	}
 //printf("pSQLCmd: %s \n",pSQLCmd);

 // export db to ascii
 if (status>=0){
   system(EXPORT_CMD);
 }
 sleep(1);
 websRedirect(wp, T("advanced/special_apps.asp"));
}

static int getSpecialAppsErrorStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int error_msg = P02003_special_applications_status;
	P02003_special_applications_status = spec_apps_none;
 return websWrite(wp, T("\"%d\""), error_msg);
}
#endif

#if defined(__P02001_Dora) || defined(__P02002_Dora)|| defined(__P02003_Dora)
//**QoS--Start**//
static int getQoSPolicy(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from QoS";

 int index = 0;
 char* pQoSOption = NULL;
 char* pThroughPutUL = NULL;
 char* pThroughPutDL = NULL;
 char* pHighestUL = NULL;
 char* pHighestDL = NULL;
 char* pHighUL = NULL;
 char* pHighDL = NULL;
 char* pNormalUL = NULL;
 char* pNormalDL = NULL;
 char* pLowUL = NULL;
 char* pLowDL = NULL;
 char* pDefault = NULL;

 int ret = 0;
 char* pQosPolicy = (char *)malloc(512);
 memset(pQosPolicy,'\0', 512);
 
 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   free(pQosPolicy);
   return 0;
 }
 if (nRows > 0) {
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Enable", pQoSOption, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "DL_Total", pThroughPutDL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "DL_Highest", pHighestDL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "DL_High", pHighDL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "DL_Nornmal", pNormalDL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "DL_Low", pLowDL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UL_Total", pThroughPutUL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UL_Highest", pHighestUL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UL_High", pHighUL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UL_Nornmal", pNormalUL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "UL_Low", pLowUL, getQoSPolicyOut);
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "Default_Priority", pDefault, getQoSPolicyOut);	  

	  gstrcat(pQosPolicy,pQoSOption);    //0
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pThroughPutDL); //1
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pHighestDL);    //2
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pHighDL);       //3
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pNormalDL);     //4
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pLowDL);        //5
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pThroughPutUL); //6
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pHighestUL);    //7
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pHighUL);       //8
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pNormalUL);     //9
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pLowUL);        //10
      gstrcat(pQosPolicy,pSep);
	  gstrcat(pQosPolicy,pDefault);      //11
      gstrcat(pQosPolicy,pSep);
 }
 
getQoSPolicyOut:
 ret = websWrite(wp, T("\"%s\""), pQosPolicy);
 free(pQosPolicy);
 return ret;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    char * sqlCmd = "select * from QoS";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

	int ret = 0;
	char* pQosPolicy = (char *)malloc(1024);
 	memset(pQosPolicy,'\0', 1024);
    while( sqlite3_step( stmt ) == SQLITE_ROW ){
		gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 0));		//0
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 1)); 	//1
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 2));   	//2
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 3));   	//3
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 4));   	//4
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 5));     //5
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 6)); 	//6
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 7));    	//7
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 8));     //8
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 9));     //9
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 10));    //10
        gstrcat(pQosPolicy,pSep);
	    gstrcat(pQosPolicy,(const char*)sqlite3_column_text( stmt, 11));    //11
        gstrcat(pQosPolicy,pSep);		
    }

    sqlite3_finalize( stmt );
    sqlite3_close(db);
    sqlite3_shutdown();
	ret = websWrite(wp, T("\"%s\""), pQosPolicy);
	free(pQosPolicy);
    return ret;
#endif
}
static void QosPolicy(webs_t wp, char_t *path, char_t *query)
{
  char * pErrMsg = NULL;
  int status = -1;
  
  char_t * pQoSoption = websGetVar(wp, T("AP_QOS"), T(""));  
  char_t * pThroughputUL = websGetVar(wp, T("QOS_THROUGHPUT_UL"), T("")); 
  char_t * pThroughputDL = websGetVar(wp, T("QOS_THROUGHPUT_DL"), T(""));      
  char_t * pHightestUL = websGetVar(wp, T("QOS_HIGHEST_UL"), T("")); 
  char_t * pHightestDL = websGetVar(wp, T("QOS_HIGHEST_DL"), T("")); 
  char_t * pHighUL = websGetVar(wp, T("QOS_HIGH_UL"), T("")); 
  char_t * pHighDL = websGetVar(wp, T("QOS_HIGH_DL"), T("")); 
  char_t * pNormalUL = websGetVar(wp, T("QOS_NORMAL_UL"), T("")); 
  char_t * pNormalDL = websGetVar(wp, T("QOS_NORMAL_DL"), T("")); 
  char_t * pLowUL = websGetVar(wp, T("QOS_LOW_UL"), T("")); 
  char_t * pLowDL = websGetVar(wp, T("QOS_LOW_DL"), T("")); 
  char_t * pDefault = websGetVar(wp, T("QOS_DEFAULT"), T(""));

  sqlite3 * pDB = NULL;
  if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
  {
      printf ("unable to open the DB file %s\n", pDBFile);
      return NULL;
  }
  sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
  
  char pQoSPolicyQry [1024] = {0};
  memset (pQoSPolicyQry, 0x00, sizeof(pQoSPolicyQry));

  snprintf (pQoSPolicyQry, sizeof(pQoSPolicyQry),"update QoS set Enable='%d', DL_Total='%d', DL_Highest='%d', DL_High='%d'"
  ", DL_Nornmal='%d', DL_Low='%d', UL_Total='%d', UL_Highest='%d', UL_High='%d', UL_Nornmal='%d', UL_Low='%d', Default_Priority='%d'"
  ,gatoi(pQoSoption), gatoi(pThroughputDL), gatoi(pHightestDL), gatoi(pHighDL), gatoi(pNormalDL), gatoi(pLowDL),
  gatoi(pThroughputUL), gatoi(pHightestUL), gatoi(pHighUL), gatoi(pNormalUL), gatoi(pLowUL), gatoi(pDefault));

  status = sqlite3_exec (pDB, pQoSPolicyQry, NULL, NULL, &pErrMsg);

  system("QosPolicy.sh");
   
  if (pDB)
  {
      sqlite3_close (pDB);
  }

  // export db to ascii
  if (status>=0){
    system(EXPORT_CMD); 
  }
  websRedirect(wp, T("advanced/QoSPolicy.asp"));
}

static void setQosRule(webs_t wp, char_t *path, char_t *query)
{
  char * pErrMsg = NULL;
  int status = -1;

  char_t * pIndex = websGetVar(wp, T("QOS_RULE_INDEX"), T(""));   
  char_t * pName = websGetVar(wp, T("QOS_NAME"), T(""));  
  char_t * pPriority = websGetVar(wp, T("QOS_PRIORITY"), T("")); 
  char_t * pFlow = websGetVar(wp, T("QOS_FLOW"), T(""));      
  char_t * pUpRule = websGetVar(wp, T("QOS_UP_RULE"), T("")); 
  char_t * pUpRule_ip = websGetVar(wp, T("UP_RULE_IP"), T("")); 
  char_t * pUpRule_mac = websGetVar(wp, T("UP_RULE_MAC"), T("")); 
  char_t * pUpRule_dscp = websGetVar(wp, T("UP_RULE_DSCP"), T(""));
  char_t * pDownRule = websGetVar(wp, T("QOS_DOWN_RULE"), T("")); 
  char_t * pDownRule_ip = websGetVar(wp, T("DOWN_RULE_IP"), T("")); 
  char_t * pDownRule_dscp = websGetVar(wp, T("DOWN_RULE_DSCP"), T("")); 

  char_t * pRule = (char *)malloc(128);
  char_t * pContent = (char *)malloc(128);

  char script[1024];

  sqlite3 * pDB = NULL;
  if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
  {
      printf ("unable to open the DB file %s\n", pDBFile);
      free(pRule);
      free(pContent);
      return NULL;
  }
  sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
  
  char pQoSRule [1024] = {0};
  memset (pQoSRule, 0x00, sizeof(pQoSRule));

  if (!gstrcmp(pFlow,"uplink")){
	if (!gstrcmp(pUpRule,"ip")){
  	  strcpy(pRule,"ip");
	  strcpy(pContent,pUpRule_ip);
  	} else if (!gstrcmp(pUpRule,"mac")){
  	  strcpy(pRule,"mac");	
	  strcpy(pContent,pUpRule_mac);
  	} else if (!gstrcmp(pUpRule,"dscp")){
  	  strcpy(pRule,"dscp");	
	  strcpy(pContent,pUpRule_dscp);
  	}
  }	else if (!gstrcmp(pFlow,"dllink")){
    if (!gstrcmp(pDownRule,"ip")){
  	  strcpy(pRule,"ip");
	  strcpy(pContent,pDownRule_ip);
  	} else if (!gstrcmp(pDownRule,"dscp")){
  	  strcpy(pRule,"dscp");	
	  strcpy(pContent,pDownRule_dscp);
  	}
  }

  snprintf (pQoSRule, sizeof(pQoSRule),"update QosRule set Enable='1', Name='%s', Priority='%d', Rule='%s', Flow='%s', Content='%s' where _ROWID_='%s'"
  , pName, gatoi(pPriority), pRule, pFlow, pContent, pIndex);

  status = sqlite3_exec (pDB, pQoSRule, NULL, NULL, &pErrMsg);

  if (pDB)
  {
      sqlite3_close (pDB);
  }

  // export db to ascii
  if (status>=0){
    system(EXPORT_CMD); 
    //system("QosRule.sh &");   
    snprintf(script, 1024, "QosRule.sh %s &", pIndex);
	system(script);	
  }

  free(pRule);
  free(pContent);  
  websRedirect(wp, T("advanced/QoSPolicy.asp"));
}

static int getQosList(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from QosRule";

 int index = 0;
 char* pEnable = NULL;
 char* pName = NULL;
 char* pPriority = NULL;
 char* pRule = NULL;
 char* pFlow = NULL;
 char* pContent = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   return 0;
 }

 for (index = 0; index < 32; index++) {
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getQosListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Name", pName, getQosListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Priority", pPriority, getQosListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Rule", pRule, getQosListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Flow", pFlow, getQosListOut);
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Content", pContent, getQosListOut);

    if (!gstrcmp(pEnable,"1")){	 
	  //printf("Enter Enable=1, index=%d\n", index);
	  websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));
      websWrite(wp, T("<td align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"qos_ck\" id=\"qos_%d\" onclick=\"setQosDelRowId('%d')\"/></td>\n"), index+1, index+1);
	  websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_id_%d\">%d</td>\n"), index+1, index+1);
	  websWrite(wp, T("<td align=\"center\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" valign=\"middle\" id=\"qos_name_%d\">%s</td>\n"), index+1, pName);

	  if (!gstrcmp(pPriority,"1")){
	    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), index+1, "Highest");
	  }else if (!gstrcmp(pPriority,"2")){
	  	websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), index+1, "High");
	  }else if (!gstrcmp(pPriority,"3")){
	   	websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), index+1, "Normal");
	  }else if (!gstrcmp(pPriority,"4")){
	   	websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), index+1, "Low");
	  }
	  
	  if (!gstrcmp(pRule,"ip")){
	    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_rule_%d\">%s</td>\n"), index+1, "IP");
	  }else if (!gstrcmp(pRule,"mac")){
	  	websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_rule_%d\">%s</td>\n"), index+1, "MAC");
	  }else if (!gstrcmp(pRule,"dscp")){
	   	websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_rule_%d\">%s</td>\n"), index+1, "DSCP");
	  }

	  if (!gstrcmp(pFlow,"uplink")){
	    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_flow_%d\">%s</td>\n"), index+1, "Uplink");
	  }else if (!gstrcmp(pFlow,"dllink")){
	  	websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_flow_%d\">%s</td>\n"), index+1, "Downlink");
	  }

 	  websWrite(wp, T("<td align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"qos_desc_%d\">%s</td>\n"), index+1, pContent);
    }  
 }

getQosListOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
    sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    char * sqlCmd = "select * from QosRule";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    sqlite3_busy_timeout(db, 1500);
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK)
    {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    
     while( sqlite3_step( stmt ) == SQLITE_ROW )
    {
        row_count++;
        websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));
	 	websWrite(wp, T("<td align=\"center\" valign=\"middle\"><input type=\"radio\" name=\"qos_ck\" id=\"qos_%d\" onclick=\"setQosDelRowId('%d')\"/></td>\n"), row_count, row_count);
 	    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_id_%d\">%s</td>\n"), row_count, row_count);
  	    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_name_%d\">%s</td>\n"), row_count, (const char*)sqlite3_column_text( stmt, 2));
	    if (!gstrcmp((const char*)sqlite3_column_text( stmt, 3),"1")){	
	      websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), row_count, "Highest");
	    }else if (!gstrcmp(pPriority,"2")){	
	      websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), row_count, "High");
	    }else if (!gstrcmp(pPriority,"3")){	
	      websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), row_count, "Normal");
	    }else if (!gstrcmp(pPriority,"4")){	
	      websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_priority_%d\">%s</td>\n"), row_count, "Low");
	    }

	    if (!gstrcmp((const char*)sqlite3_column_text( stmt, 4),"ip")){
	      websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_rule_%d\">%s</td>\n"), row_count, "IP");
	    }else if (!gstrcmp((const char*)sqlite3_column_text( stmt, 4),"mac")){
	      websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_rule_%d\">%s</td>\n"), row_count, "MAC");
	    }else if (!gstrcmp((const char*)sqlite3_column_text( stmt, 4),"dscp")){
	      websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_rule_%d\">%s</td>\n"), row_count, "DSCP");
	    }

	  if (!gstrcmp((const char*)sqlite3_column_text( stmt, 5),"uplink")){
	    websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_flow_%d\">%s</td>\n"), row_count, "Uplink");
	  }else if (!gstrcmp((const char*)sqlite3_column_text( stmt, 5),"dllink")){
	  	websWrite(wp, T("<td align=\"center\" valign=\"middle\" id=\"qos_flow_%d\">%s</td>\n"), row_count, "Downlink");
	  }

 	  websWrite(wp, T("<td align=\"center\" valign=\"middle\" style=\"overflow: hidden; text-overflow: ellipsis; white-space: nowrap;\" id=\"qos_desc_%d\">%s</td>\n"), row_count, (const char*)sqlite3_column_text( stmt, 6));
    }

    sqlite3_finalize( stmt );
    sqlite3_close(db);
    sqlite3_shutdown();
    return 0;
#endif
}

static void setQosListDel(webs_t wp, char_t *path, char_t *query)
{
  printf("setQosListDel\n");
  char * pId = websGetVar(wp, T("qos_rowid_del"), T(""));
  //printf("pId: %s\n",pId);
  char * pErrMsg = NULL;
  int status = -1;
  char script[1024];
  
  if (gatoi(pId) > 0)
  {
      sqlite3 * pDB = NULL;
      if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
      {
          printf ("unable to open the DB file %s\n", pDBFile);
          return NULL;
      }
      sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
  
      char pQosDelQry [1024] = {0};
      memset (pQosDelQry, 0x00, sizeof(pQosDelQry));
      snprintf(pQosDelQry,sizeof(pQosDelQry), "update QosRule set Enable='0' WHERE _ROWID_='%s'",pId);
      status = sqlite3_exec (pDB, pQosDelQry, NULL, NULL, &pErrMsg);
 
      if (pDB)
          sqlite3_close (pDB);
  
      // export db to ascii
      if (status>=0)
      {
          // export db to ascii
          sqlite3_free (pErrMsg);
          system(EXPORT_CMD); // use export lib in future
          snprintf(script, 1024, "QosListDelete.sh %s &", pId);
		  system(script);	
      }
  }
  websRedirect(wp, T("advanced/QoSPolicy.asp"));
}
//***QoS--End**//

//***Upnp--Start**//
static int getUpnpPortEnable(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
    char * pQryResult = jnrp_sql_getAttr_Webs ("UPNP", "Enable", 1);
#else
    char * pQryResult = jnrp_sql_getAttr (pDBFile,"UPNP", "Enable", 1);
#endif
	
    return websWrite(wp, T("%s"),pQryResult);
}
static void setUpnpEnable(webs_t wp, char_t *path, char_t *query)
{
	//printf("setUpnpEnable: \n");
    char_t * pEnable = websGetVar(wp, T("enable_upnp"), T("off"));
	int status = -1;
#ifdef __USED_WEBS_DB__
    if (!gstrcmp(pEnable,"on")) {
        status = jnrp_sql_setAttr_Webs ("UPNP", "Enable", "1", 1);
    } else {
        status = jnrp_sql_setAttr_Webs ("UPNP", "Enable", "0", 1);
    }
#else
    if (!gstrcmp(pEnable,"on")) {
        status = jnrp_sql_setAttr (pDBFile, "UPNP", "Enable", "1", 1);
    } else {
        status = jnrp_sql_setAttr (pDBFile, "UPNP", "Enable", "0", 1);
    }
#endif

    if (status>=0)
    {
        // export db to ascii
        system(EXPORT_CMD);
		system("upnp_service.sh");
		system("sleep 2");
		system("UPnPortMapping.sh refresh");
    }

    websRedirect(wp, T("advanced/upnp_port.asp"));
}
static int getUpnpPortList(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef __USED_WEBS_DB__
 int nRows=0, nCols=0;
 char** ppQueryRes = NULL;
 char * pQueryStr = "select * from UPNPortMap";

 int index = 0;
 char* pDestIP = NULL;
 char* pProtocol = NULL;
 char* pInPort = NULL;
 char* pExtPort = NULL;

 if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
 {
   printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
   //sqlite3_free(pQueryStr);
   return 0;
 }

 if (nRows > 0) {
 	for (index = 0; index < nRows; index++) {
    	DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DestIP", pDestIP, getUpnpPortOut);
    	DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Protocol", pProtocol, getUpnpPortOut);
    	DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "IntPort", pInPort, getUpnpPortOut);
		DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ExtPort", pExtPort, getUpnpPortOut);
    	if (index%2) {
      		websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#F6F6F6
    	} else {
      		websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));  //#EEEEEE
    	}    
		
#if defined(__P02003_Dora) 
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), "active");
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pProtocol);
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pInPort);
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pExtPort);
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pDestIP);		
#else
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pDestIP);
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pProtocol);
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pInPort);
		websWrite(wp, T("<td align=\"center\" valign=\"middle\" class=\"tdEven\">%s</td>\n"), pExtPort);
#endif
  	}
 }

getUpnpPortOut:
 if (ppQueryRes)
 {
  sqlite3_free_table(ppQueryRes);
 }

 return 0;
#else
	sqlite3         *db = NULL;
    sqlite3_stmt    *stmt = NULL;
    int             rc = 0;
    char* sqlCmd = "select * from UPNPortMap";
    const char      *data = NULL;

    sqlite3_initialize( );
    rc = sqlite3_open_v2( pDBFile, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
    if ( rc != SQLITE_OK) {
        sqlite3_close( db );
        printf("open sqlite not ok \n");
        return 0;
    }

    sqlite3_busy_timeout(db, 1500);
    // sqlite3 query
    rc = sqlite3_prepare_v2( db, sqlCmd, -1, &stmt, NULL );
    if ( rc != SQLITE_OK) {
        printf("prepare sqlite not ok \n");
        return 0;
    }

    int row_count = 0,i;
    // get column numbers
    while( sqlite3_step( stmt ) == SQLITE_ROW ) {
        row_count++;
        websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));
        websWrite(wp, T("<td class=\"tdEven\">%s</td>\n"), (const char*)sqlite3_column_text( stmt, 0));
        websWrite(wp, T("<td class=\"tdEven\">%s</td>\n"), (const char*)sqlite3_column_text( stmt, 1));       
        websWrite(wp, T("<td class=\"tdEven\">%s</td>\n"), (const char*)sqlite3_column_text( stmt, 2 ));
		websWrite(wp, T("<td class=\"tdEven\">%s</td>\n"), (const char*)sqlite3_column_text( stmt, 3 ));
    }
    sqlite3_finalize( stmt );
    sqlite3_close(db);
    sqlite3_shutdown();
    
    return 0;
#endif
}

static void setUpnpListRefresh(webs_t wp, char_t *path, char_t *query)
{
	//printf("setUpnpListRefresh: \n");
	system("UPnPortMapping.sh refresh");
	system("sleep 2");
    websRedirect(wp, T("advanced/upnp_port.asp"));
}

static void setUpnpListClear(webs_t wp, char_t *path, char_t *query)
{
	//printf("setUpnpListClear: \n");
	system("UPnPortMapping.sh clear");
	system("sleep 2");
    websRedirect(wp, T("advanced/upnp_port.asp"));
}
//***Upnp--End**//
#endif

static void setStaRout(webs_t wp, char_t *path, char_t *query)
{
    //printf("setStaRout\n");
    char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char_t * pRowId = websGetVar(wp, T("starout_rowid_edit"), T(""));
    //printf("editflag:%s ,pRowId: %s \n",pEditMode,pRowId);

    char * pErrMsg = NULL;
    int status = -1;

    char_t * pEnable = websGetVar(wp, T("starout_enable"), T("off"));
    char_t * pName = websGetVar(wp, T("starout_name"), T(""));
    char_t * pDestNet = websGetVar(wp, T("starout_dest_net"), T(""));
    char_t * pDestMask = websGetVar(wp, T("starout_dest_msk"), T(""));
    char_t * pLoacl = websGetVar(wp, T("starout_lcl"), T(""));
    char_t * pLoaclGw = websGetVar(wp, T("starout_lcl_gw"), T(""));
    //printf("pEnable:%s , pName:%s, pDestNet:%s, pDestMask:%s, pLoacl:%s, pLoaclGw:%s \n",pEnable,pName,pDestNet,pDestMask,pLoacl,pLoaclGw);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        //printf("setStaRout add test\n");
        snprintf(pPortQry,sizeof(pPortQry), "insert into StaticRoute values ('%s','%s','%s','%s','%s','%s')"
        ,gstrcmp(pEnable,"off")?"1":"0",pName,pDestNet,pDestMask,pLoacl,pLoaclGw);
        //printf("setStaRout add: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }
    else if (!gstrcmp(pEditMode,"edit"))
    {
        //printf("setPortFwd edit test\n");
        snprintf(pPortQry,sizeof(pPortQry), "update StaticRoute set Enable='%d', Name='%s', DestineNetwork='%s'"
        ", DestineNetmask='%s', Local='%s', LocalGateway='%s' where _ROWID_='%s'",gstrcmp(pEnable,"off")?1:0,pName,pDestNet,pDestMask,pLoacl,pLoaclGw,pRowId);
        //printf("setPortFwd edit: %s \n",pPortQry);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

    if (pDB)
    {
        sqlite3_close (pDB);
    }
    // export db to ascii
    if (status>=0)
    {
        // export db to ascii
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }
	
    websRedirect(wp, T("advanced/advanced_staticroute.asp"));
}

static void setStaRoutDel(webs_t wp, char_t *path, char_t *query)
{
    //printf("setStaRoutDel \n");
    char_t * pId = websGetVar(wp, T("starout_rowid_del"), T(""));
    //printf("pId: %s\n",pId);

    char * pErrMsg = NULL;
    int status = -1;

    if (gatoi(pId) > 0)
    {
        sqlite3 * pDB = NULL;
        if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
        {
            printf ("unable to open the DB file %s\n", pDBFile);
            //ADP_WNC_DBG ("unable to open the DB file %s\n", DbFile);
            return NULL;
        }
        sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

        char pPortDelQry [1024] = {0};
        memset (pPortDelQry, 0x00, sizeof(pPortDelQry));
        snprintf(pPortDelQry,sizeof(pPortDelQry), "DELETE FROM StaticRoute WHERE _ROWID_='%s'",pId);
        //printf("pPortDelQry: %s \n",pPortDelQry);
        status = sqlite3_exec (pDB, pPortDelQry, NULL, NULL, &pErrMsg);
        status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);

        if (pDB)
            sqlite3_close (pDB);

        // export db to ascii
        if (status>=0)
        {
            // export db to ascii
            sqlite3_free (pErrMsg);
            system(EXPORT_CMD); // use export lib in future
            system("/usr/local/bin/routing.sh");
        }
    }

    websRedirect(wp, T("advanced/advanced_staticroute.asp"));
}

// system reboot
static int setRebootform(webs_t wp, char_t *path, char_t *query)
{

#if defined(__P02003_Dora)

	//Poland WebUI skin
	websRedirect(wp, T("system/statusMessage.asp"));

#else
	websWrite(wp, T("\n\
<html>\n\
<head>\n\
<TITLE>System Reboot</TITLE>\n\
<link rel=stylesheet href=/style/normal_ws.css type=text/css>\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=iso-8859-1\">\n\
<script language=\"javascript\">\n\
var count=120;\n\
var counter=setInterval(displayRebootMsg, 1000); \n\
function displayRebootMsg()\n\
{	var chart = document.getElementById(\"countdown\");\n\
	chart.innerHTML = 'Please wait &nbsp;&nbsp;' + count + '&nbsp;&nbsp;seconds for the system reboot.';\n\
    count=count-1;\n\
    if (count <= 0)\n\
    {clearInterval(counter);window.location.href=\"../login.asp\";\n\  return;}\n\
}\n\
</script>\n\
</head>\n\
<body><h1> System Reboot </h1>\n\
<div id=\"countdown\" class=\"msg_container\" style=\"line-height:62px; text-align:center;\"> Please wait &nbsp;&nbsp;120&nbsp;&nbsp;seconds for the system reboot.</div>\n\
</body>\n\
</html>\n"));

#endif
printf("EXEC: reboot\n");
system("reboot");

}	

static int getIPv6DynamicConf(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	FILE *fd = -1;
	char cmdbuffer[100] = {0};		
	char buffer[80] = {0};
	char_t *IPV6_TYPE = (char *)malloc(128);
	char_t *OUT_IF_SEL = (char *)malloc(128);
	char_t *AP_IPADDR_V6 = (char *)malloc(128);
	char_t *PREFIX_LEN_V6 = (char *)malloc(128);
	int prefix_len=64;
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));	
	memset(buffer, '\0', sizeof(buffer));
	gstrcpy(IPV6_TYPE, "");
	gstrcpy(OUT_IF_SEL, "");	
	gstrcpy(AP_IPADDR_V6, "");
	gstrcpy(PREFIX_LEN_V6, "");	

	/***************************************************
	 * Get IPV6_TYPE & OUT_IF_SEL
	 ****************************************************/
	FILE *cfg = fopen("/sysconfig/ipv6-cfg", "r");
	if(cfg)	
	{
	  while(fgets(buffer, sizeof(buffer), cfg) != NULL)
	  {
		if (!gstrcmp(getSplitStr(buffer,"=",1),"IPV6_TYPE"))
		{					
			gstrcat(IPV6_TYPE, getSplitStr(buffer,"=",2));
			//printf("IPV6_TYPE=%s\n",IPV6_TYPE);
			if (IPV6_TYPE[strlen(IPV6_TYPE)-1] == '\n')
			  IPV6_TYPE[strlen(IPV6_TYPE)-1] = '\0';			
		}

		if (!gstrcmp(getSplitStr(buffer,"=",1),"OUT_IF_SEL"))
		{					
			gstrcat(OUT_IF_SEL, getSplitStr(buffer,"=",2));
			//printf("OUT_IF_SEL=%s\n",OUT_IF_SEL);
			if (OUT_IF_SEL[strlen(OUT_IF_SEL)-1] == '\n')
			  OUT_IF_SEL[strlen(OUT_IF_SEL)-1] = '\0';						
		}		

		if (!gstrcmp(getSplitStr(buffer,"=",1),"AP_IPADDR_V6"))
		{					
			gstrcat(AP_IPADDR_V6, getSplitStr(buffer,"=",2));
			if (AP_IPADDR_V6[strlen(AP_IPADDR_V6)-1] == '\n')
			  AP_IPADDR_V6[strlen(AP_IPADDR_V6)-1] = '\0';
			
			//printf("AP_IPADDR_V6=%s\n",AP_IPADDR_V6);
		}

		if (!gstrcmp(getSplitStr(buffer,"=",1),"PREFIX_LEN_V6"))
		{					
			gstrcat(PREFIX_LEN_V6, getSplitStr(buffer,"=",2));
			prefix_len=gatoi(PREFIX_LEN_V6);
			//printf("prefix_len=%d\n",prefix_len);
			if (PREFIX_LEN_V6[strlen(PREFIX_LEN_V6)-1] == '\n')
			  PREFIX_LEN_V6[strlen(PREFIX_LEN_V6)-1] = '\0';			
		}			
	  }
	  fclose(cfg);
	} else {	 
	}

	
	/***************************************************
	 * Get IPV6 dynamic config value
	 ****************************************************/
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));	
	memset(buffer, '\0', sizeof(buffer));	
    if (!gstrcmp(argv[0],"ipv6wanip"))
    {    			
		if (!gstrcmp(OUT_IF_SEL, "usb0"))
		{
			if (!gstrcmp(IPV6_TYPE, "static_dual") ||
				!gstrcmp(IPV6_TYPE, "dyna_dual"))
			{
				sprintf(cmdbuffer,"ifconfig usb0 | grep 'Scope:Global' | awk '{print $3}'");						
			}
			else if (!gstrcmp(IPV6_TYPE, "6in4"))
			{
				sprintf(cmdbuffer,"ifconfig sit0 | grep 'Scope:Global' | awk '{print $3}'");						
			}
			else if (!gstrcmp(IPV6_TYPE, "6to4"))
			{
				sprintf(cmdbuffer,"ifconfig tun6to4 | grep 'Scope:Global' | awk '{print $3}'");										
			}
			else if (!gstrcmp(IPV6_TYPE, "6rd"))
			{
				sprintf(cmdbuffer,"ifconfig tun6rd | grep 'Scope:Global' | awk '{print $3}'");														
			}
		}
		else
		{
			if(!gstrcmp(IPV6_TYPE,"static_dual") == 0 ||
				!gstrcmp(IPV6_TYPE,"dyna_dual") == 0)
			{
				sprintf(cmdbuffer,"ifconfig eth0 | grep 'Scope:Global' | awk '{print $3}'");						
			}
			else if(!gstrcmp(IPV6_TYPE,"pppoe") == 0)
			{
				sprintf(cmdbuffer,"ifconfig ppp0 | grep 'Scope:Global' | awk '{print $3}'");						
			}
			else if(!gstrcmp(IPV6_TYPE,"6in4") == 0)
			{
				sprintf(cmdbuffer,"ifconfig sit0 | grep 'Scope:Global' | awk '{print $3}'");						
			}
			else if(!gstrcmp(IPV6_TYPE,"6to4") == 0)
			{
				sprintf(cmdbuffer,"ifconfig tun6to4 | grep 'Scope:Global' | awk '{print $3}'");										
			}
			else if(!gstrcmp(IPV6_TYPE,"6rd") == 0)
			{
				sprintf(cmdbuffer,"ifconfig tun6rd | grep 'Scope:Global' | awk '{print $3}'");																		
			}
			
		}
    }
	else if (!gstrcmp(argv[0],"lanipv6global"))
	{
		sprintf(cmdbuffer,"ifconfig br0 | grep 'Scope:Global' | grep 'inet6 addr: 2' | awk '{print $3}'");
	}
	else if (!gstrcmp(argv[0],"lanipv6link"))
	{
		sprintf(cmdbuffer,"ifconfig br0 | grep Scope:Link | awk '{print $3}'");
	}
	else if (!gstrcmp(argv[0],"lanv6prefix"))
	{		
		/****************************************************************
		 * Get lanipv6global start
		 ****************************************************************/		
		FILE *fd_global_ip = -1;
		char global_ip[100] = {0};
		memset(global_ip, '\0', sizeof(global_ip));	
		sprintf(cmdbuffer,"ifconfig br0 | grep 'Scope:Global' | grep 'inet6 addr: 2' | awk '{print $3}'");
		fd_global_ip = popen(cmdbuffer,"r");	
		memset(cmdbuffer, '\0', sizeof(cmdbuffer)); 
		if(fgets(global_ip,sizeof(global_ip),fd_global_ip) == NULL)
		{			
			sprintf(cmdbuffer,"ipv6_prefix %s %d", AP_IPADDR_V6, prefix_len);
		}
		else
		{
			if (global_ip[strlen(global_ip)-1] == '\n')
			 global_ip[strlen(global_ip)-1] = '\0';			

			sprintf(cmdbuffer,"ipv6_prefix %s %d", global_ip, prefix_len);			
		}
		pclose(fd_global_ip); 
		printf("global_ip = %s\n", global_ip);
		/****************************************************************
		 * Get lanipv6global end
		 ****************************************************************/		
	}
	else if (!gstrcmp(argv[0],"wanip"))
	{
		sprintf(cmdbuffer,"ip -4 addr show dev usb0| grep inet| awk '{print $2}'|cut -d'/' -f 1");
	}
	else
	{
		ret = websWrite(wp, T("\"%s\""),"");
		free(IPV6_TYPE);
		free(OUT_IF_SEL);
		free(AP_IPADDR_V6);
		free(PREFIX_LEN_V6);
		return ret;
	}
	
	fd = popen(cmdbuffer,"r");		
	if(fgets(buffer,sizeof(buffer),fd) == NULL)
	{
	    ret = websWrite(wp, T("\"%s\""),"");
	}
	else
	{
		if (buffer[strlen(buffer)-1] == '\n')
		 buffer[strlen(buffer)-1] = '\0';			
		ret = websWrite(wp, T("\"%s\""),buffer);
	}
	pclose(fd);
	free(IPV6_TYPE);
	free(OUT_IF_SEL);
	free(AP_IPADDR_V6);
	free(PREFIX_LEN_V6);
	return ret;
}

void getIPv6Prefix(char_t* ip, char_t* prefixLen, char* prefixResult)
{
	FILE *fd = -1;
	char buffer[512] = {0};
	memset(buffer, '\0', sizeof(buffer));
	memset(prefixResult, '\0', sizeof(prefixResult));
	int prefix_len=gatoi(prefixLen);
	sprintf(buffer,"ipv6_prefix %s %d", ip, prefix_len);
	fd = popen(buffer,"r");
	if(fgets(buffer,sizeof(buffer),fd) == NULL)
	{
	}
	else
	{
		if (buffer[strlen(buffer)-1] == '\n')
		 buffer[strlen(buffer)-1] = '\0';
		 gstrcpy(prefixResult, buffer);
	}
	pclose(fd);
}

static int WebsgetIPv6Prefix(int eid, webs_t wp, int argc, char_t **argv)
{
    char ap_ipaddr_prefix[256] = {0};
    char_t* ip = websGetVar(wp, T(argv[0]), T("FC00::1"));
    char_t* prefixLen = websGetVar(wp, T(argv[1]), T("64"));
    getIPv6Prefix(ip,prefixLen,ap_ipaddr_prefix);
    websWrite(wp, T("\"%s\""),ap_ipaddr_prefix);
	return 0;
}

static void setIPv6OnOff(webs_t wp, char_t *path, char_t *query)
{
	char_t *IPV6_ENABLE, *IPV6_ULA_ENABLE;
	int enableIPv6 = 0;
	
	IPV6_ENABLE = websGetVar(wp, T("IPV6_ENABLE"), T("off"));
	if (!gstrcmp(IPV6_ENABLE,"on")){
	    enableIPv6 = 1;
	    IPV6_ULA_ENABLE = websGetVar(wp, T("IPV6_ULA_ENABLE"), T("off"));
	}
	/***************************************************
	 * write current setting to /sysconfig/ipv6-cfg
	 ****************************************************/	
	char line[128];
	memset(line, '\0', sizeof(line));
	FILE *fp = fopen("/sysconfig/ipv6-cfg", "r");
	FILE *out = fopen("/sysconfig/.ipv6-cfg", "w+");
	if(fp && out)
	{
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {
		  if (!gstrcmp(getSplitStr(line,"=",1),"IPV6_ENABLE"))
		  {
			  memset(line, '\0', sizeof(line));
			  sprintf(line,"IPV6_ENABLE=%s\n",IPV6_ENABLE);
		  } else if (enableIPv6 == 1){
			  if (!gstrcmp(getSplitStr(line,"=",1),"LAN_V6_CONFIG"))
			  {
				  memset(line, '\0', sizeof(line));
				  sprintf(line,"LAN_V6_CONFIG=%s\n",IPV6_ULA_ENABLE);
			  }
		  }
		  fwrite(line, strlen(line), 1, out);	
	  }
	  fclose(fp);
	  fclose(out);

	  system("mv /sysconfig/.ipv6-cfg /sysconfig/ipv6-cfg");
	  system("sync");
	  system("ipv6_service.sh restart");
	  websRedirect(wp, T("advanced/ipv6.asp"));
	} else {
	 
	}
}

static int getIPv6StaticRoutingTable(int eid, webs_t wp, int argc, char_t **argv)
{	
	char* pName = NULL;
	char* pDesNetwork = NULL;
	char* pPreLength = NULL;
	char* pNetworkType = NULL;
	char* pGateway = NULL;
	 	 	
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from IPv6StaticRouting";
	int index = 0;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	} 
	if (nRows > 0) 
	{
		for (index = 0; index < nRows; index++) 
		{					
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Name", pName, getIPv6StaticRoutingTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "DesNetwork", pDesNetwork, getIPv6StaticRoutingTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "PreLength", pPreLength, getIPv6StaticRoutingTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "NetworkType", pNetworkType, getIPv6StaticRoutingTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Gateway", pGateway, getIPv6StaticRoutingTableOut);		 			
					
			websWrite(wp, T("<tr>\n"));				
			websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"pctrl.checkbox\" value=\"%d\"></td>\n"), (index+1));
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pName);		
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pDesNetwork);		
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pPreLength);		
			if (gstrstr(pNetworkType,"0")){
				websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), "LAN");						 
			} else {
				websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), "Internet");						 
			}
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pGateway);	
			websWrite(wp, T("</tr>\n"));		
		}
	}

getIPv6StaticRoutingTableOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}	  
	return 0;
}

static void setIPv6StaticRoutingTable(webs_t wp, char_t *path, char_t *query)
{
	char_t * pAction = websGetVar(wp, T("IPv6RoutingAction"), T(""));	
	char_t * pIdx = websGetVar(wp, T("IPv6RoutingIdx"), T(""));
	char_t * pCount = websGetVar(wp, T("IPv6RoutingCount"), T("0"));
	int count=gatoi(pCount);	
	int index=0;
	
	//printf("[setIPv6StaticRoutingTable] pAction = %s, pIdx=%s, pCount=%s\n",pAction,pIdx,pCount);	

	if (!gstrcmp(pAction,"add"))
	{
		ipv6_routing_edit_idx=-1;						
		websRedirect(wp, T("advanced/ipv6_routing_editor.asp"));
	}
	else if (!gstrcmp(pAction,"edit"))
	{
		ipv6_routing_edit_idx=gatoi(pIdx);			
		websRedirect(wp, T("advanced/ipv6_routing_editor.asp"));
	}
	else if (!gstrcmp(pAction,"del"))
	{
		char * pErrMsg = NULL;
		int status = -1;	

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("advanced/ipv6_routing.asp"));
		}
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from IPv6StaticRouting where _ROWID_ in (%s)",pIdx);		
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
		status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);	
		if (pDB)
			sqlite3_close (pDB);

		if (status>=0){
			sqlite3_free (pErrMsg);
			system(EXPORT_CMD);
		}				
		websRedirect(wp, T("advanced/ipv6_routing.asp"));
	}	
}

static int getIPv6RoutingName(int eid, webs_t wp, int argc, char_t **argv)
{
  int ret = 0;
  //printf("getIPv6RoutingName\n");
  char* pIPv6RoutingNameItem = (char *)malloc(128);
  memset(pIPv6RoutingNameItem,'\0', 128);

  int nRows=0, nCols=0;
  char** pIPv6RoutingRes = NULL;
  char * pIPv6RoutingStr = "select * from IPv6StaticRouting";
  
  int index = 0;
  char* pName = NULL;
  
  if ((pIPv6RoutingRes = sqlite3QueryResGet_Webs (pIPv6RoutingStr, &nCols, &nRows)) == NULL){
    printf("sqlite3QueryResGet failed: query='%s'\n",pIPv6RoutingStr);
	free(pIPv6RoutingNameItem);
    return 0;
  }
  
  for (index = 0; index < nRows; index++) {
    DB_TABLE_VALUE_GET(pIPv6RoutingRes, nCols, index, "Name", pName, getIPv6RoutingNameOut);
    gstrcat(pIPv6RoutingNameItem, pName);
    gstrcat(pIPv6RoutingNameItem, pSep);
  }
  
  getIPv6RoutingNameOut:
  if (pIPv6RoutingRes)
  {
   sqlite3_free_table(pIPv6RoutingRes);
  }

  ret = websWrite(wp, T("\"%s\""),pIPv6RoutingNameItem);
  free(pIPv6RoutingNameItem);
  return ret;
}	

static int getIPv6RoutingEdit(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	if (ipv6_routing_edit_idx == -1){          //Add
		ret = websWrite(wp, T("\"%s\""),"");
		return ret;
    }else if (ipv6_routing_edit_idx > 0){	   //Edit	
  		//printf("getIPv6RoutingEdit\n");
  		char * pName = jnrp_sql_getAttr_Webs("IPv6StaticRouting","Name",ipv6_routing_edit_idx);
  		char * pDesNetwork = jnrp_sql_getAttr_Webs("IPv6StaticRouting","DesNetwork",ipv6_routing_edit_idx);
  		char * pPreLength = jnrp_sql_getAttr_Webs("IPv6StaticRouting","PreLength",ipv6_routing_edit_idx);
 		char * pNetworkType = jnrp_sql_getAttr_Webs("IPv6StaticRouting","NetworkType",ipv6_routing_edit_idx);
		char * pGateway = jnrp_sql_getAttr_Webs("IPv6StaticRouting","Gateway",ipv6_routing_edit_idx);

  		char* pIPv6RoutingEditItem = (char *)malloc(128);
  		memset(pIPv6RoutingEditItem,'\0', 128);
  		gstrcat(pIPv6RoutingEditItem, pName);
		gstrcat(pIPv6RoutingEditItem, pSep);
  		gstrcat(pIPv6RoutingEditItem, pDesNetwork);
  		gstrcat(pIPv6RoutingEditItem, pSep);
  		gstrcat(pIPv6RoutingEditItem, pPreLength);
  		gstrcat(pIPv6RoutingEditItem, pSep);
  		gstrcat(pIPv6RoutingEditItem, pNetworkType);
  		gstrcat(pIPv6RoutingEditItem, pSep);
  		gstrcat(pIPv6RoutingEditItem, pGateway);
  		gstrcat(pIPv6RoutingEditItem, pSep);

		ret = websWrite(wp, T("\"%s\""), pIPv6RoutingEditItem);
		free(pIPv6RoutingEditItem);
  		return ret;
    }
}

static void setIPv6RoutingEdit(webs_t wp, char_t *path, char_t *query)
{
	//printf("setIPv6RoutingEdit\n");
    char * pErrMsg = NULL;
    int status = -1;
	char script[1024];

	char_t * pName = websGetVar(wp, T("ipv6Routing_name"), T(""));
    char_t * pDesNetwork = websGetVar(wp, T("ipv6Routing_desIP"), T(""));
    char_t * pPreLength = websGetVar(wp, T("ipv6Routing_prelength"), T(""));
    char_t * pNetworkType = websGetVar(wp, T("ipv6Routing_network"), T(""));
	char_t * pGateway = websGetVar(wp, T("ipv6Routing_gateway"), T(""));

	//printf("setIPv6RoutingEdit---pName=%s, pDesNetwork=%s, pPreLength=%s, pNetworkType=%s, pGateway=%s\n",
	//	pName,pDesNetwork,pPreLength,pNetworkType,pGateway);
	
	sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pIPv6RoutingEdit [1024] = {0};
    memset (pIPv6RoutingEdit, 0x00, sizeof(pIPv6RoutingEdit));

    if (ipv6_routing_edit_idx == -1){          //Add
		snprintf(pIPv6RoutingEdit,sizeof(pIPv6RoutingEdit), "insert into IPv6StaticRouting values ('%s','%s','%s','%s','%s')"
            ,pName,pDesNetwork,pPreLength,pNetworkType,pGateway);
        status = sqlite3_exec (pDB, pIPv6RoutingEdit, NULL, NULL, &pErrMsg);
    }else if (ipv6_routing_edit_idx > 0){	  //Edit
    	snprintf (pIPv6RoutingEdit, sizeof(pIPv6RoutingEdit),"update IPv6StaticRouting set Name='%s', DesNetwork='%s', PreLength='%s', NetworkType='%s', Gateway='%s' where _ROWID_='%d'"
      		,pName,pDesNetwork,pPreLength,pNetworkType,pGateway,ipv6_routing_edit_idx);
        status = sqlite3_exec (pDB, pIPv6RoutingEdit, NULL, NULL, &pErrMsg);
    }	

	if (pDB){
        sqlite3_close (pDB);
    }
	
    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); 
    }
	websRedirect(wp, T("advanced/ipv6_routing.asp"));
}

static int getIPv6FirewallTable(int eid, webs_t wp, int argc, char_t **argv)
{	
	printf("%s\n", __func__);		
	
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from IPv6Firewall";
	int index = 0;
	char* pServiceName = NULL;
	char* pHostName = NULL;
	char* pStatus = NULL;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getIPv6FirewallOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "HostName", pHostName, getIPv6FirewallOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Status", pStatus, getIPv6FirewallOut);			
			websWrite(wp, T("<tr>\n"));			 
			websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"IPv6_FirewallRule_checkbox\" id=\"firewallTbl_id_%d\"></td>\n"), index+1);
			websWrite(wp, T("<td class=\"tdEven\" id=\"ipv6_firewall_service_name_%d\">%s</td>\n"), index+1, pServiceName);	 
			websWrite(wp, T("<td class=\"tdEven\" id=\"ipv6_firewall_host_name_%d\">%s</td>\n"), index+1, pHostName);	
			if (!gstrcmp(pStatus,"0"))			
				websWrite(wp, T("<td class=\"tdEven\" id=\"ipv6_firewall_host_status_%d\">%s</td>\n"), index+1, "Allow");							
			else
				websWrite(wp, T("<td class=\"tdEven\" id=\"ipv6_firewall_host_status_%d\">%s</td>\n"), index+1, "Block");											
			websWrite(wp, T("</tr>\n"));			
		}
	}
	
getIPv6FirewallOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;	
}

static void setIPv6FirewallTable(webs_t wp, char_t *path, char_t *query)
{
	char_t * pAction = websGetVar(wp, T("IPv6FirewallTblAction"), T(""));	
	char_t * pIdx = websGetVar(wp, T("IPv6FirewallTblIdx"), T(""));
	char_t * pCount = websGetVar(wp, T("IPv6FirewallTblCount"), T("0"));
	int count=gatoi(pCount);	
	int index=0;
	
	printf("[setIPv6FirewallTable] pAction = %s, pIdx=%s, pCount=%s\n",pAction,pIdx,pCount);	

	if (!gstrcmp(pAction,"add"))
	{
		ipv6_filter_edit_idx=-1;						
		websRedirect(wp, T("advanced/ipv6_firewall_editor.asp"));
	}
	else if (!gstrcmp(pAction,"edit"))
	{
		ipv6_filter_edit_idx=gatoi(pIdx);			
		websRedirect(wp, T("advanced/ipv6_firewall_editor.asp"));
	}
	else if (!gstrcmp(pAction,"del"))
	{
		char * pErrMsg = NULL;
		int status = -1;	

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("advanced/ipv6_filtering.asp"));
		}
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from IPv6Firewall where _ROWID_ in (%s)",pIdx);		
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
		status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);	
		if (pDB)
			sqlite3_close (pDB);

		if (status>=0){
			sqlite3_free (pErrMsg);
			system(EXPORT_CMD);
		}				
		websRedirect(wp, T("advanced/ipv6_filtering.asp"));
	}	
}

static int getIPv6HostList(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	//printf("%s\n", __func__);	
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from hostTbl where Ipv6Addr !=''";
	int index = 0;
	char* pCname = NULL;
    char * dut_name = jnrp_sql_getAttr_Webs("system", "name", 1);	

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	
	websWrite(wp, T("<option value=\"%s\" disabled>%s</option>"),dut_name, dut_name);		
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Cname", pCname, getHostListOut);
			websWrite(wp, T("<option value=\"%s\">%s</option>\n"), pCname, pCname);
		}
	}
	websWrite(wp, T("<option value=\"-1\"><script>document.write(gettext(\"Other\"));</script></option>"));

	getHostListOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;

}

static int getIPv6FirewallConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif	

	int ret = 0;
	char * pIPv6FirewallTblItem = (char *)malloc(128);
	memset(pIPv6FirewallTblItem, '\0', 128);
	
	if (ipv6_filter_edit_idx==-1) 			//add
	{
		//get first service name & host name from db
		char condition[256]={0};
		snprintf(condition,sizeof(condition), "Ipv6Addr!=''", "");	
		char * pServiceName = jnrp_sql_getAttr_Webs("Services","ServiceName",1);
		char * pHostName = jnrp_sql_getAttrByCondit_Webs("hostTbl","Cname",condition);
		if (gstrcmp(pServiceName,"")==0)		
			gstrcat(pIPv6FirewallTblItem, "-1");
		else
			gstrcat(pIPv6FirewallTblItem, pServiceName);
		gstrcat(pIPv6FirewallTblItem, pSep);
		if (gstrcmp(pHostName,"")==0)
			gstrcat(pIPv6FirewallTblItem, "-1");
		else
			gstrcat(pIPv6FirewallTblItem, pHostName);
		gstrcat(pIPv6FirewallTblItem, pSep);
		gstrcat(pIPv6FirewallTblItem, "0");			
	}
	else if (ipv6_filter_edit_idx>0) //edit
	{
		char * pServiceName = jnrp_sql_getAttr_Webs("IPv6Firewall","ServiceName",ipv6_filter_edit_idx);
		char * pHostName = jnrp_sql_getAttr_Webs("IPv6Firewall","HostName",ipv6_filter_edit_idx);
		char * pStatus = jnrp_sql_getAttr_Webs("IPv6Firewall","Status",ipv6_filter_edit_idx);
		gstrcat(pIPv6FirewallTblItem, pServiceName);
		gstrcat(pIPv6FirewallTblItem, pSep);
		gstrcat(pIPv6FirewallTblItem, pHostName);
		gstrcat(pIPv6FirewallTblItem, pSep);
		gstrcat(pIPv6FirewallTblItem, pStatus);		
	}
	#ifdef DEBUG_MSG
	printf("pIPv6FirewallTblItem=%s\n", pIPv6FirewallTblItem);
	#endif
	ret = websWrite(wp, T("\"%s\""), pIPv6FirewallTblItem);
	free(pIPv6FirewallTblItem);
	return ret;
}

static void setIPv6FirewallConfig(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	char * pIPv6FirewallConfigAction = websGetVar(wp, T("IPv6FirewallConfigAction"), T(""));

	if (!gstrcmp(pIPv6FirewallConfigAction,"addService"))
	{
		special_apps_edit_idx=-1;
		websRedirect(wp, T("advanced/special_apps_editor.asp"));
	}
	else if (!gstrcmp(pIPv6FirewallConfigAction,"addHost"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),"");		
		websRedirect(wp, T("advanced/known_computers_editor.asp"));		
	}	
	else if (!gstrcmp(pIPv6FirewallConfigAction,"apply"))
	{
		char * pServiceName = websGetVar(wp, T("service"), T(""));
		char * pHostName = websGetVar(wp, T("name"), T(""));
		char * pStatus = websGetVar(wp, T("status"), T(""));
		char * pEditFlag = websGetVar(wp, T("EditFlag"), T(""));		
		int ppStatus=0;
		if (!gstrcmp(pStatus,"Allow"))
			ppStatus=0;
		else
			ppStatus=1;

		#ifdef DEBUG_MSG
		printf("pServiceName=%s, pHostName=%s, pStatus=%d\n", pServiceName, pHostName, ppStatus);
		#endif		

		//check if rule is duplicate
		char condition[128]={0};
		snprintf(condition,sizeof(condition), "ServiceName='%s' and HostName='%s'", pServiceName, pHostName);					
		char * pQueryResult = jnrp_sql_getAttrByCondit_Webs("IPv6Firewall","ServiceName",condition);						
		#ifdef DEBUG_MSG		
		printf("pQueryResult=%s\n", pQueryResult);		
		#endif
		if (!gstrcmp(pServiceName,"-1") && !gstrcmp(pHostName,"-1"))
		{
			if (ipv6_filter_edit_idx==-1)
				setResultCode("Failed to add rule. Please select service and destination from provided list or if you have selected other for service and destination, add them by clicking Add Application and Add Computer.");
			else
				setResultCode("Failed to update rule. Please select service and destination from provided list or if you have selected other for service and destination, add them by clicking Add Application and Add Computer.");				
		}		
		else if (!gstrcmp(pServiceName,"-1"))
		{
			if (ipv6_filter_edit_idx==-1)
				setResultCode("Failed to add rule. Please select service from provided list or if you have selected other for service, add a service by clicking Add Application.");
			else
				setResultCode("Failed to update rule. Please select service from provided list or if you have selected other for service, add a service by clicking Add Application.");				
		}
		else if (!gstrcmp(pHostName,"-1"))
		{
			if (ipv6_filter_edit_idx==-1)				
				setResultCode("Failed to add rule. Please select destination from provided list or if you have selected other for destination, add a destination computer by clicking Add Computer.");		
			else
				setResultCode("Failed to update rule. Please select destination from provided list or if you have selected other for destination, add a destination computer by clicking Add Computer.");						
		}
		else if ((gstrcmp(pQueryResult,"") && ipv6_filter_edit_idx==-1) || (gstrcmp(pQueryResult,"") && !gstrcmp(pEditFlag,"change")))
		{
			//the rule exists
			setResultCode("Provided rule already exists");
		}
		else
		{					
			char pSQLCmd [128] = {0};
			memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
			if (ipv6_filter_edit_idx==-1) //add
			{
				#ifdef DEBUG_MSG
				printf("setIPv6FirewallConfig add item\n");
				#endif
				snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into IPv6Firewall values ('%s','%s',%d)", pServiceName, pHostName, ppStatus);
			}
			else if (ipv6_filter_edit_idx>0) //edit
			{
				#ifdef DEBUG_MSG
				printf("setIPv6FirewallConfig edit item=%d\n", ipv6_filter_edit_idx);
				#endif
				snprintf(pSQLCmd,sizeof(pSQLCmd), "update IPv6Firewall set ServiceName='%s', HostName='%s', Status=%d where _ROWID_=%d", pServiceName, pHostName, ppStatus, ipv6_filter_edit_idx);
			}		
			execSQLCmd(pSQLCmd);	
		}
		websRedirect(wp, T("advanced/ipv6_filtering.asp"));	
	}
}

static int getIPv6Clients(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif	

	FILE *fp = fopen("/tmp/connectipv6dev", "r");
 	char line[1024];
 	memset(line, '\0', sizeof(line));
	DumpConnectDevice dumpdevice[10];
	int num = 0;
	char *data = NULL;
	int count = 0;
 	if(fp) {
	  	while(fgets(line, sizeof(line), fp) != NULL){
			printf("num=%d, line=%s\n", num,line);
			data = strtok(line, " \n");
            count = 1;
			memset(dumpdevice[num].ip_address, 0, sizeof(dumpdevice[num].ip_address));
			memset(dumpdevice[num].mac_address, 0, sizeof(dumpdevice[num].mac_address));
			memset(dumpdevice[num].connect_type, 0, sizeof(dumpdevice[num].connect_type));
        	memset(dumpdevice[num].host_name, 0, sizeof(dumpdevice[num].host_name));
			while (data != NULL){
				switch (count) {
					case 1:
						if (gstrcmp(data, "(null)") == 0){
							gstrcpy(dumpdevice[num].ip_address, "");
						} else {
							gstrcpy(dumpdevice[num].ip_address, data);
						}
						break;
					case 2:
						if (gstrcmp(data, "(null)") == 0){
							gstrcpy(dumpdevice[num].mac_address, "");
						} else {
							gstrcpy(dumpdevice[num].mac_address, data);
						}
						break;
					case 3:
						if (gstrcmp(data, "(null)") == 0){
							gstrcpy(dumpdevice[num].connect_type, "");
						} else {
							gstrcpy(dumpdevice[num].connect_type, data);
						}
						break;
					case 4:
						if (gstrcmp(data, "(null)") == 0){
							gstrcpy(dumpdevice[num].host_name, "");
						} else {
							gstrcpy(dumpdevice[num].host_name, data);
						}
						break;
					default:
						break;
				}
				count++;
				data = strtok(NULL, " \n");
			}
			num++;
 		}
		fclose(fp);
 	}
	int i =0;
	for (i = 0; i < num ; i++) {
		websWrite(wp, T("<tr bgcolor=\"#EEEEEE\">"));
		websWrite(wp, T("<td class=\"tdEven\">%s</td>"),dumpdevice[i].host_name);
		websWrite(wp, T("<td class=\"tdEven\">%s</td>"),dumpdevice[i].ip_address);
		websWrite(wp, T("<td class=\"tdEven\">%s</td>"),dumpdevice[i].mac_address);
		websWrite(wp, T("</tr>"));
	}
 	return 0;
}

static void updateIPv6ClientList(webs_t wp, char_t *path, char_t *query)
{
    printf("Entern updateIPv6ClientList---");
	system("queryipv6dev > /dev/null 2>&1");
	websRedirect(wp, T("advanced/ipv6_clientlist.asp"));
}

static void setIPv6LAN(webs_t wp, char_t *path, char_t *query)
{
	char_t *IPV6_ENABLE, *LAN_V6_TYPE, *AP_IPADDR_V6, *PREFIX_LEN_V6,
		*LAN_V6_DNS, *WAN_IPV6PRIDNS, *WAN_IPV6SECDNS;
	int enableIPv6 = 0;
	char ap_ipaddr_prefix[256] = {0};
	
	IPV6_ENABLE = websGetVar(wp, T("IPV6_ENABLE"), T("off"));
	if (!gstrcmp(IPV6_ENABLE,"on")){
	    enableIPv6 = 1;

	    LAN_V6_TYPE = websGetVar(wp, T("LAN_V6_TYPE"), T("stateless_dhcp"));
		AP_IPADDR_V6 = websGetVar(wp, T("AP_IPADDR_V6"), T("FC00::1"));
		PREFIX_LEN_V6 = websGetVar(wp, T("PREFIX_LEN_V6"), T("64"));
		getIPv6Prefix(AP_IPADDR_V6,PREFIX_LEN_V6,ap_ipaddr_prefix);
		LAN_V6_DNS = websGetVar(wp, T("LAN_V6_DNS"), T("off"));
		WAN_IPV6PRIDNS = websGetVar(wp, T("WAN_IPV6PRIDNS"), T("2001:4860:4860::8888"));
		WAN_IPV6SECDNS = websGetVar(wp, T("WAN_IPV6SECDNS"), T("2001:470:20::2"));
	}
	/***************************************************
	 * write current setting to /sysconfig/ipv6-cfg
	 ****************************************************/	
	char line[128];
	memset(line, '\0', sizeof(line));
	FILE *fp = fopen("/sysconfig/ipv6-cfg", "r");
	FILE *out = fopen("/sysconfig/.ipv6-cfg", "w+");
	if(fp && out)
	{
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {
		if (!gstrcmp(getSplitStr(line,"=",1),"IPV6_ENABLE"))
		{
			memset(line, '\0', sizeof(line));
			sprintf(line,"IPV6_ENABLE=%s\n",IPV6_ENABLE);
		} else if (enableIPv6 == 1){
		
			if (!gstrcmp(getSplitStr(line,"=",1),"LAN_V6_TYPE"))
			{
				memset(line, '\0', sizeof(line));
				sprintf(line,"LAN_V6_TYPE=%s\n",LAN_V6_TYPE);
			}
			else if (!gstrcmp(getSplitStr(line,"=",1),"AP_IPADDR_V6"))
			{
				memset(line, '\0', sizeof(line));
				sprintf(line,"AP_IPADDR_V6=%s\n",AP_IPADDR_V6);
			}
			else if (!gstrcmp(getSplitStr(line,"=",1),"AP_IPADDR_V6_PREFIX"))
			{
				memset(line, '\0', sizeof(line));
				sprintf(line,"AP_IPADDR_V6_PREFIX=%s\n",ap_ipaddr_prefix);
			}
			else if (!gstrcmp(getSplitStr(line,"=",1),"PREFIX_LEN_V6"))
			{
				memset(line, '\0', sizeof(line));
				sprintf(line,"PREFIX_LEN_V6=%s\n",PREFIX_LEN_V6);
			}
			else if (!gstrcmp(getSplitStr(line,"=",1),"LAN_V6_DNS"))
			{
				memset(line, '\0', sizeof(line));
				sprintf(line,"LAN_V6_DNS=%s\n",LAN_V6_DNS);
			}
			else if (!gstrcmp(getSplitStr(line,"=",1),"WAN_IPV6PRIDNS"))
			{
				memset(line, '\0', sizeof(line));
				sprintf(line,"WAN_IPV6PRIDNS=%s\n",WAN_IPV6PRIDNS);
			}
			else if (!gstrcmp(getSplitStr(line,"=",1),"WAN_IPV6SECDNS"))
			{
				memset(line, '\0', sizeof(line));
				sprintf(line,"WAN_IPV6SECDNS=%s\n",WAN_IPV6SECDNS);
			}
		}
		fwrite(line, strlen(line), 1, out);	
	  }
	  fclose(fp);
	  fclose(out);

	  system("mv /sysconfig/.ipv6-cfg /sysconfig/ipv6-cfg");
	  system("sync");
	  system("ipv6_service.sh restart");
	  websRedirect(wp, T("advanced/ipv6.asp"));
	} else {
	 
	}
}

static void setIPv6(webs_t wp, char_t *path, char_t *query)
{
    printf("setIPv6 \n");	
	int ipv6_mode_select = ipv6_disable;
	char cmdbuffer[100] = {0};	

	/* auto-configuration */
	char_t *DSLITE, *DSL_CGNV6, *DSL_CGNV4, *B4_ADDR; //same as static setting
	
	/* static ipv6 */
	char_t *WAN_IPV6ADDR, *WAN_IPV6PREFIX, *WAN_IPV6GW;

	/* pppoe */
	char_t *PPPOE_ADDR, *PPPOE_V6ADDR, *WAN_USER, *WAN_PWD;

	/* ipv6 in ipv4 tunnel */
	char_t *IP6IN4REMOTEIP4, *IP6IN4REMOTEIP6, *IP6IN4LOCALIP6;

	/* ipv6 in ipv4 gogoc setting */
	char_t *GOGOC_SERVER, *GOGOC_AUTH_METHOD, *GOGOC_USERID, *GOGOC_PASSWD;

	/* 6to4 */
	char_t *IP6TO4RELAY;

	/* 6rd */
	char_t *IP6RD_CONFIG,*IP6RDPREFIX,*IP6RDPREFIXLEN,*IP6RDRELAY;

	/* common */
	char_t *IPV6_ENABLE, *IPV6_TYPE, *DHCPPD_ENABLE, *AP_IPADDR_V6,
		*PREFIX_LEN_V6, *LAN_V6_DNS, *WAN_IPV6PRIDNS, *WAN_IPV6SECDNS,
		*LAN_V6_CONFIG, *LAN_V6_TYPE, *RA_LIFETIME, 
		*LAN_DHCPV6START, *LAN_DHCPV6END;
	char_t *AP_IPADDR_V6_PREFIX;	

	//brook's request: reset ipv6 before writing config
	system("ipv6_reset.sh");

	/***************************************************
	 * common setting
	 ****************************************************/	
	IPV6_ENABLE = websGetVar(wp, T("IPV6_ENABLE"), T("off"));
	IPV6_TYPE = websGetVar(wp, T("IPV6_TYPE"), T("dyna_dual")); 		 
	DHCPPD_ENABLE = websGetVar(wp, T("DHCPPD_ENABLE"), T("off"));
	AP_IPADDR_V6 = websGetVar(wp, T("AP_IPADDR_V6"), T("FC00::1"));
	AP_IPADDR_V6_PREFIX = websGetVar(wp, T("AP_IPADDR_V6_PREFIX"), T("FC00::"));
	PREFIX_LEN_V6 = websGetVar(wp, T("PREFIX_LEN_V6"), T("64"));
	LAN_V6_DNS = websGetVar(wp, T("LAN_V6_DNS"), T("off"));
	WAN_IPV6PRIDNS = websGetVar(wp, T("WAN_IPV6PRIDNS"), T("2001:4860:4860::8888"));
	WAN_IPV6SECDNS = websGetVar(wp, T("WAN_IPV6SECDNS"), T("2001:470:20::2"));
	LAN_V6_CONFIG = websGetVar(wp, T("LAN_V6_CONFIG"), T("on"));
	LAN_V6_TYPE = websGetVar(wp, T("LAN_V6_TYPE"), T("stateless_dhcp"));
	RA_LIFETIME = websGetVar(wp, T("RA_LIFETIME"), T("1440"));
	LAN_DHCPV6START = websGetVar(wp, T("LAN_DHCPV6START"), T("100"));
	LAN_DHCPV6END = websGetVar(wp, T("LAN_DHCPV6END"), T("200"));
	
	/***************************************************
	 * auto-configuration setting - same as static setting
	 ****************************************************/	
	DSLITE = websGetVar(wp, T("DSLITE"), T("off"));
	DSL_CGNV6 = websGetVar(wp, T("DSL_CGNV6"), T(""));
	DSL_CGNV4 = websGetVar(wp, T("DSL_CGNV4"), T(""));
	B4_ADDR = websGetVar(wp, T("B4_ADDR"), T(""));

	/***************************************************
	 * static setting
	 ****************************************************/	
	WAN_IPV6ADDR = websGetVar(wp, T("WAN_IPV6ADDR"), T(""));
	WAN_IPV6PREFIX = websGetVar(wp, T("WAN_IPV6PREFIX"), T(""));
	WAN_IPV6GW = websGetVar(wp, T("WAN_IPV6GW"), T(""));

	/***************************************************
	 * pppoe setting
	 ****************************************************/		
	PPPOE_ADDR = websGetVar(wp, T("PPPOE_ADDR"), T(""));
	PPPOE_V6ADDR = websGetVar(wp, T("PPPOE_V6ADDR"), T(""));
	WAN_USER = websGetVar(wp, T("WAN_USER"), T(""));
	WAN_PWD = websGetVar(wp, T("WAN_PWD"), T(""));

	/***************************************************
	 * ipv6 in ipv4 tunnel setting
	 ****************************************************/			
	IP6IN4REMOTEIP4	= websGetVar(wp, T("IP6IN4REMOTEIP4"), T(""));
	IP6IN4REMOTEIP6 = websGetVar(wp, T("IP6IN4REMOTEIP6"), T(""));
	IP6IN4LOCALIP6 = websGetVar(wp, T("IP6IN4LOCALIP6"), T(""));

	/***************************************************
	 * 6to4 setting
	 ****************************************************/			
	IP6TO4RELAY = websGetVar(wp, T("IP6TO4RELAY"), T(""));

	/***************************************************
	 * 6rd setting
	 ****************************************************/			
	IP6RD_CONFIG = websGetVar(wp, T("IP6RD_CONFIG"), T(""));
	IP6RDPREFIX = websGetVar(wp, T("IP6RDPREFIX"), T(""));
	IP6RDPREFIXLEN = websGetVar(wp, T("IP6RDPREFIXLEN"), T(""));
	IP6RDRELAY = websGetVar(wp, T("IP6RDRELAY"), T(""));

	/***************************************************
	 * gogoc setting
	 ****************************************************/	
	GOGOC_SERVER = websGetVar(wp, T("GOGOC_SERVER"), T(""));
	GOGOC_AUTH_METHOD = websGetVar(wp, T("GOGOC_AUTH_METHOD"), T("anonymous"));
	GOGOC_USERID = websGetVar(wp, T("GOGOC_USERID"), T(""));
	GOGOC_PASSWD = websGetVar(wp, T("GOGOC_PASSWD"), T(""));	
	
	#ifdef debug_ipv6
	//for common setting
	printf("IPV6_ENABLE = %s\n", IPV6_ENABLE);
	printf("IPV6_TYPE = %s\n", IPV6_TYPE);	
	printf("DHCPPD_ENABLE = %s\n", DHCPPD_ENABLE);
	printf("AP_IPADDR_V6 = %s\n", AP_IPADDR_V6);
	printf("AP_IPADDR_V6_PREFIX = %s\n", AP_IPADDR_V6_PREFIX);
	printf("PREFIX_LEN_V6 = %s\n", PREFIX_LEN_V6);
	printf("LAN_V6_DNS = %s\n", LAN_V6_DNS);
	printf("WAN_IPV6PRIDNS = %s\n", WAN_IPV6PRIDNS);
	printf("WAN_IPV6SECDNS = %s\n", WAN_IPV6SECDNS);
	printf("LAN_V6_CONFIG = %s\n", LAN_V6_CONFIG);
	printf("LAN_V6_TYPE = %s\n", LAN_V6_TYPE);
	printf("RA_LIFETIME = %s\n", RA_LIFETIME);
	printf("LAN_DHCPV6START = %s\n", LAN_DHCPV6START);
	printf("LAN_DHCPV6END = %s\n", LAN_DHCPV6END);	
	
	//for auto-config connection type
	printf("DSLITE = %s\n", DSLITE);
	printf("DSL_CGNV6 = %s\n", DSL_CGNV6);
	printf("DSL_CGNV4 = %s\n", DSL_CGNV4);
	printf("B4_ADDR = %s\n", B4_ADDR);

	//for static connection type
	printf("WAN_IPV6ADDR = %s\n", WAN_IPV6ADDR);
	printf("WAN_IPV6PREFIX = %s\n", WAN_IPV6PREFIX);
	printf("WAN_IPV6GW = %s\n", WAN_IPV6GW);

	//for pppoe connection type
	printf("PPPOE_ADDR = %s\n", PPPOE_ADDR);
	printf("PPPOE_V6ADDR = %s\n", PPPOE_V6ADDR);
	printf("WAN_USER = %s\n", WAN_USER);
	printf("WAN_PWD = %s\n", WAN_PWD);

	//for ipv6 in ipv4 tunnel
	printf("IP6IN4REMOTEIP4 = %s\n", IP6IN4REMOTEIP4);
	printf("IP6IN4REMOTEIP6 = %s\n", IP6IN4REMOTEIP6);
	printf("IP6IN4LOCALIP6 = %s\n", IP6IN4LOCALIP6);
	
	//for 6to4
	printf("IP6TO4RELAY = %s\n", IP6TO4RELAY);

	//for 6rd
	printf("IP6RD_CONFIG = %s\n", IP6RD_CONFIG);
	printf("IP6RDPREFIX = %s\n", IP6RDPREFIX);
	printf("IP6RDPREFIXLEN = %s\n", IP6RDPREFIXLEN);
	printf("IP6RDRELAY = %s\n", IP6RDRELAY);

	//for gogoc
	printf("GOGOC_SERVER = %s\n", GOGOC_SERVER);
	printf("GOGOC_AUTH_METHOD = %s\n", GOGOC_AUTH_METHOD);
	printf("GOGOC_USERID = %s\n", GOGOC_USERID);	
	printf("GOGOC_PASSWD = %s\n", GOGOC_PASSWD);	
	#endif

	
	/***************************************************
	 * assign ipv6 mode 
	 ****************************************************/			
	if (!gstrcmp(IPV6_ENABLE,"on"))
	{
		if (!gstrcmp(IPV6_TYPE,"static_dual"))
		{
			ipv6_mode_select = ipv6_static;
		}
		else if (!gstrcmp(IPV6_TYPE,"dyna_dual"))
		{	
			ipv6_mode_select = ipv6_autoconfig;
		}	
		else if (!gstrcmp(IPV6_TYPE,"pppoe"))
		{			
			ipv6_mode_select = ipv6_pppoe;
		}
		else if (!gstrcmp(IPV6_TYPE,"6in4"))
		{
			ipv6_mode_select = ipv6_6in4;		
		}
		else if (!gstrcmp(IPV6_TYPE,"6to4"))
		{
			ipv6_mode_select = ipv6_6to4;		
		}
		else if (!gstrcmp(IPV6_TYPE,"6rd"))
		{
			ipv6_mode_select = ipv6_6rd;		
		}		
		else if (!gstrcmp(IPV6_TYPE,"gogoc"))
		{
			ipv6_mode_select = ipv6_gogoc;
		}
	}
	else 
	{
		ipv6_mode_select = ipv6_disable;
	}	
	printf("ipv6_mode_select = %d\n", ipv6_mode_select);

	/***************************************************
	 * generate ap_ipaddr_v6_prefix
	 ****************************************************/	
	printf("before : AP_IPADDR_V6_PREFIX = %s\n", AP_IPADDR_V6_PREFIX);
	FILE *fd = -1;
	char ap_ipaddr_prefix[100] = {0};
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));	
	memset(ap_ipaddr_prefix, '\0', sizeof(ap_ipaddr_prefix));	
	int prefix_len=gatoi(PREFIX_LEN_V6);
	sprintf(cmdbuffer,"ipv6_prefix %s %d", AP_IPADDR_V6, prefix_len);
	printf("cmd=%s\n", cmdbuffer);
	fd = popen(cmdbuffer,"r");	
	if(fgets(ap_ipaddr_prefix,sizeof(ap_ipaddr_prefix),fd) == NULL)
	{
	}
	else
	{
		if (ap_ipaddr_prefix[strlen(ap_ipaddr_prefix)-1] == '\n')
		 ap_ipaddr_prefix[strlen(ap_ipaddr_prefix)-1] = '\0';					
	}
	pclose(fd);	
	printf("after : AP_IPADDR_V6_PREFIX = %s\n", ap_ipaddr_prefix);



	/***************************************************
	 * write current setting to /sysconfig/ipv6-cfg
	 ****************************************************/	
	char line[128];
	memset(line, '\0', sizeof(line));	
	FILE *fp = fopen("/sysconfig/ipv6-cfg", "r");
	FILE *out = fopen("/sysconfig/.ipv6-cfg", "w+");
	if(fp && out)	
	{
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {
	  	/*
   	  	if (line[strlen(line)-1] == '\n')
 	 		line[strlen(line)-1] = '\0';
 	 	*/
 	   	printf("line=%s\n", line);	   
 	   	
		if (!gstrcmp(getSplitStr(line,"=",1),"IPV6_ENABLE"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IPV6_ENABLE=%s\n",IPV6_ENABLE);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"IPV6_TYPE")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IPV6_TYPE=%s\n",IPV6_TYPE);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"DHCPPD_ENABLE")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"DHCPPD_ENABLE=%s\n",DHCPPD_ENABLE);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"AP_IPADDR_V6")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"AP_IPADDR_V6=%s\n",AP_IPADDR_V6);					
		}	
		
		if (!gstrcmp(getSplitStr(line,"=",1),"AP_IPADDR_V6_PREFIX")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			//sprintf(line,"AP_IPADDR_V6_PREFIX=%s\n",AP_IPADDR_V6_PREFIX);	
			sprintf(line,"AP_IPADDR_V6_PREFIX=%s\n",ap_ipaddr_prefix);
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"PREFIX_LEN_V6")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"PREFIX_LEN_V6=%s\n",PREFIX_LEN_V6);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"LAN_V6_DNS")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"LAN_V6_DNS=%s\n",LAN_V6_DNS);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"WAN_IPV6PRIDNS")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"WAN_IPV6PRIDNS=%s\n",WAN_IPV6PRIDNS);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"WAN_IPV6SECDNS")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"WAN_IPV6SECDNS=%s\n",WAN_IPV6SECDNS);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"LAN_V6_CONFIG")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"LAN_V6_CONFIG=%s\n",LAN_V6_CONFIG);					
		}		

		if (!gstrcmp(getSplitStr(line,"=",1),"LAN_V6_TYPE")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"LAN_V6_TYPE=%s\n",LAN_V6_TYPE);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"RA_LIFETIME")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"RA_LIFETIME=%s\n",RA_LIFETIME);					
		}		

		if (!gstrcmp(getSplitStr(line,"=",1),"LAN_DHCPV6START")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"LAN_DHCPV6START=%s\n",LAN_DHCPV6START);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"LAN_DHCPV6END")
			&& ipv6_mode_select!=ipv6_disable)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"LAN_DHCPV6END=%s\n",LAN_DHCPV6END);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"DSLITE")
			&& (ipv6_mode_select==ipv6_static||ipv6_mode_select==ipv6_autoconfig))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"DSLITE=%s\n",DSLITE);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"DSL_CGNV6")
			&& (ipv6_mode_select==ipv6_static||ipv6_mode_select==ipv6_autoconfig))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"DSL_CGNV6=%s\n",DSL_CGNV6);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"DSL_CGNV4")
			&& (ipv6_mode_select==ipv6_static||ipv6_mode_select==ipv6_autoconfig))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"DSL_CGNV4=%s\n",DSL_CGNV4);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"B4_ADDR")
			&& (ipv6_mode_select==ipv6_static||ipv6_mode_select==ipv6_autoconfig))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"B4_ADDR=%s\n",B4_ADDR);		
		}		

		if (!gstrcmp(getSplitStr(line,"=",1),"WAN_IPV6ADDR")
			&& ipv6_mode_select==ipv6_static)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"WAN_IPV6ADDR=%s\n",WAN_IPV6ADDR); 				
		}
			
		if (!gstrcmp(getSplitStr(line,"=",1),"WAN_IPV6PREFIX")
			&& ipv6_mode_select==ipv6_static)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"WAN_IPV6PREFIX=%s\n",WAN_IPV6PREFIX);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"WAN_IPV6GW")
			&& ipv6_mode_select==ipv6_static)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"WAN_IPV6GW=%s\n",WAN_IPV6GW);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"PPPOE_ADDR")
			&& ipv6_mode_select==ipv6_pppoe)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"PPPOE_ADDR=%s\n",PPPOE_ADDR);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"PPPOE_V6ADDR")
			&& ipv6_mode_select==ipv6_pppoe)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"PPPOE_V6ADDR=%s\n",PPPOE_V6ADDR);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"WAN_USER")
			&& ipv6_mode_select==ipv6_pppoe)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"WAN_USER=%s\n",WAN_USER);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"WAN_PWD")
			&& ipv6_mode_select==ipv6_pppoe)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"WAN_PWD=%s\n",WAN_PWD);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"IP6IN4REMOTEIP4")
			&& ipv6_mode_select==ipv6_6in4)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6IN4REMOTEIP4=%s\n",IP6IN4REMOTEIP4);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"IP6IN4REMOTEIP6")
			&& ipv6_mode_select==ipv6_6in4)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6IN4REMOTEIP6=%s\n",IP6IN4REMOTEIP6);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"IP6IN4LOCALIP6")
			&& ipv6_mode_select==ipv6_6in4)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6IN4LOCALIP6=%s\n",IP6IN4LOCALIP6);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"IP6TO4RELAY")
			&& ipv6_mode_select==ipv6_6to4)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6TO4RELAY=%s\n",IP6TO4RELAY);					
		}	
		
		if (!gstrcmp(getSplitStr(line,"=",1),"IP6RD_CONFIG")
			&& ipv6_mode_select==ipv6_6rd)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6RD_CONFIG=%s\n",IP6RD_CONFIG);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"IP6RDPREFIX")
			&& ipv6_mode_select==ipv6_6rd)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6RDPREFIX=%s\n",IP6RDPREFIX);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"IP6RDPREFIX_LEN")
			&& ipv6_mode_select==ipv6_6rd)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6RDPREFIX_LEN=%s\n",IP6RDPREFIXLEN);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"IP6RDRELAY")
			&& ipv6_mode_select==ipv6_6rd)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IP6RDRELAY=%s\n",IP6RDRELAY);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"GOGOC_SERVER")
			&& ipv6_mode_select==ipv6_gogoc)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"GOGOC_SERVER=%s\n",GOGOC_SERVER);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"GOGOC_AUTH_METHOD")
			&& ipv6_mode_select==ipv6_gogoc)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"GOGOC_AUTH_METHOD=%s\n",GOGOC_AUTH_METHOD);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"GOGOC_USERID")
			&& ipv6_mode_select==ipv6_gogoc)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"GOGOC_USERID=%s\n",GOGOC_USERID);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"GOGOC_PASSWD")
			&& ipv6_mode_select==ipv6_gogoc)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"GOGOC_PASSWD=%s\n",GOGOC_PASSWD);					
		}			

		fwrite(line, strlen(line), 1, out);		
	  }
	  fclose(fp);
	  fclose(out);

	  system("mv /sysconfig/.ipv6-cfg /sysconfig/ipv6-cfg");
	  system("sleep 1");
	  //brook's request: start ipv6 after writing config
	  system("ipv6_service.sh");	  
	  
	  websRedirect(wp, T("advanced/ipv6.asp"));	  
	} else {
	 
	}
}

static void setVPNPassthrough(webs_t wp, char_t *path, char_t *query)
{
    printf("setVPN Passthrough\n");	
	char cmdbuffer[100] = {0};	
	char_t *VPNP_PPTP, *VPNP_L2TP, *VPNP_IPSEC;
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));
	
    VPNP_PPTP = websGetVar(wp, T("VPNP_PPTP"), T("off"));
	VPNP_L2TP = websGetVar(wp, T("VPNP_L2TP"), T("off"));		
	VPNP_IPSEC = websGetVar(wp, T("VPNP_IPSEC"), T("off"));
	printf("VPNP_PPTP = %s\n", VPNP_PPTP);
	printf("VPNP_L2TP = %s\n", VPNP_L2TP);	
	printf("VPNP_IPSEC = %s\n", VPNP_IPSEC);

	//write to /sysconfig/vpn-cfg
	char line[128];
	memset(line, '\0', sizeof(line));	
	FILE *fp = fopen("/sysconfig/vpn-cfg", "r");
	FILE *out = fopen("/sysconfig/.vpn-cfg", "w+");
	if(fp && out)	
	{
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {
	  	/*
   	  	if (line[strlen(line)-1] == '\n')
 	 		line[strlen(line)-1] = '\0';
 	 	*/
 	   	printf("line=%s\n", line);	   
 	   	
		if (!gstrcmp(getSplitStr(line,"=",1),"VPNP_PPTP"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNP_PPTP=%s\n",VPNP_PPTP);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNP_L2TP"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNP_L2TP=%s\n",VPNP_L2TP);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNP_IPSEC"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNP_IPSEC=%s\n",VPNP_IPSEC);		
		}
		fwrite(line, strlen(line), 1, out);		
	  }
	  fclose(fp);
	  fclose(out);

	  system("mv /sysconfig/.vpn-cfg /sysconfig/vpn-cfg");
	  websRedirect(wp, T("advanced/vpn.asp"));	  
	} else {
	  //vpn-cfg file does not exist
	  
	}
}


static void setVPN(webs_t wp, char_t *path, char_t *query)
{
    printf("setVPN \n");	
 	int vpn_mode_select = vpn_disable;
	
	char cmdbuffer[100] = {0};	
	char_t *VPN_MODE, *VPNS_MODE, *VPNC_MODE;
	char_t *VPNS_PPTP_USERNAME, *VPNS_PPTP_PW, 
		   *VPNS_PPTP_REMOTE_IP_RANGE, 
		   *VPNS_L2TP_USERNAME, *VPNS_L2TP_PW, 
           *VPNS_L2TP_REMOTE_IP_RANGE, *VPNS_L2TP_MODE;
	char_t *VPNC_PPTP_USERNAME, *VPNC_PPTP_PW, *VPNC_PPTP_SERVER,
			*VPNC_L2TP_USERNAME, *VPNC_L2TP_PW, *VPNC_L2TP_SERVER, *VPNC_L2TP_MODE;
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));
	
    VPN_MODE = websGetVar(wp, T("VPN_MODE"), T("disable"));
	VPNS_MODE = websGetVar(wp, T("VPNS_MODE"), T("pptp"));		
	VPNC_MODE = websGetVar(wp, T("VPNC_MODE"), T("pptp"));
	VPNS_PPTP_USERNAME = websGetVar(wp, T("VPNS_PPTP_USERNAME"), T(""));
	VPNS_PPTP_PW = websGetVar(wp, T("VPNS_PPTP_PW"), T(""));
	VPNS_PPTP_REMOTE_IP_RANGE = websGetVar(wp, T("VPNS_PPTP_REMOTE_IP_RANGE"), T(""));
	VPNS_L2TP_USERNAME = websGetVar(wp, T("VPNS_L2TP_USERNAME"), T(""));
	VPNS_L2TP_PW = websGetVar(wp, T("VPNS_L2TP_PW"), T(""));
	VPNS_L2TP_REMOTE_IP_RANGE = websGetVar(wp, T("VPNS_L2TP_REMOTE_IP_RANGE"), T(""));
	VPNS_L2TP_MODE = websGetVar(wp, T("VPNS_L2TP_MODE"), T(""));
		
	VPNC_PPTP_USERNAME = websGetVar(wp, T("VPNC_PPTP_USERNAME"), T(""));
	VPNC_PPTP_PW = websGetVar(wp, T("VPNC_PPTP_PW"), T("p"));
	VPNC_PPTP_SERVER = websGetVar(wp, T("VPNC_PPTP_SERVER"), T(""));
	VPNC_L2TP_USERNAME = websGetVar(wp, T("VPNC_L2TP_USERNAME"), T(""));
	VPNC_L2TP_PW = websGetVar(wp, T("VPNC_L2TP_PW"), T(""));	
	VPNC_L2TP_SERVER = websGetVar(wp, T("VPNC_L2TP_SERVER"), T(""));
	VPNC_L2TP_MODE = websGetVar(wp, T("VPNC_L2TP_MODE"), T(""));

	/***************************************************
	 * assign vpn mode 
	 ****************************************************/			
	if (!gstrcmp(VPN_MODE,"vpns"))
	{
		if (!gstrcmp(VPNS_MODE,"l2tp"))
		{
			vpn_mode_select = vpns_l2tp;
		}
		else
		{	
			vpn_mode_select = vpns_pptp;
		}	
	}
	else if (!gstrcmp(VPN_MODE,"vpnc"))
	{
		if (!gstrcmp(VPNC_MODE,"l2tp"))
		{
			vpn_mode_select = vpnc_l2tp;
		}
		else
		{
			vpn_mode_select = vpnc_pptp;
		}	
	}
	else //disable
	{
		vpn_mode_select = vpn_disable;
	}	
	printf("vpn_mode_select = %d\n", vpn_mode_select);


	/***************************************************
	 * write current setting to /sysconfig/vpn-cfg
	 ****************************************************/				
	char line[128];
	memset(line, '\0', sizeof(line));	
	FILE *fp = fopen("/sysconfig/vpn-cfg", "r");
	FILE *out = fopen("/sysconfig/.vpn-cfg", "w+");
	if(fp && out)	
	{
	  while(fgets(line, sizeof(line), fp) != NULL)
	  {
	  	/*
   	  	if (line[strlen(line)-1] == '\n')
 	 		line[strlen(line)-1] = '\0';
 	 	*/
 	   	printf("line=%s\n", line);	   
 	   	
		if (!gstrcmp(getSplitStr(line,"=",1),"VPN_MODE"))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPN_MODE=%s\n",VPN_MODE);					
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_MODE")
			&& (vpn_mode_select==vpns_pptp
			|| vpn_mode_select==vpns_l2tp))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_MODE=%s\n",VPNS_MODE);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_MODE")
			&& (vpn_mode_select==vpnc_pptp
			||vpn_mode_select==vpnc_l2tp))
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_MODE=%s\n",VPNC_MODE);		
		}

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_PPTP_USERNAME")
			&& vpn_mode_select==vpns_pptp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_PPTP_USERNAME=%s\n",VPNS_PPTP_USERNAME);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_PPTP_PW")
			&& vpn_mode_select==vpns_pptp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_PPTP_PW=%s\n",VPNS_PPTP_PW);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_PPTP_REMOTE_IP_RANGE")
			&& vpn_mode_select==vpns_pptp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_PPTP_REMOTE_IP_RANGE=%s\n",VPNS_PPTP_REMOTE_IP_RANGE);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_L2TP_USERNAME")
			&& vpn_mode_select==vpns_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_L2TP_USERNAME=%s\n",VPNS_L2TP_USERNAME);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_L2TP_PW")
			&& vpn_mode_select==vpns_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_L2TP_PW=%s\n",VPNS_L2TP_PW);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_L2TP_REMOTE_IP_RANGE")
			&& vpn_mode_select==vpns_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_L2TP_REMOTE_IP_RANGE=%s\n",VPNS_L2TP_REMOTE_IP_RANGE);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNS_L2TP_MODE")
			&& vpn_mode_select==vpns_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_L2TP_MODE=%s\n",VPNS_L2TP_MODE);					
		}		
		
		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_PPTP_USERNAME")
			&& vpn_mode_select==vpnc_pptp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_PPTP_USERNAME=%s\n",VPNC_PPTP_USERNAME);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_PPTP_PW")
			&& vpn_mode_select==vpnc_pptp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_PPTP_PW=%s\n",VPNC_PPTP_PW);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_PPTP_SERVER")
			&& vpn_mode_select==vpnc_pptp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_PPTP_SERVER=%s\n",VPNC_PPTP_SERVER);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_L2TP_USERNAME")
			&& vpn_mode_select==vpnc_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_USERNAME=%s\n",VPNC_L2TP_USERNAME);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_L2TP_PW")
			&& vpn_mode_select==vpnc_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_PW=%s\n",VPNC_L2TP_PW);					
		}	

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_L2TP_SERVER")
			&& vpn_mode_select==vpnc_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_SERVER=%s\n",VPNC_L2TP_SERVER);					
		}			

		if (!gstrcmp(getSplitStr(line,"=",1),"VPNC_L2TP_MODE")
			&& vpn_mode_select==vpnc_l2tp)
		{
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_MODE=%s\n",VPNC_L2TP_MODE);					
		}				
		fwrite(line, strlen(line), 1, out);		
	  }
	  fclose(fp);
	  fclose(out);

	  system("mv /sysconfig/.vpn-cfg /sysconfig/vpn-cfg");
	  /*howard add*/
	  int pid = -1;
	  pid = get_pid("vpn-mgr.pid");
	  if(pid > 0)
		kill_process_by_pid(pid);
	  if(strcmp(VPN_MODE, "disable"))
		system("vpn-mgr &"); 
	  /*howard add end*/
	  websRedirect(wp, T("advanced/vpn.asp"));	  
	} else {
	 
	}
}

#if defined(__P02003_Dora)
static int getSecurityConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	char * pSecurityMode = jnrp_sql_getAttr_Webs ("SecurityConfig", "Mode", 1);
	#ifdef DEBUG_MSG
	printf("getSecurityConfig: %s\n", pSecurityMode);
	#endif
	return websWrite(wp, T("%s"),pSecurityMode);
}	

static void setSecurityConfig(webs_t wp, char_t *path, char_t *query)
{
	char * pSecurityMode = websGetVar(wp, T("mode"), T("2"));
	#ifdef DEBUG_MSG
	printf("setSecurityConfig = %s\n",pSecurityMode);	
	#endif
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update SecurityConfig set Mode='%s' where _ROWID_='1'", pSecurityMode);			
	execSQLCmd(pSQLCmd);
	websRedirect(wp, T("advanced/firewall.asp"));
}

static int getDosAttackConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	printf("%s\n", __func__);
	int ret = 0;
	char * pTCPSynFlood = jnrp_sql_getAttr_Webs ("FirewallConfig", "TCPSynFlood", 1);
	char * pICMPPingFlood = jnrp_sql_getAttr_Webs ("FirewallConfig", "ICMPPingFlood", 1);
	char * pICMPFlood = jnrp_sql_getAttr_Webs ("FirewallConfig", "ICMPFlood", 1);	
	#ifdef DEBUG_MSG
	printf("TCPSynFlood=%s, ICMPPingFlood=%s, ICMPFlood=%s\n", pTCPSynFlood,pICMPPingFlood,pICMPFlood);
	#endif
	char* pDosAttackItem = (char *)malloc(128);
	memset(pDosAttackItem,'\0', 128);
	gstrcat(pDosAttackItem, pTCPSynFlood);
	gstrcat(pDosAttackItem, pSep);
	gstrcat(pDosAttackItem, pICMPPingFlood);
	gstrcat(pDosAttackItem, pSep);
	gstrcat(pDosAttackItem, pICMPFlood);
	gstrcat(pDosAttackItem, pSep);

	ret = websWrite(wp, T("\"%s\""), pDosAttackItem);
	free(pDosAttackItem);
	return ret;
}	

static void setDosAttackConfig(webs_t wp, char_t *path, char_t *query)
{
	printf("%s\n", __func__);	
	char * pTCPSynFlood = websGetVar(wp, T("syn_flood"), T("128"));
	char * pICMPPingFlood = websGetVar(wp, T("echo_storm"), T("128"));
	char * pICMPFlood = websGetVar(wp, T("icmp_flood"), T("128"));
	#ifdef DEBUG_MSG
	printf("TCPSynFlood=%s, ICMPPingFlood=%s, ICMPFlood=%s\n", pTCPSynFlood,pICMPPingFlood,pICMPFlood);	
	#endif	
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update FirewallConfig set TCPSynFlood=%d,ICMPPingFlood=%d,ICMPFlood=%d where _ROWID_='1'", gatoi(pTCPSynFlood),gatoi(pICMPPingFlood),gatoi(pICMPFlood));			
	execSQLCmd(pSQLCmd);
	system("/usr/local/bin/firewall_rule.sh");	
	websRedirect(wp, T("advanced/dos_attacks.asp"));
}

static int getHostTable(int eid, webs_t wp, int argc, char_t **argv)
{
	printf("%s\n", __func__);	
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from hostTbl";
	int index = 0;
	char* pCname = NULL;
	char* pIpAddr = NULL;
	char* pIpv6Addr = NULL;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "IpAddr", pIpAddr, getHostTableOut);		
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Ipv6Addr", pIpv6Addr, getHostTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Cname", pCname, getHostTableOut);
			#ifdef DEBUG_MSG
			//printf("index=%d, pCname=%s, pIpAddr=%s, pIpv6Addr=%s\n", index+1, pCname, pIpAddr, pIpv6Addr);			
			#endif
			websWrite(wp, T("<tr>\n"));				
			websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"dns.checkbox\" id=\"hostTbl_id_%d\"></td>\n"), index+1, index+1);
			websWrite(wp, T("<td class=\"tdEven\" id=\"hostTbl_name_%d\">%s</td>\n"), index+1, pCname);		
			if (!gstrcmp(pIpAddr,"")){
				websWrite(wp, T("<td class=\"tdEven\" id=\"hostTbl_ip_%d\">%s</td>\n"), index+1, "None");					 
			} else {
				websWrite(wp, T("<td class=\"tdEven\" id=\"hostTbl_ip_%d\">%s</td>\n"), index+1, pIpAddr);					 
			}
			if (!gstrcmp(pIpv6Addr,"")){
				websWrite(wp, T("<td class=\"tdEven\" id=\"hostTbl_ipv6_%d\">%s</td>\n"), index+1, "None");						 
			} else {
				websWrite(wp, T("<td class=\"tdEven\" id=\"hostTbl_ipv6_%d\">%s</td>\n"), index+1, pIpv6Addr);					 
			}
			websWrite(wp, T("</tr>\n"));			
		}
	}
	
getHostTableOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;
}	

static void setHostTable(webs_t wp, char_t *path, char_t *query)
{
	char_t * pHostTblAction = websGetVar(wp, T("HostTblAction"), T(""));	
	char_t * pHostName = websGetVar(wp, T("HostName"), T(""));
	char_t * pHostDelList = websGetVar(wp, T("HostDelList"), T(""));	

	#ifdef DEBUG_MSG
	printf("pHostTblAction = %s\n",pHostTblAction);	
	printf("pHostName = %s\n", pHostName);
	printf("pHostDelList= %s\n", pHostDelList);	
	#endif	
	if (!gstrcmp(pHostTblAction,"HostTblAdd"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),"");						
		websRedirect(wp, T("advanced/known_computers_editor.asp"));
	}
	else if (!gstrcmp(pHostTblAction,"HostTblEdit"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),pHostName);				
		websRedirect(wp, T("advanced/known_computers_editor.asp"));
	}
	else if (!gstrcmp(pHostTblAction,"HostTblDel"))
	{
		char *token;
		int len=strlen("delete from hostTbl where Cname in ()")+strlen(pHostDelList)+4;
		char* pSQLCmd = (char *)malloc(len);
		memset(pSQLCmd,'\0', len);
		token = gstrtok(pHostDelList, ",");
		while (token != NULL)
		{
			sprintf(pSQLCmd, "delete from hostTbl where Cname in ('%s')", token);
			execSQLCmd(pSQLCmd);
			token = gstrtok(NULL, ",");
		}
		free(pSQLCmd);
		websRedirect(wp, T("advanced/known_computers.asp"));
	}
}

static int getHostConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s=%s\n", __func__,edit_name);		
	#endif
	if (!gstrcmp(edit_name,"")) //add
	{		
		return websWrite(wp, T("\"%s\""),"");
	}
	else //edit
	{
		#ifdef DEBUG_MSG
		printf("pHostName=%s\n", edit_name);
		#endif
		char condition[256]={0};
		snprintf(condition,sizeof(condition), "Cname='%s'", edit_name);	
		char * pIpAddr = jnrp_sql_getAttrByCondit_Webs("hostTbl","IpAddr",condition);
		char * pIpv6Addr = jnrp_sql_getAttrByCondit_Webs("hostTbl","Ipv6Addr",condition);
		int ret = 0;
		char* pHostTblItem = (char *)malloc(128);
		memset(pHostTblItem,'\0', 128);
		gstrcat(pHostTblItem, edit_name);
		gstrcat(pHostTblItem, pSep);
		gstrcat(pHostTblItem, pIpAddr);
		gstrcat(pHostTblItem, pSep);
		gstrcat(pHostTblItem, pIpv6Addr);
		gstrcat(pHostTblItem, pSep);
		ret = websWrite(wp, T("\"%s\""), pHostTblItem);
		free(pHostTblItem);
		return ret;
	}		
}	

static void setHostConfig(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	char * pCname = websGetVar(wp, T("host_name"), T(""));
	char * pIpAddr = websGetVar(wp, T("ip_address"), T(""));
	char * pIpv6Addr = websGetVar(wp, T("ipv6_address"), T(""));

	char * hostname = jnrp_sql_getAttr_Webs ("system", "name", 1);
	if (!gstrcmp(hostname, pCname))
	{
		setResultCode("Computer Name with the same name already exists");
		websRedirect(wp, T("advanced/known_computers.asp"));
		return;
	}
	
	if (!gstrcmp(edit_name,"")) //add	
	{
		int len=strlen("insert into hostTbl values ('','','')")+strlen(pCname)+strlen(pIpAddr)+strlen(pIpv6Addr)+1;
		char* pSQLCmd = (char *)malloc(len);
		memset(pSQLCmd,'\0', len);
		strcat(pSQLCmd, "insert into hostTbl values ('");
		strcat(pSQLCmd, pIpAddr);
		strcat(pSQLCmd, "','");
		strcat(pSQLCmd, pIpv6Addr);
		strcat(pSQLCmd, "','");
		strcat(pSQLCmd, pCname);
		strcat(pSQLCmd, "')");
		execSQLCmd(pSQLCmd);
		free(pSQLCmd);
	}
	else //edit
	{
		int len=strlen("update hostTbl set IpAddr='', Ipv6Addr='' where Cname=''")+strlen(edit_name)+strlen(pIpAddr)+strlen(pIpv6Addr)+1;
		char* pSQLCmd = (char *)malloc(len);
		memset(pSQLCmd,'\0', len);
		strcat(pSQLCmd, "update hostTbl set IpAddr='");
		strcat(pSQLCmd, pIpAddr);
		strcat(pSQLCmd, "', Ipv6Addr='");
		strcat(pSQLCmd, pIpv6Addr);
		strcat(pSQLCmd, "' where Cname='");
		strcat(pSQLCmd, edit_name);
		strcat(pSQLCmd, "'");
		execSQLCmd(pSQLCmd);
		free(pSQLCmd);
	}

	websRedirect(wp, T("advanced/known_computers.asp"));	
}

/**
 * sqlCmd		: sqlite command
 * type			: 0: draw options, 1: draw table content
 */
int getFltrContent(webs_t wp,char *sqlCmd, int type)
{
	char *pErrMsg = NULL;		
	char **ppQueryRes = NULL;
	int nCols = 0;
	int nRows = 0;
	int index=0;
		
	if (!pWebDb) {
	 if (sqlite3_open (pDBFile, &pWebDb) != SQLITE_OK)
	 {
	   printf ("unable to open the DB file %s\n", pDBFile);
	   return -1;
	 }
	 sqlite3_busy_handler (pWebDb, sqliteBusyHandler, &bhWebArg);
	}
	
	if (sqlite3_get_table (pWebDb, sqlCmd, &ppQueryRes, &nRows, &nCols, &pErrMsg) != SQLITE_OK)
	{
	  printf ("jnrp_sql_getAttr_Webs(), sqlite3_get_table failed\n");
	  if (ppQueryRes)
	  {
		sqlite3_free_table (ppQueryRes);
		ppQueryRes = NULL;
	  }
	  sqlite3_free (pErrMsg);
	}	
	//sqlGetTablePrint(ppQueryRes,nCols,nRows);
	// ppQueryRes[0]: column name ; ppQueryRes[1]: column value	
	if (nRows>0)
	{
		for (index = 1; index <= nRows; index++) 
		{
			//printf("pFltrString=%s\n",ppQueryRes[index]);				
			if (type==0)
			{
				if (strstr(sqlCmd,"FltrType=1")!=NULL)
				{		
					websWrite(wp, T("drawOption(document.getElementById('keywords'),\"%s\");\n"), ppQueryRes[index]);				
				}
				else if (strstr(sqlCmd,"FltrType=2")!=NULL)
				{
					websWrite(wp, T("drawOption(document.getElementById('domains'),\"%s\");\n"), ppQueryRes[index]);									
				}
			}
			else
			{
				if (index==nRows)
				{								
					websWrite(wp, T("%s"), ppQueryRes[index]);
				}
				else
				{
					websWrite(wp, T("%s,"), ppQueryRes[index]);
				}				
			}
		}
	}
	return 0;
}
	
static int getFltrTable(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif		
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from ContentFltr";

	int index = 0;
	char* pContentFltrName = NULL;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	 
	if (nRows > 0) 
	{
		for (index = 0; index < nRows; index++) 
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ContentFltrName", pContentFltrName, getFltrTableOut);
			websWrite(wp, T("<tr>\n"));			 
			websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"ContentFltr.checkbox\" id=\"hostTbl_id_%d\"></td>\n"), index+1);
			websWrite(wp, T("<td class=\"tdOdd\" id=\"fltrTbl_fltr_name_%d\">%s</td>\n"), index+1, pContentFltrName);	 			

			int len=strlen("select FltrString from Fltr where ContentFltrName='' and FltrType=1")+strlen(pContentFltrName)+1;
			char* pSQLCmd = (char *)malloc(len);

			//write keywords
			memset(pSQLCmd,'\0', len);
			strcat(pSQLCmd, "select FltrString from Fltr where ContentFltrName='");
			strcat(pSQLCmd, pContentFltrName);
			strcat(pSQLCmd, "' and FltrType=1");
			websWrite(wp, T("<td class=\"tdOdd\" id=\"fltrTbl_fltr_keyword_%d\">\n"),index+1);
			getFltrContent(wp,pSQLCmd,1);
			websWrite(wp, T("</td>\n"));

			//write domains
			memset(pSQLCmd,'\0', len);
			strcat(pSQLCmd, "select FltrString from Fltr where ContentFltrName='");
			strcat(pSQLCmd, pContentFltrName);
			strcat(pSQLCmd, "' and FltrType=2");
			websWrite(wp, T("<td class=\"tdOdd\" id=\"fltrTbl_fltr_domain_%d\">\n"),index+1);
			getFltrContent(wp,pSQLCmd,1);
			websWrite(wp, T("</td>\n"));

			websWrite(wp, T("</tr>\n"));

			free(pSQLCmd);
		}
	}
	 
getFltrTableOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}	 
	return 0;
}	

static void setFltrTable(webs_t wp, char_t *path, char_t *query)
{	
	char_t * pFltrTblAction = websGetVar(wp, T("FltrTblAction"), T(""));	
	char_t * pFltrTblDelList = websGetVar(wp, T("FltrTblDelList"), T(""));
	char_t * pFltrTblFltrName = websGetVar(wp, T("FltrTblFltrName"), T(""));

	#ifdef DEBUG_MSG
	printf("pFltrTblAction = %s\n",pFltrTblAction);	
	printf("pFltrTblFltrName = %s\n", pFltrTblFltrName);
	printf("pFltrTblDelList= %s\n", pFltrTblDelList);
	#endif
	if (!gstrcmp(pFltrTblAction,"FltrTblAdd"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),"");
		websRedirect(wp, T("advanced/url_blocking_editor.asp"));
	}
	else if (!gstrcmp(pFltrTblAction,"FltrTblEdit"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),pFltrTblFltrName);		
		websRedirect(wp, T("advanced/url_blocking_editor.asp"));
	}
	else if (!gstrcmp(pFltrTblAction,"FltrTblDel"))
	{
		int i, j = 0;
		char *token = NULL;
		int len, len2 = 0;
		char* pSQLCmd;
		char* pListData;

		j = 1;
		for (i = 0; i < strlen(pFltrTblDelList); i++)
		{
			if (pFltrTblDelList[i] == ',')
				j++;
		}
		len = strlen(pFltrTblDelList)+(j*2)+2;
		pListData = (char *)malloc(len);
		memset(pListData, '\0', len);
		token = gstrtok(pFltrTblDelList, ",");
		while (token != NULL)
		{
			gstrcat(pListData, "'");
			gstrcat(pListData, token);
			gstrcat(pListData, "'");
			token = gstrtok(NULL, ",");
			if (token)
				gstrcat(pListData, ",");
		}

		len2 = strlen("delete from ContentFltr where ContentFltrName in ()")+len;
		pSQLCmd = (char *)malloc(len2);
		memset(pSQLCmd, '\0', len2);
		sprintf(pSQLCmd, "delete from ContentFltr where ContentFltrName in (%s)", pListData);
		execSQLCmd(pSQLCmd);
		free(pSQLCmd);

		len2 = strlen("delete from Fltr where ContentFltrName in ()")+len;
		pSQLCmd = (char *)malloc(len2);
		memset(pSQLCmd, '\0', len2);
		sprintf(pSQLCmd, "delete from Fltr where ContentFltrName in (%s)", pListData);
		execSQLCmd(pSQLCmd);
		free(pSQLCmd);

		free(pListData);
		system("/usr/local/bin/urlfilter.sh");
		websRedirect(wp, T("advanced/url_blocking.asp"));
	}
}

static int getFltrRule(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("getFltrRule=%s\n",edit_name);
	#endif	
	if (!gstrcmp(edit_name,"")) //add
	{		
		return websWrite(wp, T("\"%s\""),"");
	}
	else //edit
	{
		#ifdef DEBUG_MSG
		printf("pContentFltrName=%s\n", edit_name);
		#endif
		char condition[128]={0};
		memset(condition, 0x00, sizeof(condition));

		websWrite(wp, T("document.getElementById(\"cntFiltr\").innerHTML=\"%s\";\n"),edit_name);				
		websWrite(wp, T("document.frmUrlConfig.name.value=\"%s\";\n"),edit_name);		
		websWrite(wp, T("document.getElementById(\"cntFiltr\").style.display = \"block\";\n"));		
		websWrite(wp, T("document.getElementById(\"txtcntFiltr\").style.display = \"none\";\n"));		
		snprintf(condition,sizeof(condition), "ContentFltrName='%s'", edit_name);			
		char * pPolicy = jnrp_sql_getAttrByCondit_Webs("ContentFltr","Policy",condition);						
		if (gatoi(pPolicy)==0)
			websWrite(wp, T("document.frmUrlConfig.type.value=\"%s\";\n"),"ACCEPT");
		else
			websWrite(wp, T("document.frmUrlConfig.type.value=\"%s\";\n"),"DROP");		

		int len=strlen("select FltrString from Fltr where ContentFltrName='' and FltrType=1")+strlen(edit_name)+1;
		char* pSQLCmd = (char *)malloc(len);
		
		memset(pSQLCmd,'\0', len);		
		strcat(pSQLCmd, "select FltrString from Fltr where ContentFltrName='");
		strcat(pSQLCmd, edit_name);
		strcat(pSQLCmd, "' and FltrType=1");					
		getFltrContent(wp,pSQLCmd,0);

		memset(pSQLCmd,'\0', len);		
		strcat(pSQLCmd, "select FltrString from Fltr where ContentFltrName='");
		strcat(pSQLCmd, edit_name);
		strcat(pSQLCmd, "' and FltrType=2");					
		getFltrContent(wp,pSQLCmd,0);
		free(pSQLCmd);
	}
	return 0;
}	

static void setFltrRule(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif

	//for add case
	char * pDomainName = websGetVar(wp, T("domainName"), T(""));
	char * pKeywordValue = websGetVar(wp, T("keywordValue"), T(""));
	char * pType = websGetVar(wp, T("type"), T("ACCEPT"));
	char * pContentFltrName = websGetVar(wp, T("name"), T("")); 
	char * pKeywords = websGetVar(wp, T("FltrRuleKeywords"), T(""));
	char * pDomains = websGetVar(wp, T("FltrRuleDomains"), T(""));
	char * pKeywordsCount = websGetVar(wp, T("FltrRuleKeywordsCount"), T(""));
	char * pDomainsCount = websGetVar(wp, T("FltrRuleDomainsCount"), T(""));
	int keywordCnt=gatoi(pKeywordsCount);
	int domainCnt=gatoi(pDomainsCount);
	int index=0;

	#ifdef DEBUG_MSG
	printf("setFltrRule pDomainName=%s\n", pDomainName);
	printf("setFltrRule pKeywordValue=%s\n", pKeywordValue);
	printf("setFltrRule pType=%s\n", pType);
	printf("setFltrRule pContentFltrName=%s\n", pContentFltrName);
	printf("setFltrRule pKeywords=%s\n", pKeywords);
	printf("setFltrRule pDomains=%s\n", pDomains);
	printf("setFltrRule pKeywordsCount=%s\n", pKeywordsCount);
	printf("setFltrRule pDomainsCount=%s\n", pDomainsCount);
	#endif
	
	//insert content filter rule
	int policy=0;
	if (!gstrcmp(pType,"ACCEPT"))
		policy=0;
	else
		policy=1;

	char pSQLCmd [384] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	if (!gstrcmp(edit_name,"")) //add
	{
		snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into ContentFltr values ('%s',%d,%d)", pContentFltrName, policy, 1);
		int result=execSQLCmd(pSQLCmd);
		if (result>=0)
		{			
			for(index=1;index<=keywordCnt;index++)
			{
				#ifdef DEBUG_MSG
				printf("keyword=%s\n",getSplitStr2(pKeywords,",",index));
				#endif
				memset (pSQLCmd, 0x00, sizeof(pSQLCmd)); 
				snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into Fltr values ('%s',%d,'%s')", pContentFltrName, 1, getSplitStr2(pKeywords,",",index));		
				execSQLCmd(pSQLCmd);
			}
			for(index=1;index<=domainCnt;index++)
			{
				#ifdef DEBUG_MSG
				printf("domain=%s\n",getSplitStr2(pDomains,",",index));
				#endif
				memset (pSQLCmd, 0x00, sizeof(pSQLCmd)); 
				snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into Fltr values ('%s',%d,'%s')", pContentFltrName, 2, getSplitStr2(pDomains,",",index));		
				execSQLCmd(pSQLCmd);
			}
		}
	}
	else //edit
	{
		#ifdef DEBUG_MSG
		printf("setFltrRule edit item=%s\n", edit_name);		
		#endif		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "update ContentFltr set Policy=%d where ContentFltrName='%s'", policy, edit_name);
		int result=execSQLCmd(pSQLCmd);

		if (result>=0)
		{
			memset (pSQLCmd, 0x00, sizeof(pSQLCmd)); 
			snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from Fltr where ContentFltrName='%s'", pContentFltrName);		
			execSQLCmd(pSQLCmd);		
			for(index=1;index<=keywordCnt;index++)
			{
				#ifdef DEBUG_MSG
				printf("keyword=%s\n",getSplitStr2(pKeywords,",",index));
				#endif
				memset (pSQLCmd, 0x00, sizeof(pSQLCmd)); 
				snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into Fltr values ('%s',%d,'%s')", pContentFltrName, 1, getSplitStr2(pKeywords,",",index));					
				execSQLCmd(pSQLCmd);
			}
			for(index=1;index<=domainCnt;index++)
			{
				#ifdef DEBUG_MSG
				printf("domain=%s\n",getSplitStr2(pDomains,",",index));
				#endif
				memset (pSQLCmd, 0x00, sizeof(pSQLCmd)); 
				snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into Fltr values ('%s',%d,'%s')", pContentFltrName, 2, getSplitStr2(pDomains,",",index));		
				execSQLCmd(pSQLCmd);
			}		
		}
	}

	system("/usr/local/bin/urlfilter.sh");
	websRedirect(wp, T("advanced/url_blocking.asp"));	
}

static int getServiceList(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	//printf("%s\n", __func__);	
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from Services";
	int index = 0;
	char* pServiceName = NULL;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getServiceListOut);
			websWrite(wp, T("<option value=\"%s\">%s</option>\n"), pServiceName, pServiceName);
		}
	}
	websWrite(wp, T("<option value=\"-1\"><script>document.write(gettext(\"Other\"));</script></option>"));
	
	getServiceListOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;
}	
static int getHostList(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	//printf("%s\n", __func__);	
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from hostTbl where IpAddr !=''";
	int index = 0;
	char* pCname = NULL;
    char * dut_name = jnrp_sql_getAttr_Webs("system", "name", 1);	

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	
	//websWrite(wp, T("<option value=\"%s\" disabled>%s</option>"),dut_name, dut_name);		
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Cname", pCname, getHostListOut);
			websWrite(wp, T("<option value=\"%s\">%s</option>\n"), pCname, pCname);
		}
	}
	websWrite(wp, T("<option value=\"-1\"><script>document.write(gettext(\"Other\"));</script></option>"));
	
	getHostListOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;

}	

static int getACLTable(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	//printf("%s\n", __func__);		
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from FirewallRule";
	int index = 0;
	char* pServiceName = NULL;
	char* pHostName = NULL;
	char* pStatus = NULL;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getACLTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "HostName", pHostName, getACLTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Status", pStatus, getACLTableOut);			
			websWrite(wp, T("<tr>\n"));			 
			websWrite(wp, T("<td class=\"tdEven\"><input type=\"checkbox\" name=\"FirewallRule.checkbox\" id=\"firewallTbl_id_%d\"></td>\n"), index+1);
			websWrite(wp, T("<td class=\"tdEven\" id=\"firewallTbl_service_name_%d\">%s</td>\n"), index+1, pServiceName);	 
			websWrite(wp, T("<td class=\"tdEven\" id=\"firewallTbl_host_name_%d\">%s</td>\n"), index+1, pHostName);	
			if (!gstrcmp(pStatus,"0"))			
				websWrite(wp, T("<td class=\"tdEven\" id=\"firewallTbl_host_status_%d\">%s</td>\n"), index+1, "Allow");							
			else
				websWrite(wp, T("<td class=\"tdEven\" id=\"firewallTbl_host_status_%d\">%s</td>\n"), index+1, "Block");											
			websWrite(wp, T("</tr>\n"));			
		}
	}
	
getACLTableOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;	
}	

static void setACLTable(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	//printf("%s\n", __func__);
	#endif	
	char * pACLTblAction = websGetVar(wp, T("ACLTblAction"), T(""));	
	char_t * pRowId = websGetVar(wp, T("ACLTblIdx"), T(""));

	#ifdef DEBUG_MSG
	//printf("pACLTblAction = %s, pRowId=%s\n",pACLTblAction,pRowId);	
	#endif
	if (!gstrcmp(pACLTblAction,"ACLTblAdd"))
	{
		acl_tbl_idx=-1;
		websRedirect(wp, T("advanced/access_control_editor.asp"));
	}
	else if (!gstrcmp(pACLTblAction,"ACLTblEdit"))
	{
		acl_tbl_idx=gatoi(pRowId);
		websRedirect(wp, T("advanced/access_control_editor.asp"));
	}
	else if (!gstrcmp(pACLTblAction,"ACLTblDel"))
	{
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from FirewallRule where _ROWID_ in (%s)",pRowId);				
		int result = execSQLCmd(pSQLCmd);
		if (result >= 0) {
			system("/usr/local/bin/filter.sh");
		}
		websRedirect(wp, T("advanced/access_control.asp"));
	}		
}

static int getACLConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	//printf("%s\n", __func__);
	#endif
	int ret;
	char * pACLTblItem = (char *)malloc(128);
	memset(pACLTblItem, '\0', 128);	
	if (acl_tbl_idx==-1) //add
	{
		//get first service name & host name from db
		char condition[256]={0};
		snprintf(condition,sizeof(condition), "IpAddr!=''", "");	
		char * pServiceName = jnrp_sql_getAttr_Webs("Services","ServiceName",1);
		char * pHostName = jnrp_sql_getAttrByCondit_Webs("hostTbl","Cname",condition);	
		if (gstrcmp(pServiceName,"")==0)		
			gstrcat(pACLTblItem, "-1");
		else
			gstrcat(pACLTblItem, pServiceName);
		gstrcat(pACLTblItem, pSep);
		if (gstrcmp(pHostName,"")==0)
			gstrcat(pACLTblItem, "-1");
		else
			gstrcat(pACLTblItem, pHostName);
		gstrcat(pACLTblItem, pSep);
		gstrcat(pACLTblItem, "0");			
	}
	else if (acl_tbl_idx>0) //edit
	{
		char * pServiceName = jnrp_sql_getAttr_Webs("FirewallRule","ServiceName",acl_tbl_idx);
		char * pHostName = jnrp_sql_getAttr_Webs("FirewallRule","HostName",acl_tbl_idx);
		char * pStatus = jnrp_sql_getAttr_Webs("FirewallRule","Status",acl_tbl_idx);
		gstrcat(pACLTblItem, pServiceName);
		gstrcat(pACLTblItem, pSep);
		gstrcat(pACLTblItem, pHostName);
		gstrcat(pACLTblItem, pSep);
		gstrcat(pACLTblItem, pStatus);		
	}
	#ifdef DEBUG_MSG
	printf("pACLTblItem=%s\n", pACLTblItem);
	#endif
	ret = websWrite(wp, T("\"%s\""), pACLTblItem);
	free(pACLTblItem);
	return ret;
}	

static void setACLConfig(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	char * pACLConfigAction = websGetVar(wp, T("ACLConfigAction"), T(""));
	
	if (!gstrcmp(pACLConfigAction,"addService"))
	{
		special_apps_edit_idx=-1;
		websRedirect(wp, T("advanced/special_apps_editor.asp"));
	}
	else if (!gstrcmp(pACLConfigAction,"addHost"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),"");		
		websRedirect(wp, T("advanced/known_computers_editor.asp"));		
	}	
	else if (!gstrcmp(pACLConfigAction,"apply"))
	{
		char * pServiceName = websGetVar(wp, T("service"), T(""));
		char * pHostName = websGetVar(wp, T("name"), T(""));
		char * pStatus = websGetVar(wp, T("status"), T(""));
		char * pEditFlag = websGetVar(wp, T("EditFlag"), T(""));		
		int ppStatus=0;
		if (!gstrcmp(pStatus,"Allow"))
			ppStatus=0;
		else
			ppStatus=1;

		#ifdef DEBUG_MSG
		printf("pServiceName=%s, pHostName=%s, pStatus=%d\n", pServiceName, pHostName, ppStatus);
		#endif		

		//check if rule is duplicate
		char condition[128]={0};
		//snprintf(condition,sizeof(condition), "ServiceName='%s' and HostName='%s' and Status=%d", pServiceName, pHostName, ppStatus);			
		snprintf(condition,sizeof(condition), "ServiceName='%s' and HostName='%s'", pServiceName, pHostName);					
		char * pQueryResult = jnrp_sql_getAttrByCondit_Webs("FirewallRule","ServiceName",condition);						
		#ifdef DEBUG_MSG		
		printf("pQueryResult=%s\n", pQueryResult);		
		#endif
		if (!gstrcmp(pServiceName,"-1") && !gstrcmp(pHostName,"-1"))
		{
			if (acl_tbl_idx==-1)
				setResultCode("Failed to add rule. Please select service and destination from provided list or if you have selected other for service and destination, add them by clicking Add Application and Add Computer.");
			else
				setResultCode("Failed to update rule. Please select service and destination from provided list or if you have selected other for service and destination, add them by clicking Add Application and Add Computer.");				
		}		
		else if (!gstrcmp(pServiceName,"-1"))
		{
			if (acl_tbl_idx==-1)
				setResultCode("Failed to add rule. Please select service from provided list or if you have selected other for service, add a service by clicking Add Application.");
			else
				setResultCode("Failed to update rule. Please select service from provided list or if you have selected other for service, add a service by clicking Add Application.");				
		}
		else if (!gstrcmp(pHostName,"-1"))
		{
			if (acl_tbl_idx==-1)				
				setResultCode("Failed to add rule. Please select destination from provided list or if you have selected other for destination, add a destination computer by clicking Add Computer.");		
			else
				setResultCode("Failed to update rule. Please select destination from provided list or if you have selected other for destination, add a destination computer by clicking Add Computer.");						
		}
		else if ((gstrcmp(pQueryResult,"") && acl_tbl_idx==-1) || (gstrcmp(pQueryResult,"") && !gstrcmp(pEditFlag,"change")))
		{
			//the rule exists
			setResultCode("Provided rule already exists");
		}
		else
		{					
			char pSQLCmd [128] = {0};
			memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
			if (acl_tbl_idx==-1) //add
			{
				#ifdef DEBUG_MSG
				printf("setACLConfig add item\n");
				#endif
				snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into FirewallRule values ('%s','%s',%d)", pServiceName, pHostName, ppStatus);
			}
			else if (acl_tbl_idx>0) //edit
			{
				#ifdef DEBUG_MSG
				printf("setACLConfig edit item=%d\n", acl_tbl_idx);
				#endif
				snprintf(pSQLCmd,sizeof(pSQLCmd), "update FirewallRule set ServiceName='%s', HostName='%s', Status=%d where _ROWID_=%d", pServiceName, pHostName, ppStatus, acl_tbl_idx);
			}		
			execSQLCmd(pSQLCmd);	
		}
		websRedirect(wp, T("advanced/access_control.asp"));	
	}
}

static int getParCtrl(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pchkParCtrlEnable = jnrp_sql_getAttr_Webs("UrlFltrConfig","EnableProfiles",1);	 
	int enable=gatoi(pchkParCtrlEnable);	 
	return websWrite(wp, T("%d"), enable);	 
}	

static void setParCtrl(webs_t wp, char_t *path, char_t *query)
{
	char * pchkParCtrlEnable = websGetVar(wp, T("chkParCtrlEnable"), T("off"));		
	#ifdef DEBUG_MSG
	printf("%s : pchkParCtrlEnable = %s\n", __func__, pchkParCtrlEnable);  	
	#endif
	int enable=0;
	if (!gstrcmp(pchkParCtrlEnable,"on"))
		enable=1;
	else if (!gstrcmp(pchkParCtrlEnable,"off"))
		enable=0;
	
	char pSQLCmd [60] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update UrlFltrConfig set EnableProfiles=%d where _ROWID_=1",enable);				
	execSQLCmd(pSQLCmd);
	websRedirect(wp, T("advanced/rules_for_schedule.asp"));	
}

static int getUrlProfileTable(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char* pProfileName = NULL;
	char* pProfileDecr = NULL;
	char* pUserName = NULL;
	char* pSessionTimeout = NULL;
	char* pInactivityTimeout = NULL;
	char* pContentFltrName = NULL;
	char* pScheduleName = NULL;
	 	 	
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from UrlProfile";
	int index = 0;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	} 
	if (nRows > 0) 
	{
		for (index = 0; index < nRows; index++) 
		{					
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ProfileName", pProfileName, getUrlProfileOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ProfileDecr", pProfileDecr, getUrlProfileOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "UserName", pUserName, getUrlProfileOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "SessionTimeout", pSessionTimeout, getUrlProfileOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "InactivityTimeout", pInactivityTimeout, getUrlProfileOut);		 			
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ContentFltrName", pContentFltrName, getUrlProfileOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ScheduleName", pScheduleName, getUrlProfileOut);		 			
					
			websWrite(wp, T("<tr>\n"));				
			websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"pctrl.checkbox\" value=\"%d\"></td>\n"), (index+1));
			websWrite(wp, T("<td class=\"tdOdd\" id=\"profile_name_%d\">%s</td>\n"), (index+1), pProfileName);		
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pProfileDecr);						 
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pUserName);						 
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pSessionTimeout);						 
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pInactivityTimeout);	
			websWrite(wp, T("<td style=\"display:none\" id=\"profile_fltr_name_%d\">%s</td>\n"), (index+1), pContentFltrName);
			websWrite(wp, T("<td style=\"display:none\" id=\"profile_schedule_name_%d\">%s</td>\n"), (index+1), pScheduleName);
			websWrite(wp, T("</tr>\n"));		
		}
	}

getUrlProfileOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}	  
	return 0;
}	

static void setUrlProfileTable(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);  
	#endif
	char_t * pPctlTblAction = websGetVar(wp, T("PctlTblAction"), T(""));	
	char_t * pPctlProfileName = websGetVar(wp, T("PctlProfileName"), T(""));
	char_t * pPctlProfileDelList = websGetVar(wp, T("PctlProfileDelList"), T(""));	
	
	#ifdef DEBUG_MSG
	printf("pPctlTblAction = %s\n",pPctlTblAction); 
	printf("pPctlProfileName = %s\n", pPctlProfileName);
	printf("pPctlProfileDelList= %s\n", pPctlProfileDelList); 	
	#endif	
	if (!gstrcmp(pPctlTblAction,"PctlTblAdd"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),"");						
		websRedirect(wp, T("advanced/parental_conf.asp"));
	}
	else if (!gstrcmp(pPctlTblAction,"PctlTblEdit"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),pPctlProfileName);				
		websRedirect(wp, T("advanced/parental_conf.asp"));
	}
	else if (!gstrcmp(pPctlTblAction,"PctlTblDel"))
	{
		int i, j = 0;
		char *token = NULL;
		int result = 0;
		int len = 0;
		char* pSQLCmd;
		j = 1;
		for (i = 0; i < strlen(pPctlProfileDelList); i++)
		{
			if (pPctlProfileDelList[i] == ',')
				j++;
		}
		len = strlen("delete from UrlProfile where ProfileName in ()")+strlen(pPctlProfileDelList)+(j*2)+2;
		pSQLCmd = (char *)malloc(len);
		memset(pSQLCmd,'\0', len);
		token = gstrtok(pPctlProfileDelList, ",");
		gstrcpy(pSQLCmd, "delete from UrlProfile where ProfileName in (");
		while (token != NULL)
		{
			gstrcat(pSQLCmd, "'");
			gstrcat(pSQLCmd, token);
			gstrcat(pSQLCmd, "'");
			token = gstrtok(NULL, ",");
			if (token)
				gstrcat(pSQLCmd, ",");
		}
		gstrcat(pSQLCmd, ")");
		result = execSQLCmd(pSQLCmd);
		free(pSQLCmd);
		if (result >= 0) {
			 system("/usr/local/bin/schedule_rule.sh");
		}
		websRedirect(wp, T("advanced/rules_for_schedule.asp"));
	}
}

static int getUrlProfileConfig(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s=%s\n", __func__,edit_name);		
	#endif
	if (!gstrcmp(edit_name,"")) //add
	{		
		return websWrite(wp, T("\"%s\""),"");
	}
	else //edit
	{
		#ifdef DEBUG_MSG
		printf("pProfileName=%s\n",edit_name);	
		#endif
		char condition[256]={0};
		snprintf(condition,sizeof(condition), "ProfileName='%s'", edit_name);			
		char * pProfileName = jnrp_sql_getAttrByCondit_Webs("UrlProfile","ProfileName",condition);
		char * pProfileDecr = jnrp_sql_getAttrByCondit_Webs("UrlProfile","ProfileDecr",condition);
		//char * pUserType = jnrp_sql_getAttrByCondit_Webs("UrlProfile","UserType",condition);
		char * pUserName = jnrp_sql_getAttrByCondit_Webs("UrlProfile","UserName",condition);
		char * pEnableContentFltr = jnrp_sql_getAttrByCondit_Webs("UrlProfile","EnableContentFltr",condition);
		char * pContentFltrName = jnrp_sql_getAttrByCondit_Webs("UrlProfile","ContentFltrName",condition);
		char * pEnableSchedules = jnrp_sql_getAttrByCondit_Webs("UrlProfile","EnableSchedules",condition);
		char * pScheduleName = jnrp_sql_getAttrByCondit_Webs("UrlProfile","ScheduleName",condition);
		char * pSessionTimeout = jnrp_sql_getAttrByCondit_Webs("UrlProfile","SessionTimeout",condition);
		char * pInactivityTimeout = jnrp_sql_getAttrByCondit_Webs("UrlProfile","InactivityTimeout",condition);		
		int ret = 0;
		char* pPtrlItem = (char *)malloc(324);
		memset(pPtrlItem,'\0', 324);
		gstrcat(pPtrlItem, pProfileName); 
		gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pProfileDecr); 
		gstrcat(pPtrlItem, pSep);
		//gstrcat(pPtrlItem, pUserType); 
		//gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pUserName); 
		gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pEnableContentFltr); 
		gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pContentFltrName); 
		gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pEnableSchedules); 
		gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pScheduleName); 
		gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pSessionTimeout); 
		gstrcat(pPtrlItem, pSep);
		gstrcat(pPtrlItem, pInactivityTimeout);
		ret = websWrite(wp, T("\"%s\""), pPtrlItem);
		free(pPtrlItem);
		return ret;
	}	
}	

static void setUrlProfileConfig(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pProfileName = websGetVar(wp, T("pctrl.name"), T(""));
	char * pProfileDecr = websGetVar(wp, T("pctrl.descr"), T(""));
	char * pUserName = websGetVar(wp, T("pctrl.group"), T(""));	
	char * pEnableContentFltr = websGetVar(wp, T("pctrl.enable_filter_content"), T(""));
	char * pContentFltrName = websGetVar(wp, T("pctrl.filter_content"), T(""));
	char * pEnableSchedules = websGetVar(wp, T("pctrl.restrict_internet_access"), T(""));
	char * pScheduleName = websGetVar(wp, T("pctrl.schedule"), T(""));
	char * pSessionTimeout = websGetVar(wp, T("pctrl.session_timeout"), T(""));
	char * pInactivityTimeout = websGetVar(wp, T("pctrl.inactivity"), T(""));	

	char pSQLCmd [512] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
	if (!gstrcmp(edit_name,"")) //add
	{	
		#ifdef DEBUG_MSG	
		printf("setUrlProfileConfig add item\n");
		#endif
		snprintf(pSQLCmd,sizeof(pSQLCmd), 
			"insert into UrlProfile values ('%s','%s','%s','%s',%d,'%s',%d,'%s',%d,%d)", 
			pProfileName, 
			pProfileDecr,
			"group",
			pUserName,
			gatoi(pEnableContentFltr),
			pContentFltrName,
			gatoi(pEnableSchedules),
			pScheduleName,
			gatoi(pSessionTimeout),
			gatoi(pInactivityTimeout));
	}
	else  //edit
	{
		#ifdef DEBUG_MSG
		printf("setUrlProfileConfig edit item=%s\n", edit_name);
		#endif
		snprintf(pSQLCmd,sizeof(pSQLCmd), 
			"update UrlProfile set ProfileDecr='%s',UserName='%s',EnableContentFltr=%d,ContentFltrName='%s',EnableSchedules=%d,ScheduleName='%s',SessionTimeout=%d,InactivityTimeout=%d where ProfileName='%s'", 
			pProfileDecr,
			pUserName,
			gatoi(pEnableContentFltr),
			pContentFltrName,
			gatoi(pEnableSchedules),
			pScheduleName,
			gatoi(pSessionTimeout),
			gatoi(pInactivityTimeout),
			edit_name);
	}
	execSQLCmd(pSQLCmd);
	websRedirect(wp, T("advanced/rules_for_schedule.asp"));	
}


static int getFltrList(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from ContentFltr";

	int index = 0;
	char* pContentFltrName = NULL;

	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	 
	if (nRows > 0) 
	{
		for (index = 0; index < nRows; index++) 
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ContentFltrName", pContentFltrName, getFltrListOut);
			websWrite(wp, T("<option value=\"%s\">%s</option>"),pContentFltrName,pContentFltrName);			
		}
	}
	 
getFltrListOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}	 
	return 0;
}	

static int getScheduleTable(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);		
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from Schedules";
	int index = 0;
	char* pScheduleName = NULL;
	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ScheduleName", pScheduleName, getScheduleOut);
			if (!gstrcmp(argv[0],"schedule"))
			{				
				websWrite(wp, T("<tr>\n"));				
				websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"sched.checkbox\"></td>\n"));
				websWrite(wp, T("<td class=\"tdOdd\">%d</td>\n"), (index+1));		
				websWrite(wp, T("<td class=\"tdOdd\" id=\"schedule_name_%d\">%s</td>\n"), (index+1), pScheduleName);						 
				websWrite(wp, T("</tr>\n"));
			}
			else if (!gstrcmp(argv[0],"parental"))
			{
				websWrite(wp, T("<option value=\"%s\">%s</option>"),pScheduleName,pScheduleName);		 
			}			
		}
	}
	
getScheduleOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;		
}	

static void setScheduleTable(webs_t wp, char_t *path, char_t *query)
{
	char_t * pScheduleTblAction = websGetVar(wp, T("ScheduleTblAction"), T(""));	
	char_t * pScheduleName = websGetVar(wp, T("ScheduleName"), T(""));
	char_t * pScheduleDelList = websGetVar(wp, T("ScheduleDelList"), T(""));	

	#ifdef DEBUG_MSG
	printf("pScheduleTblAction = %s\n",pScheduleTblAction);	
	printf("pScheduleName = %s\n", pScheduleName);
	printf("pScheduleDelList= %s\n", pScheduleDelList);		
	#endif
	if (!gstrcmp(pScheduleTblAction,"ScheduleTblAdd"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),"");						
		websRedirect(wp, T("advanced/schedule.asp"));
	}
	else if (!gstrcmp(pScheduleTblAction,"ScheduleTblEdit"))
	{
		memset (edit_name, 0x00, sizeof(edit_name));	
		snprintf(edit_name,sizeof(edit_name),pScheduleName);				
		websRedirect(wp, T("advanced/schedule.asp"));
	}
	else if (!gstrcmp(pScheduleTblAction,"ScheduleTblDel"))
	{
		char *token;
		int len = strlen("delete from Schedules where ScheduleName in ()")+strlen(pScheduleDelList)+4;
		char* pSQLCmd = (char *)malloc(len);
		memset(pSQLCmd,'\0', len);
		token = gstrtok(pScheduleDelList, ",");
		while (token != NULL)
		{
			sprintf(pSQLCmd, "delete from Schedules where ScheduleName in ('%s')", token);
			execSQLCmd(pSQLCmd);			
			token = gstrtok(NULL, ",");
		}
		free(pSQLCmd);
		websRedirect(wp, T("advanced/rules_for_schedule.asp"));
	}
}

static int getSchedule(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s=%s\n", __func__,edit_name);		
	#endif	
	if (!gstrcmp(edit_name,"")) //add
	{		
		return websWrite(wp, T("\"%s\""),"");
	}
	else //edit
	{
		#ifdef DEBUG_MSG
		printf("pScheduleName=%s\n", edit_name);
		#endif
		char condition[256]={0};
		snprintf(condition,sizeof(condition), "ScheduleName='%s'", edit_name);	
		char * pAllDay = jnrp_sql_getAttrByCondit_Webs("Schedules","AllDay",condition);		
		char * pStartTimeHours = jnrp_sql_getAttrByCondit_Webs("Schedules","StartTimeHours",condition);
		char * pStartTimeMins = jnrp_sql_getAttrByCondit_Webs("Schedules","StartTimeMins",condition);
		char * pStartTimeMeridian = jnrp_sql_getAttrByCondit_Webs("Schedules","StartTimeMeridian",condition);
		char * pEndTimeHours = jnrp_sql_getAttrByCondit_Webs("Schedules","EndTimeHours",condition);
		char * pEndTimeMins = jnrp_sql_getAttrByCondit_Webs("Schedules","EndTimeMins",condition);
		char * pEndTimeMeridian = jnrp_sql_getAttrByCondit_Webs("Schedules","EndTimeMeridian",condition);
		char * pDays = jnrp_sql_getAttrByCondit_Webs("Schedules","Days",condition);
		char * pDaysType = jnrp_sql_getAttrByCondit_Webs("Schedules","DaysType",condition);
		int ret = 0;
		char* pScheduleItem = (char *)malloc(128);
		memset(pScheduleItem,'\0', 128);
		gstrcat(pScheduleItem, edit_name); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pAllDay); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pStartTimeHours); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pStartTimeMins); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pStartTimeMeridian); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pEndTimeHours); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pEndTimeMins); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pEndTimeMeridian); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pDays); 
		gstrcat(pScheduleItem, pSep);
		gstrcat(pScheduleItem, pDaysType);
		ret = websWrite(wp, T("\"%s\""), pScheduleItem);
		free(pScheduleItem);
		return ret;
	}	
}	

static void setSchedule(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pScheduleName = websGetVar(wp, T("name"), T(""));
	char * pAllDay = websGetVar(wp, T("time_of_day.value"), T(""));
	char * pStartTimeHours = websGetVar(wp, T("time_of_day.start_hr"), T(""));
	char * pStartTimeMins = websGetVar(wp, T("time_of_day.start_min"), T(""));
	char * pStartTimeMeridian = websGetVar(wp, T("time_of_day.start_am_pm"), T(""));
	char * pEndTimeHours = websGetVar(wp, T("time_of_day.end_hr"), T(""));
	char * pEndTimeMins = websGetVar(wp, T("time_of_day.end_min"), T(""));
	char * pEndTimeMeridian = websGetVar(wp, T("time_of_day.end_am_pm"), T(""));
	char * pDays_monday = websGetVar(wp, T("day_of_week.monday"), T(""));
	char * pDays_tuesday = websGetVar(wp, T("day_of_week.tuesday"), T(""));
	char * pDays_wednesday = websGetVar(wp, T("day_of_week.wednesday"), T(""));
	char * pDays_thursday = websGetVar(wp, T("day_of_week.thursday"), T(""));
	char * pDays_friday = websGetVar(wp, T("day_of_week.friday"), T(""));
	char * pDays_saturday = websGetVar(wp, T("day_of_week.saturday"), T(""));
	char * pDays_sunday = websGetVar(wp, T("day_of_week.sunday"), T(""));
	char * pDaysType = websGetVar(wp, T("day_of_week.value"), T(""));

	int ppAllDay = gatoi(pAllDay);
	int ppStartTimeHours = gatoi(pStartTimeHours);
	int ppStartTimeMins = gatoi(pStartTimeMins);
	int ppStartTimeMeridian = (!gstrcmp(pStartTimeMeridian,"AM"))? 0:1;
	int ppEndTimeHours = gatoi(pEndTimeHours);
	int ppEndTimeMins = gatoi(pEndTimeMins);
	int ppEndTimeMeridian = (!gstrcmp(pEndTimeMeridian,"AM"))? 0:1;	
	int ppDays = gatoi(pDays_monday)*1 + 
				gatoi(pDays_tuesday)*2 + 
				gatoi(pDays_wednesday)*4 + 
				gatoi(pDays_thursday)*8 + 
				gatoi(pDays_friday)*16 + 
				gatoi(pDays_saturday)*32 +
				gatoi(pDays_sunday)*64;
	int ppDaysType = gatoi(pDaysType);
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));

	//default settings
	if(ppAllDay==0)
	{ //5AM-12PM
	  ppStartTimeHours=5;
	  ppStartTimeMins=0;
	  ppStartTimeMeridian=0;
	  ppEndTimeHours=12;
	  ppEndTimeMins=0;
	  ppEndTimeMeridian=0;
	}
	else if (ppAllDay==1)
	{ //12PM-4PM
	  ppStartTimeHours=12;
	  ppStartTimeMins=0;
	  ppStartTimeMeridian=0;
	  ppEndTimeHours=4;
	  ppEndTimeMins=0;
	  ppEndTimeMeridian=1;	
	}
	else if (ppAllDay==2)
	{ //4PM-7PM
	  ppStartTimeHours=4;
	  ppStartTimeMins=0;
	  ppStartTimeMeridian=1;
	  ppEndTimeHours=7;
	  ppEndTimeMins=0;
	  ppEndTimeMeridian=1;		
	}
	else if (ppAllDay==3)
	{ //7PM-12PM
	  ppStartTimeHours=7;
	  ppStartTimeMins=0;
	  ppStartTimeMeridian=1;
	  ppEndTimeHours=12;
	  ppEndTimeMins=0;
	  ppEndTimeMeridian=1;		
	}
	else if (ppAllDay==4)
	{ //Entire Day
	  ppStartTimeHours=0;
	  ppStartTimeMins=0;
	  ppStartTimeMeridian=0;
	  ppEndTimeHours=12;
	  ppEndTimeMins=0;
	  ppEndTimeMeridian=1;		
	}

	if (ppDaysType==0)
	{ //Entire Week
	  ppDays=127;	
	}
	else if (ppDaysType==1)
	{ //Weekends
	  ppDays=96;
	}
	else if (ppDaysType==2)
	{ //Week Days
	  ppDays=31;
	}

	if (!gstrcmp(edit_name,"")) //add
	{	
		#ifdef DEBUG_MSG
		printf("setSchedule add item\n");
		#endif
		snprintf(pSQLCmd,sizeof(pSQLCmd), 
			"insert into Schedules values ('%s',%d,%d,%d,%d,%d,%d,%d,%d,%d)", 
			pScheduleName, 
			ppAllDay,
			ppStartTimeHours,
			ppStartTimeMins,
			ppStartTimeMeridian,
			ppEndTimeHours,
			ppEndTimeMins,
			ppEndTimeMeridian,
			ppDays,
			ppDaysType);
	}
	else //edit
	{
		#ifdef DEBUG_MSG
		printf("setSchedule edit item=%s\n", edit_name);
		#endif
		snprintf(pSQLCmd,sizeof(pSQLCmd), 
			"update Schedules set \
			AllDay=%d, StartTimeHours=%d, StartTimeMins=%d, StartTimeMeridian=%d,\
			EndTimeHours=%d, EndTimeMins=%d, EndTimeMeridian=%d,\
			Days=%d, DaysType=%d where ScheduleName='%s'", 
			ppAllDay,
			ppStartTimeHours,
			ppStartTimeMins,
			ppStartTimeMeridian,
			ppEndTimeHours,
			ppEndTimeMins,
			ppEndTimeMeridian,
			ppDays,
			ppDaysType,
			edit_name);
	}

	execSQLCmd(pSQLCmd);
	websRedirect(wp, T("advanced/rules_for_schedule.asp"));	
}

static void setStaRouteTable(webs_t wp, char_t *path, char_t *query)
{
	char * pStaRouteAction = websGetVar(wp, T("StaRouteAction"), T(""));	
	char_t * pStaRouteIdx = websGetVar(wp, T("StaRouteIdx"), T(""));
	char_t * pStaRouteCount = websGetVar(wp, T("StaRouteCount"), T("0"));
	int count=gatoi(pStaRouteCount);	
	int index=0;
	
	//printf("pStaRouteAction = %s, pStaRouteIdx=%s, pStaRouteCount=%s\n",pStaRouteAction,pStaRouteIdx,pStaRouteCount);	
	if (!gstrcmp(pStaRouteAction,"StaticRouteAdd"))	{
		static_route_edit_idx=-1;
		websRedirect(wp, T("advanced/advanced_staticroute_edit.asp"));
	} else if (!gstrcmp(pStaRouteAction,"StaticRouteEdit")) {
		static_route_edit_idx=gatoi(pStaRouteIdx);
		websRedirect(wp, T("advanced/advanced_staticroute_edit.asp"));
	} else if (!gstrcmp(pStaRouteAction,"StaticRouteDel"))	{
		char * pErrMsg = NULL;
		int status = -1;	

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("advanced/advanced_staticroute_edit.asp"));
		}
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from StaticRoute where _ROWID_ in (%s)",pStaRouteIdx);		
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
		status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);	
		if (pDB)
			sqlite3_close (pDB);
	
		if (status>=0){
			sqlite3_free (pErrMsg);
			system(EXPORT_CMD);
			system("/usr/local/bin/routing.sh");
		}				
		websRedirect(wp, T("advanced/advanced_staticroute.asp"));
	}	
}

static void setStaRoutAdd(webs_t wp, char_t *path, char_t *query)
{
	//printf("setStaRoutAdd\n");
    char_t * pEditMode = websGetVar(wp, T("editflag"), T(""));
    char * pErrMsg = NULL;
    int status = -1;

    char_t * pEnable = websGetVar(wp, T("starout_enable"), T("off"));
    char_t * pName = websGetVar(wp, T("starout_name"), T(""));
    char_t * pDestNet = websGetVar(wp, T("starout_dest_net"), T(""));
    char_t * pDestMask = websGetVar(wp, T("starout_dest_msk"), T(""));
    char_t * pLoacl = websGetVar(wp, T("starout_network"), T(""));
    char_t * pLoaclGw = websGetVar(wp, T("starout_lcl_gw"), T(""));
    //printf("pEnable:%s , pName:%s, pDestNet:%s, pDestMask:%s, pLoacl:%s, pLoaclGw:%s \n",pEnable,pName,pDestNet,pDestMask,pLoacl,pLoaclGw);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

    if (!gstrcmp(pEditMode,"add"))
    {
        snprintf(pPortQry,sizeof(pPortQry), "insert into StaticRoute values ('%s','%s','%s','%s','%s','%s')"
        ,gstrcmp(pEnable,"off")?"1":"0",pName,pDestNet,pDestMask,pLoacl,pLoaclGw);
        status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);
    }

	if (pDB)
    {
        sqlite3_close (pDB);
    }
	
    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
    }
	
    websRedirect(wp, T("advanced/advanced_staticroute.asp"));
}

static int getStaticRouteInfo(int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getStaticRouteInfo\n");
  if (static_route_edit_idx == -1){                   //Add
	  return websWrite(wp, T("\"%s\""),"");
  }else if (static_route_edit_idx > 0){	  //Edit  
  char * pName = jnrp_sql_getAttr_Webs("StaticRoute","Name",static_route_edit_idx);
  char * pDestnet = jnrp_sql_getAttr_Webs("StaticRoute","DestineNetwork",static_route_edit_idx);
  char * pDestmsk = jnrp_sql_getAttr_Webs("StaticRoute","DestineNetmask",static_route_edit_idx);
  char * pNetwork = jnrp_sql_getAttr_Webs("StaticRoute","Local",static_route_edit_idx);
  char * pLocalGateway = jnrp_sql_getAttr_Webs("StaticRoute","LocalGateway",static_route_edit_idx);
  int ret = 0;
  char* pStaticRouteItem = (char *)malloc(128);
  memset(pStaticRouteItem,'\0', 128);
  gstrcat(pStaticRouteItem, pName);
  gstrcat(pStaticRouteItem, pSep);
  gstrcat(pStaticRouteItem, pDestnet);
  gstrcat(pStaticRouteItem, pSep);
  gstrcat(pStaticRouteItem, pDestmsk);
  gstrcat(pStaticRouteItem, pSep);
  gstrcat(pStaticRouteItem, pNetwork);
  gstrcat(pStaticRouteItem, pSep);
  gstrcat(pStaticRouteItem, pLocalGateway);
  gstrcat(pStaticRouteItem, pSep);
  ret = websWrite(wp, T("\"%s\""), pStaticRouteItem);
  free(pStaticRouteItem);
  return ret;
}
}

static void setStaticRouteEdit(webs_t wp, char_t *path, char_t *query)
{
  printf("setStaticRouteEdit\n");
  char * pErrMsg = NULL;
  int status = -1;

  char_t * pEnable = websGetVar(wp, T("starout_enable"), T("off"));
  char_t * pName = websGetVar(wp, T("starout_name"), T(""));  
  char_t * pDestnet = websGetVar(wp, T("starout_dest_net"), T("")); 
  char_t * pDestmsk = websGetVar(wp, T("starout_dest_msk"), T(""));      
  char_t * pNetwork = websGetVar(wp, T("starout_network"), T("")); 
  char_t * pLocalGateway = websGetVar(wp, T("starout_lcl_gw"), T("")); 

  sqlite3 * pDB = NULL;
  if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
  {
      printf ("unable to open the DB file %s\n", pDBFile);
      return NULL;
  }
  sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
  
  char pStaticRouteQry [1024] = {0};
  memset (pStaticRouteQry, 0x00, sizeof(pStaticRouteQry));
  //printf("pName:%s , pDestnet:%s, pDestmsk:%s, pNetwork:%s, pLocalGateway:%s\n",pName,pDestnet,pDestmsk,pNetwork,pLocalGateway);
  //printf("static_route_edit_idx:%d\n",static_route_edit_idx);
  if (static_route_edit_idx == -1){          //Add
      snprintf(pStaticRouteQry,sizeof(pStaticRouteQry), "insert into StaticRoute values ('%s','%s','%s','%s','%s','%s')"
        ,"1",pName,pDestnet,pDestmsk,pNetwork,pLocalGateway);
      status = sqlite3_exec (pDB, pStaticRouteQry, NULL, NULL, &pErrMsg);
  }else	if (static_route_edit_idx > 0){      //Edit
      snprintf(pStaticRouteQry,sizeof(pStaticRouteQry), "update StaticRoute set Enable='%s', Name='%s', DestineNetwork='%s', DestineNetmask='%s', Local='%s', LocalGateway='%s' where _ROWID_='%d'"
          ,"1", pName, pDestnet, pDestmsk, pNetwork, pLocalGateway,static_route_edit_idx);
      status = sqlite3_exec (pDB, pStaticRouteQry, NULL, NULL, &pErrMsg);
  }
   
  if (pDB)
  {
      sqlite3_close (pDB);
  }

  // export db to ascii
  if (status>=0){
    system(EXPORT_CMD); 
  }
  websRedirect(wp, T("advanced/advanced_staticroute.asp"));
}

static int getStaticRouteName(int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getStaticRouteName\n");
  int ret = 0;
  char* pStaticRouteNameItem = (char *)malloc(128);
  memset(pStaticRouteNameItem,'\0', 128);

  int nRows=0, nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "select * from StaticRoute";
  
  int index = 0;
  char* pName = NULL;
  
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	free(pStaticRouteNameItem);
    return 0;
  }
  
  for (index = 0; index < nRows; index++) {
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Name", pName, getQosListOut);
	gstrcat(pStaticRouteNameItem, pName);
    gstrcat(pStaticRouteNameItem, pSep);
  }
  
  getQosListOut:
  if (ppQueryRes)
  {
   sqlite3_free_table(ppQueryRes);
  }

  ret = websWrite(wp, T("\"%s\""), pStaticRouteNameItem);
  free(pStaticRouteNameItem);
  return ret;
}

//--QoS Settings--//
static int getQosInfo(int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getQosInfo\n");
  char * pSessionLimit = jnrp_sql_getAttr_Webs("PeerToPeer","Sessions",1);
  char * pBWmanage = jnrp_sql_getAttr_Webs("QoS","Enable",1);
  char * pSessionLimitEnable = jnrp_sql_getAttr_Webs("PeerToPeer","Enable",1);

  //BandWidth Management
  char * pTotalUL = jnrp_sql_getAttr_Webs("QoS","UL_Total",1);
  char * pHighestUL = jnrp_sql_getAttr_Webs("QoS","UL_Highest",1);
  char * pHighUL = jnrp_sql_getAttr_Webs("QoS","UL_High",1);
  char * pNormalUL = jnrp_sql_getAttr_Webs("QoS","UL_Nornmal",1);
  char * pLowUL = jnrp_sql_getAttr_Webs("QoS","UL_Low",1);
  char * pTotalDL = jnrp_sql_getAttr_Webs("QoS","DL_Total",1);
  char * pHighestDL = jnrp_sql_getAttr_Webs("QoS","DL_Highest",1);
  char * pHighDL = jnrp_sql_getAttr_Webs("QoS","DL_High",1);
  char * pNormalDL = jnrp_sql_getAttr_Webs("QoS","DL_Nornmal",1);
  char * pLowDL = jnrp_sql_getAttr_Webs("QoS","DL_Low",1);

  //Priority Default
  char * pPriorityDefault = jnrp_sql_getAttr_Webs("QoS","Default_Priority",1);
  int ret = 0;
  char* pQosItem = (char *)malloc(128);
  memset(pQosItem,'\0', 128);
  gstrcat(pQosItem, pSessionLimit);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pBWmanage);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pSessionLimitEnable);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pTotalUL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pHighestUL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pHighUL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pNormalUL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pLowUL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pTotalDL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pHighestDL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pHighDL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pNormalDL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pLowDL);
  gstrcat(pQosItem, pSep);
  gstrcat(pQosItem, pPriorityDefault);
  gstrcat(pQosItem, pSep);
  ret = websWrite(wp, T("\"%s\""), pQosItem);
  free(pQosItem);
  return ret;
}

static void setSessionLimit(webs_t wp, char_t *path, char_t *query)
{
  	char * pErrMsg = NULL;
    int status = -1;
	int enable = 0;
    char_t * pSessions = websGetVar(wp, T("sessions"), T(""));  
	char_t * pSessionsEnable = websGetVar(wp, T("SessionLimitEnable"), T("off"));

	sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
    
    char pQoSenableQry [1024] = {0};
    memset (pQoSenableQry, 0x00, sizeof(pQoSenableQry));

	if (!gstrcmp(pSessionsEnable,"off") && pSessionsEnable){
        enable = 0;
    } else {
		enable = 1;
    }

    snprintf (pQoSenableQry, sizeof(pQoSenableQry),"update PeerToPeer set Sessions='%s', Enable='%d'",pSessions,enable);
    status = sqlite3_exec (pDB, pQoSenableQry, NULL, NULL, &pErrMsg);
    
    if (pDB)
    {
        sqlite3_close (pDB);
    }
  
    // export db to ascii
    if (status>=0){
	  setResultCode("Sessions Limit: Setting completed");
      system(EXPORT_CMD); 
      //system("/usr/local/bin/SessionLimit.sh");
    }
    websRedirect(wp, T("advanced/QoSPolicy.asp"));
}

static void setBandWidthManage(webs_t wp, char_t *path, char_t *query)
{
  	char_t * pBandWidthEnable = websGetVar(wp, T("BandWidthEnable"), T("off"));
	char_t * pThroughPut_UL = websGetVar(wp, T("QOS_THROUGHPUT_UL"), T(""));
	char_t * pThroughPut_DL = websGetVar(wp, T("QOS_THROUGHPUT_DL"), T(""));
	char_t * pHighest_UL = websGetVar(wp, T("QOS_HIGHEST_UL"), T(""));
	char_t * pHighest_DL = websGetVar(wp, T("QOS_HIGHEST_DL"), T(""));
	char_t * pHigh_UL = websGetVar(wp, T("QOS_HIGH_UL"), T(""));
	char_t * pHigh_DL = websGetVar(wp, T("QOS_HIGH_DL"), T(""));
	char_t * pNormal_UL = websGetVar(wp, T("QOS_NORMAL_UL"), T(""));
	char_t * pNormal_DL = websGetVar(wp, T("QOS_NORMAL_DL"), T(""));
	char_t * pLow_UL = websGetVar(wp, T("QOS_LOW_UL"), T(""));
	char_t * pLow_DL = websGetVar(wp, T("QOS_LOW_DL"), T(""));
	char_t * pPriority = websGetVar(wp, T("QOS_DEFAULT"), T(""));
	
    char * pErrMsg = NULL;
    int status = -1;

	char pQoSQry [1024] = {0};
    memset (pQoSQry, 0x00, sizeof(pQoSQry));
	
	printf("setBandWidthManage pBandWidthEnable='%s'\n",pBandWidthEnable);
    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    if (!gstrcmp(pBandWidthEnable,"off") && pBandWidthEnable){
        status = sqlite3_exec (pDB, "update QoS set Enable='0'", NULL, NULL, &pErrMsg);
    } else {
		status = sqlite3_exec (pDB, "update QoS set Enable='1'", NULL, NULL, &pErrMsg);
		snprintf (pQoSQry, sizeof(pQoSQry),"update QoS set DL_Total='%d', DL_Highest='%d', DL_High='%d', DL_Nornmal='%d', DL_Low='%d'"
			", UL_Total='%d', UL_Highest='%d', UL_High='%d', UL_Nornmal='%d', UL_Low='%d', Default_Priority='%d'"
			,gatoi(pThroughPut_DL),gatoi(pHighest_DL),gatoi(pHigh_DL),gatoi(pNormal_DL),gatoi(pLow_DL),gatoi(pThroughPut_UL),gatoi(pHighest_UL),gatoi(pHigh_UL),gatoi(pNormal_UL),gatoi(pLow_UL),gatoi(pPriority));
    	status = sqlite3_exec (pDB, pQoSQry, NULL, NULL, &pErrMsg);
    }

    if (pDB){
        sqlite3_close (pDB);
    }
	
    // export db to ascii
    if (status>=0){
        //sqlite3_free (pErrMsg);
        setResultCode("Bandwidth Management: Setting completed");
        system(EXPORT_CMD);
		system("/usr/local/bin/QosPolicy.sh; /usr/local/bin/QosRule.sh");
    }

    websRedirect(wp, T("advanced/QoSPolicy.asp"));
}

static int getQosBandwidthProfiles(int eid, webs_t wp, int argc, char_t **argv)
{
    int nRows=0, nCols=0;
    char** ppQueryRes = NULL;
    char * pQueryStr = "select * from QosRule";
   
    int index = 0;
	char* pEnable = NULL;
    char* pName = NULL;
	char* pContent = NULL;
    char* pPriority = NULL;

    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
      printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
      return 0;
    }
   
    for (index = 0; index < nRows; index++) {
	  DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Enable", pEnable, getQosListOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Name", pName, getQosListOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Content", pContent, getQosListOut);
      DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Priority", pPriority, getQosListOut);

	  if (!gstrcmp(pEnable,"1")){ 
        //printf("Enter getQosBandwidthProfiles:: Name=%s, Priority=%s, Content=%s\n", pName, pPriority, pContent);
        websWrite(wp, T("<tr bgcolor=\"#d9d9d9\">"));
        websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"qos.checkbox\" id=\"qos_ck_%d\"></td>\n"), index+1);
        websWrite(wp, T("<td class=\"tdOdd\"><span id=\"qos_name_%d\">%s</span></td>\n"), index+1, pName);
        websWrite(wp, T("<td class=\"tdOdd\"><span id=\"qos_content_%d\">%s</span></td>\n"), index+1, pContent);	
        
        if (!gstrcmp(pPriority,"1")){
        websWrite(wp, T("<td class=\"tdOdd\"><span id=\"qos_priority_%d\">%s</span></td>\n"), index+1, "Urgent");
        }else if (!gstrcmp(pPriority,"2")){
        	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"qos_priority_%d\">%s</span></td>\n"), index+1, "High");
        }else if (!gstrcmp(pPriority,"3")){
        	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"qos_priority_%d\">%s</span></td>\n"), index+1, "Medium");
        }else if (!gstrcmp(pPriority,"4")){
        	websWrite(wp, T("<td class=\"tdOdd\"><span id=\"qos_priority_%d\">%s</span></td>\n"), index+1, "Low");
        }
	  }	
    }
   
   getQosListOut:
    if (ppQueryRes)
    {
     sqlite3_free_table(ppQueryRes);
    }
   
    return 0;
}

static void setQoSTable(webs_t wp, char_t *path, char_t *query)
{
	char * pQosAction = websGetVar(wp, T("QosAction"), T(""));	
	char_t * pQosIdx = websGetVar(wp, T("QosIdx"), T(""));
	char_t * pQosCount = websGetVar(wp, T("QosCount"), T("0"));
	int count=gatoi(pQosCount);	
	int index=0;
	
	//printf("pQosAction = %s, pQosIdx=%s, pQosCount=%s\n",pQosAction,pQosIdx,pQosCount);	
	if (!gstrcmp(pQosAction,"QoSAdd"))	{
		qos_edit_idx=-1;
		websRedirect(wp, T("advanced/qos_policy_edit.asp"));
	} else if (!gstrcmp(pQosAction,"QoSEdit")) {
		qos_edit_idx=gatoi(pQosIdx);
		websRedirect(wp, T("advanced/qos_policy_edit.asp"));
	} else if (!gstrcmp(pQosAction,"QoSDel"))	{
		char * pErrMsg = NULL;
		int status = -1;	

		sqlite3 * pDB = NULL;
		if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
		{
			printf ("unable to open the DB file %s\n", pDBFile);
			websRedirect(wp, T("advanced/qos_policy_edit.asp"));
		}
		sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from QosRule where _ROWID_ in (%s)",pQosIdx);		
		status = sqlite3_exec (pDB, pSQLCmd, NULL, NULL, &pErrMsg);
		status = sqlite3_exec (pDB, "VACUUM;", NULL, NULL, &pErrMsg);	
		if (pDB)
			sqlite3_close (pDB);
	
		if (status>=0){
			setResultCode("Bandwidth Profiles: Delete completed");
			sqlite3_free (pErrMsg);
			system(EXPORT_CMD);
			system("/usr/local/bin/QosRule.sh");
		}				
		websRedirect(wp, T("advanced/QoSPolicy.asp"));
	}	
}

//----------Qos: Add page
static int getQosServiceOptions(int eid, webs_t wp, int argc, char_t **argv)
{
    int nRows=0, nCols=0, index=0;
    char * pQueryStr = "select * from Services";
	char** ppQueryRes = NULL;
    char* pServiceName = NULL;

    if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
      printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
      return 0;
    }
   
    if (nRows > 0) {
      for (index = 0; index < nRows; index++) {
        DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "ServiceName", pServiceName, getAppsListOut);
	    websWrite(wp, T("<option value=\"%s\">%s</option>\n"), pServiceName, pServiceName);
      }
 	}

getAppsListOut:
if (ppQueryRes) {
  sqlite3_free_table(ppQueryRes);
}

 return 0;
}

static void setNewServerAdd(webs_t wp, char_t *path, char_t *query)
{
	special_apps_edit_idx=-1;
	websRedirect(wp, T("advanced/special_apps_editor.asp"));
}

static void setQosAdd(webs_t wp, char_t *path, char_t *query)
{
	//printf("setQosAdd\n");
    char * pErrMsg = NULL;
    int status = -1;
	char script[1024];

	char_t * pName = websGetVar(wp, T("qosService"), T(""));
    char_t * pPriority = websGetVar(wp, T("qosAvailableProfile"), T(""));
    char_t * pRule = websGetVar(wp, T("qosTraffic"), T(""));
    char_t * pContentIP = websGetVar(wp, T("qosIP"), T(""));
	char_t * pContentDSCP = websGetVar(wp, T("qosDSCP"), T(""));
	char_t * pContent = (char *)malloc(128);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
		free(pContent);
        return;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pPortQry [1024] = {0};
    memset (pPortQry, 0x00, sizeof(pPortQry));

	if (!gstrcmp(pRule,"ip")){
	  strcpy(pContent,pContentIP);
  	} else if (!gstrcmp(pRule,"dscp")){
	  strcpy(pContent,pContentDSCP);
  	}

    snprintf(pPortQry,sizeof(pPortQry), "insert into QosRule values ('1','%s','%s','%s','dllink','%s')"
        ,pName,pPriority,pRule,pContent);
    status = sqlite3_exec (pDB, pPortQry, NULL, NULL, &pErrMsg);

	if (pDB)
    {
        sqlite3_close (pDB);
    }
	
    // export db to ascii
    if (status>=0)
    {
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
        system("QosRule.sh");
    }

	free(pContent);
	websRedirect(wp, T("advanced/QoSPolicy.asp"));
}

//----------Qos: Edit page
static int getQosEditInfo(int eid, webs_t wp, int argc, char_t **argv)
{
    if (qos_edit_idx == -1){          //Add
		return websWrite(wp, T("\"%s\""),"");
    }else if (qos_edit_idx > 0){	  //Edit	
  		//printf("getQosEditInfo\n");
  		char * pProfile = jnrp_sql_getAttr_Webs("QosRule","Priority",qos_edit_idx);
  		char * pName = jnrp_sql_getAttr_Webs("QosRule","Name",qos_edit_idx);
  		char * pTraffic = jnrp_sql_getAttr_Webs("QosRule","Rule",qos_edit_idx);
 		char * pContent = jnrp_sql_getAttr_Webs("QosRule","Content",qos_edit_idx);
		int ret = 0;
		char * pQosId = (char *)malloc(4);
		memset (pQosId, 0x00, sizeof(pQosId));
  		snprintf (pQosId, sizeof(pQosId),"%d",qos_edit_idx);

  		char* pQosEditItem = (char *)malloc(128);
  		memset(pQosEditItem,'\0', 128);
  		gstrcat(pQosEditItem, pProfile);
		gstrcat(pQosEditItem, pSep);
  		gstrcat(pQosEditItem, pName);
  		gstrcat(pQosEditItem, pSep);
  		gstrcat(pQosEditItem, pTraffic);
  		gstrcat(pQosEditItem, pSep);
  		gstrcat(pQosEditItem, pContent);
  		gstrcat(pQosEditItem, pSep);
  		gstrcat(pQosEditItem, pQosId);
  		gstrcat(pQosEditItem, pSep);

		ret = websWrite(wp, T("\"%s\""), pQosEditItem);
		free(pQosEditItem);
  		return ret;
    }
}

static void setQosEdit(webs_t wp, char_t *path, char_t *query)
{
    //printf("setQosEdit\n");
    char * pErrMsg = NULL;
    int status = -1;
	char script[1024];

	char_t * pName = websGetVar(wp, T("qosService"), T(""));
    char_t * pPriority = websGetVar(wp, T("qosAvailableProfile"), T(""));
    char_t * pRule = websGetVar(wp, T("qosTraffic"), T(""));
    char_t * pContentIP = websGetVar(wp, T("qosIP"), T(""));
	char_t * pContentDSCP = websGetVar(wp, T("qosDSCP"), T(""));
	char_t * pAction = websGetVar(wp, T("editflag"), T(""));
	char_t * pContent = (char *)malloc(128);

    sqlite3 * pDB = NULL;
    if (sqlite3_open (pDBFile, &pDB) != SQLITE_OK)
    {
        printf ("unable to open the DB file %s\n", pDBFile);
		free(pContent);
        return NULL;
    }
    sqlite3_busy_handler (pDB, sqliteBusyHandler, &bhWebArg);

    char pQoSRule [1024] = {0};
    memset (pQoSRule, 0x00, sizeof(pQoSRule));

if (!gstrcmp(pAction,"addService")){
		qos_edit_idx=-1;
		websRedirect(wp, T("advanced/special_apps_editor.asp"));
}else if (!gstrcmp(pAction,"apply")){	
	if (!gstrcmp(pRule,"IP")){
	  strcpy(pContent,pContentIP);
  	} else if (!gstrcmp(pRule,"DSCP")){
	  strcpy(pContent,pContentDSCP);
  	}
    if (qos_edit_idx == -1){          //Add
		snprintf(pQoSRule,sizeof(pQoSRule), "insert into QosRule values ('1','%s','%s','%s','dllink','%s')"
            ,pName,pPriority,pRule,pContent);
        status = sqlite3_exec (pDB, pQoSRule, NULL, NULL, &pErrMsg);
    }else if (qos_edit_idx > 0){	  //Edit
    	snprintf (pQoSRule, sizeof(pQoSRule),"update QosRule set Enable='1', Name='%s', Priority='%d', Rule='%s', Content='%s' where _ROWID_='%d'"
      		, pName, gatoi(pPriority), pRule, pContent, qos_edit_idx);
        status = sqlite3_exec (pDB, pQoSRule, NULL, NULL, &pErrMsg);
    }	

	if (pDB)
    {
        sqlite3_close (pDB);
    }
	
    // export db to ascii
    if (status>=0)
    {
    	setResultCode("Bandwidth Profiles: Setting completed");
        sqlite3_free (pErrMsg);
        system(EXPORT_CMD); // use export lib in future
	    system("QosRule.sh");
    }
	free(pContent);
	websRedirect(wp, T("advanced/QoSPolicy.asp"));
  }	
}

static int getServiceName(int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getServiceName\n");
  int ret = 0;
  char* pQosServiceNameItem = (char *)malloc(128);
  memset(pQosServiceNameItem,'\0', 128);

  int nRows=0, nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "select * from QosRule";
  
  int index = 0;
  char* pName = NULL;
  
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	free(pQosServiceNameItem);
    return 0;
  }
  
  for (index = 0; index < nRows; index++) {
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Name", pName, getQosListOut);
    gstrcat(pQosServiceNameItem, pName);
    gstrcat(pQosServiceNameItem, pSep);
  }
  
  getQosListOut:
  if (ppQueryRes)
  {
   sqlite3_free_table(ppQueryRes);
  }

  ret = websWrite(wp, T("\"%s\""), pQosServiceNameItem);
  free(pQosServiceNameItem);
  return ret;
}

static int getServiceTraffic(int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getServiceTraffic\n");
  int ret = 0;
  char* pQosServiceTrafficItem = (char *)malloc(128);
  memset(pQosServiceTrafficItem,'\0', 128);

  int nRows=0, nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "select * from QosRule";
  
  int index = 0;
  char* pRule = NULL;
  
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	free(pQosServiceTrafficItem);
    return 0;
  }
  
  for (index = 0; index < nRows; index++) {
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Rule", pRule, getQosListOut);
    gstrcat(pQosServiceTrafficItem, pRule);
    gstrcat(pQosServiceTrafficItem, pSep);
  }
  
  getQosListOut:
  if (ppQueryRes)
  {
   sqlite3_free_table(ppQueryRes);
  }

  ret = websWrite(wp, T("\"%s\""), pQosServiceTrafficItem);
  free(pQosServiceTrafficItem);
  return ret;
}

static int getServiceContent(int eid, webs_t wp, int argc, char_t **argv)
{
  //printf("getServiceContent\n");
  int ret = 0;
  char* pQosServiceContentItem = (char *)malloc(128);
  memset(pQosServiceContentItem,'\0', 128);

  int nRows=0, nCols=0;
  char** ppQueryRes = NULL;
  char * pQueryStr = "select * from QosRule";
  
  int index = 0;
  char* pRule = NULL;
  
  if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL){
    printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
	free(pQosServiceContentItem);
    return 0;
  }
  
  for (index = 0; index < nRows; index++) {
    DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "Content", pRule, getQosListOut);
    gstrcat(pQosServiceContentItem, pRule);
    gstrcat(pQosServiceContentItem, pSep);
  }
  
  getQosListOut:
  if (ppQueryRes)
  {
   sqlite3_free_table(ppQueryRes);
  }

  ret = websWrite(wp, T("\"%s\""), pQosServiceContentItem);
  free(pQosServiceContentItem);
  return ret;
}
//--QoS Settings--//

static int getVPNPT(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pPPTP = jnrp_sql_getAttr_Webs ("VpnConfig", "pt_pptp", 1);
	char * pL2TP = jnrp_sql_getAttr_Webs ("VpnConfig", "pt_l2tp", 1);
	char * pIPSEC = jnrp_sql_getAttr_Webs ("VpnConfig", "pt_ipsec", 1);	
	int ret = 0;
	char* pVpnPT = (char *)malloc(128);
	memset(pVpnPT,'\0', 128);
	gstrcat(pVpnPT, pPPTP);
	gstrcat(pVpnPT, pSep);
	gstrcat(pVpnPT, pL2TP);
	gstrcat(pVpnPT, pSep);
	gstrcat(pVpnPT, pIPSEC);
	ret = websWrite(wp, T("\"%s\""), pVpnPT);
	free(pVpnPT);
	return ret;
}

static void setVPNPT(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pPPTP = websGetVar(wp, T("VPNP_PPTP"), T("off"));
	char * pL2TP = websGetVar(wp, T("VPNP_L2TP"), T("off"));
	char * pIPSEC = websGetVar(wp, T("VPNP_IPSEC"), T("off"));
	int pptp=0,l2tp=0,ipsec=0;
	pptp = gstrcmp(pPPTP,"off") ? 1:0;
	l2tp = gstrcmp(pL2TP,"off") ? 1:0;
	ipsec = gstrcmp(pIPSEC,"off") ? 1:0;
	
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update VpnConfig set pt_pptp=%d,pt_l2tp=%d,pt_ipsec=%d where _ROWID_='1'", pptp,l2tp,ipsec);			
	if (execSQLCmd(pSQLCmd)>=0)
		websRedirect(wp, T("advanced/vpn_passthrough.asp"));	 		
}

static int getVpnProfile(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pMode = jnrp_sql_getAttr_Webs ("VpnConfig", "vpn_mode", 1);
	char * pIPSecMode = jnrp_sql_getAttr_Webs ("VpnConfig", "vpn_ipsec_mode", 1);
	return websWrite(wp, T("\"%s#%s\""),pMode,pIPSecMode); 								
}

/***************************************************
 * write current setting to /sysconfig/vpn-cfg
 * generate /sysconf/vpn-cfg	
 ****************************************************/		
int generateVpnCfg(char *pVpnMode, char *pIPSecName)
{	
	//char * pVpnMode = jnrp_sql_getAttr_Webs ("VpnConfig", "vpn_mode", 1);
	//char * pIPSecName = jnrp_sql_getAttr_Webs ("VpnConfig", "vpn_ipsec_mode", 1);
	char line[128];
	memset(line, '\0', sizeof(line));	
	FILE *out = fopen("/sysconfig/.vpn-cfg", "w+");
	if (out)
	{	 
		memset(line, '\0', sizeof(line));	
		sprintf(line,"VPN_MODE=%s\n",pVpnMode); 			
		fwrite(line, strlen(line), 1, out); 			

		if (!gstrcmp(pVpnMode,"pptp_server"))
		{			
			char * remote_ip_range = jnrp_sql_getAttr_Webs ("VpnConfig", "vpns_pptp_remote_ip_range", 1);
			char * username = NULL;
			char * password = NULL;
			int index = 0;			
			int userCount = jnrp_sql_getRowNum_Webs("VpnPPTPUserAccount");			
			if (userCount>0)
			{
			  for (index=1; index<=userCount; index++)
			  {
					username = jnrp_sql_getAttr_Webs("VpnPPTPUserAccount", "username", index);
					password = jnrp_sql_getAttr_Webs("VpnPPTPUserAccount", "password", index);
			
					//username
					memset(line, '\0', sizeof(line));
					sprintf(line, "VPNS_PPTP_USERNAME_%d=%s\n",(index-1),username);			  
					fwrite(line, strlen(line), 1, out);   
					
					//password
					memset(line, '\0', sizeof(line));
					sprintf(line, "VPNS_PPTP_PW_%d=%s\n",(index-1),password);
					fwrite(line, strlen(line), 1, out); 
			  }
			}
			else
			{
				setResultCode("Cannot active pptp server. Please add pptp user account.");
				fclose(out);				
				return -1;
			}
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_PPTP_REMOTE_IP_RANGE=%s\n",remote_ip_range); 			
			fwrite(line, strlen(line), 1, out); 						
		}
		else if (!gstrcmp(pVpnMode,"pptp_client"))
		{
			char * username = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_pptp_username", 1);
			char * password = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_pptp_password", 1);
			char * server = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_pptp_server", 1);			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_PPTP_USERNAME=%s\n",username); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_PPTP_PW=%s\n",password); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_PPTP_SERVER=%s\n",server); 			
			fwrite(line, strlen(line), 1, out); 			
		}
		else if (!gstrcmp(pVpnMode,"l2tp_server"))
		{
			char * remote_ip_range = jnrp_sql_getAttr_Webs ("VpnConfig", "vpns_l2tp_remote_ip_range", 1);	
			char * server_mode = jnrp_sql_getAttr_Webs ("VpnConfig", "vpns_l2tp_mode", 1);
			char * username = NULL;
			char * password = NULL;
			int index = 0;			
			int userCount = jnrp_sql_getRowNum_Webs("VpnL2TPUserAccount");			
			if (userCount>0)
			{
			  for (index=1; index<=userCount; index++)
			  {
					username = jnrp_sql_getAttr_Webs("VpnL2TPUserAccount", "username", index);
					password = jnrp_sql_getAttr_Webs("VpnL2TPUserAccount", "password", index);
			
					//username
					memset(line, '\0', sizeof(line));
					sprintf(line, "VPNS_L2TP_USERNAME_%d=%s\n",(index-1),username);			  
					fwrite(line, strlen(line), 1, out);   
					
					//password
					memset(line, '\0', sizeof(line));
					sprintf(line, "VPNS_L2TP_PW_%d=%s\n",(index-1),password);
					fwrite(line, strlen(line), 1, out); 
			  }
			}
			else
			{
				setResultCode("Cannot active l2tp server. Please add l2tp user account.");				
				fclose(out);
				return -1;
			}			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_L2TP_REMOTE_IP_RANGE=%s\n",remote_ip_range); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNS_L2TP_MODE=%s\n",server_mode); 			
			fwrite(line, strlen(line), 1, out);		
		}
		else if (!gstrcmp(pVpnMode,"l2tp_client"))
		{
			char * username = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_username", 1);
			char * password = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_password", 1);
			char * server = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_server", 1); 
			char * mode = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_mode", 1);

			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_USERNAME=%s\n",username); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_PW=%s\n",password); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_SERVER=%s\n",server); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"VPNC_L2TP_MODE=%s\n",mode); 			
			fwrite(line, strlen(line), 1, out); 			
		}
		else if (!gstrcmp(pVpnMode,"ipsec"))
		{
			char * nat_traversal = jnrp_sql_getAttr_Webs ("VpnConfig", "nat_traversal", 1);
			char * keep_alive = jnrp_sql_getAttr_Webs ("VpnConfig", "keep_alive", 1);	
			
			int nRows=0, nCols=0;
			char** ppQueryRes = NULL;
			char * pQueryStr = "select * from VpnIPSec where name='%s'";
			pQueryStr = sqlite3_mprintf (pQueryStr, pIPSecName);
			char * name = NULL;
			char * remote_security_gateway = NULL;
			char * remote_network_ipaddr = NULL;
			char * remote_network_netmask = NULL;
			char * preshared_key = 	NULL;
			char * mode = NULL;
			char * phase1_encryption_algorithm = NULL;
			char * phase1_integrity_algorithm = NULL;
			char * phase1_dh_group = NULL;
			char * sa_lifetime = NULL;
			char * phase2_encryption_algorithm = NULL;
			char * phase2_integrity_algorithm = NULL;
			char * phase2_dh_group = NULL;
			char * ipsec_lifetime = NULL;
			
			if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
			{
				printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
				return 0;
			}
			#ifdef DEBUG_MSG
			printf("nRows=%d,pIPSecName=%s\n",nRows,pIPSecName);
			#endif			
			if (nRows > 0) {
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "name", name, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "remote_security_gateway", remote_security_gateway, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "remote_network_ipaddr", remote_network_ipaddr, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "remote_network_netmask", remote_network_netmask, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "preshared_key", preshared_key, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "mode", mode, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase1_encryption_algorithm", phase1_encryption_algorithm, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase1_integrity_algorithm", phase1_integrity_algorithm, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase1_dh_group", phase1_dh_group, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "sa_lifetime", sa_lifetime, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase2_encryption_algorithm", phase2_encryption_algorithm, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase2_integrity_algorithm", phase2_integrity_algorithm, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase2_dh_group", phase2_dh_group, getVpnIPSecProfileOut);
				DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "ipsec_lifetime", ipsec_lifetime, getVpnIPSecProfileOut);			
			}
			
		getVpnIPSecProfileOut:
			if (ppQueryRes)
			{
				sqlite3_free_table(ppQueryRes);
			}	

			memset(line, '\0', sizeof(line));	
			sprintf(line,"NAT_TRAVERSAL=%s\n",nat_traversal); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"KEEP_ALIVE=%s\n",keep_alive); 			
			fwrite(line, strlen(line), 1, out); 	

			memset(line, '\0', sizeof(line));	
			sprintf(line,"CONNECT_NAME=%s\n",name); 			
			fwrite(line, strlen(line), 1, out); 			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"REMOTE_GATEWAY_IP=%s\n",remote_security_gateway); 			
			fwrite(line, strlen(line), 1, out); 				

			memset(line, '\0', sizeof(line));	
			sprintf(line,"REMOTE_NETWORK_SUBNET=%s/%s\n",remote_network_ipaddr, remote_network_netmask);
			fwrite(line, strlen(line), 1, out); 			
			

			memset(line, '\0', sizeof(line));	
			sprintf(line,"PRESHARED_KEY=%s\n",preshared_key);			
			fwrite(line, strlen(line), 1, out); 	
			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IPSEC_CONNECT_MODE=%s\n",mode);			
			fwrite(line, strlen(line), 1, out); 			
			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"PHASE1_ENCRYPTION_ALGORITHM=%s-%s\n",phase1_encryption_algorithm,phase1_integrity_algorithm); 			
			fwrite(line, strlen(line), 1, out); 					
			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"PHASE1_DH_GROUP=%s\n",phase1_dh_group); 			
			fwrite(line, strlen(line), 1, out); 
			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"SA_LIFETIME=%s\n",sa_lifetime);			
			fwrite(line, strlen(line), 1, out); 	
			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"PHASE2_ENCRYPTION_ALGORITHM=%s-%s\n",phase2_encryption_algorithm,phase2_integrity_algorithm);			
			fwrite(line, strlen(line), 1, out); 			
			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"PHASE2_DH_GROUP=%s\n",phase2_dh_group);			
			fwrite(line, strlen(line), 1, out); 			
			
			memset(line, '\0', sizeof(line));	
			sprintf(line,"IPSEC_LIFETIME=%s\n",ipsec_lifetime); 			
			fwrite(line, strlen(line), 1, out); 					
		}
		else //disable
		{
		}
	}	
	fclose(out);
	system("mv /sysconfig/.vpn-cfg /sysconfig/vpn-cfg");
	return 0;
}

int checkWanIP()
{
	FILE *fd = -1;
	char cmdbuffer[100] = {0};	
	char wanip[100] = {0};
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));	
	memset(wanip, '\0', sizeof(wanip));
		
	sprintf(cmdbuffer,"ip -4 addr show dev usb0| grep inet| awk '{print $2}'|cut -d'/' -f 1");
	fd = popen(cmdbuffer,"r");		
	if(fgets(wanip,sizeof(wanip),fd) == NULL)
	{	  
	}
	else
	{
		if (wanip[strlen(wanip)-1] == '\n')
		 wanip[strlen(wanip)-1] = '\0';					
	}
	pclose(fd);	
	//printf("wanip=%s\n",wanip);
	if (!gstrcmp(wanip,""))
	{
		return -1;
	}
	return 0;
}

static void setVpnProfile(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pVpnMode = websGetVar(wp, T("profile"), T("disable"));
	char * pIPSecName = websGetVar(wp, T("ipsec_profile"), T("disable"));
	
	/*howard add*/
	int pid = -1;
	pid = get_pid("vpn-mgr.pid");
	if(pid > 0)
	  kill_process_by_pid(pid);
	/*howard add end*/	

	if(checkWanIP()<0)
	{		
		setResultCode("Cannot active profile. There is no ip address.");	
		goto setVpnProfileOut;
	}	
	if(generateVpnCfg(pVpnMode,pIPSecName)<0)
	{
		goto setVpnProfileOut;;
	}
	
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update VpnConfig set vpn_mode='%s',vpn_ipsec_mode='%s' where _ROWID_='1'", pVpnMode,pIPSecName); 		
	execSQLCmd(pSQLCmd);			

setVpnProfileOut:	
	/*howard add*/
	if(strcmp(pVpnMode, "disable"))	
	  system("vpn-mgr &"); 
	/*howard add end*/	
	websRedirect(wp, T("advanced/vpn_profile.asp"));	
}

static int getVpnIPSecProfile(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from VpnIPSec";
	int index = 0;
	char* name = NULL;
	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "name", name, getVpnIPSecProfileOut);
			websWrite(wp, T("<option value=\"%s\">%s</option>\n"), name, name);
		}
	}
	
getVpnIPSecProfileOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;		
}

static int getVpnPPTP(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif	
	char * remote_ip_range = jnrp_sql_getAttr_Webs ("VpnConfig", "vpns_pptp_remote_ip_range", 1);
	char * username = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_pptp_username", 1);
	char * password = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_pptp_password", 1);
	char * server = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_pptp_server", 1);	
	int ret = 0;
	char* pVpnConfig = (char *)malloc(128);
	memset(pVpnConfig,'\0', 128);
	gstrcat(pVpnConfig, remote_ip_range);
	gstrcat(pVpnConfig, pSep);
	gstrcat(pVpnConfig, username);
	gstrcat(pVpnConfig, pSep);
	gstrcat(pVpnConfig, password);
	gstrcat(pVpnConfig, pSep);
	gstrcat(pVpnConfig, server);
	ret = websWrite(wp, T("\"%s\""), pVpnConfig);
	free(pVpnConfig);
	return ret;							
}

static void setVpnServerPPTP(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	char_t *IP_START = websGetVar(wp, T("VPNS_PPTP_REMOTE_IP_START"), T(""));
	char_t *IP_END = websGetVar(wp, T("VPNS_PPTP_REMOTE_IP_END"), T(""));
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd),
		"update VpnConfig set vpns_pptp_remote_ip_range='%s-%s' where _ROWID_='1'", 
		IP_START, IP_END); 		
	if (execSQLCmd(pSQLCmd)>=0)
		websRedirect(wp, T("advanced/vpn_pptp.asp"));	 		
}

static void setVpnClientPPTP(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char_t *VPNC_PPTP_USERNAME, *VPNC_PPTP_PW, *VPNC_PPTP_SERVER;
	VPNC_PPTP_USERNAME = websGetVar(wp, T("VPNC_PPTP_USERNAME"), T(""));
	VPNC_PPTP_PW = websGetVar(wp, T("VPNC_PPTP_PW"), T("p"));
	VPNC_PPTP_SERVER = websGetVar(wp, T("VPNC_PPTP_SERVER"), T(""));

	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd),
		"update VpnConfig set vpnc_pptp_username='%s',vpnc_pptp_password='%s',vpnc_pptp_server='%s' where _ROWID_='1'", 
		VPNC_PPTP_USERNAME,VPNC_PPTP_PW,VPNC_PPTP_SERVER); 		
	if (execSQLCmd(pSQLCmd)>=0)
		websRedirect(wp, T("advanced/vpn_pptp.asp"));	 		
}

static int getVpnL2TP(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	char * remote_ip_range = jnrp_sql_getAttr_Webs ("VpnConfig", "vpns_l2tp_remote_ip_range", 1);	
	char * server_mode = jnrp_sql_getAttr_Webs ("VpnConfig", "vpns_l2tp_mode", 1);
	char * username = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_username", 1);
	char * password = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_password", 1);
	char * server = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_server", 1);	
	char * mode = jnrp_sql_getAttr_Webs ("VpnConfig", "vpnc_l2tp_mode", 1);
	int ret = 0;
	char* pVpnConfig = (char *)malloc(128);
	memset(pVpnConfig,'\0', 128);
	gstrcat(pVpnConfig, remote_ip_range);
	gstrcat(pVpnConfig, pSep);		
	gstrcat(pVpnConfig, server_mode);
	gstrcat(pVpnConfig, pSep);	
	gstrcat(pVpnConfig, username);
	gstrcat(pVpnConfig, pSep);
	gstrcat(pVpnConfig, password);
	gstrcat(pVpnConfig, pSep);
	gstrcat(pVpnConfig, server);	
	gstrcat(pVpnConfig, pSep);
	gstrcat(pVpnConfig, mode);
	ret = websWrite(wp, T("\"%s\""), pVpnConfig);
	free(pVpnConfig);
	return ret;	
}

static void setVpnServerL2TP(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char_t *IP_START = websGetVar(wp, T("VPNS_L2TP_REMOTE_IP_START"), T(""));
	char_t *IP_END = websGetVar(wp, T("VPNS_L2TP_REMOTE_IP_END"), T(""));	
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd),
		"update VpnConfig set vpns_l2tp_remote_ip_range='%s-%s' where _ROWID_='1'", 
		IP_START,IP_END); 		
	if (execSQLCmd(pSQLCmd)>=0)
		websRedirect(wp, T("advanced/vpn_l2tp.asp"));	 		
}

static void setVpnClientL2TP(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char_t *VPNC_L2TP_USERNAME, *VPNC_L2TP_PW, *VPNC_L2TP_SERVER, *VPNC_L2TP_MODE;	
	VPNC_L2TP_USERNAME = websGetVar(wp, T("VPNC_L2TP_USERNAME"), T(""));
	VPNC_L2TP_PW = websGetVar(wp, T("VPNC_L2TP_PW"), T(""));	
	VPNC_L2TP_SERVER = websGetVar(wp, T("VPNC_L2TP_SERVER"), T(""));
	VPNC_L2TP_MODE = websGetVar(wp, T("VPNC_L2TP_MODE"), T(""));
	char pSQLCmd [256] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), 
		"update VpnConfig set vpnc_l2tp_username='%s',vpnc_l2tp_password='%s',vpnc_l2tp_server='%s',vpnc_l2tp_mode='%s' where _ROWID_='1'", 
		VPNC_L2TP_USERNAME,VPNC_L2TP_PW,VPNC_L2TP_SERVER,VPNC_L2TP_MODE); 		
	if (execSQLCmd(pSQLCmd)>=0)
		websRedirect(wp, T("advanced/vpn_l2tp.asp"));	 			
}


static int getVpnUserTbl(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr;
	if (argc==1 && argv[0])
	{
		if (!gstrcmp(argv[0],"pptp"))
			
			pQueryStr = "select * from VpnPPTPUserAccount";
		else
			pQueryStr = "select * from VpnL2TPUserAccount";
	}
	else
		return 0;
	
	int index = 0;
	char* pUserName = NULL;
	char* pPassword = NULL;
	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "username", pUserName, getVpnUserAccountOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "password", pPassword, getVpnUserAccountOut);
			
			websWrite(wp, T("<tr>\n"));				
			websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"vpn.checkbox\"></td>\n"));
			websWrite(wp, T("<td class=\"tdOdd\" id=\"user_name_%d\">%s</td>\n"), (index+1),pUserName);		
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), pPassword);						 
			websWrite(wp, T("</tr>\n"));
		}
	}
	
getVpnUserAccountOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;									
}

static void setVpnUserTbl(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	char_t * pVpnType = websGetVar(wp, T("VpnType"), T(""));	
	char_t * pVpnTblAction = websGetVar(wp, T("VpnTblAction"), T(""));	
	char_t * pRowId = websGetVar(wp, T("VpnTblIdx"), T(""));

	#ifdef DEBUG_MSG
	printf("pVpnType=%s, pVpnTblAction=%s, pRowId=%s\n",pVpnType,pVpnTblAction,pRowId); 
	#endif
	if (!gstrcmp(pVpnType,"pptp"))
		vpn_edit_type=vpn_pptp;
	else if (!gstrcmp(pVpnType,"l2tp"))
		vpn_edit_type=vpn_l2tp;
	
	if (!gstrcmp(pVpnTblAction,"VpnTblAdd"))
	{
		vpn_tbl_idx=-1;
		websRedirect(wp, T("advanced/vpn_user_account.asp"));
	}
	else if (!gstrcmp(pVpnTblAction,"VpnTblEdit"))
	{
		vpn_tbl_idx=gatoi(pRowId);
		websRedirect(wp, T("advanced/vpn_user_account.asp"));
	}
	else if (!gstrcmp(pVpnTblAction,"VpnTblDel"))
	{
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		if (!gstrcmp(pVpnType,"pptp"))
			snprintf(pSQLCmd,sizeof(pSQLCmd),"delete from VpnPPTPUserAccount where _ROWID_ in (%s)",pRowId);		
		else
			snprintf(pSQLCmd,sizeof(pSQLCmd),"delete from VpnL2TPUserAccount where _ROWID_ in (%s)",pRowId);					
		execSQLCmd(pSQLCmd);
		if (!gstrcmp(pVpnType,"pptp"))
			websRedirect(wp, T("advanced/vpn_pptp.asp"));
		else
			websRedirect(wp, T("advanced/vpn_l2tp.asp"));
	}			
}

static int getVpnUser(int eid, webs_t wp, int argc, char_t **argv)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	int ret = 0;
	char * pTableName = (vpn_edit_type==vpn_pptp) ? "VpnPPTPUserAccount" : "VpnL2TPUserAccount";
	char * pVpnTblItem = (char *)malloc(256);
	memset(pVpnTblItem,'\0', 256);

	gstrcat(pVpnTblItem, pTableName);
	gstrcat(pVpnTblItem, pSep);	
	if (vpn_tbl_idx==-1) //add
	{
	}
	else if (vpn_tbl_idx>0) //edit
	{
		char * pUserName = jnrp_sql_getAttr_Webs(pTableName,"username",vpn_tbl_idx);
		char * pPassword = jnrp_sql_getAttr_Webs(pTableName,"password",vpn_tbl_idx);		
		gstrcat(pVpnTblItem, pUserName);
		gstrcat(pVpnTblItem, pSep);
		gstrcat(pVpnTblItem, pPassword);		
	}
	ret = websWrite(wp, T("\"%s\""), pVpnTblItem);
	free(pVpnTblItem);
	return ret;
}	

static void setVpnUser(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * pUserName = websGetVar(wp, T("username"), T(""));
	char * pPassword = websGetVar(wp, T("password"), T(""));
	char * pTableName = (vpn_edit_type==vpn_pptp) ? "VpnPPTPUserAccount" : "VpnL2TPUserAccount";

	char pSQLCmd [128] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));
	if (vpn_tbl_idx==-1) //add
	{
		snprintf(pSQLCmd,sizeof(pSQLCmd), "insert into %s values ('%s','%s')", pTableName, pUserName, pPassword);
	}
	else if (vpn_tbl_idx>0) //edit
	{
		snprintf(pSQLCmd,sizeof(pSQLCmd), "update %s set username='%s', password='%s' where _ROWID_=%d", pTableName, pUserName, pPassword, vpn_tbl_idx);
	}		
	execSQLCmd(pSQLCmd);	
	if (vpn_edit_type==vpn_pptp)
		websRedirect(wp, T("advanced/vpn_pptp.asp"));	
	else
		websRedirect(wp, T("advanced/vpn_l2tp.asp"));	
}

static int getVpnNAT(int eid, webs_t wp, int argc, char_t **argv)
{
	char * nat_traversal = jnrp_sql_getAttr_Webs ("VpnConfig", "nat_traversal", 1);
	char * keep_alive = jnrp_sql_getAttr_Webs ("VpnConfig", "keep_alive", 1);	
	return websWrite(wp, T("\"%s#%s\""),nat_traversal,keep_alive);
}	

static void setVpnNAT(webs_t wp, char_t *path, char_t *query)
{
	char * nat_traversal = websGetVar(wp, T("nat_traversal"), T("off"));
	char * keep_alive = websGetVar(wp, T("keep_alive"), T(""));

	char pSQLCmd [128] = {0};
	memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
	snprintf(pSQLCmd,sizeof(pSQLCmd), "update VpnConfig set nat_traversal='%s',keep_alive='%s' where _ROWID_='1'", nat_traversal, keep_alive);			
	execSQLCmd(pSQLCmd);
	websRedirect(wp, T("advanced/vpn_ipsec.asp"));
}

static int getVpnIPSecTable(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	int nRows=0, nCols=0;
	char** ppQueryRes = NULL;
	char * pQueryStr = "select * from VpnIPSec";
	int index = 0;
	char* name = NULL;
	char* remote_network_ipaddr = NULL;
	char* remote_network_netmask = NULL;
	char* remote_security_gateway = NULL;

	FILE *fd = -1;
	char cmdbuffer[100] = {0};
	char local_network[100] = {0};
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));	
	sprintf(cmdbuffer,"ip route show dev br0 | awk '{print $1}'");
	#ifdef DEBUG_MSG
	printf("cmdbuffer=%s\n",cmdbuffer);
	#endif
	fd = popen(cmdbuffer,"r");	
	if(fgets(local_network,sizeof(local_network),fd) != NULL)
	{	
		if (local_network[strlen(local_network)-1] == '\n')
		 local_network[strlen(local_network)-1] = '\0';			
	}
	pclose(fd);	
	//printf("local_network = %s\n", local_network);
	
	if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
	{
		printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
		return 0;
	}
	if (nRows > 0) {
		for (index = 0; index < nRows; index++)
		{
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "name", name, getVpnIPSecTableOut);
			/*
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "local_network_ipaddr", local_network_ipaddr, getVpnIPSecTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "local_network_netmask", local_network_netmask, getVpnIPSecTableOut);			
			*/
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "remote_network_ipaddr", remote_network_ipaddr, getVpnIPSecTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "remote_network_netmask", remote_network_netmask, getVpnIPSecTableOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, index, "remote_security_gateway", remote_security_gateway, getVpnIPSecTableOut);			
			
			websWrite(wp, T("<tr>\n"));			 
			websWrite(wp, T("<td class=\"tdOdd\"><input type=\"checkbox\" name=\"vpn.checkbox\" id=\"IPSecTbl_id_%d\"></td>\n"), index+1);
			websWrite(wp, T("<td class=\"tdOdd\" id=\"ipsec_name_%d\">%s</td>\n"), (index+1), name);	 
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), local_network);	
			websWrite(wp, T("<td class=\"tdOdd\">%s/%s</td>\n"), remote_network_ipaddr,remote_network_netmask);		
			websWrite(wp, T("<td class=\"tdOdd\">%s</td>\n"), remote_security_gateway);					
			websWrite(wp, T("</tr>\n"));
		}
	}
	
getVpnIPSecTableOut:
	if (ppQueryRes)
	{
		sqlite3_free_table(ppQueryRes);
	}
	return 0;		
}

static void setVpnIPSecTable(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif
	char * pIPSecTblAction = websGetVar(wp, T("IPSecTblAction"), T(""));	
	char_t * pRowId = websGetVar(wp, T("IPSecTblIdx"), T(""));

	#ifdef DEBUG_MSG
	printf("pIPSecTblAction = %s, pRowId=%s\n",pIPSecTblAction,pRowId);	
	#endif
	if (!gstrcmp(pIPSecTblAction,"IPSecTblAdd"))
	{
		vpn_tbl_idx=-1;
		websRedirect(wp, T("advanced/vpn_ipsec_editor.asp"));
	}
	else if (!gstrcmp(pIPSecTblAction,"IPSecTblEdit"))
	{
		vpn_tbl_idx=gatoi(pRowId);
		websRedirect(wp, T("advanced/vpn_ipsec_editor.asp"));
	}
	else if (!gstrcmp(pIPSecTblAction,"IPSecTblDel"))
	{
		char pSQLCmd [256] = {0};
		memset (pSQLCmd, 0x00, sizeof(pSQLCmd));		
		snprintf(pSQLCmd,sizeof(pSQLCmd), "delete from VpnIPSec where _ROWID_ in (%s)",pRowId);				
		execSQLCmd(pSQLCmd);
		websRedirect(wp, T("advanced/vpn_ipsec.asp"));
	}			
}

static int getVpnIPSec(int eid, webs_t wp, int argc, char_t **argv)
{	
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);
	#endif

	if (vpn_tbl_idx==-1) //add
	{
		websWrite(wp, T("\"%s\""),"");
	}
	else if (vpn_tbl_idx>0) //edit
	{	
		int nRows=0, nCols=0;
		char** ppQueryRes = NULL;
		char * pQueryStr = "select * from VpnIPSec where _ROWID_=%d";
		pQueryStr = sqlite3_mprintf (pQueryStr, vpn_tbl_idx);

		char * name = NULL;
		char * remote_security_gateway = NULL;
		char * remote_network_ipaddr = NULL;
		char * remote_network_netmask = NULL;
		char * preshared_key = 	NULL;
		char * mode = NULL;
		char * phase1_encryption_algorithm = NULL;
		char * phase1_integrity_algorithm = NULL;
		char * phase1_dh_group = NULL;
		char * sa_lifetime = NULL;
		char * phase2_encryption_algorithm = NULL;
		char * phase2_integrity_algorithm = NULL;
		char * phase2_dh_group = NULL;
		char * ipsec_lifetime = NULL;
		
		if ((ppQueryRes = sqlite3QueryResGet_Webs (pQueryStr, &nCols, &nRows)) == NULL)
		{
			printf("sqlite3QueryResGet failed: query='%s'\n",pQueryStr);
			return 0;
		}
		#ifdef DEBUG_MSG
		printf("nRows=%d,vpn_tbl_idx=%d\n",nRows,vpn_tbl_idx);
		#endif
		if (nRows > 0) {
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "name", name, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "remote_security_gateway", remote_security_gateway, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "remote_network_ipaddr", remote_network_ipaddr, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "remote_network_netmask", remote_network_netmask, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "preshared_key", preshared_key, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "mode", mode, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase1_encryption_algorithm", phase1_encryption_algorithm, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase1_integrity_algorithm", phase1_integrity_algorithm, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase1_dh_group", phase1_dh_group, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "sa_lifetime", sa_lifetime, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase2_encryption_algorithm", phase2_encryption_algorithm, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase2_integrity_algorithm", phase2_integrity_algorithm, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "phase2_dh_group", phase2_dh_group, getVpnIPSecOut);
			DB_TABLE_VALUE_GET(ppQueryRes, nCols, 0, "ipsec_lifetime", ipsec_lifetime, getVpnIPSecOut);
				
			websWrite(wp, T("%sdocument.getElementById('name').value='%s';\n"),"    ",name);
			websWrite(wp, T("%sdocument.getElementById('remote_security_gateway').value='%s';\n"),"    ",remote_security_gateway);
			websWrite(wp, T("%sdocument.getElementById('remote_network_ipaddr').value='%s';\n"),"    ",remote_network_ipaddr);
			websWrite(wp, T("%sdocument.getElementById('remote_network_netmask').value='%s';\n"),"    ",remote_network_netmask);
			websWrite(wp, T("%sdocument.getElementById('preshared_key').value='%s';\n"),"    ",preshared_key);
			websWrite(wp, T("%sdocument.getElementById('mode').value='%s';\n"),"    ",mode);
			websWrite(wp, T("%sdocument.getElementById('phase1_encryption_algorithm').value='%s';\n"),"    ",phase1_encryption_algorithm);
			websWrite(wp, T("%sdocument.getElementById('phase1_integrity_algorithm').value='%s';\n"),"    ",phase1_integrity_algorithm);
			websWrite(wp, T("%sdocument.getElementById('phase1_dh_group').value='%s';\n"),"    ",phase1_dh_group);
			websWrite(wp, T("%sdocument.getElementById('sa_lifetime').value='%s';\n"),"    ",sa_lifetime);
			websWrite(wp, T("%sdocument.getElementById('phase2_encryption_algorithm').value='%s';\n"),"    ",phase2_encryption_algorithm);
			websWrite(wp, T("%sdocument.getElementById('phase2_integrity_algorithm').value='%s';\n"),"    ",phase2_integrity_algorithm);
			websWrite(wp, T("%sdocument.getElementById('phase2_dh_group').value='%s';\n"),"    ",phase2_dh_group);
			websWrite(wp, T("%sdocument.getElementById('ipsec_lifetime').value='%s';\n"),"    ",ipsec_lifetime);
		}
		
	getVpnIPSecOut:
		if (ppQueryRes)
		{
			sqlite3_free_table(ppQueryRes);
		}
	}
	return 0;	
}

static void setVpnIPSec(webs_t wp, char_t *path, char_t *query)
{
	#ifdef DEBUG_MSG
	printf("%s\n", __func__);	
	#endif
	char * name = websGetVar(wp, T("name"), T(""));
	char * remote_security_gateway = websGetVar(wp, T("remote_security_gateway"), T(""));
	char * remote_network_ipaddr = websGetVar(wp, T("remote_network_ipaddr"), T(""));
	char * remote_network_netmask = websGetVar(wp, T("remote_network_netmask"), T(""));
	char * preshared_key = 	websGetVar(wp, T("preshared_key"), T(""));
	char * mode = websGetVar(wp, T("mode"), T(""));
	char * phase1_encryption_algorithm = websGetVar(wp, T("phase1_encryption_algorithm"), T(""));
	char * phase1_integrity_algorithm = websGetVar(wp, T("phase1_integrity_algorithm"), T(""));
	char * phase1_dh_group = websGetVar(wp, T("phase1_dh_group"), T(""));
	char * sa_lifetime = websGetVar(wp, T("sa_lifetime"), T(""));
	char * phase2_encryption_algorithm = websGetVar(wp, T("phase2_encryption_algorithm"), T(""));
	char * phase2_integrity_algorithm = websGetVar(wp, T("phase2_integrity_algorithm"), T(""));
	char * phase2_dh_group = websGetVar(wp, T("phase2_dh_group"), T(""));
	char * ipsec_lifetime = websGetVar(wp, T("ipsec_lifetime"), T(""));	

	char ** ppQryResult = NULL;
	char * pCmd = (char *)malloc(640);
	memset(pCmd,'\0', 512);	
	char pSQLCmd[1024]={0};
	memset(pSQLCmd, 0x00, sizeof(pSQLCmd));

	//check if name is duplicate
	snprintf(pSQLCmd, sizeof(pSQLCmd), "select COUNT(*) from VpnIPSec where name='%s'", name);
	ppQryResult = jnrp_sql_getByQry_Webs(pSQLCmd);
	if ((vpn_tbl_idx==-1 && gatoi(ppQryResult[1]) > 0)||
		(vpn_tbl_idx>0 && gatoi(ppQryResult[1])>1)) 
	{
		setResultCode("Name already exists");			
	}
	else
	{
		memset(pSQLCmd, 0x00, sizeof(pSQLCmd));
		if (vpn_tbl_idx==-1) //add
		{
			gstrcat(pCmd, "insert into VpnIPSec values(");
			gstrcat(pCmd, "'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s'");
			gstrcat(pCmd, ")");
			snprintf(pSQLCmd,sizeof(pSQLCmd), pCmd, 
				name,
				remote_security_gateway,
				remote_network_ipaddr,
				remote_network_netmask,
				preshared_key,
				mode,
				phase1_encryption_algorithm,
				phase1_integrity_algorithm,
				phase1_dh_group,
				sa_lifetime,
				phase2_encryption_algorithm,
				phase2_integrity_algorithm,
				phase2_dh_group,
				ipsec_lifetime);		
		}
		else if (vpn_tbl_idx>0) //edit
		{
			gstrcat(pCmd, "update VpnIPSec set name='%s',");
			gstrcat(pCmd, "remote_security_gateway='%s',");
			gstrcat(pCmd, "remote_network_ipaddr='%s',");
			gstrcat(pCmd, "remote_network_netmask='%s',preshared_key='%s',");
			gstrcat(pCmd, "mode='%s',phase1_encryption_algorithm='%s',");
			gstrcat(pCmd, "phase1_integrity_algorithm='%s',phase1_dh_group='%s',");
			gstrcat(pCmd, "sa_lifetime='%s',phase2_encryption_algorithm='%s',");
			gstrcat(pCmd, "phase2_integrity_algorithm='%s',phase2_dh_group='%s',");
			gstrcat(pCmd, "ipsec_lifetime='%s' where _ROWID_=%d");
		
			snprintf(pSQLCmd,sizeof(pSQLCmd), pCmd, 
				name,
				remote_security_gateway,
				remote_network_ipaddr,
				remote_network_netmask,
				preshared_key,
				mode,
				phase1_encryption_algorithm,
				phase1_integrity_algorithm,
				phase1_dh_group,
				sa_lifetime,
				phase2_encryption_algorithm,
				phase2_integrity_algorithm,
				phase2_dh_group,
				ipsec_lifetime,
				vpn_tbl_idx);		
		}
		execSQLCmd(pSQLCmd);
	}
	free(pCmd);
	websRedirect(wp, T("advanced/vpn_ipsec.asp"));	
}

#endif
