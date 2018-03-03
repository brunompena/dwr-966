# 
# Copyright (C) 2008-2011 OpenWrt.org
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gogoc
PKG_VERSION:=1.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://openwrt.linux-appliance.net/sources
PKG_MD5SUM:=eeac7292a622681651ec3bd9b2e5b061
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/gogoc
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+libpthread +ip +kmod-tun +kmod-ipv6 +uclibcxx +radvd
  TITLE:=IPv6 tunnel client software
  URL:=http://www.go6.net
endef

define Package/gogoc/description
  Client to configure an IPv6 tunnel to
  Hexago/freenet6/go6.net's migration broker.
endef

define Build/Configure
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/gogoc/include
endef

#define Build/Compile
#	$(MAKE) -C $(PKG_BUILD_DIR)/gogoc-tsp \
#		CC="$(TARGET_CC)" CPP="$(TARGET_CPP)" CXX="$(TARGET_CXX)" RANLIB="$(TARGET_CROSS)ranlib" ARCHIVER="$(TARGET_CROSS)ar" C_LINKER="$(TARGET_CC)" LINKER="$(TARGET_CROSS)g++" \
#		LDFLAGS="$(TARGET_LDFLAGS) -nodefaultlibs -luClibc++ $(LIBGCC_S)" \
#		CPP_FLAGS="$(TARGET_CFLAGS) -fno-builtin -nostdinc++ -I. -Wall -DNDEBUG -I$(STAGING_DIR)/usr/include/uClibc++ $(TARGET_CPPFLAGS)" \
#		STAGING_DIR="$(STAGING_DIR)" \
#		CFLAGS="$(TARGET_CFLAGS) -fno-builtin -Wall -I$(STAGING_DIR)/usr/include/uClibc++"
#endef

define Build/Compile
	STAGING_DIR="$(STAGING_DIR)" \
	CPP_FLAGS="$(TARGET_CFLAGS) -fno-builtin -nostdinc++ -I. -Wall -DNDEBUG -I$(STAGING_DIR)/usr/include/uClibc++ $(TARGET_CPPFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR)/gogoc-tsp \
		CC="$(TARGET_CC)" CPP="$(TARGET_CPP)" CXX="$(TARGET_CXX)" RANLIB="$(TARGET_CROSS)ranlib" ARCHIVER="$(TARGET_CROSS)ar" C_LINKER="$(TARGET_CC)" LINKER="$(TARGET_CROSS)g++" \
		EXTRA_CXXFLAGS="$(TARGET_CXXFLAGS) -fno-builtin -nostdinc++ -I. -Wall -DNDEBUG -I$(STAGING_DIR)/usr/include/uClibc++ $(TARGET_CXXFLAGS)"
endef
#		installdir="$(PKG_INSTALL_DIR)"
#define Build/Compile
#	$(MAKE) -C $(PKG_BUILD_DIR)/gogoc-tsp
#endef

define Package/gogoc/install
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gogoc-tsp/bin/gogoc $(1)/usr/local/bin
endef

$(eval $(call BuildPackage,gogoc))
