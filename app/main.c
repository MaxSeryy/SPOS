#include "hardware.h"
#include "os.h"
#include "uart.h"

uint32_t task1_stack[STACK_SIZE]; // stack for first task (header OS)
uint32_t task2_stack[STACK_SIZE]; // for blinker

// comparing strings (for future)
int my_strcmp(const char *s1, const char *s2) {
    //logic: while chars are equal and not end of string, move forward
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// TASK 1
void Task_Terminal() {
    uart_puts("\r\n=================================\r\n"); // < need write own kptinf
    uart_puts(" Pico OS: Multitasking Enabled! \r\n");
    uart_puts("=================================\r\n");
    uart_puts("OS> ");

    // echo terminal
    while(1) {
        int c = uart_getc();                        // get char from UART
        if (c != -1) {                              // if char received
            uart_putc((char)c);                     // echo back
            if (c == '\r') uart_puts("\nOS> ");     // new line if enter
        }
    }
}

// TASK 2
void Task_Blinker() {
    REG32(IO_BANK0_BASE + 0x0cc) = 5;       // GPIO 25
    REG32(SIO_BASE + 0x024) = (1 << 25);    // 25th bit - output
    
    // blinker
    while(1) { //
        REG32(SIO_BASE + 0x01c) = (1 << 25); // XOR
        for (volatile int i = 0; i < 500000; i++) {}
    }
}

int main() {
    // reseet peripherals (uart troubleshooting)
    REG32(RESETS_BASE + 0x3000) = (1 << 5) | (1 << 8);                                      // UART0 and IO_BANK0
    while((REG32(RESETS_BASE + 0x8) & ((1 << 5) | (1 << 8))) != ((1 << 5) | (1 << 8))) {}   //wait fore reset complete
    
    init_uart_custom();                             //init own uart

    os_create_task(0, Task_Terminal, task1_stack);  // create TASK 1 
    os_create_task(1, Task_Blinker, task2_stack);   // create TASK 2

    os_start();

    while(1) {} //unreal to be here
    return 0;
}