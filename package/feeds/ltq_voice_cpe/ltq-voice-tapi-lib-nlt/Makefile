#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-voice-lib-nlt
PKG_VERSION:=0.3.0.0
PKG_MD5SUM:=5f546849c6bfa093debece80d1f2d039
PKG_RELEASE:=1

TAPI_LIB_NLT_NAME:=lib_tapi_nlt_cpe

# IMPORTANT - include the version-override here after PKG_VERSION definition
include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=$(TAPI_LIB_NLT_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(TAPI_LIB_NLT_NAME)-$(PKG_VERSION)

PKG_BUILD_DEPENDS:= PACKAGE_ltq-tapi:ltq-tapi PACKAGE_ltq-voice-tapi:ltq-voice-tapi
PKG_SOURCE_URL:=http://localhost

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=application
  CATEGORY:=Lantiq
  TITLE:=TAPI Line Testing library
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=ltq-voice-tapi
  MENU:=1
endef

define Package/$(PKG_NAME)/description
         TAPI Line Testing library
endef

define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
    source "$(SOURCE)/config/Config.in"
endef

# Add user specified CFLAGS
ifneq ($(CONFIG_LTQ_VOICE_LIB_NLT_USER_FLAGS),"")
LIB_NLT_USER_CFLAGS=$(strip $(subst ",, $(CONFIG_LTQ_VOICE_LIB_NLT_USER_FLAGS)))
CONFIGURE_ARGS += --with-cflags="$(LIB_NLT_USER_CFLAGS)"
endif

# Add user specified configure options
ifneq ($(CONFIG_LTQ_VOICE_LIB_NLT_USER_OPTIONS),"")
CONFIGURE_ARGS += $(subst ",, $(CONFIG_LTQ_VOICE_LIB_NLT_USER_OPTIONS))
endif

#MAKE_FLAGS+=-s

ifeq ($(CONFIG_LTQ_VOICE_LIB_NLT_VMMC),y)
CONFIGURE_ARGS += --enable-vmmc
endif

ifeq ($(CONFIG_LTQ_VOICE_LIB_NLT_VINCPE),y)
CONFIGURE_ARGS += --enable-vincpe
endif

ifeq ($(CONFIG_LTQ_VOICE_LIB_NLT_DXT),y)
CONFIGURE_ARGS += --enable-dxt
endif

ifeq ($(CONFIG_LTQ_VOICE_LIB_NLT_IFXOS_SUPPORT),y)
CONFIGURE_ARGS += --enable-ifxos
endif

CONFIGURE_ARGS += \
	--enable-warnings \
	--with-kernel-incl="$(LINUX_DIR)/include" \
	--with-tapi-incl="$(STAGING_DIR)/usr/include/drv_tapi" \
	--with-ifxos-incl="$(STAGING_DIR)/usr/include/ifxos"


ifneq ($(CONFIG_LINUX_2_4),)
CONFIGURE_ARGS += --enable-linux-26 --enable-kernelbuild="$(LINUX_DIR)" ARCH=$(LINUX_KARCH)
endif

define Build/Clean
	@- [ -e $(PKG_BUILD_DIR)/ipkg ] && rm -Rf $(PKG_BUILD_DIR)/ipkg
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/$(TAPI_LIB_NLT_NAME)}
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/$(TAPI_LIB_NLT_NAME)
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/$(TAPI_LIB_NLT_NAME).a $(1)/usr/lib/$(TAPI_LIB_NLT_NAME).a
endef

define Build/UninstallDev
	- rm -Rf $(1)/usr/include/$(TAPI_LIB_NLT_NAME)
	- rm -f $(1)/usr/lib/$(TAPI_LIB_NLT_NAME).a
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
