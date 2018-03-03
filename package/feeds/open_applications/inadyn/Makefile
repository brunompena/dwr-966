include $(TOPDIR)/rules.mk

PKG_NAME:=inadyn
PKG_VERSION:=1.99
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME).zip

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=DDNS 
  SECTION:=net
  CATEGORY:=Network
  TITLE:=inadyn
endef

define Package/$(PKG_NAME)/description
      INADYN is a dynamic DNS client
endef

#define Build/Prepare
#	$(CP) src/* $(PKG_BUILD_DIR)/
#endef

#define Build/Compile
#	echo "$(PKG_BUILD_DIR) is Here "
#	$(MAKE) -C $(PKG_BUILD_DIR) \
#		TARGET_CFLAGS="$(TARGET_CFLAGS)" \
#		TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
#		PKG_NAME="$(PKG_NAME)" \
#		$(TARGET_CONFIGURE_OPTS) \
#		all
#endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/linux/inadyn $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
