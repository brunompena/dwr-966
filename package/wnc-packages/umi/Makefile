include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=umi
PKG_VERSION:=20130617
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/umi
  SECTION:=base
  CATEGORY:=WNC
  TITLE:=UMI driver
  FILES:=$(PKG_BUILD_DIR)/umi.ko
endef

define KernelPackage/umi/description
  This package contains UMI driver
endef

define Build/Prepare
	$(CP) umi_kernel/* $(PKG_BUILD_DIR)/
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

$(eval $(call KernelPackage,umi))
