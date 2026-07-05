#include "gpio.h"
#include "hardware.h"

// each GPIO pin has an 8-byte STATUS+CTRL pair in IO_BANK0, starting at +0x004
#define IO_BANK0_GPIO_CTRL(pin) (IO_BANK0_BASE + 0x004 + (pin) * 8)

#define SIO_GPIO_OUT_SET (SIO_BASE + 0x014)
#define SIO_GPIO_OUT_CLR (SIO_BASE + 0x018)
#define SIO_GPIO_OE_SET  (SIO_BASE + 0x024)

#define GPIO_FUNC_SIO 5 // funcsel value to route pin to SIO (plain GPIO)

void gpio_init(int pin) {
    REG32(IO_BANK0_GPIO_CTRL(pin)) = GPIO_FUNC_SIO; // route pin to SIO
    REG32(SIO_GPIO_OE_SET) = (1u << pin);           // enable as output
}

void gpio_set(int pin) {
    REG32(SIO_GPIO_OUT_SET) = (1u << pin);
}

void gpio_clear(int pin) {
    REG32(SIO_GPIO_OUT_CLR) = (1u << pin);
}