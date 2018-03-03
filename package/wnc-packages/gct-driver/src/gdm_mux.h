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

#ifndef _GDM_MUX_H_
#define _GDM_MUX_H_

#include <linux/types.h>
#include <linux/usb.h>
#include <linux/list.h>

#define H2L(x)		__cpu_to_le16(x)
#define L2H(x)		__le16_to_cpu(x)
#define H4L(x)		__cpu_to_le32(x)
#define L4H(x)		__le32_to_cpu(x)

struct mux_pkt_header {
	unsigned int start_flag;
	unsigned int seq_num;
	unsigned int payload_size;
	unsigned short packet_type;
};

struct mux_tx {
	struct urb *urb;
	u8 *buf;
	int  len;
	void (*callback)(void *cb_data);
	void *cb_data;
};

struct mux_rx {
	struct list_head free_list;
	struct list_head rx_submit_list;
	struct list_head to_host_list;
	struct urb *urb;
	u8 *buf;
	void *mux_dev;
	u32 offset;
	u32 len;
	int (*callback)(void *data, int len, int tty_index, int minor, int complete);
};

struct rx_cxt {
	struct list_head to_host_list;
	struct list_head rx_submit_list;
	struct list_head rx_free_list;
	spinlock_t to_host_lock;
	spinlock_t submit_list_lock;
	spinlock_t free_list_lock;
};

struct mux_dev {
	struct usb_device *usbdev;
	struct usb_interface *control_intf;
	struct usb_interface *data_intf;
	struct rx_cxt	rx;
	struct delayed_work work_rx;
#ifdef LTE_USB_PM
	struct usb_interface *intf;
	int usb_state;
	int (*rx_cb)(void *data, int len, int tty_index, int minor, int complete);
#endif /* LTE_USB_PM */
	spinlock_t write_lock;
	u8 minor[2];
};

#endif /* _GDM_MUX_H_ */
