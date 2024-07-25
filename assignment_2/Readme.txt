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

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
------- Inputs ----------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

(Example given in inpt_ex.txt)

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
------- Outputs ---------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
The output will be a comma-seperated value text file called "output.txt"

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
------- References ------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
1. https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/