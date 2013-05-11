#include <avr/pgmspace.h>

#define NES_DDR DDRB
#define NES_PORT PORTB
#define NES_PIN PINB
#define CLK 4
#define OUT 0
#define LATCH 3

#define sbi(sfr, bit)   ((sfr) |= _BV(bit))
#define cbi(sfr, bit)   ((sfr) &= ~(_BV(bit)))
#define xbi(sfr, bit)   ((sfr) ^= _BV(bit))

const PROGMEM char usbHidReportDescriptor[42] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x02,                    //     LOGICAL_MAXIMUM (2)
    0x75, 0x02,                    //     REPORT_SIZE (2)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x04,                    //   USAGE_MAXIMUM (Button 4)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x04,                    //   REPORT_COUNT (4)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};
