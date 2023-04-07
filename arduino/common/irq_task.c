/* Run a function on timer 5 overflow interrupt.
 * Runs at 16MHz/64/512 ~= 488Hz */

#include <Arduino.h>

static void (*_task)(void);

void irq_task_setup(void (*task)(void)) {
    TIMSK5 |= _BV(TOIE5); // Enable timer 5 overflow interrupt
    _task = task;
}

ISR(TIMER5_OVF_vect) {
    _task();
}
