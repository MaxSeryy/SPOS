#include "os.h"
#include "hardware.h"

// ==========================================
// ======= MAIN CONTEX SWITCHING CODE =======
// ==========================================
// assembler really hard... page 68

__attribute__((naked)) void PendSV_Handler(void) {
    __asm volatile (
        ".syntax unified \n"        // save cont of curr task
        "mrs r0, psp \n"            // get curr process stack pointer
        "subs r0, #32 \n"           // make space for r4-r11
        "stmia r0!, {r4-r7} \n"     // save r4-r7 (can't use r8-r11 in naked func)
        "mov r4, r8 \n"             // mov to, from
        "mov r5, r9 \n"
        "mov r6, r10 \n"
        "mov r7, r11 \n"
        "stmia r0!, {r4-r7} \n"     // save r8-r11
        "subs r0, #32 \n"           // back to r0
        
        "ldr r1, =current_task \n"  // save curr task index
        "ldr r1, [r1] \n"           // get id of curr task
        "ldr r2, =tcb \n"           // get tcb arr base & save to r2
        "lsls r1, r1, #3 \n"        // shift left r1 * 8 (sizeof TCB_t = 2 pointers now)
        "adds r2, r2, r1 \n"        // get curr tcb
        "str r0, [r2] \n"           // save curr sp to tcb (first field: sp)

        "ldr r1, =current_task \n"
        "ldr r2, [r1] \n"
        "adds r2, #1 \n"
        "ldr r3, =num_tasks \n"     // wrap around the actual number of live tasks,
        "ldr r3, [r3] \n"           // not a hardcoded task count
        "cmp r2, r3 \n"
        "bne save_idx \n"           // if not at the end, keep new index
        "movs r2, #0 \n"            // else, wrap to task 0
    "save_idx: \n"
        "str r2, [r1] \n"

        "ldr r1, =tcb \n"           // get tcb arr base
        "lsls r2, r2, #3 \n"        // shift by sizeof(TCB_t) = 8 bytes
        "adds r1, r1, r2 \n"        // restore cont of new task
        "ldr r0, [r1] \n"           // restore r0 (new sp, first field of TCB_t)

        "adds r0, #16 \n"           // make space for r4-r11...
        "ldmia r0!, {r4-r7} \n"
        "mov r8, r4 \n"
        "mov r9, r5 \n"
        "mov r10, r6 \n"
        "mov r11, r7 \n"
        "subs r0, #32 \n"
        "ldmia r0!, {r4-r7} \n"
        "adds r0, #16 \n"

        "msr psp, r0 \n"            // switch to new task stack
        "ldr r0, =0xFFFFFFFD \n"    // return from exception to thread mode, use psp
        "bx r0 \n"                  // return to new task
        : : : "memory"              // tell compiler we change memory, not to optimize
    );
}

// for TASK 1
__attribute__((naked)) void SVC_Handler(void) {
    __asm volatile (
        ".syntax unified \n"        // similar to PendSV, but don't save curr task (first run)
        "ldr r0, =tcb \n"
        "ldr r0, [r0] \n"           // tcb[0].sp (first field of TCB_t)
        
        "ldmia r0!, {r4-r7} \n"
        "mov r8, r4 \n"
        "mov r9, r5 \n"
        "mov r10, r6 \n"
        "mov r11, r7 \n"
        "ldmia r0!, {r4-r7} \n"
        
        "msr psp, r0 \n"
        
        "movs r0, #2 \n"
        "msr control, r0 \n"
        "isb \n"

        "ldr r0, =0xFFFFFFFD \n"
        "bx r0 \n"
         : : : "memory"
    );
}

// sys timer
void SysTick_Handler(void) {
    os_tick_count++;
    os_check_stacks(); // cheap: one word compare per live task
    REG32(ICSR) = (1 << 28);
}

void os_yield() {
    REG32(ICSR) = (1 << 28); // trigger PendSV by setting its pending bit
    __asm volatile ("isb"); // ensure all memory operations complete before context switch
}