#include "mainFunctions.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
#include "mainFunctions.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>

char* tempBuffer[256]; //Temp buffer
char lightBuffer[100]; //Light buffer


float co2 = 0, temp = 0, h2o = 0;
int LEDBrightness = 0;
bool windowOpen = false;
bool motorAuto = true;
bool lightAuto = true;

int lightReading;

ISR(USART0_RX_vect) 
{
	int data = UDR0;

	handleReceived(data, &motorAuto, &lightAuto, &windowOpen, &targetTemp)
}

int main(void) 
{
	initSystem();
	sei();
	
	
    setTemperatureGoal(25.0);
	close();
	
    //MAIN LOOP
    while(1)
    {
		
        //_____READ_____
        readData(&lightReading, &temp, &h2o, &co2);
		
        //_____HANDLE_____
        handleData(&lightReading, &temp, &h2o, &co2, &LEDBrightness);
		
        //_____SEND____
        sendData(&LEDBrightness, &temp, &h2o, &co2);
		
		_delay_ms(500);
		
    }

}