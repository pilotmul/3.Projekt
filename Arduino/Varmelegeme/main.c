#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "PWM.h"
#include <stdio.h>
#include <stdlib.h>
#include "SCD30.h"
#include "PIDControl.h"
/**** Global variables used for sensor reading and printing data ******/
static float setPoint = 50; // temperature goal in degrees celcius
float temp, co2, hum = 0; //Variable to store temperature reading
static char outputBuffer[256]; //Buffer for formatted output strings

/******* PID controller sample rate settings *********/
#define SAMPLES_PER_SECOND 3
static uint16_t sampleWaitTimeInMilliseconds = 1000 / SAMPLES_PER_SECOND;

/******** Function prototypes ***********/
void handleByteRecieved(unsigned char i); // handles UART user input using a switch statement
void PID_print(float setPoint, float temp, float controlSignal, float proportionalPart, float integralPart, float time); // Prints PID data via UART
void printTemp(); // Prints last measured temp via UART

/******** UART interrupt *********/
ISR(USART0_RX_vect){
	unsigned char i = UDR0;
    handleByteRecieved(i);
}

int main(void) {
    InitUART(9600, 8, 1);
    initPWM();
    TWI_Init();

    SendString("Starter SCD30...");

    //PID constants
    float Kp = 6.0f;
    float Ki = 1.0f/60.0f;
    float Kd = 0.0f;
    float integralMax = 3000;
    float integralMin = -3000;
    
    
    float controlSignal = 0; //Variable to store control output
    float time = 0;
    float dt = ((float)sampleWaitTimeInMilliseconds) / 1000; // dt is measured in seconds
    
    //Initialize PID controller
    PIDControl_init(Kp, Ki, Kd, integralMax, integralMin, dt);
    PIDControl_changeSetPoint(setPoint);
    scd30_send_cmd(0x0010, 0x0000, 1); // no pressure compensation

    SendString("Setpoint, Temp, ControlSignal, ProportionalPart, IntegralPart, Time\n\r");
    while (1)
    {     
        //læs data fra sensor
        if (scd30_data_ready()) {
            scd30_read_measurements(&co2, &temp, &hum);
        } 
        
       //Retrieve updated setpoint from user input
       setPoint = get_setPoint();

       //Calculate error between setpoint and current temp
       float error = setPoint - temp;
       float proportionalPart = 0;
       float integralPart = 0;
       float derivativePart = 0;
       
       // adjust PI controller
       controlSignal = PIDControl_doStep(temp, &proportionalPart, &integralPart, &derivativePart);
       time += 0.33; //increment time spent
       setPW((int)controlSignal); //pass control siganl to PWM

       //print PID data
       PID_print(setPoint, temp, controlSignal, proportionalPart, integralPart, time);
       _delay_ms(sampleWaitTimeInMilliseconds);
    }
    
    return 0;
}

void handleByteRecieved(unsigned char i){
    switch(i){
        case '1':
            SendString("Heating OFF");
            setPW(0);
            break;

        case '2':
            SendString("Heating ON");
            setPW(100);
            break;
        default:
            SendString("Input out of bounds!");
            break;
    }
    //flush RX buffer
    while (UCSR0A & (1 << RXC0)) {
        unsigned char dummy = UDR0; // Read and discard old data
    }
}

void PID_print(float setPoint, float temp, float controlSignal, float proportionalPart, float integralPart, float time){
    SendString(float2Char(setPoint, outputBuffer));
    SendString(",");
    SendString(float2Char(temp, outputBuffer));
    SendString(",");
    SendString(float2Char(controlSignal, outputBuffer));
    SendString(",");
    SendString(float2Char(proportionalPart, outputBuffer));
    SendString(",");
    SendString(float2Char(integralPart, outputBuffer));
    SendString(",");
    SendString(float2Char(time, outputBuffer));
    SendString("\n\r");
}

void printTemp(){
    SendString("Temp: ");
    SendString(float2Char(temp, outputBuffer));// skriv temp til UART
}