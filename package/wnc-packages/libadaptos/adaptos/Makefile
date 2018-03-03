# Makefile - build rules for adaptos components
#
# Copyright (c) 2001 - 2008, TeamF1, Inc.
#
# modification history
# --------------------
# 01i,05aug08,rnp  removed TOPMAKE protection
# 01h,09aug07,rnp  added TOPMAKE clause
# 01g,27oct05,rnp  removed conditions checking for TGT_OS
# 01f,04sep05,rnp  removed posix directory for vxworks TGT_OS
#                  added support for lynxOS
# 01e,12mar05,uge  added support for linux compilation
# 01d,03jul03,ws   added support for compression scalability
# 01c,10may02,mno  made os-independant
# 01b,14nov01,mno  added LIBNAME for t101
# 01a,13nov01,mno  created.
#
# DESCRIPTION
# This file is the top level Makefile for adaptos
#
#CC=/opt/cnstools/arm-linux-le-uclibc-gcc-4.3.3-2010.05/usr/bin/arm-linux-gcc

ADAPTOS_INC_DIR=./include


LIB_SUFFIX=adaptos

# C flags
 
ADDED_CFLAGS += -I$(ADAPTOS_INC_DIR) -I$(ADAPTOS_INC_DIR)/linux
ADDED_CFLAGS += -DADAPTOS


# ADAPTOS_FLAGS is a variable exported by a parent conf.mk/Makefile. It can be
# used to pass custom options to AdaptOS. The options will be valid only for
# this invocation of make.
ADDED_CFLAGS += $(ADAPTOS_FLAGS)


SUBDIRS += generic
SUBDIRS += linux

all: $(SUBDIRS)
#	${CC} -shared -Wl,-soname,libadaptos.so -o libadaptos.so ./generic/libadaptos_generic.so ./linux/libadaptos_linux.so
	${CC} -shared -Wl,-soname,libadaptos.so -o libadaptos.so ./generic/*.o ./linux/*.o
	
$(SUBDIRS):
	make --directory=generic
	make --directory=linux



clean: 
	make -C linux clean
	make -C generic clean

.PHONY:	$(SUBDIRS)
