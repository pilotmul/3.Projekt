#include <avr/io.h>
#include <string.h>
#include "sensor.h"

// === Internt ===
#define SCD30_ADDR 0x61

static void TWI_Init(void) {
	TWBR = 72;
	TWSR = 0;
}

static void TWI_Start(void) {
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

static void TWI_Stop(void) {
	TWCR = (1<<TWSTO)|(1<<TWEN)|(1<<TWINT);
	while (TWCR & (1<<TWSTO));
}

static void TWI_Write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

static uint8_t TWI_Read_ACK(void) {
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

static uint8_t TWI_Read_NACK(void) {
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

static uint8_t calc_crc(uint8_t *data) {
	uint8_t crc = 0xFF;
	for (uint8_t i = 0; i < 2; i++) {
		crc ^= data[i];
		for (uint8_t j = 0; j < 8; j++) {
			crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
		}
	}
	return crc;
}

static void send_cmd(uint16_t cmd, uint16_t arg, uint8_t use_arg) {
	TWI_Start();
	TWI_Write(SCD30_ADDR << 1);
	TWI_Write(cmd >> 8);
	TWI_Write(cmd & 0xFF);

	if (use_arg) {
		uint8_t buf[2] = {arg >> 8, arg & 0xFF};
		TWI_Write(buf[0]);
		TWI_Write(buf[1]);
		TWI_Write(calc_crc(buf));
	}
	TWI_Stop();
}

static uint8_t data_ready(void) {
	uint8_t res[3];
	send_cmd(0x0202, 0, 0);  // check if data is ready

	TWI_Start();
	TWI_Write((SCD30_ADDR << 1) | 1);
	res[0] = TWI_Read_ACK();
	res[1] = TWI_Read_ACK();
	res[2] = TWI_Read_NACK();
	TWI_Stop();

	return (calc_crc(res) == res[2]) && ((res[0] << 8 | res[1]) == 1);
}



void scd30_init(void) {
	TWI_Init();
	send_cmd(0x0010, 0, 1); // start continuous measurement
}

void scd30_read(float *co2, float *temp, float *hum) {
	if (!data_ready()) return 0;

	send_cmd(0x0300, 0, 0); // read measurement

	TWI_Start();
	TWI_Write((SCD30_ADDR << 1) | 1);

	uint8_t buf[18];
	for (uint8_t i = 0; i < 18; i++) {
		buf[i] = (i == 17) ? TWI_Read_NACK() : TWI_Read_ACK();
	}
	TWI_Stop();

	uint32_t co2_raw = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[3] << 8) | buf[4];
	uint32_t t_raw   = ((uint32_t)buf[6] << 24) | ((uint32_t)buf[7] << 16) | ((uint32_t)buf[9] << 8) | buf[10];
	uint32_t h_raw   = ((uint32_t)buf[12]<< 24) | ((uint32_t)buf[13]<< 16) | ((uint32_t)buf[15]<< 8) | buf[16];

	*co2 = *(float*)&co2_raw;
	*temp = *(float*)&t_raw;
	*hum = *(float*)&h_raw;

}
