#
# Makefile for ppacmd
#

#CFLAGS += -O2 -I$(KERNEL_INCLUDE_PATH)
PPA_CFG_FLAG +=-DCONFIG_LTQ_PPA_IPv6_ENABLE
PPA_CFG_FLAG +=-DCONFIG_LTQ_PPA_QOS
PPA_CFG_FLAG +=-DCONFIG_LTQ_PPA_QOS_WFQ
PPA_CFG_FLAG +=-DCONFIG_LTQ_PPA_QOS_RATE_SHAPING
PPA_CFG_FLAG +=-DCONFIG_LTQ_PPA_API_SW_FASTPATH
ifeq ($(CONFIG_FEATURE_SESSION_LIMIT),y)
PPA_CFG_FLAG +=-DCONFIG_LTQ_PPA_HANDLE_CONNTRACK_SESSIONS
endif

CFLAGS += -O2 $(PPA_CFG_FLAG) -I./

all: ppacmd

ppacmd.o: ppacmd.c
ppacmd_autotest.o: ppacmd_autotest.c

ppacmd: ppacmd_autotest.o ppacmd.o

clean:
	rm -f *.o ppacmd
