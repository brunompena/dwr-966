# Makefile - build rules for adaptos/integrity
#
# Copyright (c) 2008, TeamF1, Inc.
#
# modification history
# --------------------
# 01b,27jun08,rnp  changes to use common rules to generate adpFiles.c
#                  added adpIntRamFs.o
# 01a,09apr08,san  written.
#
# DESCRIPTION
# This file contains rules to build adaptos library for integrity
#

include ../make/conf.mk

OBJS = adpErrnoLib.o adpGetTimeOfDay.o adpIntIoLib.o   \
  adpSocketIoLib.o adpSemLib.o adpTaskLib.o adpTimerLib.o \
  adpCmdLib.o adpMemLib.o  adpNetLib.o adpSysLog.o adpIntRamFs.o
  
OBJS += adpFiles.o
# create C file from sample configuration file required by kerberos 
TF1_B2C_FILES = $(TEAMF1_ADAPTOS_BASE)/src/adaptos/integrity/hosts
TF1_B2C_SRC_GEN = adpFiles.c

DOC_FILES= adpNetDbgMon.c

# to enable log levels
ifneq (,$(findstring DADP_SYS_DEBUG, $(ADAPTOS_FLAGS)))
BINARY = adpDebug
ADDED_LDFLAGS += -L$(TEAMF1_UMI_BASE)/lib         \
                 -l$(CPU)$(TOOL)umi$(OS_SUFFIX)
endif

ADDED_LDFLAGS+=                                   \
              -L$(TEAMF1_ADAPTOS_BASE)/lib        \
              -l$(CPU)$(TOOL)adaptos$(OS_SUFFIX)

include $(TEAMF1_ADAPTOS_BASE)/src/adaptos/make/lib-rules.mk

DOC_BOOK=Adaptos_User_Guide
DOC_CHAPTER=Adaptos_Config_Routines
DOC_DIR=$(TEAMF1_ADAPTOS_BASE)/docs/adaptos/

