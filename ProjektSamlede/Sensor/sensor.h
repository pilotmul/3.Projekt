#pragma once
#include <stdint.h>

// Gå til Project → Properties 
// Vælg fanen Toolchain → AVR/GNU Linker → Miscellaneous
// I feltet Other Linker Flags, tilføj:
// -Wl,-u,vfprintf -lprintf_flt -lm
void scd30_init(void);
void scd30_read(float *co2, float *temp, float *hum);


