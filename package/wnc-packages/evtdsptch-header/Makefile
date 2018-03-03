include $(TOPDIR)/rules.mk

PKG_NAME:=evtdsptch-header
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/evtdsptch-header
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=event dispatcher
endef

define Package/evtdsptch-header/description
  This package contains event dispatcher header
endef

define Build/Prepare
	$(CP) evtDsptch/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/evtDsptch.h $(1)/usr/include/
endef

$(eval $(call BuildPackage,evtdsptch-header))
