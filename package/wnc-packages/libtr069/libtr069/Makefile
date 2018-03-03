
#CC = gcc -g -fPIC -fno-stack-protector
CIFLAGS = $(CFLAGS)
CLFLAGS = ${LDFLAGS}
CDFLAGS = -march=mips32r2 -Os -DWITH_TR098 -DLINUX -DLINUX_WGET -DWITH_COOKIES -DCOPPERGATE -DWITH_OPENSSL -DHAVE_FTP_DOWNLOAD -DWITH_TR143
LIBS = -lm -ldl -lpthread -lssl -lcrypto
OBJS = dmutil.o logger.o platform.o RPCmethod.o RPCmethodImpl.o stdsoap2.o soapServer.o soapClient.o soapC.o httpform.o httpget.o udp.o stun.o tr111.o tr069entry.o ipcutil.o httpda.o md5evp.o ftp.o ftp_ft.o cmds.o http.o
all:$(OBJS)
	$(CC) $(CLFLAGS) -shared -o libtr069.so $(OBJS) $(LIBS)
dmutil.o:
	$(CC) -c dmutil.c $(CIFLAGS) $(CDFLAGS)
logger.o:
	$(CC) -c logger.c $(CIFLAGS) $(CDFLAGS)
platform.o:
	$(CC) -c platform.c $(CIFLAGS) $(CDFLAGS)
RPCmethod.o:
	$(CC) -c RPCmethod.c $(CIFLAGS) $(CDFLAGS)
RPCmethodImpl.o:
	$(CC) -c RPCmethodImpl.c $(CIFLAGS) $(CDFLAGS)
stdsoap2.o:
	$(CC) -c stdsoap2.c $(CIFLAGS) $(CDFLAGS)
soapServer.o:
	$(CC) -c soapServer.c $(CIFLAGS) $(CDFLAGS)
soapClient.o:
	$(CC) -c soapClient.c $(CIFLAGS) $(CDFLAGS)
soapC.o:
	$(CC) -c soapC.c $(CIFLAGS) $(CDFLAGS)
md5evp.o:
	$(CC) -c md5evp.c $(CIFLAGS) $(CDFLAGS)
httpda.o:
	$(CC) -c httpda.c $(CIFLAGS) $(CDFLAGS)
httpform.o:
	$(CC) -c httpform.c $(CIFLAGS) $(CDFLAGS)
httpget.o:
	$(CC) -c httpget.c $(CIFLAGS) $(CDFLAGS)
udp.o:
	$(CC) -c udp.c $(CIFLAGS) $(CDFLAGS)
stun.o:
	$(CC) -c stun.c $(CIFLAGS) $(CDFLAGS)
tr111.o:
	$(CC) -c tr111.c $(CIFLAGS) $(CDFLAGS)
download.o:
	$(CC) -c download.c $(CIFLAGS) $(CDFLAGS)
tr069entry.o:
	$(CC) -c tr069entry.c $(CIFLAGS) $(CDFLAGS)
ipcutil.o:
	$(CC) -c ipcutil.c $(CIFLAGS) $(CDFLAGS)
ftp.o:
	$(CC) -c ./ftp/ftp.c $(CIFLAGS) $(CDFLAGS)
ftp_ft.o:
	$(CC) -c ./ftp/ftp_ft.c $(CIFLAGS) $(CDFLAGS)
cmds.o:
	$(CC) -c ./ftp/cmds.c $(CIFLAGS) $(CDFLAGS)
http.o:
	$(CC) -c ./ftp/http.c $(CIFLAGS) $(CDFLAGS)
clean:
	rm -rf *.o libtr069.so
