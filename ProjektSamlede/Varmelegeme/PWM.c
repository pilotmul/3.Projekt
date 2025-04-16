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
	TCCR3A = (1 << COM3A1) | (1 << WGM31);//OC3A
	TCCR3B |= (1<<WGM32) | (1<<WGM33) | (1<<CS31) | (1<<CS30);//prescaler 64, 1.2KHz
	ICR3 = 200;//TOP = 200
	OCR3A = 0;
	DDRE |=(1<<PE3);
}

void incPW()
{
	if (OCR3A < 200)
	{
		OCR3A += 20; //duty-cycle +10%
	}
}

void decPW()
{
	if (OCR3A > 0)
	{
		OCR3A -= 20; //duty-cycle -10%
	}
}

void setPW(int pct)
{
	if(pct <= 100 && pct >= 0){
		OCR3A = 2 * pct;
	}
	else if(pct > 100){
		OCR3A = 200;
	}
	else if(pct < 0){
		OCR3A = 0;
	}
	
}
