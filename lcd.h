/*
 * lcd.h
 *
 * Created: 08/10/2019 16:05:37
 *  Author: lindon
 */ 
#ifndef LCD_H_
#define LCD_H_
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define	LCD_PRT	PORTC				//LCD DATA PORT
#define	LCD_DDR	DDRC				//LCD DATA DDR
#define	LCD_PIN	PINC				//LCD DATA PIN
#define	LCD_RS	PC6					//LCD RS
#define	LCD_RW	PC5					//LCD RW
#define LCD_EN	PC4					//LCD EN

void lcdCommand(unsigned char);
void lcdData(unsigned char);
void lcd_init();
void lcd_gotoxy(unsigned char, unsigned char );
void lcd_print(char *);
void clear_lcd();
unsigned char GetKey(void);
void CopyStringtoLCD(const char*);

#endif /* LCD_H_ */