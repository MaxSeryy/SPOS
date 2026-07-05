#include "hardware.h"
#include "os.h"
#include "uart.h"
#include "clocks.h"
#include "heap.h"
#include "shell.h"
#include "tasks.h"

uint32_t task1_stack[STACK_SIZE]; // stack for first task (terminal)
uint32_t task2_stack[STACK_SIZE]; // stack for second task (blinker)

int main() {
    init_clocks_133mhz(); // set 133MHz clock for UART and SysTick

    // reset peripherals (uart troubleshooting)
    REG32(RESETS_BASE + 0x3000) = (1 << 5) | (1 << 8); // UART0 and IO_BANK0
    while ((REG32(RESETS_BASE + 0x8) & ((1 << 5) | (1 << 8))) != ((1 << 5) | (1 << 8))) {
    } // wait for reset complete

    init_uart_custom(); // init own uart
    shell_init();        // init print_mutex + log_queue
    os_heap_init();       // init heap before any malloc

    os_create_task(Task_Terminal, task1_stack); // create TASK 1
    //os_create_task(Task_Blinker, task2_stack);  // create TASK 2

    os_start();

    while (1) {
    } // unreal to be here
    return 0;
}