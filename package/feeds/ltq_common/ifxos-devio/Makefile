#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ifxos-devio
PKG_BASE_NAME:=lib_ifxos
PKG_VERSION:=1.5.19
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://localhost
PKG_MD5SUM:=b5c502608ea3bdfde182a620d627bb3f

PKG_FIXUP:=libtool
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
	STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell $(SH_FUNC) grep '^CONFIG_PACKAGE_ifxos-devio' $(TOPDIR)/.config | md5s)
endif

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Lantiq
  SUBMENU:=IFXOS
  TITLE:=Lantiq OS abstraction library (devio)
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq
  MENU:=1
  DEPENDS:=+kmod-ifxos
endef

define Package/$(PKG_NAME)/description
 Lantiq IFXOS - compiled for devio interface.
endef

CONFIGURE_ARGS += \
	--without-kernel-module\
	--enable-add_drv_cflags="-DIFXOS_DEBUG_USR_PRINT=0 -DIFXOS_USE_ERROR_USR_PRINT=0" \
	--enable-add_appl_cflags="-DIFXOS_DEBUG_USR_PRINT=0 -DIFXOS_USE_ERROR_USR_PRINT=0 -DIFXOS_USE_DEV_IO=1" \
	--enable-targetincl=$(STAGING_DIR)/usr/include\
	--enable-target=$(STAGING_DIR)/usr/lib

define Build/Clean
	@- rm -Rf $(PKG_BUILD_DIR)/ipkg*
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/libifxos.a $(1)/usr/lib/libifxos_devio.a
endef

define Build/UninstallDev
	- rm -f $(1)/usr/lib/libifxos_devio.a
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

