#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

LANTIQ_MENU:=Lantiq
SUBTARGET_SUFFIX:=$(shell echo $(subst .,_,$(subst -,_,$(subst /,_,$(SUBTARGET)))) | cut -d_ -f2-)

define KernelPackage/lantiq-deu
  TITLE:=Lantiq data encryption unit
  SUBMENU:=$(CRYPTO_MENU)
  DEPENDS:=@TARGET_lantiq
  KCONFIG:=CONFIG_CRYPTO_DEV_LANTIQ \
	   CONFIG_CRYPTO_HW=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_AES=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_DES=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_MD5=y \
	   CONFIG_CRYPTO_DEV_LANTIQ_SHA1=y
  $(call AddDepends/crypto)
endef

define KernelPackage/lantiq-deu/description
  Kernel support for the Lantiq crypto HW
endef

$(eval $(call KernelPackage,lantiq-deu))

define KernelPackage/lantiq-pcie
  TITLE:=Lantiq PCIe Bus Support
  SUBMENU:=$(LANTIQ_MENU)
  DEPENDS:=@TARGET_lantiq
  KCONFIG:=\
	CONFIG_PCI=y \
	CONFIG_PCI_MSI=y \
	CONFIG_PCI_DOMAINS=y \
	CONFIG_PCIEPORTBUS=y \
	CONFIG_PCIE_LANTIQ=y
endef

define KernelPackage/lantiq-pcie-xrx200
  $(call KernelPackage/lantiq-pcie)
  TITLE+=for xRX200
  DEPENDS+=@TARGET_lantiq_xrx200 @!TARGET_lantiq_xrx200_EASY4210NAND
  KCONFIG+=\
	CONFIG_LANTIQ_PCIE_PHY_36MHZ_SSC_MODE=y \
	CONFIG_LANTIQ_PCIE_1ST_CORE=y \
	CONFIG_LANTIQ_PCIE_HW_SWAP=y \
	CONFIG_LANTIQ_PCIE_INBOUND_NO_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE_RST_EP_ACTIVE_HIGH=n \
	CONFIG_LANTIQ_PCIE_2ND_CORE=n \
	CONFIG_LANTIQ_PCIE_3RD_CORE=n

endef

define KernelPackage/lantiq-pcie-xrx200/description
  Kernel support for the Lantiq PCIe bus on xRX200
endef

$(eval $(call KernelPackage,lantiq-pcie-xrx200))

define KernelPackage/lantiq-pcie-grx200
  $(call KernelPackage/lantiq-pcie)
  TITLE+=for GRX200
  DEPENDS+=@TARGET_lantiq_xrx200_EASY4210NAND
  KCONFIG+=\
	CONFIG_LANTIQ_PCIE_PHY_25MHZ_MODE=y \
	CONFIG_LANTIQ_PCIE_1ST_CORE=y \
	CONFIG_LANTIQ_PCIE_HW_SWAP=y \
	CONFIG_LANTIQ_PCIE_INBOUND_NO_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE_RST_EP_ACTIVE_HIGH=n \
	CONFIG_LANTIQ_PCIE_2ND_CORE=n \
	CONFIG_LANTIQ_PCIE_3RD_CORE=n

endef

define KernelPackage/lantiq-pcie-grx200/description
  Kernel support for the Lantiq PCIe bus on GRX200
endef

$(eval $(call KernelPackage,lantiq-pcie-grx200))

define KernelPackage/lantiq-pcie-xrx300
  $(call KernelPackage/lantiq-pcie)
  TITLE+=for xRX300
  DEPENDS+=@TARGET_lantiq_xrx300
  KCONFIG+=\
	CONFIG_LANTIQ_PCIE_PHY_36MHZ_SSC_MODE=y \
	CONFIG_LANTIQ_PCIE_1ST_CORE=y \
	CONFIG_LANTIQ_PCIE_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE_INBOUND_NO_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE_RST_EP_ACTIVE_HIGH=n \
	CONFIG_LANTIQ_PCIE_2ND_CORE=y \
	CONFIG_LANTIQ_PCIE1_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE1_INBOUND_NO_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE1_RST_EP_ACTIVE_HIGH=n \
	CONFIG_LANTIQ_PCIE_3RD_CORE=n

endef

define KernelPackage/lantiq-pcie-xrx300/description
  Kernel support for the Lantiq PCIe bus on xRX300
endef

$(eval $(call KernelPackage,lantiq-pcie-xrx300))

define KernelPackage/lantiq-pcie-vrx388-bond
  $(call KernelPackage/lantiq-pcie)
  TITLE+=for VRX388 BOND
  DEPENDS+=@TARGET_lantiq_xrx300_EASY388_BOND +kmod-lantiq-pcie-vrx318
  KCONFIG+=\
	CONFIG_LANTIQ_PCIE_PHY_36MHZ_SSC_MODE=y \
	CONFIG_LANTIQ_PCIE_1ST_CORE=y \
	CONFIG_LANTIQ_PCIE_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE_INBOUND_NO_HW_SWAP=y \
	CONFIG_LANTIQ_PCIE_RST_EP_ACTIVE_HIGH=y \
	CONFIG_LANTIQ_PCIE_2ND_CORE=y \
	CONFIG_LANTIQ_PCIE1_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE1_INBOUND_NO_HW_SWAP=y \
	CONFIG_LANTIQ_PCIE1_RST_EP_ACTIVE_HIGH=y \
	CONFIG_LANTIQ_PCIE_3RD_CORE=n
endef

define KernelPackage/lantiq-pcie-vrx388-bond/description
  Kernel support for the Lantiq PCIe bus on VRX388 BOND
endef

$(eval $(call KernelPackage,lantiq-pcie-vrx388-bond))

define KernelPackage/lantiq-pcie-grx300v2
  $(call KernelPackage/lantiq-pcie)
  TITLE+=for GRX300v2
  DEPENDS+=@TARGET_lantiq_xrx330
  KCONFIG+=\
	CONFIG_LANTIQ_PCIE_PHY_36MHZ_SSC_MODE=y \
	CONFIG_LANTIQ_PCIE_1ST_CORE=y \
	CONFIG_LANTIQ_PCIE_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE_INBOUND_NO_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE_RST_EP_ACTIVE_HIGH=n \
	CONFIG_LANTIQ_PCIE_2ND_CORE=y \
	CONFIG_LANTIQ_PCIE1_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE1_INBOUND_NO_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE1_RST_EP_ACTIVE_HIGH=n \
	CONFIG_LANTIQ_PCIE_3RD_CORE=n \
	CONFIG_LANTIQ_PCIE2_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE2_INBOUND_NO_HW_SWAP=n \
	CONFIG_LANTIQ_PCIE2_RST_EP_ACTIVE_HIGH=n

endef

define KernelPackage/lantiq-pcie-grx300v2/description
  Kernel support for the Lantiq PCIe bus on GRX300v2
endef

$(eval $(call KernelPackage,lantiq-pcie-grx300v2))

define KernelPackage/lantiq-pcie-vrx318
  TITLE:=PCIe Support for VRX318
  SUBMENU:=$(LANTIQ_MENU)
  DEPENDS:=@(PACKAGE_kmod-lantiq-pcie-xrx300&&(TARGET_lantiq_xrx300_EASY388_VRX318||TARGET_lantiq_xrx300_EASY388_BOND))||PACKAGE_kmod-lantiq-pcie-grx300v2
  KCONFIG:=\
	CONFIG_LANTIQ_VRX318=y
endef

define KernelPackage/lantiq-pcie-vrx318/description
  Kernel support for the Lantiq PCIe bus on vrx318
endef

$(eval $(call KernelPackage,lantiq-pcie-vrx318))

define KernelPackage/lantiq-pcie-vrxmei
  TITLE:=PCIe Support for MEI for VRX
  SUBMENU:=$(LANTIQ_MENU)
  DEPENDS:=@PACKAGE_kmod-lantiq-pcie-xrx200
  KCONFIG:=\
	CONFIG_LANTIQ_VRX318=y
endef

define KernelPackage/lantiq-pcie-vrxmei/description
  Kernel support for the Lantiq PCIe bus
endef

$(eval $(call KernelPackage,lantiq-pcie-vrxmei))

USB_MENU:=USB Support

define KernelPackage/usb-xhci
  TITLE:=USB 3.0 XHCI support
  SUBMENU:=$(USB_MENU)
  DEPENDS+=kmod-usb-core +kmod-usb-storage
  KCONFIG:= \
	CONFIG_USB_XHCI_HCD=m \
	CONFIG_USB_SUPPORT=y \
	CONFIG_USB_ARCH_HAS_HCD=y \
	CONFIG_USB=y
  FILES:=$(LINUX_DIR)/drivers/usb/host/xhci-hcd.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,usb-xhci,1)
endef

define KernelPackage/usb-xhci/description
  Kernel support USB 3.0 XHCI support XWAY
endef

$(eval $(call KernelPackage,usb-xhci))

define KernelPackage/usb-ltqhcd
  TITLE:=LTQHCD usb driver
  SUBMENU:=$(USB_MENU)
  DEPENDS+=kmod-usb-core +kmod-usb-storage
  KCONFIG:=CONFIG_USB_HOST_LTQ \
	CONFIG_USB_HOST_LTQ_FORCE_USB11=n \
	CONFIG_USB_HOST_LTQ_WITH_HS_ELECT_TST=n \
	CONFIG_USB_HOST_LTQ_WITH_ISO=n \
	CONFIG_USB_HOST_LTQ_UNALIGNED_ADJ=y
  FILES:=$(LINUX_DIR)/drivers/usb/host/ltqusb_host.ko
  AUTOLOAD:=$(call AutoLoad,50,ltqusb_host)
endef

define KernelPackage/usb-ltqhcd/description
  Kernel support for Synopsis USB on XWAY
endef

$(eval $(call KernelPackage,usb-ltqhcd))

I2C_LANTIQ_MODULES:= \
  CONFIG_I2C_LANTIQ:drivers/i2c/busses/i2c-lantiq

define KernelPackage/i2c-lantiq
  TITLE:=Lantiq I2C controller
  $(call i2c_defaults,$(I2C_LANTIQ_MODULES),52)
  DEPENDS:=kmod-i2c-core @(TARGET_lantiq_falcon)
endef

define KernelPackage/i2c-lantiq/description
  Kernel support for the Lantiq I2C controller
endef

$(eval $(call KernelPackage,i2c-lantiq))

define KernelPackage/lantiq-vpe
  TITLE:=Lantiq VPE extensions
  SUBMENU:=Lantiq
  DEPENDS:=@TARGET_lantiq +kmod-vpe
  KCONFIG:= \
	  CONFIG_LTQ_VPE_EXT=y \
	  CONFIG_SOFT_WATCHDOG_VPE=y \
	  CONFIG_MTSCHED=y \
	  CONFIG_PERFCTRS=n
endef

define KernelPackage/lantiq-vpe/description
  Kernel extensions for the Lantiq SoC
endef

$(eval $(call KernelPackage,lantiq-vpe))

define KernelPackage/smvp
  TITLE:=SMVP configuration
  SUBMENU:=Lantiq
  DEPENDS:=@TARGET_lantiq @!(PACKAGE_kmod-vpe)
  KCONFIG:= \
        CONFIG_MTSCHED=y \
        CONFIG_MIPS_MT_SMP=y \
        CONFIG_SCHED_SMT=y \
        CONFIG_SYS_SUPPORTS_SCHED_SMT=y \
        CONFIG_LTQ_MIPS_MT_SMP_IRQAFF=y
endef

define KernelPackage/smvp/description
  Enable SMVP related kernel configuration
endef

$(eval $(call KernelPackage,smvp))

define KernelPackage/malta
  TITLE:=malta configuration
  SUBMENU:=Lantiq
  KCONFIG:= \
CONFIG_MIPS_MALTA=y \
CONFIG_EVA_OLD_MALTA_MAP=y \
CONFIG_EVA_3GB=y
endef

define KernelPackage/malta/description
  Enable SMVP related kernel configuration
endef

$(eval $(call KernelPackage,malta))

define KernelPackage/grx500-emu
  TITLE:=Enable GRX500 SOC based Emulator Config (PALLADIUM)
  SUBMENU:=Lantiq
  KCONFIG:= \
	CONFIG_SOC_TYPE_XWAY=n \
	CONFIG_SOC_XWAY=n \
	CONFIG_SOC_GRX500=y \
	CONFIG_USE_EMULATOR=y \
	CONFIG_USE_PALLADIUM=y
endef

define KernelPackage/grx500-emu/description
  Enable GRX500 SOC based Emulator Config (PALLADIUM)  
endef

$(eval $(call KernelPackage,grx500-emu))

define KernelPackage/lantiq-nf
  TITLE:=Lantiq NF extensions
  SUBMENU:=Lantiq
  DEPENDS:=@TARGET_lantiq
  KCONFIG:=CONFIG_NF_CONNTRACK_EVENTS=y
endef

define KernelPackage/lantiq-nf/description
  Netfilter extensions for the Lantiq SoC
endef

$(eval $(call KernelPackage,lantiq-nf))

define KernelPackage/lantiq_sata_ahci
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=Lantiq SATA AHCI support
  DEPENDS:=@TARGET_lantiq
  KCONFIG:= \
    CONFIG_ATA=y \
    CONFIG_SATA_AHCI=m \
    CONFIG_PCI=y
 
  FILES:=\
	$(LINUX_DIR)/drivers/ata/ahci.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/ata/libahci.$(LINUX_KMOD_SUFFIX)
endef
 
define KernelPackage/lantiq_sata_ahci/description
Kernel module for Lantiq SATA AHCI package
endef
 
$(eval $(call KernelPackage,lantiq_sata_ahci))

define KernelPackage/fs-ext3
  SUBMENU:=Filesystems
  TITLE:=EXT3 filesystem support
  KCONFIG:= \
        CONFIG_EXT3_FS \
        CONFIG_JBD
  FILES:= \
        $(LINUX_DIR)/fs/ext3/ext3.ko \
        $(LINUX_DIR)/fs/jbd/jbd.ko \
        $(LINUX_DIR)/fs/mbcache.ko
endef

define KernelPackage/fs-ext3/description
 Kernel module for EXT3 filesystem support
endef

$(eval $(call KernelPackage,fs-ext3))

define KernelPackage/ubi_mtd
 SUBMENU:=Lantiq
 TITLE:=UBI Support
 DEPENDS:=@TARGET_lantiq
 KCONFIG:= \
	CONFIG_MTD_UBI=y \
	CONFIG_MTD_UBI_BEB_RESERVE=1 \
	CONFIG_MTD_UBI_GLUEBI=y \
	CONFIG_MTD_UBI_GLUEBI_ROOTFS_DEV=y \
	CONFIG_MTD_UBI_DEBUG=n
endef
# CONFIG_MTD_UBI_WL_THRESHOLD is moved to platformX/configs

define KernelPackage/ubi_mtd/description
  Kernel Built-in support for ubi and gluebi mtd
endef

$(eval $(call KernelPackage,ubi_mtd))

define KernelPackage/ubifs
  SUBMENU:=Lantiq
  TITLE:=UBI Filesystem Support
  DEPENDS:=@TARGET_lantiq +kmod-ubi_mtd
  KCONFIG:= \
	CONFIG_UBIFS_FS=y \
	CONFIG_UBIFS_FS_XATTR=n \
	CONFIG_UBIFS_FS_ADVANCED_COMPR=y \
	CONFIG_UBIFS_FS_LZO=y \
	CONFIG_UBIFS_FS_ZLIB=n \
	CONFIG_UBIFS_FS_DEBUG=n
endef

define KernelPackage/ubifs/description
  Kernel Built-in support for ubi filesystem
endef

$(eval $(call KernelPackage,ubifs))

define KernelPackage/atm_stack
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=ATM stack
  DEPENDS:=@TARGET_lantiq
  KCONFIG:= \
	CONFIG_ATM=y \
	CONFIG_ATM_CLIP=y \
	CONFIG_ATM_DRIVERS=y \
	CONFIG_ATM_BR2684=y \
	CONFIG_ATM_MPOA=y \
	CONFIG_PPPOATM=y \
	CONFIG_ATM_LANE=y 
endef

define KernelPackage/atm_stack/description
 Kernel built-in support for ATM stack
endef

$(eval $(call KernelPackage,atm_stack))

define KernelPackage/lantiq_atm_builtin
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=LTQ ATM Support (built-in)
  DEPENDS:=@!PACKAGE_kmod-lantiq_atm_mod @TARGET_lantiq +kmod-atm_stack
  KCONFIG:= \
	CONFIG_LTQ_ATM=y \
	CONFIG_LTQ_ATM_TASKLET=y
endef

define KernelPackage/lantiq_atm_builtin/description
 Kernel support for LTQ ATM - built-in
endef

$(eval $(call KernelPackage,lantiq_atm_builtin))

define KernelPackage/lantiq_atm_retx
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=LTQ ATM ReTransmission Support
  DEPENDS:=@PACKAGE_kmod-lantiq_atm_builtin||PACKAGE_kmod-lantiq_atm_mod
  KCONFIG:= \
	CONFIG_LTQ_ATM_RETX=y
endef

define KernelPackage/lantiq_atm_retx/description
 Kernel support for LTQ ATM ReTransmission
endef

$(eval $(call KernelPackage,lantiq_atm_retx))

define KernelPackage/lantiq_eth_drv_builtin
 SUBMENU:=$(LANTIQ_MENU)
 TITLE:= LTQ Ethernet Driver (Built-in Support)
 DEPENDS:=@TARGET_lantiq
 KCONFIG:= \
	CONFIG_LANTIQ_ETH_DRV=y
endef

define KernelPackage/lantiq_eth_drv_builtin/description
 Kernel support for LTQ Ethernet Driver (Built-in Support)
endef

$(eval $(call KernelPackage,lantiq_eth_drv_builtin))

CPE_NAME:=$(shell echo $(SUBTARGET) | awk '{ print toupper($$1) }')

define KernelPackage/lantiq_ppa_xrx200
 SUBMENU:=Lantiq
 TITLE:=LTQ PPA Support for xRX200
 DEPENDS:=@TARGET_lantiq_xrx200
 KCONFIG:= \
	CONFIG_LTQ_PPA_XRX200=y
endef

$(eval $(call KernelPackage,lantiq_ppa_xrx200))

define KernelPackage/lantiq_ppa_xrx300
 SUBMENU:=Lantiq
 TITLE:=LTQ PPA Support for xRX300
 DEPENDS:=@TARGET_lantiq_xrx300
 KCONFIG:= \
	CONFIG_LTQ_PPA_XRX300=y
endef

$(eval $(call KernelPackage,lantiq_ppa_xrx300))

define KernelPackage/lantiq_ppa_xrx330
 SUBMENU:=Lantiq
 TITLE:=LTQ PPA Support for xRX330
 DEPENDS:=@TARGET_lantiq_xrx330
 KCONFIG:= \
	CONFIG_LTQ_PPA_XRX330=y
endef

$(eval $(call KernelPackage,lantiq_ppa_xrx330))

define KernelPackage/lantiq_ppa
 SUBMENU:=Lantiq
 TITLE:=LTQ PPA Support
 DEPENDS:=@TARGET_lantiq
 KCONFIG:= \
	CONFIG_LTQ_PPA=y \
	CONFIG_LTQ_PPA_API=y \
	CONFIG_LTQ_PPA_API_DIRECTPATH=y \
	CONFIG_LTQ_PPA_API_DIRECTPATH_BRIDGING=y \
	CONFIG_LTQ_PPA_API_PROC=y \
	CONFIG_LTQ_PPA_DATAPATH=y
endef

define KernelPackage/lantiq_ppa/description
 Kernel support for PPA acceleration
endef

define KernelPackage/lantiq_ppa_mod
 SUBMENU:=Lantiq
 TITLE:=LTQ PPA Support
 DEPENDS:=@TARGET_lantiq
 KCONFIG:= \
	CONFIG_LTQ_PPA=y \
	CONFIG_LTQ_PPA_API \
	CONFIG_LTQ_PPA_API_DIRECTPATH=y \
	CONFIG_LTQ_PPA_API_DIRECTPATH_BRIDGING=y \
	CONFIG_LTQ_PPA_API_PROC \
	CONFIG_LTQ_PPA_DATAPATH

 $(if $(wildcard $(LINUX_DIR)/drivers/net/lantiq_ppa),PPAPATH:="lantiq_ppa",PPAPATH:="lantiq_ppa/src")
 FILES:= \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/ppa_api/ppa_api.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/ppa_api/ppa_api_proc.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ppa_mod/description
 Kernel support for PPA acceleration
endef

define KernelPackage/lantiq_ppa_a5_builtin
  $(call KernelPackage/lantiq_ppa)
  TITLE+=(A5 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-lantiq_ppa_a5_mod @!PACKAGE_kmod-lantiq_atm_builtin @TARGET_lantiq_ar9||TARGET_lantiq_xrx200||TARGET_lantiq_xrx300 @!TARGET_lantiq_xrx330 @!TARGET_lantiq_xrx300_EASY388_VRX318 @!TARGET_lantiq_xrx300_EASY388_BOND +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_A5=y
endef

define KernelPackage/lantiq_ppa_a5_builtin/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA A5 Mode - built-in
endef

$(eval $(call KernelPackage,lantiq_ppa_a5_builtin))


define KernelPackage/lantiq_ppa_a5_mod
  $(call KernelPackage/lantiq_ppa_mod)
  TITLE+=(A5 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @!PACKAGE_kmod-lantiq_atm_builtin @TARGET_lantiq_ar9||TARGET_lantiq_xrx200||TARGET_lantiq_xrx300 @!TARGET_lantiq_xrx330 @!TARGET_lantiq_xrx300_EASY388_VRX318 +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_A5=y

  $(if $(wildcard $(LINUX_DIR)/drivers/net/lantiq_ppa),PPAPATH:="lantiq_ppa",PPAPATH:="lantiq_ppa/src")
  FILES += \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/$(SUBTARGET_SUFFIX)/a5/ppa_datapath_$(SUBTARGET_SUFFIX)_a5.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/$(SUBTARGET_SUFFIX)/a5/ppa_hal_$(SUBTARGET_SUFFIX)_a5.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ppa_a5_mod/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA A5 Mode - pluggable module
endef

$(eval $(call KernelPackage,lantiq_ppa_a5_mod))


define KernelPackage/lantiq_ppa_d5_builtin
  $(call KernelPackage/lantiq_ppa)
  TITLE+=(D5 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-lantiq_ppa_d5_mod @!PACKAGE_kmod-lantiq_eth_drv_builtin @TARGET_lantiq_ar9||TARGET_lantiq_xrx200 @!TARGET_lantiq_xrx330 @!TARGET_lantiq_xrx300_EASY388_VRX318 @!TARGET_lantiq_xrx300_EASY388_BOND
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_D5=y
endef

define KernelPackage/lantiq_ppa_d5_builtin/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA D5 Mode - built-in
endef

$(eval $(call KernelPackage,lantiq_ppa_d5_builtin))

define KernelPackage/lantiq_ppa_d5_mod
  $(call KernelPackage/lantiq_ppa_mod)
  TITLE+=(D5 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @!PACKAGE_kmod-lantiq_eth_drv_builtin @TARGET_lantiq_ar9||TARGET_lantiq_xrx200||TARGET_lantiq_xrx300 @!TARGET_lantiq_xrx300_EASY388_VRX318
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_D5=y

  $(if $(wildcard $(LINUX_DIR)/drivers/net/lantiq_ppa),PPAPATH:="lantiq_ppa",PPAPATH:="lantiq_ppa/src")
  FILES += \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/$(SUBTARGET_SUFFIX)/d5/ppa_datapath_$(SUBTARGET_SUFFIX)_d5.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/$(SUBTARGET_SUFFIX)/d5/ppa_hal_$(SUBTARGET_SUFFIX)_d5.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ppa_d5_mod/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA D5 Mode - pluggable module
endef

$(eval $(call KernelPackage,lantiq_ppa_d5_mod))


define KernelPackage/lantiq_ppa_e5_builtin
  $(call KernelPackage/lantiq_ppa)
  TITLE+=(E5 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-lantiq_ppa_e5_mod @!PACKAGE_kmod-lantiq_eth_drv_builtin @TARGET_lantiq_ar9||@TARGET_lantiq_xrx200 @!TARGET_lantiq_xrx330 @!TARGET_lantiq_xrx300_EASY388_VRX318 @!TARGET_lantiq_xrx300_EASY388_BOND +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_E5=y
endef

define KernelPackage/lantiq_ppa_e5_builtin/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA E5 Mode - built-in
endef

$(eval $(call KernelPackage,lantiq_ppa_e5_builtin))


define KernelPackage/lantiq_ppa_e5_mod
  $(call KernelPackage/lantiq_ppa_mod)
  TITLE+=(E5 mode - pluggable module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @!PACKAGE_kmod-lantiq_eth_drv_builtin @TARGET_lantiq_ar9||@TARGET_lantiq_xrx200||@TARGET_lantiq_xrx300 @!TARGET_lantiq_xrx300_EASY388_VRX318 +kmod-atm_stack
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_E5=y

  $(if $(wildcard $(LINUX_DIR)/drivers/net/lantiq_ppa),PPAPATH:="lantiq_ppa",PPAPATH:="lantiq_ppa/src")
  FILES += \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/$(SUBTARGET_SUFFIX)/e5/ppa_datapath_$(SUBTARGET_SUFFIX)_e5.$(LINUX_KMOD_SUFFIX) \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/$(SUBTARGET_SUFFIX)/e5/ppa_hal_$(SUBTARGET_SUFFIX)_e5.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ppa_e5_mod/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA E5 Mode - pluggable module
endef

$(eval $(call KernelPackage,lantiq_ppa_e5_mod))

define KernelPackage/lantiq_ppa_vrx318_d5_builtin
  $(call KernelPackage/lantiq_ppa)
  TITLE+=(D5 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-lantiq_ppa_vrx318_d5_mod @!PACKAGE_kmod-lantiq_eth_drv_builtin @TARGET_lantiq_xrx330||TARGET_lantiq_xrx300_EASY388_VRX318||TARGET_lantiq_xrx300_EASY388_BOND
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_D5=y
endef

define KernelPackage/lantiq_ppa_vrx318_d5_builtin/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA D5 Mode - built-in
endef

$(eval $(call KernelPackage,lantiq_ppa_vrx318_d5_builtin))

define KernelPackage/lantiq_ppa_vrx318_d5_mod
  $(call KernelPackage/lantiq_ppa_mod)
  TITLE+=(D5 mode - module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @!PACKAGE_kmod-lantiq_eth_drv_builtin @!PACKAGE_kmod-lantiq_ppa_d5_mod @TARGET_lantiq_xrx330||TARGET_lantiq_xrx300_EASY388_VRX318||@TARGET_lantiq_xrx300_EASY388_BOND
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_D5=y \
	CONFIG_IFX_DMA_DESCRIPTOR_NUMBER=64

  $(if $(wildcard $(LINUX_DIR)/drivers/net/lantiq_ppa),PPAPATH:="lantiq_ppa",PPAPATH:="lantiq_ppa/src")
  FILES += \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/xrx330/soc/ltqmips_ppe_drv.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ppa_vrx318_d5_mod/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA D5 Mode - pluggable module
endef

$(eval $(call KernelPackage,lantiq_ppa_vrx318_d5_mod))

define KernelPackage/lantiq_ppa_vrx318_e1_builtin
  $(call KernelPackage/lantiq_ppa)
  TITLE+=(E1 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-lantiq_ppa_vrx318_e1_mod @!PACKAGE_kmod-lantiq_eth_drv_builtin @TARGET_lantiq_xrx330||TARGET_lantiq_xrx300_EASY388_VRX318||TARGET_lantiq_xrx300_EASY388_BOND +kmod-atm_stack +kmod-lantiq_ppa_vrx318_d5_builtin
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_E5=n \
	CONFIG_LTQ_PPA_E1=y
endef

define KernelPackage/lantiq_ppa_vrx318_e1_builtin/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA E1 Mode - built-in
endef

$(eval $(call KernelPackage,lantiq_ppa_vrx318_e1_builtin))

define KernelPackage/lantiq_ppa_vrx318_e1_mod
  $(call KernelPackage/lantiq_ppa_vrx318_d5_mod)
  TITLE+=(E1 mode)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @!PACKAGE_kmod-lantiq_ppa_e5_mod @!PACKAGE_kmod-lantiq_ppa_e5_builtin @TARGET_lantiq_xrx330||@TARGET_lantiq_xrx300_EASY388_VRX318||@TARGET_lantiq_xrx300_EASY388_BOND +kmod-atm_stack +kmod-lantiq_ppa_vrx318_d5_mod
  KCONFIG += \
	CONFIG_LTQ_PPA_E5=n \
	CONFIG_LTQ_PPA_E1=y

  $(if $(wildcard $(LINUX_DIR)/drivers/net/lantiq_ppa),PPAPATH:="lantiq_ppa",PPAPATH:="lantiq_ppa/src")
  FILES += \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/xrx330/smartphy/ltqmips_vrx318_e1.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ppa_vrx318_e1_mod/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA E1 Mode - pluggable module
endef

$(eval $(call KernelPackage,lantiq_ppa_vrx318_e1_mod))

define KernelPackage/lantiq_ppa_vrx318_a1_builtin
  $(call KernelPackage/lantiq_ppa)
  TITLE+=(A1 mode - built-in)
  DEPENDS+=@!PACKAGE_kmod-lantiq_ppa_vrx318_a1_mod @!PACKAGE_kmod-lantiq_atm_builtin @TARGET_lantiq_xrx330||TARGET_lantiq_xrx300_EASY388_VRX318||TARGET_lantiq_xrx300_EASY388_BOND +kmod-atm_stack +kmod-lantiq_ppa_vrx318_d5_builtin
  KCONFIG += \
	CONFIG_IFX_EXTRA_CFG=y \
	CONFIG_LTQ_PPA_A5=n \
	CONFIG_LTQ_PPA_A1=y
endef

define KernelPackage/lantiq_ppa_vrx318_a1_builtin/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA A1 Mode - built-in
endef

$(eval $(call KernelPackage,lantiq_ppa_vrx318_a1_builtin))

define KernelPackage/lantiq_ppa_vrx318_a1_mod
  $(call KernelPackage/lantiq_ppa_vrx318_d5_mod)
  TITLE+=(A1 mode - module)
  DEPENDS+=@!PACKAGE_kmod-fs-nfs-root @!PACKAGE_kmod-lantiq_atm_builtin @!PACKAGE_kmod-lantiq_ppa_a5_mod @!PACKAGE_kmod-lantiq_ppa_a5_builtin @TARGET_lantiq_xrx330||TARGET_lantiq_xrx300_EASY388_VRX318||@TARGET_lantiq_xrx300_EASY388_BOND +kmod-atm_stack +kmod-lantiq_ppa_vrx318_d5_mod
  KCONFIG += \
	CONFIG_LTQ_PPA_A1=y \
	CONFIG_LTQ_PPA_A5=n

  $(if $(wildcard $(LINUX_DIR)/drivers/net/lantiq_ppa),PPAPATH:="lantiq_ppa",PPAPATH:="lantiq_ppa/src")
  FILES += \
	$(LINUX_DIR)/drivers/net/$(PPAPATH)/platform/xrx330/smartphy/ltqmips_vrx318_a1.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ppa_vrx318_a1_mod/description
  $(call KernelPackage/lantiq_ppa/description)
  PPA A1 Mode - pluggable module
endef

$(eval $(call KernelPackage,lantiq_ppa_vrx318_a1_mod))

define KernelPackage/lantiq_mini_jumbo_frame
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=Mini Jumbo Frames Support
  KCONFIG:= \
		CONFIG=y \
		CONFIG_LTQ_MINI_JUMBO_FRAME_SUPPORT=y
endef

define KernelPackage/lantiq_mini_jumbo_frame/description
	Support for mini jumbo frames (also called as baby jumbo frames).
endef

$(eval $(call KernelPackage,lantiq_mini_jumbo_frame))

define KernelPackage/lantiq_imq
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=Support for IMQ 
  KCONFIG:= \
	CONFIG_IMQ=y \
	CONFIG_IMQ_BEHAVIOR_AA=y \
	CONFIG_IMQ_NUM_DEVS=3 \
	CONFIG_NETFILTER_XT_TARGET_IMQ=y
  FILES:=$(LINUX_DIR)/drivers/net/imq.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_imq/description
  Kernel support for IMQ
endef

$(eval $(call KernelPackage,lantiq_imq))

define KernelPackage/lantiq_qos
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=Lantiq support for QoS
  #DEPENDS:= +ebtables +ebtables-utils 
  KCONFIG:= \
	CONFIG_LANTIQ_IPQOS=y \
	CONFIG_LANTIQ_ALG_QOS=y \
	CONFIG_LANTIQ_IPQOS_MARK_SKBPRIO=y \
	CONFIG_LANTIQ_IPQOS_CLASS_ACCELERATION_DISABLE=y \
	CONFIG_NET_SCHED=y \
	CONFIG_NET_SCH_FIFO=y \
	CONFIG_NET_SCH_CLK_GETTIMEOFDAY=y \
	CONFIG_NET_SCH_CBQ=y \
	CONFIG_NET_SCH_HTB=y \
	CONFIG_NET_SCH_RED=y \
	CONFIG_NET_SCH_DSMARK=y \
	CONFIG_NET_CLS=y \
	CONFIG_NET_CLS_FW=y \
	CONFIG_NET_CLS_U32=y \
	CONFIG_NETFILTER_XT_MATCH_U32=y \
	CONFIG_LTQ_ALG_QOS=y \
	CONFIG_IP_NF_TARGET_TOS=y \
	CONFIG_NETFILTER_XT_TARGET_DSCP=y \
	CONFIG_BRIDGE_NETFILTER=n \
	CONFIG_NET_SCH_ESFQ=n \
	CONFIG_NET_SCH_TEQL=n \
	CONFIG_NET_SCH_HFSC=n \
	CONFIG_VLAN_8021Q=y \
	CONFIG_VLAN_8021Q_UNTAG=y \
	CONFIG_LTQ_IPQOS_BRIDGE_EBT_IMQ=y
endef

define KernelPackage/lantiq_qos/description
  Kernel Support for QoS. This package enables classifier and queuing disciplines (HTB, CBQ, FIFO etc) in Kernel configuration.
endef

$(eval $(call KernelPackage,lantiq_qos))

define KernelPackage/lantiq_layer7
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=LTQ layer7 support
  KCONFIG:= CONFIG_NETFILTER_XT_MATCH_LAYER7=m
  FILES:=$(LINUX_DIR)/net/netfilter/xt_layer7.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_layer7/description
  Kernel Support for layer7. This package enables layer7 Kernel configuration.
endef
$(eval $(call KernelPackage,lantiq_layer7))

define KernelPackage/lantiq_session_limiter
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=LTQ Session Limiter
  KCONFIG:= CONFIG_LTQ_HANDLE_CONNTRACK_SESSIONS=y
endef

define KernelPackage/lantiq_session_limiter/description
  Kernel Support for session limiting.. This package enables session limiter Kernel configuration.
endef
$(eval $(call KernelPackage,lantiq_session_limiter))

define KernelPackage/lantiq_extmark
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=LTQ Extmark
  KCONFIG:= CONFIG_NF_CONNTRACK_EXTMARK=y \
	    CONFIG_NETFILTER_XT_TARGET_CONNEXTMARK=m
  FILES:=$(LINUX_DIR)/net/netfilter/xt_CONNEXTMARK.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_extmark/description
  Kernel Support for extended mark
endef
$(eval $(call KernelPackage,lantiq_extmark))

define KernelPackage/ipv6-kernel
  SUBMENU:=Netfilter Extensions
  TITLE:=Extra IPv6 kernel options
  KCONFIG:= \
	CONFIG_IPV6=y \
	CONFIG_IPV6_ROUTER_PREF=y \
	CONFIG_IPV6_ROUTE_INFO=y \
	CONFIG_NF_CONNTRACK_IPV6=y \
	CONFIG_NET_IP_TUNNEL=m \
	CONFIG_IP6_NF_QUEUE=n
  FILES:=$(LINUX_DIR)/net/ipv4/ip_tunnel.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/ipv6-kernel/description
  Enable extra IPv6 kernel options
endef

$(eval $(call KernelPackage,ipv6-kernel))

define KernelPackage/ipv6-firewall
  SUBMENU:=Netfilter Extensions
  TITLE:=IPv6 firewall options
  DEPENDS:= +kmod-ipv6-kernel
  KCONFIG:= \
	CONFIG_IP6_NF_IPTABLES=m \
	CONFIG_IP6_NF_MATCH_AH=n \
	CONFIG_IP6_NF_MATCH_EUI64=m \
	CONFIG_IP6_NF_MATCH_FRAG=m \
	CONFIG_IP6_NF_MATCH_OPTS=m \
	CONFIG_IP6_NF_MATCH_HL=n \
	CONFIG_IP6_NF_MATCH_IPV6HEADER=m \
	CONFIG_IP6_NF_MATCH_MH=m \
	CONFIG_IP6_NF_MATCH_RT=m \
	CONFIG_IP6_NF_TARGET_HL=n \
	CONFIG_IP6_NF_TARGET_LOG=m \
	CONFIG_IP6_NF_FILTER=m \
	CONFIG_IP6_NF_TARGET_REJECT=m \
	CONFIG_IP6_NF_MANGLE=m \
	CONFIG_IP6_NF_RAW=n \
	CONFIG_IP6_NF_SECURITY=n
endef

define KernelPackage/ipv6-firewall/description
  Enable  IPv6 firewall specific options
endef

$(eval $(call KernelPackage,ipv6-firewall))


define KernelPackage/lan-port-sep
  SUBMENU:=Lantiq
  TITLE:=LAN Port Seperation
  KCONFIG:= \
	CONFIG_LTQ_ETHSW=m \
	CONFIG_LTQ_PPA_PORT_SEPARATION=y
  FILES:=$(LINUX_DIR)/drivers/net/lantiq_ethsw.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lan-port-sep/description
  Enable support for LAN port seperation driver. The driver creates virtual interface for each of switch LAN ports.
endef

$(eval $(call KernelPackage,lan-port-sep))

define KernelPackage/ltq_portbinding
 SUBMENU:=Lantiq
 DEPENDS:=@PACKAGE_kmod-lan-port-sep
 TITLE:=Lantiq PortBinding 
 KCONFIG:= \
       CONFIG_IP_ADVANCED_ROUTER=y \
       CONFIG_IP_MULTIPLE_TABLES=y \
       CONFIG_BRIDGE_NETFILTER=y \
       CONFIG_NETFILTER_XT_MATCH_PHYSDEV=y
endef

define KernelPackage/ltq_portbinding//description
 kernel support for PortBinding 
endef

$(eval $(call KernelPackage,ltq_portbinding))

define KernelPackage/lantiq_ebtables
  SUBMENU:=$(LANTIQ_MENU)
  DEPENDS:=@!PACKAGE_kmod-ebtables
  TITLE:=Bridge firewalling modules
  KCONFIG:= \
          CONFIG_BRIDGE_NF_EBTABLES=y \
          CONFIG_BRIDGE_EBT_BROUTE=y \
          CONFIG_BRIDGE_EBT_T_FILTER=y \
          CONFIG_BRIDGE_EBT_T_NAT=y \
          CONFIG_BRIDGE_EBT_802_3=n \
          CONFIG_BRIDGE_EBT_AMONG=n \
          CONFIG_BRIDGE_EBT_LIMIT=n \
          CONFIG_BRIDGE_EBT_MARK=y \
          CONFIG_BRIDGE_EBT_EXTMARK=y \
          CONFIG_BRIDGE_EBT_PKTTYPE=y \
          CONFIG_BRIDGE_EBT_STP=n \
          CONFIG_BRIDGE_EBT_VLAN=y \
          CONFIG_BRIDGE_EBT_MARK_T=y \
          CONFIG_BRIDGE_EBT_EXTMARK_T=y \
          CONFIG_BRIDGE_EBT_REDIRECT=y
endef

define KernelPackage/lantiq_ebtables/description
  ebtables is a general, extensible frame/packet identification
  framework. It provides you to do Ethernet
  filtering/NAT/brouting on the Ethernet bridge.
endef

$(eval $(call KernelPackage,lantiq_ebtables))


define KernelPackage/lantiq_ebtables-ipv4
  SUBMENU:=$(LANTIQ_MENU)
  DEPENDS:=kmod-lantiq_ebtables
  TITLE:=ebtables: IPv4 support
  KCONFIG:= \
          CONFIG_BRIDGE_EBT_ARP=y \
          CONFIG_BRIDGE_EBT_IP=y \
          CONFIG_BRIDGE_EBT_ARPREPLY=y \
          CONFIG_BRIDGE_EBT_DNAT=n
  FILES:= $(LINUX_DIR)/net/bridge/netfilter/ebt_ip.$(LINUX_KMOD_SUFFIX)
endef

define KernelPackage/lantiq_ebtables-ipv4/description
 This option adds the IPv4 support to ebtables, which allows basic
 IPv4 header field filtering, ARP filtering as well as SNAT, DNAT targets.
endef

$(eval $(call KernelPackage,lantiq_ebtables-ipv4))


define KernelPackage/lantiq_ebtables-ipv6
  SUBMENU:=$(LANTIQ_MENU)
  DEPENDS:=kmod-lantiq_ebtables
  TITLE:=ebtables: IPv6 support
  KCONFIG:= \
          CONFIG_BRIDGE_EBT_IP6=n
endef

define KernelPackagelantiq_ebtables-ipv6/description
 This option adds the IPv6 support to ebtables, which allows basic
 IPv6 header field filtering and target support.
endef

$(eval $(call KernelPackage,lantiq_ebtables-ipv6))

define KernelPackage/lantiq_sflash_support
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=Lantiq SPI Flash support
  KCONFIG:= \
	CONFIG_LANTIQ_SPI=y \
	CONFIG_LANTIQ_SPI_DEBUG=y \
	CONFIG_LANTIQ_SPI_FLASH=y \
	CONFIG_SPI_BITBANG=n \
	CONFIG_SPI_GPIO=n

endef

define KernelPackage/lantiq_sflash_support/description
 This option is used to enable Lantiq SPI Flash support.
endef

$(eval $(call KernelPackage,lantiq_sflash_support))

define KernelPackage/lantiq_ipsec
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=IPSec Support
  KCONFIG:= \
	CONFIG_CRYPTO_HW=y \
	CONFIG_CRYPTO_DEV_LANTIQ_DEU=y \
	CONFIG_CRYPTO_DEV_PWR_SAVE_MODE=n \
	CONFIG_XFRM_USER=y \
	CONFIG_XFRM_SUB_POLICY=y \
	CONFIG_XFRM_IPCOMP=y \
	CONFIG_NET_KEY=y \
	CONFIG_INET_AH=y \
	CONFIG_INET_ESP=y \
	CONFIG_INET_IPCOMP=y \
	CONFIG_INET_XFRM_TUNNEL=y \
	CONFIG_INET_TUNNEL=y \
	CONFIG_INET_XFRM_MODE_TRANSPORT=y \
	CONFIG_INET_XFRM_MODE_TUNNEL=y \
	CONFIG_CRYPTO_AEAD=y \
	CONFIG_CRYPTO_AUTHENC=y \
	CONFIG_CRYPTO_HMAC=y \
	CONFIG_CRYPTO_DEFLATE=y \
	CONFIG_NETFILTER_XT_MATCH_POLICY=y \
	CONFIG_NET_IPIP=y \
	CONFIG_CRYPTO_RNG=y \
	CONFIG_CRYPTO_GF128MUL=y \
	CONFIG_CRYPTO_GCM=y \
	CONFIG_CRYPTO_SEQIV=y \
	CONFIG_CRYPTO_CTR=y \
	CONFIG_CRYPTO_ECB=y \
	CONFIG_CRYPTO_XCBC=y \
	CONFIG_CRYPTO_GHASH=y \
	CONFIG_CRYPTO_MD5=y \
	CONFIG_CRYPTO_BLOWFISH=y
endef

define KernelPackage/lantiq_ipsec/description
  IPSec Support
endef

$(eval $(call KernelPackage,lantiq_ipsec))

define KernelPackage/lantiq-cpufreq
  TITLE:=Lantiq CPUFREQ support
  SUBMENU:=Lantiq
  DEPENDS:= \
	+cpufrequtils \
	@+PACKAGE_ltq-wlan-wave&&FEATURE_LTQ_WAVE_AR10_SUPPORT:PMCU_SUPPORT \
	@+PACKAGE_ltq-voice-tapi:LTQ_VOICE_TAPI_PMC
  KCONFIG:= \
	CONFIG_CPU_FREQ=y \
	CONFIG_CPU_FREQ_GOV_POWERSAVE=y \
	CONFIG_CPU_FREQ_GOV_USERSPACE=y \
	CONFIG_CPU_FREQ_GOV_ONDEMAND=y \
	CONFIG_LTQ_CPUFREQ=y \
	CONFIG_LTQ_CPU_FREQ=y
endef

define KernelPackage/lantiq-cpufreq/description
  CPUFREQ driver support for Lantiq xway SoC's
endef

$(eval $(call KernelPackage,lantiq-cpufreq))

define KernelPackage/lantiq-cpufreq-scaling-ena
  TITLE:=Enable Lantiq frequency scaling
  SUBMENU:=Lantiq
  DEPENDS:= kmod-lantiq-cpufreq
endef

define KernelPackage/lantiq-cpufreq-scaling-ena/description
  Frequency Scaling support for Lantiq xway SoC's enabled
endef

$(eval $(call KernelPackage,lantiq-cpufreq-scaling-ena))

define KernelPackage/lantiq_vdsl_vectoring_support
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=PTM Vectoring Support
  KCONFIG += \
	CONFIG_PTM_VECTORING=y
endef

define KernelPackage/lantiq_vdsl_vectoring_support/description
  PTM Vectoring support
endef

$(eval $(call KernelPackage,lantiq_vdsl_vectoring_support))

define KernelPackage/lantiq_directlink_support
  SUBMENU:=$(LANTIQ_MENU)
  TITLE:=Direct Link Support
  DEPENDS:=@TARGET_lantiq_xrx330
  KCONFIG:= \
        CONFIG_ACCL_11AC=y
endef

define KernelPackage/lantiq_directlink_support/description
  Direct Link Support
endef

$(eval $(call KernelPackage,lantiq_directlink_support))

define KernelPackage/jffs2
  SUBMENU:=Filesystems
  TITLE:=JFFS2 filesystem Support
  KCONFIG:= \
	CONFIG_JFFS2_FS=y
endef

define KernelPackage/jffs2/description
  JFFS2 filesystem Support
endef

$(eval $(call KernelPackage,jffs2))

FS_MENU:=Filesystems

define KernelPackage/fs-nfs-root
  SUBMENU:=$(FS_MENU)
  TITLE:=NFS root support
  KCONFIG:= \
	CONFIG_NFS_FS=y \
	CONFIG_NFS_V2=y \
	CONFIG_IP_PNP=y \
	CONFIG_IP_PNP_DHCP=y \
	CONFIG_IP_PNP_BOOTP=y \
	CONFIG_IP_PNP_RARP=y \
	CONFIG_LOCKD=y \
	CONFIG_ROOT_NFS=y \
	CONFIG_RPCSEC_GSS_KRB5=y \
	CONFIG_SUNRPC=y \
	CONFIG_SUNRPC_GSS=y

endef

define KernelPackage/fs-nfs-root/description
 Enables kernel settings for NFS root support
endef

$(eval $(call KernelPackage,fs-nfs-root))

