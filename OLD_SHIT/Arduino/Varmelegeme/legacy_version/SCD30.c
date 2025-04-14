#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "SCD30.h"
/**** DEPENDENCIES: UART driver ****/

#define SCD30_ADDR 0x61


void TWI_Init(void) 
{
	TWBR = 72; //TWI Bit Rate Register 100kHz
	TWSR = 0; //TWI Status Register, prescale 1
}

void TWI_Start(void) {
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

void TWI_Stop(void) {
	TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);
	while (TWCR & (1<<TWSTO));
}

void TWI_Write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

uint8_t TWI_Read_ACK(void) {
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

uint8_t TWI_Read_NACK(void) {
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

// === CRC (Polynomial 0x31, init 0xFF) ===
uint8_t scd30_crc(uint8_t *data) {
	uint8_t crc = 0xFF;
	for (uint8_t i = 0; i < 2; i++) {
		crc ^= data[i];
		for (uint8_t j = 0; j < 8; j++) {
			if (crc & 0x80)
			crc = (crc << 1) ^ 0x31;
			else
			crc <<= 1;
		}
	}
	return crc;
}

// === Send 16-bit kommando med valgfri argument ===
void scd30_send_cmd(uint16_t cmd, uint16_t arg, uint8_t use_arg) {
	TWI_Start();
	TWI_Write(SCD30_ADDR << 1); // Write
	TWI_Write(cmd >> 8);
	TWI_Write(cmd & 0xFF);

	if (use_arg) {
		uint8_t buf[2] = {arg >> 8, arg & 0xFF};
		TWI_Write(buf[0]);
		TWI_Write(buf[1]);
		TWI_Write(scd30_crc(buf));
	}

	TWI_Stop();
}

// === Tjek om data er klar (0x0202) ===
uint8_t scd30_data_ready(void) {
	uint8_t res[3];

	scd30_send_cmd(0x0202, 0, 0);
	_delay_ms(3); // datasheet anbefaler kort ventetid

	TWI_Start();
	TWI_Write((SCD30_ADDR << 1) | 1); // Read
	res[0] = TWI_Read_ACK(); // MSB
	res[1] = TWI_Read_ACK(); // LSB
	res[2] = TWI_Read_NACK(); // CRC
	TWI_Stop();

	if (scd30_crc(res) != res[2]) return 0;
	return (res[0] << 8 | res[1]) == 1;
}

// === Læs målinger (0x0300) ===
void scd30_read_measurements(float *co2, float *temperature, float *humidity) {
	scd30_send_cmd(0x0300, 0, 0);
	_delay_ms(3);

	TWI_Start();
	TWI_Write((SCD30_ADDR << 1) | 1); // Read

	uint8_t data[18];
	for (uint8_t i = 0; i < 18; i++) {
		if (i == 17)
		data[i] = TWI_Read_NACK();
		else
		data[i] = TWI_Read_ACK();
	}
	TWI_Stop();

	// Parse CO2
	uint32_t co2_raw = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[3] << 8) | data[4];
	*co2 = *(float*)&co2_raw;

	// Temperatur
	uint32_t temp_raw = ((uint32_t)data[6] << 24) | ((uint32_t)data[7] << 16) | ((uint32_t)data[9] << 8) | data[10];
	*temperature = *(float*)&temp_raw;

	// Fugt
	uint32_t hum_raw = ((uint32_t)data[12] << 24) | ((uint32_t)data[13] << 16) | ((uint32_t)data[15] << 8) | data[16];
	*humidity = *(float*)&hum_raw;
}