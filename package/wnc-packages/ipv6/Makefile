#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipv6
PKG_VERSION:=20130624
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ipv6/Default
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=wnc-ipv6
  SUBMENU:=IPv6
endef

define Build/Prepare
  $(CP) $(PKG_NAME)/* $(PKG_BUILD_DIR)/
endef


define Package/ipv6-prefix
  $(call Package/ipv6/Default)
  TITLE:=ipv6 prefix generator
endef

define Package/ipv6-basic
  $(call Package/ipv6/Default)
  TITLE:=WNC IPv6 function
  DEPENDS:= +ipv6-prefix +radvd +wide-dhcpv6
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

define Package/ipv6-basic/install
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(INSTALL_BIN) ./P02003/*.sh $(1)/usr/local/bin
endef

define Package/ipv6-prefix/install
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ipv6_prefix $(1)/usr/local/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ipv6_reformat $(1)/usr/local/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/create_tayga_conf $(1)/usr/local/bin/
endef

$(eval $(call BuildPackage,ipv6-prefix))
$(eval $(call BuildPackage,ipv6-basic))

