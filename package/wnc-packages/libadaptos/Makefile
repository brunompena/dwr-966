include $(TOPDIR)/rules.mk

PKG_NAME:=libadaptos
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libadaptos
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=ADAPTOS library
endef

define Package/libadaptos/description
  This package contains ADAPTOS library
endef

define Build/Prepare
	$(CP) adaptos/* $(PKG_BUILD_DIR)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/adaptos
	$(INSTALL_DIR) $(1)/usr/lib
	for i in `find ${PKG_BUILD_DIR}/include/adaptos -maxdepth 1 -name *.h`; do \
	  $(CP) $$$$i $(1)/usr/include/adaptos/; \
	done
	for i in `find ${PKG_BUILD_DIR}/include/adaptos/linux -maxdepth 1 -name *.h`; do \
	  $(CP) $$$$i $(1)/usr/include/adaptos/; \
	done
	$(CP) $(PKG_BUILD_DIR)/libadaptos.so $(1)/usr/lib/
endef

define Package/libadaptos/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libadaptos.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libadaptos))
