# Makefile - build rules for umi/linux/user
#
# Copyright (c) 2005, TeamF1, Inc.
#
# modification history
# --------------------
# 01b,24sep05,sam  Makefile changes to adapt new shared library rules.
# 01a,21feb05,rks  written.
#
# DESCRIPTION
# This file contains rules to build umi library for linux(user space)
#
#*/


# these defines are for Linux
LIBS= umiLib.o
ARCH=linux

all:
	${CC} -Wall ${CFLAGS} -fPIC -c umiLib.c
	${CC} -Wall ${CFLAGS} -fPIC -c umiProbe.c
	${CC} -shared -Wl,-soname,libumi.so -o libumi.so ${LIBS}

clean: 
	rm -f *.o *.so




