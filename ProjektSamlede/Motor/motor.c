#include "motor.h"

// Initialiser Timer1 til 50 Hz PWM på OC1B (PB6 / pin 12)
void servo_init() {
	DDRB |= (1 << PB6); // Sæt PB6 (pin 12) som output

	// Fast PWM, mode 14: ICR1 som TOP
	TCCR1A = (1 << COM1B1) | (1 << WGM11);              // Brug OC1B
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler = 8

	ICR1 = 40000; // 50 Hz: 20 ms periode (2 MHz / 40000)
}

void set_servo_position(uint16_t pulse_us) {
	OCR1B = pulse_us * 2; // 1 us = 2 ticks ved 2 MHz
}

void open() {
	set_servo_position(1800); // 1.8 ms puls → f.eks. 90-120 grader
}

void close() {
	set_servo_position(1000); // 1.0 ms puls → f.eks. 0 grader
}
