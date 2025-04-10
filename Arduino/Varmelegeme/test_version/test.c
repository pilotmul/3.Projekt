#include "temperatureRegulator.h"

int main(void) {
    initTempRegulator();
    changeTemperatureGoal(50);
    regulateTemperatureContinuous();
}