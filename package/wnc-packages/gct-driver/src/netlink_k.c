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
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
#include <linux/export.h>
#endif
#include <linux/etherdevice.h>
#include <linux/netlink.h>
#include <asm/byteorder.h>
#include <net/sock.h>

#include "netlink_k.h"

#if defined(DEFINE_MUTEX)
static DEFINE_MUTEX(netlink_mutex);
#else
static struct semaphore netlink_mutex;
#define mutex_lock(x)		down(x)
#define mutex_unlock(x)		up(x)
#endif

#define ND_MAX_GROUP		30
#define ND_IFINDEX_LEN		sizeof(int)
#define ND_NLMSG_SPACE(len)	(NLMSG_SPACE(len) + ND_IFINDEX_LEN)
#define ND_NLMSG_DATA(nlh)	((void *)((char *)NLMSG_DATA(nlh) + ND_IFINDEX_LEN))
#define ND_NLMSG_S_LEN(len)	(len+ND_IFINDEX_LEN)
#define ND_NLMSG_R_LEN(nlh)	(nlh->nlmsg_len-ND_IFINDEX_LEN)
#define ND_NLMSG_IFIDX(nlh)	NLMSG_DATA(nlh)
#define ND_MAX_MSG_LEN		(1024 * 32)

static void (*rcv_cb)(struct net_device *dev, u16 type, void *msg, int len);

static void netlink_rcv_cb(struct sk_buff *skb)
{
	struct nlmsghdr	*nlh;
	struct net_device *dev;
	u32 mlen;
	void *msg;
	int ifindex;

	if (!rcv_cb) {
		printk(KERN_ERR "glte: nl cb - unregistered\n");
		return;
	}

	if (skb->len < NLMSG_SPACE(0)) {
		printk(KERN_ERR "glte: nl cb - invalid skb length\n");
		return;
	}

	nlh = (struct nlmsghdr *)skb->data;

	if (skb->len < nlh->nlmsg_len || nlh->nlmsg_len > ND_MAX_MSG_LEN) {
		printk(KERN_ERR "glte: nl cb - invalid length (%d,%d)\n",
		       skb->len, nlh->nlmsg_len);
		return;
	}

	memcpy(&ifindex, ND_NLMSG_IFIDX(nlh), ND_IFINDEX_LEN);
	msg = ND_NLMSG_DATA(nlh);
	mlen = ND_NLMSG_R_LEN(nlh);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
	dev = dev_get_by_index(ifindex);
#else
	dev = dev_get_by_index(&init_net, ifindex);
#endif
	if (dev) {
		rcv_cb(dev, nlh->nlmsg_type, msg, mlen);
		dev_put(dev);
	} else {
		printk(KERN_ERR "glte: nl cb - dev (%d) not found\n", ifindex);
	}

}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
static void netlink_rcv(struct sock *sk, int len)
{
	struct sk_buff_head *list = &sk->sk_receive_queue;
	struct sk_buff *skb;

	mutex_lock(&netlink_mutex);
	while ((skb = skb_dequeue(list)) != NULL) {
		netlink_rcv_cb(skb);
		kfree_skb(skb);
	}
	mutex_unlock(&netlink_mutex);
}
#else
static void netlink_rcv(struct sk_buff *skb)
{
	mutex_lock(&netlink_mutex);
	netlink_rcv_cb(skb);
	mutex_unlock(&netlink_mutex);
}
#endif

struct sock *netlink_init(int unit,
	void (*cb)(struct net_device *dev, u16 type, void *msg, int len))
{
	struct sock *sock;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0)
	struct netlink_kernel_cfg cfg = {
		.input  = netlink_rcv,
	};
#endif

#if !defined(DEFINE_MUTEX)
	init_MUTEX(&netlink_mutex);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)
	sock = netlink_kernel_create(unit, 0, netlink_rcv, THIS_MODULE);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
	sock = netlink_kernel_create(unit, 0, netlink_rcv, NULL, THIS_MODULE);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(3, 6, 0)
	sock = netlink_kernel_create(&init_net, unit, 0, netlink_rcv, NULL,
		THIS_MODULE);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
	sock = netlink_kernel_create(&init_net, unit, THIS_MODULE, &cfg);
#else
	sock = netlink_kernel_create(&init_net, unit, &cfg);
#endif

	if (sock)
		rcv_cb = cb;
	return sock;
}

void netlink_exit(struct sock *sock)
{
	sock_release(sock->sk_socket);
}

int netlink_send(struct sock *sock, int group, u16 type, void *msg, int len)
{
	static u32 seq;
	struct sk_buff *skb = NULL;
	struct nlmsghdr *nlh;
	int ret = 0;

	if (group > ND_MAX_GROUP)
		return -EINVAL;

	if (!netlink_has_listeners(sock, group+1))
		return -ESRCH;

	skb = alloc_skb(NLMSG_SPACE(len), GFP_ATOMIC);
	if (!skb)
		return -ENOMEM;

	seq++;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38)
	nlh = NLMSG_PUT(skb, 0, seq, type, len);
#else
	nlh = nlmsg_put(skb, 0, seq, type, len, 0);
#endif

	memcpy(NLMSG_DATA(nlh), msg, len);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0)
	NETLINK_CB(skb).pid = 0;
#else
	NETLINK_CB(skb).portid = 0;
#endif
	NETLINK_CB(skb).dst_group = 0;

	ret = netlink_broadcast(sock, skb, 0, group+1, GFP_ATOMIC);
	if (!ret)
		return len;

	if (ret != -ESRCH)
		printk(KERN_ERR "glte: nl broadcast g=%d, t=%d, l=%d, r=%d\n",
		       group, type, len, ret);
	else if (netlink_has_listeners(sock, group+1))
		return -EAGAIN;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 38)
nlmsg_failure:
#endif

	return ret;
}
