#include "os.h"
#include "hardware.h"
#include "uart.h" // kprintf, only used for the stack-overflow panic message

TCB_t tcb[MAX_TASKS];              // task control blocks
volatile int current_task = 0;     // index of currently running task
volatile int num_tasks = 0;        // how many tasks have been created so far
volatile uint32_t os_tick_count = 0; // incremented once per SysTick interrupt

extern unsigned int isr_vector[]; // get from linker script (defined in arch/port.c)

// create task in the next free slot; fills its initial stack frame so that
// the first context switch into it looks like returning from an exception.
int os_create_task(void (*task_fn)(void), uint32_t *stack) {
    if (num_tasks >= MAX_TASKS) return -1; // no free slots left

    int id = num_tasks++;

    stack[0] = STACK_CANARY; // sentinel at the very bottom of the stack

    uint32_t *sp = &stack[STACK_SIZE];
    sp -= 16;

    sp[15] = 0x01000000;                    // xPSR: set T bit (thumb state)
    sp[14] = (uint32_t)task_fn;             // PC: task entry point
    sp[13] = 0xFFFFFFFD;                    // LR: return to thread mode, use PSP after exception

    for (int i = 0; i < 13; i++) sp[i] = 0; // zero out r0-r12

    tcb[id].sp = sp;
    tcb[id].stack_base = stack;
    return id;
}

// checked once per tick from SysTick_Handler. Cheap: just one word compare
// per task. If a task's stack has grown past its bottom, the canary word
// gets overwritten, and we catch it here instead of silently corrupting
// whatever memory happens to sit below the stack.
void os_check_stacks(void) {
    for (int i = 0; i < num_tasks; i++) {
        if (tcb[i].stack_base[0] != STACK_CANARY) {
            kprintf("\r\n[PANIC] Stack overflow detected in task %d!\r\n", i);
            __asm volatile ("cpsid i"); // stop everything, this is not recoverable
            while (1) { __asm volatile ("wfi"); }
        }
    }
}

void os_start() {
    REG32(VTOR) = (unsigned int)isr_vector;

    // SET MOST low priority for PendSV and SysTick (for preemptive multitasking)
    REG32(SHPR3) = (0xFF << 16) | (0xFF << 24);

    REG32(SYSTICK_RVR) = 133000 - 1; // 1ms tick at 133MHz (133,000,000 / 1000)
    REG32(SYSTICK_CVR) = 0;         // clear current value
    REG32(SYSTICK_CSR) = 7;         // enable SysTick, use processor clock, enable interrupt

    __asm volatile (                // enable interrupts and start first task
        "cpsie i \n"
        "svc 0 \n"
    );
}