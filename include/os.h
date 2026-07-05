#ifndef OS_H
#define OS_H

#include <stdint.h>

#define STACK_SIZE 256
#define MAX_TASKS 8               // round-robin scheduler supports up to this many tasks
#define STACK_CANARY 0xDEADBEEFu  // sentinel written to the bottom word of each stack

// task control block (TCB) structure
typedef struct {
    uint32_t *sp;          // saved stack pointer (updated on every context switch)
    uint32_t *stack_base;  // lowest address of this task's stack, for overflow checking
} TCB_t;

// declare tcb array and current task index as extern, defined in os.c
extern TCB_t tcb[MAX_TASKS];
extern volatile int current_task;
extern volatile int num_tasks;
extern volatile uint32_t os_tick_count; // incremented once per SysTick (~1ms)

// creates a task in the next free slot. returns its id (0..MAX_TASKS-1),
// or -1 if MAX_TASKS is already reached.
int os_create_task(void (*task_fn)(void), uint32_t *stack);
void os_start(void);

void os_yield(void); // yield to scheduler (trigger PendSV)

// checks every task's stack canary; halts with a message if one was overwritten.
// called automatically from SysTick_Handler, but can be called manually too.
void os_check_stacks(void);

#endif // OS_H