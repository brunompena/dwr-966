#
# Makefile for the Switch API
#
#EXTRA_CFLAGS := -I./arch/mips/include/asm/mach-lantiq/switch-api
obj-$(CONFIG_LTQ_ETHSW_API)	+= drv_switch_api.o

drv_switch_api-objs	:= ltq_ethsw_ioctl_wrapper.o
drv_switch_api-objs	+= ltq_ethsw_init.o
drv_switch_api-objs += ltq_gphy_fw.o

ifeq ($(CONFIG_SOC_TYPE_XWAY),y)
  drv_switch_api-objs   += ltq_flow_core.o
  drv_switch_api-objs   += ltq_flow_core_ll_table.o
  drv_switch_api-objs   += ltq_flow_pce.o
  drv_switch_api-objs   += ltq_flow_ral.o
ifeq ($(CONFIG_LTQ_ETHSW_API_COC),y)
  EXTRA_CFLAGS += -DLTQ_ETHSW_API_COC
#  drv_switch_api-objs	+= ltq_ethsw_pm.o
#  drv_switch_api-objs	+= ltq_ethsw_pm_plat.o
ifeq ($(CONFIG_LTQ_ETHSW_API_COC_PMCU),y)
  EXTRA_CFLAGS += -DLTQ_ETHSW_API_COC_PMCU
#  drv_switch_api-objs	+= ltq_ethsw_pm_pmcu.o
endif
endif
endif

