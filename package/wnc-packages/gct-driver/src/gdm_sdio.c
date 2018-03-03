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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
#error "Kernel version does not support SDIO."
#endif

#include <linux/workqueue.h>

#include <linux/mmc/core.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>

#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/segment.h>

#include "gdm_sdio.h"
#include "gdm_lte.h"
#include "gdm_dbg.h"
#include "gdm_list.h"
#include "gdm_nv_fw.h"
#include "gdm_endian.h"
#include "hci.h"
#include "hci_packet.h"

//#define ENABLE_AUTO_WAKEUP
//#define USE_GPR

#ifdef SDIO_BOOT
#include "sdio_boot.h"
#endif /* SDIO_BOOT */

#ifdef SDIO_MODEM_PORT
#include "gdm_tty.h"

#define AT_COMMAND_INDEX 0
#define DM_MSG_INDEX 1

#define debug_loopback printk

#define DEVICE_ID_7240	0x5347
#define DEVICE_ID_7243	0xD347

struct mux_dev {
	struct phy_dev *phy_dev;
	struct tty_dev *tty_dev;
};
#endif /* SDIO_MODEM_PORT */

typedef struct sdio_ops {
	int (*op_init)(struct sdio_func *func);
	u8 (*op_readb)(struct sdio_func *func, int addr, int *ret);
	void (*op_writeb)(struct sdio_func *func, u8 data, int addr, int *ret);
	int (*op_read)(struct sdio_func *func, void *dst, unsigned int addr, int count);
	int (*op_write)(struct sdio_func *func, unsigned int addr, void *src, int count);
	int (*op_ack_irq_and_get_read_len)(struct sdio_func *func);
} sdio_ops_t;

typedef struct sdio_cb {
	sdio_ops_t ops;

} sdio_cb_t;

static sdio_cb_t sdio_cb;

#define sdio_init			sdio_cb.ops.op_init
#define sdio_read_byte			sdio_cb.ops.op_readb
#define sdio_write_byte			sdio_cb.ops.op_writeb
#define sdio_read			sdio_cb.ops.op_read
#define sdio_write			sdio_cb.ops.op_write
#define sdio_ack_irq_and_get_read_len	sdio_cb.ops.op_ack_irq_and_get_read_len

#define TX_BUF_SIZE		(1024 * 32)
#define RX_BUF_SIZE		(1024 * 32)
#define SDU_BUF_SIZE	2048
#define MAX_SDU_SIZE	(1024 * 30)
#define SDIO_BLOCK_SIZE	512

#define SDIO_FW_START			0x720f

#define GDM7240_DEVICE_READY_REGISTER		0x50
#define GDM7240_DEVICE_READY			0x30
#define GDM7240_HOST_READY_REGISTER		0x48
#define GDM7240_HOST_READY			0xcf
#define GDM7240_AHB_TRANSFER_CNT		0x1c
#define GDM7240_READ_DATA_READY_INT		0x04

#define GDM7243_INT_IND				0x08
#define GDM7243_INT_ENA				0x09
	#define READ_DATA_READY_INT		(1<<0)
#define GDM7243_AHB_TRANSFER_CNT		0x0c
#define GDM7243_HOST_GPR			0x24
	#define HOST_READY			0x48524459
#define GDM7243_DEVICE_GPR			0x28
	#define DEVICE_READY			0x44524459
#define GDM7243_CLK_WKUP_REG			0x18000
	#define AUTO_CLK_EN			(1<<0)
	#define MANUAL_CLK_EN			(1<<1)

#define SDU_PARAM_LEN 12

#ifdef TX_DELAY
#define TX_INTERVAL 15 /* ms */
#define TX_INTERVAL2 1 /* ms */
#endif /* TX_DELAY */

static struct workqueue_struct *sdio_lte_wq;
static struct workqueue_struct *sdio_irq_wq;

static int init_sdio(struct lte_sdev *sdev);
static void release_sdio(struct lte_sdev *sdev);
static int send_sdio_pkt(struct sdio_func *func, u8 *data, int len);

u8 lte_init_finish;
u8 lte_fw_start;

static void sdio_loop_Host2Dev(struct lte_sdev *sdev)
{
	struct sdio_func *func = sdev->func;
	struct tx_cxt *tx = &sdev->tx;
	struct loopback_hci *lp_hci = (struct loopback_hci *)tx->send_buf;
	u16 send_len = sdev->loopback.packet_size + 4;
	u32 i;

	lp_hci->cmd_evt = H2D(&sdev->gdm_ed, LTE_LOOPBACK_TX_PACKET);
	lp_hci->len = H2D(&sdev->gdm_ed, send_len);

	for (i = 0; i < sdev->loopback.loop_count; i++) {
		lp_hci->seq = H4D(&sdev->gdm_ed, sdev->loopback_seq);
		if (send_sdio_pkt(func, tx->send_buf, send_len + HCI_HEADER_SIZE))
			break;
		debug_loopback("Send SEQ[%d]\n", sdev->loopback_seq);
		sdev->loopback_seq++;
	}
	memset(&sdev->loopback , 0x00, sizeof(struct loopback));
	sdev->loopback_seq = 0;
}

static void sdio_loop_bi_direction(struct lte_sdev *sdev, bool is_irq)
{
	struct tx_cxt *tx = &sdev->tx;
	struct rx_cxt *rx = &sdev->rx;
	struct sdio_func *func = sdev->func;
	struct loopback_hci *lp_hci = (struct loopback_hci *)tx->send_buf;
	struct loopback_hci *lp_hci_recv = (struct loopback_hci *)rx->rx_buf;
	u16 send_len = sdev->loopback.packet_size + 4;
	u16 len = 0;
	u32 seq;
	u16 i;
	int ret;

	if (sdev->loopback.loop_count <= 0) {
		seq = D4H(&sdev->gdm_ed, lp_hci_recv->seq);
		if (seq != (sdev->loopback_seq - 1))
			debug_loopback("Loopback Packet Sequnce Error Recv[%d]-Send[%d]\n",
					  seq,
					  sdev->loopback_seq-1);
		else
			debug_loopback("Recv SEQ[%d]\n", seq);

		for (i = 0; i < sdev->loopback.packet_size; i++) {
			if (lp_hci_recv->data[i] != (u8)(i%10))
				debug_loopback("Loopback data error Recv[%d]-Send[%d]\n",
						lp_hci_recv->data[i], i%10);
		}

		memset(&sdev->loopback , 0x00, sizeof(struct loopback));
		sdev->loopback_seq = 0;

		return;
	}

	if (is_irq) {
		seq = D4H(&sdev->gdm_ed, lp_hci_recv->seq);
		if (seq != (sdev->loopback_seq - 1))
			debug_loopback("Loopback Packet Sequnce Error Recv[%d]-Send[%d]\n",
					  seq,
					  sdev->loopback_seq-1);
		else
			debug_loopback("Recv SEQ[%d]\n", seq);

		for (i = 0; i < sdev->loopback.packet_size; i++) {
			if (lp_hci_recv->data[i] != (u8)(i%10))
				debug_loopback("Loopback data error Recv[%d]-Send[%d]\n",
						lp_hci_recv->data[i], i%10);
		}
	}

	lp_hci->cmd_evt = H2D(&sdev->gdm_ed, LTE_LOOPBACK_TX_PACKET);
	lp_hci->len = H2D(&sdev->gdm_ed, send_len);
	lp_hci->seq = H4D(&sdev->gdm_ed, sdev->loopback_seq);

	debug_loopback("Send SEQ[%d]\n", sdev->loopback_seq);
	for (i = 0; i < sdev->loopback.packet_size; i++)
		lp_hci->data[i] = (u8)(i%10);

	len = send_len + HCI_HEADER_SIZE;
	len = ((len + (SDIO_BLOCK_SIZE - 1)) & ~(SDIO_BLOCK_SIZE - 1));

	if (is_irq) {
		ret = sdio_write(func, 0, tx->send_buf, len);
		if (ret < 0) {
#ifdef SDIO_DEVICE_STATUS_CHECK
			int dev_status = 1;
			if (sdev->status_cb)
				sdev->status_cb(sdev->status_cb_data, &dev_status);
#endif /* SDIO_DEVICE_STATUS_CHECK */
			drv_print(LTE_LOG_ERROR, "[sdio_loop_bi_direction] error: ret = %d\n", ret);
		}
	} else {
		send_sdio_pkt(func, tx->send_buf, len);
	}

	sdev->loopback.loop_count--;
	sdev->loopback_seq++;
}

static void sdio_loop_Dev2Host(struct lte_sdev *sdev)
{
	struct rx_cxt *rx = &sdev->rx;
	struct loopback_hci *lp_hci = (struct loopback_hci *)rx->rx_buf;
	u32 seq;

	seq = D4H(&sdev->gdm_ed, lp_hci->seq);

	if (sdev->loopback_seq != seq)
		debug_loopback("Loopback Packet Sequnce Error Recv[%d]-Host[%d]\n",
				seq, sdev->loopback_seq);
	else
		debug_loopback("Rrcv SEQ[%d]\n", sdev->loopback_seq);

	if (sdev->loopback_seq == sdev->loopback.loop_count) {
		memset(&sdev->loopback , 0x00, sizeof(struct loopback));
		sdev->loopback_seq = 0;
	} else {
		sdev->loopback_seq++;
	}
}

static u8 gdm_sdio_read_byte(struct sdio_func *func, int addr, int *ret)
{
	u8 val;

	val = sdio_readb(func, addr, ret);
	if (ret < 0)
		drv_print(LTE_LOG_ERROR, "sdio_readb(%x) error=%d\n", addr, *ret);
	return val;
}

static void gdm_sdio_write_byte(struct sdio_func *func, u8 data, int addr, int *ret)
{
	sdio_writeb(func, data, addr, ret);
	if (ret < 0)
		drv_print(LTE_LOG_ERROR, "sdio_writeb(%x=%02x) error=%d\n", addr, data, *ret);
}

static struct sdio_tx *alloc_tx_struct(int len)
{
	struct sdio_tx *t = NULL;

	t = kmalloc(sizeof(*t), GFP_ATOMIC);
	if (t == NULL) {
		drv_print(LTE_LOG_ERROR, "[alloc_tx_struct] sdio_tx struct allocate fail\n");
		goto out;
	}

	memset(t, 0, sizeof(*t));

	t->buf = kmalloc(len, GFP_ATOMIC);
	if (t->buf == NULL) {
		drv_print(LTE_LOG_ERROR, "[alloc_tx_struct] sdio_tx struct buf allocate fail\n");
		goto out;
	}

	return t;
out:
	if (t) {
		kfree(t->buf);
		kfree(t);
	}

	return NULL;
}

static void free_tx_struct(struct sdio_tx *t)
{
	if (t) {
		kfree(t->buf);
		kfree(t);
	}
}

static struct sdio_tx *get_tx_struct(struct tx_cxt *tx, int *no_spc)
{
	struct sdio_tx *t;

	if (list_empty(&tx->free_list))
		return NULL;

	t = list_entry(tx->free_list.next, struct sdio_tx, list);
	list_del(&t->list);

	tx->avail_count--;

	*no_spc = list_empty(&tx->free_list) ? 1 : 0;

	return t;
}

static void put_tx_struct(struct tx_cxt *tx, struct sdio_tx *t)
{
	list_add_tail(&t->list, &tx->free_list);
	tx->avail_count++;
}

static struct sdio_rx *alloc_rx_struct(int len)
{
	struct sdio_rx *r = NULL;

	drv_print(LTE_LOG_FUNCTION, "+alloc_rx_struct\n");

	r = kmalloc(sizeof(*r), GFP_ATOMIC);
	if (r == NULL) {
		drv_print(LTE_LOG_ERROR, "[alloc_rx_struct] sdio_rx struct allocate fail\n");
		goto out;
	}

	memset(r, 0, sizeof(*r));

	r->buf = kmalloc(len, GFP_ATOMIC);
	if (r->buf == NULL) {
		drv_print(LTE_LOG_ERROR, "[alloc_rx_struct] sdio_rx struct buf allocate fail\n");
		goto out;
	}

	return r;
out:
	if (r) {
		kfree(r->buf);
		kfree(r);
	}

	drv_print(LTE_LOG_NOTICE, "-alloc_rx_struct\n");

	return NULL;
}

static void free_rx_struct(struct sdio_rx *r)
{
	if (r) {
		kfree(r->buf);
		kfree(r);
	}
}

static int init_sdio(struct lte_sdev *sdev)
{
#define MAX_NR_SDU_BUF 64
	int ret = 0;
	int i;

	struct rx_cxt *rx = &sdev->rx;
	struct tx_cxt *tx = &sdev->tx;
	struct sdio_tx *t = NULL;

	sdev->tx_stop = 0;
	sdev->request_mac_addr = 0;

	INIT_LIST_HEAD(&tx->sdu_list);
	INIT_LIST_HEAD(&tx->hci_list);
	INIT_LIST_HEAD(&tx->free_list);
	INIT_LIST_HEAD(&rx->rx_list);
	tx->avail_count = 0;
	tx->avail_send_buf = 1;

#ifdef TX_DELAY
	tx->sdu_stamp = jiffies;
	sdev->set_delay_queue = 0;
#endif /* TX_DELAY */

	spin_lock_init(&tx->lock);
	spin_lock_init(&rx->lock);

	tx->send_buf = kmalloc(TX_BUF_SIZE, GFP_ATOMIC);
	if (tx->send_buf == NULL) {
		ret = -ENOMEM;
		drv_print(LTE_LOG_ERROR, "[init_sdio] Failed to allocate SDU tx buffer.\n");
		goto fail;
	}

	for (i = 0; i < MAX_NR_SDU_BUF; i++) {
		t = alloc_tx_struct(SDU_BUF_SIZE);
		if (t == NULL) {
			ret = -ENOMEM;
			goto fail;
		}
		list_add(&t->list, &tx->free_list);
		tx->avail_count++;
	}

	rx->rx_buf = kmalloc(RX_BUF_SIZE, GFP_ATOMIC);
	if (rx->rx_buf == NULL) {
		ret = -ENOMEM;
		drv_print(LTE_LOG_ERROR, "[init_sdio] Failed to allocate rx buffer.\n");
		goto fail;
	}

	return 0;

fail:
	release_sdio(sdev);
	return ret;
}

static void release_sdio(struct lte_sdev *sdev)
{
	struct rx_cxt	*rx = &sdev->rx;
	struct tx_cxt	*tx = &sdev->tx;
	struct sdio_tx	*t, *t_next;
	struct sdio_rx	*r, *r_next;
	unsigned long flags;

	kfree(tx->send_buf);

	spin_lock_irqsave(&tx->lock, flags);
	list_for_each_entry_safe(t, t_next, &tx->free_list, list)
	{
		list_del(&t->list);
		free_tx_struct(t);
	}

	list_for_each_entry_safe(t, t_next, &tx->sdu_list, list)
	{
		list_del(&t->list);
		free_tx_struct(t);
	}

	list_for_each_entry_safe(t, t_next, &tx->hci_list, list)
	{
		list_del(&t->list);
		free_tx_struct(t);
	}
	spin_unlock_irqrestore(&tx->lock, flags);

	spin_lock_irqsave(&rx->lock, flags);
	list_for_each_entry_safe(r, r_next, &rx->rx_list, list)
	{
		list_del(&r->list);
		free_rx_struct(r);
	}
	spin_unlock_irqrestore(&rx->lock, flags);

	kfree(rx->rx_buf);
}

static void send_get_mac_address(struct lte_sdev *sdev, struct sdio_func *func)
{
	u8 buf[16] = {0,};
	struct hci_packet *hci = (struct hci_packet *)buf;

	hci->cmd_evt = H2D(&sdev->gdm_ed, LTE_GET_INFORMATION);
	hci->len = H2D(&sdev->gdm_ed, 1);
	hci->data[0] = MAC_ADDRESS;

	send_sdio_pkt(func, buf, 16);
}

static int set_mac_address(u8 *data, struct phy_dev *phy_dev)
{
	struct lte_sdev *sdev = phy_dev->priv_dev;
	struct tlv *tlv = (struct tlv *)data;
	u8 mac_address[6] = {0, };

	if (tlv->type == MAC_ADDRESS && sdev->request_mac_addr) {
		memcpy(mac_address, tlv->data, tlv->len);

		drv_print(LTE_LOG_NOTICE, "MAC Address %02x:%02x:%02x:%02x:%02x:%02x\n"
				, mac_address[0]
				, mac_address[1]
				, mac_address[2]
				, mac_address[3]
				, mac_address[4]
				, mac_address[5]
			 );

		if (register_lte_device(phy_dev, &sdev->func->dev, mac_address) < 0) {
			drv_print(LTE_LOG_NOTICE, "[%s] Register LTE Device Failed\n", __FUNCTION__);
			return 0;
		}

		sdev->request_mac_addr = 0;

		return 1;
	}

	return 0;
}

#ifdef NV_HOST
static void nv_restore_response(u8 *read_buf, struct lte_sdev *sdev)
{
	struct sdio_func *func = sdev->func;
	struct tx_cxt *tx = &sdev->tx;
	u32 len;

	drv_print(LTE_LOG_NOTICE, "recv NV_RESTORE_REQUEST\n");

	len = nv_restore(&sdev->gdm_ed, tx->send_buf);

	if (!len || len > TX_BUF_SIZE + HCI_HEADER_SIZE) {
		drv_print(LTE_LOG_ERROR, "%s Invalid length\n", __FUNCTION__);
	} else {
		send_sdio_pkt(func, tx->send_buf, len);
		drv_print(LTE_LOG_NOTICE, "send NV_RESTORE_RESPONSE\n");
	}
}

static void nv_save_response(u8 *read_buf, struct lte_sdev *sdev)
{
	struct sdio_func *func = sdev->func;
	struct tx_cxt *tx = &sdev->tx;
	u32 len;

	drv_print(LTE_LOG_NOTICE, "recv NV_SAVE_REQUEST\n");

	len = nv_save(&sdev->gdm_ed, read_buf, tx->send_buf);
	if (!len || len > TX_BUF_SIZE + HCI_HEADER_SIZE) {
		drv_print(LTE_LOG_ERROR, "%s Invalid length\n", __FUNCTION__);
	} else {
		send_sdio_pkt(func, tx->send_buf, len);
		drv_print(LTE_LOG_NOTICE, "send NV_SAVE_RESPONSE\n");
	}
}
#endif /* NV_HOST */

static void sdu_flow_control(u8 *read_buf, struct lte_sdev *sdev)
{
	struct hci_flowcontrol *flow = (struct hci_flowcontrol *)read_buf;
	struct tx_cxt *tx = &sdev->tx;
	unsigned long flags;

	if (flow->flag) {
		drv_print(LTE_LOG_FUNCTION, "TX RESUME\n");
		spin_lock_irqsave(&tx->lock, flags);
		sdev->tx_stop = 0;
		queue_work(sdio_lte_wq, &sdev->ws.work);
		spin_unlock_irqrestore(&tx->lock, flags);
	} else {
		drv_print(LTE_LOG_FUNCTION, "TX STOP\n");
		spin_lock_irqsave(&tx->lock, flags);
		sdev->tx_stop = 1;
		spin_unlock_irqrestore(&tx->lock, flags);
	}
}

static void irq_worker(struct work_struct *work)
{
	struct lte_sdev *sdev = container_of(work, struct lte_sdev, irq_ws.work);
	struct sdio_func *func = sdev->func;
#ifdef SDIO_MODEM_PORT
	struct mux_dev *mux_dev = sdio_get_drvdata(func);
	struct phy_dev *phy_dev = mux_dev->phy_dev;
#ifdef SDIO_AT_COMMAND_EXTENDED
	struct at_command_ext *at_command_ext;
#endif
#else
	struct phy_dev *phy_dev = sdio_get_drvdata(func);
#endif /* SDIO_MODEM_PORT */
	struct tx_cxt *tx = &sdev->tx;
	struct rx_cxt *rx = &sdev->rx;
	struct sdio_rx *r = NULL;
	struct hci_packet *hci = NULL;
	u16 cmd_evt;
	unsigned long flags;

	while (!gdm_list_empty(&rx->rx_list, &rx->lock)) {
		gdm_list_entry(rx->rx_list.next, struct sdio_rx,
			       list, r, &rx->lock, flags);

		hci = (struct hci_packet *)r->buf;
		cmd_evt = D2H(&sdev->gdm_ed, hci->cmd_evt);

		switch (cmd_evt) {
		case SDIO_FW_START:
			spin_lock_irqsave(&tx->lock, flags);
			lte_fw_start = 1;
			spin_unlock_irqrestore(&tx->lock, flags);

			sdev->request_mac_addr = 1;
			send_get_mac_address(sdev, func);
			drv_print(LTE_LOG_NOTICE, "recv SDIO_FW_START\n");
			break;
		case LTE_GET_INFORMATION_RESULT:
			if (!set_mac_address(hci->data, phy_dev)) {
				if (sdev->callback)
					sdev->callback(sdev->cb_data, r->buf, r->len, KERNEL_THREAD);
			}
			break;
#ifdef NV_HOST
		case LTE_NV_RESTORE_REQUEST:
			nv_restore_response(r->buf, sdev);
			break;
		case LTE_NV_SAVE_REQUEST:
			nv_save_response(r->buf, sdev);
			if (sdev->callback)
				sdev->callback(sdev->cb_data, r->buf, r->len, KERNEL_THREAD);
			break;
#endif /* NV_HOST */
		case LTE_UL_SDU_FLOW_CONTROL:
			sdu_flow_control(r->buf, sdev);
			break;
#ifdef SDIO_MODEM_PORT
#ifdef SDIO_AT_COMMAND_EXTENDED
		case LTE_AT_CMD_FROM_DEVICE_EXT:
			at_command_ext = (struct at_command_ext *)hci->data;
			if (sdev->tty_callback)
				sdev->tty_callback((void *)at_command_ext->cmd,
						(int)(D2H(&sdev->gdm_ed, hci->len)-2),
						AT_COMMAND_INDEX ,
						sdev->tty_minor[AT_COMMAND_INDEX],
						RECV_PACKET_PROCESS_COMPLETE);

			if (sdev->callback)
				sdev->callback(sdev->cb_data, r->buf, r->len, KERNEL_THREAD);
			break;
#else
		case LTE_AT_CMD_FROM_DEVICE:
			if (sdev->tty_callback)
				sdev->tty_callback((void *)hci->data,
						(int)(D2H(&sdev->gdm_ed, hci->len)-1),
						AT_COMMAND_INDEX ,
						sdev->tty_minor[AT_COMMAND_INDEX],
						RECV_PACKET_PROCESS_COMPLETE);
			if (sdev->callback)
				sdev->callback(sdev->cb_data, r->buf, r->len, KERNEL_THREAD);
#endif
			break;
		case LTE_SDIO_DM_RECV_PKT:
			if (sdev->tty_callback)
				sdev->tty_callback((void *)hci->data,
						(int)(D2H(&sdev->gdm_ed, hci->len)),
						DM_MSG_INDEX,
						sdev->tty_minor[DM_MSG_INDEX],
						RECV_PACKET_PROCESS_COMPLETE);
			break;
#endif /* SDIO_MODEM_PORT */
		default:
			if (sdev->callback)
				sdev->callback(sdev->cb_data, r->buf, r->len, KERNEL_THREAD);
			break;
		}

		gdm_list_del(&r->list, &rx->lock);
		free_rx_struct(r);
	}
}

static void gdm_sdio_irq(struct sdio_func *func)
{
#ifdef SDIO_MODEM_PORT
	struct mux_dev *mux_dev = sdio_get_drvdata(func);
	struct phy_dev *phy_dev = mux_dev->phy_dev;
#else
	struct phy_dev *phy_dev = sdio_get_drvdata(func);
#endif /* SDIO_MODEM_PORT */
	struct lte_sdev *sdev = phy_dev->priv_dev;
	struct rx_cxt *rx = &sdev->rx;
	struct tx_cxt *tx = &sdev->tx;
	struct sdio_rx *r = NULL;
	unsigned long flags;
	u32 len;
	int ret;

	len = sdio_ack_irq_and_get_read_len(func);
	if (len <= 0)
		return;

	r = alloc_rx_struct(len);
	if (!r)
		return;

	ret = sdio_read(func, r->buf, 0, len);
	if (ret < 0) {
#ifdef SDIO_DEVICE_STATUS_CHECK
		int dev_status = 1;
		if (sdev->status_cb)
			sdev->status_cb(sdev->status_cb_data, &dev_status);
#endif /* SDIO_DEVICE_STATUS_CHECK */
		drv_print(LTE_LOG_ERROR, "[gdm_sdio_irq] Cannot Read From Device   Error : ret = %d\n", ret);
		return;
	}
	r->len = len;

	if (LOOPBACK_DRV_TO_DEV == sdev->loopback.loopback_type &&
			BI_DIRECTION == sdev->loopback.transfer_type) {
		sdio_loop_bi_direction(sdev, 1);
	} else if (LOOPBACK_DRV_TO_DEV == sdev->loopback.loopback_type &&
			FROM_DEV_TO_HOST == sdev->loopback.transfer_type) {
		sdio_loop_Dev2Host(sdev);
	} else {
		spin_lock_irqsave(&rx->lock, flags);

		list_add_tail(&r->list, &rx->rx_list);
		queue_work(sdio_irq_wq, &sdev->irq_ws.work);

		spin_unlock_irqrestore(&rx->lock, flags);
	}

	spin_lock_irqsave(&tx->lock, flags);
	queue_work(sdio_lte_wq, &sdev->ws.work);
	spin_unlock_irqrestore(&tx->lock, flags);
}

static int gdm_sdio_recv(void *priv_dev,
			int (*cb)(void *cb_data, void *data, int len, int context),
			void *cb_data,
			int context)
{
	struct lte_sdev *sdev = priv_dev;

	sdev->callback = cb;
	sdev->cb_data = cb_data;

	return 0;
}

static int send_sdio_pkt(struct sdio_func *func, u8 *data, int len)
{
	struct hci_packet *hci = (struct hci_packet *)data;
#ifdef SDIO_DEVICE_STATUS_CHECK
#ifdef SDIO_MODEM_PORT
	struct mux_dev *mux_dev = sdio_get_drvdata(func);
	struct phy_dev *phy_dev = mux_dev->phy_dev;
#else
	struct phy_dev *phy_dev = sdio_get_drvdata(func);
#endif
	struct lte_sdev *sdev = phy_dev->priv_dev;
#endif /* SDIO_DEVICE_STATUS_CHECK */
	int ret;
	sdio_claim_host(func);

	if (hci->cmd_evt == LTE_LOOPBACK_TX_PACKET) {
		struct loopback_hci *lp_hci = (struct loopback_hci *)data;
		drv_print(LTE_LOG_FUNCTION, "[send_sdio_pkt] cmd_evt 0x%04x(%d, %d) total len %d\n",
				    lp_hci->cmd_evt,
				    lp_hci->len,
				    lp_hci->seq, len);
	}

	len = ((len + (SDIO_BLOCK_SIZE - 1)) & ~(SDIO_BLOCK_SIZE - 1));

	ret = sdio_write(func, 0, data, len);
	drv_print(LTE_LOG_FUNCTION, "[send_sdio_pkt] %04x\n",  hci->cmd_evt);
	if (ret < 0) {
#ifdef SDIO_DEVICE_STATUS_CHECK
		int dev_status = 1;
		if (sdev->status_cb)
			sdev->status_cb(sdev->status_cb_data, &dev_status);
#endif /* SDIO_DEVICE_STATUS_CHECK */
		drv_print(LTE_LOG_ERROR, "[send_sdio_pkt] error: ret = %d  %04x\n", ret, hci->cmd_evt);
	}

	sdio_release_host(func);

	return ret;
}

static void send_hci(struct sdio_func *func, struct tx_cxt *tx, struct sdio_tx *t)
{
	unsigned long flags;
#ifdef SDIO_MODEM_PORT
	struct mux_dev *mux_dev = sdio_get_drvdata(func);
	struct phy_dev *phy_dev = mux_dev->phy_dev;
#else
	struct phy_dev *phy_dev = sdio_get_drvdata(func);
#endif /* SDIO_MODEM_PORT */
	struct lte_sdev *sdev = phy_dev->priv_dev;

	send_sdio_pkt(func, t->buf, t->len);

	spin_lock_irqsave(&tx->lock, flags);
	if (t->callback)
		t->callback(t->cb_data);
	free_tx_struct(t);
	queue_work(sdio_lte_wq, &sdev->ws.work);
	spin_unlock_irqrestore(&tx->lock, flags);
}

static void send_sdu(struct sdio_func *func, struct tx_cxt *tx, u32 len)
{
	unsigned long flags;
#ifdef SDIO_MODEM_PORT
	struct mux_dev *mux_dev = sdio_get_drvdata(func);
	struct phy_dev *phy_dev = mux_dev->phy_dev;
#else
	struct phy_dev *phy_dev = sdio_get_drvdata(func);
#endif /* SDIO_MODEM_PORT */
	struct lte_sdev *sdev = phy_dev->priv_dev;

	drv_print(LTE_LOG_FUNCTION, "TX SDU Len : %d\n", len);

	send_sdio_pkt(func, tx->send_buf, len);

	spin_lock_irqsave(&tx->lock, flags);
	tx->avail_send_buf = 1;
#ifdef TX_DELAY
	tx->sdu_stamp = jiffies;
#endif /* TX_DELAY */
	queue_work(sdio_lte_wq, &sdev->ws.work);
	spin_unlock_irqrestore(&tx->lock, flags);
}

static u32 packet_aggregation(struct lte_sdev *sdev)
{
	struct tx_cxt *tx = &sdev->tx;
	struct sdio_tx *t = NULL;
	struct multi_sdu *multi_sdu = (struct multi_sdu *)tx->send_buf;
	u16 send_len = 0;
	u16 num_packet = 0;
	unsigned long flags;

	multi_sdu->cmd_evt = H2D(&sdev->gdm_ed, LTE_TX_MULTI_SDU);

	while (!gdm_list_empty(&tx->sdu_list, &tx->lock)) {
		gdm_list_entry(tx->sdu_list.next, struct sdio_tx, list, t, &tx->lock, flags);
		if (send_len + t->len > MAX_SDU_SIZE) {
			drv_print(LTE_LOG_FUNCTION, "[packet_aggregation] buffer is full %d\n", send_len);
			break;
		}

		gdm_list_del(&t->list, &tx->lock);

		memcpy(multi_sdu->data + send_len, t->buf, t->len);

		send_len += (t->len + 3) & 0xfffc;
		num_packet++;

		if (tx->avail_count > 10)
			t->callback(t->cb_data);

		spin_lock_irqsave(&tx->lock, flags);
		put_tx_struct(tx, t);
		spin_unlock_irqrestore(&tx->lock, flags);
	}

	multi_sdu->len = H2D(&sdev->gdm_ed, send_len);
	multi_sdu->num_packet = H2D(&sdev->gdm_ed, num_packet);

	return send_len + offsetof(struct multi_sdu, data);
}

static void do_tx(struct work_struct *work)
{

	struct lte_sdev *sdev = container_of(work, struct lte_sdev, ws.work);
	struct sdio_func *func = sdev->func;
	struct tx_cxt *tx = &sdev->tx;
	struct sdio_tx *t = NULL;
	int is_sdu = 0;
	int is_send = 0;
	u32 len = 0;
	unsigned long flags;

	spin_lock_irqsave(&tx->lock, flags);
	if (!lte_fw_start) {
		drv_print(LTE_LOG_NOTICE, "FW not ready\n");
		spin_unlock_irqrestore(&tx->lock, flags);
		return;
	}
	if (!list_empty(&tx->hci_list)) {
		t = list_entry(tx->hci_list.next, struct sdio_tx, list);
		list_del(&t->list);
		is_sdu = 0;
		is_send = 1;
	} else if (!list_empty(&tx->sdu_list)) {
		if (sdev->tx_stop) {
			spin_unlock_irqrestore(&tx->lock, flags);
			return;
		}

		if (!tx->avail_send_buf) {
			spin_unlock_irqrestore(&tx->lock, flags);
			return;
		}

#ifdef TX_DELAY
		if (tx->avail_count > 40) {
			unsigned long current_jiffies = jiffies;
			unsigned long gap_ms = ((current_jiffies - tx->sdu_stamp) * 1000/HZ);

			if (gap_ms < TX_INTERVAL) {
				unsigned int delay_ms = TX_INTERVAL - gap_ms;
				
				if (!sdev->set_delay_queue) {
					queue_delayed_work(sdio_lte_wq,
							   &sdev->ws,
							   msecs_to_jiffies(delay_ms));
					sdev->set_delay_queue = 1;
				}
				spin_unlock_irqrestore(&tx->lock, flags);
				return;
			} else {
				if (sdev->set_delay_queue) {
					cancel_delayed_work(&sdev->ws);
					sdev->set_delay_queue = 0;
				}
			}
		} else {
			unsigned long current_jiffies = jiffies;
			unsigned long gap_ms = ((current_jiffies - tx->sdu_stamp) * 1000/HZ);

			if (gap_ms < TX_INTERVAL2) {
				unsigned int delay_ms = TX_INTERVAL2 - gap_ms;
				if (!sdev->set_delay_queue) {
					queue_delayed_work(sdio_lte_wq, 
					&sdev->ws, 
					msecs_to_jiffies(delay_ms));
					sdev->set_delay_queue = 1;
				}
				spin_unlock_irqrestore(&tx->lock, flags);
				return;
			}
		}

		if (sdev->set_delay_queue) {
			cancel_delayed_work(&sdev->ws);
			sdev->set_delay_queue = 0;
		}

#endif /* TX_DELAY */

		tx->avail_send_buf = 0;

		is_sdu = 1;
		is_send = 1;
	}

	spin_unlock_irqrestore(&tx->lock, flags);

	if (!is_send)
		return;

	if (!is_sdu) {
		send_hci(func, tx, t);
	} else {
		len = packet_aggregation(sdev);
		send_sdu(func, tx, len);
	}

	if ((LOOPBACK_DRV_TO_DEV == sdev->loopback.loopback_type) &&
	   sdev->loopback_seq == 1) {
		switch (sdev->loopback.transfer_type) {
		case BI_DIRECTION:
			/* one time */
			drv_print(LTE_LOG_FUNCTION, "BI_DIRECTION\n");
			sdio_loop_bi_direction(sdev, 0);
			break;
		case FROM_HOST_TO_DEV:
			/* for loop */
			drv_print(LTE_LOG_FUNCTION, "HOST_TO_DEV\n");
			sdio_loop_Host2Dev(sdev);
			break;
		}
	}
}

static int gdm_sdio_sdu_send(void *priv_dev, void *data, int len,
							unsigned int dftEpsId, unsigned int epsId,
							void (*cb)(void *data), void *cb_data,
							int dev_idx, int nic_type)
{
	struct lte_sdev *sdev = priv_dev;
	struct tx_cxt *tx = &sdev->tx;
	struct sdio_tx *t;
	struct sdu *sdu = NULL;
	unsigned long flags;
	int no_spc = 0;
	u16 send_len;

	spin_lock_irqsave(&tx->lock, flags);
	t = get_tx_struct(tx, &no_spc);
	spin_unlock_irqrestore(&tx->lock, flags);

	if (NULL == t) {
		drv_print(LTE_LOG_ERROR, "[gdm_sdio_sdu_send] get tx buffer fail\n");
		return TX_NO_SPC;
	}

	sdu = (struct sdu *)t->buf;
	sdu->cmd_evt = LTE_TX_SDU;
	if (nic_type == NIC_TYPE_ARP) {
		send_len = (u16)(len + SDU_PARAM_LEN);
	    memcpy(sdu->data, data, len);
	} else {
	    send_len = (u16)(len - ETHERNET_HEADER_SIZE);
	    send_len += SDU_PARAM_LEN;
	    memcpy(sdu->data, data+ETHERNET_HEADER_SIZE, len-ETHERNET_HEADER_SIZE);
	}
	sdu->len = H2D(&sdev->gdm_ed, send_len);
	sdu->dftEpsId = H4D(&sdev->gdm_ed, dftEpsId);
	sdu->bearer_ID = H4D(&sdev->gdm_ed, epsId);
	sdu->nic_type = H4D(&sdev->gdm_ed, nic_type);

	t->len = send_len + HCI_HEADER_SIZE;
	t->callback = cb;
	t->cb_data = cb_data;

	spin_lock_irqsave(&tx->lock, flags);
	list_add_tail(&t->list, &tx->sdu_list);
	queue_work(sdio_lte_wq, &sdev->ws.work);
	spin_unlock_irqrestore(&tx->lock, flags);

	if (no_spc)
		return TX_NO_BUFFER;

	return 0;
}

static int gdm_sdio_hci_send(void *priv_dev, void *data, int len, void (*cb)(void *data), void *cb_data)
{
	struct lte_sdev *sdev = priv_dev;
	struct tx_cxt *tx = &sdev->tx;
	u8 *pkt = data;
	u16 cmd_evt;
	struct sdio_tx *t;
	unsigned long flags;
	struct hci_packet *hci = (struct hci_packet *)data;
	int hci_buf_size;

	spin_lock_irqsave(&tx->lock, flags);

	cmd_evt = D2H(&sdev->gdm_ed, hci->cmd_evt);
	drv_print(LTE_LOG_FUNCTION, "cmd_evt %04x\n", cmd_evt);

	if (LTE_LOOPBACK_START == cmd_evt) {
		memcpy(&sdev->loopback, pkt+HCI_HEADER_SIZE, sizeof(struct loopback));

		if (LOOPBACK_DRV_TO_DEV != sdev->loopback.loopback_type) {
			memset(&sdev->loopback, 0x00, sizeof(struct loopback));
		} else {
			sdev->loopback.loop_count = D4H(&sdev->gdm_ed, sdev->loopback.loop_count);
			sdev->loopback.packet_size = D4H(&sdev->gdm_ed, sdev->loopback.packet_size);
		}

		sdev->loopback_seq = 1;
	}

	if ((LTE_LOOPBACK_TX_PACKET == cmd_evt || LTE_LOOPBACK_START == cmd_evt) &&
		 sdev->loopback.loopback_type != LOOPBACK_DRV_TO_DEV) {
		struct sdio_func *func = sdev->func;
		spin_unlock_irqrestore(&tx->lock, flags);
		send_sdio_pkt(func, (u8 *)data, len);
		return 0;
	} else {
		hci_buf_size = ((len + (SDIO_BLOCK_SIZE - 1)) & ~(SDIO_BLOCK_SIZE - 1));
		t = alloc_tx_struct(hci_buf_size);
		if (NULL == t) {
			drv_print(LTE_LOG_ERROR, "[gdm_sdio_hci_send] tx alloc fail\n");
			spin_unlock_irqrestore(&tx->lock, flags);
			return -ENOMEM;
		}

		list_add_tail(&t->list, &tx->hci_list);

		memcpy(t->buf, data, len);

		t->len = len;
		t->callback = cb;
		t->cb_data = cb_data;
	}

	queue_work(sdio_lte_wq, &sdev->ws.work);
	spin_unlock_irqrestore(&tx->lock, flags);

	return 0;
}
#ifdef SDIO_MODEM_PORT
#ifdef SDIO_AT_COMMAND_EXTENDED
#define DUMMY 2
#else
#define DUMMY 1
#endif
static int gdm_sdio_tty_send(void *priv_dev, void *data, int len, int tty_index,
			     void (*cb)(void *data), void *cb_data)
{
	struct lte_sdev *sdev = priv_dev;
	struct tx_cxt *tx = &sdev->tx;
	struct sdio_tx *t;
	unsigned long flags;
	struct hci_packet *hci;
	int hci_buf_size;
	int total_len;
#ifdef SDIO_AT_COMMAND_EXTENDED
	struct at_command_ext *at_command_ext;
#endif

	if (tty_index == AT_COMMAND_INDEX)
		total_len = len + HCI_HEADER_SIZE + DUMMY;
	else
		total_len = len + HCI_HEADER_SIZE;

	spin_lock_irqsave(&tx->lock, flags);

	hci_buf_size = ((total_len + (SDIO_BLOCK_SIZE - 1)) & ~(SDIO_BLOCK_SIZE - 1));
	t = alloc_tx_struct(hci_buf_size);
	if (NULL == t) {
		drv_print(LTE_LOG_ERROR, "[gdm_sdio_hci_send] tx alloc fail\n");
		spin_unlock_irqrestore(&tx->lock, flags);
		return -ENOMEM;
	}

	hci = (struct hci_packet *)t->buf;
	if (tty_index == AT_COMMAND_INDEX) {
#ifdef SDIO_AT_COMMAND_EXTENDED
		hci->cmd_evt = H2D(&sdev->gdm_ed, LTE_AT_CMD_TO_DEVICE_EXT);
		at_command_ext = (struct at_command_ext *)hci->data;
		at_command_ext->channel = 1;
		memcpy(at_command_ext->cmd, data, len);
		at_command_ext->cmd[len] = 0x0a;
#else
		hci->cmd_evt = H2D(&sdev->gdm_ed, LTE_AT_CMD_TO_DEVICE);
		memcpy(hci->data, data, len);
		hci->data[len] = 0x0a;
#endif
	} else {
		hci->cmd_evt = H2D(&sdev->gdm_ed, LTE_SDIO_DM_SEND_PKT);
		memcpy(hci->data, data, len);
	}

	list_add_tail(&t->list, &tx->hci_list);

	hci->len = H2D(&sdev->gdm_ed, (u16)(total_len - HCI_HEADER_SIZE));
	t->len = total_len;
	t->callback = cb;
	t->cb_data = cb_data;

	queue_work(sdio_lte_wq, &sdev->ws.work);
	spin_unlock_irqrestore(&tx->lock, flags);

	return 0;
}

static int gdm_sdio_tty_recv(void *priv_dev,
				int (*cb)(void *data, int len, int tty_index, int minor, int complete))
{
	struct lte_sdev *sdev = priv_dev;

	sdev->tty_callback = cb;

	return 0;
}
#endif /* SDIO_MODEM_PORT */

static struct gdm_endian *gdm_sdio_get_endian(void *priv_dev)
{
	struct lte_sdev *sdev = priv_dev;

	return &sdev->gdm_ed;
}

#ifdef SDIO_DEVICE_STATUS_CHECK
void gdm_lte_sdev_status(void *priv_dev, void *cb_data, void (*cb)(void *cb_data, void *data))
{
	struct lte_sdev *sdev = priv_dev;

	sdev->status_cb = cb;
	sdev->status_cb_data = cb_data;
}

#endif /* SDIO_DEVICE_STATUS_CHECK */

static int gdm7240_init(struct sdio_func *func)
{
#ifndef SDIO_BOOT
	u8 dev_ready;
#endif /* !SDIO_BOOT */
	int ret = 0;

#ifdef SDIO_BOOT
	ret = sdio_boot(func);
	if (ret) {
		drv_print(LTE_LOG_ERROR, "sdio boot failed\n");
	}
#else /* SDIO_BOOT */
	dev_ready = sdio_readb(func, GDM7240_DEVICE_READY_REGISTER, &ret);
	if (dev_ready != GDM7240_DEVICE_READY) {
		drv_print(LTE_LOG_ERROR, "device is not ready\n");
	}
#endif
	sdio_writeb(func, GDM7240_HOST_READY, GDM7240_HOST_READY_REGISTER, &ret); /* SDIO HOST READY */

	if (ret) {
		drv_print(LTE_LOG_ERROR, "sdio_writeb fail ret : %d\n", ret);
	}

	return ret;
}

static int gdm7240_ack_irq_and_get_read_len(struct sdio_func *func)
{
	int ret, len;
	u8 len_reg[3];

	sdio_read_byte(func, GDM7240_READ_DATA_READY_INT, &ret);

	/* read data length */
	#if 0
	sdio_memcpy_fromio(func, len_reg, GDM7243_AHB_TRANSFER_CNT, 3);
	#else
	len_reg[0] = sdio_read_byte(func, GDM7240_AHB_TRANSFER_CNT, &ret);
	len_reg[1] = sdio_read_byte(func, GDM7240_AHB_TRANSFER_CNT+1, &ret);
	len_reg[2] = sdio_read_byte(func, GDM7240_AHB_TRANSFER_CNT+2, &ret);
	#endif
	len = (len_reg[2] << 16) | (len_reg[1] << 8) | len_reg[0];

	if (ret < 0)
		len = ret;
	
	return len;
}

#if defined(ENABLE_AUTO_WAKEUP)
static int gdm7243_set_auto_wakeup(struct sdio_func *func, int enable)
{
	int ret;
	u8 val, val2;

	if (enable)
		val2 = AUTO_CLK_EN;
	else
		val2 = MANUAL_CLK_EN;

	val = sdio_read_byte(func, GDM7243_CLK_WKUP_REG, &ret);

	sdio_write_byte(func, val2, GDM7243_CLK_WKUP_REG, &ret);
	
	val2 = sdio_read_byte(func, GDM7243_CLK_WKUP_REG, &ret);
	drv_print(LTE_LOG_NOTICE, "Clock wakeup register: %02x->%02x\n", val, val2);
	return ret;
}
#endif

static int gdm7243_enable_interrupt(struct sdio_func *func)
{
	int ret;
	u8 val, val2;

	val = sdio_read_byte(func, GDM7243_INT_ENA, &ret);

	val2 = val | READ_DATA_READY_INT;

	sdio_write_byte(func, val2, GDM7243_INT_ENA, &ret);
	
	val2 = sdio_read_byte(func, GDM7243_INT_ENA, &ret);
	drv_print(LTE_LOG_NOTICE, "Interrupt enable register: %02x->%02x\n", val, val2);

	return ret;
}

#if defined(USE_GPR)
static int read_gpr(struct sdio_func *func, u32 addr, u32 *data)
{
	int ret;

	ret = sdio_memcpy_fromio(func, data, addr, 4);
	if (ret)
		drv_print(LTE_LOG_ERROR, "read gpr fail : %d\n",ret);
	return ret;
}

static int write_gpr(struct sdio_func *func, u32 addr, u32 data)
{
	int ret;

	sdio_claim_host(func);
	ret = sdio_memcpy_toio(func, addr, &data, 4);
	sdio_release_host(func);
	if (ret)
		drv_print(LTE_LOG_ERROR, "write host-gpr fail : %d\n",ret);
	return ret;
}
#endif

static int gdm7243_init(struct sdio_func *func)
{
	#if defined(USE_GPR)
	u32 data;
	
	data = 0;
	read_gpr(func, GDM7243_DEVICE_GPR, &data);
	nprintk("READ GDM7243_DEVICE_GPR: %08x\n", data);

	write_gpr(func, GDM7243_HOST_GPR, HOST_READY);
	nprintk("WRITE GDM7243_HOST_GPR: %08x\n", HOST_READY);
	#endif

	gdm7243_enable_interrupt(func);
	
	#if defined(ENABLE_AUTO_WAKEUP)
	gdm7243_set_auto_wakeup(func, 1);
	#endif

	return 0;
}

static int gdm7243_ack_irq_and_get_read_len(struct sdio_func *func)
{
	int ret, len;
	u8 len_reg[3];

	sdio_write_byte(func, READ_DATA_READY_INT, GDM7243_INT_IND, &ret);

	/* read data length */
	#if 1
	sdio_memcpy_fromio(func, len_reg, GDM7243_AHB_TRANSFER_CNT, 3);
	#else
	len_reg[0] = sdio_read_byte(func, GDM7243_AHB_TRANSFER_CNT, &ret);
	len_reg[1] = sdio_read_byte(func, GDM7243_AHB_TRANSFER_CNT+1, &ret);
	len_reg[2] = sdio_read_byte(func, GDM7243_AHB_TRANSFER_CNT+2, &ret);
	#endif
	len = (len_reg[2] << 16) | (len_reg[1] << 8) | len_reg[0];

	if (ret < 0)
		len = ret;
	
	return len;
}

static void setup_sdio_cb(const struct sdio_device_id *id)
{
	sdio_ops_t *ops = &sdio_cb.ops;

	if (id->device == DEVICE_ID_7240) {
		drv_print(LTE_LOG_NOTICE, "Set GDM7240 operation!\n");
		ops->op_init = gdm7240_init;
		ops->op_read = sdio_memcpy_fromio;
		ops->op_write = sdio_memcpy_toio;
		ops->op_ack_irq_and_get_read_len =
			gdm7240_ack_irq_and_get_read_len;
	}
	else {
		drv_print(LTE_LOG_NOTICE, "Set GDM7243 operation!\n");
		ops->op_init = gdm7243_init;
		ops->op_read = sdio_readsb;
		ops->op_write = sdio_writesb;
		ops->op_ack_irq_and_get_read_len =
			gdm7243_ack_irq_and_get_read_len;
	}
	ops->op_readb = gdm_sdio_read_byte;
	ops->op_writeb = gdm_sdio_write_byte;
}

static int sdio_lte_probe(struct sdio_func *func, const struct sdio_device_id *id)
{
	int ret = 0;
	struct phy_dev *phy_dev = NULL;
	struct lte_sdev *sdev = NULL;
#ifdef SDIO_MODEM_PORT
	struct mux_dev *mux_dev = NULL;
	struct tty_dev *tty_dev = NULL;
#endif /* SDIO_MODEM_PORT */
	drv_print(LTE_LOG_NOTICE, "+sdio_lte_probe\n");

	/* Wait sdio_lte_init */
	lte_fw_start = 0;
	if (!lte_init_finish) {
#ifdef SDIO_MODEM_PORT
		register_lte_tty_driver();
#endif /* SDIO_MODEM_PORT */
		lte_init_finish = 1;
	}

	sdio_claim_host(func);
	sdio_enable_func(func);
	sdio_claim_irq(func, gdm_sdio_irq);

	sdio_set_block_size(func, 512);

	setup_sdio_cb(id);
	ret = sdio_init(func);
	if (ret) {
		sdio_release_irq(func);
		sdio_disable_func(func);
		sdio_release_host(func);

		return ret;
	}

	do {
		phy_dev = kmalloc(sizeof(struct phy_dev), GFP_ATOMIC);
		if (phy_dev == NULL) {
			drv_print(LTE_LOG_ERROR, "[sdio_lte_probe] phy_dev kmalloc fail\n");
			ret = -ENOMEM;
			break;
		}

#ifdef SDIO_MODEM_PORT
		tty_dev = kmalloc(sizeof(struct phy_dev), GFP_ATOMIC);
		if (tty_dev == NULL) {
			drv_print(LTE_LOG_ERROR, "[sdio_lte_probe] tty_dev kmalloc fail\n");
			ret = -ENOMEM;
			break;
		}

		mux_dev = kmalloc(sizeof(struct mux_dev), GFP_ATOMIC);
		if (mux_dev == NULL) {
			drv_print(LTE_LOG_ERROR, "[sdio_lte_probe] mux_dev kmalloc fail\n");
			ret = -ENOMEM;
			break;
		}
#endif /* SDIO_MODEM_PORT */

		sdev = kmalloc(sizeof(struct lte_sdev), GFP_ATOMIC);
		if (sdev == NULL) {
			kfree(phy_dev);
			drv_print(LTE_LOG_ERROR, "[sdio_lte_probe] sdev kmalloc fail\n");
			ret = -ENOMEM;
			break;
		}

		memset(phy_dev, 0, sizeof(struct phy_dev));
		memset(sdev, 0, sizeof(struct lte_sdev));

		phy_dev->priv_dev = (void *)sdev;
		phy_dev->send_hci_func = gdm_sdio_hci_send;
		phy_dev->send_sdu_func = gdm_sdio_sdu_send;
		phy_dev->rcv_func = gdm_sdio_recv;
		phy_dev->get_endian = gdm_sdio_get_endian;
#ifdef SDIO_DEVICE_STATUS_CHECK
		phy_dev->dev_status_func = gdm_lte_sdev_status;
#endif /* SDIO_DEVICE_STATUS_CHECK */
#ifdef SDIO_MODEM_PORT
		tty_dev->priv_dev = (void *)sdev;
		tty_dev->send_func = gdm_sdio_tty_send;
		tty_dev->recv_func = gdm_sdio_tty_recv;

		if (register_lte_tty_device(tty_dev, &func->dev) < 0) {
			drv_print(LTE_LOG_ERROR, "[sdio_lte_probe] sdev kmalloc fail\n");
			break;
		}

		mux_dev->tty_dev = tty_dev;
		mux_dev->phy_dev = phy_dev;
#endif /* SDIO_MODEM_PORT */

		ret = init_sdio(sdev);
		if (sdev < 0) {
			kfree(phy_dev);
			kfree(sdev);

			drv_print(LTE_LOG_ERROR, "[sdio_lte_probe] init_sdio func fail\n");
			break;
		}

		start_rx_proc(phy_dev);

		sdev->func = func;

		if (sdio_lte_wq)
			INIT_DELAYED_WORK(&sdev->ws, do_tx);

		if (sdio_irq_wq)
			INIT_DELAYED_WORK(&sdev->irq_ws, irq_worker);

		if (id->device == DEVICE_ID_7243)
			set_endian(&sdev->gdm_ed, ENDIANNESS_BIG);
		else
			set_endian(&sdev->gdm_ed, ENDIANNESS_LITTLE);

#ifdef SDIO_MODEM_PORT
		sdio_set_drvdata(func, mux_dev);
#else
		sdio_set_drvdata(func, phy_dev);
#endif /* SDIO_MODEM_PORT */
	} while (0);

	sdio_release_host(func);

	drv_print(LTE_LOG_NOTICE, "-sdio_lte_probe\n");

	return ret;
}

static void sdio_lte_remove(struct sdio_func *func)
{
#ifdef SDIO_MODEM_PORT
	struct mux_dev *mux_dev = NULL;
	struct tty_dev *tty_dev = NULL;
#endif /* SDIO_MODEM_PORT */
	struct phy_dev *phy_dev = NULL;
	struct lte_sdev *sdev = NULL;

	drv_print(LTE_LOG_NOTICE, "+sdio_lte_remove\n");

	if (!func)
		return;
	do {
		sdio_claim_host(func);
		sdio_release_irq(func);
		sdio_disable_func(func);
		sdio_release_host(func);

#ifdef SDIO_MODEM_PORT
		mux_dev = sdio_get_drvdata(func);
		if (!mux_dev)
			break;

		phy_dev = mux_dev->phy_dev;
		tty_dev = mux_dev->tty_dev;

#else
		phy_dev = sdio_get_drvdata(func);
#endif /* SDIO_MODEM_PORT */
#ifdef SDIO_MODEM_PORT
		if (tty_dev)
			unregister_lte_tty_device(tty_dev);
#endif /* SDIO_MODEM_PORT */

		if (!phy_dev)
			break;

		unregister_lte_device(phy_dev);

		sdev = phy_dev->priv_dev;
		if (!sdev)
			break;

		release_sdio(sdev);
	} while (0);


	kfree(sdev);
	kfree(phy_dev);
#ifdef SDIO_MODEM_PORT
	kfree(tty_dev);
	kfree(mux_dev);
#endif /* SDIO_MODEM_PORT */

	drv_print(LTE_LOG_NOTICE, "-sdio_lte_remove\n");

	return;
}

static const struct sdio_device_id gdm_sdio_ids[] = {
	{ SDIO_DEVICE(0x0296, DEVICE_ID_7240) },	// VID,PID
	{ SDIO_DEVICE(0x0296, DEVICE_ID_7243) },	// VID,PID
	/* end : all zero */
	{0},
};

static struct sdio_driver sdio_lte_driver = {
	.probe = sdio_lte_probe,
	.remove = sdio_lte_remove,
	.name = "GCT LTE SDIO Driver",
	.id_table = gdm_sdio_ids,
};

static int __init sdio_lte_init(void)
{
	int ret = 0;

	drv_print(LTE_LOG_NOTICE, "+sdio_lte_init\n");

	lte_init_finish = 0;

	if (gdm_lte_event_init() < 0) {
		drv_print(LTE_LOG_ERROR, "Cannot Create Event\n");
		return -1;
	}

	sdio_lte_wq = create_workqueue("sdio_lte_wq\n");
	if (sdio_lte_wq == NULL)
		return -1;

	sdio_irq_wq = create_workqueue("sdio_irq_wq\n");
	if (sdio_irq_wq == NULL)
		return -1;

	ret = sdio_register_driver(&sdio_lte_driver);
#ifdef SDIO_MODEM_PORT
	if (!lte_init_finish)
		ret = register_lte_tty_driver();
#endif /* SDIO_MODEM_PORT */

	lte_init_finish = 1;

	drv_print(LTE_LOG_NOTICE, "-sdio_lte_init\n");

	return ret;
}

static void __exit sdio_lte_exit(void)
{
	drv_print(LTE_LOG_NOTICE, "+sdio_lte_exit\n");

	gdm_lte_event_exit();
	sdio_unregister_driver(&sdio_lte_driver);
#ifdef SDIO_MODEM_PORT
	unregister_lte_tty_driver();
#endif /* SDIO_MODEM_PORT */
	if (sdio_lte_wq) {
		flush_workqueue(sdio_lte_wq);
		destroy_workqueue(sdio_lte_wq);
	}

	if (sdio_irq_wq) {
		flush_workqueue(sdio_irq_wq);
		destroy_workqueue(sdio_irq_wq);
	}

	drv_print(LTE_LOG_NOTICE, "-sdio_lte_exit\n");
}

module_init(sdio_lte_init);
module_exit(sdio_lte_exit);

MODULE_VERSION(DRIVER_VERSION);
MODULE_DESCRIPTION("GCT LTE SDIO Device Driver");
MODULE_LICENSE("GPL");
