#ifndef UART_H
#define UART_H

void init_uart_custom(void);
void uart_putc(char c);
void uart_puts(const char *s);
int uart_getc(void);

void uart_flush(void);
void kprintf(const char *fmt, ...);

#endif // UART_H