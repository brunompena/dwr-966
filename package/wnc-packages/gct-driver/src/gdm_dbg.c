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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "gdm_dbg.h"

/*
 * Default Log Level
 */
#define LTE_LOG_LEVEL	LTE_LOG_NOTICE
#define LTE_LOG_KERN


/*
 * Default Log Header
 */
#define LTE_LOG_HEADER	"lte: "


void drv_print(enum lte_log_level level, char *fmt, ...)
{
	va_list args;
	char *buff = NULL;

	if (level > LTE_LOG_LEVEL)
		return;

	buff = kmalloc(1024, GFP_ATOMIC);
	if (buff == NULL) {
		printk(LTE_LOG_KERN "printk buf alloc fail\n");
		return;
	}

	va_start(args, fmt);
	vsnprintf(buff, 1024, fmt, args);
	va_end(args);

	printk(LTE_LOG_KERN "%s%s", LTE_LOG_HEADER, buff);

	kfree(buff);
}

void drv_print_buf(enum lte_log_level level, char *name, u8 *buf, int len)
{
	int i;

	if (level > LTE_LOG_LEVEL)
		return;

	printk(LTE_LOG_KERN "%s%s", LTE_LOG_HEADER, name);
	for (i = 0; i < len; i++) {
		printk(LTE_LOG_KERN "%02x ", *buf++);
		if ((i % 16) == 15)
			printk(LTE_LOG_KERN "\n");
	}
	printk(LTE_LOG_KERN "\n");
}

