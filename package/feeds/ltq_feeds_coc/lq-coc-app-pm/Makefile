#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lq-coc-app-pm
PKG_BASE_NAME:=lq_coc_app_pm
PKG_VERSION:=2.0.6.41
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://localhost
PKG_BIN_DIR=opt/lantiq/bin
PKG_BUILD_DEPENDS:=
PKG_INSTALL:=1
PKG_START_SCRIPT:=pm_utility.sh
PKG_MD5SUM:=a4aaa2c1175cf2d99c64f9959e09de6c
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=power
  CATEGORY:=Lantiq
  TITLE:=Power Management Utility
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=+libpthread +librt +ifxos +FEATURE_LQ_COC_APP_PM_SUPPORT_DTI:lib-dti
  MENU:=1
endef

define Package/$(PKG_NAME)/description
	Power Management Utility for XRX200 and XRX300 platforms.
   This application offers access via the ioctl interface to the builtin kernel-driver "ifx_pmcu".
   Features: 
      - Display current power state of all registered modules
      - Get module specific power state
      - Change/Set power state of a specific module
   Example: pmcu_utility -h    "print help menu"
   
endef

### Menuconfig ##################################
define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
	source "$(SOURCE)/config/Config.in"
endef

CONFIGURE_ARGS += \
	--enable-ifxos \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-ifxos-library="-L$(STAGING_DIR)/usr/lib"

ifeq ($(CONFIG_FEATURE_LQ_COC_APP_PM_SUPPORT_CLI),y)
CONFIGURE_ARGS += \
	--enable-cli \
	--enable-cli-include="-I$(STAGING_DIR)/usr/include/cli" \
	--enable-cli-library="-L$(STAGING_DIR)/usr/lib"
endif

ifeq ($(CONFIG_FEATURE_LQ_COC_APP_PM_SUPPORT_DTI),y)
CONFIGURE_ARGS += \
	--enable-dti \
	--enable-dti-include="-I$(STAGING_DIR)/usr/include/dti" \
	--enable-dti-library="-L$(STAGING_DIR)/usr/lib"
endif


#### Target Rules ###############################
define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_START_SCRIPT) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/$(PKG_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pmcu_utility $(1)/opt/lantiq/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pmcu_log $(1)/opt/lantiq/bin/
ifeq ($(CONFIG_FEATURE_LQ_COC_APP_PM_SUPPORT_DTI),y)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pmcu_utility_dti $(1)/opt/lantiq/bin/
endif
ifeq ($(CONFIG_FEATURE_LQ_COC_APP_PM_SUPPORT_CLI),y)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pmcu_utility_cli $(1)/opt/lantiq/bin/
endif
endef


define Build/Configure
### remove "configured" stamps of other configs
	@- rm $(PKG_BUILD_DIR)/.configured*
	$(call Build/Configure/Default)
endef

define Build/Clean
	@- rm -Rf $(PKG_BUILD_DIR)/ipkg*
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
