# conf.mk - build configuration parameters for adaptos
#
# Copyright (c) 2002 - 2008, TeamF1, Inc.
#
# modification history
# --------------------
# 01i,26jan10,sam  removed references to umi, lua and logging. 
# 01h,05aug08,rnp  cleaned up the file so that it can use global configuration
# 01g,28jan08,gnm  added definitions for LUA macros
# 01f,09aug07,rnp  added TOPMAKE clause
# 01e,01sep05,rnp  removed ADAPTOS_TGTOS specification
#                  added ADAPTOS_FLAGS                 
# 01d,11mar05,uge  added TGT_DIR and support for OS 
#                  independent compilation
# 01c,20juk04,rks  changes to move adaptos from SSECURE
# 01b,23jul03,ws   renamed SSECURE_MAKE_DIR to BASE_MAKE_DIR.
# 01a,05jun02,mno  written,
#
# DESCRIPTION 
# 
# This file contains configuration to build adaptos library.
#

include $(TEAMF1_BASE)/make/conf.mk

ifeq ($(ADAPTOS_INC_DIR),)
ADAPTOS_INC_DIR=$(TEAMF1_ADAPTOS_BASE)/h/adaptos
endif

LIB_SUFFIX=adaptos

# C flags
 
ADDED_CFLAGS += -I$(ADAPTOS_INC_DIR) -I$(ADAPTOS_INC_DIR)/$(TGT_OS)
ADDED_CFLAGS += -DADAPTOS

ADAPTOS_DEBUG_FLAGS= -g

ifeq ($(DEBUG),y)
ADDED_CFLAGS += $(ADAPTOS_DEBUG_FLAGS) 
endif

# ADAPTOS_FLAGS is a variable exported by a parent conf.mk/Makefile. It can be
# used to pass custom options to AdaptOS. The options will be valid only for
# this invocation of make.
ADDED_CFLAGS += $(ADAPTOS_FLAGS)
