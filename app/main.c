#include "hardware.h"
#include "os.h"
#include "uart.h"

#include "clocks.h"
#include "heap.h"

uint32_t task1_stack[STACK_SIZE]; // stack for first task (header OS)
uint32_t task2_stack[STACK_SIZE]; // for blinker

// comparing strings (for future)
int my_strcmp(const char *s1, const char *s2) {
    //logic: while chars are equal and not end of string, move forward
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
//cut arg
char* split_arg(char* cmd) {
    while (*cmd) {
        if (*cmd == ' '){
            *cmd = '\0';     //cut
            return cmd + 1;
        }
        cmd++;
    }
    return 0;
}

#define SIO_GPIO_OUT_SET (SIO_BASE + 0x014)
#define SIO_GPIO_OUT_CLR (SIO_BASE + 0x018)
#define SIO_GPIO_OE_SET  (SIO_BASE + 0x024)

//led util
void cmd_led(const char *arg) {
    REG32(IO_BANK0_BASE + 0x0cc) = 5;       //sio for gpio
    REG32(SIO_GPIO_OE_SET) = (1 << 25);     //output en

    if(!arg) {
        kprintf("\r\nUsage: led <on|off>");
        return;
    }

    if (my_strcmp(arg, "on") == 0){
        REG32(SIO_GPIO_OUT_SET) = (1 << 25);
        kprintf("\r\nLED is ON");
    }
    else if (my_strcmp(arg, "off") == 0){
        REG32(SIO_GPIO_OUT_CLR) = (1 << 25);
        kprintf("\r\nLED is OFF");
    }
    else {
        kprintf("\r\nUnknown LED state: '%s'", arg);
    }

}

// TASK 1
void Task_Terminal()
{
    uart_puts("\r\n=================================\r\n");
    uart_puts(" Pico OS v0.2 \r\n");
    uart_puts("=================================\r\n");
    uart_puts("OS> ");

    char cmd_buf[32];
    int cmd_idx = 0;

    // echo terminal
    while (1)
    {
        int c = uart_getc(); // get char from UART
        if (c != -1)
        {
            // uart_putc((char)c); // echo back
            
            if (c == '\r')
            {
                cmd_buf[cmd_idx] = '\0'; // close string

                // cut arg from command
                char *arg = split_arg(cmd_buf);

                // command handling
                if (my_strcmp(cmd_buf, "boot") == 0)
                {
                    kprintf("\r\n[DEBUG] Rebooting...\r\n");
                    uart_flush();
                    // system_reboot_to_bootloader();
                }
                else if (my_strcmp(cmd_buf, "led") == 0)
                {
                    cmd_led(arg);
                }
                else if (my_strcmp(cmd_buf, "mem") == 0)
                {
                    char *test_str = (char *)os_malloc(100);
                   if (test_str) {
                        kprintf("\r\n[DEBUG] 100 bytes allocated at 0x%x", (unsigned int)test_str);
                        os_heap_stats();
                        
                        os_free(test_str);
                        kprintf("\r\n[DEBUG] Memory freed.");
                        os_heap_stats();
                    } else {
                        kprintf("\r\n[ERROR] Out of memory!");
                    } 
                }
                else if (cmd_buf[0] != '\0')
                {
                    kprintf("\r\nUnknown command: '%s'", cmd_buf);
                }
                cmd_idx = 0; // reset command buffer index
                kprintf("\r\nOS> ");
            }
            else if (c == '\b' || c == 0x7f)
            {
                if (cmd_idx > 0) {
                    cmd_idx--;
                    uart_puts("\b \b"); // erase char on terminal
                }
            }
            // logic: if char is printable and we have space in buffer, add to command buffer
            else if (cmd_idx < 31 && c >= 32 && c <= 126) 
            {
                uart_putc((char)c); // echo char
                cmd_buf[cmd_idx] = (char)c;
                cmd_idx++;
            }
        }
    }
}

    // TASK 2
    void Task_Blinker()
    {
        // REG32(IO_BANK0_BASE + 0x0cc) = 5;    // GPIO 25
        // REG32(SIO_BASE + 0x024) = (1 << 25); // 25th bit - output

        // blinker
        while (1)
        {                                        //
        //     REG32(SIO_BASE + 0x01c) = (1 << 25); // XOR
        //     for (volatile int i = 0; i < 500000; i++)
        //     {
        //     }
        }
    }

    int main()
    {
        init_clocks_133mhz(); // set 133MHz clock for UART and SysTick
        // reseet peripherals (uart troubleshooting)
        REG32(RESETS_BASE + 0x3000) = (1 << 5) | (1 << 8); // UART0 and IO_BANK0
        while ((REG32(RESETS_BASE + 0x8) & ((1 << 5) | (1 << 8))) != ((1 << 5) | (1 << 8)))
        {
        } // wait fore reset complete

        init_uart_custom(); // init own uart

        os_heap_init(); // init heap before any malloc

        os_create_task(0, Task_Terminal, task1_stack); // create TASK 1
        os_create_task(1, Task_Blinker, task2_stack);  // create TASK 2

        os_start();

        while (1)
        {
        } // unreal to be here
        return 0;
    }