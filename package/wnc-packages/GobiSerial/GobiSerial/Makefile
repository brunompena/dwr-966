obj-m := GobiSerial.o
KDIR := /home/tim/OG4610/New_WNC_Middleware/linux-2.6.32.42
PWD := $(shell pwd)
OUTPUTDIR=/home/tim/OG4610/UGW-5.2/AddOn/lte

all: clean
	$(MAKE) CROSS_COMPILE="mips-openwrt-linux-" ARCH="mips" CONFIG_SHELL="/bin/bash" CC="mips-openwrt-linux-gcc" -C $(KDIR) M=$(PWD) modules

install: all
	mkdir -p $(OUTPUTDIR)
	cp -f GobiSerial.ko $(OUTPUTDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.* modules.order
