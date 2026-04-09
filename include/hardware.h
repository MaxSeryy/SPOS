#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
//pages of offivial RP documentation
#define CLOCKS_BASE       0x40008000    // page 194
#define RESETS_BASE       0x4000c000    // 175
#define IO_BANK0_BASE     0x40014000    // 244
#define SIO_BASE          0xd0000000    // 42
#define UART0_BASE        0x40034000    // 429

#define XOSC_BASE         0x40024000    // external crystal oscillator 219
#define PLL_SYS_BASE      0x40028000    // system PLL 234

#define SYSTICK_CSR       0xE000E010    //https://developer.arm.com/documentation/101407/0543/Debugging/Debug-Windows-and-Dialogs/Core-Peripherals/Armv6-M-cores/Armv6-M--System-Tick-Timer?lang=en
#define SYSTICK_RVR       0xE000E014
#define SYSTICK_CVR       0xE000E018
#define ICSR              0xE000ED04
#define VTOR              0xE000ED08
#define SHPR3             0xE000ED20

#define REG32(addr) (*(volatile unsigned int *)(addr))  // helper for memory-mapped registers

#endif // HARDWARE_H