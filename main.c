/*------------------------------------------
---------------- Version: 5 ---------------- 
--------------------------------------------
----------------- Authors ------------------
------------- Kymani Anderson --------------
------------- Nicholas Deane ---------------
--------------------------------------------
------------------------------------------*/

/*
	Todo:
	1. Voltage in a DC Circuit 					✅
	2. Capacitance 								❌
	3. Resistance								✅
	4. Temparature								✅
	5. Frequency of Square Wave (DC Signal)		❌
	6. Inductance								❌
	7. Distance Measurement						✅
	8. Display Modes w/ Button			 		✅
	9. Display on LCD					 		✅
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

#define capacitancePin 0
#define tempPin 1
#define voltagePin 2
#define resistancePin 3
#define R1 100000
#define R2 10000

float adc_read();
void Temperature();
void Voltage();
void Resistance();
void Frequency();
void Capacitance();
void Distance();

int mode = 0;
char str[10];
int overFlow = 0;
int pulse = 0;
long timerCount;
long time_count;

ISR(INT0_vect)
{
	if (bit_is_clear(PIND,2))
	{
		mode++;

		if(mode > 5)
		{
			mode = 0;
		}
		
		while(bit_is_clear(PIND,2))
		{
			_delay_ms(100);
		}
	}
}

ISR (INT1_vect)
{
	if (pulse==1)
	{
		TCCR1B = 0x00;
		timerCount = TCNT1;
		TCNT1 = 0;
		pulse = 0;
	}
	if (pulse==0)
	{
		TCCR1B |= (1<<CS10);
		pulse = 1;
	}
}

ISR (TIMER1_OVF_vect)
{
	overFlow++;
}

ISR(ANA_COMP_vect)
{
	time_count = TCNT1 + (65536 * overFlow);   
	TCCR1B = 0;                            
}

int main()
{
	
	/* ====== io setup ====== */
	DDRC = 0xFF;
	DDRA = 0x00;
	DDRD = 0x00;
	DDRB |= (1<<PB1);
	PORTD = 0xFF;
	
	/* ====== Interrupt Setup ====== */
	GICR |= (1<<INT0);
	MCUCR |= (1<<ISC01);
	
	/* ====== ADC Setup ======= */
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1<<ADEN)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS0);
	
	/* ====== Input Capture Setup ======*/
	TCCR1A = 0;
	TCCR1B |= (1 << ICES1)|(1<<CS10);
	TCNT1 = 0;
	
	/* ====== Timer1 Overflow Interrupt Enable ====== */
	TIMSK |= (1 << TOIE1);
	
	sei();
	lcd_init();
	
	while(1)
	{
		switch (mode)
		{
			
			case 0:			
			clear_lcd();
			lcd_gotoxy(1,1);
			lcd_print("Voltage Mode     ");
			_delay_ms(1000);
			Voltage();
			break;
			
			case 1:
			clear_lcd();
			lcd_gotoxy(1,1);
			lcd_print("Temperature Mode");
			_delay_ms(1000);
			Temperature();
			break;
			
			
			case 2:
			clear_lcd();
			lcd_gotoxy(1,1);
			lcd_print("Resistance Mode ");
			_delay_ms(1000);
			Resistance();
			break;
			
			case 3:
			clear_lcd();
			lcd_gotoxy(1,1);
			lcd_print("Frequency Mode  ");
			_delay_ms(1000);
			Frequency();
			
			case 4: 
			clear_lcd();
			lcd_gotoxy(1,1);
			lcd_print("Capacitance Mode  ");
			_delay_ms(1000);
			Capacitance();
			
			case 5:
			clear_lcd();
			lcd_gotoxy(1,1);
			lcd_print("Distance Mode  ");
			_delay_ms(1000);
			Distance();
			GICR &= ~(1<<INT1);
			MCUCR &= ~(1<<ISC10);
		}
	}
}

float adc_read(int channel)
{
	float sensorValue;
	
	ADMUX &= (0b11110000);
	ADMUX |= (channel & 0x07);
	ADCSRA |= (1<<ADSC);
	while (!(ADCSRA & (1<< ADIF)));
	ADCSRA |= (1<<ADIF);
	_delay_ms(1);

	sensorValue = ADC;
	
	return sensorValue;
}

void Voltage ()
{
	float voltage, volt;
	clear_lcd();
	lcd_gotoxy(1,1);
	lcd_print("Voltage         ");
	while (mode == 0)
	{
		volt = adc_read(voltagePin) * (5.0/1023.0);
		voltage = ((R1+R2)/R2) * volt;
		dtostrf(voltage, 6, 2, str);
		lcd_gotoxy(1,2);
		lcd_print(str);
		//lcd_gotoxy(15,2);
		//lcd_print("V");
	}
}

void Temperature ()
{
	float volt, temp;
	clear_lcd();
	lcd_gotoxy(1,1);
	lcd_print("Temperature: ");
	while(mode == 1)
	{
		volt = adc_read(tempPin) * (5.0/1023.0);
		temp = volt * 100;
		lcd_gotoxy(1,2);
		dtostrf(temp, 6, 2, str);
		lcd_print(str);	
	}
}

void Resistance()
{
	float resistance;
	clear_lcd();
	while (mode == 2)
	{
		clear_lcd();
		float volt = adc_read(resistancePin) * (5.0/1023.0);
		resistance = volt*10000/(5.0 - volt);
		lcd_gotoxy(1,1);
		lcd_print("Resistance:     ");
		lcd_gotoxy(1,2);
		dtostrf(resistance, 6, 2, str);
		lcd_print(str);
		_delay_ms(1000);
	}
}

void Frequency()
{
	clear_lcd();
	lcd_gotoxy(1,1);
	lcd_print("Frequency:      ");
	while (mode == 3)
	{
		long period, capture1, capture2;
		float frequency;
		TCNT1 = 0;
		TIFR = (1 << ICF1);

		while (!(TIFR & (1 << ICF1)));
		capture1 = ICR1 + (65536 * overFlow);
		TIFR |= (1 << ICF1);

		while (!(TIFR & (1 << ICF1)));
		capture2 = ICR1 + (65536 * overFlow);

		period = (capture2 - capture1)/(1000/0.008192);
		frequency = 1/period;

		overFlow = 0;
		lcd_gotoxy(1,2);
		dtostrf(frequency, 6, 2, str);
		lcd_print(str);
	}	
	
}

void Capacitance()
{
	
	float capacitance, volt;
	double divide_by = 180375000000;
	
	clear_lcd();
	lcd_gotoxy(1,1);
	lcd_print("Capacitance: ");
	while (mode == 4)
	{
		DDRB |= (1<<PB0)|(1<<PB2);
		DDRD |= (1<<PD7);
		PORTB &= ~(1<<PB0);    
		PORTB &= ~(1<<PB2);              
		volt = (adc_read(capacitancePin)) * (5.0/1023.0);   
		
		if(volt < 0.5)
		{
			DDRB &= ~(1<<PB2);                       
			ACSR = (1<<ACIE)|(1<<ACIS1)|(1<<ACIS0);    
			
			PORTB |= (1<<PB0);        
			TCNT1 = 0;                
			TCCR1B |= (1<<CS10);            
			PORTD |= (1<<PD7);        
			
			capacitance = time_count/divide_by;
			dtostrf(capacitance, 9, 9, str);
			lcd_gotoxy(1, 2);
			lcd_print(str);
			PORTD &= ~(1<<PD7);
			_delay_ms(1000);
		}
	}
}	

void Distance()
{
	GICR |= (1<<INT1);
	MCUCR |= (1<<ISC10);
	clear_lcd();
	lcd_gotoxy(1,1);
	lcd_print("Distance: ");
	while (mode == 5)
	{
		float distance_cm, distance_m;
		double pulseWidth;
		PORTB |= (1<<PB1);
		_delay_us(10);
		PORTB &= (~(1<<PB1));

		pulseWidth= timerCount + (65536 * overFlow);  // calculate total width of pulse
		distance_cm= pulseWidth/466.47;          // calculate distance in cm
		distance_m = distance_cm/100;        // calculate distance in meters
		dtostrf(distance_cm, 6, 2, str);
		lcd_gotoxy(1,2);
		lcd_print(str);
		
		overFlow = 0;
	}
}