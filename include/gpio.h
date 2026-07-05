#ifndef GPIO_H
#define GPIO_H

// ==========================================
// ============ GPIO DRIVER (SIO) ==========
// ==========================================
// simple register-level GPIO control via SIO (Single-cycle IO)
// (moved out of main.c/app layer — this is hardware, not app logic)

void gpio_init(int pin);   // configure pin as SIO output
void gpio_set(int pin);    // drive pin high
void gpio_clear(int pin);  // drive pin low

#endif // GPIO_H