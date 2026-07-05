#include "shell.h"
#include "hardware.h"
#include "os.h"
#include "uart.h"
#include "heap.h"
#include "gpio.h"
#include "string_utils.h"

os_mutex_t print_mutex;
os_queue_t log_queue;

#define LED_PIN 25

void shell_init(void) {
    os_mutex_init(&print_mutex);
    os_queue_init(&log_queue);
}

// led util
static void cmd_led(const char *arg) {
    gpio_init(LED_PIN);

    if (!arg) {
        kprintf("\r\nUsage: led <on|off>");
        return;
    }

    if (os_strcmp(arg, "on") == 0) {
        gpio_set(LED_PIN);
        kprintf("\r\nLED is ON");
    } else if (os_strcmp(arg, "off") == 0) {
        gpio_clear(LED_PIN);
        kprintf("\r\nLED is OFF");
    } else {
        kprintf("\r\nUnknown LED state: '%s'", arg);
    }
}

// TASK 1
void Task_Terminal(void) {
    kprintf("\r\n=================================\r\n");
    kprintf("============ Pico OS ============\r\n");
    kprintf("============== 0.4 ==============\r\n");
    kprintf("=================================\r\n");
    kprintf("OS> ");

    char cmd_buf[32];
    int cmd_idx = 0;
    cmd_buf[0] = '\0'; // initialize command buffer

    // echo terminal
    while (1) {
        char *log_msg = (char *)os_queue_receive_try(&log_queue);
        if (log_msg) {
            kprintf("\r                          \r");
            kprintf("%s\r\n", log_msg);
            cmd_buf[cmd_idx] = '\0';
            kprintf("OS> %s", cmd_buf);
        }
        int c = uart_getc(); // get char from UART
        if (c != -1) {
            // uart_putc((char)c); // echo back

            if (c == '\r') {
                cmd_buf[cmd_idx] = '\0'; // close string

                // cut arg from command
                char *arg = os_split_arg(cmd_buf);

                // command handling
                if (os_strcmp(cmd_buf, "boot") == 0) {
                    kprintf("\r\n[DEBUG] Rebooting...\r\n");
                    uart_flush();
                    // system_reboot_to_bootloader();
                } else if (os_strcmp(cmd_buf, "led") == 0) {
                    os_mutex_lock(&print_mutex);
                    cmd_led(arg);
                    os_mutex_unlock(&print_mutex);

                } else if (os_strcmp(cmd_buf, "help") == 0) {
                    kprintf("\r\nAvailable commands:\r\n");
                    kprintf("  boot - Reboot the system (not implemented)\r\n");
                    kprintf("  led <on|off> - Control the LED\r\n");
                    kprintf("  mem - Test heap allocation\r\n");
                    kprintf("  clear - clear the terminal-_-\r\n");
                } else if (os_strcmp(cmd_buf, "mem") == 0) {
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
                } else if (os_strcmp(cmd_buf, "clear") == 0) {
                    kprintf("\r\n\x1b[2J\x1b[H"); // ansi code
                } else if (cmd_buf[0] != '\0') {
                    kprintf("\r\nUnknown command: '%s'", cmd_buf);
                }
                cmd_idx = 0; // reset command buffer index
                kprintf("\r\nOS> ");
            } else if (c == '\b' || c == 0x7f) {
                if (cmd_idx > 0) {
                    cmd_idx--;
                    uart_puts("\b \b"); // erase char on terminal
                }
            }
            // logic: if char is printable and we have space in buffer, add to command buffer
            else if (cmd_idx < 31 && c >= 32 && c <= 126) {
                uart_putc((char)c); // echo char
                cmd_buf[cmd_idx] = (char)c;
                cmd_idx++;
            }
        }
    }
}