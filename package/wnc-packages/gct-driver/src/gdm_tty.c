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
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb/cdc.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
#include <linux/serial.h>
#endif

#include "gdm_tty.h"

#define GDM_TTY_MAJOR 0
#define GDM_TTY_MINOR 32

#define ACM_CTRL_DTR 0x01
#define ACM_CTRL_RTS 0x02
#define ACM_CTRL_DSR 0x02
#define ACM_CTRL_RI  0x08
#define ACM_CTRL_DCD 0x01

#define WRITE_SIZE 2048

#define MUX_TX_MAX_SIZE 2048

#define gdm_tty_send(n, d, l, i, c, b) (\
	n->tty_dev->send_func(n->tty_dev->priv_dev, d, l, i, c, b))
#define gdm_tty_recv(n, c) (\
	n->tty_dev->recv_func(n->tty_dev->priv_dev, c))
#define gdm_tty_send_control(n, r, v, d, l) (\
	n->tty_dev->send_control(n->tty_dev->priv_dev, r, v, d, l))

#define acm_set_comm_feature(n, v)	\
	gdm_tty_send_control(n, 0x02, v, NULL, 0)

#define GDM_TTY_READY(tty_str) (tty_str && tty_str->tty_dev && tty_str->port.count)

struct tty_driver *g_tty_drv[TTY_MAX_COUNT] = {NULL, };
struct tty_str *g_tty_str[TTY_MAX_COUNT][GDM_TTY_MINOR] = {{NULL, }, };

static char *DRIVER_STRING[TTY_MAX_COUNT] = {"GCTATC", "GCTDM"};
static char *DEVICE_STRING[TTY_MAX_COUNT] = {"GCT-ATC", "GCT-DM"};

static DEFINE_MUTEX(open_mutex);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
static struct tty_port_operations gdm_tty_port_ops = {
};
#endif

static int gdm_tty_open(struct tty_struct *tty, struct file *filp)
{
	struct tty_str *tty_str = NULL;
	int i;
	int ret = 0;

	mutex_lock(&open_mutex);

	for (i = 0; i < TTY_MAX_COUNT; i++) {
		if (!strcmp(tty->driver->driver_name, DRIVER_STRING[i])) {
			tty_str = g_tty_str[i][tty->index];
			break;
		}
	}

	if (!tty_str) {
		printk(KERN_INFO "glte: no tty device\n");
		mutex_unlock(&open_mutex);
		return -ENODEV;
	}

	set_bit(TTY_NO_WRITE_SPLIT, &tty->flags);

	tty->driver_data = tty_str;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
	tty_port_tty_set(&tty_str->port, tty);
#else
	tty_str->port.tty = tty;
#endif
	tty_str->port.count++;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
	set_bit(ASYNCB_INITIALIZED, &tty_str->port.flags);
	ret = tty_port_block_til_ready(&tty_str->port, tty, filp);
#endif

	mutex_unlock(&open_mutex);

	return ret;
}

static void gdm_tty_close(struct tty_struct *tty, struct file *filp)
{
	struct tty_str *tty_str = tty->driver_data;
	int i;

	if (!tty_str) {
		printk(KERN_INFO "glte: tty device already close\n");
		return;
	}

	if (tty_str->port.count != 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
		tty_port_close_start(&tty_str->port, tty, filp);
		tty_port_close_end(&tty_str->port, tty);
#else
		tty_str->port.count--;
#endif

		if (tty_str->port.count == 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
			tty_port_tty_set(&tty_str->port, NULL);
#else
			tty_str->port.tty = NULL;
#endif
	}

	if (!tty_str->tty_dev) {
		for (i = 0; i < TTY_MAX_COUNT; i++) {
			if (!strcmp(tty->driver->driver_name, DRIVER_STRING[i]))
				break;
		}

		if (i < TTY_MAX_COUNT) {
			tty_unregister_device(g_tty_drv[i], tty->index);
			g_tty_str[i][tty->index] = NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
			tty_port_tty_set(&tty_str->port, NULL);
#else
			tty_str->port.tty = NULL;
#endif
			kfree(tty_str);
			printk(KERN_INFO "glte: unregister %s\n", DEVICE_STRING[i]);
		}
	}

	tty->driver_data = NULL;
}

static int gdm_tty_recv_complete(void *data, int len, int index, int minor, int complete)
{
	struct tty_str *tty_str = g_tty_str[index][minor];
	struct tty_struct *tty;

	if (!GDM_TTY_READY(tty_str)) {
#ifdef LTE_USB
		if (complete == RECV_PACKET_PROCESS_COMPLETE)
			gdm_tty_recv(tty_str, gdm_tty_recv_complete);
#endif /* LTE_USB */
		return TO_HOST_PORT_CLOSE;
	}

#ifdef LTE_USB
	if (!data || !len)
		goto complete_routine;
#endif /* LTE_USB */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
	tty = tty_port_tty_get(&tty_str->port);
#else
	tty = tty_str->port.tty;
#endif

	if (tty_buffer_request_room(tty, len) == len) {
		tty_insert_flip_string(tty, data, len);
		tty_flip_buffer_push(tty);
	} else {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
		tty_kref_put(tty);
#endif
		return TO_HOST_BUFFER_REQUEST_FAIL;
	}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
	tty_kref_put(tty);
#endif
#ifdef LTE_USB
complete_routine:
	if (complete == RECV_PACKET_PROCESS_COMPLETE)
		gdm_tty_recv(tty_str, gdm_tty_recv_complete);
#endif /* LTE_USB */

	return TO_HOST_SUCCESS;
}

static void gdm_tty_send_complete(void *arg)
{
	struct tty_str *tty_str = (struct tty_str *)arg;
	struct tty_struct *tty;

	return;

	if (!GDM_TTY_READY(tty_str))
		return;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
	tty = tty_port_tty_get(&tty_str->port);
	tty_wakeup(tty);
	tty_kref_put(tty);
#else
	tty = tty_str->port.tty;
	tty_wakeup(tty);
#endif
}

static int gdm_tty_write(struct tty_struct *tty, const unsigned char *buf, int len)
{
	struct tty_str *tty_str = tty->driver_data;
	int remain = len;
	int sent_len = 0;
	int sending_len = 0;

	if (!GDM_TTY_READY(tty_str))
		return -ENODEV;

	if (!len)
		return 0;

	while (1) {
		sending_len = remain > MUX_TX_MAX_SIZE ? MUX_TX_MAX_SIZE : remain;
		gdm_tty_send(tty_str,
			     (void *)(buf+sent_len),
			     sending_len,
			     tty_str->tty_drv_index,
			     gdm_tty_send_complete,
			     tty_str
			    );
		sent_len += sending_len;
		remain -= sending_len;
		if (remain <= 0)
			break;
	}

	return len;
}

static int gdm_tty_write_room(struct tty_struct *tty)
{
	struct tty_str *tty_str = tty->driver_data;

	if (!GDM_TTY_READY(tty_str))
		return -ENODEV;

	return WRITE_SIZE;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
static int gdm_tty_tiocmget(struct tty_struct *tty)
#else
static int gdm_tty_tiocmget(struct tty_struct *tty, struct file *file)
#endif
{
	struct tty_str *tty_str = tty->driver_data;

	if (!GDM_TTY_READY(tty_str))
		return -ENODEV;

	return (0 & ACM_CTRL_DTR ? TIOCM_DTR : 0) |
	       (0 & ACM_CTRL_RTS ? TIOCM_RTS : 0) |
	       (0 & ACM_CTRL_DSR ? TIOCM_DSR : 0) |
	       (0 & ACM_CTRL_RI  ? TIOCM_RI  : 0) |
	       (0 & ACM_CTRL_DCD ? TIOCM_CD  : 0) |
	       TIOCM_CTS;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
static int gdm_tty_tiocmset(struct tty_struct *tty, unsigned int set, unsigned int clear)
#else
static int gdm_tty_tiocmset(struct tty_struct *tty, struct file *file, unsigned int set, unsigned int clear)
#endif
{
	struct tty_str *tty_str = tty->driver_data;

	if (!GDM_TTY_READY(tty_str))
		return -ENODEV;

	return 1;
}

int register_lte_tty_device(struct tty_dev *tty_dev, struct device *dev)
{
	struct tty_str *tty_str;
	u8 i, j;

	for (i = 0; i < TTY_MAX_COUNT; i++) {
		for (j = 0; j < GDM_TTY_MINOR; j++) {
			if (!g_tty_str[i][j])
				break;
		}

		if (j == GDM_TTY_MINOR) {
			tty_dev->minor[i] = j;
			return -1;
		}

		tty_str = kmalloc(sizeof(struct tty_str), GFP_KERNEL);
		if (!tty_str)
			return -ENOMEM;

		g_tty_str[i][j] = tty_str;

		tty_str->tty_dev = tty_dev;
		tty_str->tty_drv_index = i;
		tty_dev->minor[i] = j;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
		tty_port_init(&tty_str->port);
		tty_str->port.ops = &gdm_tty_port_ops;
#else
		tty_str->port.count = 0;
#endif

		if (strcmp(DEVICE_STRING[i], "GCT-ATC") != 0)
			dev = NULL;
		tty_register_device(g_tty_drv[i], j, dev);

		printk(KERN_INFO "glte: register %s\n", DEVICE_STRING[i]);
	}

#ifdef LTE_USB
	acm_set_comm_feature(tty_str, 1);

	for (i = 0; i < MAX_ISSUE_NUM; i++)
		gdm_tty_recv(tty_str, gdm_tty_recv_complete);
#else
	gdm_tty_recv(tty_str, gdm_tty_recv_complete);
#endif /* LTE_USB */

	return 0;
}

void unregister_lte_tty_device(struct tty_dev *tty_dev)
{
	struct tty_str *tty_str;
	struct tty_struct *tty;
	int i;

	for (i = 0; i < TTY_MAX_COUNT; i++) {
		if (tty_dev->minor[i] >= GDM_TTY_MINOR)
			continue;

		tty_str = g_tty_str[i][tty_dev->minor[i]];
		if (!tty_str)
			continue;

		tty_str->tty_dev = NULL;

		if (!tty_str->port.count) {
			tty_unregister_device(g_tty_drv[i], tty_dev->minor[i]);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
			tty_port_tty_set(&tty_str->port, NULL);
#else
			tty_str->port.tty = NULL;
#endif
			kfree(tty_str);
			g_tty_str[i][tty_dev->minor[i]] = NULL;
			printk(KERN_INFO "glte: unregister %s\n", DEVICE_STRING[i]);
		} else {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
			tty = tty_port_tty_get(&tty_str->port);
			if (tty) {
				tty_hangup(tty);
				tty_kref_put(tty);
			}
#else
			tty = tty_str->port.tty;
			if (tty)
				tty_hangup(tty);
#endif
		}
	}
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 28)
static void gdm_tty_throttle(struct tty_struct *tty)
{
}

static void gdm_tty_unthrottle(struct tty_struct *tty)
{
}

static int gdm_tty_ioctl(struct tty_struct *tty, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct tty_str *tty_str = tty->driver_data;

	return -ENOIOCTLCMD;
}

static int gdm_tty_chars_in_buffer(struct tty_struct *tty)
{
	return WRITE_SIZE;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
static void gdm_tty_set_termios(struct tty_struct *tty, struct ktermios *termios_old)
{
	return;
}
#endif

static const struct tty_operations gdm_tty_ops = {
	.open = gdm_tty_open,
	.close = gdm_tty_close,
	.write = gdm_tty_write,
	.write_room = gdm_tty_write_room,
	.tiocmget = gdm_tty_tiocmget,
	.tiocmset = gdm_tty_tiocmset,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 28)
	.throttle = gdm_tty_throttle,
	.unthrottle = gdm_tty_unthrottle,
	.ioctl = gdm_tty_ioctl,
	.chars_in_buffer = gdm_tty_chars_in_buffer,
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20)
	.set_termios = gdm_tty_set_termios,
#endif
};

int register_lte_tty_driver(void)
{
	struct tty_driver *tty_driver = NULL;
	int i;
	int ret;

	for (i = 0; i < TTY_MAX_COUNT; i++) {
		tty_driver = alloc_tty_driver(GDM_TTY_MINOR);
		if (!tty_driver) {
			printk(KERN_ERR "glte: alloc_tty_driver fail\n");
			return -ENOMEM;
		}

		tty_driver->owner = THIS_MODULE;
		tty_driver->driver_name = DRIVER_STRING[i];
		tty_driver->name = DEVICE_STRING[i];
		tty_driver->major = GDM_TTY_MAJOR;
		tty_driver->type = TTY_DRIVER_TYPE_SERIAL;
		tty_driver->subtype = SERIAL_TYPE_NORMAL;
		tty_driver->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
		tty_driver->init_termios = tty_std_termios;
		tty_driver->init_termios.c_cflag = B9600 | CS8 | HUPCL | CLOCAL;
		tty_driver->init_termios.c_lflag = ISIG | ICANON | IEXTEN;
		tty_set_operations(tty_driver, &gdm_tty_ops);

		ret = tty_register_driver(tty_driver);

		g_tty_drv[i] = tty_driver;
	}

	return ret;
}

void unregister_lte_tty_driver(void)
{
	struct tty_driver *tty_driver;
	int i;

	for (i = 0; i < TTY_MAX_COUNT; i++) {
		tty_driver = g_tty_drv[i];
		if (tty_driver) {
			tty_unregister_driver(tty_driver);
			put_tty_driver(tty_driver);
		}
	}
}
