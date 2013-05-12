/**
 * Project: AVR ATtiny USB Tutorial at http://codeandlife.com/
 * Author: Joonas Pihlajamaa, joonas.pihlajamaa@iki.fi
 * Inspired by V-USB example code by Christian Starkjohann
 * Copyright: (C) 2012 by Joonas Pihlajamaa
 * License: GNU GPL v3 (see License.txt)
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv/usbdrv.h"
#include "main.h"

struct{
  union {
    uint8_t data;
    struct {
      uint8_t X:2;
      uint8_t Y:2;
      uint8_t B:1;
      uint8_t A:1;
      uint8_t SELECT:1;
      uint8_t START:1;
    };
  };
} report_buffer;

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  return 0; // Nothing implemented
}

#define abs(x) ((x) > 0 ? (x) : (-x))

// Called by V-USB after device reset
void hadUsbReset() {
    int frameLength, targetLength = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);
    int bestDeviation = 9999;
    uchar bestCal = 0;
    uchar trialCal;
    uchar step;
    uchar region;

    // do a binary search in regions 0-127 and 128-255 to get optimum OSCCAL
    for(region = 0; region <= 1; region++) {
        frameLength = 0;
        trialCal = (region == 0) ? 0 : 128;

        for(step = 64; step > 0; step >>= 1) {
            if(frameLength < targetLength) // true for initial iteration
                trialCal += step; // frequency too low
            else
                trialCal -= step; // frequency too high

            OSCCAL = trialCal;
            frameLength = usbMeasureFrameLength();

            if(abs(frameLength-targetLength) < bestDeviation) {
                bestCal = trialCal; // new optimum found
                bestDeviation = abs(frameLength -targetLength);
            }
        }
    }

    OSCCAL = bestCal;
}

// Clock pulse width >= 500ns
#define STROBE_CLK() sbi(NES_PORT, CLK); \
                     _delay_us(1); \
                     cbi(NES_PORT, CLK); \
                     _delay_us(1);

int main() {
    uchar i;

    wdt_enable(WDTO_1S); // enable 1s watchdog timer

    usbInit();

    usbDeviceDisconnect(); // enforce re-enumeration
    for(i = 0; i<250; i++) { // wait 500 ms
        wdt_reset(); // keep the watchdog happy
        _delay_ms(2);
    }
    usbDeviceConnect();

    sei(); // Enable interrupts after re-enumeration

    sbi(NES_DDR, CLK);
    cbi(NES_DDR, OUT);
    sbi(NES_DDR, LATCH);

    cbi(NES_PORT, CLK);
    cbi(NES_PORT, LATCH);

    while(1) {
        wdt_reset(); // keep the watchdog happy
        usbPoll();
        if (usbInterruptIsReady()) {
            report_buffer.data = 0x05; // Center pad, little endian

            sbi(NES_PORT, LATCH);
            _delay_us(1); // Latch pulse width >= 500ns
            cbi(NES_PORT, LATCH);
            _delay_us(1); // Propagation time <= 1000ns

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.A = 1;

            STROBE_CLK();

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.B = 1;

            STROBE_CLK();

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.SELECT = 1;

            STROBE_CLK();

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.START = 1;

            STROBE_CLK();

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.Y--;

            STROBE_CLK();

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.Y++;

            STROBE_CLK();

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.X--;

            STROBE_CLK();

            if(bit_is_clear(NES_PIN, OUT))
                report_buffer.X++;

            //called after every poll of the interrupt endpoint
            usbSetInterrupt((uchar *) &report_buffer, sizeof(report_buffer));
        }
    }

    return 0;
}
