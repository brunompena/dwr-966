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
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/slab.h>
#include <linux/usb/cdc.h>

#ifdef LTE_USB_PM
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
#error "Kernel version does not support hibernation & suspend."
#endif
#endif /* LTE_USB_PM */

#include "gdm_mux.h"
#include "gdm_tty.h"

#ifdef LTE_USB_PM
#define PM_NORMAL 0
#define PM_SUSPEND 1
#endif /* LTE_USB_PM */

#define USB_RT_ACM          (USB_TYPE_CLASS | USB_RECIP_INTERFACE)

#define START_FLAG 0xA512485A
#define MUX_HEADER_SIZE 14
#define MUX_RX_MAX_SIZE (1024*30)
#define AT_PKT_TYPE 0xF011
#define DM_PKT_TYPE 0xF010

#define RETRY_TIMER 30 /* msec */

struct workqueue_struct *mux_rx_wq;


static u16 PACKET_TYPE[TTY_MAX_COUNT] = {0xF011, 0xF010};

#define USB_DEVICE_CDC_DATA(vid, pid) \
	.match_flags = USB_DEVICE_ID_MATCH_DEVICE | USB_DEVICE_ID_MATCH_INT_CLASS | USB_DEVICE_ID_MATCH_INT_SUBCLASS,\
	.idVendor = vid,\
	.idProduct = pid,\
	.bInterfaceClass = USB_CLASS_COMM,\
	.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM

static const struct usb_device_id id_table[] = {
	{ USB_DEVICE_CDC_DATA(0x1076, 0x8000) }, /* GCT GDM7240 */
	{ USB_DEVICE_CDC_DATA(0x1076, 0x8f00) }, /* GCT GDM7243 */
	{ USB_DEVICE_CDC_DATA(0x1076, 0x9000) }, /* GCT GDM7243 */
	{ USB_DEVICE_CDC_DATA(0x1d74, 0x2300) }, /* LGIT Phoenix */
	{}
};


MODULE_DEVICE_TABLE(usb, id_table);

int PacketTypeToIndex(u16 packetType)
{
	int i;

	for (i = 0; i < TTY_MAX_COUNT; i++) {
		if (PACKET_TYPE[i] == packetType)
			return i;
	}

	printk(KERN_ERR "glte: invalid packet type:%d\n", packetType);

	return -1;
}

static struct mux_tx *alloc_mux_tx(int len)
{
	struct mux_tx *t = NULL;
	int ret = 0;


	t = kmalloc(sizeof(struct mux_tx), GFP_ATOMIC);
	if (!t) {
		ret = -ENOMEM;
		goto out;
	}
	memset(t, 0, sizeof(struct mux_tx));

	t->urb = usb_alloc_urb(0, GFP_ATOMIC);
	t->buf = kmalloc(1024*10, GFP_ATOMIC);
	if (!t->urb || !t->buf) {
		ret = -ENOMEM;
		goto out;
	}

out:

	if (ret < 0) {
		if (t) {
			usb_free_urb(t->urb);
			kfree(t->buf);
			kfree(t);
		}

		return NULL;
	}

	return t;
}

static void free_mux_tx(struct mux_tx *t)
{
	if (t) {
		usb_free_urb(t->urb);
		kfree(t->buf);
		kfree(t);
	}
}

static struct mux_rx *alloc_mux_rx(void)
{
	struct mux_rx *r = NULL;
	int ret = 0;


	r = kmalloc(sizeof(struct mux_rx), GFP_ATOMIC);
	if (!r) {
		ret = -ENOMEM;
		goto out;
	}

	r->urb = usb_alloc_urb(0, GFP_ATOMIC);
	r->buf = kmalloc(MUX_RX_MAX_SIZE, GFP_ATOMIC);
	if (!r->urb || !r->buf) {
		ret = -ENOMEM;
		goto out;
	}
out:

	if (ret < 0) {
		if (r) {
			usb_free_urb(r->urb);
			kfree(r->buf);
			kfree(r);
		}

		return NULL;
	}

	return r;
}

static void free_mux_rx(struct mux_rx *r)
{
	if (r) {
		usb_free_urb(r->urb);
		kfree(r->buf);
		kfree(r);
	}
}

static struct mux_rx *get_rx_struct(struct rx_cxt *rx)
{
	struct mux_rx *r;
	unsigned long flags;

	spin_lock_irqsave(&rx->free_list_lock, flags);

	if (list_empty(&rx->rx_free_list)) {
		spin_unlock_irqrestore(&rx->free_list_lock, flags);
		return NULL;
	}

	r = list_entry(rx->rx_free_list.prev, struct mux_rx, free_list);
	list_del(&r->free_list);

	spin_unlock_irqrestore(&rx->free_list_lock, flags);

	return r;
}

static void put_rx_struct(struct rx_cxt *rx, struct mux_rx *r)
{
	unsigned long flags;

	spin_lock_irqsave(&rx->free_list_lock, flags);
	list_add_tail(&r->free_list, &rx->rx_free_list);
	spin_unlock_irqrestore(&rx->free_list_lock, flags);
}


static int up_to_host(struct mux_rx *r)
{
	struct mux_dev *mux_dev = (struct mux_dev *)r->mux_dev;
	struct mux_pkt_header *mux_header = NULL;
	unsigned int start_flag;
	unsigned int payload_size;
	unsigned short packet_type;
	int remain;
	int dummy_cnt;
	u32 packet_size_sum = r->offset;
	u8 *buf;
	int index;
	int ret = 0;
	int len = r->len;

	while (1) {
		mux_header = (struct mux_pkt_header *)(r->buf + packet_size_sum);
		start_flag = L4H(mux_header->start_flag);
		payload_size = L4H(mux_header->payload_size);
		packet_type = L2H(mux_header->packet_type);

		if (start_flag != START_FLAG) {
			printk(KERN_ERR "glte: invalid START_FLAG %x\n", start_flag);
			break;
		}

		remain = (MUX_HEADER_SIZE + payload_size) % 4;
		dummy_cnt = remain ? (4-remain) : 0;

		if (len - packet_size_sum < MUX_HEADER_SIZE + payload_size + dummy_cnt) {
			printk(KERN_ERR "glte: invalid payload : %d len : %d pkt type : %04x\n",
			       payload_size, len,
			       packet_type
			       );
			break;
		}

		index = PacketTypeToIndex(packet_type);
		if (index < 0) {
			printk(KERN_ERR "glte: invalid index %d\n", index);
			break;
		}

		buf = (u8 *)(mux_header);
		ret = r->callback((void *)&buf[MUX_HEADER_SIZE],
				payload_size,
				index,
				mux_dev->minor[index],
				RECV_PACKET_PROCESS_CONTINUE
				);
		if (ret == TO_HOST_BUFFER_REQUEST_FAIL) {
			r->offset += packet_size_sum;
			break;
		}

		packet_size_sum += MUX_HEADER_SIZE + payload_size + dummy_cnt;

		if (len - packet_size_sum <= MUX_HEADER_SIZE + 2) {
			ret = r->callback((void *)NULL,
					0,
					index,
					mux_dev->minor[index],
					RECV_PACKET_PROCESS_COMPLETE
					);
			break;
		}
	}

	return ret;
}

static void do_rx(struct work_struct *work)
{
	struct mux_dev *mux_dev = container_of(work, struct mux_dev , work_rx.work);
	struct mux_rx *r;
	struct rx_cxt *rx = (struct rx_cxt *)&mux_dev->rx;
	unsigned long flags;
	int ret = 0;

	while (1) {
		spin_lock_irqsave(&rx->to_host_lock, flags);
		if (list_empty(&rx->to_host_list)) {
			spin_unlock_irqrestore(&rx->to_host_lock, flags);
			break;
		}
		r = list_entry(rx->to_host_list.next, struct mux_rx, to_host_list);
		list_del(&r->to_host_list);
		spin_unlock_irqrestore(&rx->to_host_lock, flags);

		ret = up_to_host(r);
		if (ret == TO_HOST_BUFFER_REQUEST_FAIL) {
			printk(KERN_ERR "glte: failed to send mux data to host\n");
		} else {
			put_rx_struct(rx, r);
		}
	}
}

static void remove_rx_submit_list(struct mux_rx *r, struct rx_cxt *rx)
{
	unsigned long flags;
	struct mux_rx	*r_remove, *r_remove_next;

	spin_lock_irqsave(&rx->submit_list_lock, flags);
	list_for_each_entry_safe(r_remove, r_remove_next, &rx->rx_submit_list, rx_submit_list)
	{
		if (r == r_remove)
			list_del(&r->rx_submit_list);
	}
	spin_unlock_irqrestore(&rx->submit_list_lock, flags);
}

static void gdm_mux_rcv_complete(struct urb *urb)
{
	struct mux_rx *r = urb->context;
	struct mux_dev *mux_dev = (struct mux_dev *)r->mux_dev;
	struct rx_cxt *rx = &mux_dev->rx;
	unsigned long flags;

	remove_rx_submit_list(r, rx);

	if (urb->status) {
#ifdef LTE_USB_PM
		if (mux_dev->usb_state == PM_NORMAL)
			printk(KERN_ERR "glte: gdm_mux_rcv_complete urb status error %d\n", urb->status);
#else
		printk(KERN_ERR "glte: gdm_mux_rcv_complete urb status error %d\n", urb->status);
#endif /* LTE_USB_PM */

		put_rx_struct(rx, r);
	} else {
		r->len = r->urb->actual_length;

		spin_lock_irqsave(&rx->to_host_lock, flags);
		list_add_tail(&r->to_host_list, &rx->to_host_list);
		queue_work(mux_rx_wq, &mux_dev->work_rx.work);
		spin_unlock_irqrestore(&rx->to_host_lock, flags);
	}
}

static int gdm_mux_recv(void *priv_dev,
			int (*cb)(void *data, int len, int tty_index, int minor, int complete)
			)
{
	struct mux_dev *mux_dev = priv_dev;
	struct usb_device *usbdev = mux_dev->usbdev;
	struct mux_rx *r;
	struct rx_cxt *rx = &mux_dev->rx;
	unsigned long flags;
	int ret;

	if (!usbdev) {
		printk(KERN_ERR "glte: Device is Disconnect\n");
		return -ENODEV;
	}

	r = get_rx_struct(rx);
	if (!r) {
		printk(KERN_ERR "glte: alloc_rx_struct fail\n");
		return -ENOMEM;
	}

	r->offset = 0;
	r->mux_dev = (void *)mux_dev;
	r->callback = cb;
#ifdef LTE_USB_PM
	mux_dev->rx_cb = cb;
#endif /* LTE_USB_PM */

	usb_fill_bulk_urb(r->urb,
			  usbdev,
			  usb_rcvbulkpipe(usbdev, 0x86),
			  r->buf,
			  MUX_RX_MAX_SIZE,
			  gdm_mux_rcv_complete,
			  r);

	spin_lock_irqsave(&rx->submit_list_lock, flags);
	list_add_tail(&r->rx_submit_list, &rx->rx_submit_list);
	spin_unlock_irqrestore(&rx->submit_list_lock, flags);

	ret = usb_submit_urb(r->urb, GFP_KERNEL);

	if (ret) {
		spin_lock_irqsave(&rx->submit_list_lock, flags);
		list_del(&r->rx_submit_list);
		spin_unlock_irqrestore(&rx->submit_list_lock, flags);

		put_rx_struct(rx, r);

		printk(KERN_ERR "glte: usb_submit_urb ret=%d\n", ret);
	}

#ifdef LTE_USB_PM
	usb_mark_last_busy(usbdev);
#endif /* LTE_USB_PM */

	return ret;
}

static void gdm_mux_send_complete(struct urb *urb)
{
	struct mux_tx *t = urb->context;

	if (urb->status == -ECONNRESET) {
		printk(KERN_INFO "glte: CONNRESET\n");
		free_mux_tx(t);
		return;
	}

	if (t->callback)
		t->callback(t->cb_data);

	free_mux_tx(t);
}

static int gdm_mux_send(void *priv_dev, void *data, int len, int tty_index,
			void (*cb)(void *data), void *cb_data)
{
	struct mux_dev *mux_dev = priv_dev;
	struct usb_device *usbdev = mux_dev->usbdev;
	struct mux_pkt_header *mux_header;
	struct mux_tx *t = NULL;
	static u32 seq_num = 1;
	int remain;
	int dummy_cnt;
	int total_len;
	int ret;
	unsigned long flags;

#ifdef LTE_USB_PM
	if (mux_dev->usb_state == PM_SUSPEND) {
		ret = usb_autopm_get_interface(mux_dev->intf);
		if (!ret)
			usb_autopm_put_interface(mux_dev->intf);
	}
#endif /* LTE_USB_PM */

	spin_lock_irqsave(&mux_dev->write_lock, flags);

	remain = (MUX_HEADER_SIZE + len) % 4;
	dummy_cnt = remain ? (4 - remain) : 0;

	total_len = len + MUX_HEADER_SIZE + dummy_cnt;

	t = alloc_mux_tx(total_len);
	if (!t) {
		printk(KERN_ERR "glte: alloc_mux_tx fail\n");
		spin_unlock_irqrestore(&mux_dev->write_lock, flags);
		return -ENOMEM;
	}

	mux_header = (struct mux_pkt_header *)t->buf;
	mux_header->start_flag = H4L(START_FLAG);
	mux_header->seq_num = H4L(seq_num++);
	mux_header->payload_size = H4L((u32)len);
	mux_header->packet_type = H2L(PACKET_TYPE[tty_index]);

	memcpy(t->buf+MUX_HEADER_SIZE, data, len);
	memset(t->buf+MUX_HEADER_SIZE+len, 0, dummy_cnt);

	t->len = total_len;
	t->callback = cb;
	t->cb_data = cb_data;

	usb_fill_bulk_urb(t->urb,
			  usbdev,
			  usb_sndbulkpipe(usbdev, 5),
			  t->buf,
			  total_len,
			  gdm_mux_send_complete,
			  t);

	ret = usb_submit_urb(t->urb, GFP_KERNEL);

	spin_unlock_irqrestore(&mux_dev->write_lock, flags);

	if (ret)
		printk(KERN_ERR "glte: usb_submit_urb Error : %d\n", ret);

#ifdef LTE_USB_PM
	usb_mark_last_busy(usbdev);
#endif /* LTE_USB_PM */

	return ret;
}

static int gdm_mux_send_control(void *priv_dev, int request, int value, void *buf, int len)
{
	struct mux_dev *mux_dev = priv_dev;
	struct usb_device *usbdev = mux_dev->usbdev;
	int ret;

	ret = usb_control_msg(usbdev,
			      usb_sndctrlpipe(usbdev, 0),
			      request,
			      USB_RT_ACM,
			      value,
			      2,
			      buf,
			      len,
			      5000
			     );

	if (ret < 0)
		printk(KERN_ERR "glte: usb_control_msg error : %d\n", ret);

	return ret < 0 ? ret : 0;
}

static void release_usb(struct mux_dev *mux_dev)
{
	struct rx_cxt		*rx = &mux_dev->rx;
	struct mux_rx		*r, *r_next;
	unsigned long		flags;

	cancel_delayed_work(&mux_dev->work_rx);

	spin_lock_irqsave(&rx->submit_list_lock, flags);
	list_for_each_entry_safe(r, r_next, &rx->rx_submit_list, rx_submit_list)
	{
		spin_unlock_irqrestore(&rx->submit_list_lock, flags);
		usb_kill_urb(r->urb);
		spin_lock_irqsave(&rx->submit_list_lock, flags);
	}
	spin_unlock_irqrestore(&rx->submit_list_lock, flags);

	spin_lock_irqsave(&rx->free_list_lock, flags);
	list_for_each_entry_safe(r, r_next, &rx->rx_free_list, free_list)
	{
		list_del(&r->free_list);
		free_mux_rx(r);
	}
	spin_unlock_irqrestore(&rx->free_list_lock, flags);

	spin_lock_irqsave(&rx->to_host_lock, flags);
	list_for_each_entry_safe(r, r_next, &rx->to_host_list, to_host_list)
	{
		if (r->mux_dev == (void *)mux_dev) {
			list_del(&r->to_host_list);
			free_mux_rx(r);
		}
	}
	spin_unlock_irqrestore(&rx->to_host_lock, flags);
}


static int init_usb(struct mux_dev *mux_dev)
{
	struct mux_rx *r;
	struct rx_cxt *rx = &mux_dev->rx;
	int ret = 0;
	int i;
	unsigned long flags;

	spin_lock_init(&mux_dev->write_lock);
	INIT_LIST_HEAD(&rx->to_host_list);
	INIT_LIST_HEAD(&rx->rx_submit_list);
	INIT_LIST_HEAD(&rx->rx_free_list);
	spin_lock_init(&rx->to_host_lock);
	spin_lock_init(&rx->submit_list_lock);
	spin_lock_init(&rx->free_list_lock);

	for (i = 0; i < MAX_ISSUE_NUM*2; i++) {
		r = alloc_mux_rx();
		if (r == NULL) {
			ret = -ENOMEM;
			break;
		}

		spin_lock_irqsave(&rx->free_list_lock, flags);
		list_add(&r->free_list, &rx->rx_free_list);
		spin_unlock_irqrestore(&rx->free_list_lock, flags);
	}

	INIT_DELAYED_WORK(&mux_dev->work_rx, do_rx);

	return ret;
}

static int gdm_mux_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct mux_dev *mux_dev = NULL;
	struct tty_dev *tty_dev = NULL;
	u16 idVendor, idProduct;
	int bInterfaceNumber;
	int ret = 0;
	int i;
	struct usb_device *usbdev = interface_to_usbdev(intf);
	bInterfaceNumber = intf->cur_altsetting->desc.bInterfaceNumber;

	idVendor = L2H(usbdev->descriptor.idVendor);
	idProduct = L2H(usbdev->descriptor.idProduct);

	printk(KERN_INFO "glte: mux vid = 0x%04x pid = 0x%04x\n",
	       idVendor, idProduct);

	if (bInterfaceNumber != 2) {
		ret = -1;
		goto out;
	}

	mux_dev = kmalloc(sizeof(struct mux_dev), GFP_KERNEL);
	if (!mux_dev) {
		ret = -ENOMEM;
		goto out;
	}

	tty_dev = kmalloc(sizeof(struct tty_dev), GFP_KERNEL);
	if (!tty_dev) {
		kfree(mux_dev);
		ret = -ENOMEM;
		goto out;
	}

	memset(mux_dev, 0, sizeof(struct mux_dev));
	memset(tty_dev, 0, sizeof(struct tty_dev));

	mux_dev->usbdev = usbdev;
	mux_dev->control_intf = intf;

	ret = init_usb(mux_dev);
	if (ret < 0)
		goto out;

	tty_dev->priv_dev = (void *)mux_dev;
	tty_dev->send_func = gdm_mux_send;
	tty_dev->recv_func = gdm_mux_recv;
	tty_dev->send_control = gdm_mux_send_control;

	if (register_lte_tty_device(tty_dev, &intf->dev) < 0) {
		unregister_lte_tty_device(tty_dev);
		mux_dev = tty_dev->priv_dev;

		ret = -1;
		goto out;
	}
	for (i = 0; i < TTY_MAX_COUNT; i++)
		mux_dev->minor[i] = tty_dev->minor[i];

out:
	
	if (ret >= 0) {
#ifdef LTE_USB_PM
		mux_dev->intf = intf;
		mux_dev->usb_state = PM_NORMAL;
#endif /* LTE_USB_PM */
	} else {
		kfree(tty_dev);

		if (mux_dev) {
			release_usb(mux_dev);
			kfree(mux_dev);
		}
	}

	usb_get_dev(usbdev);
	usb_set_intfdata(intf, tty_dev);

	return ret;
}

static void gdm_mux_disconnect(struct usb_interface *intf)
{
	struct tty_dev *tty_dev;
	struct mux_dev *mux_dev;
	struct usb_device *usbdev = interface_to_usbdev(intf);

	tty_dev = usb_get_intfdata(intf);

	mux_dev = tty_dev->priv_dev;

	release_usb(mux_dev);
	unregister_lte_tty_device(tty_dev);

	kfree(mux_dev);
	mux_dev = NULL;

	kfree(tty_dev);
	tty_dev = NULL;

	usb_put_dev(usbdev);
}

#ifdef LTE_USB_PM
static int gdm_mux_suspend(struct usb_interface *intf, pm_message_t pm_msg)
{
	struct tty_dev *tty_dev;
	struct mux_dev *mux_dev;
	struct rx_cxt *rx;
	struct mux_rx *r, *r_next;
	unsigned long flags;

	printk(KERN_INFO "gdm_mux_suspend\n");

	tty_dev = usb_get_intfdata(intf);
	mux_dev = tty_dev->priv_dev;
	rx = &mux_dev->rx;

	if (mux_dev->usb_state != PM_NORMAL) {
		printk(KERN_ERR "glte: usb suspend - invalid state\n");
		return -1;
	}

	mux_dev->usb_state = PM_SUSPEND;


	spin_lock_irqsave(&rx->submit_list_lock, flags);
	list_for_each_entry_safe(r, r_next, &rx->rx_submit_list, rx_submit_list) {
		spin_unlock_irqrestore(&rx->submit_list_lock, flags);
		usb_kill_urb(r->urb);
		spin_lock_irqsave(&rx->submit_list_lock, flags);
	}
	spin_unlock_irqrestore(&rx->submit_list_lock, flags);

	return 0;
}

static int gdm_mux_resume(struct usb_interface *intf)
{
	struct tty_dev *tty_dev;
	struct mux_dev *mux_dev;
	u8 i;

	printk(KERN_INFO "gdm_mux_resume\n");

	tty_dev = usb_get_intfdata(intf);
	mux_dev = tty_dev->priv_dev;

	if (mux_dev->usb_state != PM_SUSPEND) {
		printk(KERN_ERR "glte: usb resume - invalid state\n");
		return -1;
	}

	mux_dev->usb_state = PM_NORMAL;

	for (i = 0;  i < MAX_ISSUE_NUM; i++)
		gdm_mux_recv((void *)mux_dev, mux_dev->rx_cb);

	return 0;
}
#endif /* LTE_USB_PM */

static struct usb_driver gdm_mux_driver = {
	.name = "gdm_mux",
	.probe = gdm_mux_probe,
	.disconnect = gdm_mux_disconnect,
	.id_table = id_table,
#ifdef LTE_USB_PM
	.supports_autosuspend = 1,
	.suspend = gdm_mux_suspend,
	.resume = gdm_mux_resume,
	.reset_resume = gdm_mux_resume,
#endif /* LTE_USB_PM */
};

static int __init gdm_usb_mux_init(void)
{

	mux_rx_wq = create_workqueue("mux_rx_wq");
	if (mux_rx_wq == NULL) {
		printk(KERN_ERR "glte: work queue create fail");
		return -1;
	}

	register_lte_tty_driver();

	return usb_register(&gdm_mux_driver);
}

static void __exit gdm_usb_mux_exit(void)
{
	unregister_lte_tty_driver();

	if (mux_rx_wq) {
		flush_workqueue(mux_rx_wq);
		destroy_workqueue(mux_rx_wq);
	}

	usb_deregister(&gdm_mux_driver);
}

module_init(gdm_usb_mux_init);
module_exit(gdm_usb_mux_exit);

MODULE_DESCRIPTION("GCT LTE TTY Device Driver");
MODULE_LICENSE("GPL");
