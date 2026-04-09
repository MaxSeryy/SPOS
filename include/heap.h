#ifndef HEAP_H
#define HEAP_H

#include <stddef.h> // for size_t

void os_heap_init(void);
void *os_malloc(size_t size);
void os_free(void *ptr);

// for debugging
void os_heap_stats(void);

#endif // HEAP_H