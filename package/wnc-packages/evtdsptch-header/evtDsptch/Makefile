# Makefile - build rules for evtDsptch/linux/user
#
# Copyright (c) 2007, TeamF1, Inc.
#
# modification history
# --------------------
# 01a,26Jul07,rks  written.
#
# DESCRIPTION
# This file contains rules to build evtDsptch for linux(user space)
#*/

TGT=evtDsptchd

# these defines are for Linux
LIBS= evtDsptchd.o
ARCH=linux

# for Mac OS X and BSD systems that have getifaddr(), uncomment the next line
#ARCH=bsd_with_getifaddrs

# for early BSD systems without getifaddrs(), uncomment the next line
#ARCH=bsd


# for solaris, uncomment the next two lines
# LIBS=-lsocket -lnsl
# ARCH=sun

all:
	${CC} -Wall ${CFLAGS} -fPIC -c evtDsptchd.c
#	${CC} -Wall ${CFLAGS} -fPIC -c evtDsptchLib.c
	${CC} -Wall ${LDFLAGS} -o evtDsptchd ${LIBS}

clean: 
	rm -f *.o
	rm -f ${TGT}

