#include "hardware.h"
#include "clocks.h"

void init_clocks_133mhz(void) {
    // external crystal on 12MHZ
    REG32(XOSC_BASE + 0x0c) = 47;
    REG32(XOSC_BASE + 0x00) = 0xAA0 | 0xFAB000; // XOSC_CTRL: Enable (0xAA0) & range 1-15MHz (0xFAB000)
    
    // wait for XOSC to stabilize (bit 31 - stable)
    while ((REG32(XOSC_BASE + 0x04) & (1 << 31)) == 0) {} 

    // reset PLL_SYS to ensure it's in a known state
    REG32(RESETS_BASE + 0x3000) = (1 << 12);                            // PLL_SYS reset
    while ((REG32(RESETS_BASE + 0x8) & (1 << 12)) != (1 << 12)) {}      // wait

    // ppl 133
    REG32(PLL_SYS_BASE + 0x00) = 1;   // PLL_CS: REFDIV = 1
    REG32(PLL_SYS_BASE + 0x08) = 133; // PLL_FBDIV_INT: VCO = 133
    
    // power up PLL (clear PD bits)
    REG32(PLL_SYS_BASE + 0x04) &= ~((1 << 5) | (1 << 0)); 
    
    // wait for PLL to lock (bit 31 - LOCK)
    while ((REG32(PLL_SYS_BASE + 0x00) & (1 << 31)) == 0) {} 
    
    // set post-dividers to get final 133MHz output: POSTDIV1 = 6, POSTDIV2 = 2 (133MHz = 798MHz / (6*2))
    REG32(PLL_SYS_BASE + 0x0c) = (6 << 16) | (2 << 12); 
    
    // enable PLL output (clear PD bits for post-dividers)
    REG32(PLL_SYS_BASE + 0x04) &= ~(1 << 3); 

    // switch system clock to PLL output
    // AUXSRC = 0 (pll_sys), SRC = 1 (clk_sys_aux)
    REG32(CLOCKS_BASE + 0x3c) = (0 << 5) | 1; 
    
    // wait for clock switch to complete
    while (!(REG32(CLOCKS_BASE + 0x44) & (1 << 1))) {} 

    // switch peripheral clock to system clock (AUXSRC = 0, SRC = 1)
    // This is critical for UART to know it's now running at 133 MHz
    REG32(CLOCKS_BASE + 0x48) = (1 << 11) | (0 << 5); // ENABLE = 1, AUXSRC = 0 (clk_sys)
}