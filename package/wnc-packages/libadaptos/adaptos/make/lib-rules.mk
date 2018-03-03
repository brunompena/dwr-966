# lib-rules.mk - generic library rules
#
# Copyright (c) 2002 - 2008, TeamF1, Inc.
#
# modification history
# --------------------
# 01b,05aug08,rnp  PRODUCT_BASE is now needed only for TEAMF1_MGMT
# 01a,13aug02,tvg  written 
#
# DESCRIPTION
# This file contains generic library rules
#

ifeq ($(TEAMF1_MGMT),y)
PRODUCT_BASE=$(TEAMF1_ADAPTOS_BASE)
endif

include $(TEAMF1_MAKE_BASE)/$(TGT_OS)/lib-rules.mk
