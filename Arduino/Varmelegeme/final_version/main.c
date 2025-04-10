#include "temperatureRegulator.h"

int main(void) {
    initTempRegulator(); // temp regulator set up
    changeTemperatureGoal(50); //set temperatur goal
    regulateTemperatureContinuous(); //regulation loop
}