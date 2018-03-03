#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vpn-mgr
PKG_VERSION:=20130701
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/vpn-mgr/Default
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=vpn-mgr
  SUBMENU:=VPN
endef

define Build/Prepare
  $(CP) $(PKG_NAME)/* $(PKG_BUILD_DIR)/
endef


define Package/vpn-mgr
  $(call Package/vpn-mgr/Default)
  TITLE:=wnc vpn manager
endef

export BUILD_CC="$(TARGET_CC)"

CONFIGURE_ARGS += \

CONFIGURE_VARS += \
	BUILD_CC="$(TARGET_CC)" \

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		BUILD_CC="$(BUILD_CC)" \
		CC="$(BUILD_CC)" \
		CFLAGS="-O2" \
		LIBS=""
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/vpn-mgr/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/vpn-mgr $(1)/usr/bin/
endef

$(eval $(call BuildPackage,vpn-mgr))

