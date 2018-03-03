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

#ifndef _GDM_LIST_H_
#define _GDM_LIST_H_

#include <linux/types.h>
#include <linux/list.h>
#include <linux/spinlock_types.h>
#include <linux/spinlock.h>

int gdm_list_empty(struct list_head *head, spinlock_t *lock);
void gdm_list_add(struct list_head *new, struct list_head *head, spinlock_t *lock);
void gdm_list_add_tail(struct list_head *new, struct list_head *haed, spinlock_t *lock);
void gdm_list_del(struct list_head *entry, spinlock_t *lock);

#define gdm_list_entry(ptr, type, member, entry, lock, flags)\
	({\
	spin_lock_irqsave(lock, flags); \
	entry = list_entry(ptr, type, member); \
	spin_unlock_irqrestore(lock, flags); \
	})
#endif
