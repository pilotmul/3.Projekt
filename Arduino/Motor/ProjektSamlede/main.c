#include<mainFunctions.h>
#include<Motor/GccApplication1/motor.h>
#include<Varmelegeme/temperatureRegulator.h>
#include<Sensor/LysSensor/LysSensor.h>
#include<stdio.h>

char* buffer[256]; //Temp buffer
char buffer[100]; //Light buffer

int main(void) 
{
    //Variables
    float co2, temp, h2o;
    int lightReading = 0;

    setTemperatureGoal(tempGoal);
    //MAIN LOOP
    while(1)
    {
        //_____READ_____
        readData(*lightReading, *co2, *temp, *h2o)

        //_____HANDLE_____
        handleData(*lightReading, *co2, *temp, *h2o)

        //_____SEND____
        sendData(*lightReading, *co2, *temp, *h2o)
    }

}