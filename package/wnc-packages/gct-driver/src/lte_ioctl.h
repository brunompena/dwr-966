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

#ifndef _LTE_IOCTL_H_
#define _LTE_IOCTL_H_

#define SIOCLTEIOCTL	SIOCDEVPRIVATE
#define SIOCG_DATA	0x8D10
#define SIOCS_DATA	0x8D11

#ifndef __packed
#define __packed __attribute__((packed))
#endif

enum {
	LINK_ON,
	LINK_OFF,
	GET_NETWORK_STATICS,
	RX_STOP,
	RX_RESUME,
	GET_DRV_VER,
	GET_SDIO_DEVICE_STATUS,
	GET_ENDIAN_INFO,
};

struct dev_endian_t {
	unsigned char dev_endian;
	unsigned char host_endian;
} __packed;

struct net_stat_t {
	unsigned long tx_packets;
	unsigned long rx_packets;
	unsigned long tx_bytes;
	unsigned long rx_bytes;
	unsigned long tx_dropped;
	unsigned long rx_dropped;
} __packed;

struct net_info_t {
	unsigned int nic_index;
	struct net_stat_t stats;
} __packed;

struct data_t {
	long len;
	void *buf;
} __packed;

struct wm_req_t {
	union {
		char ifrn_name[IFNAMSIZ];
	} ifr_ifrn;
	unsigned short cmd;
	unsigned short data_id;
	struct data_t data;
} __packed;

#ifndef ifr_name
#define ifr_name	(ifr_ifrn.ifrm_name)
#endif

#endif /* _LTE_IOCTL_H_ */
