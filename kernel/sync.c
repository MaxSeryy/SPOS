#include "sync.h"
#include "os.h"

// ====================================
// ======= MUTEX IMPLEMENTATION =======
// ====================================
void os_mutex_init(os_mutex_t *m) {
    m->locked = 0;
}

void os_mutex_lock(os_mutex_t *m) {
    while (1) {
        __asm volatile ("cpsid i"); // crit section
        
        if (m->locked == 0) {
            m->locked = 1;          // get mutex
            __asm volatile ("cpsie i"); // end crit section
            return;
        }
        
        __asm volatile ("cpsie i");
        // If the mutex is locked — yield the processor to another task and wait
        os_yield(); 
    }
}

void os_mutex_unlock(os_mutex_t *m) {
    __asm volatile ("cpsid i");
    m->locked = 0; // release mutex
    __asm volatile ("cpsie i");
}

// ==================================
// ======= COUNTING SEMAPHORE =======
// ==================================

void os_sem_init(os_sem_t *sem, int initial, int max) {
    sem->count = initial;
    sem->max_count = max;
}

void os_sem_take(os_sem_t *sem) {
    while (1) {
        __asm volatile ("cpsid i");
        
        if (sem->count > 0) {
            sem->count--;           // get ticket
            __asm volatile ("cpsie i");
            return;
        }
        
        __asm volatile ("cpsie i");
        os_yield(); // No tickets — put the task to sleep until the next tick
    }
}

void os_sem_give(os_sem_t *sem) {
    __asm volatile ("cpsid i");
    if (sem->count < sem->max_count) {
        sem->count++; // return ticket
    }
    __asm volatile ("cpsie i");
}

// =============================
// ======= MESSAGE QUEUE =======
// =============================

void os_queue_init(os_queue_t *q) {
    q->head = 0;
    q->tail = 0;
    os_mutex_init(&q->lock);
    os_sem_init(&q->items, 0, QUEUE_MAX_ITEMS);
    os_sem_init(&q->spaces, QUEUE_MAX_ITEMS, QUEUE_MAX_ITEMS); 
}

void os_queue_send(os_queue_t *q, void *msg) {
    os_sem_take(&q->spaces); 
    
    os_mutex_lock(&q->lock);
    q->buffer[q->tail] = msg;
    q->tail = (q->tail + 1) % QUEUE_MAX_ITEMS; 
    os_mutex_unlock(&q->lock);
    
    os_sem_give(&q->items); 
}

void* os_queue_receive_try(os_queue_t *q) {
    __asm volatile ("cpsid i"); 
    
    if (q->items.count == 0) {
        __asm volatile ("cpsie i");
        return 0; 
    }
    q->items.count--;
    __asm volatile ("cpsie i");

    os_mutex_lock(&q->lock);
    void *msg = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_MAX_ITEMS;
    os_mutex_unlock(&q->lock);

    os_sem_give(&q->spaces);
    return msg;
}