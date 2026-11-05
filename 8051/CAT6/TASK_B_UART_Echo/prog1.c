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

char getChar() {
	char c;
	while (RI == 0);
	c = SBUF;
	RI = 0;
	return c;
}

void sendString(char* st) {
	while (*st != '\0') {
		sendChar(*st);
		st++;
	}
	sendChar('\r');
	sendChar('\n');
}

int main() {
	initUART();
	sendString("Hello from 8051!!!");
	while (1) {
		char rec;
		rec = getChar();
		sendChar(rec);
	}
}
