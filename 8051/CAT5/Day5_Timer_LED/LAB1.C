#include <reg52.h>

sbit LED_PIN = P1^0;
int count = 0;
void timer_count()
{
	TMOD = 0x01; // CONFIGURATION OF MODE
	TH0 = 0xF3; // HIGHER BIT
	TL0 = 0x66; // LOWER BIT
	
	
		ET0 = 1; // ENABLING TIMER 0 INTERRUPT 
	  EA = 1; // ENABLING GLOBAL INTERRUPT
	  TR0 = 1; // STARTING TIMER 0
		
		TF0 = 0; // CLEAR OVERFLOW INTERRUPT

	  
		while(TF0 == 0) {}
		TR0 = 0;
		count ++;
			
		if(count >= 229)
		{
			count = 0;
			LED_PIN = ~LED_PIN;
			
		}

}

void main()
{
	LED_PIN = 0;

	while(1)
	{
		timer_count();
	}


}