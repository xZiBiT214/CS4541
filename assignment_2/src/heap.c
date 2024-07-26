#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define INITIAL_SIZE 1000	// Initial Heap Word Size
#define MAX_SIZE 100000		// Max Heap Word Size
#define WORD_SIZE 4		// in bytes
#define ALIGNMENT 8		// double-word alignment

void* myalloc(uint32_t size) {
	// Implement Allocation: Update Heap & Return Pointer to Aloc Block
}

void* myrealloc(void* ptr, uint32_t size) {
	// Implement reallocation logic
   	// If size is 0, it should free the block (equivalent to myfree)
   	// Otherwise, resize the block and return new pointer
}

void myfree(void* ptr) {
	// Implement freeing logic: Mark block as free 
}

int mysbrk(int size) {
	// Implement heap expansion or shrinking logic
   	// Returns 0 on success, -1 on failure (e.g., exceeding max heap size)
}
