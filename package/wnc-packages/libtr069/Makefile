include $(TOPDIR)/rules.mk

PKG_NAME:=libtr069
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libtr069
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=TR069 library
  DEPENDS:= +libpthread +libopenssl
endef

define Package/libtr069/description
  This package contains TR069 library
endef

CIFLAGS = -I. -I${STAGING_DIR}/usr/include -g -fPIC -fno-stack-protector -D_JNR_ES_ -D_JPR_AM_ -I../ftp
CDFLAGS = -DWITH_TR098 -DLINUX -DLINUX_WGET -DWITH_COOKIES -DCOPPERGATE -DWITH_OPENSSL -DHAVE_FTP_DOWNLOAD -DWITH_TR143
ifdef CONFIG_TARGET_lantiq_xrx330_P02003
CDFLAGS += -DCONFIG_TELENOR_NORWAY -DUPGRADE_ONLY_LTE -DWITH_WIFI_11AC
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P01003
CDFLAGS += -DWITH_TRIPLE_APN
endif

CLFLAGS = -L.
TARGET_CFLAGS += ${CIFLAGS} ${CDFLAGS}
TARGET_LDFLAGS += ${CLFLAGS}
define Build/Prepare
	$(CP) libtr069/* $(PKG_BUILD_DIR)/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtr069.so $(1)/usr/lib/
endef

define Package/libtr069/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libtr069.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtr069))
