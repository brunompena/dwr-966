# Lantiq SoC xRX200 Family Boards

define Profile/EASY80920NOR
  NAME:=EASY80920 VRX288 Family Board, NOR Flash
  PACKAGES:=
endef

$(eval $(call Profile,EASY80920NOR))

define Profile/EASY80920SPI
  NAME:=EASY80920 VRX288 Family Board, SPI Flash
  PACKAGES:=
endef

$(eval $(call Profile,EASY80920SPI))

define Profile/EASY80920NAND
  NAME:=EASY80920 VRX288 Family Board, NAND Flash
  PACKAGES:=
endef

$(eval $(call Profile,EASY80920NAND))

define Profile/EASY4210NAND
  NAME:=EASY4210 GRX288 Family Board, NAND Flash
  PACKAGES:=
endef

$(eval $(call Profile,EASY4210NAND))

define Profile/EASY220
  NAME:=EASY220 Reference Board, SPI Flash
  PACKAGES:=
endef

$(eval $(call Profile,EASY220))
