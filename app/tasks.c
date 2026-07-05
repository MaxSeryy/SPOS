#include "tasks.h"
#include "os.h"
#include "gpio.h"

#define LED_PIN 25
#define BLINK_INTERVAL_MS 500

// TASK 2 — blinks the onboard LED using os_tick_count for timing.
// Instead of a busy-wait loop, it sleeps with WFI between checks: the CPU
// is genuinely idle (lower power) until the next interrupt (SysTick, at
// worst 1ms later) wakes it up to check the clock again.
void Task_Blinker(void) {
    gpio_init(LED_PIN);
    uint32_t last_toggle = os_tick_count;
    int led_on = 0;

    while (1) {
        if ((os_tick_count - last_toggle) >= BLINK_INTERVAL_MS) {
            led_on = !led_on;
            if (led_on) gpio_set(LED_PIN);
            else gpio_clear(LED_PIN);
            last_toggle = os_tick_count;
        }
        __asm volatile ("wfi"); // sleep until the next interrupt instead of spinning
    }
}

void Shell(void) {
    // TASK 1 is implemented in shell.c
}