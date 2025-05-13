#define F_CPU 16000000UL
#define BAUD 9600
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// UART opsætning
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

// Hjælpefunktioner
int random_int(int min, int max) {
	return min + rand() % (max - min + 1);
}

// Globale variabler
float set_temp = 22.0;
int set_light = 50;
int window_state = 0; // 0 = AUTO, 1 = OPEN, 2 = CLOSE

// Fake målinger + setværdier
void send_fake_sensor_data() {
	char str[20];

	// Temperatur
	float temp = (float)random_int(180, 250) / 10.0;
	dtostrf(temp, 4, 1, str);
	uart_send_string("TEMP:");
	uart_send_string(str);
	uart_send_string("\n");

	// Lys
	itoa(random_int(0, 100), str, 10);
	uart_send_string("LIGHT:");
	uart_send_string(str);
	uart_send_string("\n");

	// CO2 og fugt
	itoa(random_int(400, 800), str, 10);
	uart_send_string("CO2:");
	uart_send_string(str);
	uart_send_string("\n");

	itoa(random_int(30, 70), str, 10);
	uart_send_string("HUMIDITY:");
	uart_send_string(str);
	uart_send_string("\n");

	if (window_state == 0) {
		uart_send_string("WINDOW:AUTO\n");
	} else if (window_state == 1) {
		uart_send_string("WINDOW:OPEN\n");
	} else if (window_state == 2) {
		uart_send_string("WINDOW:CLOSED\n");
	}
}

// Håndter tekstkommandoer fra Pi
void process_command(const char* buffer) {
    char str[20];

    if (strcmp(buffer, "600") == 0) {
        uart_send_string("MEGA: READING START\n");

        send_fake_sensor_data();  // Fake målinger til test

        uart_send_string("MEGA: READING END\n");
        return;
    }

    if (strcmp(buffer, "PING?") == 0) {
        uart_send_string("PONG\n");
    }
    else if (strcmp(buffer, "TEMP:TEST") == 0) {
        dtostrf(set_temp, 4, 1, str);
        uart_send_string("TEMP:");
        uart_send_string(str);
        uart_send_string("\n");
    }
    else if (strcmp(buffer, "LIGHT:TEST") == 0) {
        itoa(set_light, str, 10);
        uart_send_string("LIGHT:");
        uart_send_string(str);
        uart_send_string("\n");
    }
    else if (strcmp(buffer, "CO2?") == 0) {
        itoa(random_int(400, 800), str, 10);
        uart_send_string("CO2:");
        uart_send_string(str);
        uart_send_string("\n");
    }
    else if (strcmp(buffer, "HUMIDITY?") == 0) {
        itoa(random_int(30, 70), str, 10);
        uart_send_string("HUMIDITY:");
        uart_send_string(str);
        uart_send_string("\n");
    }
    else if (strcmp(buffer, "TEMP:AUTO") == 0) {
        uart_send_string("Temperatur sat til AUTO\n");
    }
    else if (strcmp(buffer, "LIGHT:AUTO") == 0) {
        uart_send_string("Lys sat til AUTO\n");
    }
    else if (strcmp(buffer, "OPEN") == 0) {
        window_state = 1;
        uart_send_string("Vindue kommando: OPEN\n");
    }
    else if (strcmp(buffer, "CLOSE") == 0) {
        window_state = 2;
        uart_send_string("Vindue kommando: CLOSE\n");
    }
    else if (strcmp(buffer, "WINDOW:AUTO") == 0) {
        window_state = 0;
        uart_send_string("Vindue sat til AUTO\n");
    }
    else if (strncmp(buffer, "TEMP:", 5) == 0) {
        set_temp = atof(buffer + 5);
        uart_send_string("Modtaget TEMP-ønske: ");
        uart_send_string(buffer + 5);
        uart_send_string("\n");
    }
    else if (strncmp(buffer, "LIGHT:", 6) == 0) {
        set_light = atoi(buffer + 6);
        uart_send_string("Modtaget LIGHT-ønske: ");
        uart_send_string(buffer + 6);
        uart_send_string("\n");
    }
    else {
        uart_send_string("Ukendt kommando: ");
        uart_send_string(buffer);
        uart_send_string("\n");
    }
}

int connected = 0;

int main(void) {
	uart_init(103);
	srand(42);
	char buffer[64];

	while (1) {
		if (UCSR1A & (1 << RXC1)) {
			uart_receive_line(buffer, sizeof(buffer));
			process_command(buffer);
			if (strcmp(buffer, "PING?") == 0) {
				connected = 1;
			}
		}
		if (connected) {
			send_fake_sensor_data();
			_delay_ms(2000);
		}
	}
}
