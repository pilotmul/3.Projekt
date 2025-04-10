
/***************************************************************/
// OBS, der skal være en implimenteret float2char() funktion    /
// i uart.h for at nogen af print funktionerne virker.          /
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
#include "uart.h"
#include "PWM.h"
#include "SCD30.h"
#include "PIDControl.h"

void initTempRegulator();
void regulateTemperatureContinuous(); //locking loop that controls the temperature
void regulateTemperature(float temp); //regulates controller once, based on the current temperature
void printTemp(); //Prints last read temp via UART
void setTemperatureGoal(float goal);

/**** prints PID data, used for debugging  ********/
void PID_print();