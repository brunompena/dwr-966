include $(TOPDIR)/rules.mk

PKG_NAME:=libmd5
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libmd5
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=MD5 library
endef

define Package/libmd5/description
  This package contains MD5 library
endef

define Build/Prepare
	$(CP) libmd5/* $(PKG_BUILD_DIR)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/md5
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/global.h $(1)/usr/include/md5/
	$(CP) $(PKG_BUILD_DIR)/src/md5.h $(1)/usr/include/md5/
	$(CP) $(PKG_BUILD_DIR)/src/libmd5.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libmd5))
