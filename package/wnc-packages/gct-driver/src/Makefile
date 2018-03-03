SRC_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
-include $(SRC_DIR)/.config
CONFIG_LTE_GDM724X=m

ifeq ("$(CONFIG_LTE_GDM724X_SDIO)", "y")
obj-$(CONFIG_LTE_GDM724X) += gdmslte.o
gdmslte-objs := gdm_lte.o netlink_k.o gdm_list.o gdm_nv_fw.o gdm_dbg.o
gdmslte-objs += gdm_sdio.o sdio_boot.o gdm_endian.o
gdmslte-objs += gdm_tty.o
ifeq ("$(CONFIG_LTE_GDM724X_SDIO_BOOT)", "y")
EXTRA_CFLAGS += -DSDIO_BOOT
endif
ifeq ("$(CONFIG_LTE_GDM724X_SDIO_MODEM_PORT)", "y")
EXTRA_CFLAGS += -DSDIO_MODEM_PORT
endif
ifeq ("$(CONFIG_LTE_GDM724X_SDIO_AT_COMMAND_EXTENDED)", "y")
EXTRA_CFLAGS += -DSDIO_AT_COMMAND_EXTENDED
endif
ifeq ("$(CONFIG_LTE_GDM724X_SDIO_DEVICE_STATUS_CHECK)", "y")
EXTRA_CFLAGS += -DSDIO_DEVICE_STATUS_CHECK
endif
endif

ifeq ("$(CONFIG_LTE_GDM724X_USB)", "y")
obj-$(CONFIG_LTE_GDM724X) += gdmulte.o
gdmulte-objs := gdm_lte.o netlink_k.o gdm_list.o gdm_nv_fw.o gdm_dbg.o
gdmulte-objs += gdm_usb.o usb_boot.o  gdm_endian.o
obj-$(CONFIG_LTE_GDM724X) += gdmtty.o
gdmtty-objs := gdm_tty.o gdm_mux.o gdm_dbg.o gdm_list.o
EXTRA_CFLAGS += -DLTE_USB
#EXTRA_CFLAGS += -DLTE_USB_PM
endif

ifeq ("$(CONFIG_LTE_GDM724X_TX_DELAY)", "y")
EXTRA_CFLAGS += -DTX_DELAY
endif
ifeq ("$(CONFIG_LTE_GDM724X_NV_HOST)", "y")
EXTRA_CFLAGS += -DNV_HOST 
endif
ifeq ("$(CONFIG_LTE_GDM724X_RX_FLOW_CONTROL)", "y")
EXTRA_CFLAGS += -DRX_FLOW_CONTROL
endif

all:
	$(MAKE) -C $(KERNELDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(SRC_DIR) SUBDIRS=$(SRC_DIR) modules

clean:
	$(MAKE) -C $(KERNELDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(SRC_DIR) SUBDIRS=$(SRC_DIR) clean

install:
	$(MAKE) -C $(KERNELDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(SRC_DIR) SUBDIRS=$(SRC_DIR) modules_install
