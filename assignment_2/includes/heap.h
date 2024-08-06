#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Structs
typedef struct block {
    int payloadIndex;
    int size;
    int free;
    struct block *next;
    struct block *prev;
} Block;

typedef struct expList {
    struct block *block;
    struct expList *next;
    struct expList *prev;
} ExpList;

// Global Variables
extern void* heapArray[1000];
extern char fit;
extern char freeList;
extern Block* head;
extern ExpList* expHead;
extern unsigned int heap[100000];
extern int heapSize;

// Macros
#define WORD unsigned int
#define MAX_HEAP 100000

// Function Declarations:
// Main Four Functions
void* myalloc(int size);
void mysbrk(int size);
void myfree(void *pointer);
int *myrealloc(void* pointer, int size);

// Types & Strategies
Block* firstFitImp(int size);
Block* bestFitImp(int size);
ExpList* firstFitExp(int size);
ExpList* bestFitExp(int size);

// Helper Functions
void split(Block* block, size_t size, ExpList* freeBlock);
Block* merge(Block* block, ExpList* freeBlock, char* conjunction);
Block* coalesce(Block* block);

#endif // HEAP_H
