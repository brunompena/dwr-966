/*
 *  Lantiq AR10 GPIO button support
 *
 *  Copyright (C) 2014 - 2016 Ophas Chang <ophas.chang@wnc.com.tw>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR10_DEV_GPIO_BUTTONS_H
#define _AR10_DEV_GPIO_BUTTONS_H

#include <linux/input.h>
#include <linux/gpio_keys.h>

int ar10_register_gpio_keys_polled(int id,
				     unsigned poll_interval,
				     unsigned nbuttons,
				     struct gpio_keys_button *buttons);

#endif /* _AR10_DEV_GPIO_BUTTONS_H */
