#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=ltq-voice-vmmc-xrx100-coef
PKG_VERSION:=0.9.3
PKG_MD5SUM:=bf956fd7ddbd1a45058d75c90984320f
PKG_BASE_NAME:=coef_voip_ar9

# IMPORTANT - include the version-override here after PKG_VERSION definition
include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_UNPACK:=gzip -dc $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) $(TAR_OPTIONS)
PKG_SOURCE_URL:=http://localhost
PKG_TARGET_BASE_NAME:=ar9_bbd_fxs.bin
PKG_TARGET_BASE_NAME_FXO:=ar9_bbd_fxo.bin
LANTIQ_FIRMWARE_DIR=/lib/firmware

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=firmware
  CATEGORY:=Lantiq
  TITLE:=XRX100 voice coefficient samples
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=ltq-voice-tapi @LTQ_VOICE_VMMC_WITH_DEVICE_XRX100
  MENU:=1
endef

define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
	source "$(SOURCE)/config/Config.in"
endef

# FXS part
ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_ETSI),y)
	PKG_ORGINAL_BASE_NAME:=$(PKG_BUILD_DIR)/ETSI_3_10.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_US600R),y)
	PKG_ORGINAL_BASE_NAME:=$(PKG_BUILD_DIR)/R600_3_10.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_GERMANY),y)
	PKG_ORGINAL_BASE_NAME:=$(PKG_BUILD_DIR)/GER_3_10.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_USE_CUSTOM_FILE),y)
	PKG_ORGINAL_BASE_NAME:=$(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_CUSTOM_FILE)
endif

# FXO part
ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_ETSI_FXO),y)
	PKG_ORGINAL_BASE_NAME_FXO:=$(PKG_BUILD_DIR)/FXO_ETSI_0_0.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_US600R_FXO),y)
	PKG_ORGINAL_BASE_NAME_FXO:=$(PKG_BUILD_DIR)/FXO_R600_0_0.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_GERMANY_FXO),y)
	PKG_ORGINAL_BASE_NAME_FXO:=$(PKG_BUILD_DIR)/FXO_GER_0_0.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_USE_CUSTOM_FILE_FXO),y)
	PKG_ORGINAL_BASE_NAME_FXO:=$(CONFIG_LTQ_VOICE_VMMC_XRX100_COEF_CUSTOM_FILE_FXO)
endif

define Package/$(PKG_NAME)/description
	xRX100 voice coefficient samples.
	Coefficients for the XRX100 SOC with XWAY SLIC reference schematic. Coefficients for different systems can be calculated with XTCOS.
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/$(LANTIQ_FIRMWARE_DIR)
	# copy FXS BBD file
	$(CP) $(PKG_ORGINAL_BASE_NAME) $(1)/$(LANTIQ_FIRMWARE_DIR)/$(PKG_TARGET_BASE_NAME)
	# copy FXO BBD file
	$(CP) $(PKG_ORGINAL_BASE_NAME_FXO) $(1)/$(LANTIQ_FIRMWARE_DIR)/$(PKG_TARGET_BASE_NAME_FXO)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
