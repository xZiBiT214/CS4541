Date: 7/22/2024
Class: CS-4541 Computer System Fundamentals
Assignment 2: Memory Allocation
Author(s): Camryn Ruiz

This program is a memory allocation simulator using the C language.
The simulator performs four primary functions as followed:
1. myalloc(size)
2. myrealloc(pointer, size)
3. myfree(pointer)
4. mysbrk(size)

--- How to Run ---
Make Targets:
all:
	* compiles & builds the project
clean:
	* cleans the project
run:
	* makes & executes the project (with inpt_ex.txt)

--- Inputs ---
The Program processes a comma-seperated value text file to get a series of allocation, reallocation, and free calls

Calls to myalloc will be indicated in the input file as follows:
* An "a" to indicate that this is an allocation call
* An integer to indicate the "size" parameter
* An integer between 0 and 999 to act as a reference the block allocated by the call
* This value will be used to tie future calls to "myfree" and "myrealloc"
* You may use this value any way you wish. It can be used to name pointers returned by your simulator, as the key to key-value pairs that keep track of your
simulated "pointer"s, etc. It is simply there to ensure that we are calling
"myrealloc" and "myfree" on the correct blocks

Calls to myrealloc will be indicated in the input file as follows:
* An "r" to indicate that this is a reallocation call
* An integer to indicate the "size" parameter
* An integer between 0 and 999 to reference which block created by a previous call to myalloc we are resizing
* An integer between 0 and 999 to reference the new block allocated by the call 

Calls to myfree will be indicated in the input file as follows:
* An "f" to indicate that this is a free call
* An integer between 0 and 999 to specify the allocation call that this call is freeing

Example given in inpt_ex.txt:
a, 5, 0
f, 0
a, 10, 1
r, 20, 1, 2
f, 2

--- Outputs ---
The output will be a comma-seperated value text file called "output.txt"

Example Output:
0, 0x00000001 // placeholder
1, 0x00000F98 // header
2, // payload
3, // payload
4, 0x00000011 // remaining footer of myalloc(5)
5, 0x00000F88 // remaining header of free block after myalloc(5)
6, 0x00000018 // remaining footer of myalloc(10)
7, 0x00000021 // remaining header of myrealloc(20)
......
10, 0x00000011 // copied payload from word 4 of myrealloc(20)
11, 0x00000F88 // copied payload from word 5 of myrealloc(20)
......
14, 0x00000021 // remaining footer of myrealloc(20)
15, 0x00000F60 // remaining header of free block after myrealloc(20)
......
998, 0x00000F98 // footer
999, 0x00000001 // placeholder

--- References ---
1. https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/
