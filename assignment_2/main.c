#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to Initialize the Heap:
void initializeHeap(){
    Block* block = malloc(sizeof(Block));
    block->free = 1;
    block->size = 3992;
    block->next = 0;
    block->prev = 0;
    head = block;
    block->payloadIndex = 2;

    if (freeList == 'E'){
        ExpList* freeBlock = malloc(sizeof(ExpList));
        freeBlock->block = block;
        freeBlock->next = 0;
        freeBlock->prev = 0;
        expHead = freeBlock;
        heap[freeBlock->block->payloadIndex - 1] = (uintptr_t) freeBlock->block;
        heap[998] = (uintptr_t) freeBlock->block;
    }
}

// Function to Process the Input File:
void process_inpt_file(FILE *file) {
    char str[100];
    while (fgets(str, 100, file)) {
        char *token = strtok(str, ", ");
        char *action = token;
        
        int params[3] = {0}; // To store second, third, and fourth parameters
        int temp = 0;

        while ((token = strtok(NULL, ", ")) != NULL) {
            if (temp < 3) {
                params[temp++] = atoi(token);
            }
        }

        void* pointer;
        switch (*action) {
            case 'a':
                pointer = myalloc(params[0]);
                heapArray[params[1]] = pointer;
                break;
            case 'r':
                pointer = heapArray[params[1]];
                if (params[0] > 0) {
                    pointer = myrealloc(pointer, params[0]);
                    heapArray[params[2]] = pointer;
                } else {
                    myfree(pointer);
                }
                break;
            case 'f':
                pointer = heapArray[params[0]];
                if (pointer) {
                    myfree(pointer);
                }
                break;
        }
    }

    fclose(file);
}

// Function to Get the User Inputs:
void user_inpts(void) {
    printf("Enter free list type (I for Implicit, E for Explicit): ");
    scanf("%c", &freeList);

    printf("Enter Fit type (F for first-fit, B for best-fit): ");
    scanf(" %c", &fit);
}

// Function to Process the Output File:
void process_out_file(void) {
    char* filename = "output.txt";
    char* mode = "w+";
    FILE* out_file = fopen(filename, mode);
    for (int i = 0; i < (heapSize/sizeof(WORD)); i++){
        fprintf(out_file, "%d, 0x%08X\n", i, heap[i]);
    }
    
    fclose(out_file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./exec inputfile\n");
        exit(1);
    }

    FILE *inpt_file = fopen(argv[1], "r");

    user_inpts();
    initializeHeap();
    printf("Processing Input File\n");
    process_inpt_file(inpt_file);
    printf("Writing to Output File\n");
    process_out_file();

    return 0;
}
