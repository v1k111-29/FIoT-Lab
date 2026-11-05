#include <reg51.h>
sbit led = P1^0;

void delay() {
	int i,j;
	// ~0.5 s at 11.0952 MHz (200 x ~2300 machine cycles) -> 1 Hz blink
	for (i = 0;i < 200;i++) {
		for (j = 0;j<400;j++) {
				}
		}
}

void main() {
			while (1) {
				led = 1;
				delay();
				led = 0;
				delay();
	}
}