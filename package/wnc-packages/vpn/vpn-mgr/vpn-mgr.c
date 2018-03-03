#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

/*
VPNS_PPTP_USERNAME_0=test
VPNS_PPTP_PW_0=test
VPNS_PPTP_USERNAME_1=test2
VPNS_PPTP_PW_1=test2
VPNS_PPTP_REMOTE_IP_RANGE=192.168.1.240-254

VPNP_PPTP=off             =>  {on,off}
VPNP_L2TP=off             =>  {on,off}
VPNP_IPSEC=off            =>  {on,off}
VPN_MODE=disable         =>  {disable/vpns/vpnc }
VPNS_MODE=pptp          =>  {pptp,l2tp}
VPNC_MODE=pptp          =>  {pptp,l2tp}
VPNS_PPTP_USERNAME=howard1#howard2
VPNS_PPTP_PW=111111#2222222
VPNS_PPTP_REMOTE_IP_RANGE=192.168.0.100-110
VPNS_L2TP_USERNAME=
VPNS_L2TP_PW=
VPNC_PPTP_USERNAME=
VPNC_PPTP_PW=
VPNC_PPTP_SERVER=
VPNC_L2TP_USERNAME=howard
VPNC_L2TP_PW=111111
VPNC_L2TP_SERVER=1.1.1.1
VPNC_L2TP_MODE=l2tp	=>{l2tp,ipsec}

VPN_MODE=ipsec
NAT_TRAVERSAL=on
KEEP_ALIVE=60
NAME=test
REMOTE_SECURITY_GATEWAY=1.1.1.1
REMOTE_NETWORK_IPADDR=192.168.1.0
REMOTE_NETWORK_NETMASK=24
PRESHARED_KEY=1234567890
MODE=main
PHASE1_ENCRYPTION_ALGORITHM=des-md5
PHASE1_DH_GROUP=modp768
SA_LIFETIME=30
PHASE2_ENCRYPTION_ALGORITHM=des-md5
PHASE2_DH_GROUP=none
IPSEC_LIFETIME=30
*/


#define VPN_MODE "VPN_MODE="         
#define VPNS_PPTP_REMOTE_IP_RANGE "VPNS_PPTP_REMOTE_IP_RANGE="
#define VPNS_L2TP_REMOTE_IP_RANGE "VPNS_L2TP_REMOTE_IP_RANGE="
#define VPNC_PPTP_USERNAME "VPNC_PPTP_USERNAME="
#define VPNC_PPTP_PW "VPNC_PPTP_PW="
#define VPNC_PPTP_SERVER "VPNC_PPTP_SERVER="
#define VPNC_L2TP_USERNAME "VPNC_L2TP_USERNAME="
#define VPNC_L2TP_PW "VPNC_L2TP_PW="
#define VPNC_L2TP_SERVER "VPNC_L2TP_SERVER="
#define VPNC_L2TP_MODE	"VPNC_L2TP_MODE="

#define NAT_TRAVERSAL "NAT_TRAVERSAL="
#define KEEP_ALIVE "KEEP_ALIVE="
#define CONNECT_NAME "CONNECT_NAME="
#define REMOTE_GATEWAY_IP "REMOTE_GATEWAY_IP="
#define REMOTE_NETWORK_SUBNET "REMOTE_NETWORK_SUBNET="
#define PRESHARED_KEY "PRESHARED_KEY="
#define IPSEC_CONNECT_MODE "IPSEC_CONNECT_MODE="
#define PHASE1_ENCRYPTION_ALGORITHM "PHASE1_ENCRYPTION_ALGORITHM="
#define PHASE1_DH_GROUP "PHASE1_DH_GROUP="
#define SA_LIFETIME "SA_LIFETIME="
#define PHASE2_ENCRYPTION_ALGORITHM "PHASE2_ENCRYPTION_ALGORITHM="
#define PHASE2_DH_GROUP "PHASE2_DH_GROUP="
#define IPSEC_LIFETIME "IPSEC_LIFETIME="


#define VPN_CONFIG_PATCH "/sysconfig/vpn-cfg"
#define PPTP_CLIENT "pptp_client"
#define PPTP_SERVER "pptp_server"
#define L2TP_CLIENT "l2tp_client"
#define L2TP_SERVER "l2tp_server"

#define WAN_INF	"usb0"
#define LAN_INF "br0"
int get_pid(const char* name);
void get_local_ip(char* local_ip);
void get_local_subnet(char* subnet);
void kill_process_by_pid(int pid);
int get_vpn_param(char* source, char* value);
void create_pptp_client_config(void);
void create_l2tp_client_config(void);
void create_pptp_server_config(void);
void create_l2tp_server_config(void);
void create_ipsec_config(void);

void start_pptp_client(void);
void start_l2tp_client(void);
void start_pptp_server(void);
void start_l2tp_server(void);
void start_ipsec(void);

void stop_pptp_client(void);
void stop_l2tp_client(void);
void stop_pptp_server(void);
void stop_l2tp_server(void);
void stop_ipsec(void);

int exit_flag = 0;

void get_local_subnet(char* LocalSubnet)
{
	FILE *fs;
	system("ip route | awk '{if($3==\"br0\")print $1}' > /var/subnet");
	fs = fopen("/var/subnet", "r");
	if (fs != NULL) {
		fgets(LocalSubnet, 64, fs);
		fclose(fs);
	}
	LocalSubnet[strlen(LocalSubnet) - 1] = '\0';
	unlink("/var/subnet");
}

void get_local_wanip(char* WanIp)
{
	int s, ret;
        struct ifreq buffer;
        struct sockaddr *sa;
        s = socket(PF_INET, SOCK_DGRAM, 0);
        if (s < 0)
        {
                printf("Unable to open socket to get IP address.\n");
                return ;
        }

        memset(&buffer, 0x00, sizeof(buffer));
        strcpy(buffer.ifr_name, WAN_INF);

        ret = ioctl(s, SIOCGIFADDR, &buffer);
        if (ret < 0)
        {
                printf("Unable to call ioctl to get IP address.\n");
                /* Close the socket handle. */
                close(s);
                return ;
        }

        sa = (struct sockaddr *)&(buffer.ifr_addr);
        strcpy(WanIp, inet_ntoa(((struct sockaddr_in *)sa)->sin_addr));
        close(s);
#if 0
        FILE *fs;
        system("ifconfig usb0 | grep \"inet addr\" | cut -d: -f2 | awk -F\" \" '{print $1}' > /var/wanip");
        fs = fopen("/var/wanip", "r");
        if (fs != NULL) {
                fgets(WanIp, 64, fs);
                fclose(fs);
        }
        unlink("/var/wanip");
#endif
}

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
	//printf("start: system will kill pid=%d\n", pid);
        if(pid>0 && kill(pid, 0) == 0){
                kill(pid,15);
                sleep(2);
                while(kill(pid, 0) == 0 )
                        sleep(1);
        }
	//printf("end: system will kill pid=%d\n", pid);
}

void get_local_ip(char* local_ip)
{
	int s, ret;
	struct ifreq buffer;
	struct sockaddr *sa;
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s < 0)
	{
		printf("Unable to open socket to get IP address.\n");
                return ;
        }
	
	memset(&buffer, 0x00, sizeof(buffer));
	strcpy(buffer.ifr_name, LAN_INF);
	
	ret = ioctl(s, SIOCGIFADDR, &buffer);
	if (ret < 0)
	{
		printf("Unable to call ioctl to get IP address.\n");
                /* Close the socket handle. */
		close(s);
		return ;
        }

	sa = (struct sockaddr *)&(buffer.ifr_addr);
	strcpy(local_ip, inet_ntoa(((struct sockaddr_in *)sa)->sin_addr));	
	close(s);
}

int get_vpn_param(char* source, char* value)
{
	FILE* file = NULL;
	char cmd[128] = {0};
	char* ptr;
	int ret = 0;
	value[0] = '\0';
	file = fopen(VPN_CONFIG_PATCH, "r");
	if(!file)
                return 0;

	while(fgets(cmd,sizeof(cmd),file))
	{
		ptr = strstr(cmd, source);
		if(ptr)
		{
			strcpy(value, ptr+strlen(source));
			ptr = strchr(value, '\n');
			if(ptr)
				ptr[0] = '\0';
			ret = 1;
			break;
		}
	}
	fclose(file);
	return ret;
}

void create_pptp_client_config(void)
{
	FILE* fs = NULL;
	char cmd[256];
	char username[64];
	char password[128];
	char server[128];
	unlink("/etc/ppp/options.pptp");
	unlink("/etc/ppp/chap-secrets");
	sprintf(cmd, "/etc/ppp/peers/%s", PPTP_CLIENT);
	unlink(cmd);
	get_vpn_param(VPNC_PPTP_USERNAME, username);
	get_vpn_param(VPNC_PPTP_PW, password);
	get_vpn_param(VPNC_PPTP_SERVER, server);
	/*create options.pptp*/
	fs = fopen("/etc/ppp/options.pptp", "w");
	if (fs != NULL ) {
		strcpy(cmd, "lock noauth nobsdcomp nodeflate");
		fputs(cmd, fs);
                fclose(fs);
	}

	/*create chap-secrets*/
	fs = fopen("/etc/ppp/chap-secrets", "w");
	if (fs != NULL ) {
		sprintf(cmd, "%s %s %s *", username, PPTP_CLIENT, password);
		fputs(cmd, fs);
		fclose(fs);
	}

	/*create main cfg*/
	sprintf(cmd, "/etc/ppp/peers/%s", PPTP_CLIENT);
	fs = fopen(cmd, "w");
	if (fs != NULL ) {
		sprintf(cmd, "pty \"pptp %s --nolaunchpppd\"\n", server);
		fputs(cmd, fs);
		strcpy(cmd, "mppe required,stateless\n");
		fputs(cmd, fs);
		sprintf(cmd, "name %s\n", username);
		fputs(cmd, fs);
		sprintf(cmd, "remotename %s\n", PPTP_CLIENT);
		fputs(cmd, fs);
		strcpy(cmd, "file /etc/ppp/options.pptp\n");
                fputs(cmd, fs);
		sprintf(cmd, "ipparam %s\n", PPTP_CLIENT);
                fputs(cmd, fs);
		fclose(fs);
	}
}

void start_pptp_client(void)
{
	char cmd[128];
	FILE* fs = NULL;
	system("killall -15 pppd");
	system("rm -rf /var/log/vpn");
	system("mkdir -p /var/log/vpn");
	fs = fopen("/var/log/vpn/vpn_status", "w");
	if (fs != NULL ) {
		strcpy(cmd, "connecting");
		fputs(cmd, fs);
		fclose(fs);
	}
	sprintf(cmd, "pppd call %s debug dump updetach &", PPTP_CLIENT);
	system(cmd);
}

void stop_pptp_client(void)
{
	system("killall -SIGTERM pppd");
}

void create_pptp_server_config(void)
{
	FILE* fs = NULL;
	char cmd[256];
	char cmd2[256];
	char local_ip[24];
	char ip_range[36];
	char ip_range_tmp[24];
	char username[128];
	char password[128];
	int byte1, byte2, byte3, byte4; 
	int i = 0;
	get_local_ip(local_ip);
	get_vpn_param(VPNS_PPTP_REMOTE_IP_RANGE, ip_range_tmp);
	sscanf(local_ip, "%d.%d.%d.%d", &byte1, &byte2, &byte3, &byte4);
	sprintf(ip_range, "%d.%d.%d.%s", byte1, byte2, byte3, ip_range_tmp);
	/*create pptpd.conf*/
	fs = fopen("/etc/pptpd.conf", "w");
	if (fs != NULL ) {
		strcpy(cmd, "debug\n");
		fputs(cmd, fs);
		strcpy(cmd, "option /etc/ppp/options.pptpd\n");
		fputs(cmd, fs);
		strcpy(cmd, "speed 115200\n");
		fputs(cmd, fs);
		strcpy(cmd, "stimeout 10\n");
		fputs(cmd, fs);
		sprintf(cmd, "localip %s\n", local_ip);
		fputs(cmd, fs);
		sprintf(cmd, "remoteip %s\n", ip_range);
		fputs(cmd, fs);
		fclose(fs);
	}

        fs = fopen("/etc/ppp/chap-secrets", "w");
	if (fs != NULL ) {
		while(1)
		{
			sprintf(cmd, "VPNS_PPTP_USERNAME_%d=",i);
			sprintf(cmd2, "VPNS_PPTP_PW_%d=",i);
			if(get_vpn_param(cmd, username) && get_vpn_param(cmd2, password))
			{
				sprintf(cmd, "%s %s %s *\n", username, PPTP_SERVER, password);
				fputs(cmd, fs);
			}
			else
				break;
			i++;
		}		
		fclose(fs);
#if 0
		get_vpn_param(VPNS_PPTP_USERNAME, username);
		get_vpn_param(VPNS_PPTP_PW, password);
		char* ptr_u = NULL;
		char* ptr_p = NULL;
		while(1)
		{
			ptr_u = strchr(username, '#');
			ptr_p = strchr(password, '#');
			if(ptr_u && ptr_p)
			{
				ptr_u[0] = '\0';
				ptr_p[0] = '\0';
				sprintf(cmd, "%s %s %s *\n", username, PPTP_SERVER, password);
				fputs(cmd, fs);
				strcpy(username, ptr_u + 1);
				strcpy(password, ptr_p + 1);
			}
			else
			{
				sprintf(cmd, "%s %s %s *\n", username, PPTP_SERVER, password);
				fputs(cmd, fs);
				break;
			}
		}
		fclose(fs);
#endif
	}

	/*create options.pptpd*/
	fs = fopen("/etc/ppp/options.pptpd", "w");
	if (fs != NULL ) {
		strcpy(cmd, "debug\n");
		fputs(cmd, fs);
		sprintf(cmd, "%s:\n", local_ip);
		fputs(cmd, fs);
		strcpy(cmd, "auth\n");
		fputs(cmd, fs);
		sprintf(cmd, "name %s\n", PPTP_SERVER);
		fputs(cmd, fs);
		strcpy(cmd, "lcp-echo-failure 3\n");
		fputs(cmd, fs);
		strcpy(cmd, "lcp-echo-interval 60\n");
		fputs(cmd, fs);
		strcpy(cmd, "default-asyncmap\n");
		fputs(cmd, fs);
		strcpy(cmd, "mtu 1482\n");
		fputs(cmd, fs);
		strcpy(cmd, "mru 1482\n");
		fputs(cmd, fs);
		strcpy(cmd, "nobsdcomp\n");
		fputs(cmd, fs);
		strcpy(cmd, "nodeflate\n");
		fputs(cmd, fs);
		strcpy(cmd, "mppe required,no40,no56,stateless\n");
		fputs(cmd, fs);
		strcpy(cmd, "require-mschap-v2\n");
		fputs(cmd, fs);
		strcpy(cmd, "refuse-chap\n");
		fputs(cmd, fs);
		strcpy(cmd, "refuse-mschap\n");
		fputs(cmd, fs);
		strcpy(cmd, "refuse-eap\n");
		fputs(cmd, fs);
		strcpy(cmd, "refuse-pap\n");
		fputs(cmd, fs);
		strcpy(cmd, "proxyarp\n");
		fputs(cmd, fs);
		strcpy(cmd, "ms-dns 8.8.8.8\n");
		fputs(cmd, fs);
		strcpy(cmd, "ms-dns 8.8.4.4\n");
		fputs(cmd, fs);
		fclose(fs);
	}
}

void start_pptp_server(void)
{
	system("pptpd start &");
	sleep(3);
	char cmd[128];
	sprintf(cmd, "iptables -t nat -A PREROUTING -i %s -p tcp --dport 1723 -j ACCEPT", WAN_INF);
	system(cmd);
	sprintf(cmd, "iptables -A INPUT -i %s -p tcp --dport 1723 -j ACCEPT", WAN_INF);
	system(cmd);
	system("iptables -A OUTPUT -p 47 -j ACCEPT");
	system("iptables -A INPUT -p 47 -j ACCEPT");
	system("iptables -A INPUT -i ppp+ -j ACCEPT");
	system("iptables -A OUTPUT -o ppp+ -j ACCEPT");
	system("iptables -A FORWARD -i ppp+ -j ACCEPT");
}

void stop_pptp_server(void)
{
	char cmd[128];
	int pid = -1;
	sprintf(cmd, "iptables -t nat -D PREROUTING -i %s -p tcp --dport 1723 -j ACCEPT", WAN_INF);
	system(cmd);
	sprintf(cmd, "iptables -D INPUT -i %s -p tcp --dport 1723 -j ACCEPT", WAN_INF);
	system(cmd);
	system("iptables -D OUTPUT -p 47 -j ACCEPT");
	system("iptables -D INPUT -p 47 -j ACCEPT");
	system("iptables -D INPUT -i ppp+ -j ACCEPT");
	system("iptables -D OUTPUT -o ppp+ -j ACCEPT");
	system("iptables -D FORWARD -i ppp+ -j ACCEPT");
	system("killall -SIGTERM pppd");
	pid = get_pid("pptpd.pid");
	if(pid > 0){
		kill_process_by_pid(pid);
		unlink("/var/run/pptpd.pid");
	}
}

void create_l2tp_client_config(void)
{
	FILE* fs = NULL;
	char cmd[256];
	char username[64];
	char password[128];
	char server[128];
	char mode[16];
	unlink("/etc/xl2tpd/xl2tpd.conf");
	unlink("/etc/ppp/options.xl2tpd");
	get_vpn_param(VPNC_L2TP_USERNAME, username);
	get_vpn_param(VPNC_L2TP_PW, password);
	get_vpn_param(VPNC_L2TP_SERVER, server);
	get_vpn_param(VPNC_L2TP_MODE, mode);

	/*create xl2tpd.conf*/
	fs = fopen("/etc/xl2tpd/xl2tpd.conf", "w");
	if (fs != NULL ) {
		strcpy(cmd, "[global]\n");
		fputs(cmd, fs);
		strcpy(cmd, "auth file=/var/xl2tpd/l2tp-secrets\n");
		fputs(cmd, fs);
		strcpy(cmd, "ipsec saref = yes\n");
		fputs(cmd, fs);
		strcpy(cmd, "port = 1701\n\n");
		fputs(cmd, fs);
		sprintf(cmd, "[lac %s]\n", L2TP_CLIENT);
		fputs(cmd, fs);
		sprintf(cmd, "lns = %s\n", server);
		fputs(cmd, fs);
		strcpy(cmd, "redial = yes\n");
		fputs(cmd, fs);
		strcpy(cmd, "redial timeout = 3\n");
		fputs(cmd, fs);
		strcpy(cmd, "ppp debug = yes\n");
		fputs(cmd, fs);
		strcpy(cmd, "pppoptfile = /etc/ppp/options.xl2tpd\n");
		fputs(cmd, fs);
		strcpy(cmd, "length bit = yes\n");
		fputs(cmd, fs);
		strcpy(cmd, "require authentication = yes\n");
		fputs(cmd, fs);
		strcpy(cmd, "challenge = no\n");
		fputs(cmd, fs);
		fclose(fs);
	}

	/*create options.xl2tpd*/
	fs = fopen("/etc/ppp/options.xl2tpd", "w");
        if (fs != NULL ) {
		strcpy(cmd, "ipcp-accept-local\n");
		fputs(cmd, fs);
		strcpy(cmd, "ipcp-accept-remote\n");
		fputs(cmd, fs);
		strcpy(cmd, "lcp-echo-interval 20\n");
		fputs(cmd, fs);
		strcpy(cmd, "lcp-echo-failure 4\n");
		fputs(cmd, fs);
		strcpy(cmd, "usepeerdns\n");
		fputs(cmd, fs);
		strcpy(cmd, "noccp\n");
		fputs(cmd, fs);
		strcpy(cmd, "noauth\n");
		fputs(cmd, fs);
		strcpy(cmd, "crtscts\n");
		fputs(cmd, fs);
		strcpy(cmd, "mtu 1452\n");
		fputs(cmd, fs);
		strcpy(cmd, "mru 1452\n");
		fputs(cmd, fs);
		strcpy(cmd, "debug\n");
		fputs(cmd, fs);
		strcpy(cmd, "lock\n");
		fputs(cmd, fs);
		strcpy(cmd, "connect-delay 5000\n");
		fputs(cmd, fs);
		sprintf(cmd, "name  %s\n", username);
		fputs(cmd, fs);
		sprintf(cmd, "password  %s\n", password);
		fputs(cmd, fs);
		sprintf(cmd, "ipparam %s\n", L2TP_CLIENT);
		fputs(cmd, fs);
                fclose(fs);
        }
}

void start_l2tp_client(void)
{
	char cmd[128];
	FILE* fs = NULL;
	system("mkdir -p /var/run/xl2tpd/");
	system("xl2tpd -D &");
	sleep(1);
	system("rm -rf /var/log/vpn");
	system("mkdir -p /var/log/vpn");
	fs = fopen("/var/log/vpn/vpn_status", "w");
	if (fs != NULL ) {
		strcpy(cmd, "connecting");
		fputs(cmd, fs);
		fclose(fs);
        }

	fs = fopen("/var/run/xl2tpd/l2tp-control", "w");
	if (fs != NULL ) {
		sprintf(cmd, "c %s", L2TP_CLIENT);
		fputs(cmd, fs);
		fclose(fs);
	}
}

void stop_l2tp_client(void)
{
	char cmd[128];
	FILE* fs = NULL;
	int pid = -1;
	fs = fopen("/var/run/xl2tpd/l2tp-control", "w");
	if (fs != NULL ) {
		sprintf(cmd, "d %s", L2TP_CLIENT);
		fputs(cmd, fs);
		fclose(fs);
	}
	sleep(1);
	pid = get_pid("xl2tpd.pid");
	if(pid > 0){
		kill_process_by_pid(pid);
		unlink("/var/run/xl2tpd.pid");
	}
}

void create_l2tp_server_config(void)
{
	FILE* fs = NULL;
	char cmd[256];
	char cmd2[256];
	char local_ip[24];
	char ip_range[36];
	char ip_range_tmp[36];
	char username[128];
	char password[128];
	int byte1, byte2, byte3, byte4;
	int i = 0;
	get_local_ip(local_ip);
	get_vpn_param(VPNS_L2TP_REMOTE_IP_RANGE, ip_range_tmp);
	sscanf(local_ip, "%d.%d.%d.%d", &byte1, &byte2, &byte3, &byte4);
	sprintf(ip_range, "%d.%d.%d.%s", byte1, byte2, byte3, ip_range_tmp);

	/*create xl2tpd.conf*/
	fs = fopen("/etc/xl2tpd/xl2tpd.conf", "w");
	if (fs != NULL ) {
		strcpy(cmd, "[global]\n");
		fputs(cmd, fs);
		strcpy(cmd, "auth file=/var/xl2tpd/l2tp-secrets\n");
		fputs(cmd, fs);
		strcpy(cmd, "ipsec saref = yes\n");
		fputs(cmd, fs);
		strcpy(cmd, "port = 1701\n\n");
		fputs(cmd, fs);
		strcpy(cmd, "[lns default]\n");
		fputs(cmd, fs);
		sprintf(cmd, "ip range = %s\n", ip_range);
		fputs(cmd, fs);
		sprintf(cmd, "local ip = %s\n", local_ip);
		fputs(cmd, fs);
		strcpy(cmd, "require authentication = yes\n");
		fputs(cmd, fs);
		sprintf(cmd, "name = %s\n", L2TP_SERVER);
		fputs(cmd, fs);
		strcpy(cmd, "pppoptfile = /etc/ppp/options.xl2tpd\n");
		fputs(cmd, fs);
		strcpy(cmd, "length bit = yes\n");
		fputs(cmd, fs);
		strcpy(cmd, "challenge = no\n");
		fputs(cmd, fs);
		fclose(fs);
	}

	/*create options.xl2tpd*/
	fs = fopen("/etc/ppp/options.xl2tpd", "w");
	if (fs != NULL ) {
		strcpy(cmd, "refuse-pap\n");
		fputs(cmd, fs);
		strcpy(cmd, "require-chap\n");
		fputs(cmd, fs);
		strcpy(cmd, "lcp-echo-interval 20\n");
		fputs(cmd, fs);
		strcpy(cmd, "lcp-echo-failure 4\n");
		fputs(cmd, fs);
		strcpy(cmd, "auth\n");
		fputs(cmd, fs);
		strcpy(cmd, "crtscts\n");
		fputs(cmd, fs);
		strcpy(cmd, "idle 1800\n");
		fputs(cmd, fs);
		strcpy(cmd, "mtu 1452\n");
		fputs(cmd, fs);
		strcpy(cmd, "mru 1452\n");
		fputs(cmd, fs);
		strcpy(cmd, "debug\n");
		fputs(cmd, fs);
		strcpy(cmd, "lock\n");
		fputs(cmd, fs);
		strcpy(cmd, "connect-delay 5000\n");
		fputs(cmd, fs);
		strcpy(cmd, "proxyarp\n");
		fputs(cmd, fs);
		strcpy(cmd, "ms-dns 8.8.8.8\n");
		fputs(cmd, fs);
		strcpy(cmd, "ms-dns 8.8.4.4\n");
		fputs(cmd, fs);
		fclose(fs);
	}

	/*create chap-secrets*/
	fs = fopen("/etc/ppp/chap-secrets", "w");
	if (fs != NULL ) {
		while(1)
                {
                        sprintf(cmd, "VPNS_L2TP_USERNAME_%d=",i);
                        sprintf(cmd2, "VPNS_L2TP_PW_%d=",i);
                        if(get_vpn_param(cmd, username) && get_vpn_param(cmd2, password))
                        {
                                sprintf(cmd, "%s %s %s *\n", username, L2TP_SERVER, password);
                                fputs(cmd, fs);
                        }
                        else
                                break;
                        i++;
                }
		fclose(fs);
	}
}

void start_l2tp_server(void)
{
	system("mkdir -p /var/run/xl2tpd/");
	system("xl2tpd -D &");
	system("iptables -A INPUT -p udp --dport 1701 -j ACCEPT");
	system("iptables -A INPUT -i ppp+ -j ACCEPT");
	system("iptables -A OUTPUT -o ppp+ -j ACCEPT");
	system("iptables -A FORWARD -i ppp+ -j ACCEPT");
}

void stop_l2tp_server(void)
{
	int pid = -1;
	system("iptables -D INPUT -p udp --dport 1701 -j ACCEPT");
	system("iptables -D INPUT -i ppp+ -j ACCEPT");
	system("iptables -D OUTPUT -o ppp+ -j ACCEPT");
	system("iptables -D FORWARD -i ppp+ -j ACCEPT");
	system("killall -SIGTERM pppd");
	pid = get_pid("xl2tpd.pid");
	if(pid > 0){
		kill_process_by_pid(pid);
		unlink("/var/run/xl2tpd.pid");
	}
}

void create_ipsec_config(void)
{
	FILE* fs = NULL;
	char cmd[256];
	char LocalSubnet[64];
	char LocalWanIp[64];
	char param[128];
	char param2[128];
	get_local_subnet(LocalSubnet);
	memset(LocalWanIp, '\0', 64);
	get_local_wanip(LocalWanIp);
	/*create ipsec.conf*/
        fs = fopen("/etc/ipsec.conf", "w");
        if (fs != NULL ) {
		strcpy(cmd, "config setup\n");
		fputs(cmd, fs);
		get_vpn_param(NAT_TRAVERSAL, param);
		if(!strcmp(param, "on"))
		{
			strcpy(cmd, "\tnat_traversal=yes\n");
			fputs(cmd, fs);
			get_vpn_param(KEEP_ALIVE, param);
			sprintf(cmd, "\tkeep_alive=%s\n", param);
			fputs(cmd, fs);
		}
		else
		{
			strcpy(cmd, "\tnat_traversal=no\n");
			fputs(cmd, fs);
		}
		strcpy(cmd, "\toe=off\n");
		fputs(cmd, fs);
		strcpy(cmd, "\tprotostack=netkey\n");
		fputs(cmd, fs);
		strcpy(cmd, "\tinterfaces=\%defaultroute\n");
		fputs(cmd, fs);
		get_vpn_param(CONNECT_NAME, param);	
		sprintf(cmd, "conn %s\n", param);
		fputs(cmd, fs);
		sprintf(cmd, "\tleft=%s\n", LocalWanIp);
		fputs(cmd, fs);
		sprintf(cmd, "\tleftsubnet=%s\n", LocalSubnet);
		fputs(cmd, fs);
		//sprintf(cmd, "\tleftid=%s\n", "1.1.1.1");
		//fputs(cmd, fs);
		get_vpn_param(REMOTE_GATEWAY_IP, param);
		sprintf(cmd, "\tright=%s\n", param);
		fputs(cmd, fs);
		get_vpn_param(REMOTE_NETWORK_SUBNET, param);
		sprintf(cmd, "\trightsubnet=%s\n", param);
		fputs(cmd, fs);
		//sprintf(cmd, "\trightid=%s\n", "2.2.2.2");
		//fputs(cmd, fs);
		strcpy(cmd, "\tconnaddrfamily=ipv4\n");
		fputs(cmd, fs);
		get_vpn_param(PHASE1_ENCRYPTION_ALGORITHM, param);
		get_vpn_param(PHASE1_DH_GROUP, param2);
		sprintf(cmd, "\tike=%s;%s\n", param, param2);
		fputs(cmd, fs);
		get_vpn_param(SA_LIFETIME, param);
		sprintf(cmd, "\tikelifetime=%sm\n", param);
		fputs(cmd, fs);
		strcpy(cmd, "\ttype=tunnel\n");
		fputs(cmd, fs);
		get_vpn_param(IPSEC_LIFETIME, param);
		sprintf(cmd, "\tsalifetime=%sm\n", param);
		fputs(cmd, fs);
		get_vpn_param(PHASE2_ENCRYPTION_ALGORITHM, param);
		get_vpn_param(PHASE2_DH_GROUP, param2);
		if(!strcmp(param2, "none"))
		{
			sprintf(cmd, "\tphase2alg=%s\n", param);
			fputs(cmd, fs);
		}else
		{
			sprintf(cmd, "\tphase2alg=%s;%s\n", param, param2);
			fputs(cmd, fs);
		}
		get_vpn_param(IPSEC_CONNECT_MODE, param);
		if(!strcmp(param, "aggressive"))
		{
			strcpy(cmd, "\taggrmode=yes\n");
			fputs(cmd, fs);
		}
		strcpy(cmd, "\tpfs=no\n");
		fputs(cmd, fs);
		strcpy(cmd, "\tphase2=esp\n");
		fputs(cmd, fs);
		strcpy(cmd, "\tkeyexchange=ike\n");
		fputs(cmd, fs);
		strcpy(cmd, "\tauthby=secret\n");
		fputs(cmd, fs);
		strcpy(cmd, "\tauto=add\n");
		fputs(cmd, fs);
		fclose(fs);
	}

	/*create ipsec.secrets*/
	fs = fopen("/etc/ipsec.secrets", "w");
	if (fs != NULL ) {
		get_vpn_param(PRESHARED_KEY, param);
		get_vpn_param(REMOTE_GATEWAY_IP, param2);
		sprintf(cmd, "%s %s: PSK \"%s\"\n", LocalWanIp, param2, param);
		fputs(cmd, fs);
		fclose(fs);
	}
}

void wait_for_establish_tunnel(char* ConnectName)
{
	char cmd[128];
	char connect[128];
	FILE *fs = NULL;
	char* ptr = NULL;
	int connected = 0;
	sprintf(connect, "auto --up %s", ConnectName);
	while(1)
	{
		system("ps | grep -r \"auto --up\" > /var/ipsec_status");
        	fs = fopen("/var/ipsec_status", "r");
		if(fs != NULL)
		{
			connected = 1;
			while(fgets(cmd, 128, fs))
			{
				ptr = strstr(cmd, connect);
				if(ptr)
				{
					connected = 0;
					break;
				}
			}
			fclose(fs);
		}
		else 
			break;
		if(connected)
		{
			printf("ipsec connected !!\n");
			break;
		}
		else
			printf("ipsec connecting !!\n");
		if(exit_flag)
		{
			printf("user disable ipsec !!\n");
			system("ipsec setup stop &");
			sleep(2);
			exit(0);
		}
		sleep(1);
	}
}

void start_ipsec(void)
{
	char cmd[128];
	char LocalSubnet[64];
	char RemoteSubnet[64];
	char ConnectName[64];
        get_local_subnet(LocalSubnet);
	get_vpn_param(REMOTE_NETWORK_SUBNET, RemoteSubnet);
	get_vpn_param(CONNECT_NAME, ConnectName);
	system("mkdir -p /var/run/pluto/");
	system("ipsec setup start");
	sleep(3);
	/*start to connect*/
	sprintf(cmd, "ipsec auto --up %s &", ConnectName);
	system(cmd);
	wait_for_establish_tunnel(ConnectName);
	sprintf(cmd, "iptables -t nat -I POSTROUTING -o usb0 -s %s -d %s -j ACCEPT", LocalSubnet, RemoteSubnet);
	system(cmd);
	system("conntrack -F");
}

void stop_ipsec(void)
{
	char LocalSubnet[64];
	char RemoteSubnet[64];
	char ConnectName[64];
	char cmd[128];
	get_local_subnet(LocalSubnet);
	get_vpn_param(REMOTE_NETWORK_SUBNET, RemoteSubnet);
	get_vpn_param(CONNECT_NAME, ConnectName);
	sprintf(cmd, "iptables -t nat -D POSTROUTING -o usb0 -s %s -d %s -j ACCEPT", LocalSubnet, RemoteSubnet);
	system(cmd);
	sprintf(cmd, "ipsec auto --down %s", ConnectName);
	system(cmd);
	system("ipsec setup stop");
}

int termhandler(int sig){
	exit_flag = 1;
}

int main(int argc, char *argv[])
{
	daemon(0,0);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGINT,  SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, termhandler);
	char vpn_mode[16];
	char cmd[64];
	FILE* fs = NULL;
	int run_pptp_client = 0;
	int run_l2tp_client = 0;
	int run_pptp_server = 0;
	int run_l2tp_server = 0;
	int run_ipsec = 0;

	fs = fopen("/var/run/vpn-mgr.pid", "w");
	if(fs != NULL)
	{
		sprintf(cmd, "%d\n", getpid());
		fputs(cmd, fs);
		fclose(fs);
	}

	if(!get_vpn_param(VPN_MODE, vpn_mode))
		return -1;
	//printf("vpn_mode:%s, len:%d\n", vpn_mode, strlen(vpn_mode));
	if(!strcmp(vpn_mode, "disable"))
		return 0;
	else if(!strcmp(vpn_mode, "pptp_client"))
	{
		create_pptp_client_config();
		start_pptp_client();
		run_pptp_client = 1;
	}
	else if(!strcmp(vpn_mode, "l2tp_client"))
	{
		create_l2tp_client_config();
		start_l2tp_client();
		run_l2tp_client = 1;
	}
	else if(!strcmp(vpn_mode, "pptp_server"))
	{
		create_pptp_server_config();
		start_pptp_server();
		run_pptp_server = 1;
	}
	else if(!strcmp(vpn_mode, "l2tp_server"))
	{
		create_l2tp_server_config();
		start_l2tp_server();
		run_l2tp_server = 1;
	}
	else if(!strcmp(vpn_mode, "ipsec"))
	{
		create_ipsec_config();
		start_ipsec();
		run_ipsec = 1;
	}

	while(1)
	{
		if(exit_flag)
		{
			if(run_pptp_client)
				stop_pptp_client();
			else if(run_pptp_server)
				stop_pptp_server();
			else if(run_l2tp_client)
				stop_l2tp_client();
			else if(run_l2tp_server)
				stop_l2tp_server();
			else if(run_ipsec)
				stop_ipsec();
			break;
		}
		sleep(1);
	}
	unlink("/var/run/vpn-mgr.pid");
}
