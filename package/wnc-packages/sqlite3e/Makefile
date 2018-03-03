#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sqlite3e
PKG_VERSION:=3071300
PKG_RELEASE:=1

PKG_SOURCE:=sqlite-autoconf-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.sqlite.org
PKG_MD5SUM:=c97df403e8a3d5b67bb408fcd6aabd8e

PKG_LICENSE:=PUBLICDOMAIN
PKG_LICENSE_FILES:=

PKG_BUILD_DIR:=$(BUILD_DIR)/sqlite-autoconf-$(PKG_VERSION)-e
PKG_BUILD_DEPENDS:=libncurses libreadline

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/sqlite3e/Default
  SUBMENU:=database
  TITLE:=SQLite (v3.x) database engine
  URL:=http://www.sqlite.org/
endef

define Package/sqlite3e/Default/description
 SQLite is a small C library that implements a self-contained, embeddable,
 zero-configuration SQL database engine.
endef

define Package/libsqlite3e
  $(call Package/sqlite3e/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libpthread +librt +libumi +evtdsptch-header
  TITLE+= (library)
endef

define Package/libsqlite3e/description
$(call Package/sqlite3e/Default/description)
 This package contains the SQLite (v3.x) shared library, used by other
 programs.
endef

define Package/sqlite3e-cli
  $(call Package/sqlite3e/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libsqlite3e +libncurses +libreadline
  TITLE+= (cli)
endef

define Package/sqlite3e-cli/description
$(call Package/sqlite3e/Default/description)
 This package contains a terminal-based front-end to the SQLite (v3.x) library
 that can evaluate queries interactively and display the results in multiple
 formats.
endef

TARGET_CFLAGS += $(FPIC) \
	-DSQLITE_ENABLE_UNLOCK_NOTIFY \

TARGET_CFLAGS += -DADAPTOS_LINUX -DUMI -I. -I${STAGING_DIR}/usr/include/adaptos

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \

CONFIGURE_VARS += \
	config_BUILD_CC="$(HOSTCC)" \
	config_BUILD_CFLAGS="-O2" \
	config_TARGET_CC="$(TARGET_CC)" \
	config_TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	config_TARGET_READLINE_INC="$(TARGET_CPPFLAGS)" \
	config_TARGET_READLINE_LIBS="$(TARGET_LDFLAGS) -lreadline -lncurses" \

define Build/Prepare
	$(PKG_UNPACK)
	rm -rf $(PKG_BUILD_DIR)
	$(MKDIR) -p $(1)/usr/lib
	mv $(BUILD_DIR)/sqlite-autoconf-$(PKG_VERSION) $(PKG_BUILD_DIR)
	$(call Build/Patch/Default)
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/sqlite3e/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/sqlite3{,ext}.h $(1)/usr/include/sqlite3e/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/sqliteWNCLib.h $(1)/usr/include/sqlite3e/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite3e.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/sqlite3.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libsqlite3e/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite3e.so.* $(1)/usr/lib/
endef

define Package/sqlite3e-cli/install
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sqlite3e $(1)/usr/local/bin/sqlite3
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sqlplus $(1)/usr/local/bin/sqlite+
endef

$(eval $(call BuildPackage,libsqlite3e))
$(eval $(call BuildPackage,sqlite3e-cli))
