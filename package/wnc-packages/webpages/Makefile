include $(TOPDIR)/rules.mk

PKG_NAME:=webpages
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/webpages
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=Web pages
endef

define Package/webpages/description
  This package contains web pages
endef

define Build/Prepare
endef

define Build/Compile
endef

WWW = P02003

define Package/webpages/install
	$(INSTALL_DIR) $(1)/usr/local/www
	$(CP) $(WWW)/* $(1)/usr/local/www/
	ln -s /mnt/data/backup_log $(1)/usr/local/www/backup_log
endef

$(eval $(call BuildPackage,webpages))
