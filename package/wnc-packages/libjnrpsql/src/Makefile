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
LIBS= jnrp_sql_lib.o
ARCH=linux

all:
	${CC} -Wall -Os ${CFLAGS} -fPIC -c jnrp_sql_lib.c
	${CC} -shared -Wl,-soname,libjnrpsql.so -o libjnrpsql.so ${LIBS}


strip:
	if [ -e libjnrpsql.so ]; then true; else exit 1; fi
	$(STRIP) libjnrpsql.so

install:
	if [ -d $(TOPDIR)/include/jnrpsql ]; then true; else mkdir $(TOPDIR)/include/jnrpsql; fi
	cp jnrp_sql_lib.h $(TOPDIR)/include/jnrpsql
	if [ -e libjnrpsql.so ]; then true; else exit 1; fi
	cp libjnrpsql.so ${LIBDIR}


clean:
	rm -f *.o *.so
