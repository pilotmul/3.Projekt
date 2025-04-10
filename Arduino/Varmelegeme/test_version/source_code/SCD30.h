#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

void TWI_Init(void);
void TWI_Start(void);
void TWI_Stop(void);
void TWI_Write(uint8_t data);
uint8_t TWI_Read_ACK(void);
uint8_t TWI_Read_NACK(void);
uint8_t scd30_crc(uint8_t *data);
void scd30_send_cmd(uint16_t cmd, uint16_t arg, uint8_t use_arg);
uint8_t scd30_data_ready(void);
void scd30_read_measurements(float *co2, float *temperature, float *humidity);

