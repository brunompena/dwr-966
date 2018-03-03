include $(TOPDIR)/rules.mk

PKG_NAME:=tayga
PKG_VERSION:=0.9.2
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/tayga
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=tayga library
endef

define Package/tayga/description
  This package contains tayga library
endef


define Build/Prepare
	$(CP) tayga/* $(PKG_BUILD_DIR)/
endef

define Package/tayga/install
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(CP) $(PKG_BUILD_DIR)/tayga $(1)/usr/local/bin
endef

$(eval $(call BuildPackage,tayga))
