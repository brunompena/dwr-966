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

#include <linux/spinlock.h>

#include "gdm_list.h"


int gdm_list_empty(struct list_head *head, spinlock_t *lock)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(lock, flags);
	ret = list_empty(head);
	spin_unlock_irqrestore(lock, flags);

	return ret;
}

void gdm_list_add(struct list_head *new, struct list_head *head, spinlock_t *lock)
{
	unsigned long flags;

	spin_lock_irqsave(lock, flags);
	list_add(new, head);
	spin_unlock_irqrestore(lock, flags);
}

void gdm_list_add_tail(struct list_head *new, struct list_head *head, spinlock_t *lock)
{
	unsigned long flags;

	spin_lock_irqsave(lock, flags);
	list_add_tail(new, head);
	spin_unlock_irqrestore(lock, flags);
}

void gdm_list_del(struct list_head *entry, spinlock_t *lock)
{
	unsigned long flags;

	spin_lock_irqsave(lock, flags);
	list_del(entry);
	spin_unlock_irqrestore(lock, flags);
}
