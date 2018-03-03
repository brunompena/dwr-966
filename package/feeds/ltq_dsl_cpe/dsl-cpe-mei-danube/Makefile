#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=dsl-cpe-mei-danube
PKG_VERSION:=5.8.5.1
PKG_MD5SUM:=2a5077304e06d675220d4d08592dacce
PKG_BASE_NAME:=drv_mei_cpe_danube
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_INSTALL:=1
PKG_SOURCE_URL:=http://localhost
#PKG_BUILD_DEPENDS:=ifxos
LANTIQ_BIN_DIR=opt/lantiq/bin
#FW_DIR=/lib/firmware/$(LINUX_VERSION)

include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
   STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell $(SH_FUNC) ( \
      grep '^CONFIG_$(PKG_NAME)' $(TOPDIR)/.config) | md5s)
endif

define Build/Compile
# add installation
	$(call Build/Compile/Default, DESTDIR="$(PKG_INSTALL_DIR)" CC="$(TARGET_CC)" all install )
endef

define Package/$(PKG_NAME)
  SECTION:=driver
  CATEGORY:=Lantiq
  SUBMENU:=DSL Subsystem
  TITLE:=DSL CPE MEI Driver for ARX Family
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=+kmod-dsl-cpe-mei-danube
  MENU:=1
endef

define KernelPackage/$(PKG_NAME)
  SUBMENU:=Lantiq
  TITLE:=DSL CPE MEI Driver for ARX Family
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=
  KCONFIG:=CONFIG_LTQ_DSL_CPE_MEI_ARX=y CONFIG_LIBCRC32C=y
#  FILES:=$(PKG_BUILD_DIR)/ifxmips_mei.$(LINUX_KMOD_SUFFIX)
endef

# package specific configuration
define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
	source "$(SOURCE)/Config.in"
endef

define Package/$(PKG_NAME)/description
	Lantiq Danube MEI CPE Kernel driver.
endef

define KernelPackage/$(PKG_NAME)/description
	Lantiq Danube MEI CPE Kernel driver.
endef

CONFIGURE_ARGS += \
	--enable-kernelincl="$(LINUX_DIR)/include"

ifeq ($(CONFIG_$(PKG_NAME)_MODEL_DEFAULT),y)
CONFIGURE_ARGS += --enable-model=default
else ifeq ($(CONFIG_$(PKG_NAME)_MODEL_DEBUG),y)
CONFIGURE_ARGS += --enable-model=debug
else
CONFIGURE_ARGS += --enable-model=default
endif

ifeq ($(CONFIG_$(PKG_NAME)_USE_KERNEL_BUILD_IN),y)
CONFIGURE_ARGS += \
	--enable-ifxos_drv=no \
	--enable-lkm=no
endif

ifeq ($(CONFIG_$(PKG_NAME)_USE_ADDITIONAL_BUILD_OPTIONS),y)
CONFIGURE_ARGS += $(CONFIG_$(PKG_NAME)_ADDITIONAL_BUILD_OPTIONS)
endif

#ifeq ($(CONFIG_LINUX_2_6),y)
CONFIGURE_ARGS += \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION=.ko
#endif

ifeq ($(CONFIG_$(PKG_NAME)_DBG_BAR_MEMORY_CHECK),y)
CONFIGURE_ARGS += --enable-bar-mem-check=yes
endif

ifeq ($(CONFIG_mips),y)
EXTRA_CFLAGS = -fno-pic -mno-abicalls -mlong-calls -G 0
endif

#EXTRA_CFLAGS = -nostdinc -fno-delete-null-pointer-checks -fno-reorder-blocks -fno-tree-ch -ffunction-sections \
#	-mno-check-zero-division -mabi=32 -ffreestanding -fno-stack-protector -fomit-frame-pointer -fno-strict-overflow

define Build/Configure
	# remove "configured" stamps of other configs
	@- rm $(PKG_BUILD_DIR)/.configured*
	$(call Build/Configure/Default)
endef

define Build/Clean
	@- rm -Rf $(PKG_BUILD_DIR)/ipkg*
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	-rm -f $(LINUX_DIR)/drivers/char/ltq_dsl_cpe_mei_arx.o
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include}
	$(CP) $(PKG_BUILD_DIR)/src/ifxmips_mei_interface.h $(1)/usr/include/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ltq_load_cpe_mei_drv.sh $(1)/etc/init.d/

	$(INSTALL_DIR) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) ./files/inst_drv_mei_cpe.sh $(1)/$(LANTIQ_BIN_DIR)

ifeq ($(CONFIG_dsl-cpe-mei-danube_USE_KERNEL_BUILD_IN),y)
	$(INSTALL_DIR) $(1)/$(FW_DIR)
	$(CP) -s $(PKG_BUILD_DIR)/src/drv_mei_cpe_danube.o $(LINUX_DIR)/drivers/char/ltq_dsl_cpe_mei_arx.o
else
	-rm -f $(LINUX_DIR)/drivers/char/ltq_dsl_cpe_mei_arx.o
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_BASE_NAME)$(PKG_MODULE_EXTENSION) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)
	ln -snf /$(LANTIQ_BIN_DIR)/$(PKG_BASE_NAME)$(PKG_MODULE_EXTENSION) $(1)/$(MODULES_SUBDIR)/$(PKG_BASE_NAME).$(LINUX_KMOD_SUFFIX)

	$(INSTALL_DIR) $(1)/$(FW_DIR)
endif

ifeq ($(CONFIG_LINUX_2_6),y)
	$(CP) -s $(PKG_BUILD_DIR)/src/ifxmips_mei_interface.h $(LINUX_DIR)/include/asm/ifx/ifxmips_mei_interface.h
	-rm -f $(LINUX_DIR)/drivers/char/ltq_dsl_cpe_mei_arx.o
endif
ifneq ($(CONFIG_LINUX_3_8)$(CONFIG_LINUX_3_10),)
	$(CP) -s $(PKG_BUILD_DIR)/src/ifxmips_mei_interface.h $(LINUX_DIR)/arch/mips/include/asm/mach-lantiq/ifxmips_mei_interface.h
endif
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call KernelPackage,$(PKG_NAME)))
