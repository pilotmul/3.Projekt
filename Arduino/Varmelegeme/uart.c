/************************************************
* "uart.c":                                     *
* Implementation file for Mega2560 UART driver. *
* Using UART 0.                                 *
* Henning Hargaard                              *
*************************************************/
#include <avr/io.h>
#include <stdlib.h>
#include "uart.h"
#include "ftoa.h"

// Target CPU frequency
#define XTAL 16000000
#define BAUD 9600

/*************************************************************************
UART 0 initialization:
    Asynchronous mode.
    Baud rate = 9600.
    Data bits = 8.
    RX and TX enabled.
    No interrupts enabled.
    Number of Stop Bits = 1.
    No Parity.
    Baud rate = 9600.
    Data bits = 8.
*************************************************************************/
void InitUART(unsigned long baudRate, unsigned char dataBit, unsigned char RX_int)
{
	if (baudRate > 299 && baudRate < 115201 && dataBit > 4 && dataBit < 9)
	{
		if (RX_int)
		{
			UCSR0B = 0b10011000; //transmit & recieve enable, RX interrupt enable
			sei(); //enable global interrupt
		} 
		else UCSR0B = 0b00011000; //transmit & recieve enable, interrupts disabled

		
		UCSR0C |= dataBit - 5; //antal data-bits=8-bit, asynkron mode, ingen paritet, 1 stop-bit
		UBRR0 = (XTAL/16/baudRate)-1; //s�tter baud-rate til 9600
	}
	
}

/*************************************************************************
  Returns 0 (FALSE), if the UART has NOT received a new character.
  Returns value <> 0 (TRUE), if the UART HAS received a new character.
*************************************************************************/
unsigned char CharReady()
{
	if (UCSR0A & 0b10000000) return 1; //nyt tegn modtaget, return true
	else return 0; //nyt tegn ikke modtaget, return false
}

/*************************************************************************
Awaits new character received.
Then this character is returned.
*************************************************************************/
char ReadChar()
{
	while(!CharReady())
	{}
	return UDR0;
}

/*************************************************************************
Awaits transmitter register ready.
Then send the character.
Parameter :
	Tegn : Character for sending. 
*************************************************************************/
void SendChar(char tegn)
{
	while(!(UCSR0A & 0b00100000))
	{}
	UDR0 = tegn;
}

/*************************************************************************
Sends 0 terminated string.
Parameter:
   Streng: Pointer to the string. 
*************************************************************************/
void SendString(char* stringPtr)
{
	while(*stringPtr != 0)
	{
		SendChar(*stringPtr);
		stringPtr++;
	}
}

/*************************************************************************
Converts the integer "Tal" to an ASCII string - and then sends this string
using the USART.
Makes use of the C standard library <stdlib.h>.
Parameter:
    Tal: The integer to be converted and sent. 
*************************************************************************/
void SendInteger(int tal)
{
	char numberString[7];
	itoa(tal, numberString, 10);
	SendString(numberString);
}

void SendFloat(float tal)
{
	char numberString[7];
	ftoa(tal, numberString, 2);
	SendString(numberString);
}

/************************************************************************/