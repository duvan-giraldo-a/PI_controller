#ifndef _TIMER_H
#define _TIMER_H

#define TIMER_PERIOD_MS      20         // Periodo de 20ms

extern struct repeating_timer timer;

bool timer_init(uint32_t);

#endif