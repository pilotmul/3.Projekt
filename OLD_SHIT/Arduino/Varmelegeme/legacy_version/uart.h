/****************************************
* "uart.h":                             *
* Header file for Mega2560 UART driver. *
* Using UART 0.                         *
* Henning Hargaard                      *
*****************************************/ 
#include <avr/interrupt.h>
void InitUART(unsigned long baudRate, unsigned char dataBit, unsigned char RX_int);
unsigned char CharReady();
char ReadChar();
void SendChar(char Tegn);
void SendString(char* Streng);
void SendInteger(int Tal);
void SendFloat(float tal);
/****************************************/
