# Makefile - rules for compiling kernel modules.
#
# Copyright (c) 2005 - 2007, TeamF1, Inc.
#
# modification history
# --------------------
# 01c,18oct07,rnp  simplied the Makefile by using kernel rules from lib-rules.mk
# 01b,20oct05,sam  revised to add support for 2.4.x  
# 01a,17mar05,sam  written 
#
# DESCRIPTION
# This file defines make rules for UMI.
#
#
# Include UMI specific configuration rules.
# 

# kernel modules to build
obj-m   += umi.o

umi-objs += af_umi.o umiLib.o

SRC := $(shell pwd)

#EXTRA_CFLAGS = "-I. -DADAPTOS -DADAPTOS_LINUX -I/home/andy/wnc/CM_prj/cavium/include -I/home/andy/wnc/CM_prj/cavium/include/umi -I/home/andy/wnc/CM_prj/cavium/include/adaptos"

ifeq ($(DEBUG),y)
EXTRA_CFLAGS += -g -DDEBUG -DDEBUG2
endif

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
