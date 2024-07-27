#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_PTRS 1000  // Max number of pointers we can manage

void* ptrs[MAX_PTRS] = {0};  // Initialize all pointers to NULL

void process_inpt_file(const char* filename) {
    FILE* file = fopen(filename, "r"); // Open the file in Read mode
    if (!file) {
        printf("Error Opening File\n");
        return;
    }

    char buff[256]; // Buffer to Hold the Line of Text Read

    // Processing the Input File Contents:
    while (fgets(buff, sizeof(buff), file)) {
        char cmd; // Character that will Hold the Command (i.e., 'a', 'r', 'f')
        int size, ref1, ref2; // ref1 & ref2 are reference identifiers for myalloc() and myrealloc()
        int num_parsed = sscanf(buff, "%c, %d, %d, %d", &cmd, &size, &ref1, &ref2);
        if (num_parsed < 3) continue; // Ensure the correct number of arguments
	
	// Debugging:
        // printf("Command: %c, Size: %d, Ref1: %d, Ref2: %d\n", cmd, size, ref1, ref2);

        switch (cmd) {
            case 'a':
                if (ref1 >= 0 && ref1 < MAX_PTRS) ptrs[ref1] = myalloc(size);
                break;
            case 'r':
                if (ref1 >= 0 && ref1 < MAX_PTRS && ptrs[ref1] != NULL) ptrs[ref1] = myrealloc(ptrs[ref1], size);
                break;
            case 'f':
                if (ref1 >= 0 && ref1 < MAX_PTRS && ptrs[ref1] != NULL) {
                    myfree(ptrs[ref1]);
                    ptrs[ref1] = NULL;
                }
                break;
            default:
                printf("Unknown command: %c\n", cmd);
                break;
        }
    }

    fclose(file); // Close the Input File
}

/*
// Function to Process the Input File:
void process_inpt_file(const char* filename) {
	FILE* file = fopen(filename, "r"); // Open the file in Read mode
	if (!file) {
		printf("Error Opening File");
		return;
	}

	char buff[256]; // Buffer ot Hold the Line of Text Read
	
	// Processing the Input Files Contents:
	while (fgets(buff, sizeof(buff), file)) {
		char cmd; // Character that will Hold the Command (i.e. 'a', 'r', 'f')
		int size, ref1, ref2; // ref1 & ref2 are reference identifiers for myalloc() and myrealloc()
		int num_parsed = sscanf(buff, "%c, %d, %d, %d", &cmd, &size, &ref1, &ref2);
		if (num_parsed < 3) continue;
		printf("Cmd: %c, Size: %d, Ref1: %d, Ref2: %d\n", cmd, size, ref1, ref2);
		switch (cmd) {
			case 'a':
				myalloc(size);
				break;
			case 'r':
				myrealloc((void*)(uintptr_t)ref1, size);
				break;
			case 'f':
				myfree((void*)(uintptr_t)ref1);
				break;
			default:
				break;
		}
	}

	fclose(file); // Close the Input File
}
*/

// Function to Process the Output File:
void write_out_file(void) {
	FILE* file = fopen("output.txt", "w"); // Opening the Output file
	if (!file) {
		printf("Error with Output File");
		return;
	}

	for (uint32_t i = 0; i < heap_top; i++) {
		fprintf(file, "%u, 0x%08X\n", i, heap[i]);
	}

	fclose(file); // Close the Output File
}

// Main Function:
int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: %s <input_file>\n", argv[0]);
		return 1;
	}	
	
	printf("Processing Input File\n");
	process_inpt_file(argv[1]);
	printf("Finished Processing Input File\n");
	write_out_file();
	printf("Finished Writing Output File\n");

	return 0;
}
