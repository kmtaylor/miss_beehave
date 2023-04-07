/* Run a function on timer 5 overflow interrupt.
 * Runs at 16MHz/64/512 ~= 488Hz */

#define IRQ_TASK_RATE_HZ    488

extern "C" {
    void irq_task_setup(void (*task)(void));
}
