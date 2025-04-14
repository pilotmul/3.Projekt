
#define F_CPU 16000000UL
#define BAUD 9600
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void uart_init(unsigned int ubrr) {
	UBRR1H = (ubrr >> 8);
	UBRR1L = ubrr;
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

void uart_transmit(char data) {
	while (!(UCSR1A & (1 << UDRE1)));
	UDR1 = data;
}

void uart_send_string(const char* str) {
	while (*str) {
		uart_transmit(*str++);
	}
}

char uart_receive() {
	while (!(UCSR1A & (1 << RXC1)));
	return UDR1;
}

void uart_receive_line(char* buffer, int max_len) {
	int i = 0;
	char c;
	do {
		c = uart_receive();
		if (c == '\r' || c == '\n') break;
		if (i < max_len - 1) {
			buffer[i++] = c;
		}
	} while (1);
	buffer[i] = '\0';
}

int random_int(int min, int max) {
	return min + rand() % (max - min + 1);
}

void send_fake_sensor_data() {
	char temp_str[20];

	float temp = (float)random_int(180, 250) / 10.0;
	dtostrf(temp, 4, 1, temp_str);
	uart_send_string("TEMP:");
	uart_send_string(temp_str);
	uart_send_string("\n");

	itoa(random_int(0, 100), temp_str, 10);
	uart_send_string("LIGHT:");
	uart_send_string(temp_str);
	uart_send_string("\n");

	itoa(random_int(400, 800), temp_str, 10);
	uart_send_string("CO2:");
	uart_send_string(temp_str);
	uart_send_string("\n");

	itoa(random_int(30, 70), temp_str, 10);
	uart_send_string("HUMIDITY:");
	uart_send_string(temp_str);
	uart_send_string("\n");
}

void process_command(const char* buffer) {
	if (strncmp(buffer, "TEMP:", 5) == 0) {
		uart_send_string("Modtaget TEMP-ønske: ");
		uart_send_string(buffer + 5);
		uart_send_string("\n");
		} else if (strncmp(buffer, "LIGHT:", 6) == 0) {
		uart_send_string("Modtaget LIGHT-ønske: ");
		uart_send_string(buffer + 6);
		uart_send_string("\n");
		} else if (strcmp(buffer, "CO2?") == 0) {
		uart_send_string("CO2:");
		uart_send_string("555\n");
		} else if (strcmp(buffer, "HUMIDITY?") == 0) {
		uart_send_string("HUMIDITY:");
		uart_send_string("45\n");
		} else if (strcmp(buffer, "OPEN") == 0 || strcmp(buffer, "CLOSE") == 0) {
		uart_send_string("Vindue kommando: ");
		uart_send_string(buffer);
		uart_send_string("\n");
	}
}

int main(void) {
	uart_init(103); // 9600 baud @ 16 MHz
	srand(42);
	char buffer[64];

	while (1) {
		if (UCSR1A & (1 << RXC1)) {
			uart_receive_line(buffer, sizeof(buffer));
			process_command(buffer);
		}

		send_fake_sensor_data();
		_delay_ms(2000);
	}
}
