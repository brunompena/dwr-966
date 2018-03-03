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

#ifndef _GDM_DBG_H_
#define _GDM_DBG_H_

enum lte_log_level {
	LTE_LOG_NONE = 0,
	LTE_LOG_ERROR = 1,
	LTE_LOG_NOTICE = 2,
	LTE_LOG_FUNCTION = 3,
	LTE_LOG_DATA = 4,
};

void drv_print(enum lte_log_level level, char *fmt, ...);
void drv_print_buf(enum lte_log_level level, char *name, u8 *buf, int len);

#endif /* _GDM_DBG_H_ */

