
#include "PIDControl.h"

//PID controller parameters and state
static float Kp = 0;
static float Ki = 0;
static float Kd = 0;
static float dt = 0;
static float integralMax = 0;
static float integralMin = 0;
static float setPoint = 30;
static float integral = 0;
static float previousError = 0;

void PIDControl_init(float _Kp, float _Ki, float _Kd, float _integralMax, float _integralMin, float _dt)
{    
    //set PID tuning parameters
    Kp = _Kp;
    Ki = _Ki;
    Kd = _Kd;
    integralMax = _integralMax;
    integralMin = _integralMin;
    dt = _dt;
}

//update setpoint
void PIDControl_changeSetPoint(float _setPoint)
{
    setPoint = _setPoint;
}

// Return current setpoint value
float get_setPoint()
{
    return setPoint;
}

float PIDControl_doStep(float systemOutput, float* proportionalPart, float* integralPart, float* derivativePart)
{
    float output = 0;
    float proportional = 0;
    float derivative = 0;
    float currentError = 0;
    
    // calculate current error
    currentError = setPoint - systemOutput;
    
    // calculate proportional part
    proportional = currentError;
    
    // calculate integral part
    integral = integral + (currentError * dt);
    
    // limit the integral
    if (integral > integralMax) integral = integralMax;
    if (integral < integralMin) integral = integralMin;
    
    // calculate derivative part
    derivative = (currentError - previousError) / dt;
    
    output = proportional * Kp + integral * Ki + derivative * Kd;
    *proportionalPart = proportional * Kp;
    *integralPart = integral * Ki;
    *derivativePart = derivative * Kd;
    
    //store errror for next cycle
    previousError = currentError;
    return output;
}
