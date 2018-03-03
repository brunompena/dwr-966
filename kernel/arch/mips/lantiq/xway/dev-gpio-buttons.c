/*
 *  Lantiq AR10 GPIO button support
 *
 *  Copyright (C) 2014 - 2016 Ophas Chang <ophas.chang@wnc.com.tw>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "linux/init.h"
#include "linux/slab.h"
#include <linux/platform_device.h>

#include "dev-gpio-buttons.h"

int __init ar10_register_gpio_keys_polled(int id,
					    unsigned poll_interval,
					    unsigned nbuttons,
					    struct gpio_keys_button *buttons)
{
	struct platform_device *pdev;
	struct gpio_keys_platform_data pdata;
	struct gpio_keys_button *p;
	int err = 0;

	p = kmemdup(buttons, nbuttons * sizeof(*p), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	pdev = platform_device_alloc("gpio-keys-polled", id);
	if (!pdev) {
		err = -ENOMEM;
		goto err_free_buttons;
	}
	memset(&pdata, 0, sizeof(pdata));
	pdata.poll_interval = poll_interval;
	pdata.nbuttons = nbuttons;
	pdata.buttons = p;

	err = platform_device_add_data(pdev, &pdata, sizeof(pdata));
	if (err)
		goto err_put_pdev;

	err = platform_device_add(pdev);
	if (err)
		goto err_put_pdev;

	return err;

err_put_pdev:
	platform_device_put(pdev);

err_free_buttons:
	kfree(p);

	return err;
}

/*
 *  Lantiq AR10 board support
 *
 *  Copyright (C) 2014 - 2016 Ophas Chang <ophas.chang@wnc.com.tw>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#define AR10_GPIO_BTN_ANT_SW	(194+11)
#define AR10_GPIO_BTN_WPS		(194+14)

#define AR10_KEYS_POLL_INTERVAL	20	/* msecs */
#define AR10_KEYS_DEBOUNCE_INTERVAL	(3 * AR10_KEYS_POLL_INTERVAL)

static struct gpio_keys_button ar10_gpio_keys[] __initdata = {
	{
		.desc		= "antenna switch",
		.type		= EV_KEY,
		.code		= BTN_SELECT,
		.debounce_interval = AR10_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AR10_GPIO_BTN_ANT_SW,
		.active_low	= 0,
	},
	{
		.desc		= "wps button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = AR10_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AR10_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static int __init ar10_keypad_init(void)
{
	return ar10_register_gpio_keys_polled(-1, AR10_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(ar10_gpio_keys),
					ar10_gpio_keys);
}

module_init(ar10_keypad_init);
