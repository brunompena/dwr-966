DESTDIR =


all: ipv6_prefix ipv6_reformat create_tayga_conf
	echo 'all'

ipv6_reformat: ipv6_reformat.o
	$(CC) $(LDFLAGS) -o $@ $<

ipv6_prefix: ipv6_prefix.o
	$(CC) $(LDFLAGS) -o $@ $<

create_tayga_conf: create_tayga_conf.o
	$(CC) $(LDFLAGS) -o $@ $<

installdirs:
	install -d -m 0755 ${DESTDIR}/usr/bin

install: installdirs
	install -m 0755 ipv6_prefix ${DESTDIR}/usr/bin
	install -m 0755 ipv6_reformat ${DESTDIR}/usr/bin
	install -m 0755 create_tayga_conf ${DESTDIR}/usr/bin

