#include "temperatureRegulator.h"
#include <stdio.h>
char* buffer[256];

int main(void) {
    initTempRegulator(); // temp regulator set up

    setTemperatureGoal(50);

    float co2, temp, hum;
    while (1)
    {
        //read sensor data
        while(scd30_data_ready()) { //flush buffer to get newest reading
            scd30_read_measurements(&co2, &temp, &hum);
        } 
        printTemp();
        
        /*evt. åben vindue*/

        /*dæmp lyset*/

        //juster varmelegeme baseret på nuværrende temperatur
        regulateTemperature(temp);

        /*send data til server*/

        _delay_ms(1000);
    }
    
}