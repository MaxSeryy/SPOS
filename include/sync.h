#ifndef SYNC_H
#define SYNC_H

// --- MUTEX ---
typedef struct {
    volatile int locked;
} os_mutex_t;

void os_mutex_init(os_mutex_t *m);
void os_mutex_lock(os_mutex_t *m);
void os_mutex_unlock(os_mutex_t *m);

// --- SEMAPHORE ---
typedef struct {
    volatile int count;
    int max_count;
} os_sem_t;

void os_sem_init(os_sem_t *sem, int initial, int max);
void os_sem_take(os_sem_t *sem);
void os_sem_give(os_sem_t *sem);

#define QUEUE_MAX_ITEMS 10

typedef struct {
    void *buffer[QUEUE_MAX_ITEMS];
    int head;
    int tail;
    os_mutex_t lock;
    os_sem_t items;
    os_sem_t spaces;
} os_queue_t;

void os_queue_init(os_queue_t *q);
void os_queue_send(os_queue_t *q, void *msg);
void* os_queue_receive_try(os_queue_t *q);

#endif // SYNC_H