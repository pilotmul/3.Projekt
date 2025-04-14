#include<mainFunctions.h>

bool motorAuto = true;
bool lightAuto = true;

float targetTemp = 50

void toggleModes(int case)
{
    switch (case)
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
    lightReading* = readLight();

    //Read sensor data
    scd30_read(co2, temp, h2o);      
    
}

void handleData(int* lightReading, float* temp, float* h2o, float* co2)
{
    //_____MOTOR_____
    if(motorMode) //Check for auto control mode
    {
        if(temp*>targetTemp+3 || h2o*>60 || co2*>1000) //Check for value limits
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
    if(lightMode) //Check for auto control mode
    {
        SetLEDBrightness(*lightReading); //Set brightness
    }

    //_____TEMP_____
    regulateTemperature(*temp)
}

void sendData(int* lightReading, float* temp, float* h2o, float* co2)
{
	char tempString[20];

    //Send temp data
	dtostrf(temp, tempString);
	uart_send_string(tempString);

    //Send light data
	itoa(lightReading, tempString);
	uart_send_string(tempString);
    
    //Send co2 data
	dtostrf(co2, tempString);
	uart_send_string(tempString);
    
    //Send humidity data
	dtostrf(h2o, tempString);
	uart_send_string(tempString);
}
