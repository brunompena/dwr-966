# Makefile - build rules for adaptos/vxWorks
#
# Copyright (c) 2005 - 2008, TeamF1, Inc.
#
# modification history
# --------------------
# 01j,26jan10,sam  moved adpModDebug.o to logging component. 
# 01i,12aug08,rnp  added adpLogLib.o and moved adpModDebug.o under TEAMF1_MGMT
# 01h,02jan08,rnp  removed adpSupportLib.o
# 01g,07dec07,rnp  added adpPasswd.o
# 01f,07oct07,san  added adpBuffIoLib.o 
# 01e,24jul07,rnp  added adpDaemonize.o
#                  added adpMemTrkLib.o
# 01d,12jul07,rnp  added adpU64Struct.o
# 01c,25oct05,sam  added adpModDebug.c to DOC_FILES  
# 01b,04sep05,rnp  removed adpNetLib.o and adpStrTokR.o
#                  added adpSupportLib.o
# 01a,11mar05,uge  written.
#
# DESCRIPTION
# This file contains rules to build adaptos library for VxWorks
#
CFLAGS=-Os -DADAPTOS -DADAPTOS_LINUX -I../include/adaptos -I../include/adaptos/linux

OBJS= adpCryptoApi.o        \
      adpEngineApi.o        \
      adpGetProtoByName.o   \
      adpInetAton.o         \
      adpInetNtoa.o         \
      adpIoLib.o            \
      adpListLib.o          \
      adpRandLib.o          \
      adpRealPath.o         \
      adpStrCaseCmp.o       \
      adpU64Struct.o        \
      adpDaemonize.o        \
      adpMemTrkLib.o        \
      adpPasswd.o	    \
      adpLogLib.o
  	
ifneq ($(TGT_CPU),arm)
OBJS += adpSnPrintf.o
endif

ifeq ($(CONFIG_BUFFER_IO),y)
OBJS += adpBuffIoLib.o \
        adpBufferFileInit.o  
endif

.c.o:
	$(CC) -c -fPIC $(CFLAGS) $(DEBUG) $<

all: $(OBJS)
	
clean:
	rm *.o *.so
