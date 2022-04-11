#include <stdio.h>

#include "events.h"
#include "general.h"
#include "pwm.h"
#include "bluetooth.h"

volatile _events_str _events;
//bool ChangeChannel = true;
uint8_t canal;                  // Canal del ADC


// Events controller
void eventsController(void) {
	while (true) {
		if (!PENDING_EVENTS) {
			WAITFORINT();
		}

		if (EV_TIMER) {
			EV_TIMER = false;
		}

		/* En caso de tenerse un caracter en el buffer de recepción se da la orden al microcontrolador de procesarlo y se apaga la bandera asociada. */
		if(EV_UART_RX){
			EV_UART_RX = false;
			getData();
		}

	}
}
