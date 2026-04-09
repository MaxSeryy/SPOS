#include "heap.h"
#include "uart.h" // kprintf
#include <stdint.h>

// 32KB heap for dynamic memory allocation
#define HEAP_SIZE (32 * 1024) 

// IMPORTANT: ARM architecture requires variables in memory to be aligned on 4-byte boundaries.
// This macro rounds the size up to the nearest multiple of 4.
#define ALIGN4(x) (((x) + 3) & ~3)

// Structure of Metadata (Header), which is stored before each block
typedef struct block_meta {
    size_t size;             // Size of the useful block
    struct block_meta *next; // Pointer to the next block in the list
    uint8_t is_free;         // Flag: 1 = free, 0 = occupied
} block_meta_t;

#define META_SIZE sizeof(block_meta_t)

// Compiler will place this array in .bss (RAM)
static uint8_t heap_memory[HEAP_SIZE]; 

// Head of the linked list of blocks
static block_meta_t *free_list = NULL;

// Initialization (convert the entire array into one large free block)
void os_heap_init(void) {
    free_list = (block_meta_t *)heap_memory;
    free_list->size = HEAP_SIZE - META_SIZE;
    free_list->next = NULL;
    free_list->is_free = 1;
}

void *os_malloc(size_t size) {
    if (size == 0) return NULL;

    size = ALIGN4(size); // Align the request (e.g., 5 bytes become 8)

    __asm volatile ("cpsid i"); // CRITICAL SECTION: Disable OS interrupts

    block_meta_t *current = free_list;
    
    // Search for the first-best free block of the appropriate size
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            
            // If the block is significantly larger than needed — split it!
            if (current->size > size + META_SIZE + 4) {
                // Create a new block right after the allocated space
                block_meta_t *new_block = (block_meta_t *)((uint8_t *)current + META_SIZE + size);
                new_block->size = current->size - size - META_SIZE;
                new_block->is_free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }
            
            current->is_free = 0; // Mark the block as occupied
            
            __asm volatile ("cpsie i"); // Enable interrupts
            
            // Return the address RIGHT AFTER the metadata (this is the user's memory)
            return (void *)((uint8_t *)current + META_SIZE); 
        }
        current = current->next;
    }

    __asm volatile ("cpsie i"); // Enable interrupts
    return NULL; // Memory exhausted or highly fragmented
}

void os_free(void *ptr) {
    if (!ptr) return;

    __asm volatile ("cpsid i"); // CRITICAL SECTION

    // We make a step back in memory to find the header of this block
    block_meta_t *block = (block_meta_t *)((uint8_t *)ptr - META_SIZE);
    block->is_free = 1; // Mark as free

    // COALESCING: We traverse the list and merge adjacent free blocks
    block_meta_t *current = free_list;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            current->size += META_SIZE + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }

    __asm volatile ("cpsie i");
}

// Heap statistics (very useful for debugging)
void os_heap_stats(void) {
    __asm volatile ("cpsid i");
    block_meta_t *current = free_list;
    size_t total_free = 0, total_used = 0;
    int blocks = 0;

    while (current != NULL) {
        blocks++;
        if (current->is_free) total_free += current->size;
        else total_used += current->size;
        current = current->next;
    }
    __asm volatile ("cpsie i");

    kprintf("\r\n=== Heap Stats ===");
    kprintf("\r\nTotal Blocks: %d", blocks);
    kprintf("\r\nUsed Memory:  %d bytes", total_used);
    kprintf("\r\nFree Memory:  %d bytes", total_free);
    kprintf("\r\nMeta Overhead:%d bytes\r\n", blocks * META_SIZE);
}