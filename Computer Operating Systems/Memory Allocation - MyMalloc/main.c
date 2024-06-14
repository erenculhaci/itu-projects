#include "my_malloc.h"

// Global variables
int heap_size; //global heap size
void* heap_start = NULL; //start of heap
BlockHeader* free_list = NULL; //free list (linked list)
BlockHeader* last_allocated_block = NULL; // keeps last allocated block


int InitMyMalloc(int HeapSize) {
    if (heap_start != NULL || HeapSize <= 0) { // Check if memory is already initialized or invalid size
        return -1; // Memory already initialized or invalid size
    }

    // Ensure HeapSize is a multiple of PAGE_SIZE
    HeapSize = (HeapSize + PAGE_SIZE - 1) / PAGE_SIZE * PAGE_SIZE;

    heap_size = HeapSize; // Set heap size
    // Open /dev/zero to obtain anonymous memory mapping
    int fd = open("/dev/zero", O_RDWR);
    if (fd == -1) {
        perror("open");
        return -1; // Failed to open /dev/zero
    }
    // Allocate memory for the heap
    heap_start = mmap(NULL, HeapSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd); // Close /dev/zero
    if (heap_start == MAP_FAILED) { // Check if mmap failed
        return -1; // mmap failed
    }

    // Initialize the free list with a single block spanning the entire heap
    free_list = (BlockHeader*) heap_start; // Set the free list to the start of the heap
    free_list->size = HeapSize - sizeof(BlockHeader); // Set the size of the free list to the entire heap
    free_list->next = NULL; // Set the next pointer to NULL
    printf("\nHeap successfully initialized with size : %d\n", HeapSize);
    return 0; // Memory allocation successful
}

void *MyMalloc(int size, int strategy) {
    if (size <= 0 || heap_start == NULL) { // Check if invalid size or memory not initialized
        return NULL; // Invalid size or memory not initialized
    }

    BlockHeader *current = NULL; // Initialize current block
    BlockHeader *prev_block = NULL; // Initialize previous block

    switch(strategy) { // Select memory allocation strategy
        case 0: // Bestfit Strategy
            {
                BlockHeader *bestfit_block = NULL; // Initialize bestfit block
                BlockHeader *prev_bestfit_block = NULL; // Initialize previous bestfit block
                size_t min_fragment = -1; // Initialize to maximum possible value

                // Search for the best fit block
                current = free_list; // Start from the beginning of the free list
                while (current != NULL) {  // Iterate through the free list
                    if (current->size >= size) {  // Check if the current block is large enough to accommodate the requested size
                        size_t fragment = current->size - size; // Calculate the fragment size (unused space after allocation)
                        if (fragment < min_fragment) { // Check if the current block has a smaller fragment size than the previous best fit block
                            min_fragment = fragment; // Update the minimum fragment
                            bestfit_block = current; // Update the best fit block pointer
                            prev_bestfit_block = prev_block; // Update the previous best fit block pointer
                        }
                    }
                    prev_block = current; // Move to the next block
                    current = current->next; // Move to the next block in the free list
                }

                if (bestfit_block == NULL) {
                    // No suitable block found
                    printf("\nMemory allocation failed. No suitable block found for size: %d\n", size);
                    return NULL;
                }

                // Remove the selected block from the free list
                if (prev_bestfit_block != NULL) { // Check if there is a previous best fit block
                    prev_bestfit_block->next = bestfit_block->next; // Update the next pointer of the previous best fit block
                } else { // If there is no previous best fit block
                    free_list = bestfit_block->next; // Update the free list to point to the next block
                }

                // If there is enough space for another block after allocation
                if (bestfit_block->size - size >= sizeof(BlockHeader)) {
                    // Create a new block with the remaining space
                    BlockHeader *remaining_block = (BlockHeader*)((char*)bestfit_block + size + sizeof(BlockHeader));
                    remaining_block->size = bestfit_block->size - size - sizeof(BlockHeader);
                    remaining_block->next = free_list;
                    free_list = remaining_block;
                }

                // Adjust the size of the allocated block
                bestfit_block->size = size;

                return (void*)(bestfit_block + 1); // Return pointer to the user area
            }
            break;
        case 1: // Worstfit Strategy
            {
                BlockHeader *worstfit_block = NULL; // Pointer to the worst fit block found so far
                BlockHeader *prev_worstfit_block = NULL; // Pointer to the previous block before the worst fit block
                size_t max_fragment = 0; // Maximum fragment size initialized to minimum possible value

                // Search for the worst fit block
                current = free_list;
                while (current != NULL) { // Start from the beginning of the free list
                    if (current->size >= size) { // Iterate through the free list
                        size_t fragment = current->size - size;  // Calculate the fragment size (unused space after allocation)
                        if (fragment > max_fragment) { // Check if the current block has a larger fragment size than the previous worst fit block
                            max_fragment = fragment; // Update the maximum fragment
                            worstfit_block = current; // Update the worst fit block pointer
                            prev_worstfit_block = prev_block; // Update the previous worst fit block pointer
                        }
                    }
                    prev_block = current; // Move to the next block
                    current = current->next; // Move to the next block in the free list
                }

                if (worstfit_block == NULL) {
                    // No suitable block found
                    printf("\nMemory allocation failed. No suitable block found for size: %d\n", size);
                    return NULL;
                }

                // Remove the selected block from the free list
                if (prev_worstfit_block != NULL) {
                    prev_worstfit_block->next = worstfit_block->next;
                } else {
                    free_list = worstfit_block->next;
                }

                // If there is enough space for another block after allocation
                if (worstfit_block->size - size >= sizeof(BlockHeader)) {
                    // Create a new block with the remaining space
                    BlockHeader *remaining_block = (BlockHeader*)((char*)worstfit_block + size + sizeof(BlockHeader));
                    remaining_block->size = worstfit_block->size - size - sizeof(BlockHeader);
                    remaining_block->next = free_list;
                    free_list = remaining_block;
                }

                // Adjust the size of the allocated block
                worstfit_block->size = size;

                return (void*)(worstfit_block + 1); // Return pointer to the user area
            }
            break;
        case 2: // Firstfit Strategy
            {
                // Search for the first fit block
                current = free_list; // Start from the beginning of the free list
                while (current != NULL) { // Iterate through the free list
                    if (current->size >= size) { // Check if the current block has enough space for the requested size
                        // Remove the selected block from the free list
                        if (prev_block != NULL) { // Check if there is a previous block
                            prev_block->next = current->next; // Update the next pointer of the previous block
                        } else { // If there is no previous block
                            free_list = current->next; // Update the free list to point to the next block
                        }

                        // If there is enough space for another block after allocation
                        if (current->size - size >= sizeof(BlockHeader)) {
                            // Create a new block with the remaining space
                            BlockHeader *remaining_block = (BlockHeader*)((char*)current + size + sizeof(BlockHeader));
                            remaining_block->size = current->size - size - sizeof(BlockHeader);
                            remaining_block->next = free_list;
                            free_list = remaining_block;
                        }

                        // Adjust the size of the allocated block
                        current->size = size;

                        return (void*)(current + 1); // Return pointer to the user area (skip the BlockHeader)
                    }
                    prev_block = current; // Move to the next block
                    current = current->next; // Move to the next block in the free list
                }

                // No suitable block found
                printf("\nMemory allocation failed. No suitable block found for size: %d\n", size);
                return NULL; 
            }
            break;
        case 3: // NextFit Strategy
            {
                // Search for the next fit block
                // Start from the block after the last allocated block if available, otherwise start from the beginning of the free list
                current = last_allocated_block != NULL ? last_allocated_block->next : free_list; 

                while (current != NULL) { // Iterate through the free list
                    if (current->size >= size) { // Check if the current block has enough space for the requested size
                        // Remove the selected block from the free list
                        if (prev_block != NULL) {
                            prev_block->next = current->next;
                        } else {
                            free_list = current->next;
                        }

                        // If there is enough space for another block after allocation
                        if (current->size - size >= sizeof(BlockHeader)) {
                            // Create a new block with the remaining space
                            BlockHeader *remaining_block = (BlockHeader*)((char*)current + size + sizeof(BlockHeader));
                            remaining_block->size = current->size - size - sizeof(BlockHeader);
                            remaining_block->next = free_list;
                            free_list = remaining_block;
                        }

                        // Adjust the size of the allocated block
                        current->size = size; 

                        // Update last allocated block pointer
                        last_allocated_block = current;

                        return (void*)(current + 1); // Return pointer to the user area
                    }
                    prev_block = current; // Move to the next block
                    current = current->next; // Move to the next block in the free list
                }

                // Search from the beginning if not found from the last allocated block
                if (last_allocated_block != NULL) { // Check if there was a last allocated block
                    current = free_list; // Start from the beginning of the free list
                    while (current != last_allocated_block->next) { // Iterate until reaching the block after the last allocated block
                        if (current->size >= size) { // Check if the current block is large enough to accommodate the requested size
                            // Remove the selected block from the free list
                            if (prev_block != NULL) {
                                prev_block->next = current->next;
                            } else {
                                free_list = current->next;
                            }

                            // If there is enough space for another block after allocation
                            if (current->size - size >= sizeof(BlockHeader)) {
                                // Create a new block with the remaining space
                                BlockHeader *remaining_block = (BlockHeader*)((char*)current + size + sizeof(BlockHeader));
                                remaining_block->size = current->size - size - sizeof(BlockHeader);
                                remaining_block->next = free_list;
                                free_list = remaining_block;
                            }

                            // Adjust the size of the allocated block
                            current->size = size;

                            // Update last allocated block pointer
                            last_allocated_block = current;

                            return (void*)(current + 1); // Return pointer to the user area
                        }
                        prev_block = current; // Move to the next block
                        current = current->next; // Move to the next block in the free list
                    }
                }
                // No suitable block found
                printf("\nMemory allocation failed. No suitable block found for size: %d\n", size);
                return NULL;
            }
            break;
        default:
            return NULL; // Invalid strategy
    }
    // If no suitable block found for the specified strategy
    return NULL;
}

// Function to free allocated memory
int MyFree(void *ptr) {
    if (ptr == NULL || heap_start == NULL) { // Check if the pointer is NULL or memory is not initialized
        return -1; // Invalid pointer or memory not initialized
    }

    BlockHeader *block = (BlockHeader*)ptr - 1; // Get the block header of the allocated memory
    BlockHeader *current = free_list; // Pointer to the current block in the free list
    BlockHeader *prev = NULL; // Pointer to the previous block in the free list

    // Find the appropriate position to insert the newly freed block into the free list
    while (current != NULL && current < block) { // Iterate until reaching the correct position
        prev = current; // Move to the next block
        current = current->next; // Move to the next block in the free list
    }

    // Insert the newly freed block into the free list
    if (prev == NULL) { // If the newly freed block is the first block in the free list
        block->next = free_list; // Set the next pointer of the newly freed block to the current free list
        free_list = block; // Update the free list pointer to the newly freed block
    } else {
        block->next = prev->next; // Set the next pointer of the newly freed block to the next block in the free list
        prev->next = block;  // Update the next pointer of the previous block to the newly freed block
    }

    // Merge adjacent free blocks
    MergeBlocks();
    return 0; // Free successful
}

// Function to merge adjacent free blocks in the free list
void MergeBlocks() {
    BlockHeader *current = free_list; // Pointer to the current block in the free list
    BlockHeader *prev = NULL; // Pointer to the previous block in the free list
    BlockHeader *temp = NULL; // Temporary pointer for swapping
    int swapped; // Flag to indicate if swapping occurred during sorting

    // Sorting the free list by addresses
    do {
        swapped = 0; // Initialize swapped flag to 0
        current = free_list; // Start from the beginning of the free list
        while (current->next != NULL) { // Iterate until reaching the end of the free list
            // If the current block's address is greater than the next block's address, swap them
            if ((char*)current > (char*)current->next) {
                temp = current->next; // Save the next block in the free list
                current->next = temp->next; // Update the next pointer of the current block
                temp->next = current; // Update the next pointer of the next block
                if (current == free_list) { // If the current block is the first block in the free list
                    free_list = temp; // Update the free list pointer to the next block
                } else {
                    BlockHeader *prev_block = free_list; // Start from the beginning of the free list
                    while (prev_block->next != current) { // Iterate until reaching the previous block
                        prev_block = prev_block->next; // Move to the next block
                    }
                    prev_block->next = temp; // Update the next pointer of the previous block
                }
                swapped = 1; // Set swapped flag to 1 to indicate a swap occurred
            } else {
                current = current->next; // Move to the next block
            }
        }
    } while (swapped); // Continue sorting until no swaps occur

    // Merging adjacent blocks
    current = free_list; // Start from the beginning of the free list
    while (current != NULL && current->next != NULL) { // Iterate until reaching the end of the free list or the last block
        BlockHeader *next_block = current->next; // Get the next block in the free list

        // Calculate the end address of the current block
        char *current_end = (char*)current + sizeof(BlockHeader) + current->size;

        // Calculate the start address of the next block
        char *next_start = (char*)next_block;

        // Calculate the end address of the next block
        char *next_end = (char*)next_block + sizeof(BlockHeader) + next_block->size;

        // Check if the current block can be merged with the next block
        if (current_end == next_start || next_end == (char*)current) {
            // Merge the two blocks
            current->size += sizeof(BlockHeader) + next_block->size;
            current->next = next_block->next;
            // Start again from the beginning of the free list to ensure thorough merging
            current = free_list;
        } else {
            // Move to the next block
            current = current->next;
        }
    }
}

void DumpFreeList() {    
    BlockHeader *current = free_list; // Pointer to the current block in the free list
    BlockHeader *temp = NULL; // Temporary pointer for swapping
    int swapped; // Flag to indicate if swapping occurred during sorting

    // Sorting the free list by addresses before dumping
    do {
        swapped = 0;
        current = free_list;
        while (current->next != NULL) {
            // If the current block's address is greater than the next block's address, swap them
            if ((char*)current > (char*)current->next) {
                temp = current->next;
                current->next = temp->next;
                temp->next = current;
                if (current == free_list) {
                    free_list = temp;
                } else {
                    BlockHeader *prev_block = free_list;
                    while (prev_block->next != current) {
                        prev_block = prev_block->next;
                    }
                    prev_block->next = temp;
                }
                swapped = 1;
            } else {
                current = current->next;
            }
        }
    } while (swapped);


    printf(" \nAddr\tSize\tStatus\n");
    char *prev_end = (char*)heap_start; // Initialize the previous end address

    current = free_list; // Start from the beginning of the free list
    while (current != NULL && current->size != 0) { // Iterate until reaching the end of the free list or a block with size 0 
        char *current_start = (char*)current; // Calculate the start address of the current block
        char *current_end = current_start + sizeof(BlockHeader) + current->size; // Calculate the end address of the current block
        
        // Print "Full" between previous block end and current block start
        if (current_start != prev_end) { // If there is a gap between the previous block and the current block
            printf("%ld\t%ld\tFull\n", prev_end - (char*)heap_start, current_start - prev_end); // Print the size of the gap as a "Full" block
        }
        
        // Print current free block
        printf("%ld\t%ld\tEmpty\n", current_start - (char*)heap_start, current->size + sizeof(BlockHeader)); // Print the address, size, and status of the current block

        // Update prev_end to the end of current block
        prev_end = current_end; // Update the previous end address to the end of the current block
        current = current->next; // Move to the next block in the free list
    }

    // Print "Full" from the end of the last block to the end of the heap
    if (prev_end != (char*)heap_start + heap_size) { // If there is a gap between the end of the last block and the end of the heap
        printf("%ld\t%ld\tFull\n", prev_end - (char*)heap_start, (char*)heap_start + heap_size - prev_end); // Print the size of the gap as a "Full" block
    }
}

int main() {
    char input[100]; // Assuming heap size won't exceed 100 characters
    printf("\nEnter the heap size: ");

    // Read the entire line of input
    fgets(input, sizeof(input), stdin);

    // parse the input as an integer
    if (sscanf(input, "%d", &heap_size) != 1) {
        // If no integer entered, get default page size
        heap_size = PAGE_SIZE;
        printf("\nNo size entered, default heap size selected. \n");
    }
    if (InitMyMalloc(heap_size) == -1) { // Check if memory initialization failed
        printf("Memory initialization failed.\n");
        return -1;
    }

    int strategy; // Variable to store the strategy type
    int input_status; // Variable to store the return value of scanf

    //Strategy type entering
    do {
        printf("\nPlease enter the strategy type (0: Bestfit, 1: Worstfit, 2: Firstfit, 3: NextFit): ");
        input_status = scanf("%d", &strategy);
        
        // If the input is not an integer
        if (input_status != 1) {
            printf("Invalid input! Input must be integer. (0: Bestfit, 1: Worstfit, 2: Firstfit, 3: NextFit)\n");
            // Clear the input buffer
            while (getchar() != '\n');
        }
        // If strategy is not in the range 0-3
        else if (strategy < 0 || strategy > 3) {
            printf("Invalid input! Strategy types are: (0: Bestfit, 1: Worstfit, 2: Firstfit, 3: NextFit) Must be in range 0-3.\n");
        }
    } while (input_status != 1 || strategy < 0 || strategy > 3); // Loop until a valid strategy is entered


    switch(strategy){ // Print the selected strategy
        case 0:
            printf ("\nSelected memory allocation strategy is : Best Fit \n");
            break;
        case 1:
            printf ("\nSelected memory allocation strategy is : Worst Fit \n");
            break;
        case 2:
            printf ("\nSelected memory allocation strategy is : First Fit \n");
            break;
        case 3:
            printf ("\nSelected memory allocation strategy is : Next Fit \n");
            break;
        default:
            printf ("\nInvalid Strategy Selection");
            break;            
    }

    printf("\n Before allocation:");
    DumpFreeList(); 

    // Example processes
    printf("\nProcess Allocation Status:\n");
    void *P1 = MyMalloc(300, strategy);
    if (P1) {
        printf("Process P1: Success, Address (16 byte is used for blockheader): %ld\n", (char*)P1 - (char*)heap_start);
    } else {
        printf("Process P1: Failed\n");
    }
    /////////////////////////////////////////////
    void *P2 = MyMalloc(680, strategy);
    if (P2) {
        printf("Process P2: Success, Address (16 byte is used for blockheader): %ld\n", (char*)P2 - (char*)heap_start);
    } else {
        printf("Process P2: Failed\n");
    }
    ////////////////////////////////////////////
    void *P3 = MyMalloc(1248, strategy);
    if (P3) {
        printf("Process P3: Success, Address (16 byte is used for blockheader): %ld\n", (char*)P3 - (char*)heap_start);
    } else {
        printf("Process P3: Failed\n");
    }
    ///////////////////////////////////////////
    void *P4 = MyMalloc(2560, strategy);
    if (P4) {
        printf("Process P4: Success, Address (16 byte is used for blockheader): %ld\n", (char*)P4 - (char*)heap_start);
    } else {
        printf("Process P4: Failed\n");
    }

    printf("\n After allocation:");
    DumpFreeList();
    
    //Free allocated memory
    MyFree(P1);
    MyFree(P2);
    MyFree(P3);
    MyFree(P4);
    
    printf("\n After deallocation:");
    DumpFreeList();
    return 0;
}
