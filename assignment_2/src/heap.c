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
	if (size % sizeof(WORD) > 0){
        leftFooter++;
    }
    if (size < sizeof(WORD)) leftFooter++;

    //calculating next header
    unsigned int nextHeader = leftFooter + 1; 
    if (nextHeader % 2 == 0) nextHeader++;
		
    //calculating next footer
    unsigned int nextFooter = block->payloadIndex + block->size / sizeof(WORD); 
    
    //padding
    if (block->size % sizeof(WORD) > 0) nextFooter++;
	
    if (nextFooter > heapSize/sizeof(WORD)) nextFooter = heapSize/sizeof(WORD);

    unsigned int nextSize = (nextFooter - nextHeader) * sizeof(WORD);

    if (nextSize < sizeof(WORD) * 2) return;

    //create a new free block
    Block* nextBlock = malloc(sizeof(Block)); 
    nextBlock->size = nextSize;
    nextBlock->free = 1;
    nextBlock->payloadIndex = nextHeader + 1;
    block->free = 0;
    block->size = size;

    if (block->next != 0) {
        nextBlock->prev = block;
        nextBlock->next = block->next; 
        block->next->prev = nextBlock;
        block->next = nextBlock;
    }
    else {
        nextBlock->prev = block;
        nextBlock->next = 0; 
        block->next = nextBlock;
    }

    //for explicit free list, update pointer of the block
    if (freeList == 'E') freeBlock->block = nextBlock;

    int sz = nextBlock->size/sizeof(WORD);
    if (nextBlock->size % sizeof(WORD) > 0) sz++;

    //update heap
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

//returns a free block based on first fit policy
//for implicit free list
Block* firstFitImp(int size) {
    Block* block;
    block = head;
    if (head == 0) return 0;
    
    while (block != 0) {
        // if block is free and the size fits
        if (block->free && block->size >= size) return block;
        block = block->next;
    }
    
    return block;
}

//returns a free block based on best fit policy
//for implicit free list
Block* bestFitImp(int size) {
    Block* block = head;
    Block* block2 = 0;

    if (head == 0) return 0;
   
    while (block != 0) {
        if (block->free) {
            if (block->size >= size) {
                if (block2 == 0) block2 = block;
                else {
                    if ((block->size-size) < (block2->size-size)) block2 = block;
                }
            }
        }
        block = block->next;
    }
	return block2;
}

//returns a free block based on first fit policy
//for explicit free list
ExpList* firstFitExp(int size) {
    ExpList* freeBlock;
        if (expHead == 0) return 0;
        freeBlock = expHead;
        while (freeBlock != 0){
            if (freeBlock->block->size >= size) return freeBlock;
            freeBlock = freeBlock->next;
        }

    return freeBlock;
}

//returns a free block based on best fit policy
//for explicit free list
ExpList* bestFitExp(int size) {
    ExpList* freeBlock;
    ExpList* freeBlock2 = 0;
    if (expHead == 0) return 0;
    freeBlock = expHead;
    while (freeBlock != 0) {
        if (freeBlock->block->size >= size) {
            if (freeBlock2 == 0) freeBlock2 = freeBlock;
            else if ((freeBlock->block->size-size) < (freeBlock2->block->size-size)) freeBlock2 = freeBlock;
        }
        freeBlock = freeBlock->next;
    }

    return freeBlock2;
}

// returns pointer to payload index
void* myalloc(int size) {
    Block *block = 0;

    if (freeList == 'I') {
        if (fit == 'F') block = firstFitImp(size);
        else if (fit == 'B') block = bestFitImp(size);
    }

    ExpList* freeBlock = 0;
    if (freeList == 'E') {
        if (fit == 'F'){
            freeBlock = firstFitExp(size);
            if (freeBlock != 0) {
                block = freeBlock->block;
	    }
        }
        else if (fit == 'B'){
            freeBlock = bestFitExp(size);
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

    //debugging purposes
    //printf("header: %08x\n", heap[block->payloadIndex - 1]);
    //printf("footer: %08x\n", heap[block->payloadIndex + sz]);
    //printf("payloadIndex: %d\n", block->payloadIndex);

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

//calls coalesce function, which calls merge function
void myfree(void *pointer) {
    Block* block = pointer;
    block->free = 1;

    coalesce(block);

    return;
}

//realloc calls malloc first before freeing old pointer
int *myrealloc(void* pointer, int size) {
    void* ptr = myalloc(size);

    Block* block1 = ptr;
    Block* block2 = pointer;

    //checking for double word alignment
    int sz = block2->size/sizeof(WORD);
    if (block2->size % sizeof(WORD) > 0) sz++;
    while ((sz*sizeof(WORD))%8 > 0) {
        sz++;
    }

    // copy old payload into new block
    for (int i = 0; i < sz; i++) {
        heap[block1->payloadIndex + i] = heap[block2->payloadIndex + i];
    }

    // free old pointer
    myfree(pointer);
    return ptr;
}
