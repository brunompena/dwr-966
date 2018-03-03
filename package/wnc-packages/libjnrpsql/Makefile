include $(TOPDIR)/rules.mk

PKG_NAME:=libjnrpsql
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libjnrpsql
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=UMI library
  DEPENDS:= +libsqlite3e
endef

define Package/libjnrpsql/description
  This package contains UMI library
endef

TARGET_CFLAGS += -DADAPTOS_LINUX -I. -I${STAGING_DIR}/usr/include/adaptos -I${STAGING_DIR}/usr/include/sqlite3e
define Build/Prepare
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/jnrp_sql_lib.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/libjnrpsql.so $(1)/usr/lib/
endef

define Package/libjnrpsql/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libjnrpsql.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libjnrpsql))
