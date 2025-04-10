/*
 * PWM.c
 *
 * Created: 07/05/2024 11.21.04
 *  Author: christianwinther
 */
#define F_CPU 16000000
#include <avr/io.h>
#include "PWM.h"
#include <util/delay.h>



void initPWM()
{
	//FAST PVM TIMER MED WAVEFORM P� PIN 11
	TCCR1A = 0b10000010;//OC1A
	TCCR1B = 0b00110011;//prescaler 64, 1.2KHz
	ICR1 = 200;//TOP = 200
	OCR1A = 0;
	DDRB = 0xFF;
}

void incPW()
{
	if (OCR1A < 200)
	{
		OCR1A += 20; //duty-cycle +10%
	}
}

void decPW()
{
	if (OCR1A > 0)
	{
		OCR1A -= 20; //duty-cycle -10%
	}
}

void setPW(int pct)
{
	if(pct <= 100 && pct >= 0){
		OCR1A = 2 * pct;
	}
	else if(pct > 100){
		OCR1A = 200;
	}
	else if(pct < 0){
		OCR1A = 0;
	}
	
}
