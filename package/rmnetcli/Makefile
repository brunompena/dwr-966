include $(TOPDIR)/rules.mk

PKG_NAME:=rmnetcli
PKG_VERSION:=LNX.LE.4.1-41009-9x35
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rmnetcli
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=RMNETCLI
  DEPENDS:= +libpthread +librt
endef

define Package/rmnetcli/description
  This package contains rmnetcli
endef

define Build/Prepare
	$(CP) data-oss/* $(PKG_BUILD_DIR)/
	(cd $(PKG_BUILD_DIR); sh autogen.sh)
endef

define Package/rmnetcli/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/rmnetctl/cli/rmnetcli $(1)/usr/bin/
endef

$(eval $(call BuildPackage,rmnetcli))
