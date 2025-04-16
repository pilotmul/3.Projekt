#include "mainFunctions.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
#include "uart.h"
#include <math.h>
#include <stdio.h>
#include "avr/delay.h"

void handleReceived(int val, bool* motorAuto, bool* lightAuto, bool* windowOpen, float* targetTemp, int* LEDBrightness)
{
	int swCase = val/100;
    switch (swCase)
    {
    case 1: //Toggle motor and set mode manual
	
		if(*motorAuto) //Set mode manual
		{
			*motorAuto = false;
		}
		
        if(*windowOpen) //Toggle window
        {
			close();
			*windowOpen = false;
	        break;
        }
        else
		{
			open();
			*windowOpen = true;
			break;
		}
		
    
    case 2: //Toggle motor control mode
		if(*motorAuto)
		{
			*motorAuto = false;
			break;
		}
		else
		{
			*motorAuto = true;
			break;
		}
		
	
	case 3: //Set target temp
		*targetTemp = val%100;
		setTemperatureGoal(val%100);
		break;
	
	case 4: //Toggle light control mode
		
		if(*lightAuto)
		{
			*lightAuto = false;
			break;
		}
		else
		{
			*lightAuto = true;
			break;
		}
		
		
	case 5: //Set brightness and set mode manual
		*LEDBrightness = val%100;
		SetLEDBrightnessProcent(val%100);
		*lightAuto = false;
		break;
    
	case 6: //Test function of components 
	cli();
		//Testing window
		SendString("Testing window\n");
		open();
		_delay_ms(1500);
		close();
		_delay_ms(500);
		SendString("Window test complete\n");
		_delay_ms(1500);
		
		//Testing light
		SendString("Testing light\n");
		for(int i = 0; i<100; i+=5)
		{
			SetLEDBrightnessProcent(i);
			_delay_ms(500);
		}
		_delay_ms(1000);
		for(int i = 100; i>0; i-=5)
		{
			SetLEDBrightnessProcent(i);
			_delay_ms(500);
		}
		_delay_ms(1000);
		SendString("Testing complete\n");
		_delay_ms(1500);
		
		//Testing heatmat
		SendString("Testing heatmat at full power\n");
		setTemperatureGoal(100);
		_delay_ms(15000);
		SendString("Testing complete. Setting heatmat to 21\n");
		setTemperatureGoal(21);
		SendString("Entire test complete\n");
		_delay_ms(2000);
		sei();
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

void handleData(int* lightReading, float* temp, float* h2o, float* co2, int* LEDBrightness, bool* motorAuto, float* targetTemp, bool* lightAuto )
{
   if(*motorAuto)
   {
        if(*temp>*targetTemp+3 || *h2o>60.0 || *co2>1000) //Check for value limits
        {
			SendString("\n\t Opening window\n");
			SendString("\n\t Temp ");
			SendInteger(*temp);
			SendString("\n\t h20 ");
			SendInteger(*h2o);
			SendString("\n\t co2 ");
			SendInteger(*co2);
			
            //Åben vindue
            open();
        }
        else
        {
			SendString("\n\t Closeing window\n");
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
	if (*windowOpen)
	{
		SendInteger(1);
	}
	else
	{
		SendInteger(0);
	}
}