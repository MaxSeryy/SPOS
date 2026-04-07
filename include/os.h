#ifndef OS_H
#define OS_H

#include <stdint.h>

#define STACK_SIZE 256
// task control block (TCB) structure
typedef struct {
    uint32_t *sp;
} TCB_t;

// declare tcb array and current task index as extern, defined in os.c
extern TCB_t tcb[2];
extern volatile int current_task;

void os_create_task(int id, void (*task_fn)(void), uint32_t *stack);
void os_start(void);

#endif // OS_H