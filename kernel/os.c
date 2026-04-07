#include "os.h"
#include "hardware.h"

TCB_t tcb[2];                   // task control blocks for 2 tasks
volatile int current_task = 0;  // index of current task (0 or 1)

extern unsigned int isr_vector[]; // get from linker script (defined in arch/port.c)

// create task by filling its TCB with initial stack frame
void os_create_task(int id, void (*task_fn)(void), uint32_t *stack) {
    uint32_t *sp = &stack[STACK_SIZE];
    sp -= 16;
    
    sp[15] = 0x01000000;                    // xPSR: set T bit (thumb state)
    sp[14] = (uint32_t)task_fn;             // PC: task entry point
    sp[13] = 0xFFFFFFFD;                    // LR: return to thread mode, use PSP after exception
    
    for(int i = 0; i < 13; i++) sp[i] = 0;  // zero out r0-r12
    
    tcb[id].sp = sp;                        // save initial stack pointer to TCB
}

void os_start() {
    REG32(VTOR) = (unsigned int)isr_vector;
    
    // SET MOST low priority for PendSV and SysTick (for preemptive multitasking)
    REG32(SHPR3) = (0xFF << 16) | (0xFF << 24);

    REG32(SYSTICK_RVR) = 65000 - 1; // ~1ms tick (assuming 65MHz clock)
    REG32(SYSTICK_CVR) = 0;         // clear current value
    REG32(SYSTICK_CSR) = 7;         // enable SysTick, use processor clock, enable interrupt

    __asm volatile (                // enable interrupts and start first task
        "cpsie i \n"
        "svc 0 \n"
    );
}