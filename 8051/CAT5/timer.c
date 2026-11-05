#include <reg51.h>

int count = 0;
sbit led = P1^1;

void initTimer() {
	TH0 = 0xF1;
	TL0 = 0x99;
	TF0 = 0;
}

int main() {
	TR0 =1;
	TMOD = 0x01;
	initTimer();
	led = 1;
	while (1) {
		if (TF0 == 1) {
			count++;
			initTimer();
		}
		if (count == 200) {
			led = !led;
			count = 0;
		}
	}
}