include $(TOPDIR)/rules.mk

PKG_NAME:=goahead
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/goahead
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=goahead
  DEPENDS:= +libjnrpsql +libopenssl
endef

define Package/goahead/description
  This package contains goahead
endef

TARGET_CFLAGS += -I. -I${STAGING_DIR}/usr/include -I${STAGING_DIR}/usr/include/adaptos -I${STAGING_DIR}/usr/include/sqlite3e
ifdef CONFIG_TARGET_lantiq_xrx330_P01001
TARGET_CFLAGS += -D__P01001_TCLGENERIC
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P01002
TARGET_CFLAGS += -D__P01002_Telenor
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P01003
TARGET_CFLAGS += -D__P01003_UNE
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P02001
TARGET_CFLAGS += -D__P02001_Dora
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P02002
TARGET_CFLAGS += -D__P02002_Dora
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P02003
TARGET_CFLAGS += -D__P02003_Dora
endif

define Build/Prepare
	$(CP) goahead/* $(PKG_BUILD_DIR)/
endef

define Package/goahead/install
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(INSTALL_DIR) $(1)/usr/local/www/cgi-bin
	$(CP) $(PKG_BUILD_DIR)/src/webs $(1)/usr/local/bin/
	$(CP) $(PKG_BUILD_DIR)/cgi-src/*.cgi $(1)/usr/local/www/cgi-bin/
endef

$(eval $(call BuildPackage,goahead))
