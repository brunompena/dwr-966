/*
 * Copyright (c) 2012 GCT Semiconductor, Inc. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/version.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/uaccess.h>
#include <net/ndisc.h>

#include <linux/ctype.h>

#include "gdm_lte.h"
#include "netlink_k.h"
#include "hci.h"
#include "hci_packet.h"
#include "gdm_endian.h"
#include "lte_ioctl.h"

//////////////////////////////////
#define GCTNET_PPA_DP
//#define GCTNET_NOOFFSET
//#define GCTNET_EXTRAOFFSET 32
//////////////////////////////////

//////////////////////////////////
#ifdef GCTNET_PPA_DP
	#include <linux/mii.h>
	#include <linux/usb.h>
	#include <linux/usb/usbnet.h>

	#include <net/ifx_ppa_api.h>
	#include <net/ifx_ppa_hook.h>
	#include <net/ifx_ppa_api_directpath.h>
#endif //GCTNET_PPA_DP
//////////////////////////////////

//////////////////////////////////
#ifdef GCTNET_PPA_DP
	static int ppa_startup_hook = 1;
	module_param (ppa_startup_hook, int, 0);
	MODULE_PARM_DESC (ppa_startup_hook, "0: don't hook ppa at startup, other: do hook");

	static int bridge_mode = 0;
	static int bridge_mode_running = 0;
	module_param (bridge_mode, int, 0);
	MODULE_PARM_DESC (bridge_mode, "0: full NAT mode other: single NAT mode");

	static char *bridge_mode_mac_addr;
	module_param(bridge_mode_mac_addr, charp, S_IRUGO);
	MODULE_PARM_DESC(bridge_mode_mac_addr, "MAC addr for single NAT mode");

	unsigned char snat_mac[6]={0,0,0,0,0,0},snat_mac_good=0;

	static u8 getnibble(unsigned char c)
	{
		if (likely(isdigit(c)))
			return c - '0';
		c = toupper(c);
		if (likely(isxdigit(c)))
			return 10 + c - 'A';
		return 0;
	}


	void check_snat_mac(const char *str)
	{
		if (str) {
			unsigned	i;

			for (i = 0; i < 6; i++) {
				unsigned char num;

				if ((*str == '.') || (*str == ':'))
					str++;
				num = getnibble(*str++) << 4;
				num |= (getnibble(*str++));
				snat_mac [i] = num;
			}
			if (is_valid_ether_addr(snat_mac))
			{
				snat_mac_good=1;
				return ;
			}
		}
		snat_mac_good=0;
		snat_mac[0]=
		snat_mac[1]=
		snat_mac[2]=
		snat_mac[3]=
		snat_mac[4]=
		snat_mac[5]=0;
		return ;
	}

	PPA_DIRECTPATH_CB gctnet_ppadp_cb;
	int32_t gctnet_ppadp_pause_rx(PPA_NETIF *dev);
	int32_t gctnet_ppadp_resume_rx(PPA_NETIF *dev);
	int32_t gctnet_ppadp_start_xmit(PPA_NETIF *rxif, PPA_NETIF *txif, PPA_BUF *skb, int32_t len);

	int     gctnet_ppadp_occupied=-1;
	int     gctnet_ppadp_running=0;
	static int ppadp_enable=-1;
	module_param (ppadp_enable, int, 1);
	MODULE_PARM_DESC (ppadp_enable, "Override default ppadp_enable");


	static void register_ppadp(struct net_device *net)
	{
		struct nic		*dev = netdev_priv(net);
		if(dev->gctnet_ppadp_on && !gctnet_ppadp_running && ppa_hook_directpath_register_dev_fn )
		{
			uint32_t status;
			uint32_t if_id;
			gctnet_ppadp_cb.stop_tx_fn =gctnet_ppadp_pause_rx;
			gctnet_ppadp_cb.start_tx_fn=gctnet_ppadp_resume_rx;
			gctnet_ppadp_cb.rx_fn      =gctnet_ppadp_start_xmit;
#ifdef PPA_F_DIRECTPATH_WAN
			bridge_mode_running=0;
			if(   bridge_mode && snat_mac_good)
			{
				status=ppa_hook_directpath_register_dev_fn(
						&if_id,net,
						&gctnet_ppadp_cb,
						PPA_F_DIRECTPATH_REGISTER
						| PPA_F_DIRECTPATH_ETH_IF
//						| PPA_F_DIRECTPATH_WAN
					);
				if(status==IFX_SUCCESS)
					bridge_mode_running=1;
			}
			else
			{
				status=ppa_hook_directpath_register_dev_fn(
						&if_id,net,
						&gctnet_ppadp_cb,
						PPA_F_DIRECTPATH_REGISTER
						| PPA_F_DIRECTPATH_ETH_IF
						| PPA_F_DIRECTPATH_WAN
						);
			}
#else
			status=ppa_hook_directpath_register_dev_fn(
					&if_id,net,
					&gctnet_ppadp_cb,
					PPA_F_DIRECTPATH_REGISTER
					| PPA_F_DIRECTPATH_ETH_IF
				);
#endif

			if(status==IFX_SUCCESS)
				gctnet_ppadp_running=1;
		}
		if(!ppa_hook_directpath_register_dev_fn)
			gctnet_ppadp_running=0;
	}

	int32_t gctnet_ppadp_pause_rx(PPA_NETIF *dev)
	{
		//drv_print(LTE_LOG_FUNCTION, "+gctnet_ppadp_pause_rx");
		if(!netif_queue_stopped(dev))
		{
			//drv_print(LTE_LOG_FUNCTION, "NETWORK STOP");
			netif_stop_queue(dev);
		}
		//drv_print(LTE_LOG_FUNCTION, "-gctnet_ppadp_pause_rx");
		return IFX_SUCCESS;
	}

	int32_t gctnet_ppadp_resume_rx(PPA_NETIF *dev)
	{
		//drv_print(LTE_LOG_FUNCTION, "+gctnet_ppadp_resume_rx");
		if(netif_queue_stopped(dev))
		{
			//drv_print(LTE_LOG_FUNCTION, "NETWORK RESUME");
			netif_wake_queue(dev);
		}
		//drv_print(LTE_LOG_FUNCTION, "-gctnet_ppadp_resume_rx");
		return IFX_SUCCESS;
	}


	int32_t gctnet_ppadp_start_xmit(PPA_NETIF *rxif, PPA_NETIF *txif, PPA_BUF *skb, int32_t len)
	{
		int pktlen;

		//drv_print(LTE_LOG_FUNCTION, "+gctnet_ppadp_start_xmit");

		if(rxif) {
			//printk("River: rxif");
			if(netif_running(rxif)) {
				pktlen = skb->len;
				skb->dev = rxif;
				skb->protocol = eth_type_trans(skb, rxif);
				if(netif_rx(skb) == NET_RX_DROP)
					rxif->stats.rx_dropped++;
				else {
					rxif->stats.rx_packets++;
					rxif->stats.rx_bytes += pktlen;
				}
				//drv_print(LTE_LOG_FUNCTION, "-gctnet_ppadp_start_xmit");
				return 0;
			}
		}
		else if(txif) {
			//printk("River: txif");
			skb->dev = txif;
			dev_queue_xmit(skb);
			txif->stats.tx_dropped++;
			//drv_print(LTE_LOG_FUNCTION, "-gctnet_ppadp_start_xmit");
			return 0;
		}
		dev_kfree_skb_any(skb);

		//drv_print(LTE_LOG_FUNCTION, "-gctnet_ppadp_start_xmit");
		return 0;
	}

	static ssize_t show_ppa_dp(struct device *dev, struct device_attribute *attr, char *buf)
	{
		struct net_device *netdev = to_net_dev(dev);
		struct nic     *udev = netdev_priv(netdev);

		return sprintf(buf, "itf_ppadp_on:%d ppadp_running:%d ppadp_occupied:%d\n",
			udev->gctnet_ppadp_on,
			gctnet_ppadp_running,
			gctnet_ppadp_occupied);
	}

	static ssize_t store_ppa_dp(struct device *dev, struct device_attribute *attr, char *buf, size_t len)
	{
		struct net_device *netdev = to_net_dev(dev);
		struct nic     *udev = netdev_priv(netdev);
		char *endp;
		unsigned long target;

		target = simple_strtoul(buf, &endp, 0);
		if (endp == buf)
			return -EBADMSG;
		if     (target==0)
		{
			if(!ppa_hook_directpath_register_dev_fn)
				gctnet_ppadp_running =0;
			if(udev->gctnet_ppadp_on)
			{
				if(gctnet_ppadp_running)
				{
					uint32_t if_id;
					if(ppa_hook_get_ifid_for_netif_fn)
					{
						if_id=ppa_hook_get_ifid_for_netif_fn(netdev);
						if(ppa_hook_directpath_rx_stop_fn)
							ppa_hook_directpath_rx_stop_fn(if_id,0);
					}
					if(ppa_hook_directpath_register_dev_fn)
						ppa_hook_directpath_register_dev_fn(&if_id,netdev, &gctnet_ppadp_cb, 0);
					gctnet_ppadp_running =0;
				}
				udev->gctnet_ppadp_on=0;
				gctnet_ppadp_occupied=0;
			}
			return len;
		}
		else if(target==1)
		{
			if(ppadp_enable &&
			   gctnet_ppadp_occupied==0 &&
			   udev->gctnet_ppadp_on==0)
			{
				udev->gctnet_ppadp_on=1;
				gctnet_ppadp_occupied=1;
				register_ppadp(netdev);
			}
			return len;
		}
		else
			return -EBADMSG;
	}

	static struct device_attribute ppa_dp_attrs[] = {
		__ATTR(ppa_dp, S_IRUGO|S_IWUSR, show_ppa_dp, store_ppa_dp),
	};

	static ssize_t show_bridge_mode_mac_addr(struct device *dev, struct device_attribute *attr, char *buf)
	{
		return sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X   %s\n"
				,snat_mac[0],snat_mac[1],snat_mac[2],snat_mac[3],snat_mac[4],snat_mac[5]
				,snat_mac_good?"Valid":"Invalid" );
	}

	static ssize_t store_bridge_mode_mac_addr(struct device *dev, struct device_attribute *attr, char *buf, size_t len)
	{
		check_snat_mac(buf);
		if(!snat_mac_good) bridge_mode_running = 0;
		return len;
	}

	static ssize_t show_bridge_mode(struct device *dev, struct device_attribute *attr, char *buf)
	{
		return sprintf(buf, "bridge_mode:%d running:%d  snat_mac_good:%d\n"
			,bridge_mode
			,bridge_mode_running
			,snat_mac_good);
	}

	static ssize_t store_bridge_mode(struct device *dev, struct device_attribute *attr, char *buf, size_t len)
	{
		char *endp;
		unsigned long target;
		target = simple_strtoul(buf, &endp, 0);
		if (endp == buf)
			return -EBADMSG;
		if     (target==0)
		{
			// TBD
			bridge_mode=0;
			bridge_mode_running=0;
		}
		else
			bridge_mode=1;
		return len;
	}

	static struct device_attribute bridge_mode_attrs[] = {
		__ATTR(bridge_mode_mac_addr, S_IRUGO|S_IWUSR, show_bridge_mode_mac_addr, store_bridge_mode_mac_addr),
		__ATTR(bridge_mode, S_IRUGO|S_IWUSR, show_bridge_mode, store_bridge_mode),
	};

#endif //GCTNET_PPA_DP
/////////////////////////////////////////////////////////////////////////////////////


/*
 * Emulate ARP and NDP NS
 */
#define EMULATE_ARP
#define EMULATE_NDP

/*
 * Netlink protocol number
 */
#define NETLINK_LTE 30

/*
 * Default MTU Size
 */
#define DEFAULT_MTU_SIZE 1500

#define gdm_dev_endian(n) (\
	n->phy_dev->get_endian(n->phy_dev->priv_dev))

#define gdm_lte_hci_send(n, d, l) (\
	n->phy_dev->send_hci_func(n->phy_dev->priv_dev, d, l, NULL, NULL))

#define gdm_lte_sdu_send(n, d, l, c, b, i, t) (\
	n->phy_dev->send_sdu_func(n->phy_dev->priv_dev, d, l, n->pdn_table.dft_eps_id, 0, c, b, i, t))

#define gdm_lte_rcv_with_cb(n, c, b, e) (\
	n->rcv_func(n->priv_dev, c, b, e))

#define IP_VERSION_4	4
#define IP_VERSION_6	6

static struct {
	int ref_cnt;
	struct sock *sock;
} lte_event;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
static struct device_type wwan_type = {
	.name   = "wwan",
};
#endif


static int gdm_lte_open(struct net_device *dev)
{
	struct nic *pNic = netdev_priv(dev);

	#ifdef GCTNET_PPA_DP
		register_ppadp(dev);
	#endif
	
	netif_start_queue(dev);
	
	#ifdef GCTNET_PPA_DP
		if(pNic->gctnet_ppadp_on && gctnet_ppadp_running)
			if(ppa_hook_directpath_rx_restart_fn && ppa_hook_get_ifid_for_netif_fn)
				ppa_hook_directpath_rx_restart_fn(ppa_hook_get_ifid_for_netif_fn(dev),0);
	#endif
	
	return 0;
}

static int gdm_lte_close(struct net_device *dev)
{
	netif_stop_queue(dev);
	
#ifdef GCTNET_PPA_DP
{
	struct nic *nic = netdev_priv(dev);

	if(!ppa_hook_directpath_register_dev_fn)
	{
		gctnet_ppadp_running =0;
	}
	if(nic->gctnet_ppadp_on)
	{
		if( ppa_hook_directpath_rx_stop_fn &&
		    ppa_hook_get_ifid_for_netif_fn)
			ppa_hook_directpath_rx_stop_fn(ppa_hook_get_ifid_for_netif_fn(dev),0);
		if(ppa_hook_directpath_register_dev_fn)
		{
			uint32_t if_id;
			ppa_hook_directpath_register_dev_fn(&if_id,dev, &gctnet_ppadp_cb, 0);
		}
		gctnet_ppadp_running =0;
	}
}
#endif

	return 0;
}

static int gdm_lte_set_config(struct net_device *dev, struct ifmap *map)
{
	if (dev->flags & IFF_UP)
		return -EBUSY;
	return 0;
}

static void tx_complete(void *arg)
{
	struct nic *nic = arg;

	if (nic == NULL) {
		printk(KERN_ERR "glte: tx complete - invalid nic\n");
		return;
	}

	if (netif_queue_stopped(nic->netdev))
	{
		#ifdef GCTNET_PPA_DP
			if(nic->gctnet_ppadp_on && gctnet_ppadp_running)
				if(ppa_hook_directpath_rx_restart_fn && ppa_hook_get_ifid_for_netif_fn)
					ppa_hook_directpath_rx_restart_fn(ppa_hook_get_ifid_for_netif_fn(nic->netdev),0);
		#endif
		
		netif_wake_queue(nic->netdev);
	}
}

static int gdm_lte_rx(struct sk_buff *skb, struct nic *nic, int nic_type)
{
	int ret;

	if (nic == NULL) {
		printk(KERN_ERR "glte: rx - invalid nic\n");
		return -1;
	}

#ifdef LTE_USB
	ret = netif_rx_ni(skb);
#else
	ret = netif_rx(skb);
#endif /* LTE_USB */

	if (ret == NET_RX_DROP) {
		printk(KERN_ERR "glte: rx - dropped\n");
		nic->stats.rx_dropped++;
	} else {
		nic->stats.rx_packets++;
		nic->stats.rx_bytes += skb->len + ETH_HLEN;
	}

	return 0;
}

#ifdef EMULATE_ARP
int gdm_lte_emulate_arp(struct sk_buff *skb_in, u32 nic_type)
{
	struct nic *nic = netdev_priv(skb_in->dev);
	struct sk_buff *skb_out;
	struct ethhdr eth;
	struct vlan_ethhdr vlan_eth;
	struct arphdr *arp_in;
	struct arphdr *arp_out;
	struct arpdata {
		u8 ar_sha[ETH_ALEN];
		u8 ar_sip[4];
		u8 ar_tha[ETH_ALEN];
		u8 ar_tip[4];
	};
	struct arpdata *arp_data_in;
	struct arpdata *arp_data_out;
	u8 arp_temp[60];
	void *mac_header_data;
	u32 mac_header_len;

	/* Format the mac header so that it can be put to skb */
	if (ntohs(((struct ethhdr *)skb_in->data)->h_proto) == ETH_P_8021Q) {
		memcpy(&vlan_eth, skb_in->data, sizeof(struct vlan_ethhdr));
		mac_header_data = &vlan_eth;
		mac_header_len = VLAN_ETH_HLEN;
	} else {
		memcpy(&eth, skb_in->data, sizeof(struct ethhdr));
		mac_header_data = &eth;
		mac_header_len = ETH_HLEN;
	}

	/* Get the pointer of the original request */
	arp_in = (struct arphdr *)(skb_in->data + mac_header_len);
	arp_data_in = (struct arpdata *)(skb_in->data + mac_header_len + sizeof(struct arphdr));

	/* Get the pointer of the outgoing response */
	arp_out = (struct arphdr *)arp_temp;
	arp_data_out = (struct arpdata *)(arp_temp + sizeof(struct arphdr));

	/* Copy the arp header */
	memcpy(arp_out, arp_in, sizeof(struct arphdr));
	arp_out->ar_op = htons(ARPOP_REPLY);

	/* Copy the arp payload: based on 2 bytes of mac and fill the IP */
	arp_data_out->ar_sha[0] = arp_data_in->ar_sha[0];
	arp_data_out->ar_sha[1] = arp_data_in->ar_sha[1];
	memcpy(&arp_data_out->ar_sha[2], &arp_data_in->ar_tip[0], 4);
	memcpy(&arp_data_out->ar_sip[0], &arp_data_in->ar_tip[0], 4);
	memcpy(&arp_data_out->ar_tha[0], &arp_data_in->ar_sha[0], 6);
	memcpy(&arp_data_out->ar_tip[0], &arp_data_in->ar_sip[0], 4);

	/* Fill the destination mac with source mac of the received packet */
	memcpy(mac_header_data, mac_header_data + ETH_ALEN, ETH_ALEN);
	/* Fill the source mac with nic's source mac */
	memcpy(mac_header_data + ETH_ALEN, nic->src_mac_addr, ETH_ALEN);

	/* Alloc skb and reserve align */
	skb_out = dev_alloc_skb(skb_in->len);
	if (!skb_out)
		return -1;
	skb_reserve(skb_out, NET_IP_ALIGN);

	memcpy(skb_put(skb_out, mac_header_len), mac_header_data, mac_header_len);
	memcpy(skb_put(skb_out, sizeof(struct arphdr)), arp_out, sizeof(struct arphdr));
	memcpy(skb_put(skb_out, sizeof(struct arpdata)), arp_data_out, sizeof(struct arpdata));

	skb_out->protocol = ((struct ethhdr *)mac_header_data)->h_proto;
	skb_out->dev = skb_in->dev;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 21)
	skb_reset_mac_header(skb_out);
#endif
	skb_pull(skb_out, ETH_HLEN);

	gdm_lte_rx(skb_out, nic, nic_type);

	return 0;
}
#endif

#ifdef EMULATE_NDP
int icmp6_checksum(struct ipv6hdr *ipv6, u16 *ptr, int len)
{
	unsigned short *w = ptr;
	int sum = 0;
	int i;

	#pragma pack(1)
	union {
		struct {
			u8 ph_src[16];
			u8 ph_dst[16];
			u32 ph_len;
			u8 ph_zero[3];
			u8 ph_nxt;
		} ph;
		u16 pa[20];
	} pseudo_header;
	#pragma pack()

	memset(&pseudo_header, 0, sizeof(pseudo_header));
	memcpy(&pseudo_header.ph.ph_src, &ipv6->saddr.in6_u.u6_addr8, 16);
	memcpy(&pseudo_header.ph.ph_dst, &ipv6->daddr.in6_u.u6_addr8, 16);
	pseudo_header.ph.ph_len = ipv6->payload_len;
	pseudo_header.ph.ph_nxt = ipv6->nexthdr;

	w = (u16 *)&pseudo_header;
	for (i = 0; i < sizeof(pseudo_header.pa) / sizeof(pseudo_header.pa[0]); i++)
		sum += pseudo_header.pa[i];

	w = ptr;
	while (len > 1) {
		sum += *w++;
		len -= 2;
	}

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	sum = ~sum & 0xffff;

	return sum;
}

int gdm_lte_emulate_ndp(struct sk_buff *skb_in, u32 nic_type)
{
	struct nic *nic = netdev_priv(skb_in->dev);
	struct sk_buff *skb_out;
	struct ethhdr eth;
	struct vlan_ethhdr vlan_eth;
	struct neighbour_advertisement {
		u8 target_address[16];
		u8 type;
		u8 length;
		u8 link_layer_address[6];
	};
	struct neighbour_advertisement na;
	struct neighbour_solicitation {
		u8 target_address[16];
	};
	struct neighbour_solicitation *ns;
	struct ipv6hdr *ipv6_in;
	struct ipv6hdr ipv6_out;
	struct icmp6hdr *icmp6_in;
	struct icmp6hdr icmp6_out;

	void *mac_header_data;
	u32 mac_header_len;

	/* Format the mac header so that it can be put to skb */
	if (ntohs(((struct ethhdr *)skb_in->data)->h_proto) == ETH_P_8021Q) {
		memcpy(&vlan_eth, skb_in->data, sizeof(struct vlan_ethhdr));
		if (ntohs(vlan_eth.h_vlan_encapsulated_proto) != ETH_P_IPV6)
			return -1;
		mac_header_data = &vlan_eth;
		mac_header_len = VLAN_ETH_HLEN;
	} else {
		memcpy(&eth, skb_in->data, sizeof(struct ethhdr));
		if (ntohs(eth.h_proto) != ETH_P_IPV6)
			return -1;
		mac_header_data = &eth;
		mac_header_len = ETH_HLEN;
	}


	/* Check if this is IPv6 ICMP packet */
	ipv6_in = (struct ipv6hdr *)(skb_in->data + mac_header_len);
	if (ipv6_in->version != 6 || ipv6_in->nexthdr != IPPROTO_ICMPV6)
		return -1;

	/* Check if this is NDP packet */
	icmp6_in = (struct icmp6hdr *)(skb_in->data + mac_header_len + sizeof(struct ipv6hdr));
	if (icmp6_in->icmp6_type == NDISC_ROUTER_SOLICITATION) { /* Check RS */
		return -1;
	} else if (icmp6_in->icmp6_type == NDISC_NEIGHBOUR_SOLICITATION) { /* Check NS */
		u8 icmp_na[sizeof(struct icmp6hdr) + sizeof(struct neighbour_advertisement)];
		u8 zero_addr8[16] = {0,};

		if (memcmp(ipv6_in->saddr.in6_u.u6_addr8, zero_addr8, 16) == 0)
			/* Duplicate Address Detection: Source IP is all zero */
			return 0;

		icmp6_out.icmp6_type = NDISC_NEIGHBOUR_ADVERTISEMENT;
		icmp6_out.icmp6_code = 0;
		icmp6_out.icmp6_cksum = 0;
		icmp6_out.icmp6_dataun.un_data32[0] = htonl(0x60000000); /* R=0, S=1, O=1 */

		ns = (struct neighbour_solicitation *)(skb_in->data + mac_header_len + sizeof(struct ipv6hdr) + sizeof(struct icmp6hdr));
		memcpy(&na.target_address, ns->target_address, 16);
		na.type = 0x02;
		na.length = 1;
		na.link_layer_address[0] = 0x00;
		na.link_layer_address[1] = 0x0a;
		na.link_layer_address[2] = 0x3b;
		na.link_layer_address[3] = 0xaf;
		na.link_layer_address[4] = 0x63;
		na.link_layer_address[5] = 0xc7;

		memcpy(&ipv6_out, ipv6_in, sizeof(struct ipv6hdr));
		memcpy(ipv6_out.saddr.in6_u.u6_addr8, &na.target_address, 16);
		memcpy(ipv6_out.daddr.in6_u.u6_addr8, ipv6_in->saddr.in6_u.u6_addr8, 16);
		ipv6_out.payload_len = htons(sizeof(struct icmp6hdr) + sizeof(struct neighbour_advertisement));

		memcpy(icmp_na, &icmp6_out, sizeof(struct icmp6hdr));
		memcpy(icmp_na + sizeof(struct icmp6hdr), &na, sizeof(struct neighbour_advertisement));

		icmp6_out.icmp6_cksum = icmp6_checksum(&ipv6_out, (u16 *)icmp_na, sizeof(icmp_na));
	} else {
		return -1;
	}

	/* Fill the destination mac with source mac of the received packet */
	memcpy(mac_header_data, mac_header_data + ETH_ALEN, ETH_ALEN);
	/* Fill the source mac with nic's source mac */
	memcpy(mac_header_data + ETH_ALEN, nic->src_mac_addr, ETH_ALEN);

	/* Alloc skb and reserve align */
	skb_out = dev_alloc_skb(skb_in->len);
	if (!skb_out)
		return -1;
	skb_reserve(skb_out, NET_IP_ALIGN);

	memcpy(skb_put(skb_out, mac_header_len), mac_header_data, mac_header_len);
	memcpy(skb_put(skb_out, sizeof(struct ipv6hdr)), &ipv6_out, sizeof(struct ipv6hdr));
	memcpy(skb_put(skb_out, sizeof(struct icmp6hdr)), &icmp6_out, sizeof(struct icmp6hdr));
	memcpy(skb_put(skb_out, sizeof(struct neighbour_advertisement)), &na, sizeof(struct neighbour_advertisement));

	skb_out->protocol = ((struct ethhdr *)mac_header_data)->h_proto;
	skb_out->dev = skb_in->dev;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 21)
	skb_reset_mac_header(skb_out);
#endif

#ifdef GCTNET_PPA_DP
	if( nic->gctnet_ppadp_on)
	{
		uint32_t	fpstatus;
		register_ppadp(skb_in->dev);
		if( gctnet_ppadp_running &&
			ppa_hook_directpath_send_fn &&
			ppa_hook_get_ifid_for_netif_fn)
		{
			skb_out->mac_header = skb_out->data;
#if 1
			if(skb_out->len==1470 || skb_out->len==1472)
				skb_put(skb_out, 4);
#endif
			fpstatus = ppa_hook_directpath_send_fn(ppa_hook_get_ifid_for_netif_fn(nic->netdev), (PPA_BUF *)skb_out, skb_out->len, 0);
			//if(fpstatus)
				//drv_print(LTE_LOG_FUNCTION, "netif_rx status %d", fpstatus);
			return 0;
		}
	}
#endif //GCTNET_PPA_DP

	skb_pull(skb_out, ETH_HLEN);

	gdm_lte_rx(skb_out, nic, nic_type);

	return 0;
}
#endif

static s32 gdm_lte_tx_nic_type(struct net_device *dev, struct sk_buff *skb)
{
	struct nic *nic = netdev_priv(dev);
	struct ethhdr *eth;
	struct vlan_ethhdr *vlan_eth;
	struct iphdr *ip;
	struct ipv6hdr *ipv6;
	int mac_proto;
	void *network_data;
	u32 nic_type = 0;

	if (nic == NULL)
		return -1;

	if (nic->nic_id >= MAX_NIC_TYPE)
		return -1;

	/* NIC TYPE is based on the nic_id of this net_device */
	nic_type = 0x00000010 | nic->nic_id;

	/* Get ethernet protocol */
	eth = (struct ethhdr *)skb->data;
	if (ntohs(eth->h_proto) == ETH_P_8021Q) {
		vlan_eth = (struct vlan_ethhdr *)skb->data;
		mac_proto = ntohs(vlan_eth->h_vlan_encapsulated_proto);
		network_data = skb->data + VLAN_ETH_HLEN;
		nic_type |= NIC_TYPE_F_VLAN;
	} else {
		mac_proto = ntohs(eth->h_proto);
		network_data = skb->data + ETH_HLEN;
	}

	/* Process packet for nic type */
	switch (mac_proto) {
	case ETH_P_ARP:
		nic_type |= NIC_TYPE_ARP;
		break;
	case ETH_P_IP:
		nic_type |= NIC_TYPE_F_IPV4;
		ip = (struct iphdr *)network_data;

		/* Check DHCPv4 */
		if (ip->protocol == IPPROTO_UDP) {
			struct udphdr *udp = (struct udphdr *)(network_data + sizeof(struct iphdr));
			if (ntohs(udp->dest) == 67 || ntohs(udp->dest) == 68)
				nic_type |= NIC_TYPE_F_DHCP;
		}
		break;
	case ETH_P_IPV6:
		nic_type |= NIC_TYPE_F_IPV6;
		ipv6 = (struct ipv6hdr *)network_data;

		if (ipv6->nexthdr == IPPROTO_ICMPV6) /* Check NDP request */ {
			struct icmp6hdr *icmp6 = (struct icmp6hdr *)(network_data + sizeof(struct ipv6hdr));
			if (/*icmp6->icmp6_type == NDISC_ROUTER_SOLICITATION || */
				icmp6->icmp6_type == NDISC_NEIGHBOUR_SOLICITATION)
				nic_type |= NIC_TYPE_ICMPV6;
		} else if (ipv6->nexthdr == IPPROTO_UDP) /* Check DHCPv6 */ {
			struct udphdr *udp = (struct udphdr *)(network_data + sizeof(struct ipv6hdr));
			if (ntohs(udp->dest) == 546 || ntohs(udp->dest) == 547)
				nic_type |= NIC_TYPE_F_DHCP;
		}
		break;
	default:
		return -1;
		break;
	}

	return nic_type;
}

static int gdm_lte_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct nic *nic = netdev_priv(dev);
	u32 nic_type;
	void *data_buf;
	int data_len;
	int idx;
	int ret = 0;

	nic_type = gdm_lte_tx_nic_type(dev, skb);
	if (nic_type == 0) {
		printk(KERN_ERR "glte: tx - invalid nic_type\n");
		return -1;
	}

#ifdef EMULATE_ARP
	if (nic_type & NIC_TYPE_ARP) {
		if (gdm_lte_emulate_arp(skb, nic_type) == 0) {
			dev_kfree_skb(skb);
			return 0;
		}
	}
#endif

#ifdef EMULATE_NDP
	if (nic_type & NIC_TYPE_ICMPV6) {
		if (gdm_lte_emulate_ndp(skb, nic_type) == 0) {
			dev_kfree_skb(skb);
			return 0;
		}
	}
#endif

	/*
	Need byte shift (that is, remove VLAN tag) if there is one
	For the case of ARP, this breaks the offset as vlan_ethhdr+4 is treated as ethhdr
	However, it shouldn't be a problem as the reponse starts from arp_hdr and ethhdr
	is created by this driver based on the NIC mac
	*/
	if (nic_type & NIC_TYPE_F_VLAN) {
		struct vlan_ethhdr *vlan_eth = (struct vlan_ethhdr *)skb->data;
		nic->vlan_id = ntohs(vlan_eth->h_vlan_TCI) & VLAN_VID_MASK;
		data_buf = skb->data + (VLAN_ETH_HLEN - ETH_HLEN);
		data_len = skb->len - (VLAN_ETH_HLEN - ETH_HLEN);
	} else {
		nic->vlan_id = 0;
		data_buf = skb->data;
		data_len = skb->len;
	}

	/* If it is a ICMPV6 packet, clear all the other bits : for backward compatibilty with the firmware */
	if (nic_type & NIC_TYPE_ICMPV6)
		nic_type = NIC_TYPE_ICMPV6;

	/* If it is not a dhcp packet, clear all the flag bits : original NIC, otherwise the special flag (IPVX | DHCP) */
	if (!(nic_type & NIC_TYPE_F_DHCP))
		nic_type &= NIC_TYPE_MASK;

	sscanf(dev->name, "usb%d", &idx);

	ret = gdm_lte_sdu_send(nic,
			       data_buf,
			       data_len,
			       tx_complete,
			       nic,
			       idx,
			       nic_type);

	if (ret == TX_NO_BUFFER || ret == TX_NO_SPC) {

#ifdef GCTNET_PPA_DP
	if(nic->gctnet_ppadp_on)
		register_ppadp(dev);

		if(nic->gctnet_ppadp_on && gctnet_ppadp_running)
			if(ppa_hook_directpath_rx_stop_fn && ppa_hook_get_ifid_for_netif_fn)
				ppa_hook_directpath_rx_stop_fn(ppa_hook_get_ifid_for_netif_fn(dev),0);
#endif //GCTNET_PPA_DP

		netif_stop_queue(dev);
		if (ret == TX_NO_BUFFER)
			ret = 0;
		else
			ret = -ENOSPC;
	} else if (ret == TX_NO_DEV) {
		ret = -ENODEV;
	}

	/* Updates tx stats */
	if (ret) {
		printk(KERN_ERR "glte: tx - sending sdu failed\n");
		nic->stats.tx_dropped++;
	} else {
		nic->stats.tx_packets++;
		nic->stats.tx_bytes += data_len;
	}
	dev_kfree_skb(skb);

	return 0;
}

static struct net_device_stats *gdm_lte_stats(struct net_device *dev)
{
	struct nic *nic = netdev_priv(dev);
	return &nic->stats;
}

static void get_network_statics(struct data_t *data, struct net_device *dev)
{
	struct nic *nic = netdev_priv(dev);
	struct phy_dev *pdev = nic->phy_dev;
	unsigned char *buf;
	struct net_info_t *net_stats;
	struct net_info_t *net_stats_requested = (struct net_info_t *)data->buf;
	int i;

	buf = kmalloc(sizeof(*net_stats), GFP_KERNEL);
	if (!buf)
		return;

	memset(buf, 0, sizeof(*net_stats));

	net_stats = (struct net_info_t *)buf;
	net_stats->nic_index = net_stats_requested->nic_index;

	if (net_stats->nic_index > MAX_NIC_TYPE) {
		printk(KERN_INFO "glte: stats - invalid nic_index\n");
		kfree(buf);
		return;
	}

	if (net_stats->nic_index == MAX_NIC_TYPE) {
		for (i = 0; i < MAX_NIC_TYPE; i++) {
			nic = netdev_priv(pdev->dev[i]);
			net_stats->stats.tx_packets += nic->stats.tx_packets;
			net_stats->stats.tx_bytes += nic->stats.tx_bytes;
			net_stats->stats.tx_dropped += nic->stats.tx_dropped;
			net_stats->stats.rx_packets += nic->stats.rx_packets;
			net_stats->stats.rx_bytes += nic->stats.rx_bytes;
			net_stats->stats.rx_dropped += nic->stats.rx_dropped;
		}
	} else {
		nic = netdev_priv(pdev->dev[net_stats->nic_index]);
		net_stats->stats.tx_packets = nic->stats.tx_packets;
		net_stats->stats.tx_bytes = nic->stats.tx_bytes;
		net_stats->stats.tx_dropped = nic->stats.tx_dropped;
		net_stats->stats.rx_packets = nic->stats.rx_packets;
		net_stats->stats.rx_bytes = nic->stats.rx_bytes;
		net_stats->stats.rx_dropped = nic->stats.rx_dropped;
	}

	copy_to_user(data->buf, buf, sizeof(*net_stats));
	kfree(buf);
}

static int get_ver(u8 *src, u8 *dst)
{
	u8 tmp_buf[4] = {0,};
	int i = 0;

	while (1) {
		if (dst[i] < '0' || dst[i] > '9')
			break;

		tmp_buf[i] = dst[i] % 0x30;
		i++;
	}

	if (i == 1)
		*src = tmp_buf[0];
	else if (i == 2)
		*src = (tmp_buf[0] * 10) + tmp_buf[1];
	else if (i == 3)
		*src = (tmp_buf[0] * 100) + (tmp_buf[1] * 10) + tmp_buf[3];

	return i;
}

static void get_driver_version(struct data_t *data, struct net_device *dev)
{
	u8 ver[16] = {0,};
	u8 drv_ver[4] = {0,};
	int i = 0;
	int j = 0;

	memcpy(ver, DRIVER_VERSION, sizeof(DRIVER_VERSION));

	while (i < sizeof(DRIVER_VERSION)) {
		if (ver[i] != '.') {
			i += get_ver(&drv_ver[j], &ver[i]);
			j++;
		} else {
			i++;
		}

		if (j == 4)
			break;
	}

	copy_to_user(data->buf, drv_ver, sizeof(drv_ver));
}

static void get_dev_endian(struct data_t *data, struct net_device *dev)
{
	struct nic *nic = netdev_priv(dev);
	copy_to_user(data->buf, gdm_dev_endian(nic), sizeof(struct dev_endian_t));
}

static int gdm_lte_ioctl_get_data(struct wm_req_t *req, struct net_device *dev)
{
	u16 id = req->data_id;

	switch (id) {
	case GET_NETWORK_STATICS:
		get_network_statics(&req->data, dev);
		break;
	case GET_DRV_VER:
		get_driver_version(&req->data, dev);
		break;
	case GET_ENDIAN_INFO:
		get_dev_endian(&req->data, dev);
		break;
	default:
		printk(KERN_ERR "glte: ioctl - unknown type %d\n", id);
		break;
	}
	return 0;
}

static int gdm_lte_ioctl_set_data(struct wm_req_t *req, struct net_device *dev)
{
	u16 id = req->data_id;

	switch (id) {
	case LINK_ON:
		netif_carrier_on(dev);
		break;
	case LINK_OFF:
		netif_carrier_off(dev);
		break;
	}

	return 0;
}

static int gdm_lte_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct wm_req_t *req = (struct wm_req_t *)ifr;

	if (cmd != SIOCLTEIOCTL)
		return -EOPNOTSUPP;

	switch (req->cmd) {
	case SIOCG_DATA:
	case SIOCS_DATA:
		if (req->data_id >= 100)
			return -EOPNOTSUPP;
		if (req->cmd == SIOCG_DATA)
			gdm_lte_ioctl_get_data(req, dev);
		else if (req->cmd == SIOCS_DATA)
			gdm_lte_ioctl_set_data(req, dev);
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

static int gdm_lte_event_send(struct net_device *dev, char *buf, int len)
{
	struct nic *nic = netdev_priv(dev);
	struct hci_packet *hci = (struct hci_packet *)buf;
	int idx;

#ifdef DEBUG
	printk(KERN_DEBUG "glte: D=>H: 0x%04x(%d) len %d\n",
	       D2H(gdm_dev_endian(nic), hci->cmd_evt),
	       D2H(gdm_dev_endian(nic), hci->len),
	       len);
#endif

	sscanf(dev->name, "usb%d", &idx);

	return netlink_send(lte_event.sock, 0, 0, buf,
			    D2H(gdm_dev_endian(nic), hci->len) + HCI_HEADER_SIZE);
}

static void gdm_lte_event_rcv(struct net_device *dev, u16 type, void *msg, int len)
{
	struct nic *nic = netdev_priv(dev);

#ifdef DEBUG
	struct hci_packet *hci = (struct hci_packet *)msg;
	printk(KERN_DEBUG, "glte: H=>D: 0x%04x(%d) len %d\n",
	       D2H(gdm_dev_endian(nic), hci->cmd_evt),
	       D2H(gdm_dev_endian(nic), hci->len),
	       len);
#endif

	gdm_lte_hci_send(nic, msg, len);
}

int gdm_lte_event_init(void)
{
	if (lte_event.ref_cnt == 0)
		lte_event.sock = netlink_init(NETLINK_LTE, gdm_lte_event_rcv);

	if (lte_event.sock) {
		lte_event.ref_cnt++;
		return 0;
	}

	printk(KERN_ERR "glte: event init failed\n");
	return -1;
}

void gdm_lte_event_exit(void)
{
	if (lte_event.sock && --lte_event.ref_cnt == 0) {
		netlink_exit(lte_event.sock);
		lte_event.sock = NULL;
	}
}

static int find_dev_index(u32 nic_type)
{
	int index;

	index = nic_type & 0x0000000f;
	if (index > MAX_NIC_TYPE || index < 0)
		index = 0;

	return index;
}

static void gdm_lte_netif_rx(struct net_device *dev, char *buf, int len, int flagged_nic_type)
{
	u32 nic_type;
	struct nic *nic;
	struct sk_buff *skb;
	struct ethhdr eth;
	struct vlan_ethhdr vlan_eth;
	void *mac_header_data;
	u32 mac_header_len;
	char ip_version = 0;

	nic_type = flagged_nic_type & NIC_TYPE_MASK;
	nic = netdev_priv(dev);
	if (nic == NULL) {
		printk(KERN_ERR "glte: rx, invalid nic\n");
		return;
	}
	if (flagged_nic_type & NIC_TYPE_F_DHCP) {
		/* Change the destination mac address with the one requested the IP */
		if (flagged_nic_type & NIC_TYPE_F_IPV4) {
			struct dhcp_packet {
				u8 op;      /* BOOTREQUEST or BOOTREPLY */
				u8 htype;   /* hardware address type. 1 = 10mb ethernet */
				u8 hlen;    /* hardware address length */
				u8 hops;    /* used by relay agents only */
				u32 xid;    /* unique id */
				u16 secs;   /* elapsed since client began acquisition/renewal */
				u16 flags;  /* only one flag so far: */
				#define BROADCAST_FLAG 0x8000 /* "I need broadcast replies" */
				u32 ciaddr; /* client IP (if client is in BOUND, RENEW or REBINDING state) */
				u32 yiaddr; /* 'your' (client) IP address */
				/* IP address of next server to use in bootstrap, returned in DHCPOFFER, DHCPACK by server */
				u32 siaddr_nip;
				u32 gateway_nip; /* relay agent IP address */
				u8 chaddr[16];   /* link-layer client hardware address (MAC) */
				u8 sname[64];    /* server host name (ASCIZ) */
				u8 file[128];    /* boot file name (ASCIZ) */
				u32 cookie;      /* fixed first four option bytes (99,130,83,99 dec) */
			} __packed;

			#define DHCP_V4_HW_ADDR_OFFSET	(\
				sizeof(struct iphdr) + \
				sizeof(struct udphdr) + \
				offsetof(struct dhcp_packet, chaddr))
			#define DHCP_V4_IP_ADDR_OFFSET (\
				sizeof(struct iphdr) + \
				sizeof(struct udphdr) + \
				offsetof(struct dhcp_packet, yiaddr))

			memcpy(nic->dest_mac_addr, buf + DHCP_V4_HW_ADDR_OFFSET, 6);
		} else if (flagged_nic_type & NIC_TYPE_F_IPV6) {
			printk(KERN_INFO "glte: DHCPv6 not supported yet\n");
		}
	}

	if (nic->vlan_id > 0) {
		mac_header_data = (void *)&vlan_eth;
		mac_header_len = VLAN_ETH_HLEN;
	} else {
		mac_header_data = (void *)&eth;
		mac_header_len = ETH_HLEN;
	}

	/* Format the data so that it can be put to skb */
	memcpy(mac_header_data, nic->dest_mac_addr, ETH_ALEN);
	memcpy(mac_header_data + ETH_ALEN, nic->src_mac_addr, ETH_ALEN);

	vlan_eth.h_vlan_TCI = htons(nic->vlan_id);
	vlan_eth.h_vlan_proto = htons(ETH_P_8021Q);

	if (nic_type == NIC_TYPE_ARP) {
		/* Should be response: Only happens because there was a request from the host */
		eth.h_proto = htons(ETH_P_ARP);
		vlan_eth.h_vlan_encapsulated_proto = htons(ETH_P_ARP);
	} else {
		ip_version = buf[0] >> 4;
		if (ip_version == IP_VERSION_4) {
			eth.h_proto = htons(ETH_P_IP);
			vlan_eth.h_vlan_encapsulated_proto = htons(ETH_P_IP);
		} else if (ip_version == IP_VERSION_6) {
			eth.h_proto = htons(ETH_P_IPV6);
			vlan_eth.h_vlan_encapsulated_proto = htons(ETH_P_IPV6);
		} else {
			printk(KERN_ERR "glte: Unknown IP version %d\n", ip_version);
			return;
		}
	}

	/* Alloc skb and reserve align */
	skb = dev_alloc_skb(len + mac_header_len + NET_IP_ALIGN);
	if (!skb)
		return;
	skb_reserve(skb, NET_IP_ALIGN);

	memcpy(skb_put(skb, mac_header_len), mac_header_data, mac_header_len);
	memcpy(skb_put(skb, len), buf, len);

	skb->protocol = ((struct ethhdr *)mac_header_data)->h_proto;
	skb->dev = dev;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 21)
	skb_reset_mac_header(skb);
#endif

#ifdef GCTNET_PPA_DP
	if( nic->gctnet_ppadp_on)
	{
		uint32_t	fpstatus;
		register_ppadp(dev);
		if( gctnet_ppadp_running &&
			ppa_hook_directpath_send_fn &&
			ppa_hook_get_ifid_for_netif_fn)
		{
			skb->mac_header = skb->data;
#if 1
			if(skb->len==1470 || skb->len==1472)
				skb_put(skb, 4);
#endif
			fpstatus = ppa_hook_directpath_send_fn(ppa_hook_get_ifid_for_netif_fn(dev), (PPA_BUF *)skb, skb->len, 0);
			//if(fpstatus)
				//drv_print(LTE_LOG_FUNCTION, "netif_rx status %d", fpstatus);
			return;
		}
	}
#endif //GCTNET_PPA_DP

	skb_pull(skb, ETH_HLEN);

	gdm_lte_rx(skb, nic, nic_type);
}

static void gdm_lte_multi_sdu_pkt(struct phy_dev *phy_dev, char *buf, int len)
{
	struct net_device *dev;
	struct multi_sdu *multi_sdu = (struct multi_sdu *)buf;
	struct sdu *sdu = NULL;
	u8 *data = (u8 *)multi_sdu->data;
	u16 i = 0;
	u16 num_packet;
	u16 hci_len;
	u16 cmd_evt;
	u32 nic_type;
	int index;

	hci_len = D2H(phy_dev->get_endian(phy_dev->priv_dev), multi_sdu->len);
	num_packet = D2H(phy_dev->get_endian(phy_dev->priv_dev), multi_sdu->num_packet);

#ifdef DEBUG
	printk(KERN_DEBUG "glte: rx sdu len %d, num_packet %d\n",
	       multi_sdu->len, multi_sdu->num_packet);
#endif

	for (i = 0; i < num_packet; i++) {
		sdu = (struct sdu *)data;

		cmd_evt = D2H(phy_dev->get_endian(phy_dev->priv_dev), sdu->cmd_evt);
		hci_len = D2H(phy_dev->get_endian(phy_dev->priv_dev), sdu->len);
		nic_type = D4H(phy_dev->get_endian(phy_dev->priv_dev), sdu->nic_type);

		if (cmd_evt != LTE_RX_SDU) {
			printk(KERN_ERR "glte: rx sdu wrong hci %04x\n", cmd_evt);
			return;
		}
		if (hci_len < 12) {
			printk(KERN_ERR "glte: rx sdu invalid len %d\n", hci_len);
			return;
		}

		index = find_dev_index(nic_type);
		if (index >= 0 && index < MAX_NIC_TYPE) {
			dev = phy_dev->dev[index];
			gdm_lte_netif_rx(dev, (char *)sdu->data, (int)(hci_len-12), nic_type);
		} else {
			printk(KERN_ERR "glte: rx sdu invalid nic_type : %x\n", nic_type);
		}

		data += ((hci_len+3) & 0xfffc) + HCI_HEADER_SIZE;
	}
}

static void gdm_lte_pdn_table(struct net_device *dev, char *buf, int len)
{
	struct nic *nic = netdev_priv(dev);
	struct hci_pdn_table_ind *pdn_table = (struct hci_pdn_table_ind *)buf;

	if (pdn_table->activate) {
		nic->pdn_table.activate = pdn_table->activate;
		nic->pdn_table.dft_eps_id = D4H(gdm_dev_endian(nic), pdn_table->dft_eps_id);
		nic->pdn_table.nic_type = D4H(gdm_dev_endian(nic), pdn_table->nic_type);

		printk(KERN_INFO "glte: pdn %s activated, nic_type=0x%x\n",
		       dev->name, nic->pdn_table.nic_type);
	} else {
		memset(&nic->pdn_table, 0x00, sizeof(struct pdn_table));
		printk(KERN_INFO "glte: pdn %s deactivated\n",
		       dev->name);
	}
}

static int gdm_lte_receive_pkt(struct phy_dev *phy_dev, char *buf, int len)
{
	struct hci_packet *hci = (struct hci_packet *)buf;
	struct hci_pdn_table_ind *pdn_table = (struct hci_pdn_table_ind *)buf;
	struct sdu *sdu;
	struct net_device *dev;
	int ret = 0;
	u16 cmd_evt;
	u32 nic_type;
	int index;

	if (!len)
		return ret;

	cmd_evt = D2H(phy_dev->get_endian(phy_dev->priv_dev), hci->cmd_evt);

	dev = phy_dev->dev[0];
	if (dev == NULL)
		return 0;

	switch (cmd_evt) {
	case LTE_RX_SDU:
		sdu = (struct sdu *)hci->data;
		nic_type = D4H(phy_dev->get_endian(phy_dev->priv_dev), sdu->nic_type);
		index = find_dev_index(nic_type);
		if (index >= 0) {
			dev = phy_dev->dev[index];
			gdm_lte_netif_rx(dev, hci->data, len, nic_type);
		}
		break;
	case LTE_RX_MULTI_SDU:
		gdm_lte_multi_sdu_pkt(phy_dev, buf, len);
		break;
	case LTE_LINK_ON_OFF_INDICATION:
		{
			struct hci_connect_ind *connect_ind = (struct hci_connect_ind *)buf;
			printk(KERN_INFO "glte: link %s\n",
			       connect_ind->connect ? "on" : "off");
		}
		break;
	case LTE_PDN_TABLE_IND:
		pdn_table = (struct hci_pdn_table_ind *)buf;
		nic_type = D4H(phy_dev->get_endian(phy_dev->priv_dev), pdn_table->nic_type);
		index = find_dev_index(nic_type);
		if (index >= 0) {
			dev = phy_dev->dev[index];
			gdm_lte_pdn_table(dev, buf, len);
		}
		/* Fall through */
	default:
		ret = gdm_lte_event_send(dev, buf, len);
		break;
	}

	return ret;
}

static int rx_complete(void *arg, void *data, int len, int context)
{
	struct phy_dev *phy_dev = (struct phy_dev *)arg;

	return gdm_lte_receive_pkt(phy_dev, (char *)data, len);
}

void start_rx_proc(struct phy_dev *phy_dev)
{
#ifdef LTE_USB
	int i;

	for (i = 0; i < MAX_RX_SUBMIT_COUNT; i++)
		gdm_lte_rcv_with_cb(phy_dev, rx_complete, phy_dev, USB_COMPLETE);
#else
	gdm_lte_rcv_with_cb(phy_dev, rx_complete, phy_dev, KERNEL_THREAD);
#endif /* LTE_USB */
}

#ifdef HAVE_NET_DEVICE_OPS
static struct net_device_ops gdm_netdev_ops = {
	.ndo_open			= gdm_lte_open,
	.ndo_stop			= gdm_lte_close,
	.ndo_set_config			= gdm_lte_set_config,
	.ndo_start_xmit			= gdm_lte_tx,
	.ndo_get_stats			= gdm_lte_stats,
	.ndo_do_ioctl			= gdm_lte_ioctl,
};
#endif

static u8 gdm_lte_macaddr[6] = {0x00, 0x0a, 0x3b, 0x00, 0x00, 0x00};

static void form_mac_address(u8 *dev_addr, u8 *nic_src, u8 *nic_dest, u8 *mac_address, u8 index)
{
	/* Form the dev_addr */
	if (!mac_address)
		memcpy(dev_addr, gdm_lte_macaddr, 6);
	else
		memcpy(dev_addr, mac_address, 6);

	/* The last byte of the mac address should be less than or equal to 0xFC */
	dev_addr[5] = (u8) (dev_addr[5] + index);

	/* Create random nic src and copy the first 3 bytes to be the same as dev_addr */
	random_ether_addr(nic_src);
	memcpy(nic_src, dev_addr, 3);

	/* Copy the nic_dest from dev_addr*/
	memcpy(nic_dest,  dev_addr, 6);
}

static void mac_validation(u8 *mac_address)
{
	u8 invalid_mac = 0;

	/* the mac address is not 00-00-00-00-00-00 */
	if (
		(mac_address[0] == 0x00) &&
		(mac_address[1] == 0x00) &&
		(mac_address[2] == 0x00) &&
		(mac_address[3] == 0x00) &&
		(mac_address[4] == 0x00) &&
		(mac_address[5] == 0x00))
		invalid_mac = 1;
	/* check for multicast bit */
	else if ((mac_address[0] & 0x01) == 0x01)
		invalid_mac = 1;

	if (invalid_mac) {
		/* Restore the default value */
		printk(KERN_ERR "glte: MAC invalid, restoring default\n");
		memcpy(mac_address, gdm_lte_macaddr, 6);
	} else {
		printk(KERN_INFO "glte: MAC validated\n");
	}
}

int register_lte_device(struct phy_dev *phy_dev, struct device *dev, u8 *mac_address)
{
	struct nic *nic;
	struct net_device *net;
	char pdn_dev_name[16];
	int ret = 0;
	u8 index;

	mac_validation(mac_address);

	for (index = 0; index < MAX_NIC_TYPE; index++) {
		/* Create device name lteXpdnX */
		sprintf(pdn_dev_name, "usb%d", index);

		/* Allocate netdev */
		net =  alloc_netdev(sizeof(struct nic), pdn_dev_name, ether_setup);
		if (net == NULL) {
			printk(KERN_ERR "glte: alloc_netdev failed\n");
			ret = -1;
			break;
		}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
		SET_MODULE_OWNER(net);
#endif

		/* Configure device */
#ifdef HAVE_NET_DEVICE_OPS
		net->netdev_ops = &gdm_netdev_ops;
#else
		net->open = gdm_lte_open;
		net->stop = gdm_lte_close;
		net->set_config = gdm_lte_set_config;
		net->hard_start_xmit = gdm_lte_tx;
		net->get_stats = gdm_lte_stats;
		net->do_ioctl = gdm_lte_ioctl;
#endif
		net->flags &= ~IFF_MULTICAST;
		net->mtu = DEFAULT_MTU_SIZE;

		/* Configure nic */
		nic = netdev_priv(net);
		memset(nic, 0, sizeof(struct nic));
		nic->netdev = net;
		nic->phy_dev = phy_dev;
		nic->nic_id = index;
#ifdef GCTNET_PPA_DP
		nic->gctnet_ppadp_on = 0;
#endif //GCTNET_PPA_DP

		form_mac_address(
				net->dev_addr,
				nic->src_mac_addr,
				nic->dest_mac_addr,
				mac_address,
				index);

		SET_NETDEV_DEV(net, dev);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 32)
		SET_NETDEV_DEVTYPE(net, &wwan_type);
#endif

		ret = register_netdev(net);
		if (ret) {
			printk(KERN_ERR "glte: register %s failed\n", net->name);
			free_netdev(net);
			break;
		}

		netif_carrier_on(net);
		
#ifdef GCTNET_PPA_DP
	{
		int i;
		int err;
		for (i = 0; i < ARRAY_SIZE(ppa_dp_attrs); i++)
			err = device_create_file(&net->dev,&ppa_dp_attrs[i]);
		for (i = 0; i < ARRAY_SIZE(bridge_mode_attrs); i++)
			err = device_create_file(&net->dev,&bridge_mode_attrs[i]);
	}
#endif //GCTNET_PPA_DP

		phy_dev->dev[index] = net;

#ifdef GCTNET_PPA_DP
	{
		if(ppa_startup_hook && gctnet_ppadp_occupied==-1)
		{
			nic->gctnet_ppadp_on =1;
			gctnet_ppadp_occupied=1;
		}
	}
#endif // GCTNET_PPA_DP

		printk(KERN_INFO "glte: register %s\n", net->name);
	}

	return 0;
}

void unregister_lte_device(struct phy_dev *phy_dev)
{
	struct net_device *net;
	int index;

	for (index = MAX_NIC_TYPE-1; index >= 0; index--) {
		net = phy_dev->dev[index];
		if (net == NULL)
			continue;

#ifdef GCTNET_PPA_DP
	{
		int i;
		for (i = 0; i < ARRAY_SIZE(ppa_dp_attrs); i++)
			device_remove_file(&net->dev, &ppa_dp_attrs[i]);
	}
	gctnet_ppadp_occupied=(ppa_startup_hook)? -1:0;
	gctnet_ppadp_running=0;

	{
		int i;
		for (i = 0; i < ARRAY_SIZE(bridge_mode_attrs); i++)
			  device_remove_file(&net->dev, &bridge_mode_attrs[i]);
	}
	bridge_mode_running=0;
#endif //GCTNET_PPA_DP

		printk(KERN_INFO "glte: unregister %s\n", net->name);

		unregister_netdev(net);
		free_netdev(net);
	}
}
