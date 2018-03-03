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
#include <linux/version.h>
#include <linux/kernel.h>

#include <linux/usb.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "gdm_dbg.h"
#include "usb_boot.h"

#define USB_TX_BUF_SIZE 1024

static int gdm_lte_send(struct usb_device *usbdev, void *data, int len)
{
	int ret = 0;
	int actual;

	ret = usb_bulk_msg(usbdev, usb_sndbulkpipe(usbdev, 2), data, len,
			&actual, 1000);

	if (ret < 0)
		drv_print(LTE_LOG_ERROR, "Error : usb_bulk_msg result = %d\n", ret);

	return ret;
}

int image_download(struct usb_device *usbdev, const char *path)
{
	u8 *buf = NULL;
	struct file *filp;
	int len, ret = 0;
	loff_t pos = 0;
	long error_code;

	mm_segment_t old_fs = get_fs();

	set_fs(get_ds());

	filp = filp_open(path, O_RDONLY | O_LARGEFILE, 0);
	do {
		error_code = IS_ERR(filp);
		if (error_code) {
			ret = -1;
			drv_print(LTE_LOG_ERROR, "no search file %s [%ld]\n", path, error_code);
			break;
		}

		buf = kmalloc(USB_TX_BUF_SIZE, GFP_KERNEL);
		if (buf == NULL) {
			ret = -1;
			drv_print(LTE_LOG_ERROR, KERN_ERR "Error: kmalloc\n");
			break;
		}

		while ((len = filp->f_op->read(filp, buf, USB_TX_BUF_SIZE, &pos))) {
			if (len < 0) {
				ret = -1;
				drv_print(LTE_LOG_ERROR, "Error: file read\n");
				break;
			}

			ret = gdm_lte_send(usbdev, buf, len);
			if (ret < 0) {
				drv_print(LTE_LOG_ERROR, "Error : gdm_lte_send\n");
				break;
			}
		}
	} while (0);

	kfree(buf);

	set_fs(old_fs);

	if (!IS_ERR(filp))
		filp_close(filp, NULL);

	return ret;
}


