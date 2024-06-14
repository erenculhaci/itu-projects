#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/wait.h>

#define PAGE_SIZE getpagesize() // defined PAGE_SIZE as page size of the system

// Structure for the header of each memory block
typedef struct BlockHeader {
    size_t size; //keeps the block size
    struct BlockHeader* next; // next pointer
} BlockHeader;

// Global variables
extern int heap_size; //global heap size
extern void* heap_start; //start of heap
extern BlockHeader* free_list; //free list (linked list)
extern BlockHeader* last_allocated_block; // keeps last allocated block

// Function declarations
int InitMyMalloc(int HeapSize); // Initialize the custom memory allocator
void *MyMalloc(int size, int strategy); // Allocate memory using custom memory allocation
int MyFree(void *ptr); // Free memory using custom memory allocation
void DumpFreeList(); // Print the contents of the free list
void MergeBlocks(); // Merge adjacent free blocks

#endif /* MY_MALLOC_H */