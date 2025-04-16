#include "mainFunctions.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
#include "mainFunctions.h"
#include "uart.h"
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>

char* tempBuffer[256]; //Temp buffer
char lightBuffer[100]; //Light buffer


float co2 = 0, temp = 0, h2o = 0, targetTemp = 25;
int LEDBrightness = 50;
bool windowOpen = false;
bool motorAuto = true;
bool lightAuto = true;

int lightReading;

char uartBuffer[10];
uint8_t uartIndex = 0;
uint16_t receivedValue = 0;
uint8_t valueReady = 0;

ISR(USART0_RX_vect)
{
	
	char receivedChar = UDR0;
	
	if(receivedChar == '\n')
	{
		uartBuffer[uartIndex] = '\0';
		receivedValue = atoi((char*)uartBuffer);
		valueReady = 1;
		uartIndex = 0;
		SendString("\nINTERRUPT!\n");
		SendInteger(receivedValue);
		handleReceived(receivedValue, &motorAuto, &lightAuto, &windowOpen, &targetTemp, &LEDBrightness);
	}
	else if(uartIndex < sizeof(uartBuffer) - 1)
	{
		uartBuffer[uartIndex++] = receivedChar;
	}
	
}

int main(void) 
{
	initSystem();
	
	
    setTemperatureGoal(targetTemp);
	close();

	
    //MAIN LOOP
    while(1)
    {
		
        //_____READ_____
        readData(&lightReading, &temp, &h2o, &co2);
		
		
        //_____HANDLE_____
        handleData(&lightReading, &temp, &h2o, &co2, &LEDBrightness, &motorAuto, &targetTemp, &lightAuto);
		
		
        //_____SEND____
        sendData(&LEDBrightness, &temp, &h2o, &co2, &windowOpen);
		
		
		_delay_ms(1000);
		
    }

}