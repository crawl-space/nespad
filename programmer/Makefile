# Makefile initially writen for Little-Wire by Omer Kilic <omerkilic@gmail.com>
# Later on modified by ihsan Kehribar <ihsan@kehribar.me> for Micronucleus bootloader application. 

CC=gcc

USBFLAGS = `libusb-config --cflags`
USBLIBS = `libusb-config --libs`
OSFLAG = -D LINUX

LIBS    = $(USBLIBS)
INCLUDE = library
CFLAGS  = $(USBFLAGS) $(LIBS) -I$(INCLUDE) -O -g $(OSFLAG)

LWLIBS = micronucleus_lib littleWire_util

.PHONY:	clean library

all: library micronucleus

library: $(LWLIBS)

$(LWLIBS):
	$(CC) $(CFLAGS) -c library/$@.c 

micronucleus: $(addsuffix .o, $(LWLIBS))
	$(CC) $(CFLAGS) -o $@ $@.c $^ $(LIBS)

clean:
	rm -f micronucleus
	rm -f *.o
