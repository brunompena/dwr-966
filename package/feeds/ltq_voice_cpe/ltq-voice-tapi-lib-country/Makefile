#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ltq-voice-tapi-lib-country
PKG_VERSION:=1.0.1.0
PKG_MD5SUM:=b141441082d7859e1f47f62698590705
PKG_RELEASE:=1

TAPI_LIB_COUNTRY_NAME:=lib_tapi_country

# IMPORTANT - include the version-override here after PKG_VERSION definition
include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=$(TAPI_LIB_COUNTRY_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(TAPI_LIB_COUNTRY_NAME)-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=ltq-voice-tapi ifxos \
                   PACKAGE_ltq-voice-vmmc:ifx-voice-cpe-vmmc

PKG_SOURCE_URL:=http://localhost

# Add dependencies on symbols in the configure file
PKG_CONFIG_DEPENDS:=\
        $(foreach _mr,$(shell grep -w config config/Config.in|awk '{ print $$2 }'),CONFIG_$(_mr))

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=application
  CATEGORY:=Lantiq
  TITLE:=TAPI country coeffient profile library
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=ltq-voice-tapi +ltq-voice-vmmc +ifxos
  MENU:=1
endef

define Package/$(PKG_NAME)/description
         TAPI country coeffient profile library
endef

define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
    source "$(SOURCE)/config/Config.in"
endef

ifneq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_USER_FLAGS),"")
LIB_COUNTRY_USER_CFLAGS=$(strip $(subst ",, $(CONFIG_VOICE_TAPI_LIB_COUNTRY_USER_FLAGS)))
CONFIGURE_ARGS += --with-cflags="$(LIB_COUNTRY_USER_CFLAGS)"
endif

ifneq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_USER_OPTIONS),"")
CONFIGURE_ARGS += $(subst ",, $(CONFIG_VOICE_TAPI_LIB_COUNTRY_USER_OPTIONS))
endif

# Force VMMC support
CONFIGURE_ARGS += --with-vmmc-incl=$(STAGING_DIR)/usr/include/drv_vmmc

# VMMC device type
ifeq ($(CONFIG_LTQ_VOICE_VMMC_WITH_DEVICE_XRX100),y)
CONFIGURE_ARGS += --with-vmmc-device=XRX100
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_WITH_DEVICE_XRX200),y)
CONFIGURE_ARGS += --with-vmmc-device=XRX200
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_WITH_DEVICE_XRX300),y)
CONFIGURE_ARGS += --with-vmmc-device=XRX300
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_AU),y)
CONFIGURE_ARGS += --enable-AU
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_AT),y)
CONFIGURE_ARGS += --enable-AT
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_BE),y)
CONFIGURE_ARGS += --enable-BE
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_DK),y)
CONFIGURE_ARGS += --enable-DK
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_FR),y)
CONFIGURE_ARGS += --enable-FR
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_DE),y)
CONFIGURE_ARGS += --enable-DE
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_IT),y)
CONFIGURE_ARGS += --enable-IT
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_NL),y)
CONFIGURE_ARGS += --enable-NL
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_NO),y)
CONFIGURE_ARGS += --enable-NO
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_PL),y)
CONFIGURE_ARGS += --enable-PL
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_ES),y)
CONFIGURE_ARGS += --enable-ES
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_SE),y)
CONFIGURE_ARGS += --enable-SE
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_CH),y)
CONFIGURE_ARGS += --enable-CH
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_GB),y)
CONFIGURE_ARGS += --enable-GB
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_AE),y)
CONFIGURE_ARGS += --enable-AE
endif

ifeq ($(CONFIG_VOICE_TAPI_LIB_COUNTRY_US),y)
CONFIGURE_ARGS += --enable-US
endif

CONFIGURE_ARGS += \
	--enable-warnings \
	--with-kernel-incl="$(LINUX_DIR)/include" \
	--with-tapi-incl="$(STAGING_DIR)/usr/include/drv_tapi" \
	--with-ifxos-incl="$(STAGING_DIR)/usr/include/ifxos"


define Build/Configure
	rm -f $(PKG_BUILD_DIR)/.configured*
	$(call Build/Configure/Default)
endef

define Build/Clean
	@- [ -e $(PKG_BUILD_DIR)/ipkg ] && rm -Rf $(PKG_BUILD_DIR)/ipkg
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/$(TAPI_LIB_COUNTRY_NAME)}
	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/$(TAPI_LIB_COUNTRY_NAME)
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/$(TAPI_LIB_COUNTRY_NAME).a $(1)/usr/lib/$(TAPI_LIB_COUNTRY_NAME).a
endef

define Build/UninstallDev
	- rm -Rf $(1)/usr/include/$(TAPI_LIB_COUNTRY_NAME)
	- rm -f $(1)/usr/lib/$(TAPI_LIB_COUNTRY_NAME).a
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
