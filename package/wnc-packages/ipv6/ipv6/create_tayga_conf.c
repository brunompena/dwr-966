
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#define USAGE_TEXT	\
"Usage: %s [-c|--config FILE] [-p|--prefix WANPrefix]\n" \
"--config FILE      : Write configuration options to FILE (/tmp/tayga.conf by default)\n" \
"--tundev           : tun-device (clat by default)\n" \
"--platprefix       : plat prefix (detect using DNS64 by default)\n" \
"--platdev          : plat-facing device (usb0 by default)\n" \
"--ipv4addr         : ipv4-addr (192.0.0.2 by default)\n" \
"--clatv4           : clat IPv4 address (192.0.0.1 by default)\n" \
"--prefix           : WAN IPv6 prefix\n"

#define TAYGA_CONF_PATH "/tmp/tayga.conf"
#define TUN_DEVICE "clat"
#define PLAT_DEVICE "usb0"
#define TAYGA_IPV4_ADDR "192.0.0.2"
#define CLAT_IPV4_ADDR "192.0.0.1"
#define IPv4_NAME "ipv4only.arpa"

#define	WKPF	(htonl(0x0064ff9b))

int validate_ip6_addr(const struct in6_addr *a)
{
	/* Well-known prefix for NAT64 */
	if (a->s6_addr32[0] == WKPF && !a->s6_addr32[1] && !a->s6_addr32[2])
		return 0;

	/* Reserved per RFC 2373 */
	if (!a->s6_addr[0])
		return -1;

	/* Multicast addresses */
	if (a->s6_addr[0] == 0xff)
		return -1;

	/* Link-local unicast addresses */
	if ((a->s6_addr16[0] & htons(0xffc0)) == htons(0xfe80))
		return -1;

	return 0;
}

int get_clat_v6_addr(char *prefix, char *clat_v6_addr)
{
  struct in6_addr addr;
  unsigned char buf[sizeof(struct in6_addr)];
  if (!inet_pton(AF_INET6, prefix, &addr)){
    fprintf(stderr, "invalid prefix.\n");
    return -1;
  }else if (validate_ip6_addr(&addr) < 0){
    fprintf(stderr, "invalid address.\n");
    return -1;
  }
  addr.s6_addr[11] = 193;//c1
  addr.s6_addr[12] = 167;//a7
  if(inet_ntop(AF_INET6, (void *) &addr, clat_v6_addr, INET6_ADDRSTRLEN) != NULL){
    fprintf(stdout, "Clat IPv6 address: %s\n", clat_v6_addr);
    return 0;
  }else
    return -1;
}

int get_clat_v6_addr_by_device(char *device, char *clat_v6_addr)
{
  //TODO:
  //This function considers any globally scoped /64 address on the PLAT-facing
  //device, checks to see if it is base on Modified EUI-64, and generates a
  //new address for the CLAT by substituting the "0xfffe" bits in the middle
  //of the Interface ID with 0xc1a7 ("clat"). 
  return -1;
}

int get_plat_prefix(char *ipv4_name, char *plat_prefix)
{
  struct addrinfo hints, *result, *p;
  int status, plat_addr_set, ipv4_records, ipv6_records;
  struct in6_addr plat_addr, this_plat_addr;
  struct sockaddr_in6 *this_addr;
  char plat_addr_str[INET6_ADDRSTRLEN];
  
  //TODO: dnssec (# RFC 7050 section 3)
  result = NULL;
  plat_addr_set = 0;
  ipv4_records = ipv6_records = 0;
  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET6;
  status = getaddrinfo(ipv4_name, NULL, &hints, &result);
  if(status != 0) {
    fprintf(stderr,"No AAAA records was returned for(%s) status = %d\n", ipv4_name, status);
    return 0;
  }
  for(p = result; p; p = p->ai_next) {
    if(p->ai_family != AF_INET6) {
      fprintf(stderr,"Got an non-AAAA RR? That's unexpected... Type=%d\n", p->ai_family);
      continue;
    }
    //TODO: find_rfc7050_wka
    ipv6_records++;
    this_addr = (struct sockaddr_in6 *)p->ai_addr;
    this_plat_addr = this_addr->sin6_addr;
    this_plat_addr.s6_addr32[3] = 0;//64:FF9B::/96
    if(!plat_addr_set) {
      plat_addr = this_plat_addr;
      plat_addr_set = 1;
      inet_ntop(AF_INET6, &plat_addr, plat_addr_str, sizeof(plat_addr_str));
      continue;
    }
    if(!IN6_ARE_ADDR_EQUAL(&plat_addr, &this_plat_addr)) {
      char this_plat_addr_str[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, &this_plat_addr, this_plat_addr_str, sizeof(this_plat_addr_str));
      fprintf(stderr,"plat_prefix/two different plat addrs = %s,%s\n",plat_addr_str,this_plat_addr_str);
    }else{
      //we've seen this prefix already, ignore it
      //(in most cases this will happen at least once, since ipv4only.arpa has two A records)
    }
  }
  if(result != NULL) {
    freeaddrinfo(result);
  }
  if(ipv6_records > 1) {
    // Cool! More than one prefix! Here we might at some point implement a
    // connectivity check which tests that the prefixes actually work, and
    // skips to the next one if so...
    fprintf(stdout,"Multiple PLAT prefixes discovered, using the first seen\n");
  }else if(ipv6_records == 0){
    fprintf(stdout,"No PLAT prefix could be discovered. Your ISP probably doesn't provide  NAT64/DNS64 PLAT service. Exiting.\n");
    return -1;
  }
  sprintf(plat_prefix,"%s/96",plat_addr_str);
  fprintf(stdout, "Detected NAT64 prefix %s\n", plat_prefix);
  return 1;
}

int write_config(char* conffile, char* clatDev, char* platPrefix, char* taygaV4Addr, char* clatV4Addr, char* clatV6Addr)
{
  FILE *fp;
  char buf[512];
  fp = fopen(conffile, "w+");
  if(fp){
    sprintf(buf,"tun-device %s\n",clatDev);
    fputs(buf, fp);
    sprintf(buf,"prefix %s\n",platPrefix);
    fputs(buf, fp);
    sprintf(buf,"ipv4-addr %s\n",taygaV4Addr);
    fputs(buf, fp);
    sprintf(buf,"map %s %s\n",clatV4Addr,clatV6Addr);
    fputs(buf, fp);
    fclose(fp);
	return 0;
  }else{
    fprintf(stderr,"can no open fle\n");
  }
  return -1;
}

int main(int argc, char **argv)
{
    int c;
    int longind;
	char *conffile = TAYGA_CONF_PATH;
	char *tunDev = TUN_DEVICE;
	char *platDev = PLAT_DEVICE;
	char *ipv4Addr = TAYGA_IPV4_ADDR;
	char *clatV4Addr = CLAT_IPV4_ADDR;
	char clatV6Addr[256];
	char platPrefix[256];
    char *wanPrefix = NULL;
	char *ipv4Name = IPv4_NAME;
	
	static struct option longopts[] = {
		{ "help", 0, 0, 0 },
		{ "config", 1, 0, 'c' },
		{ "prefix", 1, 0, 'p' },
		{ "tundev", 1, 9000 },
		{ "platprefix", 1, 0, 9001 },
		{ "platdev", 1, 0, 9002 },
		{ "ipv4addr", 0, 0, 9003 },
		{ "clatv4", 1, 0, 9004 },
		{ "ipv4name", 1, 0, 9005 },		
		{ 0, 0, 0, 0 }
	};
    memset(clatV6Addr,0,sizeof(clatV6Addr));
	
	for (;;) {
		c = getopt_long(argc, argv, "c:p:", longopts, &longind);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			fprintf(stderr, USAGE_TEXT, argv[0]);
			exit(0);
			break;
		case 'c':
			conffile = optarg;
			break;
		case 'p':
			wanPrefix = optarg;
			break;
		case 9000:
			tunDev = optarg;
			break;
		case 9001:
			memcpy(platPrefix,optarg,sizeof(platPrefix));
			break;
		case 9002:
			platDev = optarg;
			break;
		case 9003:
			ipv4Addr = optarg;
			break;
		case 9004:
			clatV4Addr = optarg;
			break;
		case 9005:
			ipv4Name = optarg;
			break;
		default:
			fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
			exit(1);
			break;
		}
	}
	//get clat ipv6 address
	if(wanPrefix != NULL){
	   if(get_clat_v6_addr(wanPrefix,clatV6Addr) < 0)
	     exit(1);
	}else if( get_clat_v6_addr_by_device(platDev, clatV6Addr) < 0 )
       exit(1);
	
	//get plat prefix
	if(get_plat_prefix(ipv4Name,platPrefix) <=0)
	  exit(1);
	  
	//create config file
	if(write_config(conffile,tunDev,platPrefix,ipv4Addr,clatV4Addr,clatV6Addr) < 0)
	  exit(1);
	  
	return 0;
}
