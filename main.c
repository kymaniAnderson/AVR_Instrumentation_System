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

/*======= 8MHz External Crystal Attached =======*/
#ifndef F_CPU
#define F_CPU 8000000UL 
#endif

/*======= LCD Data Ports =======*/
#define D4 eS_PORTC4
#define D5 eS_PORTC5
#define D6 eS_PORTC6
#define D7 eS_PORTC7

/*======= Register-Select and Enable =======*/
#define RS eS_PORTC1
#define EN eS_PORTC3

#define temp_pin 1

/*======= List of Libraries =======*/
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

/*======= Functions =======*/
void check_mode();
void io_setup();
void adc_setup();
void start_conversion();
float adc_read(char channel);

/*======= Global Variables =======*/
int mode = 0;

int main() 
{	
	int temp, Vout;
	char str[20];

	io_setup();
	Lcd4_Init();
	adc_setup();
	
	while (1)
	{
		switch (mode)
        {
            case 0:
				_delay_ms(50);
				Lcd4_Clear();

				Lcd4_Set_Cursor(1,1);
				Lcd4_Write_String("Voltmeter Mode");
            break;
            case 1:
				_delay_ms(50);
				Lcd4_Clear();

				Lcd4_Set_Cursor(1,1);
				Lcd4_Write_String("Capacitance Mode");
            break;
            case 2:
				_delay_ms(50);
				Lcd4_Clear();

				Lcd4_Set_Cursor(1,1);
				Lcd4_Write_String("Resistance Mode");
            break;
			 case 3:
				_delay_ms(50);
				Lcd4_Clear();

				Lcd4_Set_Cursor(1,1);
				Lcd4_Write_String("Temperature");
				
				Vout = adc_read(temp_pin);	
				temp = Vout / 10;

				dtostrf(temp,6, 2, str);
				Lcd4_Set_Cursor(2,1);
				Lcd4_Write_String(str);
            break;
            case 4:
				_delay_ms(50);
				Lcd4_Clear();

				Lcd4_Set_Cursor(1,1);
				Lcd4_Write_String("Frequency Mode");
            break;
            case 5:
				_delay_ms(50);
				Lcd4_Clear();

				Lcd4_Set_Cursor(1,1);
				Lcd4_Write_String("Inductance Mode");
            break;
			case 6:
				_delay_ms(50);
				Lcd4_Clear();

				Lcd4_Set_Cursor(1,1);
				Lcd4_Write_String("Distance Mode");
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
	ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1 ) | (1 << ADPS2); 
	start_conversion();
}

void start_conversion()
{
	ADCSRA |= (1 << ADSC);
}

float adc_read(char channel)
{
	int Vout;
	
	ADMUX |= (channel & 0xFF);
	
	start_conversion();
	while(!(ADCSRA & (1 << ADSC)));
	
	Vout = ADCL * (5/1024);
	
	return Vout;	
}

