#include "mainFunctions.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
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
}


void readData(int* lightReading, float* temp, float* h2o, float* co2)
{
    //Get light reading
    *lightReading = analogRead();

    //Read sensor data
    scd30_read(co2, temp, h2o);      
    
}

void handleData(int* lightReading, float* temp, float* h2o, float* co2)
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
        SetLEDBrightness(*lightReading); //Set brightness
    }

    //_____TEMP_____
    regulateTemperature(*temp);
}

void sendData(int* lightReading, float* temp, float* h2o, float* co2)
{
	char tempString[20];

	// Send temperature
	dtostrf(*temp, 5, 2, tempString);
	SendString(tempString);

	// Send light
	itoa(*lightReading, tempString, 10);
	SendString(tempString);

	// Send CO2
	dtostrf(*co2, 6, 2, tempString);
	SendString(tempString);

	// Send humidity
	dtostrf(*h2o, 5, 2, tempString);
	SendString(tempString);
}