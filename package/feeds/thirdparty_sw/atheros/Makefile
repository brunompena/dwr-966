#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 13904 2009-01-06 21:56:22Z jow $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=atheros
PKG_VERSION:=1.1
###
#
# source files pathc
#
###
FILE_BASE=wnc_files
FILE_COMMON=common
# File for each branch
# TCLGENERIC
ifdef CONFIG_TARGET_lantiq_xrx330_P01001
FILE_BRANCH=TCLGENERIC
endif
# Telenor
ifdef CONFIG_TARGET_lantiq_xrx330_P01002
FILE_BRANCH=TCLGENERIC
#FILE_BRANCH=TELENOR
endif
# UNE
ifdef CONFIG_TARGET_lantiq_xrx330_P01003
#FILE_BRANCH=TCLGENERIC
FILE_BRANCH=UNE
endif
# Dora
ifdef CONFIG_TARGET_lantiq_xrx330_P02001
FILE_BRANCH=DORA
endif
# Dora GCT
ifdef CONFIG_TARGET_lantiq_xrx330_P02002
FILE_BRANCH=DORA
endif
# Dora style UI
ifdef CONFIG_TARGET_lantiq_xrx330_P02003
FILE_BRANCH=DORA
endif
# For WLD9 in WLD7 PCB
ifdef CONFIG_TARGET_lantiq_xrx300
FILE_BRANCH=DORA
endif

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Lantiq
	TITLE:= Atheros scripts/binaries
	DEPENDS:= +kmod-ltqcpe_wlan_atheros \
		@+FEATURE_WIRELESS \
		@+FEATURE_WIRELESS_3P_ATH \
		@+FEATURE_IFX_WLAN_WMM \
		@+FEATURE_IFX_WLAN_WPS \
		@+FEATURE_DEVM_WIFILAN\
		@+CONFIG_FEATURE_DEVM_WIFILAN_WPS \
		@+CONFIG_FEATURE_DEVM_WIFILAN_WMM \
		@+BUSYBOX_CONFIG_WC \
		@+BUSYBOX_CONFIG_WHICH \
		@+BUSYBOX_CONFIG_AWK \
		@+BUSYBOX_CONFIG_FEATURE_AWK_MATH \
		@+BUSYBOX_CONFIG_FIND \
		@+BUSYBOX_CONFIG_FEATURE_FIND_MTIME \
		@+BUSYBOX_CONFIG_FEATURE_FIND_PERM \
		@+BUSYBOX_CONFIG_FEATURE_FIND_TYPE \
		@+BUSYBOX_CONFIG_FEATURE_FIND_XDEV \
		@+BUSYBOX_CONFIG_FEATURE_FIND_NEWER \
		@+BUSYBOX_CONFIG_FEATURE_FIND_INUM \
		@+BUSYBOX_CONFIG_XARGS \
		@+BUSYBOX_CONFIG_LSMOD \
		@+PACKAGE_libnl \
		@+PACKAGE_libstdcpp \
		@+PACKAGE_libopenssl \
		@+PACKAGE_libuuid \
		@+PACKAGE_uuidgen
	URL:=http://www.infineon.com
	MAINTAINER:=Infineon Technologies AG
	MENU:=1
endef

define Package/$(PKG_NAME)/description
 This package contains atheros scripts/binaries
endef

define Package/$(PKG_NAME)/config
  config PACKAGE_$(PKG_NAME)
	select PACKAGE_kmod-ltqcpe_dma_descr_atheros

  config FEATURE_ATHEROS_WLAN_TYPE_USB
	depends on PACKAGE_$(PKG_NAME)
	bool 'Atheros wlan type USB'
	help
	  'This feature loads Atheros wlan usb driver instead of pci(default) driver'
endef

define KernelPackage/ltqcpe_wlan_atheros
 SUBMENU:=Lantiq
 TITLE:=IFX Supoort for Atheros WLAN 
 KCONFIG:= \
         CONFIG_IFX_PCI=y \
         CONFIG_IFX_PCI_HW_SWAP=y \
         CONFIG_FW_LOADER=y \
         CONFIG_HOTPLUG=y \
         CONFIG_KMALLOC_HIGH_ATHEROS=y
endef

define KernelPackage/ltqcpe_wlan_atheros/description
  Kernel Support for Atheros WLAN 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default
endef

define Build/Compile
	$(call Build/Compile/Default)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/rc.d
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)/net
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)/net
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) -p $(1)/usr/local/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(if $(CONFIG_FEATURE_ATHEROS_WLAN_TYPE_USB), \
		$(CP) ./files/ath_modules/k2/*.ko $(1)/lib/modules/$(LINUX_VERSION)/net/; \
		$(CP) ./files/ath_k2_ramdisk_copy/* $(1)/; \
        , \
		$(if $(CONFIG_TARGET_lantiq_xrx330), \
			$(if $(CONFIG_PACKAGE_kmod-smvp), \
				$(if $(CONFIG_PACKAGE_kmod-lantiq_directlink_support), \
					$(CP) ./files/ath_modules/grx389/smp/directlink/*.ko $(1)/lib/modules/$(LINUX_VERSION)/net/; \
				, \
					$(CP) ./files/ath_modules/grx389/smp/*.ko $(1)/lib/modules/$(LINUX_VERSION)/net/; \
				) \
			, \
				$(if $(CONFIG_PACKAGE_kmod-lantiq_directlink_support), \
					$(CP) ./files/ath_modules/grx389/directlink/*.ko $(1)/lib/modules/$(LINUX_VERSION)/net/; \
				, \
					$(CP) ./files/ath_modules/grx389/*.ko $(1)/lib/modules/$(LINUX_VERSION)/net/; \
				) \
			) \
		, \
			$(if $(CONFIG_TARGET_lantiq_xrx300), \
				$(CP) ./files/ath_modules/ar10/*.ko $(1)/lib/modules/$(LINUX_VERSION)/net/; \
			) \
			$(if $(CONFIG_TARGET_lantiq_xrx200), \
				$(CP) ./files/ath_modules/vrx/*.ko $(1)/lib/modules/$(LINUX_VERSION)/net/; \
			) \
		) \
	) 
	$(CP) ./files/ath_modules/*.so* $(1)/lib/
	$(CP) ./files/ath_binaries/* $(1)/sbin/
	$(CP) ./files/ath_scripts/* $(1)/etc/rc.d/
	$(CP) ./files/ath_ramdisk_copy/* $(1)/
	$(CP) ./$(FILE_BASE)/$(FILE_COMMON)/* $(1)/usr/local/bin
	$(CP) ./$(FILE_BASE)/$(FILE_BRANCH)/* $(1)/usr/local/bin	
	$(CP) ./$(FILE_BASE)/boot/acinit.sh $(1)/etc/init.d/acinit.sh
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call KernelPackage,ltqcpe_wlan_atheros))
