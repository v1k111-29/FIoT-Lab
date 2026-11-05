#include <reg52.h>

sbit LCD_RS = P3^2;
sbit LCD_EN = P3^3;
sbit LCD_D4 = P3^4;
sbit LCD_D5 = P3^5;
sbit LCD_D6 = P3^6;
sbit LCD_D7 = P3^7;

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

void main() {
		char *row1 = "HELLO";
		char *row2 = "8051 LCD DRIVER";
		lcd_init();

		//Character 'A' at the first position of the first row
		lcd_cmd(0x80);
		lcd_data('A');

		//String on the first row, after the character
		lcd_cmd(0x82);
		while (*row1 != '\0') {
			lcd_data(*row1);
			row1++;
		}

		//String on the second row
		lcd_cmd(0xC0);
		while (*row2 != '\0') {
			lcd_data(*row2);
			row2++;
		}

		while(1);
}
