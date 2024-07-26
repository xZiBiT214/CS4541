#include "heap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
		sscanf(buff, "%c, %d, %d, %d", &cmd, &size, &ref1, &ref2);
		switch (cmd) {
			case 'a':
				myalloc(size);
				break;
			case 'r':
				//myrealloc();
				break;
			case 'f':
				//myfree();
				break;
			default:
				break;
		}
	}

	fclose(file); // Close the Input File
}

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
	
	process_inpt_file(argv[1]);
	write_out_file();

	return 0;
}
