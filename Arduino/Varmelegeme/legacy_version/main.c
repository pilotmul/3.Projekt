#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "PWM.h"
#include <stdio.h>
#include <stdlib.h>
#include "SCD30.h"
#include "PIDControl.h"

#define F_CPU 16000000UL
static float setPoint = 80; // degrees celcius
static char outputBuffer[256]; //Buffer for formatted output strings

#define SAMPLES_PER_SECOND 3
static uint16_t sampleWaitTimeInMilliseconds = 1000 / SAMPLES_PER_SECOND;

void handleByteRecieved(unsigned char i); //function prototype
void PID_print(float setPoint, float temp, float error, float controlSignal, float Kp, float Ki, float Kd, float proportionalPart, float integralPart, float derivativePart, float time);

ISR(USART0_RX_vect) //UART interrupt
{
	unsigned char i = UDR0;
    handleByteRecieved(i);
}

char* float2Char(float val){
  int val_int = (int) val;   // compute the integer part of the float

  float val_float = (abs(val) - abs(val_int)) * 100000;

  int val_fra = (int)val_float;

  sprintf (outputBuffer, "%d.%d", val_int, val_fra);
  return outputBuffer;
}

int main(void) {
    InitUART(9600, 8, 1);
    initPWM();
    TWI_Init();

    SendString("Starter SCD30...");
	// Start continuous målinger
	//scd30_send_cmd(0x0010, 0x0000, 1); // no pressure compensation

    //PID constants
    float Kp = 6.0f;
    float Ki = 3.0f/30.0f;
    float Kd = 0.0f;
    float integralMax = 3000;
    float integralMin = -3000;
    float temp = 0; //Variable to store temperature reading
    float co2, hum;
    float controlSignal = 0; //Variable to store control output
    float time = 0;
    float dt = ((float)sampleWaitTimeInMilliseconds) / 1000; // dt is measured in seconds
    
    //Initialize PID controller
    PIDControl_init(Kp, Ki, Kd, integralMax, integralMin, dt);
    PIDControl_changeSetPoint(setPoint);

    while (1)
    {     
        if (scd30_data_ready()) {
            scd30_read_measurements(&co2, &temp, &hum);

            // SendString("temp: ");
            // SendFloat(temp);
            // SendString("\n\r");
        } 
        
       //Retrieve updated setpoint from user input
       setPoint = get_setPoint();
       //Calculate error between setpoint and current temp
       float error = setPoint - temp;
       float proportionalPart = 0;
       float integralPart = 0;
       float derivativePart = 0;
       
       controlSignal = PIDControl_doStep(temp, &proportionalPart, &integralPart, &derivativePart);
       time += 0.33;
       setPW((int)controlSignal); //pass control siganl to PWM
       //PID_print(setPoint, temp, error, controlSignal, Kp, Ki, Kd, proportionalPart, integralPart, derivativePart, time);
       SendString("Setpoint, Temp, ControlSignal, Time\n\r");
       SendString(float2Char(setPoint));
       SendString(" , ");
       SendString(float2Char(temp));
       SendString(" , ");
       SendString(float2Char(controlSignal));
       SendString(" , ");
       SendString(float2Char(time));
       SendString("\n\r");

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
    }
    //flush RX buffer
    while (UCSR0A & (1 << RXC0)) {
        unsigned char dummy = UDR0; // Read and discard old data
    }
}

void PID_print(float setPoint, float temp, float error, float controlSignal, float Kp, float Ki, float Kd, float proportionalPart, float integralPart, float derivativePart, float time){
    SendString("SetPoint, temp, error, controlSignal, Kp, Ki, Kd, proportionalPart, integralPart, derivativePart, time\n\r");
    SendFloat(setPoint);
    SendString(" , ");
    SendFloat(temp);
    SendString(" , ");
    SendFloat(error);
    SendString(" , ");
    SendFloat(controlSignal);
    SendString(" , ");
    SendFloat(Kp);
    SendString(" , ");
    SendFloat(Ki);
    SendString(" , ");
    SendFloat(Kd);
    SendString(" , ");
    SendFloat(proportionalPart);
    SendString(" , ");
    SendFloat(integralPart);
    SendString(" , ");
    SendFloat(derivativePart);
    SendString(" , ");
    SendFloat(time);
    SendString("\n\r");
}