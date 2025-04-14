#ifndef SCD30_H_
#define SCD30_H_
#include <stdint.h>


void scd30_init(void);
void scd30_read(float *co2, float *temp, float *hum);

#endif
