#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-voice-kpi2udp
PKG_VERSION:=3.0.5.1
PKG_MD5SUM:=bebab09fb251f0fb241e6af21298eb93
PKG_RELEASE:=1

# IMPORTANT - include the version-override here after PKG_VERSION definition
include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=drv_kpi2udp-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://localhost
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/drv_kpi2udp-$(PKG_VERSION)
PKG_START_SCRIPT:=load_voice_kpi2udp.sh
PKG_BUILD_DEPENDS:=ltq-voice-tapi ifxos
LANTIQ_BIN_DIR=opt/lantiq/bin

# Add dependencies on symbols in the configure file
PKG_CONFIG_DEPENDS:=\
       $(foreach _mr,$(shell grep -w config config/Config.in|awk '{ print $$2 }'),CONFIG_$(_mr)) \

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=driver
  CATEGORY:=Lantiq
  TITLE:=TAPI KPI2UDP plug-in
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  MENU:=1
  DEPENDS:=ltq-voice-tapi +kmod-ifxos
endef

define Package/$(PKG_NAME)/description
	RTP packet path acceleration into IP stack (strongly recommended)
endef

define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
#	menu "configuration"
#		depends on PACKAGE_$(PKG_NAME)
#		source "$(SOURCE)/config/Config.in"
#	endmenu
	source "$(SOURCE)/config/Config.in"
endef

# Add user specified CFLAGS
ifneq ($(CONFIG_LTQ_VOICE_KPI2UDP_USER_FLAGS),"")
KPI2UDP_USER_CFLAGS=$(strip $(subst ",, $(CONFIG_LTQ_VOICE_KPI2UDP_USER_FLAGS)))
CONFIGURE_ARGS += --with-cflags="$(KPI2UDP_USER_CFLAGS)"
endif

# Add user specified configure options
ifneq ($(CONFIG_LTQ_VOICE_KPI2UDP_USER_OPTIONS),"")
CONFIGURE_ARGS += $(subst ",, $(CONFIG_LTQ_VOICE_KPI2UDP_USER_OPTIONS))
endif

DRV_TAPI_INCLUDE:=$(STAGING_DIR)/usr/include/drv_tapi

CONFIGURE_ARGS += --with-kernel-incl="$(LINUX_DIR)/include"
CONFIGURE_ARGS += --with-tapi-incl="$(DRV_TAPI_INCLUDE)"
CONFIGURE_ARGS += --with-ifxos-incl=$(STAGING_DIR)/usr/include/ifxos
CONFIGURE_ARGS += --enable-warnings

ifneq ($(CONFIG_LINUX_2_4),y)
CONFIGURE_ARGS += --enable-linux-26 --with-kernel-build="$(LINUX_DIR)" ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION=.$(LINUX_KMOD_SUFFIX)
endif

#define Build/InstallDev
#	$(INSTALL_DIR) $(1)/usr/include
#	mkdir -p $(1)/usr/include/drv_kpi2udp
#	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/drv_kpi2udp
#endef

#define Build/UninstallDev
#	rm -rf $(BUILD_DIR)/drv_kpi2udp
#endef

define Build/Clean
	@- [ -e $(PKG_BUILD_DIR)/ipkg ] && rm -Rf $(PKG_BUILD_DIR)/ipkg
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/Configure
	rm -f $(PKG_BUILD_DIR)/.configured*
	$(call Build/Configure/Default)
endef

define Package/$(PKG_NAME)/install
	# Startup script
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_START_SCRIPT) $(1)/etc/init.d/
	# Primary install location of the driver binary
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/drv_kpi2udp$(PKG_MODULE_EXTENSION) $(1)/$(MODULES_SUBDIR)
	# For easy access also create a link in Lantiq bin directory
	$(INSTALL_DIR) $(1)/$(LANTIQ_BIN_DIR)
	ln -snf /$(MODULES_SUBDIR)/drv_kpi2udp$(PKG_MODULE_EXTENSION) $(1)/$(LANTIQ_BIN_DIR)/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
