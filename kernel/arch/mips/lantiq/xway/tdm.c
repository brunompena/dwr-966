#include <linux/module.h>
#include <linux/of_platform.h>

static struct pinctrl *pin;
static struct pinctrl_state *pin_default, *pin_no_tx;

void tdm_default(void)
{
	pinctrl_select_state(pin, pin_default);
}
EXPORT_SYMBOL(tdm_default);

void tdm_no_tx(void)
{
	pinctrl_select_state(pin, pin_no_tx);
}
EXPORT_SYMBOL(tdm_no_tx);

static int tdm_probe(struct platform_device *pdev)
{
	pin = devm_pinctrl_get(&pdev->dev);
	pin_default = pinctrl_lookup_state(pin, PINCTRL_STATE_DEFAULT);
	pin_no_tx = pinctrl_lookup_state(pin, "no_tx");
	pinctrl_select_state(pin, pin_default);
	return 0;
}

static const struct of_device_id tdm_match[] = {
	{ .compatible = "lantiq,tdm" },
	{},
};
MODULE_DEVICE_TABLE(of, tdm_match);

static struct platform_driver tdm_driver = {
	.probe = tdm_probe,
	.driver = {
		.name = "lantiq,tdm",
		.owner = THIS_MODULE,
		.of_match_table = tdm_match,
	},
};

module_platform_driver(tdm_driver);

static int no_tx;
static int set_no_tx(const char *val, struct kernel_param *kp)
{
	int ret = param_set_int(val, kp);

	if (ret)
		return ret;

	if (no_tx)
		tdm_no_tx();
	else
		tdm_default();
	return 0;
}

static int get_no_tx(char *buffer, struct kernel_param *kp)
{
	param_set_int((char*) &no_tx, kp);
	return param_get_int(buffer, kp);
}

module_param_call(pcm_no_tx, set_no_tx, get_no_tx, &no_tx, S_IWUSR | S_IRUGO);

