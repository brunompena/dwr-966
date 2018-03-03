include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gdm-lte
PKG_VERSION:=v3.7.18.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gdm-lte
  SUBMENU:=USB Support
  TITLE:=GCT gdm-lte driver
  FILES:=$(PKG_BUILD_DIR)/gdmulte.ko $(PKG_BUILD_DIR)/gdmtty.ko
endef

define KernelPackage/gdm-lte/description
  This package contains GCT gdm-lte driver
endef

define Build/Prepare
  echo "Prepare Go"
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)/
	$(CP) src/.config $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) LINUX_KARCH=$(LINUX_KARCH) TARGET_CROSS=$(TARGET_CROSS) LINUX_DIR=$(LINUX_DIR)
endef

#define Package/gdm-lte/install
#	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
#	$(CP) $(PKG_BUILD_DIR)/*.ko $(1)/lib/modules/$(LINUX_VERSION)/
#endif

$(eval $(call KernelPackage,gdm-lte))
