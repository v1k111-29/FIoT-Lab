#include <reg52.h>

void initUART() {
	SCON = 0x50;
	
	TMOD = 0x20;
	TH1 = 0xFD;
	TL1 = 0xFD;
	TR1 = 1;
}

void sendChar(char c) {
	SBUF = c;
	
	while (TI == 0);
	TI = 0;
}

void sendString(char* st) {
	while (*st != '\0') {
		sendChar(*st);
		st++;
	}
	sendChar('\r');
	sendChar('\n');
}

void delay() {
	int i,j;
	for (i = 0;i < 200;i++) {
		for (j = 0;j < 400;j++);
	}
}

int main() {
	initUART();
	while (1) {
		sendString("Hello from 8051");
		delay();
	}
}
