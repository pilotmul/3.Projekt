#include <avr/io.h>
#include <stdlib.h>
#include "lysSensor.h"
#include "uart.h"

void setup_sensors() {
	InitUART(9600,8,0); //Init uart with correct BAUD
	
	//__________Init ADC__________
	ADMUX = (1 << REFS0); //Set refrence for ADC to AVcc
	//Start ADC and set prescaler to 128
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	//__________Init LED pin (pin 10 = PB4 / OC2A on ATmega2560)__________
	DDRB |= (1 << PB4); // Set PB4 as output

	// Configure Timer2 for Fast PWM, non-inverting mode on OC2A
	TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20); // Fast PWM, non-inverting on OC2A
	TCCR2B = (1 << CS21); // Prescaler = 8

	// Set duty cycle (50% of 255 = ~128)
	OCR2A = 128;
}

int analogRead() {
	//Choosing ADC-channel and zero the last 3 bits
	ADMUX = (ADMUX & 0xF8) | (PF4 & 0x07);
	
	ADCSRA |= (1 << ADSC); //Start conversion
	while (ADCSRA & (1 << ADSC)); //While converting
	{
		//Wait for conversion to finish
	}
	
	return ADCW; //Return ADC value
}

int SetLEDBrightness(int lightReading)
{	
	//Ensure reading is within bounds
	if(lightReading < 0) //If too small
	{
		lightReading = 0;	
	}
	if (lightReading > 1000) //If too large
	{
		lightReading = 1000;
	}
	
	int brightness = lightReading / 4; //Roughly converts to 8bit
	
	OCR2A = brightness; //Sets LED brightness
	
	int percentage = lightReading/10;
	
	return percentage; //Return percentage of brightness
}