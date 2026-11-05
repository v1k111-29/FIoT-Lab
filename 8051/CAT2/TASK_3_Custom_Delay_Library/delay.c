#include "delay.h"

// ~1 ms per count: two 153-pass loops of 3 machine cycles each
// (2 x 153 x 3 = 918 cycles, plus loop overhead) x 1.0815 us
void delay_ms(unsigned int ms) {
	unsigned int i;
	unsigned char j;
	for (i = 0;i < ms;i++) {
		for (j = 0;j < 153;j++);
		for (j = 0;j < 153;j++);
	}
}
