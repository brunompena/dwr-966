# 
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=minidlna
PKG_VERSION:=1.0.21
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION)_src.tar.gz
PKG_SOURCE_URL:=@SF/minidlna
PKG_MD5SUM:=a53e0008ad225843a1782e4af6e5bf0b

PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

MAKE_VARS += \
	ICONV_PREFIX="$(ICONV_PREFIX)" \
	INTL_PREFIX="$(INTL_PREFIX)" \
	$(if $(CONFIG_FFMPEG_PATENTED),$(if $(CONFIG_FFMPEG_MINIDLNA_SUPPORT),\
		LTQ_CFLAGS="-DFFMPEG_MINIDLNA_SUPPORT"))

define Package/minidlna
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=UPnP A/V & DLNA Media Server
  URL:=http://minidlna.sourceforge.net/
  DEPENDS:= +libpthread +libexif +libjpeg +libsqlite3 +libffmpeg \
  	+@FFMPEG_MINIDLNA_SUPPORT +libid3tag +libflac +libvorbis +libuuid \
  	+@BUILD_NLS $(ICONV_DEPENDS) $(INTL_DEPENDS)
endef

define Package/minidlna/description
  MiniDLNA (aka ReadyDLNA) is server software with the aim of
  being fully compliant with DLNA/UPnP-AV clients.
endef

define Package/minidlna/conffiles
/etc/minidlna.conf
endef

define Package/minidlna/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/minidlna.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/minidlna $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ltq_mediaserver_ctl.sh $(1)/etc/init.d/ltq_mediaserver_ctl.sh
endef

$(eval $(call BuildPackage,minidlna))
