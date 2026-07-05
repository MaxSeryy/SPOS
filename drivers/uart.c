#include "uart.h"
#include "hardware.h"
#include "stdarg.h"

// ==========================================
// ======= UART DRIVER IMPLEMENTATION =======
// ==========================================

void init_uart_custom() {
    REG32(CLOCKS_BASE + 0x48) = (1 << 11) | 0;                      // 48 it's clock for UART0 around 48MHz, 11 - enable
    REG32(RESETS_BASE + 0x3000) = (1 << 22) | (1 << 5) | (1 << 8);  // UART0, IO_BANK0 and SIO reset
    while((REG32(RESETS_BASE + 0x8) & ((1 << 22) | (1 << 5) | (1 << 8))) != ((1 << 22) | (1 << 5) | (1 << 8))) {}

    REG32(IO_BANK0_BASE + 0x004) = 2; // TX
    REG32(IO_BANK0_BASE + 0x00c) = 2; // RX

    REG32(UART0_BASE + 0x24) = 72;                              // baud rate divisor for 115200 baud with 48MHz clock
    REG32(UART0_BASE + 0x28) = 10;                              // no fractional part
    REG32(UART0_BASE + 0x2c) = (3 << 5) | (1 << 4);             // enable FIFOs, clear them
    REG32(UART0_BASE + 0x30) = (1 << 0) | (1 << 8) | (1 << 9);  // UARTCR: UARTEN | TXE | RXE (this is polled RX via uart_getc, no interrupt involved)
}
// send char to UART & timeout
void uart_putc(char c) {
    int timeout = 50000;
    while ((REG32(UART0_BASE + 0x18) & (1 << 5)) && timeout > 0) timeout--;
    if (timeout > 0) REG32(UART0_BASE + 0x00) = c;
}
// send string to UART
void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}
// get char from UART, return -1 if no char received
int uart_getc() {
    if (REG32(UART0_BASE + 0x18) & (1 << 4)) return -1; 
    int data = REG32(UART0_BASE + 0x00) & 0xFF;
    REG32(UART0_BASE + 0x04) = 0xFF; 
    return data;
}
// wait until UART is not busy
void uart_flush() {
    // Чread UARTFR (Flag Register) 0x18
    // bit 3 (TXFF) - Transmit FIFO Full
    while (REG32(UART0_BASE + 0x18) & (1 << 3)) {
        // wait...
    }
}

// number printing helper for kprintf (for future)
static void kprint_num(unsigned int val, int base, int is_signed) {
    char buf[16];       // enough for 32-bit number in binary + null terminator
    int i = 14;         // start filling from the end
    buf[15] = '\0';     // null terminator
    
    if (val == 0) {     // special case for 0
        uart_putc('0');
        return;
    }
    // logic: if signed and negative, print '-' and convert to positive (it's genius) )
    if (is_signed && (int)val < 0 && base == 10) {
        uart_putc('-');
        val = (unsigned int)(-(int)val);
    }
    // logic: get last digit by mod, convert to char, store in buffer, remove last digit by div
    while (val > 0) {
        int digit = val % base;
        buf[i--] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        val /= base;
    }
    uart_puts(&buf[i + 1]);
}

// simplified printf for kernel (for future)
void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++; // skip '%'
            switch (*fmt) {
                case 's': {
                    char *s = va_arg(args, char *);
                    uart_puts(s ? s : "(null)");
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    uart_putc(c);
                    break;
                }
                case 'd': {
                    int val = va_arg(args, int);
                    kprint_num(val, 10, 1);
                    break;
                }
                case 'x': {
                    unsigned int val = va_arg(args, unsigned int);
                    kprint_num(val, 16, 0);
                    break;
                }
                case '%': {
                    uart_putc('%');
                    break;
                }
                default: {
                    uart_putc('%');
                    uart_putc(*fmt);
                    break;
                }
            }
        } else {
            uart_putc(*fmt);
        }
        fmt++;
    }

    va_end(args);
}