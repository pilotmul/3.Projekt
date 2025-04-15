#include "mainFunctions.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
#include "mainFunctions.h"
#include "uart.h"
#include <stdio.h>
#include <util/delay.h>

char* tempBuffer[256]; //Temp buffer
char lightBuffer[100]; //Light buffer

int main(void) 
{
	initSystem();
    //Variables_delay_ms(2000);
	float co2 = 0, temp = 0, h2o = 0;
	
    int lightReading;
	
	
    setTemperatureGoal(50.0);
	close();
	
    //MAIN LOOP
    while(1)
    {
		
        //_____READ_____
        readData(&lightReading, &temp, &h2o, &co2);
		
        //_____HANDLE_____
        handleData(&lightReading, &temp, &h2o, &co2);
		
        //_____SEND____
        sendData(&lightReading, &temp, &h2o, &co2);
		
		_delay_ms(500);
		
    }

}