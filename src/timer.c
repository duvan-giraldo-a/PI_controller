#include <stdio.h>
#include "pico/stdlib.h"

#include "timer.h"
#include "events.h"

struct repeating_timer timer;

/* Manejador de interrupción o callback */
bool timer_callback(struct repeating_timer *t) {
    EV_TIMER = true;
    return true;
}

/* Rutina para preparar el timer */
bool timer_init(uint32_t delay_ms) {
    if (!add_repeating_timer_ms(-delay_ms, timer_callback, NULL, &timer))
        return false;
    else
        return true;    
}