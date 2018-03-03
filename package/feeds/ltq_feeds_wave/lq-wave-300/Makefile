#
# Openwrt Makefile for ltq-wlan-wave/wave-300
####################################

# Includes
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

# UGW Version
# TODO: need to encapsulate libnl added to DEPENDS (+libnl +...) with new value UGW_VER=new_6.1?
# UGW_VER:=$(shell cat $(TOPDIR)/ugw_version)
UGW_VER:=6.1
#ifneq (,$(filter $(UGW_VER),6.1 5.4))
ifeq ($(UGW_VER),6.1)
 KERNEL_PKG_NAME:=lantiq-wlan-wave-support
 PKG_NAME:=ltq-wlan-wave
 LINUX_ENV_NAME:=ugw-linux310
 PKG_BASE_NAME:=ltq-wave300
 PKG_BUILD_DEPENDS_EXTRA:=kmod-lantiq-cpufreq
 DEPENDS_EXTRA:=+libnl +libpthread +librt
else
 KERNEL_PKG_NAME:=ltqcpe-wlan-wave-support
 PKG_NAME:=ltq-wlan-wave
 LINUX_ENV_NAME:=ugw-linux26
 PKG_BASE_NAME:=lq_wave300
 DEPENDS_EXTRA:=@+FEATURE_IFX_WLAN_WMM \
		@+FEATURE_IFX_WLAN_WPS \
		@+PACKAGE_ifx-devm:FEATURE_IFX_TR69_WIFILAN_WPS \
		@+PACKAGE_ifx-devm:FEATURE_IFX_TR69_WIFILAN_WMM
endif

# Package Info
PKG_VERSION:=03.04.02.40_05_UGW_6.1
DRV_VERSION:=03.04.02.00.25.c9c85872fd07
FW_VERSION:=FW_3.4.2_based_on_3.4.2.40_for_UGW_r3625
PROGMODEL_VERSION_WAVE300:=1668
PROGMODEL_VERSION_WAVE400:=1449
PROGMODEL_VERSION_AR10:=1796
FPGA_SIM_VERSION:=7.4
WAVE_RDLIM_VER:=11001.3173
PKG_RELEASE:=1
PKG_SOURCE_VERSION:=$(DRV_VERSION).gpl.wls.src
PKG_SOURCE_NAME:=$(DRV_VERSION).wls.src
PKG_SOURCE:=lq-wave-300-$(PKG_SOURCE_VERSION).tar.bz2

DRV_RFLIB_PKG_NAME:=$(PKG_NAME)-rflib
DRV_RFLIB_PKG_VERSION:=03.04.02.40_05_UGW_6.1
DRV_RFLIB_PKG_SOURCE:=lq-wave-300-$(DRV_VERSION).rflib.wls.src.tar.bz2
DRV_RFLIB_PKG_SOURCE_URL:=http://localhost
DRV_RFLIB_PKG_BIN:=lq-wave-300-$(DRV_VERSION).rflib.wls.bin.tar.bz2

BINARY_PKG:=$(if $(wildcard $(TOPDIR)/dl/$(DRV_RFLIB_PKG_BIN)),y)
UGW_MODEL_NAME:=$(shell echo $(CONFIG_BUILD_SUFFIX)|tr a-z A-Z)

ifndef CONFIG_$(PKG_NAME)_USE_CUSTOM_SOURCE_DIR
  PKG_SOURCE_DIR:=lq-wave-300-$(PKG_SOURCE_NAME)
  PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
  PKG_FULL_DIR:=$(PKG_BUILD_DIR)/$(PKG_SOURCE_DIR)
else
  PKG_FULL_DIR:=$(CONFIG_lq-wave-300_CUSTOM_SOURCE_DIR)
endif

PKG_BUILD_DEPENDS:=libnl $(DRV_RFLIB_PKG_NAME) $(PKG_BUILD_DEPENDS_EXTRA)
PKG_SOURCE_URL:=http://localhost
PKG_DEFAULT_CUSTOM_SOURCE_DIR:=
PKG_NO_AUTORECONF:=y

WAVE_SCRIPTS_VER:=$(shell hg summ > /dev/null 2>&1 && hg summ | head -n1 | awk '{print $$2}')

ifeq ($(CONFIG_FEATURE_LTQ_WAVE_300),y)
 COPY_WAVE300:=y
 WAVE_CFG_SFX:=
endif
ifeq ($(CONFIG_FEATURE_LTQ_WAVE_400),y)
 COPY_WAVE400:=y
 WAVE_CFG_SFX:=-vbg400
endif
ifeq ($(CONFIG_FEATURE_LTQ_WAVE_AR10_SUPPORT),y)
 COPY_AR10:=y
 ifeq ($(CONFIG_PACKAGE_kmod-ltqcpe_pcie_ar10),y)
  WAVE_CFG_SFX:=-wave400
 else
  WAVE_CFG_SFX:=-wave400-no_pci
 endif
 ifeq ($(UGW_VER),6.1)
  COPY_AR10:=y
  WAVE_CFG_SFX:=-wave400-61_no_pci
 endif
endif
ifeq ($(CONFIG_FEATURE_LTQ_WAVE_STA),y)
 COPY_STA:=y
endif

PKG_CONFIG_DEPENDS:=CONFIG_PACKAGE_kmod-lantiq-cpufreq
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
# Dora Style UI
ifdef CONFIG_TARGET_lantiq_xrx330_P02003
FILE_BRANCH=DORA
endif
# For WLD9 in WLD7 PCB
ifdef CONFIG_TARGET_lantiq_xrx300
FILE_BRANCH=DORA
endif

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/package-version-override.mk


define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Lantiq
	TITLE:=Wave300/wave400 scripts/binaries
	DEPENDS:= +kmod-$(KERNEL_PKG_NAME) \
		+$(DRV_RFLIB_PKG_NAME) \
		@+FEATURE_WIRELESS \
		@+FEATURE_WIRELESS_WAVE300 \
		@+PACKAGE_ltq-devm:FEATURE_DEVM_WIFILAN \
		$(DEPENDS_EXTRA) \
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
		@+PACKAGE_libopenssl \
		@+PACKAGE_libuuid \
		@+PACKAGE_uuidgen \
		@+PACKAGE_hostapd_08x \
		@+PACKAGE_tinytcl
	URL:=http://www.lantiq.com
	MAINTAINER:=Lantiq Technologies
	MENU:=1
endef

define Package/$(PKG_NAME)/description
 This package contains wave300/wave400 scripts/binaries
endef

define Package/$(DRV_RFLIB_PKG_NAME)
$(call Package/$(PKG_NAME)/Default)
 SECTION:=utils
 CATEGORY:=Lantiq
 TITLE:=RFLIB package for Wave300/Wave400 driver
endef

define Package/$(DRV_RFLIB_PKG_NAME)/description
 This package contains the wave300/wave400 RF library
endef

define KernelPackage/$(KERNEL_PKG_NAME)
 SUBMENU:=Lantiq
 TITLE:=Lantiq Support for Wave/Wave300 WLAN 
 ifeq ($(UGW_VER),6.1)
  DEPENDS:=@TARGET_ltqcpe||TARGET_lantiq
 else
  DEPENDS:=@TARGET_ifxcpe_platform_ar9||TARGET_ifxcpe_platform_danube||TARGET_ifxcpe_platform_vr9||TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_vr9||TARGET_ltqcpe_platform_danube||TARGET_ltqcpe_platform_ar10||TARGET_ltqcpe_platform_vbg400||TARGET_ltqcpe_platform_hn1||TARGET_ltqcpe_ar10
 endif
 KCONFIG:= \
 CONFIG_FW_LOADER=y \
 CONFIG_HOTPLUG=y
endef

define KernelPackage/$(KERNEL_PKG_NAME)/description
  Kernel Support for Wave/Wave300 WLAN 
endef


#### Exports ###############################
export THISDIR = $(shell pwd)
export TARGET_CROSS

### Menuconfig #############################
define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
      menu "$(PKG_NAME) features"
      depends on PACKAGE_$(PKG_NAME)
      source "$(SOURCE)/config/Config.in"
      endmenu
endef


#### Target Rules ##########################
# Helper func for downloading rflib tarball:
define Download/files
ifneq ($(4),y)
 define Download/$(1)
  FILE:=$(2)
  URL:=http://localhost/
  MD5SUM:=$(3)
 endef
 define Prepare/$(1)
  tar xjf "$(DL_DIR)/$(2)" -C "$(BUILD_DIR)"
 endef
 $$(eval $$(call Download,$(1)))
else
 define Prepare/$(1)
 endef
endif
endef

# Download rflib src tarball
ifneq ($(CONFIG_PACKAGE_$(DRV_RFLIB_PKG_NAME)),)
 ifneq ($(BINARY_PKG),y)
   $(eval $(call Download/files,$(DRV_RFLIB_PKG_NAME),$(DRV_RFLIB_PKG_SOURCE),$(DRV_RFLIB_PKG_MD5SUM),,))
 else
   $(eval $(call Download/files,$(DRV_RFLIB_PKG_NAME),$(DRV_RFLIB_PKG_BIN),$(DRV_RFLIB_PKG_MD5SUM),$(BUILD_DIR),,))
 endif
endif


ifdef CONFIG_TARGET_ltqcpe_platform_ar10_P02003
CPPFLAGS += -DRSSI_ANALYSIS
endif
define Build/Prepare
	$(call Build/Prepare/Default)
	$(call Prepare/$(DRV_RFLIB_PKG_NAME))
ifndef CONFIG_lq-wave-300_USE_CUSTOM_SOURCE_DIR
	mv $(BUILD_DIR)/$(PKG_SOURCE_DIR) $(PKG_BUILD_DIR)/
endif
	$(if $(BINARY_PKG),\
		$(INSTALL_DIR) $(PKG_FULL_DIR)/wireless/driver/rflib/prebuilt/;
		cp $(BUILD_DIR)/lq-wave*rflib*bin/ugw_models/ARX388_GW_HE_ADSL_SAMPLE/* $(PKG_FULL_DIR)/wireless/driver/rflib/prebuilt/ || true;
	)
	sh -c "cd $(PKG_FULL_DIR); patch -p0 < $(THISDIR)/files/config/MTLKConfig.patch"
endef


define Build/Configure
	rm $(PKG_FULL_DIR)/tools/kconfig/*.o || true
	rm $(PKG_FULL_DIR)/tools/kconfig/*.d || true
	rm $(PKG_FULL_DIR)/tools/kconfig/.config || true
	cp files/config/$(LINUX_ENV_NAME).env $(PKG_FULL_DIR)/support/
	cp files/config/ugw$(WAVE_CFG_SFX).config $(PKG_FULL_DIR)/ugw.config
	$(if $(BINARY_PKG),\
		echo "CONFIG_USE_PREBUILT_RFLIB=y" >> $(PKG_FULL_DIR)/ugw.config;
		echo "CONFIG_RFLIB_PATH=\"\$$$$(pwd)/../../wireless/driver/rflib/prebuilt\"" >> $(PKG_FULL_DIR)/ugw.config;
	)
	$(if $(CONFIG_PACKAGE_kmod-lantiq-cpufreq),\
		echo "CONFIG_PMCU_SUPPORT=y"  >> $(PKG_FULL_DIR)/ugw.config;
	)
	sh -c "cd $(PKG_FULL_DIR); $(MAKE) defconfig PWD=$(PKG_FULL_DIR)/ MINICONFIG=ugw.config APP_TOOLCHAIN_DIR=$(TOPDIR) KERNEL_DIR=$(LINUX_DIR) CPPFLAGS=\"-I$(STAGING_DIR)/usr/include\" LDFLAGS=\"-L$(STAGING_DIR)/usr/lib -lnl\""
endef


define Build/Compile
	sh -c "cd $(PKG_FULL_DIR) && $(MAKE) APP_TOOLCHAIN_DIR=$(TOPDIR) KERNEL_DIR=$(LINUX_DIR)"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) -p $(1)/ramdisk_copy/flash/
	$(INSTALL_DIR) -p $(1)/lib/modules/$(LINUX_VERSION)/net/
	$(INSTALL_DIR) -p $(1)/etc/rc.d/
	$(INSTALL_DIR) -p $(1)/bin/
	$(INSTALL_DIR) -p $(1)/sbin/
	$(INSTALL_DIR) -p $(1)/root/mtlk/images/
	$(INSTALL_DIR) -p $(1)/root/mtlk/cal/
	$(INSTALL_DIR) -p $(1)/etc/hotplug/
	$(INSTALL_DIR) -p $(1)/etc/hotplug.d/
	$(INSTALL_DIR) -p $(1)/usr/local/bin
	$(CP) ./files/ramdisk_copy/* $(1)/ramdisk_copy/flash/
	$(if $(COPY_WAVE300),
		$(CP) ./files/root/mtlk/images/progmodels/wave300/* $(1)/root/mtlk/images/
		$(CP) ./files/root/mtlk/images/fw/ap_upper.bin.platform.Gen3 $(1)/root/mtlk/images/ap_upper_wave300.bin
		$(CP) ./files/root/mtlk/images/fw/ap_upper_debug.bin.platform.Gen3 $(1)/root/mtlk/images/ap_upper_wave300_debug.bin || true
		$(CP) ./files/root/mtlk/images/fw/contr_lm.bin.platform.Gen3 $(1)/root/mtlk/images/contr_lm.bin
		$(CP) ./files/root/mtlk/images/fw/contr_lm_debug.bin.platform.Gen3 $(1)/root/mtlk/images/contr_lm_debug.bin || true
		$(if $(COPY_STA),
			$(CP) ./files/root/mtlk/images/fw/sta_upper.bin.platform.Gen3 $(1)/root/mtlk/images/sta_upper_wave300.bin
			$(CP) ./files/root/mtlk/images/fw/sta_upper_debug.bin.platform.Gen3 $(1)/root/mtlk/images/sta_upper_wave300_debug.bin || true))
	$(if $(COPY_WAVE400),
		$(CP) ./files/root/mtlk/images/progmodels/wave400/* $(1)/root/mtlk/images/ || true
		$(CP) ./files/root/mtlk/images/fw/ap_upper.bin.platform.wave400.fpga $(1)/root/mtlk/images/ap_upper_wave400.bin || true
		$(CP) ./files/root/mtlk/images/cal/cal_wlan0.bin $(1)/root/mtlk/images/cal_wlan0.bin || true
		$(if $(COPY_STA),
			$(CP) ./files/root/mtlk/images/fw/sta_upper.bin.platform.wave400.fpga $(1)/root/mtlk/images/sta_upper_wave400.bin))
	$(if $(COPY_AR10),
		$(CP) ./files/root/mtlk/images/progmodels/ar10/* $(1)/root/mtlk/images/
		$(CP) ./files/root/mtlk/images/fw/ap_upper_debug.bin.platform.wave400.asic $(1)/root/mtlk/images/ap_upper_ar10_debug.bin || true
		$(CP) ./files/root/mtlk/images/fw/ap_upper.bin.platform.wave400.asic $(1)/root/mtlk/images/ap_upper_ar10.bin
		$(if $(COPY_STA),
			$(CP) ./files/root/mtlk/images/fw/sta_upper.bin.platform.wave400.asic $(1)/root/mtlk/images/sta_upper_ar10.bin
			$(CP) ./files/root/mtlk/images/fw/sta_upper_debug.bin.platform.wave400.asic $(1)/root/mtlk/images/sta_upper_ar10_debug.bin || true))
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/driver/mtlk.ko $(1)/lib/modules/$(LINUX_VERSION)/net/
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/rtlogger/mtlklog.ko $(1)/lib/modules/$(LINUX_VERSION)/net/ || $(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/driver/mtlkroot.ko $(1)/lib/modules/$(LINUX_VERSION)/net/ || true
	chmod 555 $(1)/lib/modules/$(LINUX_VERSION)/net/mtlk.ko
	chmod 555 $(1)/lib/modules/$(LINUX_VERSION)/net/mtlklog.ko || chmod 555 $(1)/lib/modules/$(LINUX_VERSION)/net/mtlkroot.ko || true
	$(CP) ./files/scripts/layer_2_actions/* $(1)/etc/rc.d/
	$(CP) ./files/scripts/layer_3_components/*  $(1)/etc/rc.d/
	$(CP) ./files/scripts/host_api/*.csv $(1)/bin/
	$(CP) ./files/scripts/host_api/host_api $(1)/bin/
	$(CP) ./files/root/mtlk/images/fw/fw_scd_file.scd $(1)/bin/ || echo "Skipping fw_scd_file.scd install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/driver/*.mtidlc $(1)/bin/	|| echo "Skipping mtidlc from driver folder install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/mttools/*.mtidlc $(1)/bin/ || echo "Skipping mtidlc from mttools folder install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/mttools/drvhlpr $(1)/bin/	|| echo "Skipping drvhlpr install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/mttools/mtdump $(1)/bin/ || echo "Skipping mtdump install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/mttools/mtlk_cli $(1)/bin/ || echo "Skipping mtlk_cli install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/mttools/BclSockServer $(1)/bin || echo "Skipping BclSockServer install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/wls/rtlogger/logserver $(1)/bin || echo "Skipping logserver install"
	$(CP) $(PKG_FULL_DIR)/builds/$(LINUX_ENV_NAME)/binaries/dut/dutserver $(1)/bin || echo "Skipping Dutserver install"
	./files/config/mtidlc-generator.sh $(1)/bin/ || echo "Skipping generation of minimized mtidlc files"
	
	chmod 555 $(1)/bin/drvhlpr || true
	chmod 555 $(1)/bin/*.mtidlc* || true
	chmod 555 $(1)/bin/mtdump || true
	chmod 555 $(1)/bin/mtlk_cli || true
	chmod 555 $(1)/bin/BclSockServer || true
	chmod 555 $(1)/bin/logserver || true
	chmod 555 $(1)/bin/dutserver || true
	$(CP) ./files/binaries/sbin/* $(1)/sbin/
	$(CP) ./files/etc/* $(1)/etc/
	echo -e "wave_release_minor=\"$(PKG_VERSION)\"\nwave_mac_ver=\"$(FW_VERSION)\"\nwave_progmodel_ver=\"$(PROGMODEL_VERSION_WAVE300)\"\nwave_rdlim_ver=\"$(WAVE_RDLIM_VER)\"\nwave_driver_ver=\"$(DRV_VERSION)\"\nwave_scripts_ver=\"$(WAVE_SCRIPTS_VER)\"" > $(1)/etc/wave_components.ver
	$(if $(COPY_WAVE400),
		echo -e "fpga_sim_ver=\"$(FPGA_SIM_VERSION)\"\nwave400_progmodel_ver=\"$(PROGMODEL_VERSION_WAVE400)\"" >> $(1)/etc/wave_components.ver)
	$(if $(COPY_AR10),
		echo -e "ar10_progmodel_ver=\"$(PROGMODEL_VERSION_AR10)\"" >> $(1)/etc/wave_components.ver)
	$(CP) ./$(FILE_BASE)/$(FILE_COMMON)/* $(1)/usr/local/bin
	$(CP) ./$(FILE_BASE)/$(FILE_BRANCH)/* $(1)/usr/local/bin
	$(CP) ./$(FILE_BASE)/cal_default/* $(1)/root/mtlk/cal/
endef

$(eval $(call BuildPackage,$(DRV_RFLIB_PKG_NAME)))
$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call KernelPackage,$(KERNEL_PKG_NAME)))
