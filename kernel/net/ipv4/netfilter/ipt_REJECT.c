/*
 * This is a module which is used for rejecting packets.
 */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/route.h>
#include <net/dst.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_REJECT.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#include <linux/netfilter_bridge.h>
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("Xtables: packet \"rejection\" target for IPv4");

/* Send RST reply */
static void send_reset(struct sk_buff *oldskb, int hook)
{
	struct sk_buff *nskb;
	const struct iphdr *oiph;
	struct iphdr *niph;
	const struct tcphdr *oth;
	struct tcphdr _otcph, *tcph;

	/* IP header checks: fragment. */
	if (ip_hdr(oldskb)->frag_off & htons(IP_OFFSET))
		return;

	oth = skb_header_pointer(oldskb, ip_hdrlen(oldskb),
				 sizeof(_otcph), &_otcph);
	if (oth == NULL)
		return;

	/* No RST for RST. */
	if (oth->rst)
		return;

	if (skb_rtable(oldskb)->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		return;

	/* Check checksum */
	if (nf_ip_checksum(oldskb, hook, ip_hdrlen(oldskb), IPPROTO_TCP))
		return;
	oiph = ip_hdr(oldskb);

	nskb = alloc_skb(sizeof(struct iphdr) + sizeof(struct tcphdr) +
			 LL_MAX_HEADER, GFP_ATOMIC);
	if (!nskb)
		return;

	skb_reserve(nskb, LL_MAX_HEADER);

	skb_reset_network_header(nskb);
	niph = (struct iphdr *)skb_put(nskb, sizeof(struct iphdr));
	niph->version	= 4;
	niph->ihl	= sizeof(struct iphdr) / 4;
	niph->tos	= 0;
	niph->id	= 0;
	niph->frag_off	= htons(IP_DF);
	niph->protocol	= IPPROTO_TCP;
	niph->check	= 0;
	niph->saddr	= oiph->daddr;
	niph->daddr	= oiph->saddr;

	skb_reset_transport_header(nskb);
	tcph = (struct tcphdr *)skb_put(nskb, sizeof(struct tcphdr));
	memset(tcph, 0, sizeof(*tcph));
	tcph->source	= oth->dest;
	tcph->dest	= oth->source;
	tcph->doff	= sizeof(struct tcphdr) / 4;

	if (oth->ack)
		tcph->seq = oth->ack_seq;
	else {
		tcph->ack_seq = htonl(ntohl(oth->seq) + oth->syn + oth->fin +
				      oldskb->len - ip_hdrlen(oldskb) -
				      (oth->doff << 2));
		tcph->ack = 1;
	}

	tcph->rst	= 1;
	tcph->check = ~tcp_v4_check(sizeof(struct tcphdr), niph->saddr,
				    niph->daddr, 0);
	nskb->ip_summed = CHECKSUM_PARTIAL;
	nskb->csum_start = (unsigned char *)tcph - nskb->head;
	nskb->csum_offset = offsetof(struct tcphdr, check);

	/* ip_route_me_harder expects skb->dst to be set */
	skb_dst_set_noref(nskb, skb_dst(oldskb));

	nskb->protocol = htons(ETH_P_IP);
	if (ip_route_me_harder(nskb, RTN_UNSPEC))
		goto free_nskb;

	niph->ttl	= ip4_dst_hoplimit(skb_dst(nskb));

	/* "Never happens" */
	if (nskb->len > dst_mtu(skb_dst(nskb)))
		goto free_nskb;

	nf_ct_attach(nskb, oldskb);

	ip_local_out(nskb);
	return;

 free_nskb:
	kfree_skb(nskb);
}

static inline void send_unreach(struct sk_buff *skb_in, int code)
{
	icmp_send(skb_in, ICMP_DEST_UNREACH, code, 0);
}


// GARYeh 20131008: add http redirect with REJECT target
#define NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

#define NIPQUAD_FMT		"%u.%u.%u.%u"

#define LAN_INTERFACE	"br0"
#define BLOCK_PAGE		"urlFilterBlocked.asp"
#define REDIRECT_PAGE	"urlFilterLogin.asp"

#define MAX_HTTP_REDIRECT_CONTENT 512
static int send_http_redirect(struct sk_buff *oldskb, int hook, char *fun)
{   
	char http_content[MAX_HTTP_REDIRECT_CONTENT], *pHttp_content;   

	// Get LAN interface IP
	struct net_device *br0_dev; 
	struct in_device *br0_in_dev;

	// get interface
	br0_dev = dev_get_by_name(&init_net, LAN_INTERFACE);
	br0_in_dev = in_dev_get(br0_dev);

	memset(http_content, 0, sizeof(http_content));
	if(strcmp(fun,"block") == 0)
	{
		//printk("ipt_REJECT: IPT_WEB_BLOCK.\n");
		sprintf(http_content,"HTTP/1.1 303\r\nContent-Type: text/html\r\nConnection: close\r\nLocation: http://"NIPQUAD_FMT"/"BLOCK_PAGE"\r\n", NIPQUAD(br0_in_dev->ifa_list->ifa_address));
	}
	else if(strcmp(fun,"redirect") == 0)
	{	
		//printk("ipt_REJECT: IPT_WEB_REDIRECT.\n");
		sprintf(http_content,"HTTP/1.1 303\r\nContent-Type: text/html\r\nConnection: close\r\nLocation: http://"NIPQUAD_FMT"/"REDIRECT_PAGE"\r\n", NIPQUAD(br0_in_dev->ifa_list->ifa_address));	
	}
	// GARYeh 20131009:release interface after dev_get_by_name() & in_dev_get()
	in_dev_put(br0_in_dev);
	dev_put(br0_dev);

	struct sk_buff *nskb;
	const struct iphdr *oiph;
	struct iphdr *niph;
	const struct tcphdr *oth;
	struct tcphdr _otcph, *tcph;
	unsigned int otcplen;   /* IP header checks: fragment. */	
	if (ip_hdr(oldskb)->frag_off & htons(IP_OFFSET)) 
		return 0;  	
	oth = skb_header_pointer(oldskb, ip_hdrlen(oldskb), sizeof(_otcph), &_otcph);
	otcplen = oldskb->len - ip_hdr(oldskb)->ihl*4;
	if (oth == NULL)
		return 0;   /* No RST for RST. */
	if (oth->rst)
		return 0;   /* Check checksum */
	if (skb_rtable(oldskb)->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		return 0;
	if (nf_ip_checksum(oldskb, hook, ip_hdrlen(oldskb), IPPROTO_TCP))
		return 0;
	oiph = ip_hdr(oldskb);	   
	unsigned char *http_data = (void *)oth + oth->doff*4;
	/*debug message*/
	   /*
	   unsigned int datalen = (oldskb)->len - (oiph->ihl*4) - (oth->doff*4);  
	   char show_data[20];
	   memset(show_data, 0, sizeof(show_data));
	   memcpy(show_data, http_data, sizeof(show_data) - 1);
	   printk(KERN_EMERG "__[Kernel]datalen=%d http data=%sn", datalen,show_data);	 
	   if (datalen < 10){
		  return;
	   }
	   */	   
	if(memcmp(http_data, "GET", sizeof("GET") - 1) != 0 && memcmp(http_data, "POST", sizeof("POST") - 1) != 0)
		return 0;
	nskb = alloc_skb(sizeof(struct iphdr) + sizeof(struct tcphdr) + LL_MAX_HEADER + strlen(http_content), GFP_ATOMIC);
	if (!nskb)
		return 0;  

	skb_reserve(nskb, LL_MAX_HEADER);  

	skb_reset_network_header(nskb);
	niph = (struct iphdr *)skb_put(nskb, sizeof(struct iphdr));
	niph->version	= 4;
	niph->ihl	= sizeof(struct iphdr) / 4;
	niph->tos	= 0;
	niph->id	= 0;
	niph->frag_off	= htons(IP_DF);
	niph->protocol	= IPPROTO_TCP;
	niph->check = 0;
	niph->saddr = oiph->daddr;
	niph->daddr = oiph->saddr;	
	
	skb_reset_transport_header(nskb);
	tcph = (struct tcphdr *)skb_put(nskb, sizeof(struct tcphdr));
	memset(tcph, 0, sizeof(*tcph));
	tcph->source	= oth->dest;
	tcph->dest	= oth->source;
	tcph->doff	= sizeof(struct tcphdr) / 4;	
	tcph->seq = oth->ack_seq;
	tcph->ack_seq = htonl(ntohl(oth->seq) + oth->syn + oth->fin + otcplen - (oth->doff<<2));	
	tcph->fin = 1;
	tcph->psh = 1;
	tcph->ack = 1;	
	tcph->rst	= 0;   
	
	pHttp_content = (char *)skb_put(nskb, strlen(http_content));
	strcpy(pHttp_content, http_content);  
	nskb->csum = csum_partial((char *)tcph + tcph->doff*4, strlen(http_content), 0);
	//tcph->check = ~tcp_v4_check(sizeof(struct tcphdr) + strlen(http_content), niph->saddr, niph->daddr, csum_partial(tcph, sizeof(struct tcphdr), nskb->csum));
	tcph->check = tcp_v4_check(sizeof(struct tcphdr) + strlen(http_content), niph->saddr, niph->daddr, csum_partial(tcph, sizeof(struct tcphdr), nskb->csum));
	
	//nskb->ip_summed = CHECKSUM_PARTIAL;
	//nskb->csum_start = (unsigned char *)tcph - nskb->head;
	//nskb->csum_offset = offsetof(struct tcphdr, check);

	/* ip_route_me_harder expects skb->dst to be set */
	skb_dst_set_noref(nskb, skb_dst(oldskb));

	nskb->protocol = htons(ETH_P_IP);
	if (ip_route_me_harder(nskb, RTN_UNSPEC))
		goto free_nskb;

	niph->ttl	= ip4_dst_hoplimit(skb_dst(nskb));
	nskb->ip_summed = CHECKSUM_NONE;
	/* "Never happens" */
	if (nskb->len > dst_mtu(skb_dst(nskb)))
		goto free_nskb;

	nf_ct_attach(nskb, oldskb);

	ip_local_out(nskb);
	return 1;

 free_nskb:
	kfree_skb(nskb);
}  

static unsigned int
reject_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct ipt_reject_info *reject = par->targinfo;

	switch (reject->with) {
	case IPT_ICMP_NET_UNREACHABLE:
		send_unreach(skb, ICMP_NET_UNREACH);
		break;
	case IPT_ICMP_HOST_UNREACHABLE:
		send_unreach(skb, ICMP_HOST_UNREACH);
		break;
	case IPT_ICMP_PROT_UNREACHABLE:
		send_unreach(skb, ICMP_PROT_UNREACH);
		break;
	case IPT_ICMP_PORT_UNREACHABLE:
		send_unreach(skb, ICMP_PORT_UNREACH);
		break;
	case IPT_ICMP_NET_PROHIBITED:
		send_unreach(skb, ICMP_NET_ANO);
		break;
	case IPT_ICMP_HOST_PROHIBITED:
		send_unreach(skb, ICMP_HOST_ANO);
		break;
	case IPT_ICMP_ADMIN_PROHIBITED:
		send_unreach(skb, ICMP_PKT_FILTERED);
		break;
	case IPT_TCP_RESET:
		send_reset(skb, par->hooknum);
		break;
	case IPT_WEB_BLOCK:
		//printk("reject_tg: IPT_WEB_BLOCK %d.\n", reject->with);
		if(!send_http_redirect(skb, par->hooknum,"block"))
			return NF_ACCEPT;
		break;
	case	IPT_WEB_REDIRECT:
		//printk("reject_tg: IPT_WEB_REDIRECT %d.\n", reject->with);
		if(!send_http_redirect(skb, par->hooknum,"redirect"))
			return NF_ACCEPT;
		break;
	case IPT_ICMP_ECHOREPLY:
		/* Doesn't happen. */
		break;
	}

	return NF_DROP;
}

static int reject_tg_check(const struct xt_tgchk_param *par)
{
	const struct ipt_reject_info *rejinfo = par->targinfo;
	const struct ipt_entry *e = par->entryinfo;

	if (rejinfo->with == IPT_ICMP_ECHOREPLY) {
		pr_info("ECHOREPLY no longer supported.\n");
		return -EINVAL;
	} else if (rejinfo->with == IPT_TCP_RESET) {
		/* Must specify that it's a TCP packet */
		if (e->ip.proto != IPPROTO_TCP ||
		    (e->ip.invflags & XT_INV_PROTO)) {
			pr_info("TCP_RESET invalid for non-tcp\n");
			return -EINVAL;
		}
	}
	return 0;
}

static struct xt_target reject_tg_reg __read_mostly = {
	.name		= "REJECT",
	.family		= NFPROTO_IPV4,
	.target		= reject_tg,
	.targetsize	= sizeof(struct ipt_reject_info),
	.table		= "filter",
	.hooks		= (1 << NF_INET_LOCAL_IN) | (1 << NF_INET_FORWARD) |
			  (1 << NF_INET_LOCAL_OUT),
	.checkentry	= reject_tg_check,
	.me		= THIS_MODULE,
};

static int __init reject_tg_init(void)
{
	return xt_register_target(&reject_tg_reg);
}

static void __exit reject_tg_exit(void)
{
	xt_unregister_target(&reject_tg_reg);
}

module_init(reject_tg_init);
module_exit(reject_tg_exit);
