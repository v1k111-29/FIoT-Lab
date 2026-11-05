#include <reg52.h>

sbit LCD_RS = P3^2;
sbit LCD_EN = P3^3;
sbit LCD_D4 = P3^4;
sbit LCD_D5 = P3^5;
sbit LCD_D6 = P3^6;
sbit LCD_D7 = P3^7;

//Keypad
sbit row1 = P2^7;
sbit row2 = P2^6;
sbit row3 = P2^5;
sbit row4 = P2^4;

sbit col1 = P2^3;
sbit col2 = P2^2;
sbit col3 = P2^1;
sbit col4 = P2^0;

void delay_us(unsigned int us) {
		int i,j;
		for (i = 0;i < us;i++) {
			for (j = 0;j < 3;j++);
		}
}

void send_nibble(bit b3, bit b2, bit b1, bit b0) {
    LCD_D7 = b3;
    LCD_D6 = b2;
    LCD_D5 = b1;
    LCD_D4 = b0;
    LCD_EN = 1;
    delay_us(50);
    LCD_EN = 0;
}

//Send a byte as two nibbles, high nibble first
void lcd_write(unsigned char value) {
    send_nibble((value >> 7) & 1, (value >> 6) & 1, (value >> 5) & 1, (value >> 4) & 1);
    send_nibble((value >> 3) & 1, (value >> 2) & 1, (value >> 1) & 1, value & 1);
}

void lcd_cmd(unsigned char cmd) {
    LCD_RS = 0;
    lcd_write(cmd);
		//Clear and Return home take longer than other commands
		if (cmd == 0x01 || cmd == 0x02) {
			delay_us(2000);
		} else {
			delay_us(50);
		}
}

void lcd_data(unsigned char ch) {
    LCD_RS = 1;
    lcd_write(ch);
		delay_us(50);
}

void lcd_init() {
    LCD_RS = 0;
    LCD_EN = 0;
    delay_us(20000);

		//Initialize 4 bit mode
    send_nibble(0,0,1,1);  delay_us(5000);
    send_nibble(0,0,1,1);  delay_us(150);
    send_nibble(0,0,1,1);  delay_us(150);
    send_nibble(0,0,1,0);  delay_us(150);

		//Set 4 bit, 2 lines, 5x8 font
    lcd_cmd(0x28);
		//Display on, Cursor off, Cursor Blink off
    lcd_cmd(0x0C);
		//Cursor moves right, Shift off
    lcd_cmd(0x06);
		//Clear display
    lcd_cmd(0x01);
}

//Row 0 starts at DDRAM 0x00, row 1 at 0x40
void lcd_goto(unsigned char row, unsigned char col) {
		if (row == 0) {
			lcd_cmd(0x80 + col);
		} else {
			lcd_cmd(0xC0 + col);
		}
}

//Returns the debounced key 2, 4, 6 or 8, or -1 if none is pressed
int btn(void)
{
		int i;
		col1 = 1;
		col2 = 1;
		col3 = 1;
		col4 = 1;

		//1st Row: key 2
		row1 = 0;
		row2 = 1;
		row3 = 1;
		row4 = 1;
		if (col2 == 0) {
			for (i = 0; i < 1400; i++);
			if (col2 == 0)
				return 2;
		}

		//2nd Row: keys 4 and 6
		row1 = 1;
		row2 = 0;
		if (col1 == 0) {
			for (i = 0; i < 1400; i++);
			if (col1 == 0)
				return 4;
		}
		if (col3 == 0) {
			for (i = 0; i < 1400; i++);
			if (col3 == 0)
				return 6;
		}

		//3rd Row: key 8
		row2 = 1;
		row3 = 0;
		if (col2 == 0) {
			for (i = 0; i < 1400; i++);
			if (col2 == 0)
				return 8;
		}

		row3 = 1;
		return -1;
}

void main() {
		unsigned char row = 0;
		unsigned char col = 0;
		int key;
		lcd_init();
		lcd_goto(row, col);
		lcd_data('*');

		while(1) {
			key = btn();
			if (key == -1) {
				continue;
			}

			//Erase the character at the old position
			lcd_goto(row, col);
			lcd_data(' ');

			if (key == 2 && row > 0) {
				row--;          //Up
			} else if (key == 8 && row < 1) {
				row++;          //Down
			} else if (key == 4 && col > 0) {
				col--;          //Left
			} else if (key == 6 && col < 15) {
				col++;          //Right
			}

			//Draw the character at the new position
			lcd_goto(row, col);
			lcd_data('*');

			//Wait for the key to be released: one press moves one step
			while (btn() != -1);
		}
}
