include $(TOPDIR)/rules.mk

PKG_NAME:=jnrp-rootfs
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/jnrp-rootfs
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=JNRP rootfs
endef

define Package/jnrp-rootfs/description
  This package contains JNRP rootfs
endef

define Build/Prepare
	$(CP) JNRP/* $(PKG_BUILD_DIR)/
endef

PROJECT_NAME = Dora_WLD3
LTE_STATUS_SCRIPT = Generic
SAMBA_CONFIG = Dora_WLD3

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	PROJECT="$(PROJECT_NAME)"
endef

define Package/jnrp-rootfs/install
	$(INSTALL_DIR) $(1)/sysconfig
	$(INSTALL_DIR) $(1)/usr/local/etc/syslog-ng
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(CP) $(PKG_BUILD_DIR)/jnr-cfg.ascii.default_$(PROJECT_NAME) $(1)/usr/local/etc/jnr-cfg.ascii.default
	$(CP) $(PKG_BUILD_DIR)/jnr-cfg.ascii.patch $(1)/usr/local/etc/
	$(CP) $(PKG_BUILD_DIR)/ipv6-cfg.default $(1)/usr/local/etc/
	$(CP) $(PKG_BUILD_DIR)/vpn-cfg.default $(1)/usr/local/etc/
	$(CP) $(PKG_BUILD_DIR)/samba-cfg.default_$(SAMBA_CONFIG) $(1)/usr/local/etc/samba-cfg.default
	$(CP) $(PKG_BUILD_DIR)/system.sql $(1)/usr/local/etc/
	$(CP) $(PKG_BUILD_DIR)/version $(1)/usr/local/etc/
	$(CP) $(PKG_BUILD_DIR)/syslog-ng/* $(1)/usr/local/etc/syslog-ng
	$(CP) $(PKG_BUILD_DIR)/ssh/* $(1)/usr/local/etc/
	$(CP) $(PKG_BUILD_DIR)/system/config_$(PROJECT_NAME)/user.sql $(1)/usr/local/etc/
	$(CP) $(PKG_BUILD_DIR)/lte-script/lteStatus_$(LTE_STATUS_SCRIPT).sh $(PKG_BUILD_DIR)/hook-scripts/init/lteStatus.sh
	$(CP) $(PKG_BUILD_DIR)/hook-scripts/init/* $(1)/usr/local/bin/
	ln -sf /usr/bin/lua $(1)/usr/local/bin/lua
	$(CP) $(PKG_BUILD_DIR)/base-files/* $(1)/usr/local/
endef

$(eval $(call BuildPackage,jnrp-rootfs))
