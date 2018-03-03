# Makefile - build rules for adaptos/linux
#
# Copyright (c) 2004 - 2008, TeamF1, Inc.
#
# modification history
# --------------------
# 01g,05aug08,rnp  protected TEAMF1_MGMT rules under macro
# 01f,26Dec07,arf  Added adpLog binary creation rule.
# 01e,27oct05,rnp  removed ADP_NET_DEBUG
# 01d,25oct05,sam  removed adpDebug.c from DOC_FILES 
# 01c,13sep05,sam  introduced  ADP_SYS_DEBUG and ADP_NET_DEBUG
# 01b,09sep05,rnp  added adpTaskLib.o, adpSocketIoLib.o, adpMsgQLib.o from k_p
# 01a,01aug04,rks  written.
#
# DESCRIPTION
# This file contains rules to build adaptos library for linux
#
CFLAGS=-Os -DADAPTOS -DADAPTOS_LINUX -I../include/adaptos -I../include/adaptos/linux

OBJS = adpErrnoLib.o adpGetTimeOfDay.o adpLnxIoLib.o adpMsgQLib.o \
       adpSemLib.o adpSocketIoLib.o adpTaskLib.o adpTimerLib.o \
       adpCmdLib.o adpMemLib.o adpShmLib.o adpUtentLib.o

# create adaptos shared library
SHLIBNAME=adaptos


.c.o:
	$(CC) -c -fPIC $(CFLAGS) $(DEBUG) $<

all: $(OBJS)
	
clean:
	rm *.o *.so
