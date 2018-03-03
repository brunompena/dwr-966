AM_CFLAGS = -Wall -Werror -Wundef -Wstrict-prototypes -Wno-trigraphs
AM_CFLAGS += -I./../inc

librmnetctl_la_C = @C@
librmnetctl_la_SOURCES = librmnetctl.c

common_CFLAGS = -static
common_LDFLAGS = -lpthread -lrt

librmnetctl_la_CFLAGS := $(AM_CFLAGS) $(common_CFLAGS)
librmnetctl_la_LDFLAGS := $(common_LDFLAGS)

library_includedir = $(pkgincludedir)
library_include_HEADERS = ./../inc/librmnetctl.h

lib_LTLIBRARIES = librmnetctl.la
