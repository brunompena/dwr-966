/******************************************************************************
**
** FILE NAME    : swa_stack_al.c
** PROJECT      : PPA
** MODULES      : Software Acceleration Stack Adaption Layer (Linux)
**
** DATE         : 13 MAR 2014
** AUTHOR       : Lantiq Deutschland GmbH
** DESCRIPTION  : Software Acceleration Stack Adaption Layer (Linux)
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
*******************************************************************************/

/*
 *  Common Head File
 */
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
#include <linux/autoconf.h>
#else
#include <generated/autoconf.h>
#endif

#include <linux/ip.h>
#include <linux/swap.h>
#include <linux/ipv6.h>
#include <net/ip.h>
#include <net/netfilter/nf_conntrack.h>
/*
 *  PPA Specific Head File
 */
#include <net/ppa_api.h>
#include <net/ppa_stack_al.h>
#include "ppa_api_session.h"
#include "ppa_api_netif.h"

void *swa_malloc(uint32_t size)
{
    gfp_t flags;

    if ( in_atomic() || in_interrupt() )
        flags = GFP_ATOMIC;
    else
        flags = GFP_KERNEL;
    return kmalloc(size, flags);
}
EXPORT_SYMBOL(swa_malloc);

int32_t swa_free(void *buf)
{
    kfree(buf);
    return PPA_SUCCESS;
}
EXPORT_SYMBOL(swa_free);

void swa_memset(void *dst, uint32_t pad, uint32_t n)
{
    memset(dst, pad, n);
}
EXPORT_SYMBOL(swa_memset);

int swa_skb_mark(void *skb)
{
	return ((struct sk_buff *)skb)->mark;
}
EXPORT_SYMBOL(swa_skb_mark);

void swa_skb_update_mark(void *skb1,int flg)
{
	struct sk_buff *skb;
	skb = (struct sk_buff *)skb1;
	skb->mark &= ~flg;
}
EXPORT_SYMBOL(swa_skb_update_mark);

unsigned char *swa_skb_head(void *skb)
{
	return ((struct sk_buff *)skb)->head;
}
EXPORT_SYMBOL(swa_skb_head);

unsigned char *swa_skb_pull(void *skb,unsigned int len)
{
	return skb_pull((struct sk_buff *)skb,len);
}
EXPORT_SYMBOL(swa_skb_pull);

unsigned char *swa_skb_data(void *skb)
{
	return ((struct sk_buff *)skb)->data;
}
EXPORT_SYMBOL(swa_skb_data);

unsigned char *swa_skb_push(void *skb,unsigned int len)
{
	return skb_push((struct sk_buff *)skb,len);
}
EXPORT_SYMBOL(swa_skb_push);

unsigned int swa_skb_headroom(void *skb)
{
	return skb_headroom((struct sk_buff *)skb);
}
EXPORT_SYMBOL(swa_skb_headroom);

struct sk_buff *swa_skb_realloc_headroom(void *skb, unsigned int len)
{
	return skb_realloc_headroom(((struct sk_buff *)skb),len);
}
EXPORT_SYMBOL(swa_skb_realloc_headroom);

void swa_nf_conntrack_put(void *sess)
{
	struct nf_conn *ip_sess;
	ip_sess = (struct nf_conn *)sess;
	nf_conntrack_put(&ip_sess->ct_general);
}
EXPORT_SYMBOL(swa_nf_conntrack_put);

void SWA_SKB_FREE(void *skb)
{
	PPA_SKB_FREE((struct sk_buff *)skb);
}
EXPORT_SYMBOL(SWA_SKB_FREE);

struct sock *swa_skb_sk(void *skb)
{
	return (((struct sk_buff *)skb)->sk);
}
EXPORT_SYMBOL(swa_skb_sk);

void swa_skb_set_owner_w(void *skb,void *sk)
{
	skb_set_owner_w((struct sk_buff *)skb,(struct sock *)sk);
}
EXPORT_SYMBOL(swa_skb_set_owner_w);

void swa_skb_set_mac_header(void *skb, int offset)
{
	skb_set_mac_header((struct sk_buff *)skb,offset);
}
EXPORT_SYMBOL(swa_skb_set_mac_header);

void swa_skb_set_network_header(void *skb, int offset)
{
	skb_set_network_header((struct sk_buff *)skb,offset);
}
EXPORT_SYMBOL(swa_skb_set_network_header);

void swa_skb_set_transport_header(void *skb, int offset)
{
	skb_set_transport_header((struct sk_buff *)skb,offset);
}
EXPORT_SYMBOL(swa_skb_set_transport_header);

unsigned char *swa_skb_network_header(void *skb)
{
	return skb_network_header((struct sk_buff *)skb);
}
EXPORT_SYMBOL(swa_skb_network_header);

__sum16 swa_ip_fast_csum(void *iph, unsigned int len)
{
	return ip_fast_csum((unsigned char *)iph,len);
}
EXPORT_SYMBOL(swa_ip_fast_csum);

unsigned char *swa_skb_transport_header(void *skb)
{
	return skb_transport_header((struct sk_buff *)skb);
}
EXPORT_SYMBOL(swa_skb_transport_header);

struct net_device *swa_get_netif(void *ifinfo)
{
	struct netif_info *tx_ifinfo;
	tx_ifinfo = (struct netif_info *)ifinfo;
	return ppa_get_netif(tx_ifinfo->phys_netif_name);
}
EXPORT_SYMBOL(swa_get_netif);

unsigned int swa_get_flags(void *ifinfo)
{	
	struct netif_info *tx_ifinfo;
	tx_ifinfo = (struct netif_info *)ifinfo;
	return tx_ifinfo->flags;
}
EXPORT_SYMBOL(swa_get_flags);

int swa_get_br_dst_port(void *netif, void *skb, void **p_netif)
{
	return ppa_get_br_dst_port((struct net_device *)netif,(struct sk_buff *)skb,(struct net_device *)p_netif);
}
EXPORT_SYMBOL(swa_get_br_dst_port);

void swa_memcpy_data(void *dst, void *src, uint32_t offset)
{
	struct sk_buff *skb;
	skb = (struct sk_buff *)src;
	ppa_memcpy(dst,skb->data,offset);
}
EXPORT_SYMBOL(swa_memcpy_data);

void swa_memcpy(void *dst, void *src, uint32_t offset)
{
	ppa_memcpy(dst,src,offset);
}
EXPORT_SYMBOL(swa_memcpy);

int swa_get_netif_hwaddr(void *tx_if, unsigned char *hdr, uint32_t i)
{
	return ppa_get_netif_hwaddr((struct net_device *)tx_if,hdr,i);
}
EXPORT_SYMBOL(swa_get_netif_hwaddr);

int swa_get_conntrack_from_skb(void *skb, void **sess, int *reply)
{
	struct sk_buff *skb1;
	struct nf_conn *p_sess;
	int ret;
	skb1 = (struct sk_buff *)skb;
	p_sess = (struct nf_conn *)sess;
	ret = get_conntrack_from_skb(skb1,p_sess,reply);
	sess = (void *)p_sess;
	return ret;
}
EXPORT_SYMBOL(swa_get_conntrack_from_skb);

extern int32_t __ppa_lookup_session(PPA_SESSION *p_session, uint32_t is_reply, struct session_list_item **pp_item);
int __swa_lookup_session(void *sess, int reply, void **pitem)
{
	struct session_item_list *p_item;
	int ret;
	struct nf_conn *p_sess;
	p_item = (struct session_list_item *)pitem;
	p_sess = (struct nf_conn *)sess;
	ret = __ppa_lookup_session(p_sess, reply, p_item);
	return ret;
}
EXPORT_SYMBOL(__swa_lookup_session);

unsigned int swa_skb_len(void *skb)
{
	struct sk_buff *skb1;
	skb1 = (struct sk_buff *)skb;
	return (skb1->len);
}
EXPORT_SYMBOL(swa_skb_len);

void swa_inet_proto_csum_replace4_u(void *hdr, void *skb, __be32 from, __be32 to)
{
	struct udphdr *uhdr;
	uhdr = (struct udphdr *)hdr;
	inet_proto_csum_replace4(&uhdr->check,(struct sk_buff *)skb,from,to,1);
}
EXPORT_SYMBOL(swa_inet_proto_csum_replace4_u);

void swa_skb_dev(void *skb, void *itf)
{
	struct sk_buff *skb1;
	struct net_device *dev;
	skb1 = (struct sk_buff *)skb;
	dev = (struct net_device *)itf;
	skb1->dev = dev;
}
EXPORT_SYMBOL(swa_skb_dev);

void swa_inet_proto_csum_replace4_t(void *hdr, void *skb, __be32 from, __be32 to)
{
	struct tcphdr *thdr;
	thdr = (struct tcphdr *)hdr;
	inet_proto_csum_replace4(&thdr->check,(struct sk_buff *)skb,from,to,1);
}
EXPORT_SYMBOL(swa_inet_proto_csum_replace4_t);

void swa_inet_proto_csum_replace2_u(void *hdr, void *skb, __be16 from, __be16 to)
{
	struct udphdr *uhdr;
	uhdr = (struct udphdr *)hdr;
	inet_proto_csum_replace2(&uhdr->check,(struct sk_buff *)skb,from,to,0);
}
EXPORT_SYMBOL(swa_inet_proto_csum_replace2_u);

void swa_inet_proto_csum_replace2_t(void *hdr, void *skb, __be16 from, __be16 to)
{
	struct tcphdr *thdr;
	thdr = (struct tcphdr *)hdr;
	inet_proto_csum_replace2(&thdr->check,(struct sk_buff *)skb,from,to,0);
}
EXPORT_SYMBOL(swa_inet_proto_csum_replace2_t);

int swa_get_time_in_sec()
{
	return (jiffies + HZ / 2) / HZ;
}
EXPORT_SYMBOL(swa_get_time_in_sec);

int swa_dev_queue_xmit(void *skb)
{
	return dev_queue_xmit((struct sk_buff *)skb);
}
EXPORT_SYMBOL(swa_dev_queue_xmit);

