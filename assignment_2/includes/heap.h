#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Function Prototypes
void* myalloc(uint32_t size);
void* remyalloc(void* ptr);
void myfree(void* ptr, uint32_t size);
int mysbrk(int size);

#endif // HEAP_H
