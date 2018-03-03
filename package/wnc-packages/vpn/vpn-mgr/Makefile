DESTDIR =

vpn-mgr: vpn-mgr.o
	$(CC) $(LDFLAGS) -o $@ $<

all: vpn-mgr
	echo 'all'

installdirs:
	install -d -m 0755 ${DESTDIR}/usr/bin

install: installdirs
	install -m 0755 vpn-mgr ${DESTDIR}/usr/bin

