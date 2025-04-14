#include "motor.h"
#include <util/delay.h>



int main(void) {
	servo_init();

	while (1) {
		open();
		_delay_ms(2000); // Vent 2 sekunder

		close();
		_delay_ms(2000);
	}
}