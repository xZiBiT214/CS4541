#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define INITIAL_SIZE 1000	// Initial Heap Word Size
#define MAX_SIZE 100000		// Max Heap Word Size
#define WORD_SIZE 4		// in bytes
#define ALIGNMENT 8		// double-word alignment

uint32_t heap[INITIAL_SIZE];
uint32_t heap_size = INITIAL_SIZE;
uint32_t heap_top = 0; // Points to the Top of the Heap

// Heap Structure:
typedef struct {
	uint32_t header;
	uint32_t payload[];
	// Footer Placed at the end of the block
} Block;

// Functions to Help:
uint32_t align(uint32_t size) {
	return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

uint32_t block_size(uint32_t size) {
	return sizeof(uint32_t) + align(size) + sizeof(uint32_t);
} 

// Implement Allocation: Update the Heap & Return Pointer to Alloc Block
void* myalloc(uint32_t size) {
	uint32_t total_size = block_size(size); // Get the Size for the Allocation
	// Check to see if the requested allocated space is greater than the total heap size
	if ( (heap_top + total_size / WORD_SIZE) > heap_size ) {
		// Attempt to expand the heap 
		if (mysbrk( (total_size / WORD_SIZE) - (heap_size - heap_top)) == -1) {
			return NULL; // Return NULL because it failed
		}
	} 

	// Otherwise, Update the Heap & Return Pointer to Allocated Block
	uint32_t* header = &heap[heap_top]; // Get the address of the header for the new block
	*header = total_size | 1; // Mark as Allocated
	uint8_t* payload = (uint8_t*)(header + 1); // calculate the address of the payload (just after the header)
	uint32_t* footer = (uint32_t*)((uint8_t*)header + total_size - sizeof(uint32_t)); // Calculate new footer
	*footer = *header; // update the footer value to match the header
	
	heap_top += total_size / WORD_SIZE; // Update the heap_top point to the next free word in the heap
	return (void*)payload; // Return address of the payload
}

// Implement Reallocation:
void* myrealloc(void* ptr, uint32_t size) {
	if ( ptr == NULL ) {
		return myalloc(size); // If pointer is NULL allocate new memory
	}

	if ( size == 0 ) {
		// If the size is 0, free the Memory
		myfree(ptr);
		return NULL;
	}

   	Block* block = (Block*)((uint8_t*)ptr - sizeof(uint32_t)); // Get the block from the pointer
	uint32_t old_size = block->header & ~1; // Get the old size from the block header 
	uint32_t new_size = block_size(size); // Calculate the new size needed
	
	if ( new_size <= old_size ) {
		return ptr; // If the new size is <= old size return the original pointer
	}

	void* new_ptr = myalloc(size); // Allocate new memory of the new size
	if (new_ptr != NULL) {
		memcpy(new_ptr, ptr, old_size - sizeof(uint32_t) * 2); // Copy the old data to the new memory
		myfree(ptr); // Free the old memory
	}

	return new_ptr; // Return the new pointer
}

// Implement Free: Mark Block as Free
void myfree(void* ptr) {
	if (ptr == NULL) return;
	
	Block* block = (Block*)((uint8_t*)ptr - sizeof(uint32_t));
	block->header &= ~1;
	uint32_t* footer = (uint32_t*)((uint8_t*)block + (block->header & ~1) - sizeof(uint32_t));
	*footer = block->header;
	
	return;
}

// Implement Heap Expansion or Shrinking Logic:
int mysbrk(int size) { 
	if (size > 0) {
		// Check to see if the new requested size is bigger than the max size heap
		if ( heap_size + size > MAX_SIZE ) {
			return -1;
		}
	}
	else {	
		// Check to see if the new requested size is smaller than the initial size heap
		if ( heap_size + size < INITIAL_SIZE ) {
			return -1;
		}
		heap_size += size; // Update the size of the heap
	}
 
	return 0;
}
