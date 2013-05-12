#define NES_DDR DDRB
#define NES_PORT PORTB
#define NES_PIN PINB
#define CLK 4
#define OUT 0
#define LATCH 3

#define sbi(sfr, bit)   ((sfr) |= _BV(bit))
#define cbi(sfr, bit)   ((sfr) &= ~(_BV(bit)))
#define xbi(sfr, bit)   ((sfr) ^= _BV(bit))

// Clock pulse width >= 500ns
#define STROBE_CLK() sbi(NES_PORT, CLK); \
                     _delay_us(1); \
                     cbi(NES_PORT, CLK); \
                     _delay_us(1);
