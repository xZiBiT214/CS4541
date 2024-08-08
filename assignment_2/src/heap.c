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
// Function to merge two adjacent free blocks together, called by the coalesce function.
// Merges the current block with the previous block.
Block* merge(Block* currentBlock, ExpList* freeListEntry, char* mergeDirection)
{
    // If the current block is not the last one in the heap
    if (currentBlock->next != 0) {
        // Calculate size in words for the previous block
        unsigned int prevBlockSizeInWords = currentBlock->prev->size / sizeof(WORD);

        // Adjust size if it's not aligned to a WORD boundary
        if (currentBlock->prev->size % sizeof(WORD) > 0) {
            prevBlockSizeInWords++;
        }

        // Ensure size is aligned to 8 bytes (for double word alignment)
        while ((prevBlockSizeInWords * sizeof(WORD)) % 8 > 0) {
            prevBlockSizeInWords++;
        }

        // Include space for header and footer, then add current block's size
        prevBlockSizeInWords += 2;
        prevBlockSizeInWords += currentBlock->size / sizeof(WORD);

        // Adjust size if the current block is not aligned to a WORD boundary
        if (currentBlock->size % sizeof(WORD) > 0) {
            prevBlockSizeInWords++;
        }

        // Ensure size is aligned to 8 bytes
        while ((prevBlockSizeInWords * sizeof(WORD)) % 8 > 0) {
            prevBlockSizeInWords++;
        }

        // Update the previous block's size to the merged size
        currentBlock->prev->size = prevBlockSizeInWords * sizeof(WORD);

    } else { // If the current block is the last one in the heap
        // Calculate size in words for the previous block
        unsigned int prevBlockSizeInWords = currentBlock->prev->size / sizeof(WORD);

        // Adjust size if it's not aligned to a WORD boundary
        if (currentBlock->prev->size % sizeof(WORD) > 0) {
            prevBlockSizeInWords++;
        }

        // Ensure size is aligned to 8 bytes (for double word alignment)
        while ((prevBlockSizeInWords * sizeof(WORD)) % 8 > 0) {
            prevBlockSizeInWords++;
        }

        // Include space for header and footer, then add current block's size
        prevBlockSizeInWords += 2;
        prevBlockSizeInWords += currentBlock->size / sizeof(WORD);

        // Update the previous block's size to the merged size
        currentBlock->prev->size = prevBlockSizeInWords * sizeof(WORD);
    }
    
    // Update links: set the previous block's next pointer to the current block's next
    currentBlock->prev->next = currentBlock->next;
    if (currentBlock->next != 0) {
        currentBlock->next->prev = currentBlock->prev;
    }

    Block* mergedBlock = currentBlock->prev;

    // Handle updates for the explicit free list, if applicable
    if (freeList == 'E') {
        // Search for the block to coalesce in the explicit free list
        ExpList* tempFreeListEntry = expHead;
        while (tempFreeListEntry != 0) {
            if (tempFreeListEntry->block == currentBlock->prev || tempFreeListEntry->block == currentBlock) {
                freeListEntry = tempFreeListEntry;
                break;
            }
            tempFreeListEntry = tempFreeListEntry->next;
        }

        // If the block was found in the free list, update the entry
        if (freeListEntry != 0) {
            freeListEntry->block = currentBlock->prev;

            // Update links in the explicit free list
            if (freeListEntry->prev != 0) {
                if (freeListEntry->next != 0) {
                    freeListEntry->prev->next = freeListEntry->next;
                } else {
                    freeListEntry->prev->next = 0;
                }
            }

            // Handle coalescing direction; if merged with the next block
            if (strcmp(mergeDirection, "right") == 0) {
                if (freeListEntry->next != 0) {
                    if (freeListEntry->next->next != 0) {
                        ExpList* temp = freeListEntry->next;
                        freeListEntry->next = freeListEntry->next->next;
                        freeListEntry->next->next->prev = freeListEntry->prev;
                        free(temp);
                    } else {
                        free(freeListEntry->next);
                        freeListEntry->next = 0;
                    }
                }
            }

            // Update links and head of the explicit free list if necessary
            if (freeListEntry->next != 0) {
                if (freeListEntry->prev != 0) {
                    freeListEntry->next->prev = freeListEntry->prev;
                } else {
                    freeListEntry->next->prev = 0;
                }
            } else if (freeListEntry->next == 0) {
                if (freeListEntry->prev != 0) {
                    freeListEntry->next = freeListEntry->prev;
                    freeListEntry->prev->prev = freeListEntry;
                    freeListEntry->prev = 0;
                }
            }
            if (freeListEntry != expHead) {
                freeListEntry->next = expHead;
                expHead->prev = freeListEntry;
                expHead = freeListEntry;
            }
        
        } else { // If the block was not in the free list, create a new entry and set it as the root
            freeListEntry = malloc(sizeof(ExpList));
            freeListEntry->block = mergedBlock;
            freeListEntry->next = expHead;
            freeListEntry->prev = 0;
            if (expHead != 0) {
                expHead->prev = freeListEntry;
            }
            expHead = freeListEntry;
        }
    }

    // Free the current block as it has been merged
    free(currentBlock);
    return mergedBlock;
}


// Function to coalesce (merge) adjacent free blocks in the heap.
// If the block is the last one in the heap, it coalesces with the remaining free space.
Block* coalesce(Block* currentBlock) 
{
    // If it is the last block in the heap, extend its size to include the remaining free space
    if (currentBlock->next == 0) {
        // Calculate size in words (assuming WORD is the size of one unit in the heap)
        int sizeInWords = currentBlock->size / sizeof(WORD);

        // Adjust size if it's not aligned to a WORD boundary
        if (currentBlock->size % sizeof(WORD) > 0) {
            sizeInWords++;
        }

        // Ensure the size is aligned to 8 bytes (for double word alignment)
        while ((sizeInWords * sizeof(WORD)) % 8 > 0) {
            sizeInWords++;
        }

        // Check if there is remaining free space to extend into
        if (currentBlock->payloadIndex + sizeInWords < (heapSize / sizeof(WORD))) {
            int remainingFreeSpace = heapSize / sizeof(WORD) - currentBlock->payloadIndex;
            currentBlock->size = remainingFreeSpace * sizeof(WORD);
        }
    }

    ExpList* newFreeListEntry = 0;

    // If the block is free, attempt to coalesce with adjacent free blocks
    if (currentBlock->free == 1) {
        int merged = 0;

        // Try to merge with the previous block if it's free
        if (currentBlock->prev != 0) {
            if (currentBlock->prev->free == 1) {
                currentBlock = merge(currentBlock, newFreeListEntry, "left");
                merged = 1;
            }
        }

        // Try to merge with the next block if it's free
        if (currentBlock->next != 0) {
            if (currentBlock->next->free == 1) {
                currentBlock = merge(currentBlock->next, newFreeListEntry, "right");
                merged = 1;
            }
        }

        // If no merging occurred, add the block to the explicit free list
        if (merged == 0) {
            // Only do this if the free list type is 'E' (explicit free list)
            if (freeList == 'E') {
                newFreeListEntry = malloc(sizeof(ExpList));
                newFreeListEntry->block = currentBlock;
                newFreeListEntry->next = expHead;
                newFreeListEntry->prev = 0;
                if (expHead != 0) {
                    expHead->prev = newFreeListEntry;
                }
                expHead = newFreeListEntry;
            }
        }
    }
    
    // Update the heap metadata (headers and footers)
    if (currentBlock->next != 0) {
        int sizeInWords = currentBlock->size / sizeof(WORD);
        if (currentBlock->size % sizeof(WORD) > 0) {
            sizeInWords++;
        }
        while ((sizeInWords * sizeof(WORD)) % 8 > 0) {
            sizeInWords++;
        }
        int headerFooterValue = (sizeInWords + 2) * 4;
        heap[currentBlock->payloadIndex - 1] = headerFooterValue; // header
        heap[currentBlock->payloadIndex + sizeInWords] = headerFooterValue; // footer

    } else if (currentBlock->next == 0) { // Last block in the heap
        int sizeInWords = currentBlock->size / sizeof(WORD);
        int headerFooterValue = (sizeInWords - 1) * 4;
        heap[currentBlock->payloadIndex - 1] = headerFooterValue; // header
        heap[currentBlock->payloadIndex + sizeInWords - 3] = headerFooterValue; // footer
    }

    return currentBlock;
}

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

