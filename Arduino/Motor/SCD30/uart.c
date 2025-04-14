#include <avr/io.h>
#include <stdlib.h>
#include "uart.h"


#define F_CPU 16000000  // Definerer systemets clock frekvens til 16 MHz




// Funktion til initialisering af UART0 med specificeret baud rate og antal databits sættes normalt til
// InitUART(9600,8,1);
void InitUART(unsigned long BaudRate, unsigned char DataBit, unsigned char Rx_Int)
{
	// Sikrer, at inputparametrene er inden for tilladte grænser
	if (BaudRate < 300 || BaudRate >= 115200 || DataBit < 5 || DataBit > 8) {
		return;  // Returnerer og afbryder funktionen, hvis parametre er ugyldige
	}
	
	//Starter interrupt baseret på om Rx_Int er 0 eller 1.
	if (Rx_Int>0)
	{
		sei();
		UCSR0B =(1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0);
	}
	else
	{
		// Aktiverer modtageren og transmitteren, men deaktiverer alle interrupts
		UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	}
	
	
	// Beregner værdien til UBRR0 registeret for at indstille baud raten
	unsigned int ubrrValue = (F_CPU / 16 / BaudRate) - 1;
	UBRR0H = (unsigned char)(ubrrValue >> 8);  // Sætter de højeste 8 bits af UBRR0
	UBRR0L = (unsigned char)ubrrValue;  // Sætter de laveste 8 bits af UBRR0

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 databits, 1 stopbit, ingen paritet


	// Nulstiller først UCSR0C og indstiller derefter de nødvendige flags
	UCSR0C = 0;
	UCSR0C |= (1 << UCSZ00);  // Sætter basisindstillingerne for frame format: asynkron mode, 1 stop bit

	// Indstiller antallet af databits per frame baseret på inputparameteren
	switch (DataBit) {
		case 5: UCSR0C |= 0; break;  // 5 databits
		case 6: UCSR0C |= (1 << UCSZ00); break;  // 6 databits
		case 7: UCSR0C |= (1 << UCSZ01); break;  // 7 databits
		case 8: UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); break;  // 8 databits
		default: return;  // Afslutter, hvis databits er uden for tilladt omfang
	}
}

// Funktion til at kontrollere, om der er modtaget en ny karakter
unsigned char CharReady()
{
	if (UCSR0A & (1 << RXC0)) {  // Tjekker RXC0-flaget for at se, om der er modtaget data
		return 1;  // Returnerer sand, hvis der er modtaget en ny karakter
		} else {
		return 0;  // Returnerer falsk, hvis der ikke er modtaget data
	}
}

// Funktion til at læse en karakter fra UART
char ReadChar()
{
	while (!(UCSR0A & (1 << RXC0))) {}  // Vent indtil der er modtaget en karakter
	return UDR0;  // Returnerer den modtagne karakter fra dataregisteret
}

// Funktion til at sende en karakter over UART
void SendChar(char Tegn)
{
	while (!(UCSR0A & (1 << UDRE0))) {}  // Vent indtil dataregisteret er tomt
	UDR0 = Tegn;  // Sender karakteren
}

// Funktion til at sende en streng over UART
void SendString(char* Streng)
{
	while (*Streng) {  // Løkke gennem hver karakter i strengen indtil null-terminator
		SendChar(*Streng++);  // Sender en karakter ad gangen og inkrementerer pointeren
	}
}

// Funktion til at konvertere et heltal til en streng og sende det over UART
void SendInteger(int Tal)
{
	char streng[7];  // Buffer til at holde den konverterede streng
	itoa(Tal, streng, 10);  // Konverterer tallet til en streng i decimal
	SendString(streng);  // Sender den konverterede streng
}
