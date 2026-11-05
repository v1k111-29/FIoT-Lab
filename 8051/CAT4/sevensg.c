#include <reg51.h>
//Buttons
sbit row1 = P2^7;
sbit row2 = P2^6;
sbit row3 = P2^5;
sbit row4 = P2^4;

sbit col1 = P2^3;
sbit col2 = P2^2;
sbit col3 = P2^1;
sbit col4 = P2^0;

//LEDs
sbit led1 = P1^1;
sbit led2 = P1^3;
sbit led3 = P1^5;
sbit led4 = P1^7;

int btn(void)
{
		col1 = 1;
		col2 = 1;
		col3 = 1;
		col4 = 1;
		
		//1st Row
		row1 = 0;
		row2 = 1;
		row3 = 1;
		row4 = 1;
    if (col1 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col1 == 0)     
            return 1;
    }
    if (col2 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col2 == 0)     
            return 2;
    }
    if (col3 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col3 == 0)     
            return 3;
    }
		
		//2nd Row
		row1 = 1;
		row2 = 0;
		row3 = 1;
		row4 = 1;
    if (col1 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col1 == 0)     
            return 4;
    }
    if (col2 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col2 == 0)     
            return 5;
    }
    if (col3 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col3 == 0)     
            return 6;
    }
		
		//3rd Row
		row1 = 1;
		row2 = 1;
		row3 = 0;
		row4 = 1;
    if (col1 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col1 == 0)     
            return 7;
    }
    if (col2 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col2 == 0)     
            return 8;
    }
    if (col3 == 0) {        
        int i;
				for (i = 0; i < 1400; i++);
        if (col3 == 0)     
            return 9;
    }
		
		//4th Row
		row1 = 1;
		row2 = 1;
		row3 = 1;
		row4 = 0;
		if (col2 == 0) {
        int i;
				for (i = 0; i < 1400; i++);
        if (col2 == 0)     
            return 0;
		}
		
		return -1;
}

void glowLed(int led) {
	if (led == 1) {
		led1 = 1;
		led2 = 0;
		led3 = 0;
		led4 = 0;
	} else if (led == 2) {
		led1 = 0;
		led2 = 1;
		led3 = 0;
		led4 = 0;
	} else if (led == 3) {
		led1 = 0;
		led2 = 0;
		led3 = 1;
		led4 = 0;
	} else {
		led1 = 0;
		led2 = 0;
		led3 = 0;
		led4 = 1;
	}
}

void glowPair(int p) {
	if (p == 0) {
		led1 = 1;
		led2 = 0;
		led3 = 1;
		led4 = 0;
	} else {
		led1 = 0;
		led2 = 1;
		led3 = 0;
		led4 = 1;
	}
}

void glowAll(int on) {
	if (on) {
		led1 = 1;
		led2 = 1;
		led3 = 1;
		led4 = 1;
	} else {
		led1 = 0;
		led2 = 0;
		led3 = 0;
		led4 = 0;
	}
}

void glowBinary(int n) {
	led1 = n % 2;
	led2 = (n / 2) % 2;
	led3 = (n / 4) % 2;
	led4 = (n / 8) % 2;
}

void delay(int n) {
	int i,j;
	for (i = 0;i < n;i++) {
		for (j = 0;j < n;j++);
	}
}

void displayNumber(int no) {
	if (no == 0) {
		P0 = 0x3F;
	} else if (no == 1) {
		P0 = 0x6;
	} else if (no == 2) {
		P0 = 0x5B;
	} else if (no == 3) {
		P0 = 0x4F;
	} else if (no == 4) {
		P0 = 0x66;
	} else if (no == 5) {
		P0 = 0x6D;
	} else if (no == 6) {
		P0 = 0x7D;
	} else if (no == 7) {
		P0 = 0x07;
	} else if (no == 8) {
		P0 = 0x7F;
	} else if (no == 9) {
		P0 = 0x6F;
	} else {
		P0 = 0;
	}
}

int main() {
	int c;
	int runningLed = 0;
	int lastLed = 1;
	int altLed = 0;
	int blinkLed = 0;
	int binLed = 0;
	int pat = 0;
	int count = 0;

	led1 = 0;
	led2 = 0;
	led3 = 0;
	led4 = 0;
	displayNumber(-1);
	
	while (1) {
		int no = btn();
		if ((no == 1) || (no == 2) || (no == 3) || (no == 4)) {
			displayNumber(no);
			runningLed = 0;
			altLed = 0;
			blinkLed = 0;
			binLed = 0;
			glowLed(no);
		} else if (no == 5) {
			displayNumber(no);
			runningLed = 1;
			altLed = 0;
			blinkLed = 0;
			binLed = 0;
			lastLed = 1;
			glowLed(1);
		} else if (no == 6) {
			displayNumber(no);
			runningLed = 0;
			altLed = 1;
			blinkLed = 0;
			binLed = 0;
			pat = 0;
			glowPair(0);
		} else if (no == 7) {
			displayNumber(no);
			runningLed = 0;
			altLed = 0;
			blinkLed = 0;
			binLed = 1;
			count = 0;
		} else if (no == 8) {
			displayNumber(no);
			runningLed = 0;
			altLed = 0;
			blinkLed = 1;
			binLed = 0;
			pat = 0;
			glowAll(1);
		} else if (no == 9) {
			displayNumber(no);
			runningLed = 0;
			altLed = 0;
			blinkLed = 0;
			binLed = 0;
			pat = 0;
			glowAll(0);
		} else if (no == 0) {
			//Reset System
			displayNumber(no);
			runningLed = 0;
			altLed = 0;
			blinkLed = 0;
			binLed = 0;
			lastLed = 1;
			pat = 0;
			count = 0;
			glowAll(0);
		}
		
		//Running LED
		if (runningLed) {
			if (lastLed == 1) {
				glowLed(2);
				lastLed = 2;
			} else if (lastLed == 2) {
				glowLed(3);
				lastLed = 3;
			} else if (lastLed == 3) {
				glowLed(4);
				lastLed = 4;
			} else if (lastLed == 4) {
				glowLed(1);
				lastLed = 1;
			}
			delay(100);
		} else if (altLed) {
			if (pat == 0) {
				glowPair(1);
				pat = 1;
			} else {
				glowPair(0);
				pat = 0;
			}
			delay(100);
		} else if (blinkLed) {
			if (pat == 0) {
				glowAll(0);
				pat = 1;
			} else {
				glowAll(1);
				pat = 0;
			}
			delay(100);
		} else if (binLed) {
			glowBinary(count);
			if (count < 15) {
				count = count + 1;
			} else {
				count = 0;
			}
			delay(100);
		}
	}
}