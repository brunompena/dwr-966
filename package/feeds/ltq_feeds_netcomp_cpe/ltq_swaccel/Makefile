include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lantiq-swaccel
PKG_VERSION:=1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/lantiq-swaccel
  SUBMENU:=Lantiq
  TITLE:=Lantiq Software Acceleration Module
  URL:=http://www.lantiq.com
  MAINTAINER:=Lantiq Technologies AG
  FILES:=$(LINUX_DIR)/drivers/net/lantiq_ppa/ppa_api/ppa_api_sw_accel_mod.ko
endef

define Build/Prepare
	echo -n
endef

define Build/Compile
	echo -n;
endef

$(eval $(call KernelPackage,lantiq-swaccel))
