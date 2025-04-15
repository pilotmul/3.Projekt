#include "mainFunctions.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
#include "uart.h"
#include <stdio.h>

bool motorAuto = true;
bool lightAuto = true;

float targetTemp = 50;

void toggleModes(int mode)
{
    switch (mode)
    {
    case 1: //Toggle motor control mode
        if(motorAuto)
        {
            motorAuto = false;
            break;
        }
        //else
        motorAuto = true;
        break;
    
    case 2: //Toggle light control mode
        if(lightAuto)
        {
            lightAuto = false;
            break;
        }
        //else
        lightAuto = true;
        break;
    
    default:
        break;
    }
}

void initSystem()
{
    initTempRegulator();
    scd30_init();
    init_light();
    servo_init();
	InitUART(9600, 8, 1);
}


void readData(int* lightReading, float* temp, float* h2o, float* co2)
{
    //Get light reading
    *lightReading = analogRead();

    //Read sensor data
    scd30_read(co2, temp, h2o);      
    
}

void handleData(int* lightReading, float* temp, float* h2o, float* co2, int* LEDBrightness)
{
   
        if(*temp>targetTemp+3 || *h2o>60.0 || *co2>1000) //Check for value limits
        {
            //Åben vindue
            open();
        }
        else
        {
            //luk vindue
            close();
        }
   

    //_____LIGHT_____
    if(lightAuto) //Check for auto control mode
    {
		*LEDBrightness = SetLEDBrightness(*lightReading); //Set brightness
    }

    //_____TEMP_____
    regulateTemperature(*temp);
}

void sendData(int* LEDBrightness, float* temp, float* h2o, float* co2)
{
	char String[20];

	//Send temperature
	SendString("\nREADING START\n");
	SendString("\nTemp\n");
	sprintf(String, "%f", *temp);
	SendString(String);

	//Send light	
	SendString("\nLED Brightness\n");
	sprintf(String, "%d", *LEDBrightness);
	SendString(String);

	//Send CO2
	SendString("\nCo2\n");
	sprintf(String, "%f", *co2);
	SendString(String);

	//Send humidity
	SendString("\nHumidity\n");
	sprintf(String, "%f", *h2o);
	SendString(String);
	SendString("\nREADING END\n");
}