#include "motor.h"

// Initialiser Timer1 til 50 Hz PWM p� OC1A (PB5)
void servo_init() {
	DDRB |= (1 << PB6); // S�t PB5 som output

	// Fast PWM, mode 14: ICR1 som TOP
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); 

	ICR1 = 40000; 
}


void set_servo_position(uint16_t pulse_us) {
	OCR1A = (pulse_us * 2); // 1 us = 2 timer ticks ved 8 prescaler
}


void open() {
	set_servo_position(1800);
}


void close() {
	set_servo_position(1000);
}
