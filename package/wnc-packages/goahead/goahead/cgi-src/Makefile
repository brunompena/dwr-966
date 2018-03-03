#
# Makefile for the GoAhead web server reference source base
#  for the uClinux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.29 2009-12-14 03:45:48 winfred Exp $
#


#CONF	= $(ROOTDIR)/$(LINUXDIR)
#ZLIB_H  = $(ROOTDIR)/lib/zlib
#CONF_H  = $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h

#
#  As you can see, RRDTool(traffic graphic) is obsoleted.
#
UPLOAD_CGI = upload.cgi 
#UPLOAD_BOOTLOADER_CGI = upload_bootloader.cgi
#UPLOAD_SETTINGS = upload_settings.cgi
#USB_UPGRADE_CGI = usb_upgrade.cgi
#UPLOAD_CA_CLIENT_CERT_CGI = upload_caclcert.cgi
#UPLOAD_KEY_CERT_CGI = upload_keycert.cgi
#UPLOAD_WAPI_AS_CERT_CGI = upload_wapi_as_cert.cgi
#UPLOAD_WAPI_USER_CERT_CGI = upload_wapi_user_cert.cgi

#RRDTOOL_CGIWARPPER = rrdtool_cgiwrapper.cgi
#CPU_MEM_RRD_UPDATER	= cpu_mem_updater
#NETWORK_RRD_UPDATER	= network_updater

CFLAGS:= -Os $(CFLAGS)
ALL_EXE = $(UPLOAD_CGI) gui.cgi uploadMD.cgi
#ifeq ("$(CONFIG_USB_STORAGE)", "y")
#ifeq ("$(CONFIG_USER_STORAGE)", "y")
#ALL_EXE += $(USB_UPGRADE_CGI)
#endif
#endif
#ifeq ("$(CONFIG_USER_WPA_SUPPLICANT)", "y")
#ifeq ("$(CONFIG_RT2860V2_STA_WPA_SUPPLICANT)", "y")
#ALL_EXE += $(UPLOAD_CA_CLIENT_CERT_CGI)
#ALL_EXE += $(UPLOAD_KEY_CERT_CGI)
#endif
#endif
#ifeq ("$(CONFIG_RALINKAPP_WAPI)", "y")
#ifeq ("$(CONFIG_RT2860V2_AP_WAPI)", "y")
#ALL_EXE += $(UPLOAD_WAPI_AS_CERT_CGI)
#ALL_EXE += $(UPLOAD_WAPI_USER_CERT_CGI)
#endif
#endif

all:	$(ALL_EXE)

$(UPLOAD_CGI): upload.cgi.o
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT upload.cgi.o -o $@
	#$(STRIP) --remove-section=.note --remove-section=.comment $@
	
gui.cgi : cgiMain.o
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT cgiMain.o -o $@
	#$(STRIP) --remove-section=.note --remove-section=.comment $@

uploadMD.cgi: uploadMD.cgi.o
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT uploadMD.cgi.o -o $@
	#$(STRIP) --remove-section=.note --remove-section=.comment $@

#$(UPLOAD_BOOTLOADER_CGI): upload_bootloader.cgi.o
#	$(CC) $(CFLAGS) upload_bootloader.cgi.o -o $@
#	$(STRIP) --remove-section=.note --remove-section=.comment $@

#$(UPLOAD_SETTINGS): upload_settings.cgi.o
#	$(CC) $(CFLAGS) upload_settings.cgi.o -o $@
#	$(STRIP) --remove-section=.note --remove-section=.comment $@

#$(USB_UPGRADE_CGI): usb_upgrade.cgi.o crc32.o
#	$(CC) $(CFLAGS) crc32.o usb_upgrade.cgi.o -o $@
#	$(STRIP) --remove-section=.note --remove-section=.comment $@

#$(UPLOAD_CA_CLIENT_CERT_CGI): upload_caclcert.cgi.o crc32.o
#	$(CC) $(CFLAGS) crc32.o upload_caclcert.cgi.o -o $@
#	$(STRIP) --remove-section=.note --remove-section=.comment $@

#$(UPLOAD_KEY_CERT_CGI): upload_keycert.cgi.o crc32.o
#	$(CC) $(CFLAGS) crc32.o upload_keycert.cgi.o -o $@
#	$(STRIP) --remove-section=.note --remove-section=.comment $@
#$(RRDTOOL_CGIWARPPER): rrdtool_cgiwrapper.o
#	$(CC) -o $@	rrdtool_cgiwrapper.o
#	$(STRIP) --remove-section=.note --remove-section=.comment $@
#
#$(CPU_MEM_RRD_UPDATER): cpu_mem_updater.o
#	$(CC) -o $@	cpu_mem_updater.o
#	$(STRIP) --remove-section=.note --remove-section=.comment $@
#
#$(NETWORK_RRD_UPDATER): network_updater.o
#	$(CC) -o $@	network_updater.o
#	$(STRIP) --remove-section=.note --remove-section=.comment $@

clean:
	#rm -f $(ALL_EXE) $(ARCH) $(USB_UPGRADE_CGI) $(UPLOAD_CA_CLIENT_CERT_CGI) $(UPLOAD_KEY_CERT_CGI) $(UPLOAD_WAPI_AS_CERT_CGI) $(UPLOAD_WAPI_USER_CERT_CGI) *.o
	rm -f $(ALL_EXE) $(ARCH) *.o

#
#	Dependencies
#
upload.cgi.o: upload.cgi.c $(CONF_H)
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT -c upload.cgi.c -o $@

cgiMain.o: cgiMain.c $(CONF_H)
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT -c cgiMain.c -o $@

uploadMD.cgi.o: uploadMD.cgi.c $(CONF_H)
	$(CC) $(CFLAGS) -DUPLOAD_FIRMWARE_SUPPORT -c uploadMD.cgi.c -o $@
