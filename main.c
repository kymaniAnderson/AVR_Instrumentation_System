/*------------------------------------------
---------------- Version: 1 ---------------- 
--------------------------------------------
------------------ Author ------------------
------------- Kymani Anderson --------------
--------------------------------------------
------------------------------------------*/

/*
	Todo:
	1. Voltage in a DC Circuit 					❌
	2. Capacitance 								❌
	3. Resistance								❌
	4. Temparature								✅
	5. Frequency of Square Wave (DC Signal)		❌
	6. Inductance								❌
	7. Distance Measurement						❌
	8. Display Modes w/ Button			 		✅
	9. Display on LCD					 		✅
*/

/*
	ADMUX sets the reference voltage as well as the pin we are using
*/

#define temp_pin 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include <util/delay.h>
#include <stdlib.h>

void check_mode();
void io_setup();
void adc_setup();
float adc_read(char channel);

int mode = 0;

int main() 
{	
	int temp, Vout;
	char str[20];

	io_setup();
	lcd_init();
	adc_setup();
	
	while (1)
	{
		switch (mode)
        {
            case 0:
				lcd_gotoxy(0,0);
				lcd_print("Voltmeter Mode");
            break;
            case 1:
				lcd_gotoxy(0,0);
				lcd_print("Capacitance Mode");
            break;
            case 2:
				lcd_gotoxy(0,0);
				lcd_print("Resistance Mode");
            break;
			 case 3:
				lcd_gotoxy(0,0);
				lcd_print("Temperature");
				
				Vout = adc_read(temp_pin);	
				temp = Vout / 10;
				
				ftoa(temp,str,10);
				
				lcd_gotoxy(0,1);
				lcd_print(str);
            break;
            case 4:
				lcd_gotoxy(0,0);
				lcd_print("Frequency Mode");
            break;
            case 5:
				lcd_gotoxy(0,0);
				lcd_print("Inductance Mode");
            break;
			case 6:
				lcd_gotoxy(0,0);
				lcd_print("Distance Mode");
            break;
        }
        
		check_mode();
	}
}

void check_mode()
{    
    if (bit_is_clear(PIND,2))
    {
        mode++;

        if(mode > 6)
        {
            mode = 0;
        }
 
        while(bit_is_clear(PIND,2))
        {
            _delay_ms(100);
        }
    }
}

void io_setup()
{
	/*
		Setup:
		1. PA1 (ADC1) -> INPUT 
		2. PORTC (LCD) -> OUTPUT
		3. PD2 (BUTTON) -> INPUT
	*/	
	
	DDRA &= ~(1 << PINA1);
	DDRC = 0xFF;
	DDRD &= ~(1 << PIND2);
}

void adc_setup()
{
	ADMUX |= (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS0) | 1 << ADPS1 ) | 1 << ADPS2);
}

float adc_read(char channel)
{
	int Vout;
	
	ADMUX |= (channel & 0x0f);
	
	ADSCRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADIF)) == 0);
	
	Vout = ADC * (5/1024);
	
	return Vout;	
}

