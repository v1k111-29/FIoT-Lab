#ifndef DELAY_H
#define DELAY_H

// Software delay library for the AT89S52 at 11.0952 MHz
// (1 machine cycle = 12 / 11.0952 MHz = 1.0815 us)
void delay_ms(unsigned int ms);

#endif
