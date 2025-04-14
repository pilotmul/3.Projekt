#include "sensor.h"
#include "uart.h"
#include <stdio.h>
#include <util/delay.h>
#define F_CPU 16000000UL

int main(void) 
{
	InitUART(9600, 8, 1);
	scd30_init();

	while (1) 
	{
		float co2, temp, hum;
		scd30_read(&co2, &temp, &hum);
		char buf[64];
		sprintf(buf, "CO2: %.1f ppm | Temp: %.1f C | RH: %.1f %%\n\r", co2, temp, hum);
		SendString(buf);
		_delay_ms(20000);
	}
}
