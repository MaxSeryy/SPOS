#include "tasks.h"
#include "hardware.h"
#include "sync.h"

// TASK 2
void Task_Blinker(void) {

    while (1) {
        // Задача намагається щось надрукувати кожні кілька мільйонів тактів
        // for (volatile int i = 0; i < 5000000; i++);
        // os_queue_send(&log_queue, "[Task 2] I am alive!");
        // os_mutex_lock(&print_mutex); // Чекає своєї черги до UART
        // kprintf("\r\n[Task 2] I am alive!\r\nOS> ");
        // os_mutex_unlock(&print_mutex);
    }
    // REG32(IO_BANK0_BASE + 0x0cc) = 5;    // GPIO 25
    // REG32(SIO_BASE + 0x024) = (1 << 25); // 25th bit - output

    // blinker
    // while (1)
    // {                                        //
    //     REG32(SIO_BASE + 0x01c) = (1 << 25); // XOR
    //     for (volatile int i = 0; i < 500000; i++)
    //     {
    //     }
    // }
}