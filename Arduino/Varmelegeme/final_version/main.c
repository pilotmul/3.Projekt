#include "temperatureRegulator.h"
#include <stdio.h>

int main(void) {
    initTempRegulator(); // temp regulator set up
    float input;
    printf("input desired temperature: ");
    scanf("%f", &input);

    setTemperatureGoal(input);
}