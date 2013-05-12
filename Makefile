F_CPU = 16500000
DEVICE = attiny85
FUSEOPT = $(FUSEOPT_t85)
LOCKOPT = -U lock:w:0x2f:m

# hexadecimal address for bootloader section to begin. To calculate the best value:
# - make clean; make main.hex; ### output will list data: 2124 (or something like that)
# - for the size of your device (8kb = 1024 * 8 = 8192) subtract above value 2124... = 6068
# - How many pages in is that? 6068 / 64 (tiny85 page size in bytes) = 94.8125
# - round that down to 94 - our new bootloader address is 94 * 64 = 6016, in hex = 1780
BOOTLOADER_ADDRESS = 1780

# PROGRAMMER contains AVRDUDE options to address your programmer
# This is set for using a TI launchpad programmed as an avr910
PROGRAMMER = -c avr910 -b 9600 -P /dev/ttyACM0

FUSEOPT_t85 = -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m
FUSEOPT_t85_DISABLERESET = -U lfuse:w:0xe1:m -U efuse:w:0xfe:m -U hfuse:w:0x5d:m

# Tools:
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
CC = avr-gcc

# Options:
DEFINES = -DBOOTLOADER_ADDRESS=0x$(BOOTLOADER_ADDRESS) #-DDEBUG_LEVEL=2
# Remove the -fno-* options when you use gcc 3, it does not understand them
CFLAGS = -Wall -Os -fno-move-loop-invariants -fno-tree-scev-cprop \
	 -fno-inline-small-functions -I. -Iosccal -mmcu=$(DEVICE) \
	 -DF_CPU=$(F_CPU) $(DEFINES)
LDFLAGS = -Wl,--relax,--gc-sections \
	  -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS),-Map=bootloader.map

OSCCAL_OBJECTS = osccal/osccal.o
VUSB_OBJECTS =  usbdrv/usbdrvasm.o usbdrv/oddebug.o
BOOTLOADER_OBJECTS = bootloader/main.o $(OSCCAL_OBJECTS) $(VUSB_OBJECTS)
FIRMWARE_OBJECTS = firmware/main.o $(OSCCAL_OBJECTS) $(VUSB_OBJECTS)

# symbolic targets:
all: bootloader.hex firmware.hex

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ -Wa,-ahls=$<.lst

.S.o:
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(CC) $(CFLAGS) -S $< -o $@

flash: bootloader.hex 
	$(AVRDUDE) -U flash:w:bootloader.hex:i

readflash:
	$(AVRDUDE) -U flash:r:read.hex:i

fuse:
	$(AVRDUDE) $(FUSEOPT)
	
disablereset:
	$(AVRDUDE) $(FUSEOPT_t85_DISABLERESET)

lock:
	$(AVRDUDE) $(LOCKOPT)

read_fuses:
	$(UISP) --rd_fuses

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(CC) $(CFLAGS) -E main.c

# Main targets

clean:
	rm -f *.map
	rm -f *.hex
	rm -f *.bin
	rm -f $(BOOTLOADER_OBJECTS)
	rm -f $(FIRMWARE_OBJECTS)
	rm -f osccal/*.lst
	rm -f usbdrv/*.lst
	rm -f bootloader/*.lst
	rm -f firmware/*.lst

%.hex: %.bin
	rm -f $@ $<.eep.hex
	avr-objcopy -j .text -j .data -O ihex $? $@
	avr-size $@

bootloader.bin: $(BOOTLOADER_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $?

firmware.bin: $(FIRMWARE_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $?
