#define F_CPU 16000000UL
#include <avr/io.h>


//Starter til timer1 og s�tter 50Hz PWM p� OC1A (PB5)
void servo_init();


void set_servo_position(uint16_t pulse_us);


void open();


void close();

