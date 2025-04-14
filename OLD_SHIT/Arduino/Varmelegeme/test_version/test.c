#include "temperatureRegulator.h"

int main(void) {
    initTempRegulator();
    setTemperatureGoal(50);
    regulateTemperatureContinuous();
}