#pragma once
#include "Sensor/sensor.h"
#include "Motor/motor.h"
#include "Varmelegeme/temperatureRegulator.h"
#include "Sensor/LysSensor/LysSensor.h"
#include <stdio.h>
#include <stdbool.h>

void initSystem();

void toggleModes();

void readData(int* lightReading, float* temp, float* h2o, float* co2);

void handleData(int* lightReading, float* temp, float* h2o, float* co2);

void sendData(int* lightReading, float* temp, float* h2o, float* co2);