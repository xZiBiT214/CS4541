#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Function Prototypes
void* myalloc(uint32_t size);
void* myrealloc(void* ptr, uint32_t size);
void myfree(void* ptr);
int mysbrk(int size);

// Heap Variables:
extern uint32_t heap[];
extern uint32_t heap_size;
extern uint32_t heap_top;

#endif // HEAP_H
