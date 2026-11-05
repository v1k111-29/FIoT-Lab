#include <reg52.h>
 
sbit LED_PIN1 = P1^0;
sbit LED_PIN2 = P1^1;
sbit LED_PIN3 = P1^2;
sbit LED_PIN4 = P1^3;


sbit C1 = P2^3;
sbit C2 = P2^2;
sbit C3 = P2^1;
sbit C4 = P2^0;

sbit R1 = P2^7;
sbit R2 = P2^6;
sbit R3 = P2^5;
sbit R4 = P2^4;


void delay();
void mode_select(int m);
int scanKey();

void main()
{
	LED_PIN1 = 0; 
	LED_PIN2 = 0; 
	LED_PIN3 = 0; 
	LED_PIN4 = 0; 

	
	int mode;
	mode = 0;
	
	while(1)
	{
		
		int mode = scanKey();
		
		mode_select(mode);
		
		
		

	
	
	
	}







}



void delay()
{
	int i , j;
	for(i = 0 ; i < 200 ; i++) {for(j = 0; j < 200 ; j++) {}}

}

int scanKey()
{

	  C1 = 0;
    C2 = 1;
    C3 = 1;
    C4 = 1;

    if(R1 == 0)
    {
        delay();
        while(R1 == 0);
        return 1;
    }

    C1 = 1;
    C2 = 0;
    C3 = 1;
    C4 = 1;

    if(R1 == 0)
    {
        delay();
        while(R1 == 0);
        return 2;
    }

    C1 = 1;
    C2 = 1;
    C3 = 0;
    C4 = 1;

    if(R1 == 0)
    {
        delay();
        while(R1 == 0);
        return 3;
    }

    C1 = 1;
    C2 = 1;
    C3 = 1;
    C4 = 0;

    if(R1 == 0)
    {
        delay();
        while(R1 == 0);
        return 4;
    }

    return 0;
}


void mode_select(int mode)
{
	
		if(mode == 1)
		{
				LED_PIN1 = 1; 
				delay();
				LED_PIN1 = 0; 
				LED_PIN2 = 1; 
				delay();
				LED_PIN2 = 0; 
				LED_PIN3 = 1; 
				delay();
				LED_PIN3 = 0;
				LED_PIN4 = 1;
				delay();
				LED_PIN4 = 0; 
		}

		if(mode == 2)
		{

				LED_PIN1 = 1;
        delay();
	      LED_PIN1 = 0;
				LED_PIN2 = 1;
        delay();
				LED_PIN2 = 0;
        LED_PIN3 = 1;
        delay();
        LED_PIN1 = 1;
				LED_PIN2 = 1;
        LED_PIN3 = 1;
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
				
			 }
			 
			 if(mode == 3)
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

				 if(mode == 4)
        {
         LED_PIN1 = 1;
         LED_PIN2 = 1;
         LED_PIN3 = 1;
  			 LED_PIN4 = 1;
         delay();

         LED_PIN1 = 0;
         LED_PIN2 = 0;
         LED_PIN3 = 0
  			 LED_PIN4 = 0;

         delay();
         LED_PIN1 = 1;
         LED_PIN2 = 1;
         LED_PIN3 = 1;
  			 LED_PIN4 = 1;
         delay();

         LED_PIN1 = 0;
         LED_PIN2 = 0;
         LED_PIN3 = 0
  			 LED_PIN4 = 0;

         delay();
         LED_PIN1 = 1;
         LED_PIN2 = 1;
         LED_PIN3 = 1;
  			 LED_PIN4 = 1;
         delay();

         LED_PIN1 = 0;
         LED_PIN2 = 0;
         LED_PIN3 = 0
  			 LED_PIN4 = 0;

         delay();
         LED_PIN1 = 1;
         LED_PIN2 = 1;
         LED_PIN3 = 1;
  			 LED_PIN4 = 1;
         delay();

         LED_PIN1 = 0;
         LED_PIN2 = 0;
         LED_PIN3 = 0
  			 LED_PIN4 = 0;

         delay();
         LED_PIN1 = 1;
         LED_PIN2 = 1;
         LED_PIN3 = 1;
  			 LED_PIN4 = 1;
         delay();

         LED_PIN1 = 0;
         LED_PIN2 = 0;
         LED_PIN3 = 0
  			 LED_PIN4 = 0;

         delay();
        }
			}
	
	
			