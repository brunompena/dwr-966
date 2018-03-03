include $(TOPDIR)/rules.mk

PKG_NAME:=libumi
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libumi
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=UMI library
  DEPENDS:= +libadaptos
endef

define Package/libumi/description
  This package contains UMI library
endef

TARGET_CFLAGS += -DADAPTOS_LINUX -I. -I${STAGING_DIR}/usr/include/adaptos
define Build/Prepare
	$(CP) umi_user/* $(PKG_BUILD_DIR)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/umi.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libumi.so $(1)/usr/lib/
endef

define Package/libumi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libumi.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libumi))
