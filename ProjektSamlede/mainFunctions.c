#include "mainFunctions.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
#include "uart.h"
#include <math.h>
#include <stdio.h>





void handleReceived(int val, bool* motorAuto, bool* lightAuto, bool* windowOpen, float* targetTemp)
{
    switch (floor(val/100))
    {
    case 1: //Toggle motor and set mode manual
        
        *motorAuto = false;
        break;
    
    case 2: //Toggle motor control mode
		if(*motorAuto)
		{
			*motorAuto = false;
			break;
		}
		*motorAuto = true;
		break;
	
	case 3: //Set target temp
		*targetTemp = val%100;
		break;
	
	case 4: //Toggle light control mode
		
		if(*lightAuto)
		{
			*lightAuto = false;
			break;
		}
		//else
		*lightAuto = true;
		break;
		
	case 5: //Set brightness and set mode manual
		SetLEDBrightness(val%100);
		*lightAuto = false;
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

void handleData(int* lightReading, float* temp, float* h2o, float* co2, int* LEDBrightness, bool* motorAuto, int* targetTemp, bool* lightAuto )
{
   if(*motorAuto)
   {
        if(*temp>*targetTemp+3 || *h2o>60.0 || *co2>1000) //Check for value limits
        {
            //Åben vindue
            open();
        }
        else
        {
            //luk vindue
            close();
        }
   }

    //_____LIGHT_____
    if(*lightAuto) //Check for auto control mode
    {
		*LEDBrightness = SetLEDBrightness(*lightReading); //Set brightness
    }

    //_____TEMP_____
    regulateTemperature(*temp);
}

void sendData(int* LEDBrightness, float* temp, float* h2o, float* co2, bool* windowOpen)
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
	
	//Send windowState
	if (windowOpen)
	{
		SendInteger(1);
	}
	else
	{
		SendInteger(0);
	}
}