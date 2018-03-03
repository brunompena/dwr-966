#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ushare
PKG_VERSION:=1.1a
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://ushare.geexbox.org/releases/
PKG_MD5SUM:=5bbcdbf1ff85a9710fa3d4e82ccaa251

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

ifdef CONFIG_TARGET_lantiq_xrx330_P01001
DLNA_CONF=ushare_P01001.conf
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P01002
DLNA_CONF=ushare_P01001.conf
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P01003
DLNA_CONF=ushare_P01001.conf
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P02001
DLNA_CONF=ushare_P02001.conf
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P02002
DLNA_CONF=ushare_P02001.conf
endif
ifdef CONFIG_TARGET_lantiq_xrx330_P02003
DLNA_CONF=ushare_P02003.conf
endif

define Package/ushare
 SECTION:=multimedia
 CATEGORY:=Multimedia
 TITLE:= UPnP A/V & DLNA Media Server
 URL:=http://ushare.geexbox.org/
 DEPENDS:=+libdlna +libupnp +libffmpeg
endef

define Package/ushare/description
 uShare is a UPnP (TM) A/V & DLNA Media Server. It implements the server 
 component that provides UPnP media devices with information on available 
 multimedia files.
endef

MAKE_INSTALL_FLAGS+= \
	INSTALL="install" \

define Build/Configure
	# this is *NOT* GNU configure
	( cd $(PKG_BUILD_DIR); \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) -Wl,-rpath-link=$(STAGING_DIR)/usr/lib" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(STAGING_DIR)/usr/lib/pkgconfig" \
		./configure \
		--cross-compile \
		--cross-prefix="$(TARGET_CROSS)" \
		--prefix="/usr" \
		--sysconfdir="/etc" \
		--enable-dlna \
		--disable-nls \
		--disable-debug \
		--disable-optimize \
		--disable-strip \
	)
endef

define Package/ushare/conffiles
/etc/config/ushare
/etc/ushare.conf
endef

define Package/ushare/install
	$(INSTALL_DIR) $(1)/etc
	$(CP) ./files/$(DLNA_CONF) $(1)/etc/ushare.conf
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ushare $(1)/usr/local/bin/
	$(CP) ./files/dlna.sh $(1)/usr/local/bin/
	$(CP) ./files/rebuild_playlist.sh $(1)/usr/local/bin/
endef

$(eval $(call BuildPackage,ushare))
