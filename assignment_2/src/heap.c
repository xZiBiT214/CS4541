#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Global Variables
void* heapArray[1000];
char fit;
char freeList;
Block* head;
ExpList* expHead;
unsigned int heap[MAX_HEAP];
int heapSize = 1000 * sizeof(WORD);

// Function Definitions
void split(Block* block, size_t size, ExpList* freeBlock) {
    unsigned int leftFooter = block->payloadIndex + size / sizeof(WORD); 
    if (size % sizeof(WORD) > 0) leftFooter++;
    if (size < sizeof(WORD)) leftFooter++;

    // Calculating the next header
    unsigned int nextHeader = leftFooter + 1; 
    if (nextHeader % 2 == 0) nextHeader++;
		
    // Calculate the next footer
    unsigned int nextFooter = block->payloadIndex + block->size / sizeof(WORD); 
    if (block->size % sizeof(WORD) > 0) nextFooter++;
	
    if (nextFooter > heapSize/sizeof(WORD)) nextFooter = heapSize/sizeof(WORD);

    unsigned int nextSize = (nextFooter - nextHeader) * sizeof(WORD);
    if (nextSize < sizeof(WORD) * 2) return; // Ensure there's enough space for a new block

    // Create a new free block
    Block* nextBlock = malloc(sizeof(Block)); 
    nextBlock->size = nextSize;
    nextBlock->free = 1;
    nextBlock->payloadIndex = nextHeader + 1;
    block->free = 0;
    block->size = size;

    // Update the linked list pointers
    nextBlock->prev = block;
    nextBlock->next = block->next;
    if (block->next != NULL) {
        block->next->prev = nextBlock;
    }
    block->next = nextBlock;

    // For Explicit Free List, update pointer of the block
    if (freeList == 'E') freeBlock->block = nextBlock;

    int sz = nextBlock->size/sizeof(WORD);
    if (nextBlock->size % sizeof(WORD) > 0) sz++;

    // Finally update the heap
    int update = 0;
    update = (sz-1)*4;
    heap[nextBlock->payloadIndex - 1] = update; // header
    heap[nextBlock->payloadIndex + sz - 3] = update; // footer
}

void mysbrk(int size) {
    if (heapSize + size + 8 > MAX_HEAP) {
        char* filename = "output.txt";
        char* mode = "w+";
        FILE* file = fopen(filename, mode);
	fprintf(file, "Error. Heap can't grow past 100000 words.\n");
       	fclose(file);
        exit(1);
    }

    //+8 for header and footer
    heapSize += size + 8;
}

Block* implicit_first_fit(int size) {
    // Traverse the implicit free list starting from the head
    Block* block = head;

    // If the list is empty, return NULL
    if (block == NULL) return NULL;

    // Loop through the blocks in the list
    while (block != NULL) {
        // If the block is free and its size is sufficient, return the block
        if (block->free && block->size >= size) return block;
        block = block->next; // Move to the next block
    }

    // If no suitable block is found, return NULL
    return NULL;
}

// Returns a free block based on Best-Fit Policy for the Implicit Free List
Block* implicit_best_fit(int size) {
    // Pointer to traverse the implicit free list
    Block* currentBlock = head;

    // Pointer to keep track of the best fit block
    Block* bestFitBlock = 0;

    // Traverse the implicit free list to find the best fit block
    while (currentBlock != 0) {
        // Check if the current block is free and of sufficient size
        if (currentBlock->free && currentBlock->size >= size) {
            // If no best fit block has been found yet or the current block is a better fit, update bestFitBlock
            if (bestFitBlock == 0 || (currentBlock->size - size) < (bestFitBlock->size - size)) bestFitBlock = currentBlock;
        }

        currentBlock = currentBlock->next; // Move to the next block in the list
    }

    // Return the best fit block found, or 0 if none was found
    return bestFitBlock;
}

// Returns a free block based on the First-Fit Policy for the Explicit Free List
ExpList* explicit_first_fit(int size) {
    // Start with the head of the explicit free list
    ExpList* freeBlock = expHead;

    // If the explicit free list is empty, return NULL
    if (freeBlock == NULL) return NULL;

    // Traverse the explicit free list to find a suitable block
    while (freeBlock != NULL) {
        // Check if the current block's size is sufficient for the requested size
        if (freeBlock->block->size >= size) return freeBlock;
        freeBlock = freeBlock->next; // Move to the next block in the list
    }

    // If no suitable block is found, return NULL
    return NULL;
}

// Returns a free block based on the Best-Fit Policy for the Explicit Free List
ExpList* explicit_best_fit(int size) {
    ExpList* freeBlock = expHead; // Pointer to traverse the explicit free list
    ExpList* bestFitBlock = NULL; // Pointer to track the best fitting block found so far

    // If the explicit free list is empty, return NULL
    if (freeBlock == NULL) return NULL;

    // Traverse the explicit free list to find the best fitting block
    while (freeBlock != NULL) {
        if (freeBlock->block->size >= size) {
            // If this is the first suitable block or a better fit, update bestFitBlock
            if (bestFitBlock == NULL || (freeBlock->block->size - size) < (bestFitBlock->block->size - size)) {
                bestFitBlock = freeBlock;
            }
        }
        freeBlock = freeBlock->next; // Move to the next block in the list
    }

    // Return the best fitting block found, or NULL if none found
    return bestFitBlock;
}

// returns pointer to payload index
void* myalloc(int size) {
    Block *block = 0;

    if (freeList == 'I') {
        if (fit == 'F') block = implicit_first_fit(size);
        else if (fit == 'B') block = implicit_best_fit(size);
    }

    ExpList* freeBlock = 0;
    if (freeList == 'E') {
        if (fit == 'F'){
            freeBlock = explicit_first_fit(size);
            if (freeBlock != 0) {
                block = freeBlock->block;
	    }
        }
        else if (fit == 'B'){
            freeBlock = explicit_best_fit(size);
            if (freeBlock != 0){
                block = freeBlock->block;
            }
        }
    }

    //if there is not enough space
    if (block == 0 && head != 0){
        Block* temp = head;
        block = temp;
        while (1){
            temp = temp->next;
            if (temp == 0){
                break;
            } 
            block = temp;
        }      
        mysbrk(size);
        Block* newFreeBlock = malloc(sizeof(Block));
        newFreeBlock->size = size;
        newFreeBlock->free = 1;
        newFreeBlock->payloadIndex = heapSize/sizeof(WORD) - size/sizeof(WORD) - 2;
        newFreeBlock->prev = block;
        block->next = newFreeBlock;
        newFreeBlock->next = 0;
        //use the newly allocated block
        block = newFreeBlock;

        //for explicit free list, add the newly allocated block to the list, and use it
        if (freeList == 'E'){
            freeBlock = malloc(sizeof(ExpList));
            freeBlock->next = expHead;
            freeBlock->prev = 0;
            expHead = freeBlock;
        }
    }

    if (block == 0){
        block = malloc(sizeof(Block));
        block->size = size;
        block->free = 0;
        block->next = 0;
        if (head == 0){
            // empty heap, fresh allocation
            block->payloadIndex = 2;
            block->prev = 0;
            head = block;
        }
    }else{
        //check if we can split the block
        if (block->size - size > 2){
            split(block, size, freeBlock);
        }
        else{
            //otherwise, use the block
            block->size = size;
            block->free = 1;

            // for explicit list, update the links
            if (freeList == 'E'){
                if (freeBlock->prev != 0){
                    if (freeBlock->next != 0){
                        freeBlock->prev->next = freeBlock->next;
                    }else{
                        freeBlock->prev->next = 0;
                    }
                }
                if (freeBlock->next != 0){
                    if (freeBlock->prev != 0){
                        freeBlock->next->prev = freeBlock->prev;
                    }else{
                        freeBlock->next->prev = 0;
                    }                    
                }

                //update expHead
                if (freeBlock == expHead){
                    if (freeBlock->next != 0){
                        expHead = freeBlock->next;
                    }else{
                        expHead = 0;
                    }
                }
                free(freeBlock);
            }
        }
    }

    //update heap
    //check for double word alignment
    int sz = block->size/sizeof(WORD);
    if (block->size % sizeof(WORD) > 0){
        sz++;
    }
    while ((sz*sizeof(WORD))%8 > 0){
        sz++;
    }

    int update = 0;
    update = (sz+2)*4 + 1;
    heap[block->payloadIndex - 1] = update; // header
    heap[block->payloadIndex + sz] = update; // footer

    return &block->payloadIndex;
}

//merge two free blocks together (block and block->prev), called by coalesce function
Block* merge(Block* block, ExpList* freeBlock, char* conjunction){

    if (block->next != 0){
        //check for double word alignment
        unsigned int sz = block->prev->size / sizeof(WORD);
        if (block->prev->size % sizeof(WORD) > 0){
            sz++;
        }
        while ((sz*sizeof(WORD))%8 > 0){
            sz++;
        }

        // header and footer size
        sz += 2; 
        sz += block->size / sizeof(WORD);
        if (block->size % sizeof(WORD) > 0){
            sz++;
        }
        while ((sz*sizeof(WORD))%8 > 0){
            sz++;
        }
        //update size
        block->prev->size = sz * sizeof(WORD); 

    }else{
        //check for double word alignment
        unsigned int sz = block->prev->size / sizeof(WORD);
        if (block->prev->size % sizeof(WORD) > 0){
            sz++;
        }
        while ((sz*sizeof(WORD))%8 > 0){
            sz++;
        }

        // footer and header size
        sz += 2; 
        sz += block->size / sizeof(WORD);
        
        //update size
        block->prev->size = sz * sizeof(WORD); 
    }
    
    //update link
    block->prev->next = block->next;
    if (block->next != 0){
        block->next->prev = block->prev;
    }
    Block* prev = block->prev;

    //update for explicit list
    if (freeList == 'E'){
        //search for block to coalesce
        ExpList* tempBlock = expHead;
        while (tempBlock != 0){
            if (tempBlock->block == block->prev || tempBlock->block == block){
                freeBlock = tempBlock;
                break;
            } 
            tempBlock = tempBlock->next;
        }

        //check if block needs coalescing
        if (freeBlock != 0){
            //update block pointer
            freeBlock->block = block->prev;

            //update links
            if (freeBlock->prev != 0){
                if (freeBlock->next != 0){
                    freeBlock->prev->next = freeBlock->next;
                }else{
                    freeBlock->prev->next = 0;
                }
            }

            //if coalesce was done with block and block->next, and not block->prev and block, update the link of freeBlock->next to freeBlock->next->next,
            //basically updating the links
            if (strcmp(conjunction, "right") == 0){
                if (freeBlock->next != 0){
                    if (freeBlock->next->next != 0){
                        ExpList* temp = freeBlock->next;
                        freeBlock->next = freeBlock->next->next;
                        freeBlock->next->next->prev = freeBlock->prev;
                        free(temp);
                    }else{
                        free(freeBlock->next);
                        freeBlock->next = 0;
                    }
                }
            }

            //update links and expHead if necessary
            if (freeBlock->next != 0){
                if (freeBlock->prev != 0){
                    freeBlock->next->prev = freeBlock->prev;
                }else{
                    freeBlock->next->prev = 0;
                }                    
            }else if (freeBlock->next == 0){
                if (freeBlock->prev != 0){
                    freeBlock->next = freeBlock->prev;
                    freeBlock->prev->prev = freeBlock;
                    freeBlock->prev = 0;
                }
            }
            if (freeBlock != expHead){
                freeBlock->next = expHead;
                expHead->prev = freeBlock;
                expHead = freeBlock;
            }
        
        //if block does not need coalescing, create a new ExpList block holding the free block,
        //set it to the root
        }else{    
            freeBlock = malloc(sizeof(ExpList));
            freeBlock->block = prev;
            freeBlock->next = expHead;
            freeBlock->prev = 0;
            expHead->prev = freeBlock;
            expHead = freeBlock;
        }
    }

    free(block);
    return prev;
}

//coalescing function, which further calls merge function
Block* coalesce(Block* block)
{
    // if it is the last block, coalesce with the remaining free space
    if (block->next == 0){
        //check for double word alignment
        int sz = block->size/sizeof(WORD);
        if (block->size % sizeof(WORD) > 0){
            sz++;
        }
        while ((sz*sizeof(WORD))%8 > 0){
            sz++;
        }

        if (block->payloadIndex + sz < (heapSize/sizeof(WORD))){
            int remainingFreeSpace = heapSize/sizeof(WORD) - block->payloadIndex;
            block->size = remainingFreeSpace*sizeof(WORD);
        }
    }

    ExpList* freeBlock = 0;

    if (block->free == 1){
        int merged = 0;
        if (block->prev != 0){
            if (block->prev->free == 1){
                block = merge(block, freeBlock, "left");
                merged = 1;
            }
        }
        if (block->next != 0){
            if (block->next->free == 1){
                block = merge(block->next, freeBlock, "right");
                merged = 1;
            }
        }   

        if (merged == 0){
            //if coalescing did not happen, create a new ExpList block, pointing to the free block
            //set it to the root
            if (freeList == 'E'){
                freeBlock = malloc(sizeof(ExpList));
                freeBlock->block = block;
                freeBlock->next = expHead;
                freeBlock->prev = 0;
                expHead->prev = freeBlock;
                expHead = freeBlock;
            }
        }
    }
    
    
    // update heap
    if (block->next != 0){
        int sz = block->size/sizeof(WORD);
        if (block->size % sizeof(WORD) > 0){
            sz++;
        }
        while ((sz*sizeof(WORD))%8 > 0){
            sz++;
        }
        int update = 0;
        update = (sz+2)*4;
        heap[block->payloadIndex - 1] = update; // header
        heap[block->payloadIndex + sz] = update; // footer

    }else if (block->next == 0){
        int sz = block->size/sizeof(WORD);

        int update = 0;
        update = (sz-1)*4;
        heap[block->payloadIndex - 1] = update; // header
        heap[block->payloadIndex + sz - 3] = update; // footer
    }

	return block;
}

// LAST TWO DONE
// Free Function:
void myfree(void *pointer) {
    Block* block = pointer;
    block->free = 1;

    // Call the coalesce function, which will then call the merge function
    coalesce(block); 
}

// Realloc Function:
int *myrealloc(void* pointer, int size) {
    // Allocate a new block with the requested size
    void* ptr = myalloc(size);

    // Cast the new and old pointers to Block structures
    Block* newBlock = ptr;
    Block* oldBlock = pointer;

    // Checking for double word alignment
    int oldBlockSizeInWords = oldBlock->size / sizeof(WORD);
    if (oldBlock->size % sizeof(WORD) > 0) oldBlockSizeInWords++;
    while ((oldBlockSizeInWords * sizeof(WORD)) % 8 > 0) oldBlockSizeInWords++;

    // Copy old payload into the new block
    for (int i = 0; i < oldBlockSizeInWords; i++) heap[newBlock->payloadIndex + i] = heap[oldBlock->payloadIndex + i];

    // Free the old pointer & Return new pointer
    myfree(pointer);
    return ptr;
}

