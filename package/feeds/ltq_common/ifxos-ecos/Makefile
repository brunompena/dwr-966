#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ifxos-ecos
PKG_BASE_NAME:=lib_ifxos
PKG_VERSION:=1.5.19
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_BUILD_DEPENDS:=ecos
PKG_SOURCE_URL:=http://localhost
PKG_MD5SUM:=b5c502608ea3bdfde182a620d627bb3f

PKG_FIXUP:=libtool
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
	STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell $(SH_FUNC) grep '^CONFIG_PACKAGE_.*ifxos' $(TOPDIR)/.config | md5s)$(shell $(SH_FUNC) grep '^CONFIG_ECOS_' $(TOPDIR)/.config | md5s)
endif

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Lantiq
  SUBMENU:=IFXOS
  TITLE:=Lantiq OS abstraction library (eCos)
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq
  MENU:=1
  DEPENDS:=+ecos
endef

define Package/$(PKG_NAME)/config
  $(call Package/$(PKG_NAME)/override_source_path)
	menu "configuration"
	depends on PACKAGE_$(PKG_NAME)
		source "$(SOURCE)/config/Config.ecos"
	endmenu
endef

define Package/$(PKG_NAME)/description
  This library will be used by Lantiq SFP applications / drivers under eCos
endef

# make the submake silent:
ifeq ($(QUIET),1)
MAKE_FLAGS+=-s
endif
ifneq (,$(findstring 99,$(V)))
MAKE_FLAGS+=-s
endif

CONFIGURE_ARGS += \
	--enable-ecos \
	--enable-kernelincl="$(STAGING_DIR)/usr/include/ecos" \
	--without-kernel-module

TARGET_CFLAGS += -DIFXOS_ADD_STATIC_MEMORY_FILE -ggdb

ifeq ($(CONFIG_IFXOS_ECOS_OVERRIDE_DEVIO_SETTINGS),y)

TARGET_CFLAGS += \
	-DDEVIO_MAXDRIVERS=$(strip $(subst ",,$(CONFIG_IFXOS_ECOS_DEVIO_MAXDRIVERS))) \
	-DDEVIO_MAXDEVICES=$(strip $(subst ",,$(CONFIG_IFXOS_ECOS_DEVIO_MAXDEVICES))) \
	-DDEVIO_MAXFDS=$(strip $(subst ",,$(CONFIG_IFXOS_ECOS_DEVIO_MAXFDS)))

endif

define Build/Clean
	@- rm -Rf $(PKG_BUILD_DIR)/ipkg*
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/ifxos}
	$(CP) $(PKG_BUILD_DIR)/src/include/* $(1)/usr/include/ifxos

	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libifxos.a $(1)/usr/lib/libifxos-ecos.a

endef

define Build/UninstallDev
	- rm -Rf $(1)/usr/include/ifxos
	- rm -f $(1)/usr/lib/libifxos*
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
