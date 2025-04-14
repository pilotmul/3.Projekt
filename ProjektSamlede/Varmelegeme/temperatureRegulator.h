
/***************************************************************/
// OBS, der skal være en implimenteret float2char() funktion    /
// i uart.h for at nogen af print funktionerne virker.          /
// PWM BRUGER PIN 11                                            /
// SCD30 sensor bruger SCL(hvid) -> PIN 21, SDA(gul) -> PIN 20  /
//                                                              /
// initTempRegulator() skal kaldes i starten af programmet.     /
//                                                              /
// regulateTemperatureContinuous() kunne evt. bruges som        /
// en thread-function hvis vi vælger at gå den retning.         /
//                                                              /
// RegulateTemperature(float temp) kan bruges til at            /
// regulerer temperatur uden brug af threads, der bør måske     /
// være en lille delay mellem hver kald tho. Tænker dog at      /
// det nok ville virke uden delay, men det ville nok belaste    /
// ret hårdt... regulator funktionerne behøves i bund og grund  /
// ikke kaldes ret ofte, da varmemåtten varmer så langsomt      /
/***************************************************************/

/********** Dependencies *********/

#include "PWM.h"
#include "PIDControl.h"

void initTempRegulator();
void regulateTemperature(float currentTemp); //regulates controller once, based on passed temp
void printTemp(); //Prints last read temp via UART
void setTemperatureGoal(float goal);

/**** prints PID data, used for debugging  ********/
void PID_print();