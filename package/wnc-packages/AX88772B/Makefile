include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=asix
PKG_VERSION:=20100816
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/asix
  SUBMENU:=USB Support
  TITLE:=asix driver
  FILES:=$(PKG_BUILD_DIR)/asix.ko
endef

define KernelPackage/asix/description
  This package contains asix driver
endef

define Build/Prepare
  echo "Prepare Go"
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) AX88772B/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

MAKE_OPTS = \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) $(MAKE_OPTS) modules
	
endef

$(eval $(call KernelPackage,asix))
