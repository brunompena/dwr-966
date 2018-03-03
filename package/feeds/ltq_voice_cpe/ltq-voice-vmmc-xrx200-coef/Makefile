#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=ltq-voice-vmmc-xrx200-coef
PKG_VERSION:=0.9.5
PKG_MD5SUM:=7291417cfceff6ae4285dd4eea38beaa
PKG_BASE_NAME:=coef_voip_vr9

# IMPORTANT - include the version-override here after PKG_VERSION definition
include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_UNPACK:=gzip -dc $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) $(TAR_OPTIONS)
PKG_SOURCE_URL:=http://localhost

VOICE_TARGET_FILENAME_FXS_IBB:=vr9_bbd_fxs.bin
VOICE_TARGET_FILENAME_FXS_CIBB:=vr9_bbd_cmb_fxs.bin
VOICE_TARGET_FILENAME_FXO:=vr9_bbd_fxo.bin
LANTIQ_FIRMWARE_DIR=/lib/firmware

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=firmware
  CATEGORY:=Lantiq
  TITLE:=XRX200 voice coefficient samples
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=ltq-voice-tapi @LTQ_VOICE_VMMC_WITH_DEVICE_XRX200
  MENU:=1
endef

define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
	source "$(SOURCE)/config/Config.in"
endef

# FXS part
ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_ETSI),y)
	FXS_IBB_SRC_FILENAME:=$(PKG_BUILD_DIR)/dcdc_IBB/ETSI_3_10.BIN
	FXS_CIBB_SRC_FILENAME:=$(PKG_BUILD_DIR)/dcdc_CIBB/ETSI_3_10.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_US600R),y)
	FXS_IBB_SRC_FILENAME:=$(PKG_BUILD_DIR)/dcdc_IBB/R600_3_10.BIN
	FXS_CIBB_SRC_FILENAME:=$(PKG_BUILD_DIR)/dcdc_CIBB/R600_3_10.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_GERMANY),y)
	FXS_IBB_SRC_FILENAME:=$(PKG_BUILD_DIR)/dcdc_IBB/GER_3_10.BIN
	FXS_CIBB_SRC_FILENAME:=$(PKG_BUILD_DIR)/dcdc_CIBB/GER_3_10.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_USE_CUSTOM_FILE),y)
	FXS_IBB_SRC_FILENAME:=$(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_CUSTOM_FILE_FXS_IBB)
	FXS_CIBB_SRC_FILENAME:=$(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_CUSTOM_FILE_FXS_CIBB)
endif

# FXO part
ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_ETSI_FXO),y)
	FXO_SRC_FILENAME:=$(PKG_BUILD_DIR)/fxo/FXO_ETSI_0_0.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_US600R_FXO),y)
	FXO_SRC_FILENAME:=$(PKG_BUILD_DIR)/fxo/FXO_R600_0_0.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_GERMANY_FXO),y)
	FXO_SRC_FILENAME:=$(PKG_BUILD_DIR)/fxo/FXO_GER_0_0.BIN
endif

ifeq ($(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_USE_CUSTOM_FILE_FXO),y)
	FXO_SRC_FILENAME:=$(CONFIG_LTQ_VOICE_VMMC_COEF_VR9_CUSTOM_FILE_FXO)
endif

define Package/$(PKG_NAME)/description
	xRX200 voice coefficient samples suitable for the reference schematic.
	Coefficients for different schematics can be calculated with XTCOS.
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/$(LANTIQ_FIRMWARE_DIR)
	# copy FXS BBD file for inverting buck-boost DC/DC
	$(CP) $(FXS_IBB_SRC_FILENAME) $(1)$(LANTIQ_FIRMWARE_DIR)/$(VOICE_TARGET_FILENAME_FXS_IBB)
	# copy FXS BBD file for combined inverting buck-boost DC/DC
	$(CP) $(FXS_CIBB_SRC_FILENAME) $(1)$(LANTIQ_FIRMWARE_DIR)/$(VOICE_TARGET_FILENAME_FXS_CIBB)
	# copy FXO BBD file
	$(CP) $(FXO_SRC_FILENAME) $(1)/$(LANTIQ_FIRMWARE_DIR)/$(VOICE_TARGET_FILENAME_FXO)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
