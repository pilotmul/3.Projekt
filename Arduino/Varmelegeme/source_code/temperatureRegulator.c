#include "temperatureRegulator.h"

/***** Global variables used for sensor reading and printing data ******/
float temp, co2, hum = 0; //Variable to store temperature reading
static char outputBuffer[256]; //Buffer for formatted output strings

/******* PID controller sample rate settings *********/
#define SAMPLES_PER_SECOND 3
static uint16_t sampleWaitTimeInMilliseconds = 1000 / SAMPLES_PER_SECOND;

/****** PID variables *******/
float Kp = 6.0f;
float Ki = 1.0f/60.0f;
float Kd = 0.0f;
float integralMax = 3000;
float integralMin = -3000;
float controlSignal = 0; //Variable to store control output
static float setPoint = 50; // temperature goal in degrees celcius
float time = 0;

void initTempRegulator(){
    InitUART(9600, 8, 1);
    initPWM();
    TWI_Init();

    //Initialize PI controller
    float dt = ((float)sampleWaitTimeInMilliseconds) / 1000; // dt is measured in seconds
    PIDControl_init(Kp, Ki, Kd, integralMax, integralMin, dt);
    PIDControl_changeSetPoint(setPoint); //sets the temperature goal
    scd30_send_cmd(0x0010, 0x0000, 1); // starts continous sensor reading
}

void setTemperatureGoal(float goal){
    PIDControl_changeSetPoint(goal); //sets the temperature goal
    setPoint = goal;
}

/******** locking loop that controls the temperature ********/
void regulateTemperatureContinuous(){
    while(1){
        //læs data fra sensor
        if (scd30_data_ready()) {
            scd30_read_measurements(&co2, &temp, &hum);
        } 

        //Calculate error between setpoint and current temp
       float error = setPoint - temp;

       //PI controller variables
       float proportionalPart = 0;
       float integralPart = 0;
       float derivativePart = 0;

       // adjust heating
       controlSignal = PIDControl_doStep(temp, &proportionalPart, &integralPart, &derivativePart);
       setPW((int)controlSignal); //pass control siganl to PWM
       _delay_ms(sampleWaitTimeInMilliseconds); //delay between readings
    }
}

/********* regulates controller once, non locking *********/
void regulateTemperature(float currentTemp){
    temp = currentTemp;
    //Calculate error between setpoint and current temp
   float error = setPoint - temp;

   //PI controller variables
   float proportionalPart = 0;
   float integralPart = 0;
   float derivativePart = 0;

   // adjust heating
   controlSignal = PIDControl_doStep(temp, &proportionalPart, &integralPart, &derivativePart);
   setPW((int)controlSignal); //pass control siganl to PWM
}

/****** Prints last read temp via UART *******/
void printTemp(){ 
    SendString("Temp: ");
    SendString(float2Char(temp, outputBuffer));// skriv temp til UART
}

void PID_print(){
    //SendString("Setpoint, Temp, ControlSignal, ProportionalPart, IntegralPart, Time\n\r");
    SendString(float2Char(setPoint, outputBuffer));
    SendString(",");
    SendString(float2Char(temp, outputBuffer));
    SendString(",");
    SendString(float2Char(controlSignal, outputBuffer));
    SendString("\n\r");
}