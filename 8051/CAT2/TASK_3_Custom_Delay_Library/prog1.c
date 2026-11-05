#include <reg51.h>
#include "delay.h"
sbit led = P1^0;

void main() {
	while (1) {
		led = 1;
		delay_ms(500);
		led = 0;
		delay_ms(500);
	}
}
