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

#ifndef _GDM_SDIO_H_
#define _GDM_SDIO_H_

#include <linux/workqueue.h>
#include <linux/spinlock.h>

#include "hci_packet.h"
#include "gdm_endian.h"

struct tx_cxt {
	struct list_head sdu_list;
	struct list_head hci_list;
	struct list_head free_list;
	u8 *send_buf;
	u32 avail_count;
	u8 avail_send_buf;
#ifdef TX_DELAY
	unsigned long sdu_stamp;
#endif /* TX_DELAY */
	spinlock_t lock;
};

struct sdio_tx {
	struct list_head list;
	u8 *buf;
	int len;
	void (*callback)(void *cb_data);
	void *cb_data;
};

struct rx_cxt {
	struct list_head rx_list;
	u8 *rx_buf;
	int rx_len;
	spinlock_t lock;
};

struct sdio_rx {
	struct list_head list;
	u8 *buf;
	int len;
};

struct lte_sdev {
	struct sdio_func *func;
	struct tx_cxt tx;
	struct rx_cxt rx;
	int (*callback)(void *cb_data, void *data, int len, int context);
	void *cb_data;
#ifdef SDIO_MODEM_PORT
	int (*tty_callback)(void *data, int len, int tty_index, int context, int complete);
	int tty_minor[2];
#endif /* SDIO_MODEM_PORT */
#ifdef SDIO_DEVICE_STATUS_CHECK
	void (*status_cb)(void *cb_data, void *data);
	void *status_cb_data;
#endif /* SDIO_DEVICE_STATUS_CHECK */
	struct delayed_work ws;
	struct delayed_work irq_ws;
	struct loopback loopback;
	u32 loopback_seq;
	u8 tx_stop;
	u8 request_mac_addr;
#ifdef TX_DELAY
	u8 set_delay_queue;
#endif /* TX_DELAY */
	struct gdm_endian gdm_ed;
};

#endif /* _GDM_SDIO_H_ */
