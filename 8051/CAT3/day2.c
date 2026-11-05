#include <reg51.h>

sbit row = P2^0;
sbit col = P2^5;


sbit led1 = P1^1;
sbit led2 = P1^3;
sbit led3 = P1^5;
sbit led4 = P1^7;

bit btn(void)
{
    if (row == 0) {
        int i;
        for (i = 0; i < 1400; i++);
        if (row == 0)
            return 1;
    }
    return 0;
}

void wait(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 120; j++);
}

void main(void)
{
    unsigned char mode = 0;
    unsigned char pos = 1;
    unsigned char count = 0;
    unsigned char alt = 0;
    unsigned char flash = 0;

    col = 0;
    row = 1;

    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;

    while (1) {
        if (btn()) {
            if (mode < 4)
                mode = mode + 1;
            else
                mode = 1;

            pos = 1;
            count = 0;
            alt = 0;
            flash = 0;
        }

        if (mode == 0) {
            led1 = 0;
            led2 = 0;
            led3 = 0;
            led4 = 0;
            wait(20);
        }

        if (mode == 1) {
            led1 = 0;
            led2 = 0;
            led3 = 0;
            led4 = 0;

            if (pos == 1) led1 = 1;
            if (pos == 2) led2 = 1;
            if (pos == 3) led3 = 1;
            if (pos == 4) led4 = 1;

            if (pos < 4)
                pos = pos + 1;
            else
                pos = 1;

            wait(300);
        }

        if (mode == 2) {
            led1 = count % 2;
            led2 = (count / 2) % 2;
            led3 = (count / 4) % 2;
            led4 = (count / 8) % 2;

            if (count < 15)
                count = count + 1;
            else
                count = 0;

            wait(300);
        }

        if (mode == 3) {
            if (alt == 0) {
                led1 = 1;
                led2 = 0;
                led3 = 1;
                led4 = 0;
                alt = 1;
            } else {
                led1 = 0;
                led2 = 1;
                led3 = 0;
                led4 = 1;
                alt = 0;
            }
            wait(300);
        }

        if (mode == 4) {
            if (flash == 0) {
                led1 = 1;
                led2 = 1;
                led3 = 1;
                led4 = 1;
                flash = 1;
            } else {
                led1 = 0;
                led2 = 0;
                led3 = 0;
                led4 = 0;
                flash = 0;
            }
            wait(300);
        }
    }
}