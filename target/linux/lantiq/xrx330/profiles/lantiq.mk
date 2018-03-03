# Lantiq SoC xRX330 Family/Reference Boards

define Profile/P02003
  NAME:=P02003 Reference Board
  PACKAGES:=
endef
$(eval $(call Profile,P02003))

define Profile/EASY389_BOND
  NAME:=EASY389 BOND Reference Board (600MHz)
  PACKAGES:=
endef

$(eval $(call Profile,EASY389_BOND))

define Profile/EASY300_BOND
  NAME:=EASY300 VDSL BOND Board (600MHz)
  PACKAGES:=
endef

$(eval $(call Profile,EASY300_BOND))

define Profile/EASY330_BOND
  NAME:=EASY330 VDSL BOND Board (720MHz)
  PACKAGES:=
endef

$(eval $(call Profile,EASY330_BOND))
