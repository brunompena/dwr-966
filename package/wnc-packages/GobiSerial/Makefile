include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=GobiSerial
PKG_VERSION:=20100816
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/GobiSerial
  SUBMENU:=USB Support
  TITLE:=GobiSerial driver
  FILES:=$(PKG_BUILD_DIR)/GobiSerial.ko
endef

define KernelPackage/GobiSerial/description
  This package contains GobiSerial driver
endef

define Build/Prepare
  echo "Prepare Go"
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) GobiSerial/* $(PKG_BUILD_DIR)/
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

$(eval $(call KernelPackage,GobiSerial))
