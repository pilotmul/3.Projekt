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
	
	//__________Init LED pin__________
	//Set pin 10 (PB5/OC1A) as output
	DDRB |= (1 << PB4);

	//Configure Timer1 for Fast PWM, non-inverting mode
	TCCR1A = (1 << COM1A1) | (1 << WGM11) | (1 << WGM10);
	TCCR1B = (1 << WGM12) | (1 << CS11); //Prescaler = 8

	//Set duty cycle (50% brightness)
	OCR1A = 512; //50% of 10-bit range (0-1023)
}

int analogRead(uint8_t channel) {
	//Choosing ADC-channel and zero the last 3 bits
	ADMUX = (ADMUX & 0xF8) | (channel & 0x07);
	
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
	
	OCR1A = brightness; //Sets LED brightness
	
	int percentage = lightReading/10;
	
	return percentage; //Return percentage of brightness
}