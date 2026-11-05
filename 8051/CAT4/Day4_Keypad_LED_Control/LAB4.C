#include <reg52.h>


// COLUMN DECLARATION FOR THE KEYPAD
sbit R1 = P2^7;
sbit R2 = P2^6;
sbit R3 = P2^5;
sbit R4 = P2^4;

// ROW DECLARTATION FOR THE KEYPAD
sbit C1 = P2^3;
sbit C2 = P2^2;
sbit C3 = P2^1;
sbit C4 = P2^0;
	
// DECLARATION OF LEDS
sbit LED_PIN1 = P3^0;
sbit LED_PIN2 = P3^1;
sbit LED_PIN3 = P3^2;
sbit LED_PIN4 = P3^3;

sbit a = P0^0;
sbit b = P0^1;
sbit c = P0^2;
sbit d = P0^3;
sbit e = P0^4;
sbit f = P0^5;
sbit g = P0^6;
sbit dp = P0^7;

void runningLED();
void alternativeLED();
void BinaryCounter();
void blink_all();
void displayDigit(int n);

// DELAY FUNCTION
void delay()
{
    int i , j;
    for(i = 0 ; i < 200 ; i++)
    {
        for(j = 0 ; j < 200 ; j++)
        {}
    }

}


// KEYPAD SCAN

int keypad_scan()
{
    C1 = 0;
    C2 = 1;
    C3 = 1;
    C4 = 1;
    if(R1 == 0)
    {
        return 1;
    }

    C1 = 1;
    C2 = 0;
    C3 = 1;
    C4 = 1;
    if(R1 == 0)
    {
        return 2;
    }

    C1 = 1;
    C2 = 1;
    C3 = 0;
    C4 = 1;
    if(R1 == 0)
    {
        return 3;
    }

    C1 = 0;
    C2 = 1;
    C3 = 1;
    C4 = 1;
    if(R2 == 0)
    {
        return 4;
    }
    C1 = 1;
    C2 = 0;
    C3 = 1;
    C4 = 1;
    if(R2 == 0)
    {
        return 5;
    }

    C1 = 1;
    C2 = 1;
    C3 = 0;
    C4 = 1;
    if(R2 == 0)
    {
        return 6;
    }
    C1 = 0;
    C2 = 1;
    C3 = 1;
    C4 = 1;
    if(R3 == 0)
    {
        return 7;
    }

    C1 = 1;
    C2 = 0;
    C3 = 1;
    C4 = 1;
    if(R3 == 0)
    {
        return 8;
    }

    C1 = 1;
    C2 = 1;
    C3 = 0;
    C4 = 1;
    if(R3 == 0)
    {
        return 9;
    }    
    
    C1 = 1;
    C2 = 0;
    C3 = 1;
    C4 = 1;
    if(R4 == 0)
    {
        return 0;
    }    
    
    return -1;




}



// MODE SELECTOR 

void mode_switch(int mode)
{
    int n = mode;
    displayDigit(n);
    if(n == 1)
    {

				LED_PIN1 = 1;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
    }
    if(n == 2)
    {
        LED_PIN1 = 0;
        LED_PIN2 = 1;
        LED_PIN3 = 0;
        LED_PIN4 = 0;

    }
    if(n == 3)
    {
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 1;
        LED_PIN4 = 0;

    }
    if(n == 4)
    {
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 1;

    }

    if(n == 5)
    {
        runningLED();

    }
    if(n == 6)
    {
        alternativeLED();
    }

    if(n==7)
    {
        BinaryCounter();
    }

    if(n == 8)
    {
        blink_all();
    }

    if(n == 9)
    {
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
    }

    if(n == 0)
    {
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
        }

}
    
// RUNNING LED

    void runningLED()
    {
        LED_PIN1 = 1;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 1;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 1;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 1;
        delay();
    }


    // ALTERNATIVE LED 
    void alternativeLED()
    {
        LED_PIN1 = 1;
        LED_PIN2 = 0;
        LED_PIN3 = 1;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 1;
        LED_PIN3 = 0;
        LED_PIN4 = 1;
        delay();


    }


//  DIGIT IN 7 SEGMENT DISPLAY
void displayDigit(int n)
{
    if(n == 0)
    {
        a = 1;
        b = 1;
        c = 1;
        d = 1;
        e = 1;
        f = 1;
        g = 0;
        dp = 0;
    }

    if(n == 1)
    {
        a = 0;
        b = 1;
        c = 1;
        d = 0;
        e = 0;
        f = 0;
        g = 0;
        dp = 0;
    }

    if(n == 2)
    {
        a = 1;
        b = 1;
        c = 0;
        d = 1;
        e = 1;
        f = 0;
        g = 1;
        dp = 0;
    }

    if(n == 3)
    {
        a = 1;
        b = 1;
        c = 1;
        d = 1;
        e = 0;
        f = 0;
        g = 1;
        dp = 0;
    }

    if(n == 4)
    {
        a = 0;
        b = 1;
        c = 1;
        d = 0;
        e = 0;
        f = 1;
        g = 1;
        dp = 0;
    }

    if(n == 5)
    {
        a = 1;
        b = 0;
        c = 1;
        d = 1;
        e = 0;
        f = 1;
        g = 1;
        dp = 0;
    }

    if(n == 6)
    {
        a = 1;
        b = 0;
        c = 1;
        d = 1;
        e = 1;
        f = 1;
        g = 1;
        dp = 0;
    }

    if(n == 7)
    {
        a = 1;
        b = 1;
        c = 1;
        d = 0;
        e = 0;
        f = 0;
        g = 0;
        dp = 0;
    }

    if(n == 8)
    {
        a = 1;
        b = 1;
        c = 1;
        d = 1;
        e = 1;
        f = 1;
        g = 1;
        dp = 0;
    }

    if(n == 9)
    {
        a = 1;
        b = 1;
        c = 1;
        d = 1;
        e = 0;
        f = 1;
        g = 1;
        dp = 0;
    }
}

    // BINARY COUNTER
    void BinaryCounter()
    {

        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 1;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 1;
        LED_PIN3 = 0;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 1;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 0;
        LED_PIN3 = 1;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 1;
        LED_PIN3 = 1;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 1;
        LED_PIN3 = 1;
        LED_PIN4 = 0;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 1;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 1;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 1;
        LED_PIN3 = 0;
        LED_PIN4 = 1;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 1;
        LED_PIN3 = 0;
        LED_PIN4 = 1;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 1;
        LED_PIN4 = 1;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 0;
        LED_PIN3 = 1;
        LED_PIN4 = 1;
        delay();
        LED_PIN1 = 0;
        LED_PIN2 = 1;
        LED_PIN3 = 1;
        LED_PIN4 = 1;
        delay();
        LED_PIN1 = 1;
        LED_PIN2 = 1;
        LED_PIN3 = 1;
        LED_PIN4 = 1;
        delay();



    }

    // BLINK ALL
    void blink_all()
    {
        LED_PIN1 = 1;
        LED_PIN2 = 1;
        LED_PIN3 = 1;
        LED_PIN4 = 1;
        
    }
		
	

    // MAIN FUNCTION
    void main()
    {
        int mode;
				int key;
				mode = -1;
   
			  key = -1;
				LED_PIN1 = 0;
        LED_PIN2 = 0;
        LED_PIN3 = 0;
        LED_PIN4 = 0;

        while(1)
        {
        

        key = keypad_scan();
        
        if(key >= 0)
        {
            mode = key;
        }
				
				

        if(mode != -1)
        {
            mode_switch(mode);
        }

    }
}