#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE 1024  // Define the heap size

typedef struct Block { //the structure defines the meta-data of the heap element
    size_t size;
    int free;
    struct Block *next;
} Block;

Block *heap = NULL; // Pointer to the start of the heap

// Function to initialize the heap
void init_heap() {
    heap = (Block *)malloc(HEAP_SIZE);
    if (!heap) {
        printf("Heap initialization failed.\n");
        exit(1);
    }
    heap->size = HEAP_SIZE - sizeof(Block);
    heap->free = 1;
    heap->next = NULL;
}

// Function to allocate memory
void *allocate(size_t size) {
    if (size == 0) {
        return NULL;
    }

    //if the request is too huge for allocating 
    if (size > HEAP_SIZE - sizeof(Block)) {
        printf("Error: Requested allocation exceeds available heap size.\n");
        return NULL;
    }

    Block *curr = heap; // to traverse the heap
    while (curr) {
        if (curr->free && curr->size >= size) {
            // Split block if extra space remains
            size_t leftover_space = curr->size - size;
            if (curr->size >= size + sizeof(Block)) {
                Block *NewAvailableBlock = (Block *)((char *)curr + sizeof(Block) + size);
                NewAvailableBlock->size = curr->size - size - sizeof(Block);
                NewAvailableBlock->free = 1;
                NewAvailableBlock->next = curr->next;
                curr->next = NewAvailableBlock;
                curr->size = size;

                leftover_space = sizeof(Block);
            }
            curr->free = 0;

            printf("Memory allocated: %lu bytes\n", size);
            printf("Internal fragmentation: %lu bytes\n", leftover_space);
            printf("\n");

            return (void *)((char *)curr + sizeof(Block));
        }

        curr = curr->next;
    }
    return NULL; // No suitable block found
}

// Function to merge adjacent free blocks
void merge_free_blocks() {
    Block *curr = heap;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += curr->next->size + sizeof(Block);
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

// Function to free memory
void free_block(void *ptr) {
    if (!ptr) return;

    //if the pointer is not in the heap range, it retruns 
    if (!ptr || ptr < (void *)heap || ptr > (void*)heap + (HEAP_SIZE))
    {
        printf("Error: Invalid free request!\n");
        return;
    }

    Block *block = (Block *)((char *)ptr - sizeof(Block));

    if (block->free) {
        printf("Warning: Double free detected!\n");
        return;
    }

    block->free = 1;
    merge_free_blocks();
}

// Function to display the heap status
void display_heap() {
    Block *curr = heap;
    printf("Heap Status:\n");
    while (curr) {
        printf("[Size: %lu, Free: %d] -> ", curr->size, curr->free);
        curr = curr->next;
    }
    printf("NULL\n");
}

// Main function
int main() {
    init_heap();
    
    void *p1 = allocate(200);
    void *p2 = allocate(150);
    void *p3 = allocate(100);
    
    printf("The address at which of the allocated block: %p\n",p1);
    printf("Heap after allocations:\n");

    printf("The address at which of the allocated block: %p\n",p2);
    printf("Heap after allocations:\n");

    printf("The address at which of the allocated block: %p\n",p3);
    printf("Heap after allocations:\n");
    display_heap();
    
    free_block(p2);
    
    printf("Heap after freeing p2:\n");
    display_heap();
    
    void *p4 = allocate(140);
    
    printf("Heap after allocating p4:\n");
    display_heap();

    free_block(p1);
    free_block(p3);
    printf("Heap after freeing up 2 blocks:\n");
    display_heap();
    printf("\n");
    free_block(p4);

    printf("Heap after freeing all blocks:\n");
    display_heap();
    
    return 0;
}